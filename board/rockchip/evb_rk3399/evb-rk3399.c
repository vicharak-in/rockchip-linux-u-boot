// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2016 Rockchip Electronics Co., Ltd
 */

#include <common.h>
#include <dm.h>
#include <efi_loader.h>
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
#define ROCKPI4_UPDATABLE_IMAGES	2

#if IS_ENABLED(CONFIG_EFI_HAVE_CAPSULE_SUPPORT)
static struct efi_fw_image fw_images[ROCKPI4_UPDATABLE_IMAGES] = {0};

struct efi_capsule_update_info update_info = {
	.num_images = ROCKPI4_UPDATABLE_IMAGES,
	.images = fw_images,
};

#endif

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

#if defined(CONFIG_EFI_HAVE_CAPSULE_SUPPORT) && defined(CONFIG_EFI_PARTITION)
static bool board_is_rockpi_4b(void)
{
	return CONFIG_IS_ENABLED(TARGET_EVB_RK3399) &&
		of_machine_is_compatible("radxa,rockpi4b");
}

static bool board_is_rockpi_4c(void)
{
	return CONFIG_IS_ENABLED(TARGET_EVB_RK3399) &&
		of_machine_is_compatible("radxa,rockpi4c");
}

static bool board_is_vaaman(void)
{
	return CONFIG_IS_ENABLED(TARGET_EVB_RK3399) &&
		of_machine_is_compatible("vicharak,vaaman");
}

void rockchip_capsule_update_board_setup(void)
{
	if (board_is_rockpi_4b()) {
		efi_guid_t idbldr_image_type_guid =
			ROCKPI_4B_IDBLOADER_IMAGE_GUID;
		efi_guid_t uboot_image_type_guid = ROCKPI_4B_UBOOT_IMAGE_GUID;

		guidcpy(&fw_images[0].image_type_id, &idbldr_image_type_guid);
		guidcpy(&fw_images[1].image_type_id, &uboot_image_type_guid);

		fw_images[0].fw_name = u"ROCKPI4B-IDBLOADER";
		fw_images[1].fw_name = u"ROCKPI4B-UBOOT";
	} else if (board_is_vaaman()) {
		efi_guid_t idbldr_image_type_guid =
			ROCKPI_4B_IDBLOADER_IMAGE_GUID;
		efi_guid_t uboot_image_type_guid = ROCKPI_4B_UBOOT_IMAGE_GUID;

		guidcpy(&fw_images[0].image_type_id, &idbldr_image_type_guid);
		guidcpy(&fw_images[1].image_type_id, &uboot_image_type_guid);

		fw_images[0].fw_name = u"VAAMAN-IDBLOADER";
		fw_images[1].fw_name = u"VAAMAN-UBOOT";
	} else if (board_is_rockpi_4c()) {
		efi_guid_t idbldr_image_type_guid =
			ROCKPI_4C_IDBLOADER_IMAGE_GUID;
		efi_guid_t uboot_image_type_guid = ROCKPI_4C_UBOOT_IMAGE_GUID;

		guidcpy(&fw_images[0].image_type_id, &idbldr_image_type_guid);
		guidcpy(&fw_images[1].image_type_id, &uboot_image_type_guid);

		fw_images[0].fw_name = u"ROCKPI4C-IDBLOADER";
		fw_images[1].fw_name = u"ROCKPI4C-UBOOT";
	}
}
#endif /* CONFIG_EFI_HAVE_CAPSULE_SUPPORT && CONFIG_EFI_PARTITION */
#define PMUGRF_BASE	0xff320000
#define GPIO3_BASE	0xff788000

void led_setup(void)
{
	struct rockchip_gpio_regs * const gpio0 = (void *)GPIO3_BASE;
	struct rk3399_pmugrf_regs * const pmugrf = (void *)PMUGRF_BASE;
	bool press_pwr_key = false;

	if (IS_ENABLED(CONFIG_SPL_ENV_SUPPORT)) {
		env_init();
		env_load();
		if (env_get_yesno("pwr_key") == 1)
			press_pwr_key = true;
	}

	if (press_pwr_key && !strcmp(get_reset_cause(), "POR")) {
		spl_gpio_output(gpio0, GPIO(BANK_D, 4), 1);

		spl_gpio_set_pull(&pmugrf->gpio0_p, GPIO(BANK_D, 5),
				  GPIO_PULL_NORMAL);
		while (readl(&gpio0->ext_port) & 0x20)
			;

		spl_gpio_output(gpio0, GPIO(BANK_D, 4), 0);
	}

	spl_gpio_output(gpio0, GPIO(BANK_D, 5), 1);
}
#endif /* !CONFIG_SPL_BUILD */
