#include "port_rgb.h"
#include "port_system.h"

typedef struct
{
GPIO_TypeDef *p_port_red;
uint8_t pin_red; 
GPIO_TypeDef *p_port_green;
uint8_t pin_green;
GPIO_TypeDef *p_port_blue;
uint8_t pin_blue;
} port_rgb_hw_t;

static  port_rgb_hw_t rgb_arr[] = {
    [RGB_0_ID] = {.p_port_red = RGB_R_0_GPIO, .pin_red = RGB_R_0_PIN, .p_port_green = RGB_G_0_GPIO, .pin_green = RGB_G_0_PIN, .p_port_blue = RGB_B_0_GPIO, .pin_blue = RGB_B_0_PIN},
};


void port_rgb_init(uint8_t rgb_id){

    port_system_gpio_config(rgb_arr[rgb_id].p_port_red, rgb_arr[rgb_id].pin_red, GPIO_MODE_OUT, GPIO_PUPDR_NOPULL);
    port_system_gpio_config(rgb_arr[rgb_id].p_port_green, rgb_arr[rgb_id].pin_green, GPIO_MODE_OUT, GPIO_PUPDR_NOPULL);
    port_system_gpio_config(rgb_arr[rgb_id].p_port_blue, rgb_arr[rgb_id].pin_blue, GPIO_MODE_OUT, GPIO_PUPDR_NOPULL);

    port_rgb_set_color(rgb_id, 0, 0, 0);
}

void port_rgb_set_color(uint8_t rgb_id, uint8_t r, uint8_t g, uint8_t b){

    port_system_gpio_write(rgb_arr[rgb_id].p_port_red, rgb_arr[rgb_id].pin_red, (bool )r);
    port_system_gpio_write(rgb_arr[rgb_id].p_port_green, rgb_arr[rgb_id].pin_green, (bool )g);
    port_system_gpio_write(rgb_arr[rgb_id].p_port_blue, rgb_arr[rgb_id].pin_blue, (bool )b);
}	
