#ifndef INPUTS_H
#define INPUTS_H

void input_init(void);
void joystick_init(void);
unsigned char read_joystick(void);
void led_init(void);
void potentiometer_init(void);
unsigned char debounce(void);
#endif
