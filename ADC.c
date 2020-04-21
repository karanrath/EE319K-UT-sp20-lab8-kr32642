// ADC.c
// Runs on LM4F120/TM4C123
// Provide functions that initialize ADC0
// Last Modified: 1/17/2020
// Student names: change this to your names or look very silly
// Last modification date: change this to the last modification date or look very silly

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"

// ADC initialization function 
// Input: none
// Output: none
// measures from PD2, analog channel 5
uint32_t delay;
void ADC_Init(void){ 
		SYSCTL_RCGCGPIO_R |= 0x08; // activate clock for Port D
		while ((SYSCTL_PRGPIO_R&0x08) == 0){};
		GPIO_PORTD_DIR_R &= ~0x04; // make PD2 input
		GPIO_PORTD_AFSEL_R |= 0x04; // enable alternate function on PD2
		GPIO_PORTD_DEN_R &= ~ 0x04; // disable digital I/O on PD2
		GPIO_PORTD_AMSEL_R |= 0x04; // enable analog function on PD2
		SYSCTL_RCGCADC_R |= 0x01; // activate ADC0
		delay = SYSCTL_RCGCADC_R; ; //extra TIMER0_CFG_R to stabilize
		delay = SYSCTL_RCGCADC_R; //extra TIMER0_CFG_R to stabilize
		delay = SYSCTL_RCGCADC_R; ; //extra TIMER0_CFG_R to stabilize
		delay = SYSCTL_RCGCADC_R; //extra TIMER0_CFG_R to stabilize
		ADC0_PC_R = 0x01; // configure for 125K 
		ADC0_SSPRI_R = 0x0123; // Seq 3 is highest priority
		ADC0_ACTSS_R &= ~0x0008; // disable sample sequencer 3
		ADC0_EMUX_R &= ~0xF000; // seq3 is software trigger
		ADC0_SSMUX3_R = (ADC0_SSMUX3_R&0xFFFFFFF0)+5; // Ain5 PD2
		ADC0_SSCTL3_R = 0x0006; // no TS0 D0, yes IE0 END0
		ADC0_IM_R &= ~0x0008; // disable SS3 interrupts
		ADC0_ACTSS_R |= 0x0008; // enable sample sequencer 3

}

//------------ADC_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
// measures from PD2, analog channel 5
uint32_t data;
uint32_t ADC_In(void){  
		ADC0_PSSI_R = 0x0008;            // 1) initiate seq 3
		while((ADC0_RIS_R&0x08)==0){};   // 2) wait for conversion complete
		data = ADC0_SSFIFO3_R&0xFFF; 	  // 3) read result
		ADC0_ISC_R = 0x0008;             // 4) reset ris flag
		return data;
  
}


