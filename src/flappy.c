#include "flappy.h"

lv_obj_t *create_flappy(void)
{
    lv_obj_t *flappy_img;
    LV_IMG_DECLARE(flappy_lvgl);
    
    flappy_img = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(flappy_img,&flappy_lvgl);
    lv_obj_align(flappy_img, NULL, LV_ALIGN_CENTER, 0, 0);
    return flappy_img;
}


void move_flappy_x(lv_obj_t *dot, lv_coord_t x)
{
    lv_obj_set_x(dot, x);
}

void move_flappy_y(lv_obj_t *dot, lv_coord_t y)
{
    lv_obj_set_y(dot, y);
}

void reset_pos_flappy(lv_obj_t *dot)
{
    lv_obj_set_pos(dot, 0, 0);
}
