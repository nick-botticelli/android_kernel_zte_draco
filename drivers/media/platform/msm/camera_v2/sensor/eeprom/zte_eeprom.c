/* Copyright (c) 2011-2015, The Linux Foundation. All rights reserved.
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

#include <linux/module.h>
#include <linux/of_gpio.h>
#include <linux/delay.h>
#include <linux/crc32.h>
#include "msm_sd.h"
#include "msm_cci.h"
#include "msm_eeprom.h"

#define CONFIG_MSMB_CAMERA_DEBUG 1
#undef CDBG
#ifdef CONFIG_MSMB_CAMERA_DEBUG
#define CDBG(fmt, args...) pr_err(fmt, ##args)
#else
#define CDBG(fmt, args...) do { } while (0)
#endif

/*
  * camera sensor module compatile
  *
  * by ZTE_YCM_20140728 yi.changming 000028
  */
#define	SENSOR_INFO_MODULE_ID_QETCH 0X0001
#define	SENSOR_INFO_MODULE_ID_SUNNY 0X000a
#define	SENSOR_INFO_MODULE_ID_MCNEX 0X00a0
#define	SENSOR_INFO_MODULE_ID_TRULY 0X0010
#define	SENSOR_INFO_MODULE_ID_SAMSUNG 0X0033
#define	SENSOR_INFO_MODULE_ID_KARR 0X0002
#define	ZTE_EEPROM_ERROR -1

/*
* Post compatible module info to vendor.by FENGYUAO_20150528.
*/
#define SENSOR_NAME_MAX_SIZE 32
/*
char post_sensor_module_name[SENSOR_NAME_MAX_SIZE];
static char post_chromtix_lib_name[SENSOR_NAME_MAX_SIZE];
static char post_default_chromtix_lib_name[SENSOR_NAME_MAX_SIZE];
*/
/* end */

typedef struct {
	uint16_t id;
	const char *sensor_module_name;
	const char *chromtix_lib_name;
	const char *default_chromtix_lib_name;
} MODULE_Map_Table;

MODULE_Map_Table AR0542_MODULE_MAP[] = {
	{SENSOR_INFO_MODULE_ID_QETCH, "qtech_ar0542", "qtech_ar0542", NULL},
	{SENSOR_INFO_MODULE_ID_SUNNY, "sunny_ar0542", "sunny_ar0542", NULL},
	{SENSOR_INFO_MODULE_ID_MCNEX, "mcnex_ar0542", "mcnex_ar0542", NULL},
	{SENSOR_INFO_MODULE_ID_TRULY, "truly_ar0542", "truly_ar0542", NULL},
	{SENSOR_INFO_MODULE_ID_SAMSUNG, "samsung_ar0542", "samsung_ar0542", NULL},
	{SENSOR_INFO_MODULE_ID_KARR, "karr_ar0542", "karr_ar0542", NULL},
};
#define T4K35_SENSOR_INFO_MODULE_ID_SUNNY		0x01
#define T4K35_SENSOR_INFO_MODULE_ID_TRULY		0x02
#define T4K35_SENSOR_INFO_MODULE_ID_A_KERR		0x03
#define T4K35_SENSOR_INFO_MODULE_ID_LITEARRAY		0x04
#define T4K35_SENSOR_INFO_MODULE_ID_DARLING		0x05
#define T4K35_SENSOR_INFO_MODULE_ID_QTECH		0x06
#define T4K35_SENSOR_INFO_MODULE_ID_QFLIM		0x07
#define T4K35_SENSOR_INFO_MODULE_ID_RICHTEK		0x08
#define T4K35_SENSOR_INFO_MODULE_ID_FOXCONN		0x11
#define T4K35_SENSOR_INFO_MODULE_ID_IMPORTEK		0x12
#define T4K35_SENSOR_INFO_MODULE_ID_ALTEK		0x13
#define T4K35_SENSOR_INFO_MODULE_ID_ABICO_ABILITY	0x14
#define T4K35_SENSOR_INFO_MODULE_ID_LITE_ON		0x15
#define T4K35_SENSOR_INFO_MODULE_ID_CHICONY		0x16
#define T4K35_SENSOR_INFO_MODULE_ID_PRIMAX		0x17
#define T4K35_SENSOR_INFO_MODULE_ID_SHARP		0x21
#define T4K35_SENSOR_INFO_MODULE_ID_MCNEX		0x31

MODULE_Map_Table T4K35_MODULE_MAP[] = {
	{T4K35_SENSOR_INFO_MODULE_ID_SUNNY, "sunny_t4k35", "sunny_t4k35", NULL},
	{T4K35_SENSOR_INFO_MODULE_ID_TRULY, "truly_t4k35", "truly_t4k35", NULL},
	{T4K35_SENSOR_INFO_MODULE_ID_A_KERR, "a_kerr_t4k35", "a_kerr_t4k35", NULL},
	{T4K35_SENSOR_INFO_MODULE_ID_LITEARRAY, "litearray_t4k35", "litearray_t4k35", NULL},
	{T4K35_SENSOR_INFO_MODULE_ID_DARLING, "darling_t4k35", "darling_t4k35", NULL},
	{T4K35_SENSOR_INFO_MODULE_ID_QTECH, "qtech_t4k35", "qtech_t4k35", NULL},
	{T4K35_SENSOR_INFO_MODULE_ID_QFLIM, "qflim_t4k35", "qflim_t4k35", NULL},
	{T4K35_SENSOR_INFO_MODULE_ID_RICHTEK, "richtek_t4k35", "richtek_t4k35", NULL},
	{T4K35_SENSOR_INFO_MODULE_ID_FOXCONN, "foxconn_t4k35", "foxconn_t4k35", NULL},
	{T4K35_SENSOR_INFO_MODULE_ID_IMPORTEK, "importek_t4k35", "importek_t4k35", NULL},
	{T4K35_SENSOR_INFO_MODULE_ID_ALTEK, "altek_t4k35", "altek_t4k35", NULL},
	{T4K35_SENSOR_INFO_MODULE_ID_ABICO_ABILITY, "abico_ability_t4k35", "abico_ability_t4k35", NULL},
	{T4K35_SENSOR_INFO_MODULE_ID_LITE_ON, "lite_on_t4k35", "lite_on_t4k35", NULL},
	{T4K35_SENSOR_INFO_MODULE_ID_CHICONY, "chicony_t4k35", "chicony_t4k35", NULL},
	{T4K35_SENSOR_INFO_MODULE_ID_PRIMAX, "primax_t4k35", "primax_t4k35", NULL},
	{T4K35_SENSOR_INFO_MODULE_ID_SHARP, "sharp_t4k35", "sharp_t4k35", NULL},
	{T4K35_SENSOR_INFO_MODULE_ID_MCNEX, "mcnex_t4k35", "mcnex_t4k35", NULL},
};

#define OV5670_SENSOR_INFO_MODULE_ID_SUNWIN		0x68
#define OV5670_SENSOR_INFO_MODULE_ID_QTECH		0x06

MODULE_Map_Table OV5670_MODULE_MAP[] = {
	{OV5670_SENSOR_INFO_MODULE_ID_SUNWIN, "sunwin_ov5670_30010a3", "sunwin_ov5670_30010a3", NULL},
	{OV5670_SENSOR_INFO_MODULE_ID_QTECH, "qtech_ov5670_30010a3", "qtech_ov5670_30010a3", NULL},
};

#define OV8856_SENSOR_INFO_MODULE_ID_SUNNY		0x01
#define OV8856_SENSOR_INFO_MODULE_ID_TRULY		0x02
#define OV8856_SENSOR_INFO_MODULE_ID_A_KERR		0x03
#define OV8856_SENSOR_INFO_MODULE_ID_LITEARRAY	0x04
#define OV8856_SENSOR_INFO_MODULE_ID_DARLING		0x05
#define OV8856_SENSOR_INFO_MODULE_ID_QTECH		0x06
#ifdef CONFIG_BOARD_CALBEE
MODULE_Map_Table OV8856_MODULE_MAP[] = {
	{OV8856_SENSOR_INFO_MODULE_ID_A_KERR, "a_kerr_ov8856_tm107v'", "a_kerr_ov8856_tm107v", NULL},
	{OV8856_SENSOR_INFO_MODULE_ID_QTECH, "qtech_ov8856_tm107v", "qtech_ov8856_tm107v", NULL},
};
#else
MODULE_Map_Table OV8856_MODULE_MAP[] = {
	{OV8856_SENSOR_INFO_MODULE_ID_SUNNY, "sunny_ov8856", "sunny_ov8856", NULL},
	{OV8856_SENSOR_INFO_MODULE_ID_TRULY, "truly_ov8856", "truly_ov8856", NULL},
	{OV8856_SENSOR_INFO_MODULE_ID_A_KERR, "a_kerr_ov8856", "a_kerr_ov8856", NULL},
	{OV8856_SENSOR_INFO_MODULE_ID_LITEARRAY, "litearray_ov8856", "litearray_ov8856", NULL},
	{OV8856_SENSOR_INFO_MODULE_ID_DARLING, "darling_ov8856", "darling_tov8856", NULL},
	{OV8856_SENSOR_INFO_MODULE_ID_QTECH, "qtech_ov8856", "qtech_ov8856", NULL},
};
#endif

#define S5K5E8_SENSOR_INFO_MODULE_ID_SUNNY		0x01
#define S5K5E8_SENSOR_INFO_MODULE_ID_SUNWIN		0x68
#define S5K5E8_SENSOR_INFO_MODULE_ID_QTECH		0x06
#define S5K5E8_SENSOR_INFO_MODULE_ID_A_KERR		0x03
#define S5K5E8_SENSOR_INFO_MODULE_ID_BYD		0x42
#define S5K5E8_SENSOR_INFO_MODULE_ID_SHINETECH	0xa1
#ifdef CONFIG_BOARD_XRAY45
MODULE_Map_Table S5K5E8_MODULE_MAP[] = {
	{ S5K5E8_SENSOR_INFO_MODULE_ID_SUNNY, "sunny_s5k5e8_spr", "sunny_s5k5e8_spr", NULL},
	{S5K5E8_SENSOR_INFO_MODULE_ID_SUNWIN, "sunwin_s5k5e8_spr", "sunwin_s5k5e8_spr", NULL},
	{ S5K5E8_SENSOR_INFO_MODULE_ID_QTECH, "qtech_s5k5e8_spr", "qtech_s5k5e8_spr", NULL},
	{ S5K5E8_SENSOR_INFO_MODULE_ID_A_KERR, "a_kerr_s5k5e8_spr", "a_kerr_s5k5e8_spr", NULL},
};
#else
MODULE_Map_Table S5K5E8_MODULE_MAP[] = {
	{S5K5E8_SENSOR_INFO_MODULE_ID_SUNNY, "sunny_s5k5e8", "sunny_s5k5e8", NULL},
	{S5K5E8_SENSOR_INFO_MODULE_ID_SUNWIN, "sunwin_s5k5e8", "sunwin_s5k5e8", NULL},
	{S5K5E8_SENSOR_INFO_MODULE_ID_QTECH, "qtech_s5k5e8", "qtech_s5k5e8", NULL},
	{S5K5E8_SENSOR_INFO_MODULE_ID_A_KERR, "a_kerr_s5k5e8", "a_kerr_s5k5e8", NULL},
	{S5K5E8_SENSOR_INFO_MODULE_ID_BYD, "byd_s5k5e8", "byd_s5k5e8", NULL},
	{S5K5E8_SENSOR_INFO_MODULE_ID_SHINETECH, "shinetech_s5k5e8", "shinetech_s5k5e8", NULL},
};
#endif

#define S5K4H8_SENSOR_INFO_MODULE_ID_SUNWIN		0x52
#define S5K4H8_SENSOR_INFO_MODULE_ID_A_KERR		0x03

MODULE_Map_Table S5K4H8_MODULE_MAP[] = {
	{S5K4H8_SENSOR_INFO_MODULE_ID_SUNWIN, "sunwin_s5k4h8", "sunwin_s5k4h8", NULL},
	{S5K4H8_SENSOR_INFO_MODULE_ID_A_KERR, "a_kerr_s5k4h8", "a_kerr_s5k4h8", NULL},
};

#define GC5025_SENSOR_INFO_MODULE_ID_SUNWIN		0x56
#define GC5025_SENSOR_INFO_MODULE_ID_SHINETECH	0x58
MODULE_Map_Table GC5025_MODULE_MAP[] = {
	{GC5025_SENSOR_INFO_MODULE_ID_SUNWIN, "sunwin_gc5025", "sunwin_gc5025", NULL},
	{GC5025_SENSOR_INFO_MODULE_ID_SHINETECH, "shinetech_gc5025", "shinetech_gc5025", NULL},
};
DEFINE_MSM_MUTEX(msm_eeprom_mutex);
#ifdef CONFIG_COMPAT
static struct v4l2_file_operations msm_eeprom_v4l2_subdev_fops;
#endif
#if 0
/**
  * msm_eeprom_verify_sum - verify crc32 checksum
  * @mem:	data buffer
  * @size:	size of data buffer
  * @sum:	expected checksum
  *
  * Returns 0 if checksum match, -EINVAL otherwise.
  */
static int msm_eeprom_verify_sum(const char *mem, uint32_t size, uint32_t sum)
{
	uint32_t crc = ~0;

	/* check overflow */
	if (size > crc - sizeof(uint32_t))
		return -EINVAL;

	crc = crc32_le(crc, mem, size);
	if (~crc != sum) {
		CDBG("%s: expect 0x%x, result 0x%x\n", __func__, sum, ~crc);
		return -EINVAL;
	}
	CDBG("%s: checksum pass 0x%x\n", __func__, sum);
	return 0;
}
/**
  * msm_eeprom_match_crc - verify multiple regions using crc
  * @data:	data block to be verified
  *
  * Iterates through all regions stored in @data.  Regions with odd index
  * are treated as data, and its next region is treated as checksum.  Thus
  * regions of even index must have valid_size of 4 or 0 (skip verification).
  * Returns a bitmask of verified regions, starting from LSB.  1 indicates
  * a checksum match, while 0 indicates checksum mismatch or not verified.
  */
static uint32_t msm_eeprom_match_crc(struct msm_eeprom_memory_block_t *data)
{
	int j, rc;
	uint32_t *sum;
	uint32_t ret = 0;
	uint8_t *memptr;
	struct msm_eeprom_memory_map_t *map;

	if (!data) {
		pr_err("%s data is NULL", __func__);
		return -EINVAL;
	}
	map = data->map;
	memptr = data->mapdata;

	for (j = 0; j + 1 < data->num_map; j += 2) {
		/* empty table or no checksum */
		if (!map[j].mem.valid_size || !map[j + 1].mem.valid_size) {
			memptr += map[j].mem.valid_size
					  + map[j + 1].mem.valid_size;
			continue;
		}
		if (map[j + 1].mem.valid_size != sizeof(uint32_t)) {
			CDBG("%s: malformatted data mapping\n", __func__);
			return -EINVAL;
		}
		sum = (uint32_t *) (memptr + map[j].mem.valid_size);
		rc = msm_eeprom_verify_sum(memptr, map[j].mem.valid_size,
								   *sum);
		if (!rc)
			ret |= 1 << (j / 2);
		memptr += map[j].mem.valid_size + map[j + 1].mem.valid_size;
	}
	return ret;
}
#endif
/*
  * add  camera sensor engineering mode  show module_name
  *
  * by ZTE_WQW_20151204 weiqiwei
  */
/*
char *msm_eeprom_get_post_sensor_module_name(void)
{
	return post_sensor_module_name;
}*/
/*EXPORT_SYMBOL(msm_eeprom_get_post_sensor_module_name);*/

static int msm_eeprom_get_cmm_data(struct msm_eeprom_ctrl_t *e_ctrl,
								   struct msm_eeprom_cfg_data *cdata)
{
	int rc = 0;
	struct msm_eeprom_cmm_t *cmm_data = &e_ctrl->eboard_info->cmm_data;

	cdata->cfg.get_cmm_data.cmm_support = cmm_data->cmm_support;
	cdata->cfg.get_cmm_data.cmm_compression = cmm_data->cmm_compression;
	cdata->cfg.get_cmm_data.cmm_size = cmm_data->cmm_size;
	return rc;
}

static int eeprom_config_read_cal_data(struct msm_eeprom_ctrl_t *e_ctrl,
									   struct msm_eeprom_cfg_data *cdata)
{
	int rc;

	/* check range */
	if (cdata->cfg.read_data.num_bytes >
		e_ctrl->cal_data.num_data) {
		CDBG("%s: Invalid size. exp %u, req %u\n", __func__,
			 e_ctrl->cal_data.num_data,
			 cdata->cfg.read_data.num_bytes);
		return -EINVAL;
	}
	if (!e_ctrl->cal_data.mapdata)
		return -EFAULT;
	rc = copy_to_user(cdata->cfg.read_data.dbuffer,
					  e_ctrl->cal_data.mapdata,
					  cdata->cfg.read_data.num_bytes);

	return rc;
}

static int msm_eeprom_config(struct msm_eeprom_ctrl_t *e_ctrl,
							 void __user *argp)
{
	struct msm_eeprom_cfg_data *cdata =
		(struct msm_eeprom_cfg_data *)argp;
	int rc = 0;

	CDBG("%s E\n", __func__);
	switch (cdata->cfgtype) {
	case CFG_EEPROM_GET_INFO:
		CDBG("%s E CFG_EEPROM_GET_INFO\n", __func__);
		cdata->is_supported = e_ctrl->is_supported;
		/*
		* Post compatible module info to vendor.by FENGYUAO_20150528.
		*/

		memcpy(cdata->chromtix_lib_name,
			   e_ctrl->zte_post_chromtix_lib_name,
			   sizeof(cdata->chromtix_lib_name));
		pr_err("%s cdata->chromtix_lib_name %s\n", __func__, cdata->chromtix_lib_name);
		memcpy(cdata->sensor_module_name,
			   e_ctrl->zte_post_sensor_module_name,
			   sizeof(cdata->sensor_module_name));
		memcpy(cdata->default_chromtix_lib_name,
			   e_ctrl->zte_post_default_chromtix_lib_name,
			   sizeof(cdata->default_chromtix_lib_name));
		/* end */

		memcpy(cdata->cfg.eeprom_name,
			   e_ctrl->eboard_info->eeprom_name,
			   sizeof(cdata->cfg.eeprom_name));
		break;
	case CFG_EEPROM_GET_CAL_DATA:
		CDBG("%s E CFG_EEPROM_GET_CAL_DATA\n", __func__);
		cdata->cfg.get_data.num_bytes =
			e_ctrl->cal_data.num_data;
		break;
	case CFG_EEPROM_READ_CAL_DATA:
		CDBG("%s E CFG_EEPROM_READ_CAL_DATA\n", __func__);
		rc = eeprom_config_read_cal_data(e_ctrl, cdata);
		break;
	case CFG_EEPROM_GET_MM_INFO:
		CDBG("%s E CFG_EEPROM_GET_MM_INFO\n", __func__);
		rc = msm_eeprom_get_cmm_data(e_ctrl, cdata);
		break;
	default:
		break;
	}

	CDBG("%s X rc: %d\n", __func__, rc);
	return rc;
}

static int msm_eeprom_get_subdev_id(struct msm_eeprom_ctrl_t *e_ctrl,
									void *arg)
{
	uint32_t *subdev_id = (uint32_t *)arg;

	CDBG("%s E\n", __func__);
	if (!subdev_id) {
		pr_err("%s failed\n", __func__);
		return -EINVAL;
	}
	*subdev_id = e_ctrl->subdev_id;
	CDBG("subdev_id %d\n", *subdev_id);
	CDBG("%s X\n", __func__);
	return 0;
}

static long msm_eeprom_subdev_ioctl(struct v4l2_subdev *sd,
									unsigned int cmd, void *arg)
{
	struct msm_eeprom_ctrl_t *e_ctrl = v4l2_get_subdevdata(sd);
	void __user *argp = (void __user *)arg;

	CDBG("%s E\n", __func__);
	CDBG("%s:%d a_ctrl %p argp %p\n", __func__, __LINE__, e_ctrl, argp);
	switch (cmd) {
	case VIDIOC_MSM_SENSOR_GET_SUBDEV_ID:
		return msm_eeprom_get_subdev_id(e_ctrl, argp);
	case VIDIOC_MSM_EEPROM_CFG:
		return msm_eeprom_config(e_ctrl, argp);
	default:
		return -ENOIOCTLCMD;
	}

	CDBG("%s X\n", __func__);
}

static struct msm_camera_i2c_fn_t msm_eeprom_qup_func_tbl = {
	.i2c_read = msm_camera_qup_i2c_read,
	.i2c_read_seq = msm_camera_qup_i2c_read_seq,
	.i2c_write = msm_camera_qup_i2c_write,
	.i2c_write_table = msm_camera_qup_i2c_write_table,
	.i2c_write_seq_table = msm_camera_qup_i2c_write_seq_table,
	.i2c_write_table_w_microdelay =
	msm_camera_qup_i2c_write_table_w_microdelay,
};

static int msm_eeprom_open(struct v4l2_subdev *sd,
						   struct v4l2_subdev_fh *fh)
{
	int rc = 0;
	struct msm_eeprom_ctrl_t *e_ctrl =  v4l2_get_subdevdata(sd);

	CDBG("%s E\n", __func__);
	if (!e_ctrl) {
		pr_err("%s failed e_ctrl is NULL\n", __func__);
		return -EINVAL;
	}
	CDBG("%s X\n", __func__);
	return rc;
}

static int msm_eeprom_close(struct v4l2_subdev *sd,
							struct v4l2_subdev_fh *fh)
{
	int rc = 0;
	struct msm_eeprom_ctrl_t *e_ctrl =  v4l2_get_subdevdata(sd);

	CDBG("%s E\n", __func__);
	if (!e_ctrl) {
		pr_err("%s failed e_ctrl is NULL\n", __func__);
		return -EINVAL;
	}
	CDBG("%s X\n", __func__);
	return rc;
}

static const struct v4l2_subdev_internal_ops msm_eeprom_internal_ops = {
	.open = msm_eeprom_open,
	.close = msm_eeprom_close,
};
#if 0
/**
  * read_eeprom_memory() - read map data into buffer
  * @e_ctrl:	eeprom control struct
  * @block:	block to be read
  *
  * This function iterates through blocks stored in block->map, reads each
  * region and concatenate them into the pre-allocated block->mapdata
  */
static int read_eeprom_memory(struct msm_eeprom_ctrl_t *e_ctrl,
							  struct msm_eeprom_memory_block_t *block)
{
	int rc = 0;
	int j;
	struct msm_eeprom_memory_map_t *emap = block->map;
	struct msm_eeprom_board_info *eb_info;
	uint8_t *memptr = block->mapdata;

	if (!e_ctrl) {
		pr_err("%s e_ctrl is NULL", __func__);
		return -EINVAL;
	}

	eb_info = e_ctrl->eboard_info;

	for (j = 0; j < block->num_map; j++) {
		if (emap[j].saddr.addr) {
			eb_info->i2c_slaveaddr = emap[j].saddr.addr;
			e_ctrl->i2c_client.cci_client->sid =
				eb_info->i2c_slaveaddr >> 1;
			pr_err("qcom,slave-addr = 0x%X\n",
				   eb_info->i2c_slaveaddr);
		}

		if (emap[j].page.valid_size) {
			e_ctrl->i2c_client.addr_type = emap[j].page.addr_t;
			rc = e_ctrl->i2c_client.i2c_func_tbl->i2c_write(
					 &(e_ctrl->i2c_client), emap[j].page.addr,
					 emap[j].page.data, emap[j].page.data_t);
			msleep(emap[j].page.delay);
			if (rc < 0) {
				pr_err("%s: page write failed\n", __func__);
				return rc;
			}
		}
		if (emap[j].pageen.valid_size) {
			e_ctrl->i2c_client.addr_type = emap[j].pageen.addr_t;
			rc = e_ctrl->i2c_client.i2c_func_tbl->i2c_write(
					 &(e_ctrl->i2c_client), emap[j].pageen.addr,
					 emap[j].pageen.data, emap[j].pageen.data_t);
			msleep(emap[j].pageen.delay);
			if (rc < 0) {
				pr_err("%s: page enable failed\n", __func__);
				return rc;
			}
		}
		if (emap[j].poll.valid_size) {
			e_ctrl->i2c_client.addr_type = emap[j].poll.addr_t;
			rc = e_ctrl->i2c_client.i2c_func_tbl->i2c_poll(
					 &(e_ctrl->i2c_client), emap[j].poll.addr,
					 emap[j].poll.data, emap[j].poll.data_t);
			msleep(emap[j].poll.delay);
			if (rc < 0) {
				pr_err("%s: poll failed\n", __func__);
				return rc;
			}
		}

		if (emap[j].mem.valid_size) {
			e_ctrl->i2c_client.addr_type = emap[j].mem.addr_t;
			rc = e_ctrl->i2c_client.i2c_func_tbl->i2c_read_seq(
					 &(e_ctrl->i2c_client), emap[j].mem.addr,
					 memptr, emap[j].mem.valid_size);
			if (rc < 0) {
				pr_err("%s: read failed\n", __func__);
				return rc;
			}
			memptr += emap[j].mem.valid_size;
		}
		if (emap[j].pageen.valid_size) {
			e_ctrl->i2c_client.addr_type = emap[j].pageen.addr_t;
			rc = e_ctrl->i2c_client.i2c_func_tbl->i2c_write(
					 &(e_ctrl->i2c_client), emap[j].pageen.addr,
					 0, emap[j].pageen.data_t);
			if (rc < 0) {
				pr_err("%s: page disable failed\n", __func__);
				return rc;
			}
		}
	}
	return rc;
}

/**
  * msm_eeprom_parse_memory_map() - parse memory map in device node
  * @of:	device node
  * @data:	memory block for output
  *
  * This functions parses @of to fill @data.  It allocates map itself, parses
  * the @of node, calculate total data length, and allocates required buffer.
  * It only fills the map, but does not perform actual reading.
  */
static int msm_eeprom_parse_memory_map(struct device_node *of,
									   struct msm_eeprom_memory_block_t *data)
{
	int i, rc = 0;
	char property[PROPERTY_MAXSIZE];
	uint32_t count = 6;
	struct msm_eeprom_memory_map_t *map;

	snprintf(property, PROPERTY_MAXSIZE, "qcom,num-blocks");
	rc = of_property_read_u32(of, property, &data->num_map);
	CDBG("%s: %s %d\n", __func__, property, data->num_map);
	if (rc < 0) {
		pr_err("%s failed rc %d\n", __func__, rc);
		return rc;
	}

	map = kzalloc((sizeof(*map) * data->num_map), GFP_KERNEL);
	if (!map) {
		pr_err("%s failed line %d\n", __func__, __LINE__);
		return -ENOMEM;
	}
	data->map = map;

	for (i = 0; i < data->num_map; i++) {
		snprintf(property, PROPERTY_MAXSIZE, "qcom,page%d", i);
		rc = of_property_read_u32_array(of, property,
										(uint32_t *) &map[i].page, count);
		if (rc < 0) {
			pr_err("%s: failed %d\n", __func__, __LINE__);
			goto ERROR;
		}

		snprintf(property, PROPERTY_MAXSIZE,
				 "qcom,pageen%d", i);
		rc = of_property_read_u32_array(of, property,
										(uint32_t *) &map[i].pageen, count);
		if (rc < 0)
			pr_err("%s: pageen not needed\n", __func__);

		snprintf(property, PROPERTY_MAXSIZE, "qcom,saddr%d", i);
		rc = of_property_read_u32_array(of, property,
										(uint32_t *) &map[i].saddr.addr, 1);
		if (rc < 0)
			CDBG("%s: saddr not needed - block %d\n", __func__, i);

		snprintf(property, PROPERTY_MAXSIZE, "qcom,poll%d", i);
		rc = of_property_read_u32_array(of, property,
										(uint32_t *) &map[i].poll, count);
		if (rc < 0) {
			pr_err("%s failed %d\n", __func__, __LINE__);
			goto ERROR;
		}

		snprintf(property, PROPERTY_MAXSIZE, "qcom,mem%d", i);
		rc = of_property_read_u32_array(of, property,
										(uint32_t *) &map[i].mem, count);
		if (rc < 0) {
			pr_err("%s failed %d\n", __func__, __LINE__);
			goto ERROR;
		}
		data->num_data += map[i].mem.valid_size;
	}

	CDBG("%s num_bytes %d\n", __func__, data->num_data);

	data->mapdata = kzalloc(data->num_data, GFP_KERNEL);
	if (!data->mapdata) {
		pr_err("%s failed line %d\n", __func__, __LINE__);
		rc = -ENOMEM;
		goto ERROR;
	}
	return rc;

ERROR:
	kfree(data->map);
	memset(data, 0, sizeof(*data));
	return rc;
}
#endif
static struct msm_cam_clk_info cam_8960_clk_info[] = {
	[SENSOR_CAM_MCLK] = {"cam_clk", 24000000},
};

static struct v4l2_subdev_core_ops msm_eeprom_subdev_core_ops = {
	.ioctl = msm_eeprom_subdev_ioctl,
};

static struct v4l2_subdev_ops msm_eeprom_subdev_ops = {
	.core = &msm_eeprom_subdev_core_ops,
};

static int msm_eeprom_get_dt_data(struct msm_eeprom_ctrl_t *e_ctrl)
{
	int rc = 0, i = 0;
	struct msm_eeprom_board_info *eb_info;
	struct msm_camera_power_ctrl_t *power_info =
			&e_ctrl->eboard_info->power_info;
	struct device_node *of_node = NULL;
	struct msm_camera_gpio_conf *gconf = NULL;
	uint16_t gpio_array_size = 0;
	uint16_t *gpio_array = NULL;

	eb_info = e_ctrl->eboard_info;
	if (e_ctrl->eeprom_device_type == MSM_CAMERA_SPI_DEVICE)
		of_node = e_ctrl->i2c_client.
				  spi_client->spi_master->dev.of_node;
	else if (e_ctrl->eeprom_device_type == MSM_CAMERA_PLATFORM_DEVICE)
		of_node = e_ctrl->pdev->dev.of_node;
	else if (e_ctrl->eeprom_device_type == MSM_CAMERA_I2C_DEVICE)
		of_node = e_ctrl->i2c_client.client->dev.of_node;

	if (!of_node) {
		pr_err("%s: %d of_node is NULL\n", __func__, __LINE__);
		return -ENOMEM;
	}
	rc = msm_camera_get_dt_vreg_data(of_node, &power_info->cam_vreg,
									 &power_info->num_vreg);
	if (rc < 0)
		return rc;

	rc = msm_camera_get_dt_power_setting_data(of_node,
			power_info->cam_vreg, power_info->num_vreg,
			power_info);
	if (rc < 0)
		goto ERROR1;

	power_info->gpio_conf = kzalloc(sizeof(struct msm_camera_gpio_conf),
									GFP_KERNEL);
	if (!power_info->gpio_conf) {
		rc = -ENOMEM;
		goto ERROR2;
	}
	gconf = power_info->gpio_conf;
	gpio_array_size = of_gpio_count(of_node);
	CDBG("%s gpio count %d\n", __func__, gpio_array_size);

	if (gpio_array_size) {
		gpio_array = kcalloc(1, sizeof(uint16_t) * gpio_array_size,
							 GFP_KERNEL);
		if (!gpio_array) {
			pr_err("%s failed %d\n", __func__, __LINE__);
			goto ERROR3;
		}
		for (i = 0; i < gpio_array_size; i++) {
			gpio_array[i] = of_get_gpio(of_node, i);
			CDBG("%s gpio_array[%d] = %d\n", __func__, i,
				 gpio_array[i]);
		}

		rc = msm_camera_get_dt_gpio_req_tbl(of_node, gconf,
											gpio_array, gpio_array_size);
		if (rc < 0) {
			pr_err("%s failed %d\n", __func__, __LINE__);
			goto ERROR4;
		}

		rc = msm_camera_init_gpio_pin_tbl(of_node, gconf,
										  gpio_array, gpio_array_size);
		if (rc < 0) {
			pr_err("%s failed %d\n", __func__, __LINE__);
			goto ERROR4;
		}
		kfree(gpio_array);
	}

	return rc;
ERROR4:
	kfree(gpio_array);
ERROR3:
	kfree(power_info->gpio_conf);
ERROR2:
	kfree(power_info->cam_vreg);
ERROR1:
	kfree(power_info->power_setting);
	return rc;
}


static int msm_eeprom_cmm_dts(struct msm_eeprom_board_info *eb_info,
							  struct device_node *of_node)
{
	int rc = 0;
	struct msm_eeprom_cmm_t *cmm_data = &eb_info->cmm_data;

	cmm_data->cmm_support =
		of_property_read_bool(of_node, "qcom,cmm-data-support");
	if (!cmm_data->cmm_support)
		return -EINVAL;
	cmm_data->cmm_compression =
		of_property_read_bool(of_node, "qcom,cmm-data-compressed");
	if (!cmm_data->cmm_compression)
		CDBG("No MM compression data\n");

	rc = of_property_read_u32(of_node, "qcom,cmm-data-offset",
							  &cmm_data->cmm_offset);
	if (rc < 0)
		CDBG("No MM offset data\n");

	rc = of_property_read_u32(of_node, "qcom,cmm-data-size",
							  &cmm_data->cmm_size);
	if (rc < 0)
		CDBG("No MM size data\n");

	CDBG("cmm_support: cmm_compr %d, cmm_offset %d, cmm_size %d\n",
		 cmm_data->cmm_compression,
		 cmm_data->cmm_offset,
		 cmm_data->cmm_size);
	return 0;
}

#ifdef CONFIG_COMPAT
static int eeprom_config_read_cal_data32(struct msm_eeprom_ctrl_t *e_ctrl,
		void __user *arg)
{
	int rc;
	uint8_t *ptr_dest = NULL;
	struct msm_eeprom_cfg_data32 *cdata32 =
		(struct msm_eeprom_cfg_data32 *) arg;
	struct msm_eeprom_cfg_data cdata;

	cdata.cfgtype = cdata32->cfgtype;
	cdata.is_supported = cdata32->is_supported;
	cdata.cfg.read_data.num_bytes = cdata32->cfg.read_data.num_bytes;
	/* check range */
	if (cdata.cfg.read_data.num_bytes >
		e_ctrl->cal_data.num_data) {
		CDBG("%s: Invalid size. exp %u, req %u\n", __func__,
			 e_ctrl->cal_data.num_data,
			 cdata.cfg.read_data.num_bytes);
		return -EINVAL;
	}
	if (!e_ctrl->cal_data.mapdata)
		return -EFAULT;

	ptr_dest = (uint8_t *) compat_ptr(cdata32->cfg.read_data.dbuffer);

	rc = copy_to_user(ptr_dest, e_ctrl->cal_data.mapdata,
					  cdata.cfg.read_data.num_bytes);

	/* should only be called once.  free kernel resource */
	if (!rc) {
		kfree(e_ctrl->cal_data.mapdata);
		kfree(e_ctrl->cal_data.map);
		memset(&e_ctrl->cal_data, 0, sizeof(e_ctrl->cal_data));
	}
	return rc;
}

static int msm_eeprom_config32(struct msm_eeprom_ctrl_t *e_ctrl,
							   void __user *argp)
{
	struct msm_eeprom_cfg_data *cdata = (struct msm_eeprom_cfg_data *)argp;
	int rc = 0;

	CDBG("%s E\n", __func__);
	switch (cdata->cfgtype) {
	case CFG_EEPROM_GET_INFO:
		CDBG("%s E CFG_EEPROM_GET_INFO\n", __func__);
		cdata->is_supported = e_ctrl->is_supported;
		/*
		* Post compatible module info to vendor.by FENGYUAO_20150528.
		*/
		memcpy(cdata->chromtix_lib_name,
			   e_ctrl->zte_post_chromtix_lib_name,
			   sizeof(cdata->chromtix_lib_name));

		memcpy(cdata->sensor_module_name,
			   e_ctrl->zte_post_sensor_module_name,
			   sizeof(cdata->sensor_module_name));

		memcpy(cdata->default_chromtix_lib_name,
			   e_ctrl->zte_post_default_chromtix_lib_name,
			   sizeof(cdata->default_chromtix_lib_name));
		/* end */

		memcpy(cdata->cfg.eeprom_name,
			   e_ctrl->eboard_info->eeprom_name,
			   sizeof(cdata->cfg.eeprom_name));
		break;
	case CFG_EEPROM_GET_CAL_DATA:
		CDBG("%s E CFG_EEPROM_GET_CAL_DATA\n", __func__);
		cdata->cfg.get_data.num_bytes =
			e_ctrl->cal_data.num_data;
		break;
	case CFG_EEPROM_READ_CAL_DATA:
		CDBG("%s E CFG_EEPROM_READ_CAL_DATA\n", __func__);
		rc = eeprom_config_read_cal_data32(e_ctrl, argp);
		break;
	default:
		break;
	}

	CDBG("%s X rc: %d\n", __func__, rc);
	return rc;
}

static long msm_eeprom_subdev_ioctl32(struct v4l2_subdev *sd,
									  unsigned int cmd, void *arg)
{
	struct msm_eeprom_ctrl_t *e_ctrl = v4l2_get_subdevdata(sd);
	void __user *argp = (void __user *)arg;

	CDBG("%s E\n", __func__);
	CDBG("%s:%d a_ctrl %p argp %p\n", __func__, __LINE__, e_ctrl, argp);
	switch (cmd) {
	case VIDIOC_MSM_SENSOR_GET_SUBDEV_ID:
		return msm_eeprom_get_subdev_id(e_ctrl, argp);
	case VIDIOC_MSM_EEPROM_CFG32:
		return msm_eeprom_config32(e_ctrl, argp);
	default:
		return -ENOIOCTLCMD;
	}

	CDBG("%s X\n", __func__);
}

static long msm_eeprom_subdev_do_ioctl32(
	struct file *file, unsigned int cmd, void *arg)
{
	struct video_device *vdev = video_devdata(file);
	struct v4l2_subdev *sd = vdev_to_v4l2_subdev(vdev);

	return msm_eeprom_subdev_ioctl32(sd, cmd, arg);
}

static long msm_eeprom_subdev_fops_ioctl32(struct file *file, unsigned int cmd,
		unsigned long arg)
{
	return video_usercopy(file, cmd, arg, msm_eeprom_subdev_do_ioctl32);
}

#endif


int ar0542_check_block_status(struct msm_eeprom_ctrl_t *e_ctrl, int block_type)
{
	uint16_t i = 0;
	uint16_t temp = 0;

	e_ctrl->i2c_client.i2c_func_tbl->i2c_write(&(e_ctrl->i2c_client), 0x304C, (block_type & 0xff) << 8,
			MSM_CAMERA_I2C_WORD_DATA);
	e_ctrl->i2c_client.i2c_func_tbl->i2c_write(&(e_ctrl->i2c_client), 0x304A, 0x0200,
			MSM_CAMERA_I2C_WORD_DATA);
	e_ctrl->i2c_client.i2c_func_tbl->i2c_write(&(e_ctrl->i2c_client), 0x304A, 0x0010,
			MSM_CAMERA_I2C_WORD_DATA);
	do {
		e_ctrl->i2c_client.i2c_func_tbl->i2c_read(&(e_ctrl->i2c_client), 0x304A, &temp,
				MSM_CAMERA_I2C_WORD_DATA);
		if (0x60 == (temp & 0x60)) {
			pr_err("%s: read success\n", __func__);
			break;
		}
		usleep_range(5000, 5100);
		i++;
	} while (i < 10);
	e_ctrl->i2c_client.i2c_func_tbl->i2c_read(&(e_ctrl->i2c_client), 0x3800, &temp,
			MSM_CAMERA_I2C_WORD_DATA);
	pr_err("%s: %d   = %d\n", __func__, block_type, temp);
	return temp;
}

/*
  * camera sensor module compatile
  *
  * by ZTE_YCM_20140728 yi.changming 000028
  */
static int lookupIndexByid(MODULE_Map_Table arr[], int len, uint16_t value)
{
	int i = 0;

	for (i = 0; i < len; i++) {
		if (arr[i].id == value) {
			return i;
		}
	}
	return ZTE_EEPROM_ERROR;
}
static void parse_module_name(struct msm_eeprom_ctrl_t *e_ctrl,
		 MODULE_Map_Table *map, uint16_t len, uint16_t  sensor_module_id)
{
	int index = lookupIndexByid(map, len, sensor_module_id);

	if (index != -1) {
		e_ctrl->sensor_module_name = map[index].sensor_module_name;
		/*
		* Post compatible module info to vendor.by FENGYUAO_20150528.
		*/
		if (map && (map[index].sensor_module_name)) {
			if (strlen(map[index].sensor_module_name) <  SENSOR_NAME_MAX_SIZE)
				strlcpy(e_ctrl->zte_post_sensor_module_name,  map[index].sensor_module_name,
					strlen(map[index].sensor_module_name) + 1);

			pr_err("ZTE_CAMERA:%s:%d: sensor_module_name = %s\n",
				   __func__, __LINE__, e_ctrl->zte_post_sensor_module_name);
		}
		/* end */

		e_ctrl->chromtix_lib_name = map[index].chromtix_lib_name;
		/*
		* Post compatible module info to vendor.by hujian _20170817.
		*/
		if (map && (map[index].chromtix_lib_name)) {
			if (strlen(map[index].chromtix_lib_name) <  SENSOR_NAME_MAX_SIZE)
				strlcpy(e_ctrl->zte_post_chromtix_lib_name, map[index].chromtix_lib_name,
					strlen(map[index].chromtix_lib_name) + 1);

			pr_err("ZTE_CAMERA:%s:%d: chromtix_lib_name = %s\n",
				   __func__, __LINE__, e_ctrl->zte_post_chromtix_lib_name);
		}
		/* end */

		e_ctrl->default_chromtix_lib_name = map[index].default_chromtix_lib_name;
		/*
		* Post compatible module info to vendor.by FENGYUAO_20150528.
		*/
		if (map && (map[index].default_chromtix_lib_name)) {
			if (strlen(map[index].default_chromtix_lib_name) <  SENSOR_NAME_MAX_SIZE)
				strlcpy(e_ctrl->zte_post_default_chromtix_lib_name,
					map[index].default_chromtix_lib_name,
						strlen(map[index].default_chromtix_lib_name) + 1);
			pr_err("ZTE_CAMERA:%s:%d: default_chromtix_lib_name = %s\n",
				   __func__, __LINE__, e_ctrl->zte_post_default_chromtix_lib_name);
		}
		/* end */

		pr_err("ZTE_CAMERA:%s:%d: sensor_module_name = %s\n",
			   __func__, __LINE__, e_ctrl->sensor_module_name);
	}
}

static int ar0542_read_eeprom_memory(struct msm_eeprom_ctrl_t *e_ctrl,
									 struct msm_eeprom_memory_block_t *block)
{
	int rc = 0;
	int found = 0;
	struct msm_eeprom_memory_map_t *emap = block->map;
	struct msm_eeprom_board_info *eb_info;
	uint8_t *memptr = block->mapdata;
	/*
	  * camera sensor module compatile
	  *
	  * by ZTE_YCM_20140728 yi.changming 000028
	  */

	uint16_t  sensor_module_id = 0;

	if (!e_ctrl) {
		pr_err("%s e_ctrl is NULL", __func__);
		return -EINVAL;
	}
	eb_info = e_ctrl->eboard_info;
	e_ctrl->i2c_client.addr_type = MSM_CAMERA_I2C_WORD_ADDR;
	e_ctrl->i2c_client.i2c_func_tbl->i2c_write(&(e_ctrl->i2c_client), 0x301A, 0x0610, MSM_CAMERA_I2C_WORD_DATA);
	e_ctrl->i2c_client.i2c_func_tbl->i2c_write(&(e_ctrl->i2c_client), 0x3134, 0xCD95, MSM_CAMERA_I2C_WORD_DATA);
	if (ar0542_check_block_status(e_ctrl, 0x31) == 0x0002) {
		found = 1;
	} else if (ar0542_check_block_status(e_ctrl, 0x30) == 0x0001) {
		found = 1;
	} else {
		found = 0;
		pr_err("%s:%d: read failed\n", __func__, __LINE__);
		return ZTE_EEPROM_ERROR;
	}
	if (found == 1) {
		e_ctrl->i2c_client.addr_type = emap[0].mem.addr_t;
		rc = e_ctrl->i2c_client.i2c_func_tbl->i2c_read_seq(
				 &(e_ctrl->i2c_client), emap[0].mem.addr,
				 memptr, emap[0].mem.valid_size);
		if (rc < 0) {
			pr_err("%s:%d: read failed\n", __func__, __LINE__);
			return rc;
		}
		memptr += emap[0].mem.valid_size;
	}

	/*
	  * camera sensor module compatile
	  *
	  * by ZTE_YCM_20140728 yi.changming 000028
	  */
	sensor_module_id = (block->mapdata[0] << 8) | block->mapdata[1];

	parse_module_name(e_ctrl, AR0542_MODULE_MAP,
					  sizeof(AR0542_MODULE_MAP) / sizeof(MODULE_Map_Table), sensor_module_id);
	found = 0;
	e_ctrl->i2c_client.addr_type = MSM_CAMERA_I2C_WORD_ADDR;
	if (ar0542_check_block_status(e_ctrl, 0x33) == 0x0002) {
		found = 1;
	} else if (ar0542_check_block_status(e_ctrl, 0x32) == 0x0001) {
		found = 1;
	} else {
		found = 0;
		pr_err("%s:%d: read failed\n", __func__, __LINE__);
		return ZTE_EEPROM_ERROR;
	}

	if (found == 1) {
		e_ctrl->i2c_client.addr_type = emap[1].mem.addr_t;
		rc = e_ctrl->i2c_client.i2c_func_tbl->i2c_read_seq(
				 &(e_ctrl->i2c_client), emap[1].mem.addr,
				 memptr, emap[1].mem.valid_size);
		if (rc < 0) {
			pr_err("%s:%d: read failed\n", __func__, __LINE__);
			return rc;
		}
		memptr += emap[1].mem.valid_size;

		e_ctrl->i2c_client.addr_type = emap[2].mem.addr_t;
		rc = e_ctrl->i2c_client.i2c_func_tbl->i2c_read_seq(
				 &(e_ctrl->i2c_client), emap[2].mem.addr,
				 memptr, emap[2].mem.valid_size);
		if (rc < 0) {
			pr_err("%s:%d: read failed\n", __func__, __LINE__);
			return rc;
		}
		memptr += emap[2].mem.valid_size;

		e_ctrl->i2c_client.addr_type = emap[3].mem.addr_t;
		rc = e_ctrl->i2c_client.i2c_func_tbl->i2c_read_seq(
				 &(e_ctrl->i2c_client), emap[3].mem.addr,
				 memptr, emap[3].mem.valid_size);
		if (rc < 0) {
			pr_err("%s:%d: read failed\n", __func__, __LINE__);
			return rc;
		}
		memptr += emap[3].mem.valid_size;
	}

	return rc;
}

/*
  * add t4k35 eeprom driver
  *
  * by ZTE_YCM_20140911 yi.changming 000064
  */
#define  T4K35_PAGE_LEN 64
#define  T4k35_OTP_DATA_BEGIN_ADDR 0x3504

void t4k35_enable_read_mode(struct msm_eeprom_ctrl_t *e_ctrl, int enable)
{
	if (enable)
		e_ctrl->i2c_client.i2c_func_tbl->i2c_write(&(e_ctrl->i2c_client), 0x3500, 0x01,
				MSM_CAMERA_I2C_BYTE_DATA);
	else
		e_ctrl->i2c_client.i2c_func_tbl->i2c_write(&(e_ctrl->i2c_client), 0x3500, 0x00,
				MSM_CAMERA_I2C_BYTE_DATA);
}

void t4k35_set_page(struct msm_eeprom_ctrl_t *e_ctrl, uint16_t page_number)
{

	e_ctrl->i2c_client.i2c_func_tbl->i2c_write(&(e_ctrl->i2c_client), 0x3502, page_number,
			MSM_CAMERA_I2C_BYTE_DATA);

}

void t4k35_check_start_acces(struct msm_eeprom_ctrl_t *e_ctrl)
{
	uint16_t temp;

	e_ctrl->i2c_client.i2c_func_tbl->i2c_read(&(e_ctrl->i2c_client), 0x3500, &temp,
			MSM_CAMERA_I2C_BYTE_DATA);
	e_ctrl->i2c_client.i2c_func_tbl->i2c_write(&(e_ctrl->i2c_client), 0x3500, temp | 0x80,
			MSM_CAMERA_I2C_BYTE_DATA);
	usleep(30);
}

int32_t t4k35_read_page_data(struct msm_eeprom_ctrl_t *e_ctrl,
							 struct msm_eeprom_memory_map_t *emap, uint8_t *buff)
{
	int rc = 0;

	e_ctrl->i2c_client.addr_type = emap->mem.addr_t;
	rc = e_ctrl->i2c_client.i2c_func_tbl->i2c_read_seq(
			 &(e_ctrl->i2c_client), T4k35_OTP_DATA_BEGIN_ADDR,
			 buff, T4K35_PAGE_LEN);
	if (rc < 0) {
		pr_err("%s:%d: read failed\n", __func__, __LINE__);
		return rc;
	}

	return 0;
}

void t4k35_read_page_and_back_page_data(struct msm_eeprom_ctrl_t *e_ctrl,
		struct msm_eeprom_memory_map_t *emap, int page, uint8_t *buff_data, uint8_t *bake_buff_data)
{
	int i = 0;

	t4k35_set_page(e_ctrl, page);
	t4k35_check_start_acces(e_ctrl);
	t4k35_read_page_data(e_ctrl, emap, buff_data);
	t4k35_set_page(e_ctrl, page + 6);
	t4k35_check_start_acces(e_ctrl);
	t4k35_read_page_data(e_ctrl, emap, bake_buff_data);
	for (i = 0; i < T4K35_PAGE_LEN; i++) {
		CDBG("%s:page = %d:  buff_data[%d]= %d\n", __func__, page, i, buff_data[i]);
		CDBG("%s:page = %d:  bake_data[%d]= %d\n", __func__, page + 6, i, bake_buff_data[i]);
		buff_data[i] = buff_data[i] | bake_buff_data[i];
	}
}

int32_t t4k35_read_module_info_golden_data(struct msm_eeprom_ctrl_t *e_ctrl,
		struct msm_eeprom_memory_map_t *emap, uint8_t *memptr,
		uint8_t *buff_data, uint8_t *bake_buff_data)
{
	int32_t check_sum = 0;
	uint8_t segment_flag = 0;
	int32_t i = 0;

	CDBG("%s:start\n", __func__);
	t4k35_read_page_and_back_page_data(e_ctrl, emap, 4, buff_data, bake_buff_data);
	CDBG("%s:end\n", __func__);
	if (buff_data[32])
		segment_flag = 32;
	else if (buff_data[0])
		segment_flag = 0;
	else {
		pr_err("otp no module information!\n");
		return ZTE_EEPROM_ERROR;
	}

	for (i = segment_flag + 2; i < segment_flag + T4K35_PAGE_LEN / 2; i++)
		check_sum = check_sum + buff_data[i];


	if ((check_sum & 0xFF) == buff_data[segment_flag + 1]) {
		pr_err("otp module info checksum ok!\n");
		*memptr = 1;
		memptr++;
		for (i = segment_flag + 3; i <= segment_flag + 12; i++) {
			*memptr = buff_data[i];
			memptr++;
		}
		*memptr = 1;
		memptr++;
		for (i = segment_flag + 15; i <= segment_flag + 22; i++) {
			*memptr = buff_data[i];
			memptr++;
		}
		return 0;
	}
	pr_err("otp module info checksum error!\n");
	return ZTE_EEPROM_ERROR;
}

int32_t t4k35_read_lsc_awb_data(struct msm_eeprom_ctrl_t *e_ctrl,
								struct msm_eeprom_memory_map_t *emap, uint8_t *memptr,
								uint8_t *buff_data, uint8_t *bake_buff_data)
{
	int32_t check_sum = 0;
	int32_t i = 0, j = 0;

	for (i = 3; i >= 0; i--) {
		CDBG("%s:start i = %d\n", __func__, i);
		t4k35_read_page_and_back_page_data(e_ctrl, emap, i, buff_data, bake_buff_data);
		CDBG("%s:end i = %d\n", __func__, i);
		if (buff_data[0] == 0) {
			memset(buff_data, 0, T4K35_PAGE_LEN);
			memset(bake_buff_data, 0, T4K35_PAGE_LEN);
			continue;
		} else {
			for (j = 2; j < 64; j++)
				check_sum = check_sum + buff_data[j];

			if ((check_sum & 0xFF) == buff_data[1]) {
				pr_err("otp lsc checksum ok!\n");
				*memptr = 1;
				memptr++;
				for (j = 3; j <= 63; j++) {
					*memptr = buff_data[j];
					memptr++;
				}
				return 0;
			}
			pr_err("otp lsc checksum error!\n");
			*memptr = 0;
			return ZTE_EEPROM_ERROR;
		}
	}

	if (i < 0) {
		pr_err("No otp lsc data on sensor t4k35\n");
		*memptr = 0;
		return ZTE_EEPROM_ERROR;
	}

	return 0;

}

int32_t t4k35_read_af_data(struct msm_eeprom_ctrl_t *e_ctrl,
						   struct msm_eeprom_memory_map_t *emap, uint8_t *memptr,
						   uint8_t *buff_data, uint8_t *bake_buff_data)
{
	int32_t check_sum = 0;
	uint8_t segment_flag = 0;
	int32_t i = 0, j = 0;

	CDBG("%s:start\n", __func__);
	t4k35_read_page_and_back_page_data(e_ctrl, emap, 5, buff_data, bake_buff_data);
	CDBG("%s:end\n", __func__);

	/*
	 *macro AF
	 *check flag
	 */
	if (buff_data[24])
		segment_flag = 24;
	else if (buff_data[16])
		segment_flag = 16;
	else if (buff_data[8])
		segment_flag = 8;
	else if (buff_data[0])
		segment_flag = 0;
	else {
		pr_err("no otp macro AF information!\n");
		*memptr = 0;
		memptr += 3;
		*memptr = 0;
		memptr += 3;
		return ZTE_EEPROM_ERROR;
	}

	for (i = segment_flag + 2; i <= segment_flag + 7; i++) {
		check_sum = check_sum + buff_data[i];
	}

	if ((check_sum & 0xFF) == buff_data[segment_flag + 1]) {
		pr_err("otp macro AF checksum ok!\n");
		*memptr = 1;
		memptr++;
		for (j = segment_flag + 3; j <= segment_flag + 4; j++) {
			*memptr = buff_data[j];
			memptr++;
		}
	} else {
		pr_err("otp macro AF checksum error!\n");
		*memptr = 0;
		memptr += 3;
	}


	/*
	 *inifity AF
	 *check flag
	 */
	if (buff_data[56])
		segment_flag = 56;
	else if (buff_data[48])
		segment_flag = 48;
	else if (buff_data[40])
		segment_flag = 40;
	else if (buff_data[32])
		segment_flag = 32;
	else {
		pr_err("no otp inifity AF information!\n");
		*memptr = 0;
		memptr += 3;
		return ZTE_EEPROM_ERROR;
	}

	check_sum = 0;
	for (i = segment_flag + 2; i <= segment_flag + 7; i++) {
		pr_err("otp inifity AF  %d!\n", buff_data[i]);
		check_sum += buff_data[i];
	}

	if ((check_sum & 0xFF) == buff_data[segment_flag + 1]) {
		pr_err("otp inifity AF checksum ok!\n");
		*memptr = 1;
		memptr++;
		for (j = segment_flag + 4; j <= segment_flag + 5; j++) {
			*memptr = buff_data[j];
			memptr++;
		}
	} else {
		pr_err("otp inifity AF checksum error!\n");
		*memptr = 0;
	}
	return 0;

}

static int t4k35_read_eeprom_memory(struct msm_eeprom_ctrl_t *e_ctrl,
									struct msm_eeprom_memory_block_t *block)
{
	int rc = 0;
	struct msm_eeprom_memory_map_t *emap = block->map;
	struct msm_eeprom_board_info *eb_info;
	uint8_t *memptr = block->mapdata;
	int32_t num_byte = T4K35_PAGE_LEN;
	uint8_t *buff_data;
	uint8_t *bake_buff_data;
	uint16_t  sensor_module_id = 0;

	if (!e_ctrl) {
		pr_err("%s e_ctrl is NULL", __func__);
		return -EINVAL;
	}

	buff_data = kzalloc(num_byte, GFP_KERNEL);
	memset(buff_data, 0, num_byte);
	bake_buff_data = kzalloc(num_byte, GFP_KERNEL);
	memset(bake_buff_data, 0, num_byte);

	eb_info = e_ctrl->eboard_info;
	e_ctrl->i2c_client.addr_type = MSM_CAMERA_I2C_WORD_ADDR;

	t4k35_enable_read_mode(e_ctrl, 1);

	rc = t4k35_read_module_info_golden_data(e_ctrl, &(emap[0]), memptr, buff_data, bake_buff_data);

	if (rc) {
		pr_err("%s:read module information fail\n", __func__);
	} else {
		if (block->mapdata[0] == 1) {
			sensor_module_id = block->mapdata[4];
			parse_module_name(e_ctrl, T4K35_MODULE_MAP,
			  sizeof(T4K35_MODULE_MAP) / sizeof(MODULE_Map_Table), sensor_module_id);
		}
	}
	memset(buff_data, 0, num_byte);
	memset(bake_buff_data, 0, num_byte);

	memptr += emap[0].mem.valid_size;

	rc = t4k35_read_lsc_awb_data(e_ctrl, &(emap[1]), memptr, buff_data, bake_buff_data);
	if (rc) {
		pr_err("%s:read lsc awb info fail\n", __func__);
	}

	memset(buff_data, 0, num_byte);
	memset(bake_buff_data, 0, num_byte);

	memptr += emap[1].mem.valid_size;

	rc = t4k35_read_af_data(e_ctrl, &(emap[2]), memptr, buff_data, bake_buff_data);
	if (rc) {
		pr_err("%s:read af info fail\n", __func__);
	}

	t4k35_enable_read_mode(e_ctrl, 0);

	kfree(buff_data);
	kfree(bake_buff_data);
	return rc;
}

/*
 *add for ov5670 eeprom begin by zte_cam_wxl_20150915
 */
enum {
	Invlid_Group,
	Group_One,
	Group_Two,
	Group_Three,
	Group_Four,
} Group_t;

void ov5670_read_eeprom_init(struct msm_eeprom_ctrl_t *e_ctrl)
{
	uint16_t temp;

	e_ctrl->i2c_client.i2c_func_tbl->i2c_read(&(e_ctrl->i2c_client), 0x5002, &temp,
			MSM_CAMERA_I2C_BYTE_DATA);
	CDBG("%s:0x5002 temp=0x%X", __func__, temp);

	e_ctrl->i2c_client.i2c_func_tbl->i2c_write(&(e_ctrl->i2c_client), 0x0100, 0x01,
			MSM_CAMERA_I2C_BYTE_DATA);

	e_ctrl->i2c_client.i2c_func_tbl->i2c_write(&(e_ctrl->i2c_client), 0x5002, 0x20,
			MSM_CAMERA_I2C_BYTE_DATA);

	e_ctrl->i2c_client.i2c_func_tbl->i2c_write(&(e_ctrl->i2c_client), 0x3d84, 0xc0,
			MSM_CAMERA_I2C_BYTE_DATA);
	e_ctrl->i2c_client.i2c_func_tbl->i2c_read(&(e_ctrl->i2c_client), 0x3d84, &temp,
			MSM_CAMERA_I2C_BYTE_DATA);
	e_ctrl->i2c_client.i2c_func_tbl->i2c_write(&(e_ctrl->i2c_client), 0x3d88, 0x70,
			MSM_CAMERA_I2C_BYTE_DATA);
	e_ctrl->i2c_client.i2c_func_tbl->i2c_write(&(e_ctrl->i2c_client), 0x3d89, 0x10,
			MSM_CAMERA_I2C_BYTE_DATA);
	e_ctrl->i2c_client.i2c_func_tbl->i2c_write(&(e_ctrl->i2c_client), 0x3d8a, 0x70,
			MSM_CAMERA_I2C_BYTE_DATA);
	e_ctrl->i2c_client.i2c_func_tbl->i2c_write(&(e_ctrl->i2c_client), 0x3d8b, 0x29,
			MSM_CAMERA_I2C_BYTE_DATA);
	e_ctrl->i2c_client.i2c_func_tbl->i2c_write(&(e_ctrl->i2c_client), 0x3d81, 0x01,
			MSM_CAMERA_I2C_BYTE_DATA);
	udelay(5);
}

void ov5670_read_eeprom_end(struct msm_eeprom_ctrl_t *e_ctrl)
{
	e_ctrl->i2c_client.i2c_func_tbl->i2c_write(&(e_ctrl->i2c_client), 0x5002, 0x28,
			MSM_CAMERA_I2C_BYTE_DATA);
	e_ctrl->i2c_client.i2c_func_tbl->i2c_write(&(e_ctrl->i2c_client), 0x0100, 0x00,
			MSM_CAMERA_I2C_BYTE_DATA);
}

int32_t ov5670_check_module_info_group(struct msm_eeprom_ctrl_t *e_ctrl)
{
	uint16_t temp;

	e_ctrl->i2c_client.i2c_func_tbl->i2c_read(&(e_ctrl->i2c_client), 0x7010, &temp,
			MSM_CAMERA_I2C_BYTE_DATA);
	CDBG("%s:temp=0x%X", __func__, temp);
	if ((temp & 0xC0) == 0x40) {
		return Group_One;
	} else if ((temp & 0x30) == 0x10) {
		return Group_Two;
	} else if ((temp & 0x0C) == 0x04) {
		return Group_Three;
	}
	return Invlid_Group;
}

static int ov5670_read_eeprom_memory(struct msm_eeprom_ctrl_t *e_ctrl,
									 struct msm_eeprom_memory_block_t *block)
{
	int rc = 0;
	struct msm_eeprom_memory_map_t *emap = block->map;
	struct msm_eeprom_board_info *eb_info;
	uint8_t *memptr = block->mapdata;
	uint16_t  sensor_module_id = 0;
	int32_t group_number;
	uint32_t module_id_addr;
	int i;

	if (!e_ctrl) {
		pr_err("%s e_ctrl is NULL", __func__);
		return -EINVAL;
	}
	CDBG("%s begin", __func__);
	eb_info = e_ctrl->eboard_info;
	e_ctrl->i2c_client.addr_type = MSM_CAMERA_I2C_WORD_ADDR;

	/*init before read eeprom */
	ov5670_read_eeprom_init(e_ctrl);

	group_number = ov5670_check_module_info_group(e_ctrl);
	switch (group_number) {
	case Group_One:
		module_id_addr = 0x7011;
		break;
	case Group_Two:
		module_id_addr = 0x7016;
		break;
	case Group_Three:
		module_id_addr = 0x701b;
		break;
	default:
		break;
	}
	if (module_id_addr != 0) {
		e_ctrl->i2c_client.i2c_func_tbl->i2c_read(&(e_ctrl->i2c_client), module_id_addr, &sensor_module_id,
				MSM_CAMERA_I2C_BYTE_DATA);
		CDBG("sensor_module_id =0x%X\n", sensor_module_id);
		parse_module_name(e_ctrl, OV5670_MODULE_MAP,
				  sizeof(OV5670_MODULE_MAP) / sizeof(MODULE_Map_Table), sensor_module_id);
	}
	for (i = 0; i < block->num_map; i++) {
		e_ctrl->i2c_client.addr_type = emap[i].mem.addr_t;
		rc = e_ctrl->i2c_client.i2c_func_tbl->i2c_read_seq(
				 &(e_ctrl->i2c_client), emap[i].mem.addr,
				 memptr, emap[i].mem.valid_size);
		if (rc < 0) {
			pr_err("%s: read failed\n", __func__);
			return rc;
		}
		memptr += emap[i].mem.valid_size;
	}
	ov5670_read_eeprom_end(e_ctrl);
	CDBG("%s end", __func__);
	return rc;
}

void ov8856_read_eeprom_init(struct msm_eeprom_ctrl_t *e_ctrl)
{
	uint16_t temp;

	e_ctrl->i2c_client.i2c_func_tbl->i2c_read(&(e_ctrl->i2c_client), 0x5000, &temp,
			MSM_CAMERA_I2C_BYTE_DATA);
	pr_err("%s:0x5000 temp=0x%X", __func__, temp);

	e_ctrl->i2c_client.i2c_func_tbl->i2c_read(&(e_ctrl->i2c_client), 0x5001, &temp,
			MSM_CAMERA_I2C_BYTE_DATA);
	pr_err("%s:0x5001 temp=0x%X", __func__, temp);

	e_ctrl->i2c_client.i2c_func_tbl->i2c_write(&(e_ctrl->i2c_client), 0x0100, 0x01,
			MSM_CAMERA_I2C_BYTE_DATA);

	e_ctrl->i2c_client.i2c_func_tbl->i2c_write(&(e_ctrl->i2c_client), 0x5001, (0x00 & 0x08) | (temp & (~0x08)),
			MSM_CAMERA_I2C_BYTE_DATA);

	e_ctrl->i2c_client.i2c_func_tbl->i2c_write(&(e_ctrl->i2c_client), 0x3d84, 0xc0,
			MSM_CAMERA_I2C_BYTE_DATA);
	e_ctrl->i2c_client.i2c_func_tbl->i2c_read(&(e_ctrl->i2c_client), 0x3d84, &temp,
			MSM_CAMERA_I2C_BYTE_DATA);
	e_ctrl->i2c_client.i2c_func_tbl->i2c_write(&(e_ctrl->i2c_client), 0x3d88, 0x70,
			MSM_CAMERA_I2C_BYTE_DATA);
	e_ctrl->i2c_client.i2c_func_tbl->i2c_write(&(e_ctrl->i2c_client), 0x3d89, 0x10,
			MSM_CAMERA_I2C_BYTE_DATA);
	e_ctrl->i2c_client.i2c_func_tbl->i2c_write(&(e_ctrl->i2c_client), 0x3d8a, 0x72,
			MSM_CAMERA_I2C_BYTE_DATA);
	e_ctrl->i2c_client.i2c_func_tbl->i2c_write(&(e_ctrl->i2c_client), 0x3d8b, 0x0a,
			MSM_CAMERA_I2C_BYTE_DATA);
	e_ctrl->i2c_client.i2c_func_tbl->i2c_write(&(e_ctrl->i2c_client), 0x3d81, 0x01,
			MSM_CAMERA_I2C_BYTE_DATA);
	udelay(5);
}

void ov8856_read_eeprom_end(struct msm_eeprom_ctrl_t *e_ctrl)
{
	uint16_t temp;

	e_ctrl->i2c_client.i2c_func_tbl->i2c_read(&(e_ctrl->i2c_client), 0x5001, &temp,
			MSM_CAMERA_I2C_BYTE_DATA);
	pr_err("%s:0x5001 temp=0x%X", __func__, temp);

	e_ctrl->i2c_client.i2c_func_tbl->i2c_write(&(e_ctrl->i2c_client), 0x5001, (0x08 & 0x08) | (temp & (~0x08)),
			MSM_CAMERA_I2C_BYTE_DATA);
	e_ctrl->i2c_client.i2c_func_tbl->i2c_write(&(e_ctrl->i2c_client), 0x0100, 0x00,
			MSM_CAMERA_I2C_BYTE_DATA);
}

int32_t ov8856_check_module_info_group(struct msm_eeprom_ctrl_t *e_ctrl)
{
	uint16_t temp;

	e_ctrl->i2c_client.i2c_func_tbl->i2c_read(&(e_ctrl->i2c_client), 0x7010, &temp,
			MSM_CAMERA_I2C_BYTE_DATA);
	CDBG("%s:temp=0x%X", __func__, temp);
	if ((temp & 0xC0) == 0x40) {
		return Group_One;
	} else if ((temp & 0x30) == 0x10) {
		return Group_Two;
	}
	return Invlid_Group;
}

static int ov8856_read_eeprom_memory(struct msm_eeprom_ctrl_t *e_ctrl,
							 struct msm_eeprom_memory_block_t *block)
{
	int rc = 0;
	struct msm_eeprom_memory_map_t *emap = block->map;
	struct msm_eeprom_board_info *eb_info;
	uint8_t *memptr = block->mapdata;
	uint16_t  sensor_module_id = 0;
	int32_t group_number;
	uint32_t module_id_addr;
	int i;

	if (!e_ctrl) {
		pr_err("%s e_ctrl is NULL", __func__);
		return -EINVAL;
	}
	pr_err("%s begin", __func__);
	eb_info = e_ctrl->eboard_info;
	e_ctrl->i2c_client.addr_type = MSM_CAMERA_I2C_WORD_ADDR;

	ov8856_read_eeprom_init(e_ctrl);

	group_number = ov8856_check_module_info_group(e_ctrl);
	switch (group_number) {
	case Group_One:
		module_id_addr = 0x7011;
		break;
	case Group_Two:
		module_id_addr = 0x7019;
		break;
	default:
		break;
	}
	if (module_id_addr != 0) {
		e_ctrl->i2c_client.i2c_func_tbl->i2c_read(&(e_ctrl->i2c_client), module_id_addr, &sensor_module_id,
				MSM_CAMERA_I2C_BYTE_DATA);
		pr_err("sensor_module_id =0x%X\n", sensor_module_id);
		parse_module_name(e_ctrl, OV8856_MODULE_MAP,
				  sizeof(OV8856_MODULE_MAP) / sizeof(MODULE_Map_Table), sensor_module_id);
	}
	for (i = 0; i < block->num_map; i++) {
		e_ctrl->i2c_client.addr_type = emap[i].mem.addr_t;
		rc = e_ctrl->i2c_client.i2c_func_tbl->i2c_read_seq(
				 &(e_ctrl->i2c_client), emap[i].mem.addr,
				 memptr, emap[i].mem.valid_size);
		if (rc < 0) {
			pr_err("%s: read failed\n", __func__);
			return rc;
		}
		memptr += emap[i].mem.valid_size;
	}
	ov8856_read_eeprom_end(e_ctrl);
	pr_err("%s end", __func__);
	return rc;
}

void s5k5e8_read_eeprom_init(struct msm_eeprom_ctrl_t *e_ctrl)
{

	e_ctrl->i2c_client.i2c_func_tbl->i2c_write(&(e_ctrl->i2c_client), 0x0A00, 0x04,
			MSM_CAMERA_I2C_BYTE_DATA);
	e_ctrl->i2c_client.i2c_func_tbl->i2c_write(&(e_ctrl->i2c_client), 0x0A02, 0x04,
			MSM_CAMERA_I2C_BYTE_DATA);
	e_ctrl->i2c_client.i2c_func_tbl->i2c_write(&(e_ctrl->i2c_client), 0x0A00, 0x01,
			MSM_CAMERA_I2C_BYTE_DATA);
	mdelay(2);
}

void s5k5e8_read_eeprom_end(struct msm_eeprom_ctrl_t *e_ctrl)
{

	e_ctrl->i2c_client.i2c_func_tbl->i2c_write(&(e_ctrl->i2c_client), 0x0A00, 0x04,
			MSM_CAMERA_I2C_BYTE_DATA);
	e_ctrl->i2c_client.i2c_func_tbl->i2c_write(&(e_ctrl->i2c_client), 0x0A00, 0x00,
			MSM_CAMERA_I2C_BYTE_DATA);
}
int32_t s5k5e8_check_module_info_group(struct msm_eeprom_ctrl_t *e_ctrl)
{
	uint16_t temp;
	int32_t rc = Invlid_Group;

	e_ctrl->i2c_client.i2c_func_tbl->i2c_read(&(e_ctrl->i2c_client), 0x0a04, &temp,
			MSM_CAMERA_I2C_BYTE_DATA);
	CDBG("%s:temp=0x%X\n", __func__, temp);
	if (temp == 0x01) {
		CDBG("%s:AWB group 1 flag,temp=0x%X\n", __func__, temp);
		rc = Group_One;
	} else {
		e_ctrl->i2c_client.i2c_func_tbl->i2c_read(&(e_ctrl->i2c_client), 0x0a19, &temp,
				MSM_CAMERA_I2C_BYTE_DATA);
		CDBG("%s:AWB group 2 flag,temp=0x%X\n", __func__, temp);
		if (temp == 0x01)
			rc = Group_Two;
	}
	return rc;
}
int32_t s5k5e8_check_module_AF_info_group(struct msm_eeprom_ctrl_t *e_ctrl)
{
	uint16_t temp;
	int32_t rc = Invlid_Group;

	e_ctrl->i2c_client.i2c_func_tbl->i2c_read(&(e_ctrl->i2c_client), 0x0a04, &temp,
			MSM_CAMERA_I2C_BYTE_DATA);
	CDBG("%s:temp=0x%X\n", __func__, temp);
	if (temp == 0x01) {
		CDBG("%s:AF group 1 flag,temp=0x%X\n", __func__, temp);
		rc = Group_Three;
	} else {
		e_ctrl->i2c_client.i2c_func_tbl->i2c_read(&(e_ctrl->i2c_client), 0x0a0e, &temp,
				MSM_CAMERA_I2C_BYTE_DATA);
		CDBG("%s:AF group 2 flag,temp=0x%X\n", __func__, temp);
		if (temp == 0x01)
			rc = Group_Four;
	}
	return rc;
}
#define  S5K5E8_LSC_SIZE 360
#define  S5K5E8_AWB_SIZE 0x13
#define  S5K5E8_AF_SIZE 4

static int s5k5e8_read_eeprom_memory(struct msm_eeprom_ctrl_t *e_ctrl,
									 struct msm_eeprom_memory_block_t *block)
{
	uint16_t temp;
	int rc = 0;
	struct msm_eeprom_memory_map_t *emap = block->map;
	struct msm_eeprom_board_info *eb_info;
	uint8_t *memptr = block->mapdata;
	uint16_t  sensor_module_id = 0;
	int32_t group_number;
	uint32_t module_id_addr;
	uint32_t af_flag_addr;
	uint32_t check_sum;
	uint8_t *buff_data;
	uint8_t lsc_flag;
	int num_byte;
	int i;

	if (!e_ctrl) {
		pr_err("%s e_ctrl is NULL", __func__);
		return -EINVAL;
	}

	pr_err("%s begin", __func__);
	eb_info = e_ctrl->eboard_info;
	e_ctrl->i2c_client.addr_type = MSM_CAMERA_I2C_WORD_ADDR;


	s5k5e8_read_eeprom_init(e_ctrl);

	group_number = s5k5e8_check_module_info_group(e_ctrl);
	switch (group_number) {
	case Group_One:
		module_id_addr = 0x0A05;
		pr_err("module_id_addr =0x%X\n", module_id_addr);
		break;
	case Group_Two:
		module_id_addr = 0x0A1A;
		pr_err("module_id_addr =0x%X\n", module_id_addr);
		break;
	default:
		break;
	}
	if (module_id_addr != 0) {
		e_ctrl->i2c_client.i2c_func_tbl->i2c_read(&(e_ctrl->i2c_client), module_id_addr, &sensor_module_id,
				MSM_CAMERA_I2C_BYTE_DATA);
		pr_err("sensor_module_id =0x%X\n", sensor_module_id);
		parse_module_name(e_ctrl, S5K5E8_MODULE_MAP,
				  sizeof(S5K5E8_MODULE_MAP) / sizeof(MODULE_Map_Table), sensor_module_id);
	}
	/*
	 *LSC otp
	 */
	e_ctrl->i2c_client.addr_type = emap[0].mem.addr_t;
	rc = e_ctrl->i2c_client.i2c_func_tbl->i2c_read_seq(
			 &(e_ctrl->i2c_client), emap[0].mem.addr,
			 &lsc_flag, emap[0].mem.valid_size);
	if (lsc_flag == 0x01) {
		*memptr = 1;
		pr_err("LSC OTP read success");
	} else {
		*memptr = 0;
		pr_err("LSC OTP read failed");
	}
	memptr += 1;

	/*
	 *AWB otp
	 */
	num_byte = S5K5E8_AWB_SIZE;
	buff_data = kzalloc(num_byte, GFP_KERNEL);
	memset(buff_data, 0, num_byte);

	e_ctrl->i2c_client.addr_type = emap[group_number].mem.addr_t;
	rc = e_ctrl->i2c_client.i2c_func_tbl->i2c_read_seq(
			 &(e_ctrl->i2c_client), emap[group_number].mem.addr,
			 buff_data, emap[group_number].mem.valid_size);
	check_sum = 0;
	for (i = 0; i < S5K5E8_AWB_SIZE; i++) {
		check_sum += buff_data[i];
		pr_err("buff_data[%d]=0x%x\n", i, buff_data[i]);
	}
	check_sum = check_sum % 255 + 1;
	e_ctrl->i2c_client.i2c_func_tbl->i2c_read(&(e_ctrl->i2c_client), module_id_addr + S5K5E8_AWB_SIZE,
						 &temp,	MSM_CAMERA_I2C_BYTE_DATA);
	pr_err("%s:AWB checksum reg temp=0x%X", __func__, temp);

	if (check_sum == temp) {
		pr_err("AWB OTP read success\n");
		*memptr = 1;
		memptr += 1;
		for (i = 0; i < S5K5E8_AWB_SIZE; i++) {
			*memptr = buff_data[i];
			pr_err("*memptr=0x%x memptr=%p\n", *memptr, memptr);
			memptr += 1;
		}
	} else {
		*memptr = 0;
		memptr += 1;
		pr_err("AWB OTP read failed:group=%d\n", group_number);
	}

	kfree(buff_data);
	/*
	 *AF otp
	 */

	e_ctrl->i2c_client.i2c_func_tbl->i2c_write(&(e_ctrl->i2c_client), 0x0A00, 0x04,
			MSM_CAMERA_I2C_BYTE_DATA);
	e_ctrl->i2c_client.i2c_func_tbl->i2c_write(&(e_ctrl->i2c_client), 0x0A02, 0x0F,
			MSM_CAMERA_I2C_BYTE_DATA);
	e_ctrl->i2c_client.i2c_func_tbl->i2c_write(&(e_ctrl->i2c_client), 0x0A00, 0x01,
			MSM_CAMERA_I2C_BYTE_DATA);
	mdelay(2);

	group_number = s5k5e8_check_module_AF_info_group(e_ctrl);
	if (group_number == 0) {
		pr_err(" s5k5e8 AF OTP read failed !");
		*memptr = 0;
		memptr += 1;
	} else {
		switch (group_number) {
		case Group_Three:
			af_flag_addr = 0x0A04;
			pr_err("af_flag_addr =0x%X\n", af_flag_addr);
			break;
		case Group_Four:
			af_flag_addr = 0x0A0E;
			pr_err("af_flag_addr=0x%X\n", af_flag_addr);
			break;
		default:
			break;
		}
		num_byte = S5K5E8_AF_SIZE;
		buff_data = kzalloc(num_byte, GFP_KERNEL);
		memset(buff_data, 0, num_byte);

		e_ctrl->i2c_client.addr_type = emap[group_number].mem.addr_t;
		rc = e_ctrl->i2c_client.i2c_func_tbl->i2c_read_seq(
				 &(e_ctrl->i2c_client), emap[group_number].mem.addr,
				 buff_data, emap[group_number].mem.valid_size);
		check_sum = 0;
		for (i = 0; i < S5K5E8_AF_SIZE; i++) {
			check_sum += buff_data[i];
			pr_err("buff_data[%d]=0x%x\n", i, buff_data[i]);
		}
		check_sum = check_sum % 255 + 1;
		e_ctrl->i2c_client.i2c_func_tbl->i2c_read(&(e_ctrl->i2c_client), af_flag_addr + S5K5E8_AF_SIZE + 1,
				 &temp,	MSM_CAMERA_I2C_BYTE_DATA);
		pr_err("%s:AF checksum reg temp=0x%X", __func__, temp);

		if (check_sum == temp) {
			pr_err("AF OTP read success\n");
			*memptr = 1;
			memptr += 1;
			for (i = 0; i < S5K5E8_AF_SIZE; i++) {
				*memptr = buff_data[i];
				pr_err("*memptr=0x%x memptr=%p\n", *memptr, memptr);
				memptr += 1;
			}
		} else {
			*memptr = 0;
			memptr += 1;
			pr_err("AF OTP read failed:group=%d\n", group_number);
		}
		kfree(buff_data);
	}

	s5k5e8_read_eeprom_end(e_ctrl);
	pr_err("%s end\n", __func__);
	return rc;
}

/*
 *add for ov5670 eeprom begin by zte_cam_wxl_20150915
 */
enum {
	invlid_group,
	group_one,
	group_two,
	group_three,
	group_four,
} group_t;

#define  S5K4H8_LSC_SIZE 360
#define  S5K4H8_AWB_SIZE 0x14
#define  S5K4H8_AF_SIZE 4
#define  S5K4H8_OTP_FLAG 0x1

void s5k4h8_read_eeprom_init(struct msm_eeprom_ctrl_t *e_ctrl)
{

	e_ctrl->i2c_client.i2c_func_tbl->i2c_write(&(e_ctrl->i2c_client), 0x0100, 0x01,
		MSM_CAMERA_I2C_BYTE_DATA);
	msleep(20);
	e_ctrl->i2c_client.i2c_func_tbl->i2c_write(&(e_ctrl->i2c_client), 0x0A02, 0x0F,
		MSM_CAMERA_I2C_BYTE_DATA);
	e_ctrl->i2c_client.i2c_func_tbl->i2c_write(&(e_ctrl->i2c_client), 0x0A00, 0x01,
		MSM_CAMERA_I2C_BYTE_DATA);
	msleep(20);
}

void s5k4h8_read_eeprom_end(struct msm_eeprom_ctrl_t *e_ctrl)
{
	e_ctrl->i2c_client.i2c_func_tbl->i2c_write(&(e_ctrl->i2c_client), 0x0A00, 0x04,
		MSM_CAMERA_I2C_BYTE_DATA);
	udelay(2000);
	e_ctrl->i2c_client.i2c_func_tbl->i2c_write(&(e_ctrl->i2c_client), 0x0A00, 0x00,
		MSM_CAMERA_I2C_BYTE_DATA);
}

int32_t s5k4h8_check_module_info_group(struct msm_eeprom_ctrl_t *e_ctrl)
{
	uint16_t val;
	int32_t rc = invlid_group;

	e_ctrl->i2c_client.i2c_func_tbl->i2c_read(&(e_ctrl->i2c_client), 0x0A04, &val,
		MSM_CAMERA_I2C_BYTE_DATA);
	CDBG("%s:zte_eeprom: val=0x%X", __func__, val);
	if (val == S5K4H8_OTP_FLAG) {
		CDBG("%s:MID AWB group 1 flag,val=0x%X", __func__, val);
		rc = group_one;
	} else {
		e_ctrl->i2c_client.i2c_func_tbl->i2c_read(&(e_ctrl->i2c_client), 0x0A1A, &val,
				MSM_CAMERA_I2C_BYTE_DATA);
		CDBG("%s:MID AWB group 2 flag,val=0x%X", __func__, val);
		if (val == S5K4H8_OTP_FLAG)
			rc = group_two;
	}
	return rc;
}

int32_t s5k4h8_check_module_AF_info_group(struct msm_eeprom_ctrl_t *e_ctrl)
{
	uint16_t val;
	int32_t rc = invlid_group;

	e_ctrl->i2c_client.i2c_func_tbl->i2c_read(&(e_ctrl->i2c_client), 0x0A30, &val,
			MSM_CAMERA_I2C_BYTE_DATA);
	CDBG("%s:val=0x%X\n", __func__, val);
	if (val == S5K4H8_OTP_FLAG) {
		CDBG("%s:AF group 1 flag,val=0x%X\n", __func__, val);
		rc = group_three;
	} else {
		e_ctrl->i2c_client.i2c_func_tbl->i2c_read(&(e_ctrl->i2c_client), 0x0A37, &val,
				MSM_CAMERA_I2C_BYTE_DATA);
		CDBG("%s:AF group 2 flag,val=0x%X\n", __func__, val);
		if (val == S5K4H8_OTP_FLAG)
			rc = group_four;
	}
	return rc;
}

static int s5k4h8_read_eeprom_memory(struct msm_eeprom_ctrl_t *e_ctrl,
	struct msm_eeprom_memory_block_t *block)
{
	uint16_t val;
	int rc = 0;
	struct msm_eeprom_memory_map_t *emap = block->map;
	struct msm_eeprom_board_info *eb_info;
	uint8_t *memptr = block->mapdata;
	uint16_t  sensor_module_id = 0;
	uint16_t  lsc_group1_flag = 0;
	uint16_t  lsc_group2_flag = 0;
	int32_t group_number;
	uint32_t module_id_addr;
	uint32_t lsc_group1_flag_addr;
	uint32_t lsc_group2_flag_addr;
	uint32_t af_flag_addr;
	uint32_t check_sum;
	uint8_t *buff_data;
	uint8_t lsc_flag;
	int num_byte;
	int i;

	if (!e_ctrl) {
		pr_info("%s e_ctrl is NULL", __func__);
		return -EINVAL;
	}

	pr_info("%s begin", __func__);
	eb_info = e_ctrl->eboard_info;
	e_ctrl->i2c_client.addr_type = MSM_CAMERA_I2C_WORD_ADDR;

	module_id_addr = 0;

	s5k4h8_read_eeprom_init(e_ctrl);

	group_number = s5k4h8_check_module_info_group(e_ctrl);
	switch (group_number) {
	case group_one:
		module_id_addr = 0x0A05;
		pr_info("module_id_addr =0x%X\n", module_id_addr);
		break;
	case group_two:
		module_id_addr = 0x0A1B;
		pr_info("module_id_addr =0x%X\n", module_id_addr);
		break;
	default:
		break;
	}

	if (module_id_addr != 0) {
		e_ctrl->i2c_client.i2c_func_tbl->i2c_read(&(e_ctrl->i2c_client), module_id_addr, &sensor_module_id,
				MSM_CAMERA_I2C_BYTE_DATA);
		pr_info("sensor_module_id =0x%X\n module_id_addr =0x%X\n", sensor_module_id, module_id_addr);
		parse_module_name(e_ctrl, S5K4H8_MODULE_MAP,
				  sizeof(S5K4H8_MODULE_MAP) / sizeof(MODULE_Map_Table), sensor_module_id);
	}

	/*read lsc group1 flag*/
	lsc_group1_flag_addr = 0x0A3E;
	e_ctrl->i2c_client.i2c_func_tbl->i2c_read(&(e_ctrl->i2c_client), lsc_group1_flag_addr, &lsc_group1_flag,
		MSM_CAMERA_I2C_BYTE_DATA);
	pr_info("zte_eeprom: read lsc group1 flag is =0x%X\n", lsc_group1_flag);

	s5k4h8_read_eeprom_end(e_ctrl);

	e_ctrl->i2c_client.i2c_func_tbl->i2c_write(&(e_ctrl->i2c_client), 0x0100, 0x01,
		MSM_CAMERA_I2C_BYTE_DATA);
	msleep(20);
	e_ctrl->i2c_client.i2c_func_tbl->i2c_write(&(e_ctrl->i2c_client), 0x0A02, 0x09,
		MSM_CAMERA_I2C_BYTE_DATA);
	e_ctrl->i2c_client.i2c_func_tbl->i2c_write(&(e_ctrl->i2c_client), 0x0A00, 0x01,
		MSM_CAMERA_I2C_BYTE_DATA);
	msleep(20);

	lsc_group2_flag_addr = 0x0A1B;
	e_ctrl->i2c_client.i2c_func_tbl->i2c_read(&(e_ctrl->i2c_client), lsc_group2_flag_addr, &lsc_group2_flag,
		MSM_CAMERA_I2C_BYTE_DATA);
	pr_info("zte_eeprom: read lsc flag2 is =0x%X\n", lsc_group2_flag);

	e_ctrl->i2c_client.addr_type = emap[0].mem.addr_t;
	(void)e_ctrl->i2c_client.i2c_func_tbl->i2c_read_seq(
			&(e_ctrl->i2c_client), emap[0].mem.addr,
			&lsc_flag, emap[0].mem.valid_size);

	if ((lsc_group1_flag == S5K4H8_OTP_FLAG) || (lsc_group2_flag == S5K4H8_OTP_FLAG)) {
		*memptr = 1;
		pr_err("LSC OTP read success");
	} else {
		*memptr = 0;
		pr_err("LSC OTP read failed");
	}
	memptr += 1;

	/*
	 *AWB otp
	 */
	s5k4h8_read_eeprom_init(e_ctrl);
	num_byte = S5K4H8_AWB_SIZE;
	buff_data = kzalloc(num_byte, GFP_KERNEL);

	e_ctrl->i2c_client.addr_type = emap[group_number].mem.addr_t;
	rc = e_ctrl->i2c_client.i2c_func_tbl->i2c_read_seq(
			&(e_ctrl->i2c_client), emap[group_number].mem.addr,
			buff_data, emap[group_number].mem.valid_size);
	check_sum = 0;
	for (i = 0; i < S5K4H8_AWB_SIZE; i++) {
		check_sum += buff_data[i];
		pr_info("buff_data[%d]=0x%x\n", i, buff_data[i]);
	}
	check_sum = check_sum % 255 + 1;

	e_ctrl->i2c_client.i2c_func_tbl->i2c_read(&(e_ctrl->i2c_client), module_id_addr + S5K4H8_AWB_SIZE,
		&val,	MSM_CAMERA_I2C_BYTE_DATA);
	pr_info("%s:AWB checksum reg temp=0x%X Read addr is 0x%X,checksum is 0x%X\n", __func__,
		val, (module_id_addr + S5K4H8_AWB_SIZE), check_sum);

	if (check_sum == val) {
		pr_err("AWB OTP read success\n");
		*memptr = 1;
		memptr += 1;
		for (i = 0; i < S5K4H8_AWB_SIZE; i++) {
			*memptr = buff_data[i];
			pr_info("*memptr=0x%x memptr=%p\n", *memptr, memptr);
			memptr += 1;
		}
	} else {
		*memptr = 0;
		memptr += 1;
		pr_info("AWB OTP read failed:group=%d\n", group_number);
	}

	kfree(buff_data);

	/*read AF otp*/

	e_ctrl->i2c_client.i2c_func_tbl->i2c_write(&(e_ctrl->i2c_client), 0x0100, 0x01,
			MSM_CAMERA_I2C_BYTE_DATA);
	msleep(20);
	e_ctrl->i2c_client.i2c_func_tbl->i2c_write(&(e_ctrl->i2c_client), 0x0A02, 0x0F,
			MSM_CAMERA_I2C_BYTE_DATA);
	e_ctrl->i2c_client.i2c_func_tbl->i2c_write(&(e_ctrl->i2c_client), 0x0A00, 0x01,
			MSM_CAMERA_I2C_BYTE_DATA);
	msleep(20);

	group_number = s5k4h8_check_module_AF_info_group(e_ctrl);
	if (group_number == 0) {
		pr_info(" s5k5e8 AF OTP read failed !");
		*memptr = 0;
		memptr += 1;
	} else {
		switch (group_number) {
		case group_three:
			af_flag_addr = 0x0A30;
			pr_info("zte_eeprom: s5k4h8 af_flag_addr =0x%X\n", af_flag_addr);
			break;
		case group_four:
			af_flag_addr = 0x0A37;
			pr_info("zte_eeprom: s5k4h8  af_flag_addr=0x%X\n", af_flag_addr);
			break;
		default:
			break;
		}
		num_byte = S5K4H8_AF_SIZE;
		buff_data = kzalloc(num_byte, GFP_KERNEL);

		e_ctrl->i2c_client.addr_type = emap[group_number].mem.addr_t;
		rc = e_ctrl->i2c_client.i2c_func_tbl->i2c_read_seq(
				 &(e_ctrl->i2c_client), emap[group_number].mem.addr,
				 buff_data, emap[group_number].mem.valid_size);
		check_sum = 0;
		for (i = 0; i < S5K4H8_AF_SIZE; i++) {
			check_sum += buff_data[i];
		}
		check_sum = check_sum % 255 + 1;
		e_ctrl->i2c_client.i2c_func_tbl->i2c_read(&(e_ctrl->i2c_client),
		af_flag_addr + S5K4H8_AF_SIZE + 2, &val, MSM_CAMERA_I2C_BYTE_DATA);
		pr_info("%s:AF checksum reg val=0x%X,read addr is 0x%X,checksum is 0x%X\n", __func__,
		val, (af_flag_addr + S5K4H8_AF_SIZE + 2), check_sum);

		if (check_sum == val) {
			pr_err("AF OTP read success\n");
			*memptr = 1;
			memptr += 1;
			for (i = 0; i < S5K4H8_AF_SIZE; i++) {
				*memptr = buff_data[i];
				pr_info("*memptr=0x%x memptr=%p\n", *memptr, memptr);
				memptr += 1;
			}
		} else {
			*memptr = 0;
			memptr += 1;
			pr_info("AF OTP read failed:group=%d\n", group_number);
		}
		kfree(buff_data);
	}
	s5k4h8_read_eeprom_end(e_ctrl);
	pr_info("%s end\n", __func__);
	return rc;
}

#define IMAGE_NORMAL_MIRROR
#define DD_PARAM_QTY	200
#define WINDOW_WIDTH	0x0a30
#define WINDOW_HEIGHT	0x079c
#define RG_TYPICAL	0x0400
#define BG_TYPICAL	0x0400
#define INFO_ROM_START	0x01
#define INFO_WIDTH	0x08
#define WB_ROM_START	0x11
#define WB_WIDTH	0x05
#define GOLDEN_ROM_START	0x1c
#define GOLDEN_WIDTH	0x05
#define REG_ROM_START	0x62

typedef struct otp_gc5025 {
	uint16_t module_id;
	uint16_t lens_id;
	uint16_t vcm_id;
	uint16_t vcm_driver_id;
	uint16_t year;
	uint16_t month;
	uint16_t day;
	uint16_t rg_gain;
	uint16_t bg_gain;
	uint16_t wb_flag;
	uint16_t golden_flag;
	uint16_t dd_param_x[DD_PARAM_QTY];
	uint16_t dd_param_y[DD_PARAM_QTY];
	uint16_t dd_param_type[DD_PARAM_QTY];
	uint16_t dd_cnt;
	uint16_t dd_flag;
	uint16_t golden_rg;
	uint16_t golden_bg;
	uint16_t reg_addr[10];
	uint16_t reg_value[10];
	uint16_t reg_num;
} gc5025_otp;

static gc5025_otp gc5025_otp_info;

typedef enum{
	otp_page0 = 0,
	otp_page1,
} otp_page;

typedef enum {
	otp_close = 0,
	otp_open,
} otp_state;

static uint16_t gc5025_Sensor_ReadReg(
	struct msm_eeprom_ctrl_t *e_ctrl, uint8_t reg_addr)
{
	uint16_t reg_value = 0;

	e_ctrl->i2c_client.i2c_func_tbl->i2c_read(
				&(e_ctrl->i2c_client),
				reg_addr,
				&reg_value, MSM_CAMERA_I2C_BYTE_DATA);
	return reg_value;
}

static void gc5025_Sensor_WriteReg(
	struct msm_eeprom_ctrl_t *e_ctrl, uint8_t reg_addr, uint8_t reg_value)
{

	e_ctrl->i2c_client.i2c_func_tbl->i2c_write(
		&(e_ctrl->i2c_client), reg_addr, reg_value, MSM_CAMERA_I2C_BYTE_DATA);
}


static uint8_t gc5025_read_otp(struct msm_eeprom_ctrl_t *e_ctrl, uint8_t addr)
{
	uint8_t value;
	uint8_t regd4;
	uint16_t realaddr = addr * 8;

	regd4 = gc5025_Sensor_ReadReg(e_ctrl, 0xd4);
	gc5025_Sensor_WriteReg(e_ctrl, 0xfe, 0x00);
	gc5025_Sensor_WriteReg(e_ctrl, 0xd4, (regd4&0xfc)+((realaddr>>8)&0x03));
	gc5025_Sensor_WriteReg(e_ctrl, 0xd5, realaddr&0xff);
	gc5025_Sensor_WriteReg(e_ctrl, 0xf3, 0x20);
	value = gc5025_Sensor_ReadReg(e_ctrl, 0xd7);

	return value;
}

static void gc5025_read_otp_group(struct msm_eeprom_ctrl_t *e_ctrl, uint8_t addr, uint8_t *buff, int size)
{
	uint8_t i;
	uint8_t regd4;
	uint16_t realaddr = addr * 8;

	regd4 = gc5025_Sensor_ReadReg(e_ctrl, 0xd4);

	gc5025_Sensor_WriteReg(e_ctrl, 0xfe, 0x00);
	gc5025_Sensor_WriteReg(e_ctrl, 0xd4, (regd4&0xfc) + ((realaddr >> 8)&0x03));
	gc5025_Sensor_WriteReg(e_ctrl, 0xd5, realaddr);
	gc5025_Sensor_WriteReg(e_ctrl, 0xf3, 0x20);
	gc5025_Sensor_WriteReg(e_ctrl, 0xf3, 0x88);

	for (i = 0; i < size; i++) {
		buff[i] = gc5025_Sensor_ReadReg(e_ctrl, 0xd7);
	}
}

static void gc5025_select_page_otp(struct msm_eeprom_ctrl_t *e_ctrl, otp_page otp_select_page)
{
	uint8_t page;

	gc5025_Sensor_WriteReg(e_ctrl, 0xfe, 0x00);
	page = gc5025_Sensor_ReadReg(e_ctrl, 0xd4);
	CDBG("GC5025 select read page is 0x%x !!\n", page);

	switch (otp_select_page) {
	case otp_page0:
		page = page & 0xfb;
		break;
	case otp_page1:
		page = page | 0x04;
		break;
	default:
		break;
	}

	msleep(20);
	CDBG("GC5025 select write page is 0x%x !!\n", page);
	gc5025_Sensor_WriteReg(e_ctrl, 0xd4, page);

}

static void gc5025_gcore_read_otp_info(struct msm_eeprom_ctrl_t *e_ctrl)
{
	uint8_t flag1;
	uint8_t index, i = 0;
	uint16_t check;
	uint8_t info[8];

	memset(&gc5025_otp_info, 0, sizeof(gc5025_otp));

	gc5025_select_page_otp(e_ctrl, otp_page1);
	flag1 = gc5025_read_otp(e_ctrl, 0x00);
	CDBG("GC5025_OTP : flag1 = 0x%x\n", flag1);

	for (index = 0; index < 2; index++) {
		switch ((flag1>>(4 + 2 * index))&0x03) {
		case 0x00:
			CDBG("GC5025_OTP_INFO group %d is Empty !!\n", index + 1);
			break;
		case 0x01:
			CDBG("GC5025_OTP_INFO group %d is Valid !!\n", index + 1);
			check = 0;
			gc5025_read_otp_group(e_ctrl, INFO_ROM_START + index * INFO_WIDTH, &info[0], INFO_WIDTH);
			for (i = 0; i < INFO_WIDTH - 1; i++) {
				check += info[i];
			}
			CDBG("GC5025_OTP_INFO cacluate checksum is %x,checksum data is %x\n", check%256,
				info[INFO_WIDTH-1]);
			if ((check % 256) == info[INFO_WIDTH-1]) {
				gc5025_otp_info.module_id = info[0];
				gc5025_otp_info.lens_id = info[1];
				gc5025_otp_info.vcm_driver_id = info[2];
				gc5025_otp_info.vcm_id = info[3];
				gc5025_otp_info.year = info[4];
				gc5025_otp_info.month = info[5];
				gc5025_otp_info.day = info[6];
			} else {
				CDBG("GC5025_OTP_INFO Check sum %d Error !!\n", index + 1);
			}
			break;
		case 0x02:
		case 0x03:
			CDBG("GC5025_OTP_INFO group %d is Invalid !!\n", index + 1);
			break;
		default:
			break;
		}


	}

	/*print otp information*/
	CDBG("GC5025_OTP_INFO:module_id=0x%x\n", gc5025_otp_info.module_id);
	CDBG("GC5025_OTP_INFO:lens_id=0x%x\n", gc5025_otp_info.lens_id);
	CDBG("GC5025_OTP_INFO:vcm_id=0x%x\n", gc5025_otp_info.vcm_id);
	CDBG("GC5025_OTP_INFO:vcm_driver_id=0x%x\n", gc5025_otp_info.vcm_driver_id);
	CDBG("GC5025_OTP_INFO:data=%d-%d-%d\n", gc5025_otp_info.year,
		gc5025_otp_info.month, gc5025_otp_info.day);
}


static void gc5025_gcore_enable_otp(struct msm_eeprom_ctrl_t *e_ctrl, otp_state state)
{
	uint8_t otp_clk, otp_en;

	otp_clk = gc5025_Sensor_ReadReg(e_ctrl, 0xfa);
	otp_en = gc5025_Sensor_ReadReg(e_ctrl, 0xd4);
	if (state) {
		otp_clk = otp_clk | 0x10;
		otp_en = otp_en | 0x80;
		msleep(20);
		gc5025_Sensor_WriteReg(e_ctrl, 0xfa, otp_clk);
		gc5025_Sensor_WriteReg(e_ctrl, 0xd4, otp_en);
	} else {
		otp_en = otp_en & 0x7f;
		otp_clk = otp_clk & 0xef;
		msleep(20);
		gc5025_Sensor_WriteReg(e_ctrl, 0xd4, otp_en);
		gc5025_Sensor_WriteReg(e_ctrl, 0xfa, otp_clk);
	}

}

static int gc5025_read_eeprom_memory(struct msm_eeprom_ctrl_t *e_ctrl,
	struct msm_eeprom_memory_block_t *block)
{
	int rc = 0;
	uint16_t  sensor_module_id = 0;
	struct msm_eeprom_board_info *eb_info;

	if (!e_ctrl) {
		pr_info("%s e_ctrl is NULL", __func__);
		return -EINVAL;
	}

	pr_info("%s begin", __func__);
	eb_info = e_ctrl->eboard_info;
	e_ctrl->i2c_client.addr_type = MSM_CAMERA_I2C_BYTE_ADDR;

	gc5025_Sensor_WriteReg(e_ctrl, 0xfe, 0x00);
	gc5025_Sensor_WriteReg(e_ctrl, 0xfe, 0x00);
	gc5025_Sensor_WriteReg(e_ctrl, 0xfe, 0x00);
	gc5025_Sensor_WriteReg(e_ctrl, 0xf7, 0x01);
	gc5025_Sensor_WriteReg(e_ctrl, 0xf8, 0x11);
	gc5025_Sensor_WriteReg(e_ctrl, 0xf9, 0x00);
	gc5025_Sensor_WriteReg(e_ctrl, 0xfa, 0xa0);
	gc5025_Sensor_WriteReg(e_ctrl, 0xfc, 0x2e);
	gc5025_gcore_enable_otp(e_ctrl, otp_open);
	gc5025_gcore_read_otp_info(e_ctrl);
	gc5025_gcore_enable_otp(e_ctrl, otp_close);


	sensor_module_id = gc5025_otp_info.module_id;
	if (sensor_module_id != 0) {
		pr_err("sensor_module_id =0x%x\n", sensor_module_id);
		parse_module_name(e_ctrl, GC5025_MODULE_MAP,
				  sizeof(GC5025_MODULE_MAP) / sizeof(MODULE_Map_Table), sensor_module_id);
	}
	block->mapdata[0] = gc5025_otp_info.module_id;
	block->mapdata[1] = gc5025_otp_info.lens_id;

	pr_info("%s end\n", __func__);
	return rc;
}
static int zte_eeprom_generate_map(struct device_node *of,
								   struct msm_eeprom_memory_block_t *data)
{
	int i, rc = 0;
	char property[PROPERTY_MAXSIZE];
	uint32_t count = 6;
	struct msm_eeprom_memory_map_t *map;

	snprintf(property, PROPERTY_MAXSIZE, "zte,num-blocks");
	rc = of_property_read_u32(of, property, &data->num_map);
	CDBG("%s: %s %d\n", __func__, property, data->num_map);
	if (rc < 0) {
		pr_err("%s failed rc %d\n", __func__, rc);
		return rc;
	}
	map = kzalloc((sizeof(*map) * data->num_map), GFP_KERNEL);
	if (!map) {
		pr_err("%s failed line %d\n", __func__, __LINE__);
		return -ENOMEM;
	}
	data->map = map;
	for (i = 0; i < data->num_map; i++) {
		snprintf(property, PROPERTY_MAXSIZE, "zte,mem%d", i);
		rc = of_property_read_u32_array(of, property,
										(uint32_t *) &map[i].mem, count);
		if (rc < 0) {
			pr_err("%s failed %d\n", __func__, __LINE__);
			goto ERROR;
		}
		data->num_data += map[i].mem.valid_size;
	}
	CDBG("%s num_bytes %d\n", __func__, data->num_data);
	data->mapdata = kzalloc(data->num_data, GFP_KERNEL);
	if (!data->mapdata) {
		pr_err("%s failed line %d\n", __func__, __LINE__);
		rc = -ENOMEM;
		goto ERROR;
	}
	return rc;
ERROR:
	kfree(data->map);
	memset(data, 0, sizeof(*data));
	return rc;
}
static int msm_eeprom_i2c_probe(struct i2c_client *client,
								const struct i2c_device_id *id)
{
	int rc = 0;
	int j = 0;
	uint32_t temp;
	struct msm_eeprom_ctrl_t *e_ctrl = NULL;
	struct msm_camera_power_ctrl_t *power_info = NULL;
	struct msm_eeprom_board_info *eb_info = NULL;
	struct device_node *of_node = client->dev.of_node;

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		pr_err("%s i2c_check_functionality failed\n", __func__);
		goto probe_failure;
	}

	e_ctrl = kzalloc(sizeof(*e_ctrl), GFP_KERNEL);
	if (!e_ctrl) {
		pr_err("%s:%d kzalloc failed\n", __func__, __LINE__);
		return -ENOMEM;
	}
	e_ctrl->eeprom_v4l2_subdev_ops = &msm_eeprom_subdev_ops;
	e_ctrl->eeprom_mutex = &msm_eeprom_mutex;
	e_ctrl->eboard_info = kzalloc(sizeof(
									  struct msm_eeprom_board_info), GFP_KERNEL);
	if (!e_ctrl->eboard_info) {
		pr_err("%s failed line %d\n", __func__, __LINE__);
		rc = -ENOMEM;
		goto ectrl_free;
	}
	e_ctrl->is_supported = 0;
	if (!client->dev.of_node) {
		pr_err("%s dev.of_node NULL\n", __func__);
		return -EINVAL;
	}

	rc = of_property_read_u32(client->dev.of_node, "cell-index",
							  &e_ctrl->subdev_id);
	CDBG("cell-index %d, rc %d\n", e_ctrl->subdev_id, rc);
	if (rc < 0) {
		pr_err("failed rc %d\n", rc);
		return rc;
	}

	rc = of_property_read_u32(client->dev.of_node, "qcom,slave-addr",
							  &temp);
	if (rc < 0) {
		pr_err("%s failed rc %d\n", __func__, rc);
		return rc;
	}

	eb_info = e_ctrl->eboard_info;
	power_info = &e_ctrl->eboard_info->power_info;
	e_ctrl->i2c_client.client = client;
	eb_info->i2c_slaveaddr = temp;

	/* Set device type as I2C */
	e_ctrl->eeprom_device_type = MSM_CAMERA_I2C_DEVICE;
	e_ctrl->i2c_client.i2c_func_tbl = &msm_eeprom_qup_func_tbl;

	rc = of_property_read_string(client->dev.of_node, "qcom,eeprom-name",
								 &eb_info->eeprom_name);
	pr_err("%s qcom,eeprom-name %s, rc %d\n", __func__, eb_info->eeprom_name, rc);
	if (rc < 0) {
		pr_err("%s failed %d\n", __func__, __LINE__);
		goto i2c_board_free;
	}

	if (e_ctrl->eboard_info->i2c_slaveaddr != 0)
		e_ctrl->i2c_client.client->addr =
			e_ctrl->eboard_info->i2c_slaveaddr;
	power_info->clk_info = cam_8960_clk_info;
	power_info->clk_info_size = ARRAY_SIZE(cam_8960_clk_info);
	power_info->dev = &client->dev;

	rc = msm_eeprom_cmm_dts(e_ctrl->eboard_info, of_node);
	if (rc < 0)
		CDBG("%s MM data miss:%d\n", __func__, __LINE__);

	rc = msm_eeprom_get_dt_data(e_ctrl);
	if (rc)
		goto i2c_board_free;
#if 0
	rc = msm_eeprom_parse_memory_map(of_node, &e_ctrl->cal_data);
#else
	rc = zte_eeprom_generate_map(of_node, &e_ctrl->cal_data);
#endif
	if (rc < 0)
		goto i2c_board_free;

	rc = msm_camera_power_up(power_info, e_ctrl->eeprom_device_type,
							 &e_ctrl->i2c_client);
	if (rc) {
		pr_err("failed rc %d\n", rc);
		goto i2c_memdata_free;
	}
#if 0
	rc = read_eeprom_memory(e_ctrl, &e_ctrl->cal_data);
#else
	if (strcmp(eb_info->eeprom_name, "common_ar0542") == 0) {
		rc = ar0542_read_eeprom_memory(e_ctrl, &e_ctrl->cal_data);
	} else if (strcmp(eb_info->eeprom_name, "common_t4k35") == 0) {
		rc = t4k35_read_eeprom_memory(e_ctrl, &e_ctrl->cal_data);
	} else if (strcmp(eb_info->eeprom_name, "sunrise_pc0fe") == 0) {
		rc = ov5670_read_eeprom_memory(e_ctrl, &e_ctrl->cal_data);
	} else if (strcmp(eb_info->eeprom_name, "common_ov8856") == 0) {
		rc = ov8856_read_eeprom_memory(e_ctrl, &e_ctrl->cal_data);
	} else if (strcmp(eb_info->eeprom_name, "common_s5k5e8") == 0) {
		rc = s5k5e8_read_eeprom_memory(e_ctrl, &e_ctrl->cal_data);
	} else if (strcmp(eb_info->eeprom_name, "common_s5k4h8") == 0) {
		rc = s5k4h8_read_eeprom_memory(e_ctrl, &e_ctrl->cal_data);
	} else if (strcmp(eb_info->eeprom_name, "common_gc5025") == 0) {
		rc = gc5025_read_eeprom_memory(e_ctrl, &e_ctrl->cal_data);
	} else {
		pr_err("%s read_eeprom_memory not configured\n", __func__);
	}
#endif
	if (rc < 0) {
		pr_err("%s read_eeprom_memory failed\n", __func__);
		goto i2c_power_down;
	}
	for (j = 0; j < e_ctrl->cal_data.num_data; j++)
		CDBG("memory_data[%d] = 0x%X\n", j,
			 e_ctrl->cal_data.mapdata[j]);
	e_ctrl->is_supported = 1;
#if 0
	e_ctrl->is_supported |= msm_eeprom_match_crc(&e_ctrl->cal_data);
#endif
	rc = msm_camera_power_down(power_info, e_ctrl->eeprom_device_type,
							   &e_ctrl->i2c_client);
	if (rc) {
		pr_err("failed rc %d\n", rc);
		goto i2c_memdata_free;
	}
	/*IMPLEMENT READING PART*/
	/* Initialize sub device */
	v4l2_i2c_subdev_init(&e_ctrl->msm_sd.sd,
						 e_ctrl->i2c_client.client,
						 e_ctrl->eeprom_v4l2_subdev_ops);
	v4l2_set_subdevdata(&e_ctrl->msm_sd.sd, e_ctrl);
	e_ctrl->msm_sd.sd.internal_ops = &msm_eeprom_internal_ops;
	e_ctrl->msm_sd.sd.flags |= V4L2_SUBDEV_FL_HAS_DEVNODE;
	snprintf(e_ctrl->msm_sd.sd.name,
			 ARRAY_SIZE(e_ctrl->msm_sd.sd.name), "msm_eeprom");
	media_entity_init(&e_ctrl->msm_sd.sd.entity, 0, NULL, 0);
	e_ctrl->msm_sd.sd.entity.type = MEDIA_ENT_T_V4L2_SUBDEV;
	e_ctrl->msm_sd.sd.entity.group_id = MSM_CAMERA_SUBDEV_EEPROM;
	msm_sd_register(&e_ctrl->msm_sd);
	e_ctrl->is_supported = (e_ctrl->is_supported << 1) | 1;
	return rc;

i2c_power_down:
	msm_camera_power_down(power_info, e_ctrl->eeprom_device_type,
						  &e_ctrl->i2c_client);
i2c_memdata_free:
	kfree(e_ctrl->cal_data.mapdata);
	kfree(e_ctrl->cal_data.map);
i2c_board_free:
	kfree(e_ctrl->eboard_info);
ectrl_free:
	kfree(e_ctrl);
probe_failure:
	pr_err("%s failed! rc = %d\n", __func__, rc);
	return rc;
}

static int msm_eeprom_i2c_remove(struct i2c_client *client)
{
	struct v4l2_subdev *sd = i2c_get_clientdata(client);
	struct msm_eeprom_ctrl_t  *e_ctrl;

	if (!sd) {
		pr_err("%s: Subdevice is NULL\n", __func__);
		return 0;
	}

	e_ctrl = (struct msm_eeprom_ctrl_t *)v4l2_get_subdevdata(sd);
	if (!e_ctrl) {
		pr_err("%s: eeprom device is NULL\n", __func__);
		return 0;
	}

	kfree(e_ctrl->cal_data.mapdata);
	kfree(e_ctrl->cal_data.map);
	if (e_ctrl->eboard_info) {
		kfree(e_ctrl->eboard_info->power_info.gpio_conf);
		kfree(e_ctrl->eboard_info);
	}
	kfree(e_ctrl);
	return 0;
}

static const struct of_device_id msm_eeprom_i2c_dt_match[] = {
	{ .compatible = "qcom,eeprom" },
	{ }
};

MODULE_DEVICE_TABLE(of, msm_eeprom_i2c_dt_match);

static const struct i2c_device_id msm_eeprom_i2c_id[] = {
	{ "msm_eeprom", (kernel_ulong_t)NULL},
	{ }
};

static struct i2c_driver msm_eeprom_i2c_driver = {
	.id_table = msm_eeprom_i2c_id,
	.probe  = msm_eeprom_i2c_probe,
	.remove = msm_eeprom_i2c_remove,
	.driver = {
		.name = "msm_eeprom",
		.owner = THIS_MODULE,
		.of_match_table = msm_eeprom_i2c_dt_match,
	},
};

static int __init msm_eeprom_init_module(void)
{
	int rc = 0;

	pr_err("%s E\n", __func__);

	rc = i2c_add_driver(&msm_eeprom_i2c_driver);
	pr_err("%s:%d i2c rc %d\n", __func__, __LINE__, rc);
	return rc;
}

static void __exit msm_eeprom_exit_module(void)
{
	CDBG("%s E\n", __func__);
	i2c_del_driver(&msm_eeprom_i2c_driver);
}

module_init(msm_eeprom_init_module);
module_exit(msm_eeprom_exit_module);
MODULE_DESCRIPTION("MSM EEPROM driver");
MODULE_LICENSE("GPL v2");
