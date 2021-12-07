#include <msp430.h>
#include <libTimer.h>
#include "lcdutils.h"
#include "lcddraw.h"
#include "buzzer.h"
#include "led.h"
#include "led-assy.h"

// WARNING: LCD DISPLAY USES P1.0.  Do not touch!!! 

#define LED BIT6		/* note that bit zero req'd for display */

#define SW1 1
#define SW2 2
#define SW3 4
#define SW4 8

#define SWITCHES 15

static char 
switch_update_interrupt_sense()
{
  char p2val = P2IN;
  /* update switch interrupt to detect changes from current buttons */
  P2IES |= (p2val & SWITCHES);	/* if switch up, sense down */
  P2IES &= (p2val | ~SWITCHES);	/* if switch down, sense up */
  return p2val;
}

void 
switch_init()			/* setup switch */
{  
  P2REN |= SWITCHES;		/* enables resistors for switches */
  P2IE |= SWITCHES;		/* enable interrupts from switches */
  P2OUT |= SWITCHES;		/* pull-ups for switches */
  P2DIR &= ~SWITCHES;		/* set switches' bits for input */
  switch_update_interrupt_sense();
}

int switches = 0;

void
switch_interrupt_handler()
{
  char p2val = switch_update_interrupt_sense();
  switches = ~p2val & SWITCHES;
}


// axis zero for col, axis 1 for row
short drawPos[2] = {10,10}, controlPos[2] = {10,10};
short velocity[2] = {3,8}, limits[2] = {screenWidth-36, screenHeight-8};

short redrawScreen = 1;
u_int controlFontColor = COLOR_GREEN;

void wdt_c_handler()
{
  static int secCount = 0;

  secCount ++;
  if (secCount >= 25) {		/* 10/sec */
    secCount = 0;
    redrawScreen = 1;
  }
}

void main()
{
  
  // P1DIR |= LED;		/**< Green led on when CPU on */
  // P1OUT |= LED;
  configureClocks();
  lcd_init();
  switch_init();
  buzzer_init();
  led_init();
  
  enableWDTInterrupts();      /**< enable periodic interrupt */
  or_sr(0x8);	              /**< GIE (enable interrupts) */
  
  clearScreen(COLOR_BLUE);
  int size = 10;
  int drawn = 0;
  int seconds = 0;
  int blink_count = 0;
  
  while (1) {			/* forever */
    if (redrawScreen) {
      redrawScreen = 0;

      if(switches & SW4){
	christmas_tree(size);
	drawn = 1;
	size = size + 5;
      }

      if(switches & SW3){
	if(drawn){
	  addlights(size);
	}
      }

      if(switches & SW2){
	char* msg1 = "Merry";
	char* msg2 = "Christmas";
	drawString11x16(0,0,msg1,COLOR_WHITE, COLOR_BLUE);
	drawString11x16(11,16,msg2,COLOR_WHITE, COLOR_BLUE);

	alternate_leds(blink_count);
	blink_count = blink_count + 50;
	if(blink_count >250){
	  blink_count = 0;
	}
      }

      if(switches & SW1){
	playMerryChristmas();
      }
      
      if(size == 60){
	size = 0;
	clearScreen(COLOR_BLUE);
      }

      
    }
    // P1OUT &= ~LED;	/* led off */
    or_sr(0x10);	/**< CPU OFF */
    // P1OUT |= LED;	/* led on */
  }
}


void
addlights(int size){
  int row = 160, col = 128 / 2;
  drawPixel(row,row, COLOR_BLACK);
  
  row = row - size;
  int offset = (int)size/2;
  int edge = (int)size/8;

  
  fillRectangle(col+ offset, row - size + offset, edge, edge, COLOR_RED);
  fillRectangle(col- offset, row - offset, edge, edge, COLOR_YELLOW);
  fillRectangle(col-offset/2,row - size,edge, edge, COLOR_WHITE);
  fillRectangle(col- edge*2 ,row - size*2+edge, edge * 4, edge * 4, COLOR_YELLOW);
  fillRectangle(col+offset/2,row - size-offset + offset,edge,edge, COLOR_PURPLE);
  fillRectangle(col+offset,row+ offset/2-2, edge,edge, COLOR_RED);
  fillRectangle(col-offset,row + offset/2-2, edge, edge, COLOR_YELLOW);
  fillRectangle(col,row - offset, edge, edge, COLOR_BLUE);
}

void
christmas_tree(int size){
  static unsigned char row = screenHeight / 2, col = screenWidth / 2;

  u_char c_col = col;
  u_char c_row = screenHeight - 2 * size;
  int center =  (int)(size/2)/2;

  fillRectangle(c_col-center, screenHeight - size, size/2, size, COLOR_BROWN);
  for(int i = 0; i<size ; i++){
    for(int j = 0;j<size-i; j++){
      drawPixel(c_col + j ,c_row-i, COLOR_GREEN);
      drawPixel(c_col - j, c_row-i, COLOR_GREEN);

      u_char row2 = c_row + size/2;
      drawPixel(c_col + j ,row2 - i, COLOR_GREEN);
      drawPixel(c_col - j, row2 - i, COLOR_GREEN);

      u_char row3 = row2 + size/2;
      drawPixel(c_col + j, row3 - i, COLOR_GREEN);
      drawPixel(c_col - j, row3 - i, COLOR_GREEN);
    }
  }
}


/* Switch on S2 */
void
__interrupt_vec(PORT2_VECTOR) Port_2(){
  if (P2IFG & SWITCHES) {	      /* did a button cause this interrupt? */
    P2IFG &= ~SWITCHES;		      /* clear pending sw interrupts */
    switch_interrupt_handler();	/* single handler for all switches */
  }
}
