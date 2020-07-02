/* Copyright (c) 2013-2014, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/kernel.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/cpu.h>
#include <linux/sched.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <soc/qcom/socinfo.h>

struct boot_stats {
	uint32_t bootloader_start;
	uint32_t bootloader_end;
	uint32_t bootloader_display;
	uint32_t bootloader_load_kernel;
};

static void __iomem *mpm_counter_base;
static uint32_t mpm_counter_freq;
static struct boot_stats __iomem *boot_stats;

static int mpm_parse_dt(void)
{
	struct device_node *np;
	u32 freq;

	np = of_find_compatible_node(NULL, NULL, "qcom,msm-imem-boot_stats");
	if (!np) {
		pr_err("can't find qcom,msm-imem node\n");
		return -ENODEV;
	}
	boot_stats = of_iomap(np, 0);
	if (!boot_stats) {
		pr_err("boot_stats: Can't map imem\n");
		return -ENODEV;
	}

	np = of_find_compatible_node(NULL, NULL, "qcom,mpm2-sleep-counter");
	if (!np) {
		pr_err("mpm_counter: can't find DT node\n");
		return -ENODEV;
	}

	if (!of_property_read_u32(np, "clock-frequency", &freq))
		mpm_counter_freq = freq;
	else
		return -ENODEV;

	if (of_get_address(np, 0, NULL, NULL)) {
		mpm_counter_base = of_iomap(np, 0);
		if (!mpm_counter_base) {
			pr_err("mpm_counter: cant map counter base\n");
			return -ENODEV;
		}
	}

	return 0;
}

static void print_boot_stats(void)
{
	pr_info("KPI: Bootloader start count = %u\n",
		readl_relaxed(&boot_stats->bootloader_start));
	pr_info("KPI: Bootloader end count = %u\n",
		readl_relaxed(&boot_stats->bootloader_end));
	pr_info("KPI: Bootloader display count = %u\n",
		readl_relaxed(&boot_stats->bootloader_display));
	pr_info("KPI: Bootloader load kernel count = %u\n",
		readl_relaxed(&boot_stats->bootloader_load_kernel));
	pr_info("KPI: Kernel MPM timestamp = %u\n",
		readl_relaxed(mpm_counter_base));
	pr_info("KPI: Kernel MPM Clock frequency = %u\n",
		mpm_counter_freq);
}

int boot_stats_init(void)
{
	int ret;

	ret = mpm_parse_dt();
	if (ret < 0)
		return -ENODEV;

	print_boot_stats();

	iounmap(boot_stats);
	iounmap(mpm_counter_base);

	return 0;
}

/*
 * Support for FTM & RECOVERY mode by ZTE_BOOT_RXZ_20131018 ruan.xianzhang
 */
#ifdef CONFIG_ZTE_BOOT_MODE
#define SOCINFO_CMDLINE_BOOTMODE          "androidboot.mode="
#define SOCINFO_CMDLINE_BOOTMODE_NORMAL   "normal"
#define SOCINFO_CMDLINE_BOOTMODE_FTM      "ftm"
#define SOCINFO_CMDLINE_BOOTMODE_RECOVERY "recovery"
#define SOCINFO_CMDLINE_BOOTMODE_CHARGER  "charger"
static int __init bootmode_init(char *mode)
{
	int is_boot_into_ftm = 0;
	int is_boot_into_recovery = 0;
	int is_boot_into_charger = 0;

	if (!strncmp(mode, SOCINFO_CMDLINE_BOOTMODE_NORMAL, strlen(SOCINFO_CMDLINE_BOOTMODE_NORMAL))) {
		is_boot_into_ftm = 0;
		is_boot_into_recovery = 0;
		is_boot_into_charger = 0;
	} else if (!strncmp(mode, SOCINFO_CMDLINE_BOOTMODE_FTM, strlen(SOCINFO_CMDLINE_BOOTMODE_FTM))) {
		is_boot_into_ftm = 1;
		is_boot_into_recovery = 0;
		is_boot_into_charger = 0;
	} else if (!strncmp(mode, SOCINFO_CMDLINE_BOOTMODE_RECOVERY, strlen(SOCINFO_CMDLINE_BOOTMODE_RECOVERY))) {
		is_boot_into_ftm = 0;
		is_boot_into_recovery = 1;
		is_boot_into_charger = 0;
	} else if (!strncmp(mode, SOCINFO_CMDLINE_BOOTMODE_CHARGER, strlen(SOCINFO_CMDLINE_BOOTMODE_CHARGER))) {
		is_boot_into_ftm = 0;
		is_boot_into_recovery = 0;
		is_boot_into_charger = 1;
	} else {
		is_boot_into_ftm = 0;
		is_boot_into_recovery = 0;
		is_boot_into_charger = 0;
	}

	socinfo_set_ftm_flag(is_boot_into_ftm);
	socinfo_set_recovery_flag(is_boot_into_recovery);
	socinfo_set_charging_flag(is_boot_into_charger);

	return 1;
}

__setup(SOCINFO_CMDLINE_BOOTMODE, bootmode_init);

/*ZTE_PM add code for pv version*/
#define SOCINFO_CMDLINE_PV_FLAG "androidboot.pv-version="
#define SOCINFO_CMDLINE_PV_VERSION   "1"
#define SOCINFO_CMDLINE_NON_PV_VERSION      "0"
static int __init zte_pv_flag_init(char *ver)
{
	int is_pv_ver = 0;

	if (!strncmp(ver, SOCINFO_CMDLINE_PV_VERSION, strlen(SOCINFO_CMDLINE_PV_VERSION))) {
		is_pv_ver = 1;
	}
	/*printk(KERN_ERR "pv flag: %d ", is_pv_ver);*/
	socinfo_set_pv_flag(is_pv_ver);
	return 0;
}
__setup(SOCINFO_CMDLINE_PV_FLAG, zte_pv_flag_init);

#endif /* CONFIG_ZTE_BOOT_MODE */

static int __init zte_hw_ver_init(char *ver)
{
	socinfo_set_hw_ver(ver);
	return 0;
}

#define SOCINFO_CMDLINE_HW_VER "androidboot.hw_ver="
__setup(SOCINFO_CMDLINE_HW_VER, zte_hw_ver_init);
