/*
 * Copyright (c) 2019 Sergey Koziakov <dya.eshshmai@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <device.h>
#include <drivers/display.h>
#include <drivers/gpio.h>
#include <drivers/clock_control.h>
#include <counter.h>
#include <lvgl.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <zephyr.h>

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <logging/log.h>
LOG_MODULE_REGISTER(app);


/* rtc configuration */

static void counter_callback (struct device *, void *);
struct counter_top_cfg counter_cfg;

static time_t m_time = 0;

/* backlight configuration */

#define BLLED0_PORT        DT_ALIAS_BLLED0_GPIOS_CONTROLLER
#define BLLED0             DT_ALIAS_BLLED0_GPIOS_PIN

#define BLLED1_PORT        DT_ALIAS_BLLED1_GPIOS_CONTROLLER
#define BLLED1             DT_ALIAS_BLLED1_GPIOS_PIN

#define BLLED2_PORT        DT_ALIAS_BLLED2_GPIOS_CONTROLLER
#define BLLED2             DT_ALIAS_BLLED2_GPIOS_PIN

static void backlight_init(void)
{
	struct device *dev;

	/*
	dev = device_get_binding(BLLED0_PORT);
	gpio_pin_configure(dev, BLLED0, GPIO_DIR_OUT);
	gpio_pin_write(dev, BLLED0, 0);

	dev = device_get_binding(BLLED1_PORT);
	gpio_pin_configure(dev, BLLED1, GPIO_DIR_OUT);
	gpio_pin_write(dev, BLLED1, 0);
	*/

	dev = device_get_binding(BLLED2_PORT);
	gpio_pin_configure(dev, BLLED2, GPIO_DIR_OUT);
	gpio_pin_write(dev, BLLED2, 0);
}


static int rtc_init(void)
{
	int err;

	struct device *counter = device_get_binding(DT_RTC_2_NAME);
	if (counter == NULL) {
		LOG_ERR("Device not found\n");
		return -ENODEV;
	}

	counter_cfg.flags = 0;
	counter_cfg.ticks = counter_us_to_ticks(counter, USEC_PER_SEC);
	counter_cfg.callback = counter_callback;
	counter_cfg.user_data = &counter_cfg;

	err = counter_set_top_value(counter, &counter_cfg);
	if (err != 0) {
		LOG_ERR("Unable to set top value");
		return err;
	}

	err = counter_start(counter);
	if (err != 0) {
		LOG_ERR("Unable to set top value");
		return err;
	}

	return 0;
}

static char clock_str[10] = "00:00:00";
volatile bool clock_str_updated = false;

void main(void)
{
	// u32_t err_code;
	struct device *display_dev;
	lv_obj_t *clock_label;

	if (rtc_init() != 0) {
		LOG_ERR("Unable to init RTC");
		return;
	}

	display_dev = device_get_binding(CONFIG_LVGL_DISPLAY_DEV_NAME);

	if (display_dev == NULL) {
		LOG_ERR("device not found.  Aborting test.");
		return;
	}
	backlight_init();

	clock_label = lv_label_create(lv_scr_act(), NULL);
	lv_label_set_text(clock_label, clock_str);

	/* get the style so that we can set larger font */
	lv_style_t big_font_style;
	lv_style_copy(&big_font_style, lv_label_get_style(clock_label, LV_LABEL_STYLE_MAIN));

	big_font_style.text.font = &lv_font_roboto_16;
	lv_label_set_style(clock_label, LV_LABEL_STYLE_MAIN, &big_font_style);

	lv_obj_align(clock_label, NULL, LV_ALIGN_CENTER, 0, 0);
	lv_label_set_align(clock_label, LV_LABEL_ALIGN_CENTER);

	display_blanking_off(display_dev);

	while (1) {
		if (clock_str_updated) {
			clock_str_updated = false;
			lv_label_set_text(clock_label, clock_str);
		}

		lv_task_handler();
		k_sleep(K_MSEC(10));
	}
}

static void counter_callback (struct device *counter_dev, void *user_data)
{
	m_time++;

	struct tm t = *localtime(&m_time);
	snprintf(clock_str, 10, "%02d:%02d:%02d", t.tm_hour, t.tm_min, t.tm_sec);

	clock_str_updated = true;
}
