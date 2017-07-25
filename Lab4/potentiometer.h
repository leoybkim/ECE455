#ifndef __POT_H_
#define __POT_H_

void init_potentiometer (void);
uint32_t read_potentiometer (void);
void enable_irq_potentiometer (void);
void potentiometer_conversion (void);

#endif
