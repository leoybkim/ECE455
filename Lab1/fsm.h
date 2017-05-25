#ifndef __FSM_H_
#define __FSM_H_

typedef enum {
// inputs go here
    FURNACE_ON,
    FURNACE_OFF
} fsm_input;

typedef unsigned int state;

void init_FSM(void);
void input_FSM(fsm_input);

// user defined
void FSM_action(state);

#endif

