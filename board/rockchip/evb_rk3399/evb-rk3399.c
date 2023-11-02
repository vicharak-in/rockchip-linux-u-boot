// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2016 Rockchip Electronics Co., Ltd
 */

#include <common.h>
#include <dm.h>
#include <init.h>
#include <log.h>
#include <asm/arch-rockchip/periph.h>
#include <asm/arch-rockchip/cru.h>
#include <asm/arch-rockchip/gpio.h>
#include <asm/arch-rockchip/grf_rk3399.h>
#include <asm/io.h>
#include <linux/kernel.h>
#include <power/regulator.h>
#include <adc.h>
#include <spl_gpio.h>

#define KEY_DOWN_MIN_VAL        0
#define KEY_DOWN_MAX_VAL        30

int rockchip_dnl_key_pressed(void)
{
	unsigned int key_val;

	if (adc_channel_single_shot("saradc", 1, &key_val)) {
		printf("%s read recovery key failed\n", __func__);
		return false;
	}

	if (key_val >= KEY_DOWN_MIN_VAL && key_val <= KEY_DOWN_MAX_VAL)
		return true;
	else
		return false;
}

#ifndef CONFIG_SPL_BUILD
int board_early_init_f(void)
{
	struct udevice *regulator;
	int ret;

	ret = regulator_get_by_platname("vcc5v0_host", &regulator);
	if (ret) {
		debug("%s vcc5v0_host init fail! ret %d\n", __func__, ret);
		goto out;
	}

	ret = regulator_set_enable(regulator, true);
	if (ret)
		debug("%s vcc5v0-host-en set fail! ret %d\n", __func__, ret);

out:
	return 0;
}
#endif
