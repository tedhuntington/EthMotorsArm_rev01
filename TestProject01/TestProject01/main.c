#include <atmel_start.h>
#include <hal_gpio.h>
#include <hal_delay.h>

#define LED0 GPIO(GPIO_PORTB, 12)

int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();
	
	gpio_set_pin_level(LED0,true);

	// Set pin direction to output
	gpio_set_pin_direction(LED0, GPIO_DIRECTION_OUT);

	gpio_set_pin_function(LED0, GPIO_PIN_FUNCTION_OFF);

	/* Replace with your application code */
	while (1) {
		delay_ms(500);
		gpio_toggle_pin_level(LED0);		   
	}
}
