/*
 * cx2092x.h -- CX20921 and CX20924 Audio driver
 *
 * Copyright:   (C) 2017 Conexant Systems, Inc.
 *
 * This is based on Alexander Sverdlin's CS4271 driver code.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#ifndef __CX2092X_PRIV_H__
#define __CX2092X_PRIV_H__

#include <linux/regmap.h>

#define CX2092X_CMD_GET(item)   ((item) |  0x0100)
#define CX2092X_CMD_SIZE 13
#define CX2092X_APP_ID_CAPT (((('C')-0x20)<<8)|((('A')-0x20)<<14)|((('P')-0x20)<<20)|((('T')-0x20)<<26))
#define CX2092X_SPATIAL_SEP2_CMD_INTERNAL_STATE 0x0042


#define SUPPORT_CX2092X_MISC_DRV



/*
 * Defines the command format which is used to communicate with cx2092x device.
 */
struct cx2092x_cmd {
    int num_32b_words:16;   /* Indicates how many data to be sent.
                     * If operation is successful, this will
                     * be updated with the number of returned
                     * data in word. one word == 4 bytes.
                     */

    u32 command_id:15;
    u32 reply:1;            /* The device will set this flag once
                     * the operation is complete.
                     */
    u32 app_module_id;
    u32 data[CX2092X_CMD_SIZE]; /* Used for storing parameters and
                     * receiving the returned data from
                     * device.
                     */
};

/* codec private data*/
struct cx2092x_priv {
    struct device *dev;
    struct regmap *regmap;
#if 1 // hjkoh
    int gpio_reset;
    int gpio_pwr_enable_3p3;
    int gpio_pwr_enable_1p8;
#else
    struct gpio_desc *gpiod_reset;
#endif
    struct cx2092x_cmd cmd;
    int cmd_res;

#ifdef SUPPORT_CX2092X_MISC_DRV
	struct miscdevice miscdev;
    struct snd_soc_codec *codec;
#endif /* SUPPORT_CX2092X_MISC_DRV */
};



extern const struct of_device_id cx2092x_dt_ids[];
extern const struct regmap_config cx2092x_regmap_config;

int cx2092x_dev_probe(struct device *dev, struct regmap *regmap);
#if 1 // hjkoh
int cx2092x_dev_remove(struct device *dev);
#endif

#define CX2092X_REG_MAX 0x2000

#endif
