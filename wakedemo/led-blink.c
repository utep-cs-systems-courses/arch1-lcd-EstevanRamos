#include <msp430.h>
#include "led.h"


void alternate_leds(int blink_count)  /* 1 sec = 250 interrupts */
{
  if(blink_count <= 125){    
    P1OUT |= LED_GREEN;
    P1OUT &= ~LED_RED;
  }else if(blink_count <= 250){
    P1OUT |= LED_RED;
    P1OUT &= ~LED_GREEN;
  }
}
