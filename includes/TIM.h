#ifndef TIM_H  //If GPIO_H is not defined
#define TIM_H  // define
/**
 * @file TIM.h
 * @brief Module that generates delays using hardware timers.
 * 
 * The user may use this module with multiple functions to:
 * Initialize a STM32 timer and generate delays
 *
 * @authors David Mijares, Ximena Cedillo, Xavier Clemente
 *
 */


#include <stdint.h>
#include "stm32f4xx.h" 

#define TIMER_CLOCK_HZ 16000000 // 16 MHz
//#define PSC 16000               // 16 kHz

typedef enum tim
{
    TIM_1 = 0,
    TIM_2 = 1,
    TIM_3 = 2,
    TIM_4 = 3,
    TIM_5 = 4,
    TIM_9 = 5,
    TIM_10 = 6,
    TIM_11 = 7,
    TIM_SIZE = 8  
} tim_t;

extern TIM_TypeDef* TIM[TIM_SIZE];


#endif