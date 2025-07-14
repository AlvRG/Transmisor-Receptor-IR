/**
 * @file fsm_tx.c
 * @brief Infrared transmitter FSM main file.
 * @author Alvaro Rodriguez Gabaldon
 * @author Miguel Lobo Benito
 * @date fecha
 */

/* Includes ------------------------------------------------------------------*/
#include "fsm_tx.h"
#include "port_tx.h"
#include <stdlib.h>

/* Typedefs --------------------------------------------------------------------*/
typedef struct 
{
    fsm_t f; /*Infrared transmitter FSM*/
    uint32_t code; /*NEC code to be sent*/
    uint8_t tx_id; /*Transmitter ID. Must be unique.*/
}fsm_tx_t;



/* Defines and enums ----------------------------------------------------------*/
/* Enums */
enum FSM_TX{
    WAIT_TX /*Unique state of the FSM waiting to receive a code.*/
};


/* NEC private functions */

/*	Wait for some ticks. Each tick if the symbol timer lasts for NEC_TX_TIMER_TICK_BASE_US microseconds.*/
static void _wait_for_tx_ticks (uint32_t ticks){

    uint32_t initial_tick = port_tx_tmr_get_tick();

    while ((port_tx_tmr_get_tick() - initial_tick) < ticks)
  {
  }

}

/*Send a PWM burst and wait for a silence*/
static void _send_NEC_burst	(uint8_t tx_id, uint32_t ticks_ON, uint32_t ticks_OFF){

    port_tx_pwm_timer_set(tx_id, true);
    _wait_for_tx_ticks (ticks_ON);
    port_tx_pwm_timer_set(tx_id, false);
    _wait_for_tx_ticks (ticks_OFF);

}	


/* State machine input or transition functions */

/*	Check if it has been received a code other than '0x00'.*/
static bool check_tx_start (fsm_t *p_this){


    fsm_tx_t *p_fsm = (fsm_tx_t *)(p_this);
    uint32_t code0 = p_fsm->code;

    if(code0 != 0x00){
        return true;
    }
    else{
        return false;
    }

}


/* State machine output or action functions */

/*	Start the transmission of the received NEC code.*/
static void do_tx_start	(fsm_t *p_this){

    fsm_tx_t *p_fsm = (fsm_tx_t *)(p_this);
    fsm_send_NEC_code(p_fsm->tx_id, p_fsm->code);
    p_fsm->code = 0x00;
}

/*	Array representing the transitions table of the FSM infrared transmitter.*/
static fsm_trans_t fsm_trans_tx[] = {

    {WAIT_TX, check_tx_start, WAIT_TX, do_tx_start},
    { -1 , NULL , -1, NULL },
    
};


/* Other auxiliary functions */

/*	Set the code given*/
void fsm_tx_set_code(fsm_t *p_this, uint32_t code)
{
    fsm_tx_t *p_fsm = (fsm_tx_t *)(p_this);

    if(code != 0x00){
        p_fsm->code = code;
    }
}

/*	Start the process to transmit the code stored.*/
void fsm_send_NEC_code(uint8_t tx_id, uint32_t code)
{
    uint32_t bit_mask = 0x80000000;
    port_tx_symbol_tmr_start();
    _send_NEC_burst(tx_id, NEC_TX_PROLOGUE_TICKS_ON, NEC_TX_PROLOGUE_TICKS_OFF);

    while(bit_mask > 0){

        if(code & bit_mask){

             _send_NEC_burst(tx_id, NEC_TX_SYM_1_TICKS_ON, NEC_TX_SYM_1_TICKS_OFF);
        }

        else{
            
            _send_NEC_burst(tx_id, NEC_TX_SYM_0_TICKS_ON, NEC_TX_SYM_0_TICKS_OFF);
        }

        bit_mask >>= 1;
    }

     _send_NEC_burst(tx_id, NEC_TX_EPILOGUE_TICKS_ON, NEC_TX_EPILOGUE_TICKS_OFF);
     port_tx_symbol_tmr_stop();

}

bool fsm_tx_check_activity(fsm_t *p_this){

    fsm_tx_t *p_fsm = (fsm_tx_t *)(p_this);
    return p_fsm->f.current_state != WAIT_TX;
}



fsm_t *fsm_tx_new(uint8_t tx_id) 
{
    fsm_t *p_fsm = malloc(sizeof(fsm_tx_t)); /* Do malloc to reserve memory of all other FSM elements, although it is interpreted as fsm_t (the first element of the structure) */
    fsm_tx_init(p_fsm, tx_id);
    return p_fsm;
}

/*	Initialize an infrared transmitter FSM.*/
void fsm_tx_init(fsm_t *p_this, uint8_t tx_id)
{
    fsm_tx_t *p_fsm = (fsm_tx_t *)(p_this);
    fsm_init(p_this, fsm_trans_tx);

    p_fsm->tx_id = tx_id;
    p_fsm->code = 0x00;
    port_tx_init(tx_id, false);
}
