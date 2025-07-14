#ifndef PORT_RGB_H_
#define PORT_RGB_H_

#include <stdint.h>

#define RGB_0_ID 0
#define RGB_R_0_GPIO GPIOB
#define RGB_R_0_PIN 4
#define RGB_G_0_GPIO GPIOC
#define RGB_G_0_PIN 7
#define RGB_B_0_GPIO GPIOB
#define RGB_B_0_PIN 5

void port_rgb_init(uint8_t rgb_id);
void port_rgb_set_color(uint8_t rgb_id, uint8_t r, uint8_t g, uint8_t b);

#endif