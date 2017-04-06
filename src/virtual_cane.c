#include "board.h"
#include <stdio.h>
#include "i2c.h"
#include "haptic.h"
#include "pwm.h"
#include "lidar.h"
//#include "serial.h"

#define TICKRATE_HZ1 (50)	/* 10 ticks per second */

/**
 * @brief	Handle interrupt from SysTick timer
 * @return	Nothing
 */
void SysTick_Handler(void)
{
	// wait until lidar is ready
	while( lidar_status() ){}

	Board_LED_Toggle(0);

	// read lidar distance
	uint16_t distance;
	distance = lidar_read();
	distance_to_sound( distance );

	if (distance < 92) //(3 feet)
	{
		//haptic_notify();
	}

	//printf("%hu\n", distance);

	lidar_request();
}

/**
 * @brief	main routine for systick example
 * @return	Function should not exit.
 */
int main(void)
{

	//Serial_begin(0, 57600);


	/* Generic Initialization */
	SystemCoreClockUpdate();
	Board_Init();

	i2c_app_init(I2C0, SPEED_100KHZ);
	//haptic_init();

	/* Enable and setup SysTick Timer at a periodic rate */
	SysTick_Config(SystemCoreClock / TICKRATE_HZ1);


	/* LEDs toggle in interrupt handlers */
	while (1) {
		__WFI();
	}

	return 0;
}



