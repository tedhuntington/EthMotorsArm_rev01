#include <atmel_start.h>
#include <hal_gpio.h>
#include <hal_delay.h>
#include <driver_examples.h>
#include <peripheral_clk_config.h>
#include <lwip/netif.h>
#include <lwip/timers.h>
#include "lwip_demo_config.h"
#include <stdio.h>
#include <string.h>

#define LED0 GPIO(GPIO_PORTB, 12)


/* Saved total time in mS since timer was enabled */
volatile static u32_t systick_timems;
volatile static bool  recv_flag = false;
static bool           link_up   = false;

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
	recv_flag = true;
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
	memset(mac, 0x11, 6);
	#endif
}



int main(void)
{
	struct io_descriptor *io;
	int count;
	uint8_t OutStr[256];
	int32_t ret;
	u8_t    mac[6];

	/* Initializes MCU, drivers and middleware */
	atmel_start_init();
	
	gpio_set_pin_level(LED0,true);

	// Set pin direction to output
	gpio_set_pin_direction(LED0, GPIO_DIRECTION_OUT);

	gpio_set_pin_function(LED0, GPIO_PIN_FUNCTION_OFF);

	//MACIF_example();
	
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



	/* Read MacAddress from EEPROM */
	read_macaddress(mac);

	systick_enable();

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
	LWIP_MACIF_init(mac);
	netif_set_up(&LWIP_MACIF_desc);

	netif_set_default(&LWIP_MACIF_desc);
	mac_async_enable(&MACIF);



	/* Replace with your application code */
	while (1) {

		if (recv_flag) {
			recv_flag = false;
			ethernetif_mac_input(&LWIP_MACIF_desc);
		}
		/* LWIP timers - ARP, DHCP, TCP, etc. */
		sys_check_timeouts();

		/* Print IP address info */
		if (link_up && LWIP_MACIF_desc.ip_addr.addr) {
			link_up = false;
			print_ipaddress();
		}

/*		delay_ms(1000);
		gpio_toggle_pin_level(LED0);
		
		sprintf((char *)OutStr,"\b\b\b%03d",count);
		io_write(io, OutStr, strlen(OutStr));
		count++;
		//USART_0_example();
*/

	}  //while(1)
} //main

