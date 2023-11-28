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
/*
 * Get button configuration from the devicetree sw0 alias. This is mandatory.
 */
#define SW0_NODE DT_ALIAS(sw0)
#if !DT_NODE_HAS_STATUS(SW0_NODE, okay)
#error "Unsupported board: sw0 devicetree alias is not defined"
#endif
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET_OR(SW0_NODE, gpios,
															  {0});
static struct gpio_callback button_cb_data;

void button_pressed(const struct device *dev, struct gpio_callback *cb,
					uint32_t pins)
{
	if (y > 0)
		y = y + ascend;
	move_flappy_y(flappy, y);
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

void main(void)
{

	init_display();

	int ret;

	if (!device_is_ready(button.port))
	{
		printk("Error: button device %s is not ready\n",
			   button.port->name);
		return;
	}

	ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
	if (ret != 0)
	{
		printk("Error %d: failed to configure %s pin %d\n",
			   ret, button.port->name, button.pin);
		return;
	}

	ret = gpio_pin_interrupt_configure_dt(&button,
										  GPIO_INT_EDGE_TO_ACTIVE);
	if (ret != 0)
	{
		printk("Error %d: failed to configure interrupt on %s pin %d\n",
			   ret, button.port->name, button.pin);
		return;
	}

	gpio_init_callback(&button_cb_data, button_pressed, BIT(button.pin));
	gpio_add_callback(button.port, &button_cb_data);
	printk("Set up button at %s pin %d\n", button.port->name, button.pin);

	flappy = create_flappy();
	reset_pos_flappy(flappy);
	lv_task_handler();

	while (1)
	{
		move_flappy_y(flappy, y);
		move_flappy_x(flappy, x);
		lv_task_handler();
		k_sleep(K_MSEC(rateOfDescend));
		if (y < descendMax)
		{
			y = y + descend;
		}
	}
}
