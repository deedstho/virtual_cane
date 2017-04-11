#include "board.h"
#include <stdio.h>
#include "i2c.h"
#include "haptic.h"
#include "pwm.h"
#include "lidar.h"
//#include "serial.h"

// *DEBUG* define scope pin and port
#define PIN_HIGH	1
#define PIN_LOW		0
#define SCOPE_PORT	0
#define SCOPE_PIN	9

#define TICKRATE_HZ1 (50)	/* 10 ticks per second */

/**
 * @brief	Handle interrupt from SysTick timer
 * @return	Nothing
 */
void SysTick_Handler(void)
{
	// *DEBUG* set H[53] high (for oscilloscope)
	Chip_GPIO_SetPinState(LPC_GPIO, SCOPE_PORT, SCOPE_PIN, PIN_HIGH);

	// wait until lidar is ready
	while( lidar_status() ){}

	// *DEBUG* blink LED
	Board_LED_Toggle(0);

	// read lidar distance
	uint16_t distance;
	distance = lidar_read();
	printf("%i\n",distance);

	// *DEBUG* print distance to uart
	//printf("%i\n", distance);


	// output pitch based on distance
	distance_to_sound( distance );

	// output vibration amplitude based on distance (correct for lidar distance offset)
	if (distance < 255) //(3 feet)
	{
		//haptic_notify();
		if(distance < 20) haptic_intensity(255);
		else haptic_intensity( 255 - distance );
	}
	else
	{
		haptic_intensity(0);
	}

	// request a measurement for the next tick
	lidar_request();

	// *DEBUG* set H[53] low (for oscilloscope)
	Chip_GPIO_SetPinState(LPC_GPIO, SCOPE_PORT, SCOPE_PIN, PIN_LOW);
}

/**
 * @brief	main routine for systick example
 * @return	Function should not exit.
 */
int main(void)
{
	// *DEBUG* initialize gpio (for oscilloscope)
	Chip_GPIO_Init(LPC_GPIO);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, SCOPE_PORT, SCOPE_PIN);

	/* Generic Initialization */
	SystemCoreClockUpdate();
	Board_Init();

	// initialize i2c
	i2c_app_init(I2C0, SPEED_100KHZ);

	// initialize haptic driver
	haptic_init();

	// playback mode
	haptic_playback_mode();

	/* Enable and setup SysTick Timer at a periodic rate */
	SysTick_Config(SystemCoreClock / TICKRATE_HZ1);


	/* LEDs toggle in interrupt handlers */
	while (1) {
		__WFI();
	}

	return 0;
}



