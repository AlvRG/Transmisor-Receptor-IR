/**
 * @file port_rx.c
 * @brief Portable functions to interact with the infrared receiver FSM library.
 * @author alumno1
 * @author alumno2
 * @date fecha
 * */

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <string.h> /* To use memset */

/* Other includes */
#include "port_rx.h"
#include "port_system.h"
#include "fsm_rx_nec.h"

/* Typedefs --------------------------------------------------------------------*/
/**
 * @brief Structure to define the HW dependencies of an infrared receiver. 
 */
typedef struct
{
GPIO_TypeDef *p_port;
uint8_t pin;
uint16_t edge_ticks[NEC_FRAME_EDGES];
uint16_t edge_idx;
} port_rx_hw_t;

/* Global variables ------------------------------------------------------------*/
/**
 * @brief Array of elements that represents the HW characteristics of the infrared receivers.
 */
static port_rx_hw_t receivers_arr[] = {
    [IR_RX_0_ID] = {.p_port = IR_RX_0_GPIO, .pin = IR_RX_0_PIN},
};

/* Infrared receiver private functions */
/**
 * @brief Set the elements of the array of time ticks to '0' and init the index to '0' as well.
 *
 * > &nbsp;&nbsp;&nbsp;&nbsp;💡 To set all the elements of an array to one value, you can use function `memset`. You need a pointer to the array (its name), the value you want to set, and the length (in bytes) of the array (so you need to multiply the number of elements by the size of the type of the elements of the array). To use this function you need to include the <string.h> library.
 *
 * @param rx_id Receiver ID. This index is used to select the element of the `receivers_arr[]` array.
 */
static void _reset_edge_ticks_idx(uint8_t rx_id)
{
  memset(receivers_arr[rx_id].edge_ticks, 0, sizeof(uint16_t) * NEC_FRAME_EDGES);
  receivers_arr[rx_id].edge_idx = 0;
}

static void _store_edge_tick(uint8_t rx_id)
{
  uint16_t value = receivers_arr[rx_id].edge_idx;
  
  if((port_system_gpio_read(receivers_arr[rx_id].p_port, receivers_arr[rx_id].pin) == false && value%2 == 0) || (port_system_gpio_read(receivers_arr[rx_id].p_port, receivers_arr[rx_id].pin) == true && value%2 != 0)){

     if(receivers_arr[rx_id].edge_idx < NEC_FRAME_EDGES){

     receivers_arr[rx_id].edge_ticks[receivers_arr[rx_id].edge_idx] = TIM3->CNT;
     receivers_arr[rx_id].edge_idx++;
    }  
  }
}

void _timer_rx_setup()
{
  RCC -> APB1ENR |= RCC_APB1ENR_TIM3EN;
  TIM3 -> CNT = 0;
  TIM3 -> ARR = 65535;
  TIM3 -> PSC = 159; /*(SystemCoreClock * NEC_RX_TIMER_TICK_BASE_US * pow(10, -6)) - 1;*/
  TIM3 -> EGR = TIM_EGR_UG;
}

void port_rx_init(uint8_t rx_id)
{
  _timer_rx_setup();
  port_system_gpio_config(receivers_arr[rx_id].p_port, receivers_arr[rx_id].pin, GPIO_MODE_IN, GPIO_PUPDR_NOPULL);
  port_system_gpio_config_exti(receivers_arr[rx_id].p_port, receivers_arr[rx_id].pin, TRIGGER_BOTH_EDGE);
  port_system_gpio_config_exti(receivers_arr[rx_id].p_port, receivers_arr[rx_id].pin, TRIGGER_ENABLE_INTERR_REQ);
  port_system_gpio_exti_enable(receivers_arr[rx_id].pin, 2, 0);
  _reset_edge_ticks_idx(rx_id);
}

void port_rx_en(uint8_t rx_id, bool interr_en)
{
  _reset_edge_ticks_idx(rx_id);
  if(interr_en == true){
    port_system_gpio_exti_enable(receivers_arr[rx_id].pin, 2, 0);
  }
  else{
    port_system_gpio_exti_disable(receivers_arr[rx_id].pin);
  }
}

void port_rx_tmr_start()
{
  TIM3 -> CNT = 0;
  TIM3 -> CR1 |= TIM_CR1_CEN;
}

void port_rx_tmr_stop()
{
  TIM3 -> CR1 &= ~TIM_CR1_CEN;
}

uint32_t port_rx_get_num_edges(uint8_t rx_id)
{
  return receivers_arr[rx_id].edge_idx;
}

uint16_t *port_rx_get_buffer_edges(uint8_t rx_id)
{
  return (uint16_t *)(&(receivers_arr[rx_id].edge_ticks));
}

void port_rx_clean_buffer(uint8_t rx_id)
{
  _reset_edge_ticks_idx(rx_id);
}

void EXTI9_5_IRQHandler(void)
{
  port_system_systick_resume();
  if (EXTI->PR & BIT_POS_TO_MASK(receivers_arr[IR_RX_0_ID].pin))
  {
    EXTI -> PR |= BIT_POS_TO_MASK(receivers_arr[IR_RX_0_ID].pin);
    _store_edge_tick(IR_RX_0_ID);
  }
}
