// Lab8.c
// Runs on LM4F120 or TM4C123
// Student names: change this to your names or look very silly
// Last modification date: change this to the last modification date or look very silly
// Last Modified: 1/17/2020 

// Specifications:
// Measure distance using slide pot, sample at 10 Hz
// maximum distance can be any value from 1.5 to 2cm
// minimum distance is 0 cm
// Calculate distance in fixed point, 0.01cm
// Analog Input connected to PD2=ADC5
// displays distance on Sitronox ST7735
// PF3, PF2, PF1 are heartbeats (use them in creative ways)
// 

#include <stdint.h>

#include "ST7735.h"
#include "TExaS.h"
#include "ADC.h"
#include "print.h"
#include "../inc/tm4c123gh6pm.h"

//*****the first three main programs are for debugging *****
// main1 tests just the ADC and slide pot, use debugger to see data
// main2 adds the LCD to the ADC and slide pot, ADC data is on ST7735
// main3 adds your convert function, position data is no ST7735

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))

// Initialize Port F so PF1, PF2 and PF3 are heartbeats
uint32_t delay1;
void PortF_Init(void){
SYSCTL_RCGCGPIO_R |= 0X20;
delay1 = 150;
GPIO_PORTF_DIR_R |= 0X02; 
GPIO_PORTF_DEN_R |= 0X02;
}

uint32_t Data;        // 12-bit ADC
uint32_t Position;    // 32-bit fixed-point 0.001 cm
int main1(void){       // single step this program and look at Data
  TExaS_Init();       // Bus clock is 80 MHz 
  ADC_Init();         // turn on ADC, set channel to 5
  while(1){                
    Data = ADC_In();  // sample 12-bit channel 5
  }
}
uint32_t time0,time1,time2,time3;
uint32_t ADCtime,OutDectime; // in usec
int main2(void){
  TExaS_Init();   // Bus clock is 80 MHz
  NVIC_ST_RELOAD_R = 0x00FFFFFF; // maximum reload value
  NVIC_ST_CURRENT_R = 0;       // any write to current clears it
  NVIC_ST_CTRL_R = 5;
  ADC_Init();     // turn on ADC, set channel to 5
  ADC0_SAC_R = 4;   // 16-point averaging, move this line into your ADC_Init()
  ST7735_InitR(INITR_REDTAB);
  while(1){       // use SysTick 
    time0 = NVIC_ST_CURRENT_R;
    Data = ADC_In();  // sample 12-bit channel 5
    time1 = NVIC_ST_CURRENT_R;
    ST7735_SetCursor(0,0);
    time2 = NVIC_ST_CURRENT_R;
    LCD_OutDec(Data);
    ST7735_OutString(" ");  // spaces cover up characters from last output
    time3 = NVIC_ST_CURRENT_R;
    ADCtime = ((time0-time1)&0x0FFFFFF)/80; // usec
    OutDectime = ((time2-time3)&0x0FFFFFF)/80; // usec
  }
}

// your function to convert ADC sample to distance (0.01cm)
uint32_t Convert(uint32_t Data){
  return (Data*118)/4096+6; // replace this line with your Lab 8 solution
}
int main3(void){ 
  TExaS_Init();         // Bus clock is 80 MHz 
  ST7735_InitR(INITR_REDTAB); 
  PortF_Init();
  ADC_Init();         // turn on ADC, set channel to 5
  while(1){  
    PF2 ^= 0x04;      // Heartbeat
    Data = ADC_In();  // sample 12-bit channel 5
    PF3 = 0x08;       // Profile Convert
    Position = Convert(Data); 
    PF3 = 0;          // end of Convert Profile
    PF1 = 0x02;       // Profile LCD
    ST7735_SetCursor(0,0);
    LCD_OutDec(Data); 
		ST7735_OutString("    "); 
    ST7735_SetCursor(6,0);
    LCD_OutFix(Position);
    PF1 = 0;          // end of LCD Profile
  }
}  


uint32_t ADCStatus;
uint32_t ADCMail;

void SysTick_Init(){
 NVIC_ST_CTRL_R = 0;
 NVIC_ST_RELOAD_R = 7999999; // (80*10^6/10) - 1;  //reload value for 10Hz
 NVIC_ST_CURRENT_R = 0;
 NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R & 0X00FFFFFF) | 0X20000000; //set priority
 NVIC_ST_CTRL_R = 7; //allow interrupts
}

void SysTick_Handler(void){
	GPIO_PORTF_DATA_R ^= 0X02; //heart 
	ADCMail = ADC_In(); //sample 12-bit ADC value and store in ADCMail
	ADCStatus = 1; //set flag to inticate new data
}

int main(void){ //you're Lab 8
  TExaS_Init();
	ST7735_InitR(INITR_REDTAB); 
  ADC_Init();         // turn on ADC, set channel to 5
	SysTick_Init();
	PortF_Init();
  EnableInterrupts();
  while(1){
		while(ADCStatus == 0){}; //
		Data = ADCMail;
		ADCStatus = 0; //clear flag 
		Position = Convert(Data);
		ST7735_SetCursor(0,0);
		LCD_OutFix(Position);
		ST7735_SetCursor(6,0);
		ST7735_OutString("cm"); 
  }
 }
