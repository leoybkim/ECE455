#include <lpc17xx.h> 
#include "glcd.h"

typedef int bool;
#define TRUE 1
#define FALSE 0

/* Global variables */
const led_pos[8] = { 28, 29, 31, 2, 3, 4, 5, 6 };
int led = 7;    // Furnace indicator
bool wait;      // Wait indicator; Furnace must wait for period of time before turning back on again if it has been previously timed out 
bool timeout;   // T.O. indicator; Furnace must timeout after having been on for period of time 
int mask;       // LED masker

/* Input variables */
int wait_t;
int timeout_t;

/* STATE MACHINE */
enum states { STATE_OFF, STATE_INTER, STATE_ON, STATE_ERR, MAX_STATES } current_state;
enum events { HEAT_REQUEST, HEAT, COOL, TIME_OUT, REPEAT, ERR, MAX_EVENTS } new_events;

void action_s1_e1 (void);
void action_s1_e2 (void);
void action_s1_e3 (void);
void action_s1_e4 (void);
void action_s1_e5 (void);
void action_s1_e6 (void);
void action_s2_e1 (void);
void action_s2_e2 (void);
void action_s2_e3 (void);
void action_s2_e4 (void);
void action_s2_e5 (void);
void action_s2_e6 (void);
void action_s3_e1 (void);
void action_s3_e2 (void);
void action_s3_e3 (void);
void action_s3_e4 (void);
void action_s3_e5 (void);
void action_s3_e6 (void);
void action_s4_e1 (void);
void action_s4_e2 (void);
void action_s4_e3 (void);
void action_s4_e4 (void);
void action_s4_e5 (void);
void action_s4_e6 (void);
enum events get_new_events (int desired_temp, int measured_temp);

void (*const state_transitions [MAX_STATES][MAX_EVENTS]) (void) = {
    { action_s1_e1, action_s1_e2, action_s1_e3, action_s1_e4, action_s1_e5, action_s1_e6 },
    { action_s2_e1, action_s2_e2, action_s2_e3, action_s2_e4, action_s2_e5, action_s2_e6 },
    { action_s3_e1, action_s3_e2, action_s3_e3, action_s3_e4, action_s3_e5, action_s3_e6 },
    { action_s4_e1, action_s4_e2, action_s4_e3, action_s4_e4, action_s4_e5, action_s4_e6 }
};

void input_init(void) 
{
    // cofig INT0 button for input
    LPC_PINCON->PINSEL4 &= ~(3<<20); // P2.10 is GPIO
    LPC_GPIO2->FIODIR &= ~(1<<10);   // P2.10 is input
}

void joystick_init(void)
{
    // this step is actually not required normally since they are by default are set to GPIO
    LPC_PINCON->PINSEL3 &= ~((3<< 8) | (3<<14) | (3<<16) | (3<<18) | (3<<20));     /* P1.20, P1.23..26 is GPIO (Joystick) */

    // set the I/O direction; There are five LPC_GPIOx, where x=0,1,2,3,4 
    // To set pin as input, set the corresponding bit in FIODIR to 0, for output, set FIODIR to 1
    // By default all I/Os are input
    LPC_GPIO1->FIODIR &= ~((1<<20) | (1<<23) | (1<<24) | (1<<25) | (1<<26));     /* P1.20, P1.23..26 is input */
}

unsigned char read_joystick(void)
{
    char kbd_val = ~(LPC_GPIO1->FIOPIN >> 20) & 0x79; // 0x79 === 0111 1001
    return kbd_val;
}

unsigned char debounce(void) 
{
    // debounce: read the value 3 times in a row; iff the 3 values align, register as input
    char debounce_frame[3] = {0};
    char kbd_val; 
    debounce_frame[0] = read_joystick();
    debounce_frame[1] = read_joystick();
    debounce_frame[2] = read_joystick();
    
    if ((debounce_frame[0] == 0x08 && debounce_frame[1] == 0x08 && debounce_frame[2] == 0x08)
        || (debounce_frame[0] == 0x20 && debounce_frame[1] == 0x20 && debounce_frame[2] == 0x20))
    {
        kbd_val = debounce_frame[2];
    }
    return kbd_val;    
}

void led_init(void) 
{
    // configure LEDs 
    LPC_GPIO1->FIODIR |= 0xB0000000; // LEDs on PORT1 P1.28..29, P1.31 is output  // 1011 0000 0000 0000 0000 0000 0000 0000
    LPC_GPIO2->FIODIR |= 0x0000007C; // LEDs on PORT2 P2.2..6 is output           // 0000 0000 0000 0000 0000 0000 0111 1100
}

void potentiometer_init(void)
{
    LPC_SC->PCONP |= (1 << 12);      // enable power to ADC
    
    LPC_PINCON->PINSEL1 &= ~(3<<18); // clear bits
    LPC_PINCON->PINSEL1 |= (1<<18);  // set bits P0.25 is AD0.2
    
    LPC_ADC->ADCR =  ( 1 <<  2 ) |   // select the second channel
                     ( 4 <<  8 ) |   // ADC clock = 25 MHz / 5
                     ( 1 << 21 );    // enable ADC
    
    LPC_ADC->ADINTEN = ( 1 << 8 );   // enable interrupt for all ADC channels
    
    //NVIC_EnableIRQ(ADC_IRQn);      // register interrupt
}

// timer to wake up from sleep
void timer0_init(void)
{
    LPC_TIM0->TCR = 0x02;         // reset time
    LPC_TIM0->TCR = 0x01;         // enable timer
    LPC_TIM0->MR0 = 4000000;      // match value (can be anything)
    LPC_TIM0->MCR |= 0x03;        // on match, generate interrupt and reset
    NVIC_EnableIRQ(TIMER0_IRQn);  // allow interrupts from the timer
}

void timer1_init(void)
{
    LPC_TIM1->TCR = 0x02;         // reset time
    LPC_TIM1->TCR = 0x01;         // enable timer
    LPC_TIM1->MR0 = 40000000;     // match value (can be anything)
    LPC_TIM1->MCR |= 0x03;        // on match, generate interrupt and reset
    NVIC_EnableIRQ(TIMER1_IRQn);  // allow interrupts from the timer

    wait_t    = 0;
    timeout_t = 0;
    wait      = FALSE;
    timeout   = FALSE;
}

void TIMER0_IRQHandler(void)
{
    if((LPC_TIM0->IR & 0x01) == 0x01) // if MR0 interrupt
    {
        LPC_TIM0->IR |= 1 << 0; 
    }
}

void TIMER1_IRQHandler(void)
{    
    if((LPC_TIM1->IR & 0x01) == 0x01) // if MR0 interrupt
    {
        LPC_TIM1->IR |= 1 << 0; 
        
        if (wait) { wait_t += 1; } // if waiting, increment wait timer; must wait for 5 unit time before turning on the furnace again after it timeouts
        if (((LPC_GPIO2->FIOSET >> led_pos[led]) & 0x01) == 0x01) {timeout_t += 1; } // if the furnace is on, increment timeout timer; time out is triggered after 10 unit time
    }
    
    if (((LPC_GPIO2->FIOSET >> led_pos[led]) & 0x01) == 0x01) {
        GLCD_DisplayString(9, 0, 1, "on ");
        if(!timeout && timeout_t >= 10) {
            timeout = TRUE;
            wait = TRUE;
            timeout_t = 0;
        }
    } else {
        timeout = FALSE;
        timeout_t = 0;
        GLCD_DisplayString(9, 0, 1, "off");
    }
    
    if(wait && wait_t >= 5) {
        wait = FALSE;
        wait_t = 0;
    }
}

// Determine the type of event with input variables
// Desired temperature has to have a difference greater than 2 units 
// from the measured temperature to avoid hysteresis
enum events get_new_events(int desired_temp, int measured_temp) 
{
    if (desired_temp > (measured_temp + 2)) {
        if (wait) {
            return HEAT_REQUEST;
        } else if (!wait && !timeout) {
            return HEAT;
        } else if (timeout) {
            timeout = FALSE;
            return TIME_OUT;
        } else {
            return ERR;
        }
    } else if (desired_temp < measured_temp - 2) {
        timeout = FALSE;
        return COOL;
    } else {
        return REPEAT;
    }
}


/* FSM ACTIONS */

// OFF
void action_s1_e1 (void) { current_state = STATE_INTER; LPC_GPIO2->FIOCLR = mask; }
void action_s1_e2 (void) { current_state = STATE_ON;    LPC_GPIO2->FIOCLR = mask; }
void action_s1_e3 (void) { /*DO NOTHING */              LPC_GPIO2->FIOCLR = mask; }
void action_s1_e4 (void) { current_state = STATE_ERR;   LPC_GPIO2->FIOCLR = mask; }
void action_s1_e5 (void) { /*DO NOTHING */              LPC_GPIO2->FIOCLR = mask; }
void action_s1_e6 (void) { current_state = STATE_ERR;   LPC_GPIO2->FIOCLR = mask; }
 
// INTERMEDIATE STEP 
void action_s2_e1 (void) { /*DO NOTHING */              LPC_GPIO2->FIOCLR = mask; }
void action_s2_e2 (void) { current_state = STATE_ON;    LPC_GPIO2->FIOCLR = mask; }
void action_s2_e3 (void) { current_state = STATE_OFF;   LPC_GPIO2->FIOCLR = mask; }
void action_s2_e4 (void) { current_state = STATE_ERR;   LPC_GPIO2->FIOCLR = mask; }
void action_s2_e5 (void) { /*DO NOTHING */              LPC_GPIO2->FIOCLR = mask; }
void action_s2_e6 (void) { current_state = STATE_ERR;   LPC_GPIO2->FIOCLR = mask; }
 
// ON 
void action_s3_e1 (void) { current_state = STATE_ERR;   LPC_GPIO2->FIOSET = mask; }
void action_s3_e2 (void) { /*DO NOTHING */              LPC_GPIO2->FIOSET = mask; }
void action_s3_e3 (void) { current_state = STATE_OFF;   LPC_GPIO2->FIOSET = mask; }
void action_s3_e4 (void) { current_state = STATE_OFF;   LPC_GPIO2->FIOSET = mask; }
void action_s3_e5 (void) { /*DO NOTHING */              LPC_GPIO2->FIOSET = mask; }
void action_s3_e6 (void) { current_state = STATE_ERR;   LPC_GPIO2->FIOSET = mask; }
 
// ERROR: restart when enc ountered with error
void action_s4_e1 (void) { current_state = STATE_OFF;   LPC_GPIO2->FIOCLR = mask; }
void action_s4_e2 (void) { current_state = STATE_OFF;   LPC_GPIO2->FIOCLR = mask; }
void action_s4_e3 (void) { current_state = STATE_OFF;   LPC_GPIO2->FIOCLR = mask; }
void action_s4_e4 (void) { current_state = STATE_OFF;   LPC_GPIO2->FIOCLR = mask; }
void action_s4_e5 (void) { current_state = STATE_OFF;   LPC_GPIO2->FIOCLR = mask; }
void action_s4_e6 (void) { current_state = STATE_OFF;   LPC_GPIO2->FIOCLR = mask; }

int main(void)
{
    /* Debugging tools */
    char str_joystick[10];    
    char str_potentiometer[10];
    char str_debug[12];
    char str_timer[12];
    char str_status[12];

    /* Initialize system */
    int measured_temp;        // simulated with potentiometer reading
    int desired_temp;         // controlled with joystick
    int joystick_val = 0;     // determine direction (up or down)
    int new_event;            // FSM event
    mask = 1 << led_pos[led]; // LED P2.6
    current_state = 1;        // default state: STATE_OFF
    desired_temp = 25;        // default desired temperature
    wait    = FALSE;          // wait furnace
    timeout = FALSE;          // timeout
    SystemInit();   
    GLCD_Init();    
    GLCD_Clear(White);  
    GLCD_DisplayString(0, 0, 1, str_joystick); 
    joystick_init();                              // desired temperature controller
    potentiometer_init();                         // measured temperature simulator
    led_init();                                   // furnace indicator (P2.6)
    timer0_init();                                // power mode timer interrupt 
    timer1_init();                                // furnace timer interrupt

    sprintf(str_joystick, " desired temp: %d", desired_temp);
    
    /* Main Loop */
    for(;;)
    {
        /* Poll Potentiometer*/
        LPC_ADC->ADCR |= ( 1 << 24 );                     // read the converted value
        while (LPC_ADC->ADGDR & 0x8000 == 0);             // wait for conversion
        measured_temp = ((LPC_ADC->ADGDR>>4) & 0xFFF) / 80; // conversion to celcius range

        /* Poll Joystick */
        joystick_val = debounce();
        if (joystick_val == 0x08) {
            desired_temp += 1;
        } else if (joystick_val == 0x20) {
            desired_temp -= 1;
        }

        /* Display */
        sprintf(str_potentiometer, "measured temp: %d", measured_temp);
        GLCD_DisplayString(1, 0, 1, str_potentiometer);
        sprintf(str_joystick, " desired temp: %d", desired_temp);
        GLCD_DisplayString(0, 0, 1, str_joystick);

        new_event = get_new_events(desired_temp, measured_temp);    // determine next event for FSM
        if (((new_event >= 0) && (new_event < MAX_EVENTS)) && ((current_state >= 0) && (current_state < MAX_STATES))) {
            state_transitions [current_state][new_event] ();        // call the action procedure 

            sprintf(str_debug, "state:%2d event:%2d", current_state+1, new_event+1);
            sprintf(str_timer, "wait:%d to:%d", wait_t, timeout_t);
            sprintf(str_status,"wait:%d to:%d", wait,   timeout);
            GLCD_DisplayString(4, 0, 1, str_debug);
            GLCD_DisplayString(5, 0, 1, str_timer);
            GLCD_DisplayString(8, 0, 1, str_status);
        } else {
            sprintf(str_debug, "state:%2d event:%2d", current_state+1, new_event+1);
            GLCD_DisplayString(4, 0, 1, str_debug);
            GLCD_DisplayString(6, 0, 1, "Incorrect state");
        }
        
        __WFI();    // stay in low power mode until interrupt occurs
    }

    return 0;
}