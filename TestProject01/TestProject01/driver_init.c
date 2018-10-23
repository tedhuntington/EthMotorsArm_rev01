/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */

#include "driver_init.h"
#include <peripheral_clk_config.h>
#include <utils.h>
#include <hal_init.h>

#include <hpl_rtc_base.h>

struct timer_descriptor TIMER_0;

struct usart_sync_descriptor USART_0;

struct mac_async_descriptor ETHERNET_MAC_0;

/**
 * \brief Timer initialization function
 *
 * Enables Timer peripheral, clocks and initializes Timer driver
 */
static void TIMER_0_init(void)
{
	hri_mclk_set_APBAMASK_RTC_bit(MCLK);
	timer_init(&TIMER_0, RTC, _rtc_get_timer());
}

void USART_0_PORT_init(void)
{

	gpio_set_pin_function(PA00, PINMUX_PA00D_SERCOM1_PAD0);

	gpio_set_pin_function(PA01, PINMUX_PA01D_SERCOM1_PAD1);
}

void USART_0_CLOCK_init(void)
{
	hri_gclk_write_PCHCTRL_reg(GCLK, SERCOM1_GCLK_ID_CORE, CONF_GCLK_SERCOM1_CORE_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
	hri_gclk_write_PCHCTRL_reg(GCLK, SERCOM1_GCLK_ID_SLOW, CONF_GCLK_SERCOM1_SLOW_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));

	hri_mclk_set_APBAMASK_SERCOM1_bit(MCLK);
}

void USART_0_init(void)
{
	USART_0_CLOCK_init();
	usart_sync_init(&USART_0, SERCOM1, (void *)NULL);
	USART_0_PORT_init();
}

void ETHERNET_MAC_0_PORT_init(void)
{

	gpio_set_pin_function(PB14, PINMUX_PB14L_GMAC_GMDC);

	gpio_set_pin_function(PB15, PINMUX_PB15L_GMAC_GMDIO);

	gpio_set_pin_function(PA13, PINMUX_PA13L_GMAC_GRX0);

	gpio_set_pin_function(PA12, PINMUX_PA12L_GMAC_GRX1);
}

void ETHERNET_MAC_0_CLOCK_init(void)
{
	hri_mclk_set_AHBMASK_GMAC_bit(MCLK);
	hri_mclk_set_APBCMASK_GMAC_bit(MCLK);
}

void ETHERNET_MAC_0_init(void)
{
	ETHERNET_MAC_0_CLOCK_init();
	mac_async_init(&ETHERNET_MAC_0, GMAC);
	ETHERNET_MAC_0_PORT_init();
}

void ETHERNET_MAC_0_example(void)
{
	mac_async_enable(&ETHERNET_MAC_0);
	mac_async_write(&ETHERNET_MAC_0, (uint8_t *)"Hello World!", 12);
}

void system_init(void)
{
	init_mcu();

	TIMER_0_init();

	USART_0_init();

	ETHERNET_MAC_0_init();
}
