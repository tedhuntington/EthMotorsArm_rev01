#include <atmel_start.h>
#include <hal_gpio.h>
#include <hal_delay.h>
#include <driver_examples.h>
#include <peripheral_clk_config.h>
#include <ethernet_phy_main.h>
#include <lwip/netif.h>
#include <lwip/dhcp.h>
#include <lwip/udp.h>
#include <lwip/timers.h>
#include "lwip_demo_config.h"
#include <stdio.h>
#include <string.h>
#include "udpserver.h"
   
//#define LED0 GPIO(GPIO_PORTB, 12)


/* Saved total time in mS since timer was enabled */
volatile static u32_t systick_timems;
volatile static bool  gmac_recv_flag = false;
static bool           link_up   = false;

struct udp_pcb *udpserver_pcb; //udp server

u32_t sys_now(void)
{
	return systick_timems;
}

void SysTick_Handler(void)
{
	systick_timems++;
}

void systick_enable(void)
{
	systick_timems = 0;
	SysTick_Config((CONF_CPU_FREQUENCY) / 1000);
}

void mac_receive_cb(struct mac_async_descriptor *desc)
{
	gmac_recv_flag = true;
	//printf("recvd\n");
}
static void print_ipaddress(void)
{
	static char tmp_buff[16];
	printf("IP_ADDR    : %s\r\n", ipaddr_ntoa_r((const ip_addr_t *)&(LWIP_MACIF_desc.ip_addr), tmp_buff, 16));
	printf("NET_MASK   : %s\r\n", ipaddr_ntoa_r((const ip_addr_t *)&(LWIP_MACIF_desc.netmask), tmp_buff, 16));
	printf("GATEWAY_IP : %s\r\n", ipaddr_ntoa_r((const ip_addr_t *)&(LWIP_MACIF_desc.gw), tmp_buff, 16));
}

static void read_macaddress(u8_t *mac)
{
#if CONF_AT24MAC_ADDRESS != 0
	uint8_t addr = 0x9A;
	i2c_m_sync_enable(&I2C_AT24MAC);
	i2c_m_sync_set_slaveaddr(&I2C_AT24MAC, CONF_AT24MAC_ADDRESS, I2C_M_SEVEN);
	io_write(&(I2C_AT24MAC.io), &addr, 1);
	io_read(&(I2C_AT24MAC.io), mac, 6);

#else
	/* set mac to 0x11 if no EEPROM mounted */
	//memset(mac, 0x11, 6);
	mac[0]=0x74;
	mac[1]=0x27;
	mac[2]=0xea;
	mac[3]=0xda;
	mac[4]=0x89;
	mac[5]=0x85;
#endif
}


void udpserver_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, struct ip_addr *addr, u16_t port) 
{
	int i;

	printf("received at %d, echoing to the same port\n",pcb->local_port);
	//dst_ip = &(pcb->remote_ip); // this is zero always
	if (p != NULL) {
		    	  printf("UDP rcv %d bytes: ", (*p).len);
		//    	  for (i = 0; i < (*p).len; ++i)
		//			printf("%c",((char*)(*p).payload)[i]);
		//    	printf("\n");
		//udp_sendto(pcb, p, IP_ADDR_BROADCAST, 1234); //dest port

//		udp_sendto(pcb, p, &forward_ip, fwd_port); //dest port
		pbuf_free(p);
	}
}


int main(void)
{
	struct io_descriptor *io;
	int count;//,StartDHCP;
	uint8_t OutStr[256];
	int32_t ret;
	u8_t    mac[6];
	u8_t ReadBuffer[256];

	/* Initializes MCU, drivers and middleware - tph - inits phy*/
	atmel_start_init();

	//initialize user gpio pins	
	//gpio_set_pin_level(LED0,true);
	// Set pin direction to output
	//gpio_set_pin_direction(LED0, GPIO_DIRECTION_OUT);
	//gpio_set_pin_function(LED0, GPIO_PIN_FUNCTION_OFF);

	//init usart
	usart_sync_get_io_descriptor(&USART_0, &io);
	usart_sync_enable(&USART_0);
	count=0;
	sprintf((char *)OutStr,"**************************\n");
	io_write(io,OutStr,strlen(OutStr));
	sprintf((char *)OutStr,"\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\n");
	io_write(io,OutStr,strlen(OutStr));
	sprintf((char *)OutStr,"EthMotorsArm_DRV8800_rev01\n");
	io_write(io,OutStr,strlen(OutStr));
	sprintf((char *)OutStr,"\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\n");
	io_write(io,OutStr,strlen(OutStr));
	sprintf((char *)OutStr,"**************************\n");
	io_write(io,OutStr,strlen(OutStr));
	sprintf((char *)OutStr,"\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\n");
	io_write(io,OutStr,strlen(OutStr));

	/* Read MacAddress from EEPROM */  //tph: currently just adding a valid public MAC address
	read_macaddress(mac);

	systick_enable();

	//MACIF_example();
	
	MACIF_PHY_example();  //restarts autonegotiation



	printf("\r\nHello ATMEL World!\r\n");
	mac_async_register_callback(&MACIF, MAC_ASYNC_RECEIVE_CB, (FUNC_PTR)mac_receive_cb);

	eth_ipstack_init();
	do {
		ret = ethernet_phy_get_link_status(&MACIF_PHY_desc, &link_up);
		if (ret == ERR_NONE && link_up) {
			break;
		}
	} while (true);
	printf("Ethernet Connection established\n");
	LWIP_MACIF_init(mac);  //tph: add LWIP callback for recvd input: ethernet_input()
	netif_set_up(&LWIP_MACIF_desc);

	netif_set_default(&LWIP_MACIF_desc);
	mac_async_enable(&MACIF);

	mac_async_enable_irq(&LWIP_MACIF_desc);

	//StartDHCP=1;
	dhcp_start(&LWIP_MACIF_desc); //tph start dhcp

#if 0 
	//udpecho_init(); //START UDP ECHO THREAD - requires netconn 
	//start_udp();
	udpserver_pcb = udp_new();  //create udp server
	//IP4_ADDR(&forward_ip, 192, 168,   2, 254);
//	udp_bind(udpserver_pcb, IP_ADDR_ANY, 53510);   //port 53510 
	udp_bind(udpserver_pcb, &LWIP_MACIF_desc.ip_addr.addr, 53510);   //port 53510 
	udp_recv(udpserver_pcb, udpserver_recv, NULL);  //set udpserver callback function
	
	const int out_buf_size=4;
	const char buf[]="test";

	//send a udp packet
	struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, out_buf_size * sizeof(char), PBUF_REF);
	if (p!=0) {
		p->payload = buf;
		udp_sendto(udpserver_pcb, p, IP_ADDR_BROADCAST, 1234); //dest port
		//udp_sendto(pcb, p, &forward_ip, fwd_port); //dest port
		pbuf_free(p);
	} //if (p!=0)
#endif

	/* Replace with your application code */
	while (true) {

/*
		if (StartDHCP) {
			StartDHCP=0;
			dhcp_start(&LWIP_MACIF_desc); //tph start dhcp
		}
*/

		if (gmac_recv_flag) {
			//printf("gmac_recd");
			sprintf((char *)OutStr,"recvd2\n");
			io_write(io,OutStr,strlen(OutStr));
			
			gmac_recv_flag = false;
			ethernetif_mac_input(&LWIP_MACIF_desc);
		}
		/* LWIP timers - ARP, DHCP, TCP, etc. */
		sys_check_timeouts();

		/* Print IP address info */
		if (link_up && LWIP_MACIF_desc.ip_addr.addr) {
			link_up = false;
			print_ipaddress();
		}

		//netif_poll(&LWIP_MACIF_desc);  //tph need?

		//check interface for DHCP
//		if (LWIP_MACIF_desc.dhcp->state == DHCP_BOUND) {
//			sprintf((char *)OutStr,"DHCP bound\n");
//			io_write(io,OutStr,strlen(OutStr));			
//		}
	//autoip_tmr(); //call every 100ms AUTOIP_TMR_INTERVAL msces,

/*		delay_ms(1000);
		gpio_toggle_pin_level(LED0);
		
		sprintf((char *)OutStr,"\b\b\b%03d",count);
		io_write(io, OutStr, strlen(OutStr));
		count++;
		//USART_0_example();
*/

//	GMAC_Handler();
	mac_async_read(&MACIF, ReadBuffer, 10);

	}  //while(1)
} //main

