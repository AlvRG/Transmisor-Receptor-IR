/**
 * @file fsm_tx.h
 * @brief Header for fsm_tx.c file.
 * @author Alvaro Rodriguez Gabaldon
 * @author Miguel Lobo Benito
 * @date fecha
 */

#ifndef FSM_TX_H_
#define FSM_TX_H_

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdint.h>
#include <stdbool.h>

/* Other includes */
#include "fsm.h"

/* Defines and enums ----------------------------------------------------------*/
/* Defines */
/* NEC transmission macros */
#define NEC_TX_TIMER_TICK_BASE_US 56.25 /*!< Time base in microseconds to create the ticks for the timer of symbols */
#define NEC_TX_PROLOGUE_TICKS_ON    160    /*!< Number of time base ticks for prologue ON in transmission  */
#define NEC_TX_PROLOGUE_TICKS_OFF  80     /*!< Number of time base ticks for prologue OFF in transmission  */
#define NEC_TX_SYM_0_TICKS_ON       10    /*!< Number of time base ticks for symbol 0 ON in transmission  */
#define NEC_TX_SYM_0_TICKS_OFF     10     /*!< Number of time base ticks for symbol 0 OFF in transmission  */
#define NEC_TX_SYM_1_TICKS_ON       10    /*!< Number of time base ticks for symbol 1 ON in transmission  */
#define NEC_TX_SYM_1_TICKS_OFF      30    /*!< Number of time base ticks for symbol 1 OFF in transmission  */
#define NEC_TX_EPILOGUE_TICKS_ON     10   /*!< Number of time base ticks for epilogue ON in transmission  */
#define NEC_TX_EPILOGUE_TICKS_OFF 3560  /*!< Number of time base ticks for epilogue OFF in transmission ~200 miliseconds */
#define NEC_PWM_FREQ_HZ        38000         /*!< PWM timer frequency in Hz */
#define NEC_PWM_DC         0.35             /*!< PWM duty cycle 0-1  */

/* Function prototypes and explanation ----------------------------------------*/

/*Create a new infrared transmitter FSM.*/
fsm_t * fsm_tx_new (uint8_t tx_id);

/*	Initialize an infrared transmitter FSM. */
void fsm_tx_init (fsm_t *p_this, uint8_t tx_id);

/*	Set the code given*/
void fsm_tx_set_code (fsm_t *p_this, uint32_t code);

/*	Start the process to transmit the code stored*/
void fsm_send_NEC_code (uint8_t tx_id, uint32_t code);

/*Check if the transmitter FSM is active, or not. As the system is the one controlling the output (PWM and symbol timer), this FSM will always be inactive and will wake up at each interruption of the symbol timer or PWM timer*/
bool fsm_tx_check_activity (fsm_t *p_this);

#endif
