/**
 * @file fsm_button.c
 * @brief Button FSM main file.
 * @author Alvaro Rodriguez Gabaldon
 * @author Miguel Lobo Benito
 * @date fecha
 */

/* Includes ------------------------------------------------------------------*/
#include "fsm_button.h"
#include "port_button.h"

/* Typedefs --------------------------------------------------------------------*/
typedef struct
{
    fsm_t f; /*Button FSM*/
    uint32_t debounce_time; /*Button debounce time in ms*/
    uint32_t next_timeout; /*Next timeout for the anti-debounce in ms*/
    uint32_t tick_pressed; /*Number of ticks when the button was pressed*/
    uint32_t duration; /*How much time the button has been pressed*/
    uint32_t button_id; /*Button ID. Must be unique.*/
} fsm_button_t; 

/* Defines and enums ----------------------------------------------------------*/

/* Enums */
enum
{
  BUTTON_RELEASED = 0, /*Starting state. Also comes here when the button has been released*/
  BUTTON_RELEASED_WAIT, /*State to perform the anti-debounce mechanism for a falling edge*/
  BUTTON_PRESSED, /*State while the button is being pressed*/
  BUTTON_PRESSED_WAIT, /*State to perform the anti-debounce mechanism for a rising edge*/
};   
/* State machine input or transition functions */

/*Check if the button has been pressed.*/
static bool check_button_pressed (fsm_t * p_this)
{
    fsm_button_t *p_fsm = (fsm_button_t *)(p_this);
    return port_button_is_pressed(p_fsm->button_id);
}	

/*Check if the button has been released.*/
static bool check_button_released (fsm_t * p_this)
{
    fsm_button_t *p_fsm = (fsm_button_t *)(p_this);
    return !port_button_is_pressed(p_fsm->button_id);
}	

/*Check if the debounce-time has passed.*/
static bool check_timeout (fsm_t * p_this)
{
    fsm_button_t *p_fsm = (fsm_button_t *)(p_this);
    if(port_button_get_tick() > p_fsm->next_timeout)
    {
        return true;
    }	
    else
    {
        return false;
    }
}	

/* State machine output or action functions */

/*Store the system tick when the button was pressed.*/
static void do_store_tick_pressed (fsm_t * p_this)	
{
    fsm_button_t *p_fsm = (fsm_button_t *)(p_this);
    p_fsm->tick_pressed = port_button_get_tick();
    p_fsm->next_timeout = port_button_get_tick() + p_fsm->debounce_time;
}

/*Store the duration of the button press.*/
static void do_set_duration	(fsm_t * p_this)	
{
    fsm_button_t *p_fsm = (fsm_button_t *)(p_this);
    p_fsm->duration = port_button_get_tick() - p_fsm->tick_pressed;
    p_fsm->next_timeout = port_button_get_tick() + p_fsm->debounce_time;
}

/*Array representing the transitions table of the FSM button.*/
static fsm_trans_t fsm_trans_button[] = {

    {BUTTON_RELEASED, check_button_pressed, BUTTON_RELEASED_WAIT, do_store_tick_pressed},
    {BUTTON_RELEASED_WAIT, check_timeout, BUTTON_PRESSED, NULL},
    {BUTTON_PRESSED, check_button_released, BUTTON_PRESSED_WAIT, do_set_duration},
    {BUTTON_PRESSED_WAIT, check_timeout, BUTTON_RELEASED, NULL}, 
    { -1 , NULL , -1, NULL },
    
};
/* Other auxiliary functions */

/*Create a new button FSM.

This FSM implements an anti-debounce mechanism. Debounces (or very fast button presses) lasting less than the debounce_time are filtered out.

The FSM stores the duration of the last button press. The user should ask for it using the function fsm_button_get_duration().

At start and reset, the duration value must be 0 ms. A value of 0 ms means that there has not been a new button press.*/
fsm_t *fsm_button_new(uint32_t debounce_time, uint32_t button_id)
{
    fsm_t *p_fsm = malloc(sizeof(fsm_button_t)); /* Do malloc to reserve memory of all other FSM elements, although it is interpreted as fsm_t (the first element of the structure) */
    fsm_button_init(p_fsm, debounce_time, button_id);
    return p_fsm;
}

/*Return the duration of the last button press.*/
uint32_t fsm_button_get_duration (fsm_t * p_this)	
{
     fsm_button_t *p_fsm = (fsm_button_t *)(p_this);
     return p_fsm->duration;
}

/*Reset the duration of the last button press.*/
void fsm_button_reset_duration (fsm_t * p_this)
{
    fsm_button_t *p_fsm = (fsm_button_t *)(p_this);
    p_fsm->duration = 0;
}

bool fsm_button_check_activity(fsm_t *p_this){

    fsm_button_t *p_fsm = (fsm_button_t *)(p_this);

    if(p_fsm->f.current_state == BUTTON_RELEASED){
        return false;
    }
    else{
        return true;
    }
}	


/*Initialize a button FSM.

This function initializes the default values of the FSM struct and calls to the port to initialize the HW of associated to the given ID.*/
void fsm_button_init(fsm_t *p_this, uint32_t debounce_time, uint32_t button_id)
{
    fsm_button_t *p_fsm = (fsm_button_t *)(p_this);
    fsm_init(p_this, fsm_trans_button);

    p_fsm->debounce_time = debounce_time;
    p_fsm->button_id = button_id;
    p_fsm->tick_pressed = 0;
    p_fsm->duration = 0;
    port_button_init(button_id);
}
