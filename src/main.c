#include <device.h>
#include <drivers/display.h>
#include <drivers/sensor.h>
#include <lvgl.h>
#include <stdio.h>
#include <string.h>
#include <zephyr.h>
#include <flappy.h>
#include <math.h>
#include <drivers/gpio.h>

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <logging/log.h>
LOG_MODULE_REGISTER(app);

lv_coord_t y = 25;
lv_coord_t x = 28;
const int16_t ascend = -5;
const int16_t descend = 1;
const uint16_t rateOfDescend = 100;
uint16_t ascendMax = 0;
uint16_t descendMax = 50; // screen resolution_y = 64, flappy_h = 15 (64 - 15 =~ 50)
lv_obj_t *flappy;
lv_obj_t *score;

/*
 * Get button configuration from the devicetree sw0 alias. This is mandatory.
 */
#define SW1_NODE DT_ALIAS(sw1)
#if !DT_NODE_HAS_STATUS(SW1_NODE, okay)
#error "Unsupported board: sw1 devicetree alias is not defined"
#endif
static const struct gpio_dt_spec button1 = GPIO_DT_SPEC_GET_OR(SW1_NODE, gpios,
															   {0});

static struct gpio_callback button_cb_data;

static void cooldown_expired(struct k_work *work)
{
	ARG_UNUSED(work);
	int val = gpio_pin_get_dt(&button1);
	if (val)
	{
		if (y > 0)
			y = y + ascend;
		move_flappy_y(flappy, y);
	}
}

static K_WORK_DELAYABLE_DEFINE(cooldown_work, cooldown_expired);

void button_pressed(const struct device *dev, struct gpio_callback *cb,
					uint32_t pins)
{
	k_work_reschedule(&cooldown_work, K_MSEC(15));
}

void init_display()
{
	const struct device *display_dev;
	display_dev = device_get_binding(CONFIG_LVGL_DISPLAY_DEV_NAME);

	if (display_dev == NULL)
	{
		LOG_ERR("device not found.  Aborting test.");
		return;
	}
	display_blanking_off(display_dev);
}

lv_obj_t *create_scoreboard()
{
	lv_obj_t *scoreLabel = lv_label_create(lv_scr_act(), NULL);
	lv_obj_align(scoreLabel, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);
	lv_label_set_text(scoreLabel, "0");
}

lv_obj_t *create_rect(lv_align_t startposition)
{
	/*Create an array for the points of the line*/
	static lv_point_t line_points[] = {{0, 0}, {0, 30}};
	/*Create style*/
	static lv_style_t style_line;
	lv_style_init(&style_line);
	lv_style_set_line_width(&style_line, LV_STATE_DEFAULT, 8);
	lv_style_set_line_color(&style_line, LV_STATE_DEFAULT, LV_COLOR_BLACK);

	/*Create a line and apply the new style*/
	lv_obj_t *dot;
	dot = lv_line_create(lv_scr_act(), NULL);
	lv_line_set_points(dot, line_points, 2); /*Set the points*/
	lv_obj_add_style(dot, LV_LINE_PART_MAIN, &style_line);
	lv_obj_align(dot, NULL, startposition, 0, 0);
	return dot;
}

bool collisionCheck(lv_obj_t *obj1, lv_obj_t *obj2)
{
	if (obj2->coords.x1 + 2 < obj1->coords.x2 && obj1->coords.x1 < obj2->coords.x2 - 2 &&
		obj1->coords.y1 < obj2->coords.y2 - 3 && obj2->coords.y1 - 2 < obj1->coords.y2)
	{
		return true;
	}
	return false;
}

void main(void)
{

	init_display();

	int ret;

	if (!device_is_ready(button1.port))
	{
		printk("Error: button device %s is not ready\n",
			   button1.port->name);
		return;
	}

	ret = gpio_pin_configure_dt(&button1, GPIO_INPUT);
	if (ret != 0)
	{
		printk("Error %d: failed to configure %s pin %d\n",
			   ret, button1.port->name, button1.pin);
		return;
	}

	ret = gpio_pin_interrupt_configure_dt(&button1,
										  GPIO_INT_EDGE_BOTH);
	if (ret != 0)
	{
		printk("Error %d: failed to configure interrupt on %s pin %d\n",
			   ret, button1.port->name, button1.pin);
		return;
	}

	gpio_init_callback(&button_cb_data, button_pressed, BIT(button1.pin));
	gpio_add_callback(button1.port, &button_cb_data);
	printk("Set up button at %s pin %d\n", button1.port->name, button1.pin);

	flappy = create_flappy();
	score = create_scoreboard();
	reset_pos_flappy(flappy);
	move_flappy_x(flappy, x);

	lv_obj_t *left = create_rect(LV_ALIGN_IN_BOTTOM_RIGHT);
	lv_obj_t *right = create_rect(LV_ALIGN_IN_BOTTOM_RIGHT);
	move_flappy_x(right, lv_obj_get_x(left) + 64);
	lv_task_handler();
	lv_align_t startPostions[] = {LV_ALIGN_IN_BOTTOM_RIGHT, LV_ALIGN_IN_RIGHT_MID, LV_ALIGN_IN_TOP_RIGHT};

	int i = 0;

	while (1)
	{
		move_flappy_y(flappy, y);
		if (!collisionCheck(flappy, left))
		{
			if (lv_obj_get_x(left) <= 0)
			{
				lv_obj_t *tmp = left;
				left = right;
				right = tmp;
				lv_obj_align(right, NULL, startPostions[i], 0, 0);
			}
			else
			{
				move_flappy_x(left, lv_obj_get_x(left) - 1);
				move_flappy_x(right, lv_obj_get_x(right) - 1);
			}
			k_sleep(K_MSEC(rateOfDescend));
			if (y < descendMax)
			{
				y = y + descend;
			}
			i++;
			if (i >= 4)
			{
				i = 0;
			}
			lv_task_handler();
		}
	}
}
