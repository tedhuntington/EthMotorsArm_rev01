#include <atmel_start.h>
#include <hal_gpio.h>
#include <hal_delay.h>
#include <driver_examples.h>
#include <stdio.h>
#include <string.h>

#define LED0 GPIO(GPIO_PORTB, 12)

int main(void)
{
	struct io_descriptor *io;
	int count;
	uint8_t OutStr[256];

	/* Initializes MCU, drivers and middleware */
	atmel_start_init();
	
	gpio_set_pin_level(LED0,true);

	// Set pin direction to output
	gpio_set_pin_direction(LED0, GPIO_DIRECTION_OUT);

	gpio_set_pin_function(LED0, GPIO_PIN_FUNCTION_OFF);

	//ETHERNET_MAC_0_example();
	
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


	/* Replace with your application code */
	while (1) {
		delay_ms(1000);
		gpio_toggle_pin_level(LED0);
		
		sprintf((char *)OutStr,"\b\b\b%03d",count);
		io_write(io, OutStr, strlen(OutStr));
		count++;
		//USART_0_example();
	}
}

