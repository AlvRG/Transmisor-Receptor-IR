/**
 * @file port_tx.h
 * @brief Header for port_tx.c file.
 * @author Alvaro Rodriguez Gabaldon
 * @author Miguel Lobo Benito
 * @date fecha
 */

#ifndef PORT_TX_H_
#define PORT_TX_H_


/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdint.h>
#include <stdbool.h>

/* HW dependent includes */

/* Defines and enums ----------------------------------------------------------*/
/* Defines */
#define IR_TX_0_ID 0 /*Infrared transmitter identifier*/
#define IR_TX_0_GPIO GPIOB /*Infrared transmitter GPIO port*/
#define IR_TX_0_PIN 10 /*Infrared transmitter GPIO pin*/

/* Function prototypes and explanation -------------------------------------------------*/

/*Configure the HW specifications of a given infrared transmitter.*/
void port_tx_init (uint8_t tx_id, bool status);
/*Set the PWM ON or OFF.*/
void port_tx_pwm_timer_set (uint8_t tx_id, bool status);
/*Start the symbol timer and reset the count of ticks.*/
void port_tx_symbol_tmr_start ();
/*Stop the symbol timer.*/
void port_tx_symbol_tmr_stop ();
/*Get the count of the symbol ticks.*/
uint32_t port_tx_tmr_get_tick ();

#endif