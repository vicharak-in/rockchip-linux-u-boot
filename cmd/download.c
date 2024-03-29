/*
 * (C) Copyright 2019 Rockchip Electronics Co., Ltd
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#include <common.h>
#include <command.h>
#include <console.h>

__weak void do_board_download(void)
{
}

static int do_download(cmd_tbl_t *cmdtp, int flag,
		       int argc, char * const argv[])
{
	disable_ctrlc(1);

	/* Allow board specific download, maybe noreturn */
	do_board_download();

	/* Generic download */
#if defined(CONFIG_CMD_ROCKUSB) && !defined(CONFIG_TARGET_RK3399_VAAMAN)
	run_command("rockusb 0 $devtype $devnum", 0);
#endif
	printf("Enter rockusb failed, fallback to bootrom...\n");
	flushc();
	run_command("rbrom", 0);

	return 0;
}

U_BOOT_CMD_ALWAYS(
	download, 1, 1, do_download,
	"enter rockusb/bootrom download mode", ""
);
