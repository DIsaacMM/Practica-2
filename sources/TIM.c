/**
 * @file TIM.c
 * @brief Module that generates delays using hardware timers.
 * 
 * The user may use this module with multiple functions to:
 * Initialize a STM32 timer and generate delays
 *
 * @authors David Mijares, Ximena Cedillo, Xavier Clemente
 *
 */

#include "TIM.h"



TIM_TypeDef* TIM[TIM_SIZE]; 

void tim_init()
{
    TIM[0] = TIM1; 
    TIM[1] = TIM2; 
    TIM[2] = TIM3; 
    TIM[3] = TIM4; 
    TIM[4] = TIM5; 
    TIM[5] = TIM9; 
    TIM[6] = TIM10; 
    TIM[7] = TIM11; 

    
}

void tim_initTimer(tim_t t)
{
    if (t < 0 || t > 7)
    {
        return; 
    }
    else if(t < 5)
    {
        tim_init(); 
        if(t == 0) // TIM1
        {
            RCC -> APB2ENR |= (1U << t); 
        }
        else // TIM2 - TIM5
        {
            RCC -> APB1ENR |= (1U << (t-1)); 
        }
    }
    else // TIM9 - TIm11
    {        
        tim_init(); 
        RCC -> APB2ENR |= (1U << (t+7));
    }
        
}

/*
TIM1 = (RCC -> APB2ENR) bit 0
TIM_2 = (RCC -> APB1ENR) bit 0
TIM_3 = (RCC -> APB1ENR) bit 1
TIM_4 = (RCC -> APB1ENR) bit 2
TIM_5 = (RCC -> APB1ENR) bit 3
TIM_9 = RCC -> APB2ENR bit 16
TIM_10 = RCC -> APB2ENR bit 17
TIM_11 = RCC -> APB2ENR bit 18
*/




void tim_setTimerMs(tim_t t, uint32_t time_ms)
{
    if (t < 0 || t > 7 || time_ms == 0)
    {
        return;
    }
    
    tim_initTimer(t);
    TIM[t]->CR1 &= ~(1U << 0);
    
    uint16_t psc;
    uint16_t arr;
    
    // Usar PSC fijo para resolución de 1ms
    psc = 15999;  // 16000 - 1 (cada tick = 1ms)
    arr = (uint16_t)time_ms - 1;
    
    // Limitar valores
    if (arr > 65535)
    {
        arr = 65535;
    } 
    if (arr < 1) 
    {
        arr = 1;
    }
    
    TIM[t]->PSC = psc;
    TIM[t]->ARR = arr;
    TIM[t]->EGR |= (1U << 0);
    TIM[t]->SR &= ~(1U << 0);
}


void tim_setTimerFreq(tim_t t, uint32_t freq_hz)
{
    if (t < 0 || t > 7 || freq_hz == 0 || freq_hz > TIMER_CLOCK_HZ)
    {
        return;
    }
    
    tim_initTimer(t);
    TIM[t]->CR1 &= ~(1U << 0);
    
    uint16_t psc;
    uint16_t arr;
    
    // Usar PSC fijo según el rango de frecuencia
    if (freq_hz >= 1000)  // Frecuencias altas
    {
        psc = 0;
        arr = (uint16_t)(TIMER_CLOCK_HZ / freq_hz) - 1;
    }
    else  // Frecuencias bajas
    {
        psc = 15999;  // 1ms de resolución
        arr = (uint16_t)((TIMER_CLOCK_HZ / (psc + 1)) / freq_hz) - 1;
    }
    
    if (arr > 65535) arr = 65535;
    if (arr < 1) arr = 1;
    
    TIM[t]->PSC = psc;
    TIM[t]->ARR = arr;
    TIM[t]->EGR |= (1U << 0);
    TIM[t]->SR &= ~(1U << 0);
}

void tim_enableTimer(tim_t t)
{
    if (t < 0 || t > 7)
    {
        return; 
    }
    TIM[t]->CR1 |= (1U << 0);  
}


void tim_disableTimer(tim_t t)
{
    if (t < 0 || t > 7)
    {
        return; 
    }
    TIM[t]->CR1 &= ~(1U << 0);  
}


void tim_waitTimer(tim_t t)
{
    if (t < 0 || t > 7)
    {
        return; 
    }
    while (!(TIM[t]->SR & (1<<0)))
    {
        __NOP(); 
    }
    TIM[t]->SR &= ~(1U << 0); 
}

/*
    Modificar el registro CCRx 

*/
void tim_setTimerCompareChannelValue(tim_t t, uint8_t channel, uint16_t compare_value)
{
    if (t < 0 || t > 7)
    {
        return; 
    }
    if (channel < 1 || channel > 4)
    {
        return;
    }

    switch (channel)
    {
    case 1:
        TIM[t]->CCR1 = compare_value; 
        break;
    
    case 2:
        TIM[t]->CCR2 = compare_value; 
        break;

    case 3:
        TIM[t]->CCR3 = compare_value; 
        break;
    
    case 4:
        TIM[t]->CCR4 = compare_value; 
        break;
    }
}

void tim_setTimerCompareMode(tim_t t, uint8_t channel, uint8_t mode)
{
    if (t < 0 || t > 7)
    {
        return; 
    }
    else if (mode > 7)
    {
        return; 
    }
    if(channel == 1 || channel == 2)
    {
         
        channel = (channel - 1)*8;

        // Limpiar los bits OCxM (3 bits)
        TIM[t]->CCMR1 &= ~(7 << (channel + 4)); 
        
        // Configurar el nuevo modo
        TIM[t]->CCMR1 |= (mode << (channel + 4)); 
        
        // Configurar como modo comparación (output)
        TIM[t]->CCMR1 &= ~(3 << channel);  
    }
    else if(channel == 3 || channel == 4)
    {
        channel = (channel - 3)*8;
        // Limpiar los bits OCxM (3 bits)
        TIM[t]->CCMR2 &= ~(7 << (channel + 4));
        
        // Configurar el nuevo modo
        TIM[t]->CCMR2 |= (mode << (channel + 4));
        
        // Configurar como modo comparación (output)
        TIM[t]->CCMR2 &= ~(3 << channel);  
    }
    else
    {
        return; 
    }
}

void tim_enableTimerCompareChannel(tim_t t, uint8_t channel)
{
    if (t < 0 || t > 7)
    {
        return; 
    }
    if (channel < 1 || channel > 4)
    {
        return;
    }

    channel = (channel-1)*4; 

    TIM[t]->CCER |= (1 << channel);

}

void tim_disableTimerCompareChannel(tim_t t, uint8_t channel)
{
    if (t < 0 || t > 7)
    {
        return; 
    }
    if (channel < 1 || channel > 4)
    {
        return;
    }

    channel = (channel-1)*4; 

    TIM[t]->CCER &= ~(1 << channel);

}