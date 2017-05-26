#include <lpc17xx.h> 
#include "glcd.h"

typedef int bool;
#define TRUE 1
#define FALSE 0

// Global variable
const led_pos[8] = { 28, 29, 31, 2, 3, 4, 5, 6 };
int led = 7;
bool init = TRUE;
bool off = TRUE;

// thermo inputs
const int measured_temp;
const int desired_temp;
int timer;

/* STATE MACHINE */
enum states { STATE_OFF, STATE_INTER, STATE_ON, STATE_ERR, MAX_STATES } current_state;
enum events { HEAT_REQUEST, HEAT, COOL, TIME_OUT, MAX_EVENTS } new_events;

void action_s1_e1 (void);
void action_s1_e2 (void);
void action_s1_e3 (void);
void action_s1_e4 (void);
void action_s2_e1 (void);
void action_s2_e2 (void);
void action_s2_e3 (void);
void action_s2_e4 (void);
void action_s3_e1 (void);
void action_s3_e2 (void);
void action_s3_e3 (void);
void action_s3_e4 (void);
void action_s4_e1 (void);
void action_s4_e2 (void);
void action_s4_e3 (void);
void action_s4_e4 (void);
enum events get_new_events (void);

void (*const state_transitions [MAX_STATES][MAX_EVENTS]) (void) = {
    { action_s1_e1, action_s1_e2, action_s1_e3, action_s1_e4 },
    { action_s2_e1, action_s2_e2, action_s2_e3, action_s2_e4 },
    { action_s3_e1, action_s3_e2, action_s3_e3, action_s3_e4 },
    { action_s4_e1, action_s4_e2, action_s4_e3, action_s4_e4 },
}

void input_init(void) 
{
    // cofig INT0 button for input
    LPC_PINCON->PINSEL4 &= ~(3<<20); // P2.10 is GPIO
    LPC_GPIO2->FIODIR &= ~(1<<10);   // P2.10 is input
}

void joystick_init(void)
{
    // configure joystick
    // this step is actually not required normally since they are by default are set to GPIO
    LPC_PINCON->PINSEL3 &= ~((3<< 8) | (3<<14) | (3<<16) | (3<<18) | (3<<20));     /* P1.20, P1.23..26 is GPIO (Joystick) */

    // set the I/O direction
    // There are five LPC_GPIOx, where x=0,1,2,3,4 
    // To set pin as input, set the corre
    sponding bit in FIODIR to 0, for output, set FIODIR to 1
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
    // debounce: read the value 3 times in a row
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
    LPC_TIM1->MR0 = 100000000;     // match value (can be anything)
    LPC_TIM1->MCR |= 0x03;        // on match, generate interrupt and reset
    NVIC_EnableIRQ(TIMER1_IRQn);  // allow interrupts from the timer
}


// Turns LED on and off to simulate the furnace
void check_furnace(int desired_temp, int current_temp, int led) 
{
    int mask = 1 << led_pos[led];

    // hysteresis range +/- 2 degrees
    if (led < 3) {
        if (desired_temp > current_temp + 2) {
            LPC_GPIO1->FIOSET = mask;
            timer1_init();
        } else if (desired_temp < current_temp - 2) {
            LPC_GPIO1->FIOCLR = mask;
        }
    } else {
        if (desired_temp > current_temp + 2) {
            LPC_GPIO2->FIOSET = mask;
            timer1_init();
        } else if (desired_temp < current_temp - 2) {
            LPC_GPIO2->FIOCLR = mask;
        }
    } 
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
    int mask = 1 << led_pos[led];
    if((LPC_TIM1->IR & 0x01) == 0x01) // if MR0 interrupt
    {
        LPC_TIM1->IR |= 1 << 0; 
        
        // Time out (max furnace time)
        LPC_GPIO2->FIOCLR = mask;
    }
}

enum events get_new_events(int desired_temp, int measured_temp) 
{
    if (desired_temp > measured_temp + 2) {
        event = HEAT;
    } else if (desired_temp < measured_temp - 2) {
        event = COOL;
    } 

    return event;
}


// ACTIONS

void action_s1_e1 (void)
{
    /* PREV STATE: OFF
       EVENT     : REQUEST HEAT */

    current_state = STATE_ON; 
}

void action_s1_e2 (void) {/* */}
void action_s1_e3 (void) {/* */}
void action_s1_e4 (void) {/* */}
void action_s2_e1 (void) {/* */}
void action_s2_e2 (void) {/* */}
void action_s2_e3 (void) {/* */}
void action_s2_e4 (void) {/* */}
void action_s3_e1 (void) {/* */}
void action_s3_e2 (void) {/* */}
void action_s3_e3 (void) {/* */}
void action_s3_e4 (void) {/* */}
void action_s4_e1 (void) {/* */}
void action_s4_e2 (void) {/* */}
void action_s4_e3 (void) {/* */}
void action_s4_e4 (void) {/* */}


// make a generic state machine
// For state: make a table (one axis is previous state, the other axis is the input) and the cell would tell you which state to go to (it could stay in the same state) and only thne
// you would use some sort of an swich case to define an action
// this is so that the user can define what the states are going to be and what actions are going to be 

int main(void)
{
    int joystick_val = 0;
    int int_temperature = 25;
    int ADC_Value = 0;
    unsigned int aDCStat;
    char str_temperature[10];    
    char str_potentiometer[10];
    int i; // for sleep counter

    int mask = 1 << led_pos[led];

    sprintf(str_temperature, "desired temp: %d", int_temperature);

    SystemInit();   
    GLCD_Init();    
    GLCD_Clear(White);  
    GLCD_DisplayString(0, 0, 1, str_temperature); // line 0, row 0, font 1 (larger of the two available fonts)
      
    joystick_init();                              // desired temperature
    led_init();                                   // furnace indicator (P2.6)
    potentiometer_init();                         // measured temperature
    timer0_init();                                // setup timer
    
    // start program
    for(;;)
    {
        //joystick_val = read_joystick();
        joystick_val = debounce();
        
        /* polling potentiometer*/
        LPC_ADC->ADCR |= ( 1 << 24 );            // Read the converted value
        // start Conversion
        while (LPC_ADC->ADGDR & 0x8000 == 0);
        ADC_Value = (LPC_ADC->ADGDR>>4) & 0xFFF / 80;
        sprintf(str_potentiometer, "measured temp: %4d", ADC_Value);
        GLCD_DisplayString(1, 0, 1, str_potentiometer);
        
        // read joystick 
        if (joystick_val == 0x08) {
            int_temperature += 1;
        } else if (joystick_val == 0x20) {
            int_temperature -= 1;
        }

        // determine what event happened
        new_ event = get_new_events(int_temperature, ADC_Value);
        if (((new_event >= 0) && (new_event < MAX_EVENTS))
        && ((current_state >= 0) && (current_state < MAX_STATES))) {
            /* call the action procedure */
            state_transitions [current_state][new_event] (); 
        } else {
            /* invalid event/state - handle appropriately */
        }


        // if (joystick_val == 0x08) {
        //     int_temperature += 1;
        //     sprintf(str_temperature, "desired temp: %d", int_temperature);
        //     GLCD_DisplayString(0, 0, 1, str_temperature);
        //     check_furnace(int_temperature, ADC_Value, led);
            
        // } else if (joystick_val == 0x20) {
        //     int_temperature -= 1;
        //     sprintf(str_temperature, "desired temp: %d", int_temperature);
        //     GLCD_DisplayString(0, 0, 1, str_temperature);
        //     check_furnace(int_temperature, ADC_Value, led);
        // } 
        
        //for (i=0; i < 1000000; i++) {}  // counter that acts like sleep()
        __WFI();                          // low power mode until interrupt occurs
    }
    return 0;
}