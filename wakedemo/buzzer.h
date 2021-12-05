#ifndef buzzer_included
#define buzzer_included

void buzzer_init();
void buzzer_set_period(short cycles);
void buzzer_off();
void play_happyBirthday(int seconds);
void playPart0();

unsigned char sound_state;
void playMerryChristmas();

#endif // included 
