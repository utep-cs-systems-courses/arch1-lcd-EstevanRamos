#include <msp430.h>
#include "libTimer.h"
#include "buzzer.h"

void buzzer_init()
{
    /* 
       Direct timer A output "TA0.1" to P2.6.  
        According to table 21 from data sheet:
          P2SEL2.6, P2SEL2.7, anmd P2SEL.7 must be zero
          P2SEL.6 must be 1
        Also: P2.6 direction must be output
    */
    timerAUpmode();		/* used to drive speaker */
    P2SEL2 &= ~(BIT6 | BIT7);
    P2SEL &= ~BIT7; 
    P2SEL |= BIT6;
    P2DIR = BIT6;		/* enable output to speaker (P2.6) */
}

void buzzer_set_period(short cycles)
{
  CCR0 = cycles; 
  CCR1 = cycles >> 1;		/* one half cycle */
}
  
void buzzer_off(){
  buzzer_set_period(0);
}

unsigned short song_state = 0;

void playMerryChristmas(){
  if(song_state == 78){
    song_state = 0;
  }
  short christmas[77] = {523,523,1,699,699,1,699,1,784,1,699,1,659,1,587,587,1,587,587,1,587,587,1,784,784,1,784,1,880,1,784,1,699,1,659,659,1,523,523,1,523,523,1,880,880,1,880,932,1,880,1,784,1,699,699,1,587,587,1,523,1,523,1,587,587,1,784,784,1,659,659,1,784,784,784,784};
  short note = 2000000/christmas[song_state];
  buzzer_set_period(note);
  song_state++;
}



