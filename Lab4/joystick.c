#include <lpc17xx.h>
#include "joystick.h"

void init_joystick (void) 
{
    LPC_PINCON->PINSEL3 &= ~((3<<8)|(3<<14)|(3<<16)|(3<<18)|(3<<20)); 
    LPC_GPIO1->FIODIR &= ~((1<<20)|(1<<23)|(1<<24)|(1<<25)|(1<<26));
}

uint32_t read_joystick (void) 
{
  uint32_t k_val;
  k_val = (LPC_GPIO1->FIOPIN >> 20) & K_MASK;
  return k_val;
}
