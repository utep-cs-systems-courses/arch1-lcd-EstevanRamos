#include <msp430.h>
#include <libTimer.h>
#include "lcdutils.h"
#include "lcddraw.h"

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
  
void update_shape();


void main()
{
  
  P1DIR |= LED;		/**< Green led on when CPU on */
  P1OUT |= LED;
  configureClocks();
  lcd_init();
  switch_init();
  
  enableWDTInterrupts();      /**< enable periodic interrupt */
  or_sr(0x8);	              /**< GIE (enable interrupts) */
  
  clearScreen(COLOR_BLUE);
  int size = 10;
  int drawn = 0;
  while (1) {			/* forever */
    if (redrawScreen) {
      redrawScreen = 0;

      if(switches & SW4){
	if(size % 5 == 0){
	  christmas_tree(size);
	  drawn = 1;
	}
	size++;
      }

      if(switches & SW3){
	if(drawn){
	  addlights(size);
	}
      }
      if(switches & SW2){
	char* msg = "Merry Christmas";
	drawString5x7(0,0,msg,COLOR_WHITE, COLOR_BLUE);
      }
      
      if(size == 60){
	size = 0;
	clearScreen(COLOR_BLUE);
      }

      
    }
    P1OUT &= ~LED;	/* led off */
    or_sr(0x10);	/**< CPU OFF */
    P1OUT |= LED;	/* led on */
  }
}


void
addlights(int size){
  static unsigned char row = screenHeight, col = screenWidth / 2;
  row = row - size;
  int offset = (int)size/2;
  drawPixel(col+ offset, row - size + offset, COLOR_RED);
  drawPixel(col- offset, row - offset, COLOR_BLUE);
  drawPixel(col,row - size, COLOR_YELLOW);
  drawPixel(col,row - size - offset , COLOR_BLACK);
  drawPixel(col+ offset ,row - offset, COLOR_ORANGE);
  drawPixel(col-offset,row - size + offset, COLOR_PURPLE);
  drawPixel(col+offset,row, COLOR_RED);
  drawPixel(col,row - offset, COLOR_BLUE);
}

void
christmas_tree(int size){
  //clearScreen(COLOR_BLUE);
  static unsigned char row = screenHeight / 2, col = screenWidth / 2;

  u_char c_col = col;
  u_char c_row = screenHeight - 2 * size;
  int center =  (int)(size/2)/2;

  //if(switches & SW4) return;

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
    
    
void
update_shape()
{
  static unsigned char row = screenHeight / 2, col = screenWidth / 2;
  static char blue = 31, green = 0, red = 31;
  static unsigned char step = 0;
  if (switches & SW4) return;
  if (step <= 60) {
    int startCol = col - step;
    int endCol = col + step;
    int width = 1 + endCol - startCol;
    // a color in this BGR encoding is BBBB BGGG GGGR RRRR
    unsigned int color = (blue << 11) | (green << 5) | red;
    fillRectangle(startCol, row+step, width, 1, color);
    fillRectangle(startCol, row-step, width, 1, color);
    if (switches & SW3) green = (green + 1) % 64;
    if (switches & SW2) blue = (blue + 2) % 32;
    if (switches & SW1) red = (red - 3) % 32;
    step ++;
  } else {
     clearScreen(COLOR_BLUE);
     step = 0;
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
