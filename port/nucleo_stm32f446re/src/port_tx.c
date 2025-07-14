/**
 * @file port_tx.c
 * @brief Portable functions to interact with the infrared transmitter FSM library.
 * @author Alvaro Rodriguez Gabaldon
 * @author Miguel Lobo Benito
 * @date fecha
 */

/* Includes ------------------------------------------------------------------*/
#include "port_tx.h"
#include "fsm_tx.h"
#include "port_system.h"

/* Defines --------------------------------------------------------------------*/
#define ALT_FUNC1_TIM2  0x01U /*!< TIM2 Alternate Function mapping */ 

/* IMPORTANT
The timer symbol is the same for all the TX, so it is not in the structure of TX. It has been decided to be the TIM1. It is like a systick but faster.
*/

/* Typedefs --------------------------------------------------------------------*/
typedef struct 
{
    GPIO_TypeDef *p_port; /*GPIO where the infrared transmitter is connected*/
    uint8_t pin; /*Pin/line where the infrared transmitter is connected*/
    uint8_t alt_func; /*Alternate function value according to the Alternate function table of the datasheet*/
}port_tx_hw_t;

/* Global variables ------------------------------------------------------------*/
static volatile uint32_t symbol_tick; /*Variable to store the count of ticks of the symbol timer*/
static port_tx_hw_t transmitters_arr[] = { /*Array of elements that represents the HW characteristics of the infrared transmitters.*/
     [IR_TX_0_ID] = {.p_port = IR_TX_0_GPIO, .pin = IR_TX_0_PIN, .alt_func = ALT_FUNC1_TIM2},
};
/* Infrared transmitter private functions */

/*Configure the symbol timer. This timer sets the tick base as a reference for the symbols of the protocol.*/
static void _timer_symbol_setup()
{

  RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
  TIM1 -> CNT = 0;
  TIM1 -> ARR = 899;   /*  (16*NEC_TX_TIMER_TICK_BASE_US) - 1  */
  TIM1 -> PSC = 0;
  TIM1 -> EGR = TIM_EGR_UG;
  TIM1 -> SR &= ~TIM_SR_UIF;
  TIM1 -> DIER |= TIM_DIER_UIE ;

  /* Finally, set the priority and enable interruptions globally. */
  NVIC_SetPriority(TIM1_UP_TIM10_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 1, 0)); /* Priority 1, sub-priority 0 */
  NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);                                                          /* Enable interrupt */
}

/*Configure the PWM timer. This timer configures the PWM for the NEC protocol.*/
static void _timer_pwm_setup(uint32_t tx_id)
{

  if(tx_id == IR_TX_0_ID){

   RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

   TIM2 -> CNT = 0;
   TIM2 -> ARR = 420;  /*    (16000000 Hz/NEC_PWM_FREQ_HZ)-1     */
   TIM2 -> PSC = 0;
   TIM2 -> EGR = TIM_EGR_UG;
   TIM2 -> CCER &= ~TIM_CCER_CC3E;
   TIM2 -> CCMR2 |= 0x0060;
   TIM2 -> CCMR2 |= TIM_CCMR2_OC3PE ;
   TIM2 -> CCR3 = 147; /*  (ARR + 1)*NEC_PWM_DC    */
  } 

}

/* Public functions */

/*	Configure the HW specifications of a given infrared transmitter. */
void port_tx_init(uint8_t tx_id, bool status)
{
  port_system_gpio_config (transmitters_arr[tx_id].p_port, transmitters_arr[tx_id].pin, GPIO_MODE_ALTERNATE, GPIO_PUPDR_NOPULL);
  port_system_gpio_config_alternate(transmitters_arr[tx_id].p_port, transmitters_arr[tx_id].pin, transmitters_arr[tx_id].alt_func);
  _timer_symbol_setup();
  _timer_pwm_setup(tx_id);
  port_tx_pwm_timer_set(tx_id, status);	
}

/* In order to make the academic effort of configuring the PWM, the values: timer, channel and masks are hardcoded and this function is not generic for any timer/channel. It is not the best way, but it is as it. */

/*	Set the PWM ON or OFF*/
void port_tx_pwm_timer_set(uint8_t tx_id, bool status)
{
 if(tx_id == IR_TX_0_ID){
  if(status == true){
    TIM2 -> CCER |= TIM_CCER_CC3E;
    TIM2 -> CR1 |= TIM_CR1_CEN;
  }

  else{
    TIM2 -> CCER &= ~TIM_CCER_CC3E;
    TIM2 -> CR1 &= ~TIM_CR1_CEN;
  }
  }
}

/*	Start the symbol timer and reset the count of ticks.*/
void port_tx_symbol_tmr_start()
{
  TIM1 -> CNT = 0;
  TIM1 -> CR1 |= TIM_CR1_CEN;
  symbol_tick = 0;
}

/*Stop the symbol timer.*/
void port_tx_symbol_tmr_stop()
{
  TIM1 -> CR1 &= ~TIM_CR1_CEN;
}

/*	Get the count of the symbol ticks.*/
uint32_t port_tx_tmr_get_tick()
{
  return symbol_tick;
}

//------------------------------------------------------
// INTERRUPT SERVICE ROUTINES
//------------------------------------------------------

/*	This function handles TIM1-TIM10 global interrupts.*/
void TIM1_UP_TIM10_IRQHandler(void)
{
 TIM1 -> SR &= ~TIM_SR_UIF;
  symbol_tick ++;
}