/**
 * @file port_button.h
 * @brief Header for port_button.c file.
 * @author Alvaro Rodriguez Gabaldon
 * @author Miguel Lobo Benito
 * @date fecha
 */

#ifndef PORT_BUTTON_H_
#define PORT_BUTTON_H_

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdint.h>
#include <stdbool.h>
#include "port_system.h"

/* HW dependent includes */


/* Defines and enums ----------------------------------------------------------*/
/* Defines */
#define BUTTON_0_ID 0 /*Button identifier*/
#define BUTTON_0_GPIO GPIOC /*Button GPIO port*/
#define BUTTON_0_PIN 13 /*Button GPIO pin*/
#define BUTTON_0_DEBOUNCE_TIME_MS 150 /*Button debounce time*/

/* Function prototypes and explanation -------------------------------------------------*/

void port_button_init (uint32_t button_id); /*Configure the HW specifications of a given button.*/
bool port_button_is_pressed (uint32_t button_id); /*Return the status of the button (pressed or not)*/
uint32_t port_button_get_tick(); /*Return the count of the System tick in milliseconds.*/
#endif
