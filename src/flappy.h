#ifndef FLAPPY_H
#define FLAPPY_H

#include <device.h>
#include <drivers/display.h>
#include <lvgl.h>
#include <stdio.h>
#include <string.h>
#include <zephyr.h>

lv_obj_t *create_flappy(void);
void move_flappy_x(lv_obj_t *dot, lv_coord_t x);
void move_flappy_y(lv_obj_t *dot, lv_coord_t y);
void reset_pos_flappy(lv_obj_t *dot);

#endif