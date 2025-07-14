/**
 * @file fsm_button.h
 * @brief Header for fsm_button.c file.
 * @author Alvaro Rodriguez Gabaldon
 * @author Miguel Lobo Benito
 * @date fecha
 */

#ifndef FSM_BUTTON_H_
#define FSM_BUTTON_H_

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdint.h>
#include <stdbool.h>

/* Other includes */
#include "fsm.h"

/* Function prototypes and explanation -------------------------------------------------*/

/*	Return the duration of the last button press.*/
uint32_t fsm_button_get_duration(fsm_t* p_this);

/*Initialize a button FSM.*/
void fsm_button_init(fsm_t* p_this, uint32_t debounce_time, uint32_t button_id);

/*Create a new button FSM. */
fsm_t* fsm_button_new(uint32_t debounce_time, uint32_t button_id); 

/*	Return the duration of the last button press.*/
void fsm_button_reset_duration(fsm_t* p_this);

bool fsm_button_check_activity(fsm_t *p_this);	


#endif
