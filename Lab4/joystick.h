#ifndef INPUTS_H
#define INPUTS_H

#define K_SELECT      0x78               
#define K_LEFT        0x39               
#define K_UP          0x71               
#define K_RIGHT       0x69               
#define K_DOWN        0x59
#define K_MASK        0x79  

void init_joystick (void);
uint32_t read_joystick (void);

#endif
