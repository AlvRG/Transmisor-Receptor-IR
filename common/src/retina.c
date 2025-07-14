/**
 * @file retina.c
 * @brief Main file.
 * @author Sistemas Digitales II
 * @date 2023-01-01
 */

/* Includes ------------------------------------------------------------------*/
#include "retina.h"
#include "fsm_button.h"
#include "port_button.h"
#include "fsm_retina.h"
#include "fsm_tx.h"
#include "port_tx.h"
#include "fsm_rx.h"
#include "port_rx.h"
#include "port_rgb.h"

/* Defines */
#define LD2_PORT GPIOA
#define LD2_PIN 5
#define CHANGE_MODE_BUTTON_TIME 3000

/* Variable initialization functions */

/* State machine input or transition functions */

/* State machine output or action functions */

/* Other auxiliary functions */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{

    port_system_init();

    fsm_t *p_fsm_user_button = fsm_button_new(BUTTON_0_DEBOUNCE_TIME_MS, BUTTON_0_ID);

    fsm_t *p_fsm_tx = fsm_tx_new(IR_TX_0_ID);

    fsm_t *p_fsm_rx = fsm_rx_new(IR_RX_0_ID);

    fsm_t *p_fsm_retina = fsm_retina_new(p_fsm_user_button, CHANGE_MODE_BUTTON_TIME, p_fsm_tx, p_fsm_rx, RGB_0_ID);

  /*  #if VERSION == VERSION_1
    port_system_gpio_config(LD2_PORT, LD2_PIN, GPIO_MODE_OUT, GPIO_PUPDR_NOPULL);
    #endif  */


    


    /* Infinite loop */
    while (1)
    {

       fsm_fire(p_fsm_user_button);
       fsm_fire(p_fsm_tx);
        fsm_fire(p_fsm_rx);
       fsm_fire(p_fsm_retina);
       

/*#if VERSION == VERSION_1
        uint32_t but_press_duration = fsm_button_get_duration(p_fsm_user_button);
        if (but_press_duration){
            if(but_press_duration >= 3000){
                port_system_gpio_toggle(LD2_PORT, LD2_PIN);
            }
            fsm_button_reset_duration(p_fsm_user_button);
        }
        #endif */
    }

    fsm_destroy(p_fsm_user_button);
    fsm_destroy(p_fsm_tx);
    fsm_destroy(p_fsm_rx);
    fsm_destroy(p_fsm_retina); 
   
}
