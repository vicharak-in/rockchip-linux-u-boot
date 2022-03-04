/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (c) 2023 Vicharak India
 */

#ifndef __RK3399_VAAMAN_H
#define __RK3399_VAAMAN_H

#define ROCKCHIP_DEVICE_SETTINGS \
		"stdin=serial,usbkbd\0" \
		"stdout=serial,vidconsole\0" \
		"stderr=serial,vidconsole\0"

#define CONFIG_SYS_MMC_ENV_DEV		1	/* eMMC */
#define CONFIG_SYS_MMC_MAX_BLK_COUNT	32768

#include <configs/rk3399_common.h>

#endif /* __RK3399_VAAMAN_H */
