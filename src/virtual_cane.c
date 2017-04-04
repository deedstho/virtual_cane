#include "board.h"
#include <stdio.h>
#include "i2c.h"
#include "haptic.h"
#include "pwm.h"
#include "lidar.h"

#define TICKRATE_HZ1 (50)	/* 10 ticks per second */

/**
 * @brief	Handle interrupt from SysTick timer
 * @return	Nothing
 */
void SysTick_Handler(void)
{
	while(check_status()){}

	uint16_t distance;
	distance = read_data();

//	static uint16_t distance = 0;

	Board_LED_Toggle(0);
//
//	++distance;
//	if (distance > 150) distance = 0;
	distance_to_sound(distance);

	static unsigned int i = 0;
	++i;
	if (i % 100 == 0)
	{
		haptic_notify();
	}

	calibrate_lidar();
}

/**
 * @brief	main routine for systick example
 * @return	Function should not exit.
 */
int main(void)
{
	/* Generic Initialization */
	SystemCoreClockUpdate();
	Board_Init();

	i2c_app_init(I2C0, SPEED_100KHZ);
	haptic_init();

	/* Enable and setup SysTick Timer at a periodic rate */
	SysTick_Config(SystemCoreClock / TICKRATE_HZ1);




	/* LEDs toggle in interrupt handlers */
	while (1) {
		__WFI();
	}

	return 0;
}



