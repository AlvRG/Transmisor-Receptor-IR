/**
 * @file fsm_rx_nec.c
 * @brief NEC processing FSM main file.
 * @author alumno1
 * @author alumno2
 * @date fecha
 */

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdlib.h>

/* Ohter includes */
#include "fsm_rx_nec.h"

/* Typedefs --------------------------------------------------------------------*/
typedef struct
{
  fsm_t f;
  uint16_t *p_edge_ticks;
  uint32_t num_edges_to_read;
  uint32_t bits_remaining_to_read;
  uint32_t code;
  bool is_repetition;
} fsm_rx_nec_t;

/* Defines and enums ----------------------------------------------------------*/
/* Enums */
enum FSM_RX_NEC {
  NEC_IDLE,
  NEC_INIT,
  NEC_SYMBOL_SILENCE,
  NEC_SYMBOL_PULSE
};

/* Private functions */
/**
 * @brief Auxiliary function to compute the time difference (in ticks) between the current and the next edge.
 *
 * @param p_edge_ticks Pointer to the array containing the the time ticks of the edges detected by the infrared receiver
 * @return Time difference in ticks
 */
static uint16_t _get_diff_ticks(uint16_t *p_edge_ticks)
{
  return *(p_edge_ticks + 1) - *(p_edge_ticks);
}

/**
 * @brief Auxiliary function to check if a given value is between two other values.
 *
 * This is used to check if a time difference is between the NEC tolerance time interval.
 *
 * @param value Value to check
 * @param min Minimum value allowed by the range (greater than or equal to).
 * @param max Maximum value allowed by the range (lower than or equal to).
 * @return true
 * @return false
 */
bool _value_in_range(uint16_t value, uint16_t min, uint16_t max)
{
  return ((value >= min) && (value <= max));
}

/* State machine input or transition functions */

static bool check_is_init_silence (fsm_t *p_this){

  fsm_rx_nec_t *p_fsm = (fsm_rx_nec_t *)(p_this);

  uint16_t value = _get_diff_ticks(p_fsm->p_edge_ticks);
  
  return _value_in_range(value, NEC_RX_PROLOGUE_TICKS_SILENCE_MIN, NEC_RX_PROLOGUE_TICKS_SILENCE_MAX);
}	

static bool check_is_init_noise	(fsm_t *p_this){

  return !check_is_init_silence(p_this);

}

static bool check_is_prologue_pulse	(fsm_t *p_this){

  fsm_rx_nec_t *p_fsm = (fsm_rx_nec_t *)(p_this);

  uint16_t value = _get_diff_ticks(p_fsm->p_edge_ticks);
  
  return _value_in_range(value, NEC_RX_PROLOGUE_TICKS_PULSE_MIN, NEC_RX_PROLOGUE_TICKS_PULSE_MAX);
}	

static bool check_is_repetition_pulse	(fsm_t *p_this){

  fsm_rx_nec_t *p_fsm = (fsm_rx_nec_t *)(p_this);

 	uint16_t value = _get_diff_ticks(p_fsm->p_edge_ticks);
  
  return _value_in_range(value, NEC_RX_REPETITION_TICKS_PULSE_MIN, NEC_RX_REPETITION_TICKS_PULSE_MAX);

}

static bool check_is_init_pulse_noise	(fsm_t *p_this){

  if(check_is_prologue_pulse(p_this) == false && check_is_repetition_pulse(p_this) == false){
    return true;
  }

  else{
    return false;
  }
}

static bool check_is_last_symbol(fsm_t *p_this){

  fsm_rx_nec_t *p_fsm = (fsm_rx_nec_t *)(p_this);
  if(p_fsm->bits_remaining_to_read == 0){
    return true;
  }
  else{
    return false;
  }
}

static bool check_is_symbol_silence	(fsm_t *p_this){

  if(check_is_last_symbol(p_this)){
    return false;
  }

  fsm_rx_nec_t *p_fsm = (fsm_rx_nec_t *)(p_this);

 	uint16_t value = _get_diff_ticks(p_fsm->p_edge_ticks);
  
  return _value_in_range(value, NEC_RX_SYMBOL_TICKS_SILENCE_MIN, NEC_RX_SYMBOL_TICKS_SILENCE_MAX);

}

static bool check_is_symbol_silence_noise	(fsm_t *p_this){

  if(check_is_last_symbol(p_this)){
    return false;
  }

  return !check_is_symbol_silence(p_this);
}

static bool check_is_symbol_0_pulse	(fsm_t *p_this){

  fsm_rx_nec_t *p_fsm = (fsm_rx_nec_t *)(p_this);

 	uint16_t value = _get_diff_ticks(p_fsm->p_edge_ticks);
  
  return _value_in_range(value, NEC_RX_SYMBOL_0_TICKS_PULSE_MIN, NEC_RX_SYMBOL_0_TICKS_PULSE_MAX);
}	

static bool check_is_symbol_1_pulse	(fsm_t *p_this){

  fsm_rx_nec_t *p_fsm = (fsm_rx_nec_t *)(p_this);

 	uint16_t value = _get_diff_ticks(p_fsm->p_edge_ticks);
  
  return _value_in_range(value, NEC_RX_SYMBOL_1_TICKS_PULSE_MIN, NEC_RX_SYMBOL_1_TICKS_PULSE_MAX);
}	

static bool check_is_symbol_pulse_noise	(fsm_t *p_this){

  if(check_is_symbol_0_pulse(p_this) == false && check_is_symbol_1_pulse(p_this) == false){
    return true;
  }

  else{
    return false;
  }
}	

/* State machine output or action functions */

static void do_jump_to_next_edge (fsm_t *p_this){

  fsm_rx_nec_t *p_fsm = (fsm_rx_nec_t *)(p_this);

  p_fsm->p_edge_ticks++;
  p_fsm->num_edges_to_read = p_fsm->num_edges_to_read - 1;
}	

static void do_repetition_starts (fsm_t *p_this){

  fsm_rx_nec_t *p_fsm = (fsm_rx_nec_t *)(p_this);

  p_fsm->p_edge_ticks++;
  p_fsm->num_edges_to_read = p_fsm->num_edges_to_read - 1;
  p_fsm->bits_remaining_to_read = 0;
  p_fsm->is_repetition = true;
}

static void do_command_starts	(fsm_t *p_this){

  fsm_rx_nec_t *p_fsm = (fsm_rx_nec_t *)(p_this);

  p_fsm->p_edge_ticks++;
  p_fsm->num_edges_to_read = p_fsm->num_edges_to_read - 1;
  p_fsm->bits_remaining_to_read = NEC_FRAME_BITS;
  p_fsm->is_repetition = false;

}

static void do_reset_and_jump_to_next_edge	(fsm_t *p_this){

   fsm_rx_nec_t *p_fsm = (fsm_rx_nec_t *)(p_this);

  p_fsm->p_edge_ticks++;
  p_fsm->num_edges_to_read = p_fsm->num_edges_to_read - 1;
  p_fsm->code = 0;

}

static void do_reset_and_jump_two_edges	(fsm_t *p_this){

  fsm_rx_nec_t *p_fsm = (fsm_rx_nec_t *)(p_this);

  p_fsm->p_edge_ticks++;
  p_fsm->p_edge_ticks++;

   if(p_fsm->num_edges_to_read > 1){
    p_fsm->num_edges_to_read = p_fsm->num_edges_to_read - 2;
   }
   else{
    p_fsm->num_edges_to_read = 0;
   }
}	

static void do_store_bit_0	(fsm_t *p_this){

  fsm_rx_nec_t *p_fsm = (fsm_rx_nec_t *)(p_this);

  p_fsm->code = p_fsm->code << 1;
  p_fsm->p_edge_ticks++;
  p_fsm->num_edges_to_read = p_fsm->num_edges_to_read - 1;
  p_fsm->bits_remaining_to_read = p_fsm->bits_remaining_to_read - 1;

}

static void do_store_bit_1	(fsm_t *p_this){

  fsm_rx_nec_t *p_fsm = (fsm_rx_nec_t *)(p_this);

  p_fsm->code = p_fsm->code << 1;
  p_fsm->code = p_fsm->code + 1;
  p_fsm->p_edge_ticks++;
  p_fsm->num_edges_to_read = p_fsm->num_edges_to_read - 1;
  p_fsm->bits_remaining_to_read = p_fsm->bits_remaining_to_read - 1;

}	

static void do_set_end	(fsm_t *p_this){

  fsm_rx_nec_t *p_fsm = (fsm_rx_nec_t *)(p_this);
  p_fsm->num_edges_to_read = 0;
}

static fsm_trans_t fsm_trans_rx_nec[] = {

  {NEC_IDLE, check_is_init_noise, NEC_IDLE, do_reset_and_jump_two_edges},
  {NEC_IDLE, check_is_init_silence, NEC_INIT, do_reset_and_jump_to_next_edge},
  {NEC_INIT, check_is_init_pulse_noise, NEC_IDLE, do_jump_to_next_edge},
  {NEC_INIT, check_is_repetition_pulse, NEC_SYMBOL_SILENCE, do_repetition_starts},
  {NEC_INIT, check_is_prologue_pulse, NEC_SYMBOL_SILENCE, do_command_starts},
  {NEC_SYMBOL_SILENCE, check_is_last_symbol, NEC_IDLE, do_set_end},
  {NEC_SYMBOL_SILENCE, check_is_symbol_silence_noise, NEC_IDLE, NULL},
  {NEC_SYMBOL_SILENCE, check_is_symbol_silence, NEC_SYMBOL_PULSE, do_jump_to_next_edge},
  {NEC_SYMBOL_PULSE, check_is_symbol_0_pulse, NEC_SYMBOL_SILENCE, do_store_bit_0},
  {NEC_SYMBOL_PULSE, check_is_symbol_1_pulse, NEC_SYMBOL_SILENCE, do_store_bit_1},
  {NEC_SYMBOL_PULSE, check_is_symbol_pulse_noise, NEC_IDLE, do_jump_to_next_edge},
  { -1 , NULL , -1, NULL },
};

/* Other auxiliary functions */

void fsm_rx_NEC_init(fsm_t *p_this)
{
  fsm_rx_nec_t *p_fsm = (fsm_rx_nec_t *)(p_this);
  fsm_init(p_this, fsm_trans_rx_nec);

  p_fsm->code = 0;
  p_fsm->num_edges_to_read = 0;
  p_fsm->p_edge_ticks = NULL;
  p_fsm->is_repetition = false;
}

bool fsm_rx_NEC_parse_code	(	fsm_t *p_this, uint16_t *p_edge_ticks, uint32_t num_edges, uint32_t *p_code){

  fsm_rx_nec_t *p_fsm = (fsm_rx_nec_t *)(p_this);
  p_fsm->f.current_state = NEC_IDLE;
  p_fsm->code = 0;
  p_fsm->is_repetition = false;
  p_fsm->num_edges_to_read = num_edges;
  p_fsm->p_edge_ticks = p_edge_ticks;

  while(p_fsm->num_edges_to_read > 1){

    fsm_fire(&(p_fsm->f));

  }

  *p_code = p_fsm->code;
  return p_fsm->is_repetition;
}

fsm_t *fsm_rx_NEC_new()
{
  fsm_t *p_fsm = malloc(sizeof(fsm_rx_nec_t));
  fsm_rx_NEC_init(p_fsm);
  return p_fsm;
}