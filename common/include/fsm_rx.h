/**
 * @file fsm_rx.h
 * @brief Header for fsm_rx.c file.
 * @author alumno1
 * @author alumno2
 * @date fecha
 */

#ifndef FSM_RX_H_
#define FSM_RX_H_

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdint.h>
#include <stdbool.h>

/* Other includes */
#include "fsm.h"

/* Function prototypes and explanation ----------------------------------------*/
/**
 * @brief Create a new infrared receiver FSM
 *
 * The infrared reception module indeed manages 2 FSMs. (i) The first one (`fsm_trans_rx_nec`) controls the reception of infrared pulses and stores the times where the changes on the GPIO occur. (ii) The second one (`fsm_trans_rx_nec`) parses the data received (an array of timestamps) to extract the NEC command. This second FSM that decodes the NEC protocol. Refer to `fsm_rx_NEC_new()` for further information about this FSM.
 *
 * At start and reset, the code value must be '0x00'. A value of '0x00' means that it has not been received any new code. The Retina FSM is the one which stores and retains the last code until a new one is received.
 *
 * @attention The user is required to reset the code value once it has been read. Otherwise, this value may be misinterpreted by the Retina FSM. In such a case we would be interpreting the code constantly. The function `fsm_rx_reset_code()` resets the code when its called.
 *
 * The FSM contains information of the receiver ID. This ID is a unique identifier that is managed by the user in the `port`. That is where the user provides identifiers and HW information for all the receivers on his system. The FSM does not have to know anything of the underlying HW.
 *
 * @param rx_id Unique infrared receiver identifier number
 *
 * @return A pointer to the infrared receiver FSM
 */
fsm_t *fsm_rx_new(uint8_t rx_id);
void fsm_rx_init(fsm_t *p_this, uint8_t rx_id);
uint32_t fsm_rx_get_code(fsm_t *p_this);
bool fsm_rx_get_repetition(fsm_t *p_this);
bool fsm_rx_get_error_code(fsm_t *p_this);
void fsm_rx_set_rx_status(fsm_t *p_this, bool status);
void fsm_rx_reset_code(fsm_t *p_this);
bool fsm_rx_check_activity(fsm_t *p_this);

#endif
