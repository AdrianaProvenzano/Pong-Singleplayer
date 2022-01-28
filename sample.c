/* Includes ------------------------------------------------------------------*/
#include "LPC17xx.h"
#include "GLCD/GLCD.h" 
#include "button_EXINT/button.h"
#include "RIT/RIT.h"
#include "adc/adc.h"
#include "timer/timer.h"
#include "game.h"

#define SIMULATOR 1

#ifdef SIMULATOR
extern uint8_t ScaleFlag; // <- ScaleFlag needs to visible in order for the emulator to find the symbol (can be placed also inside system_LPC17xx.h but since it is RO, it needs more work)
#endif

int main(void)
{
  SystemInit();  												/* inizializzazione sistema (i.e., PLL)  */
	BUTTON_init();												/* inizializzazione bottoni              */
	
	init_RIT(0x004C4B40);									/* inizializzazione RIT (50 msec) per il debouncing dei bottoni*/
	
	ADC_init();														/* inizializzazione ADC */	
	LCD_Initialization();									/* inizializzazione display */ 
		
	printWelcomeScreen(); 
	
	init_timer(0, 0x0003D090);						// timer per ADC (0x0003D090 -> 10ms non scalato)
	
	init_timer(1, 0x0000E848);						// timer per il movimento della palla (0x0000E848 -> 2,3 ms scalato)
 
	LPC_SC->PCON |= 0x1;									/* power-down	mode										*/
	LPC_SC->PCON &= ~(0x2);						
	
  LPC_PINCON->PINSEL1 |= (1<<21); 
	LPC_PINCON->PINSEL1 &= ~(1<<20); 
	LPC_GPIO0->FIODIR |= (1<<26); 
	
	while (1)	
  {
		//__ASM("wfi");
  }
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
