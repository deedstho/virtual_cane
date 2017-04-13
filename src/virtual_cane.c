#include "board.h"
#include <stdio.h>
#include "i2c.h"
#include "haptic.h"
#include "pwm.h"
#include "lidar.h"
#include "accelerometer.h"
//#include "serial.h"

// *DEBUG* define scope pin and port
#define PIN_HIGH	1
#define PIN_LOW		0
#define SCOPE_PORT	0
#define SCOPE_PIN	9

#define TICKRATE_HZ1 (100)	/* 10 ticks per second */

static uint8_t haptic_sleep = 0;
static uint8_t bool_service_EINT3 = 0;
static uint8_t bool_service_systick = 0;

void initialize_sleep_interrupt(void){
	uint32_t pin = 0x00020000;	//H[12] & P0[17]
	//enable GPIO pin as input
	Chip_GPIO_Init(LPC_GPIO);
	Chip_GPIO_SetPinDIRInput(LPC_GPIO, 0, 17);
	//enable GPIO pin as interrupt
	Chip_GPIOINT_Init(LPC_GPIOINT);
	Chip_GPIOINT_SetIntRising(LPC_GPIOINT, GPIOINT_PORT0, pin);
	Chip_GPIOINT_SetIntFalling(LPC_GPIOINT, GPIOINT_PORT0, pin);

	NVIC_ClearPendingIRQ(EINT3_IRQn);

	NVIC_EnableIRQ(EINT3_IRQn);
}

//void service_EINT3(void)
//{
////	// Disable Systick
////	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
//
//	// clear pending interrupts from this source
//	NVIC_ClearPendingIRQ(EINT3_IRQn);
//
//	//Chip_GPIO_SetPinState(LPC_GPIO, 0, 18, 1);
//
//	// go to sleep
//	if(Chip_GPIOINT_GetStatusRising(LPC_GPIOINT, GPIOINT_PORT0) == 0x00020000){
//		Chip_GPIOINT_ClearIntStatus(LPC_GPIOINT, 0, 0x00020000);
//
//		// Disable Systick
//		SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
//
//		//Chip_GPIO_SetPinState(LPC_GPIO, 0, 18, 1);// DEBUG
//		haptic_standby_mode();
//		lidar_sleep();
//
//		//haptic_sleep = 1;
//	}
//
//	// wake up
//	else if(Chip_GPIOINT_GetStatusFalling(LPC_GPIOINT, GPIOINT_PORT0) == 0x00020000){
//		Chip_GPIOINT_ClearIntStatus(LPC_GPIOINT, 0, 0x00020000);
//
//		//Chip_GPIO_SetPinState(LPC_GPIO, 0, 18, 0);// DEBUG
//
//		lidar_wake();
//		haptic_playback_mode();
//
//		// Enable Systick
//		SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
//
//		//haptic_sleep = 0;
//	}
//}

//Chip_GPIO_SetPinState(LPC_GPIO, 0, 18, 0);

//void EINT3_IRQHandler(void)
//{
//	bool_service_EINT3 = 1;
//}

void EINT3_IRQHandler(void)
{
	// Disable Systick
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;

	// clear pending interrupts from this source
	NVIC_ClearPendingIRQ(EINT3_IRQn);

	//Chip_GPIO_SetPinState(LPC_GPIO, 0, 18, 1);

	// go to sleep
	if(Chip_GPIOINT_GetStatusRising(LPC_GPIOINT, GPIOINT_PORT0) == 0x00020000){
		Chip_GPIOINT_ClearIntStatus(LPC_GPIOINT, 0, 0x00020000);

		// Disable Systick
		//SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;

		//Chip_GPIO_SetPinState(LPC_GPIO, 0, 18, 1);// DEBUG
		haptic_standby_mode();
		lidar_sleep();

		haptic_sleep = 1;
	}

	// wake up
	else if(Chip_GPIOINT_GetStatusFalling(LPC_GPIOINT, GPIOINT_PORT0) == 0x00020000){
		Chip_GPIOINT_ClearIntStatus(LPC_GPIOINT, 0, 0x00020000);

		//Chip_GPIO_SetPinState(LPC_GPIO, 0, 18, 0);// DEBUG

		lidar_wake();
		haptic_playback_mode();

		// Enable Systick
		SysTick_Config(SystemCoreClock / TICKRATE_HZ1);
		SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;

		haptic_sleep = 0;
	}
}

//void service_systick(void)
//{
//	//	// *DEBUG* set H[53] high (for oscilloscope)
//	//	Chip_GPIO_SetPinState(LPC_GPIO, SCOPE_PORT, SCOPE_PIN, PIN_HIGH);
//	//
//		// wait until lidar is ready
//	//	while( lidar_status() ){}
//
//		// *DEBUG* blink LED
//		Board_LED_Toggle(0);
//
//	//	// read lidar distance
//		uint16_t distance;
//		distance = lidar_read();
//	//
//	//	// *DEBUG* print distance to uart
//	//	//printf("%i\n", distance);
//
//		// output pitch based on distance
//		distance_to_sound( distance );
//
//		// output vibration amplitude based on distance (correct for lidar distance offset)
//		if (distance < 255) //(3 feet)
//		{
//			//haptic_notify();
//			if(distance < 20) haptic_intensity(255);
//			else haptic_intensity( 255 - distance );
//		}
//		else
//		{
//			haptic_intensity(0);
//		}
//
//		// request a measurement for the next tick
//		lidar_request();
//	//
//	//	// *DEBUG* set H[53] low (for oscilloscope)
//	//	Chip_GPIO_SetPinState(LPC_GPIO, SCOPE_PORT, SCOPE_PIN, PIN_LOW);
//}

/**
 * @brief	Handle interrupt from SysTick timer
 * @return	Nothing
 */
//void SysTick_Handler(void)
//{
//	bool_service_systick = 1;
//}

void SysTick_Handler(void)
{
	// Disable EINT3
	//NVIC_DisableIRQ(EINT3_IRQn);

	//	// *DEBUG* set H[53] high (for oscilloscope)
	Chip_GPIO_SetPinState(LPC_GPIO, SCOPE_PORT, SCOPE_PIN, PIN_HIGH);
	//
		// wait until lidar is ready
	//	while( lidar_status() ){}

		// *DEBUG* blink LED
		Board_LED_Toggle(0);

	//	// read lidar distance
		uint16_t distance;
		distance = lidar_read();
	//
	//	// *DEBUG* print distance to uart
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

	//	// *DEBUG* set H[53] low (for oscilloscope)
	Chip_GPIO_SetPinState(LPC_GPIO, SCOPE_PORT, SCOPE_PIN, PIN_LOW);

	// Enable EINT3
	//NVIC_EnableIRQ(EINT3_IRQn);
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

	// initialize accelerometer
	accel_init();

	// allow board to receive interrupts from accelerometer
	initialize_sleep_interrupt(); ///////////////////// ACCELEROMETER

	// Enable Systick
	SysTick_Config(SystemCoreClock / TICKRATE_HZ1);

	/* LEDs toggle in interrupt handlers */
	while (1) {
		__WFI();
	}

	return 0;
}
