# makfile configuration
CPU             	= msp430g2553
CFLAGS          	= -mmcu=${CPU} -Os -I../h
LDFLAGS		= -L../lib -L/opt/ti/msp430_gcc/include/ 

#switch the compiler (for the internal make rules)
CC              = msp430-elf-gcc
AS              = msp430-elf-gcc -mmcu=${CPU} -c

all:wakedemo.elf

#additional rules for files
wakedemo.elf: ${COMMON_OBJECTS} wakedemo.o wdt_handler.o buzzer.o led.o led-blink.o
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ $^ -lTimer -lLcd

wakedemo-s.elf: ${COMMON_OBJECTS} wakedemo.o wdt_handler.o buzzer.o led.o led-assy.o
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ $^ -lTimer -lLcd

load: wakedemo.elf
	msp430loader.sh $^

load-s: wakedemo-s.elf
	msp430loader.sh $^

led.o: led.h

buzzer.o: buzzer.h

led-assy.o: led.h led-assy.h

led-blink.o: led.h led-assy.h

clean:
	rm -f *.o *.elf
