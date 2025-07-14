/**
 * @file fsm_retina.c
 * @brief Retina FSM main file.
 * @author Alvaro Rodriguez Gabaldon
 * @author Miguel Lobo Benito
 * @date fecha
 */

/* Includes ------------------------------------------------------------------*/
#include "fsm_retina.h"
#include "fsm_button.h"
#include "fsm_tx.h"
#include "commands.h"
#include <stdio.h>
#include "fsm_rx.h"
#include "port_rgb.h"
#include "port_system.h"


/* Defines and enums ----------------------------------------------------------*/
/* Defines */
#define COMMANDS_MEMORY_SIZE 3 /*!< Number of NEC commands stored in the memory of the system Retina */

/* Enums */
enum
{
    WAIT_TX = 0, /*!< **Single state in Version 2**. State to wait in transmission mode */
    WAIT_RX,
    SLEEP_TX,
    SLEEP_RX,
};

/* Typedefs --------------------------------------------------------------------*/
/**
 * @brief Structure to define the Retina FSM.
 */
typedef struct
{
    fsm_t f; /*!< Retina FSM  */
    fsm_t *p_fsm_button; /*Pointer to the FSM of the user button*/
    uint32_t long_button_press_ms; /*Duration of the button press to change between transmitter and receiver modes*/
    fsm_t *p_fsm_tx; /*Pointer to the FSM of the infrared transmitter*/
    uint32_t tx_codes_arr[COMMANDS_MEMORY_SIZE]; /*Array to store in the memory of the system a number of codes to send in a loop*/
    uint8_t tx_codes_index; /*Index to go though the elements of the tx_codes_arr*/
    fsm_t *p_fsm_rx;
    uint32_t rx_code;
    uint8_t rgb_id;

} fsm_retina_t;

void _process_rgb_code(uint8_t rgb_id, uint32_t code){

    if(code == LIL_RED_BUTTON){
        port_rgb_set_color(rgb_id, HIGH, LOW, LOW);
    }

    else if(code == LIL_GREEN_BUTTON){
        port_rgb_set_color(rgb_id, LOW, HIGH, LOW);
    }

    else if(code == LIL_BLUE_BUTTON){
        port_rgb_set_color(rgb_id, LOW, LOW, HIGH);
    }

    else if(code == LIL_CYAN_BUTTON){
         port_rgb_set_color(rgb_id, LOW, HIGH, HIGH);
    }

     else if(code == LIL_MAGENTA_BUTTON){
         port_rgb_set_color(rgb_id, HIGH, LOW, HIGH);
    }

     else if(code == LIL_YELLOW_BUTTON){
         port_rgb_set_color(rgb_id, HIGH, HIGH, LOW);
    }

     else if(code == LIL_WHITE_BUTTON){
         port_rgb_set_color(rgb_id, HIGH, HIGH, HIGH);
    }

     else if(code == LIL_OFF_BUTTON){
         port_rgb_set_color(rgb_id, LOW, LOW, LOW);
    }
}	

/* State machine input or transition functions */

/*Check if the button has been pressed fast to send a new command.*/
static bool check_short_pressed	(fsm_t *p_this){

    fsm_retina_t *p_fsm = (fsm_retina_t *)(p_this);
    uint32_t duration = fsm_button_get_duration(p_fsm -> p_fsm_button);

   if((duration > 0)  &&  (duration < p_fsm->long_button_press_ms)){
        return true;
    }

    else{
        return false;
    }
}

static bool check_long_pressed(fsm_t *p_this){

    fsm_retina_t *p_fsm = (fsm_retina_t *)(p_this);
    uint32_t duration = fsm_button_get_duration(p_fsm -> p_fsm_button);

    if((duration != 0)  &&  (duration >= p_fsm->long_button_press_ms)){
        return true;
    }

    else{
        return false;
    }
}

static bool check_code(fsm_t *p_this){

    fsm_retina_t *p_fsm = (fsm_retina_t *)(p_this);

    if(fsm_rx_get_code(p_fsm->p_fsm_rx) != 0x00){
        p_fsm->rx_code = fsm_rx_get_code(p_fsm->p_fsm_rx);
        return true;
    }
    else{
        return false;
    }
}

static bool check_repetition(fsm_t *p_this){

    fsm_retina_t *p_fsm = (fsm_retina_t *)(p_this);
    return fsm_rx_get_repetition(p_fsm->p_fsm_rx);

}

static bool check_error(fsm_t *p_this){

    fsm_retina_t *p_fsm = (fsm_retina_t *)(p_this);
    return fsm_rx_get_error_code(p_fsm->p_fsm_rx);

}


static bool check_activity(fsm_t *p_this){

    fsm_retina_t *p_fsm = (fsm_retina_t *)(p_this);

    if(fsm_button_check_activity(p_fsm->p_fsm_button) == true || fsm_tx_check_activity(p_fsm->p_fsm_tx) == true || fsm_rx_check_activity(p_fsm->p_fsm_rx) == true){
        return true;
    }
    else{
        return false;
    }
}

static bool check_no_activity(fsm_t *p_this){

    return !check_activity(p_this);
}	




/* State machine output or action functions */

/*Transmit the next code stored in memory after a short button press.*/
static void do_send_next_msg (fsm_t *p_this){ 

    fsm_retina_t *p_fsm = (fsm_retina_t *)(p_this);

    fsm_tx_set_code(p_fsm->p_fsm_tx, p_fsm->tx_codes_arr[p_fsm->tx_codes_index]);
    fsm_button_reset_duration (p_fsm->p_fsm_button);

    printf("%ld\n",p_fsm->tx_codes_arr[p_fsm->tx_codes_index]); 

    if(p_fsm->tx_codes_index < 2){
    p_fsm->tx_codes_index ++;
    }

    else{
        p_fsm->tx_codes_index = 0;
    }
}

static void do_execute_code(fsm_t *p_this){

    fsm_retina_t *p_fsm = (fsm_retina_t *)(p_this);
    _process_rgb_code(p_fsm->rgb_id, p_fsm->rx_code);
    fsm_rx_reset_code(p_fsm->p_fsm_rx);
}


static void do_tx_off_rx_on(fsm_t *p_this){

    fsm_retina_t *p_fsm = (fsm_retina_t *)(p_this);
    fsm_rx_set_rx_status(p_fsm->p_fsm_rx, true);
    _process_rgb_code(p_fsm->rgb_id, p_fsm->rx_code);
    fsm_button_reset_duration(p_fsm->p_fsm_button);
}

static void do_rx_off_tx_on(fsm_t *p_this){

    fsm_retina_t *p_fsm = (fsm_retina_t *)(p_this);
    fsm_rx_set_rx_status(p_fsm->p_fsm_rx, false);
    port_rgb_set_color(p_fsm->rgb_id, 0, 0, 0);
    fsm_button_reset_duration(p_fsm->p_fsm_button);
}	

static void do_execute_repetition(fsm_t *p_this){

    fsm_retina_t *p_fsm = (fsm_retina_t *)(p_this);
    fsm_rx_reset_code(p_fsm->p_fsm_rx);
}

static void do_discard_rx_and_reset(fsm_t *p_this){

    fsm_retina_t *p_fsm = (fsm_retina_t *)(p_this);
    fsm_rx_reset_code(p_fsm->p_fsm_rx);

}	

static void do_sleep(fsm_t *p_this){

    port_system_sleep();
}	


/*Array representing the transitions table of the FSM Retina.*/
fsm_trans_t fsm_trans_retina[] = {

    {WAIT_TX, check_short_pressed, WAIT_TX, do_send_next_msg},
    {WAIT_TX, check_long_pressed, WAIT_RX, do_tx_off_rx_on},
    {WAIT_TX, check_no_activity, SLEEP_TX, do_sleep},
    {SLEEP_TX, check_no_activity, SLEEP_TX, do_sleep},
    {SLEEP_TX, check_activity, WAIT_TX, NULL},
    {WAIT_RX, check_code, WAIT_RX, do_execute_code},
    {WAIT_RX, check_repetition, WAIT_RX, do_execute_repetition},
    {WAIT_RX, check_error, WAIT_RX, do_discard_rx_and_reset},
    {WAIT_RX, check_long_pressed, WAIT_TX, do_rx_off_tx_on},
    {WAIT_RX, check_no_activity, SLEEP_RX, do_sleep},
    {SLEEP_RX, check_no_activity, SLEEP_RX, do_sleep},
    {SLEEP_RX, check_activity, WAIT_RX, NULL},
    { -1 , NULL , -1, NULL },
    
};


/* Other auxiliary functions */

/*Create a new RETINA FSM.

This FSM is the main state machine of the Retina system that governs the interaction between the other state machines of the system: button, transmitter and receiver FSM.*/
fsm_t *fsm_retina_new(fsm_t *p_fsm_button, uint32_t button_press_time, fsm_t *p_fsm_tx, fsm_t *p_fsm_rx, uint8_t rgb_id)
{
    fsm_t *p_fsm = malloc(sizeof(fsm_retina_t)); /* Do malloc to reserve memory of all other FSM elements, although it is interpreted as fsm_t (the first element of the structure) */
    fsm_retina_init(p_fsm, p_fsm_button, button_press_time, p_fsm_tx, p_fsm_rx, rgb_id);
    return p_fsm;
}

/*Initialize the infrared transmitter FSM.*/
void fsm_retina_init(fsm_t *p_this, fsm_t *p_fsm_button, uint32_t button_press_time, fsm_t *p_fsm_tx, fsm_t *p_fsm_rx, uint8_t rgb_id)
{
    fsm_retina_t *p_fsm = (fsm_retina_t *)(p_this);
    fsm_init(p_this, fsm_trans_retina);


    p_fsm->p_fsm_button = p_fsm_button;
    p_fsm->p_fsm_tx = p_fsm_tx;
    p_fsm->long_button_press_ms = button_press_time;
    p_fsm->tx_codes_index = 0;
    p_fsm->tx_codes_arr[0] = LIL_RED_BUTTON;
    p_fsm->tx_codes_arr[1] = LIL_GREEN_BUTTON;
    p_fsm->tx_codes_arr[2] = LIL_BLUE_BUTTON;

    p_fsm->p_fsm_rx = p_fsm_rx;
    p_fsm->rx_code = 0x00;
    p_fsm->rgb_id = rgb_id;
    port_rgb_init(rgb_id);
}

