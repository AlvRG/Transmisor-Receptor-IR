/**
 * @file fsm_rx.c
 * @brief Infrared receiver FSM main file.
 * @author alumno1
 * @author alumno2
 * @date fecha
 */

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdlib.h>
#include <stdio.h>

/* Other includes */
#include "fsm_rx.h"
#include "fsm_rx_nec.h"
#include "port_rx.h"
#include "port_system.h"


/* Typedefs --------------------------------------------------------------------*/
typedef struct
{
  fsm_t f;
  fsm_t *p_fsm_rx_nec;
  uint32_t message_timeout_ms;
  uint32_t last_tick;
  uint32_t num_edges_detected;
  uint32_t code;
  bool is_repetition;
  bool is_error;
  bool status;
  uint8_t rx_id;
} fsm_rx_t;

/* Defines and enums ----------------------------------------------------------*/
/* Enums */
enum FSM_RX {
  OFF_RX,
  IDLE_RX,
  WAIT_RX
};

/* State machine input or transition functions */
static bool check_on_rx(fsm_t *p_this){

  fsm_rx_t *p_fsm = (fsm_rx_t *)(p_this);
  return p_fsm->status;
}

static bool check_off_rx(fsm_t *p_this){

  fsm_rx_t *p_fsm = (fsm_rx_t *)(p_this);
  return !p_fsm->status;
}

static bool check_edge_detection(fsm_t *p_this){

  fsm_rx_t *p_fsm = (fsm_rx_t *)(p_this);
  uint32_t value = port_rx_get_num_edges(p_fsm->rx_id);

  if(value != p_fsm->num_edges_detected){
    return true;
  }	
  else{
    return false;
  }
}

static bool check_timeout(fsm_t *p_this){

  fsm_rx_t *p_fsm = (fsm_rx_t *)(p_this);
  uint32_t value = port_system_get_millis();

  if((value - p_fsm->last_tick) > p_fsm->message_timeout_ms){
    return true;
  }
  else{
    return false;
  }
}	

/* State machine output or action functions */
static void do_rx_start(fsm_t *p_this){

   fsm_rx_t *p_fsm = (fsm_rx_t *)(p_this);
   p_fsm->p_fsm_rx_nec = fsm_rx_NEC_new();
   port_rx_tmr_start();
   p_fsm->num_edges_detected = 0;
   port_rx_clean_buffer(p_fsm->rx_id);
   port_rx_en(p_fsm->rx_id, true);		
}	

static void do_rx_stop(fsm_t *p_this){

  fsm_rx_t *p_fsm = (fsm_rx_t *)(p_this);
  port_rx_tmr_stop();
  port_rx_en(p_fsm->rx_id, false);
  fsm_destroy(p_fsm->p_fsm_rx_nec);
}

static void do_store_data(fsm_t *p_this){

  fsm_rx_t *p_fsm = (fsm_rx_t *)(p_this);
  p_fsm->is_repetition = fsm_rx_NEC_parse_code(p_fsm->p_fsm_rx_nec, port_rx_get_buffer_edges(p_fsm->rx_id), port_rx_get_num_edges(p_fsm->rx_id), &(p_fsm->code));

  if(p_fsm->code == 0x00 && p_fsm->is_repetition == false){
    p_fsm->is_error = true;
  }
  
  p_fsm->num_edges_detected = 0;
  port_rx_clean_buffer(p_fsm->rx_id);	
}	

static void do_update_len_and_timeout(fsm_t *p_this){

  fsm_rx_t *p_fsm = (fsm_rx_t *)(p_this);
  p_fsm->last_tick = port_system_get_millis();
  p_fsm->num_edges_detected = port_rx_get_num_edges(p_fsm->rx_id);
}	

static fsm_trans_t fsm_trans_rx[] = {

  {OFF_RX, check_on_rx, IDLE_RX, do_rx_start},
  {IDLE_RX, check_off_rx, OFF_RX, do_rx_stop},
  {IDLE_RX, check_edge_detection, WAIT_RX, do_update_len_and_timeout},
  {WAIT_RX, check_timeout, IDLE_RX, do_store_data},
  {WAIT_RX, check_edge_detection, WAIT_RX, do_update_len_and_timeout},
  { -1 , NULL , -1, NULL },
};  

/* Other auxiliary functions */
void fsm_rx_init(fsm_t *p_this, uint8_t rx_id)
{
  fsm_rx_t *p_fsm = (fsm_rx_t *)(p_this);
  fsm_init(p_this, fsm_trans_rx);

  p_fsm->rx_id = rx_id;
  p_fsm->code = 0;
  p_fsm->num_edges_detected = 0;
  p_fsm->last_tick = 0;
  p_fsm->is_error = false;
  p_fsm->is_repetition = false;
  p_fsm->status = true;
  p_fsm->message_timeout_ms = NEC_MESSAGE_TIMEOUT_US/1000;
  port_rx_init(p_fsm->rx_id);	
}

fsm_t *fsm_rx_new(uint8_t rx_id)
{
  fsm_t *p_fsm = malloc(sizeof(fsm_rx_t));
  fsm_rx_init(p_fsm, rx_id);
  return p_fsm;
}

void fsm_rx_set_rx_status(fsm_t *p_this, bool status){

  fsm_rx_t *p_fsm = (fsm_rx_t *)(p_this);
  p_fsm->status = status;
}	

uint32_t fsm_rx_get_code(fsm_t *p_this){

  fsm_rx_t *p_fsm = (fsm_rx_t *)(p_this);
  return p_fsm->code;
}	

bool fsm_rx_get_repetition(fsm_t *p_this){

   fsm_rx_t *p_fsm = (fsm_rx_t *)(p_this);
   return p_fsm->is_repetition;
}	

bool fsm_rx_get_error_code(fsm_t *p_this){

  fsm_rx_t *p_fsm = (fsm_rx_t *)(p_this);
   return p_fsm->is_error;
}

void fsm_rx_reset_code(fsm_t *p_this){

   fsm_rx_t *p_fsm = (fsm_rx_t *)(p_this);
   p_fsm->code = 0;
   p_fsm->is_error = false;
   p_fsm->is_repetition = false;
}

bool fsm_rx_check_activity(fsm_t *p_this){

  fsm_rx_t *p_fsm = (fsm_rx_t *)(p_this);

  if(p_fsm->f.current_state == WAIT_RX){
    return true;
  }
  else{
    return false;
  }

}	




