// pwm.h

#ifndef PWM_H
#define PWM_H

#define SBIT_CNTEN     0
#define SBIT_PWMEN     2

#define SBIT_PWMMR0R   1

#define SBIT_LEN0      0
#define SBIT_LEN1      1
#define SBIT_LEN2      2
#define SBIT_LEN3      3
#define SBIT_LEN4      4

#define SBIT_PWMENA1   9
#define SBIT_PWMENA2   10
#define SBIT_PWMENA3   11
#define SBIT_PWMENA4   12

#define PWM_1          0 //P2_0 (0-1 Bits of PINSEL4)
#define PWM_2          2 //P2_1 (2-3 Bits of PINSEL4)
#define PWM_3          4 //P2_2 (4-5 Bits of PINSEL4)
#define PWM_4          6 //P2_3 (6-7 Bits of PINSEL4)


static LPC_PWM_T * pTMR = (LPC_PWM_T*) LPC_PWM1;

// ** PUBLIC ** //


// ** PRIVATE ** //

// input: distance (cm)
// output: pwm sound
// currently usable between 3cm and ~1597 cm
void distance_to_sound(uint16_t in_distance)
{

	// RESET
    Chip_PWM_Reset(pTMR);

    /* Cofigure pins(P2_0 - P2_3) for PWM mode. */
    LPC_IOCON->PINSEL[4] = (1<<PWM_1) | (1<<PWM_2) | (1<<PWM_3) | (1<<PWM_4); /////    SKIPPING THIS
    //Chip_IOCON_PinMux(LPC_IOCON, uint8_t port, uint8_t pin, uint32_t mode, IOCON_FUNC1);

    /* Enable Counters,PWM module */
    pTMR->TCR = (1<<SBIT_CNTEN) | (1<<SBIT_PWMEN);  ///

    uint32_t prescale = 99;				// prescale x100
    pTMR->PR  =  prescale;
    pTMR->MCR = (1<<SBIT_PWMMR0R);  /*Reset on PWMMR0, reset TC if it matches MR0 */

    // make sure we don't have a value that will scale to negative - bad behavior unsigned
    if (in_distance < 20) in_distance = 20;

    uint32_t period =  2 * ((uint32_t)in_distance) + 16 - 2*20;

    uint32_t width = period / 2;
    pTMR->MR0 = period;
    pTMR->MR1 = width;
    pTMR->MR2 = width;
    pTMR->MR3 = width;
    pTMR->MR4 = width;
    /* Trigger the latch Enable Bits to load the new Match Values */
    pTMR->LER = (1<<SBIT_LEN0) | (1<<SBIT_LEN1) | (1<<SBIT_LEN2) | (1<<SBIT_LEN3) | (1<<SBIT_LEN4);

    /* Enable the PWM output pins for PWM_1-PWM_4(P2_0 - P2_3) */
    pTMR->PCR = (1<<SBIT_PWMENA1) | (1<<SBIT_PWMENA2) | (1<<SBIT_PWMENA3) | (1<<SBIT_PWMENA4);
}

void pwm_sleep(uint8_t duty)
{
	// TODO
}


#endif /* PWM_H */
