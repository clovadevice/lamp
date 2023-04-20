
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/pm.h>
#include <linux/i2c.h>
#include <linux/gpio.h>
#include <linux/regulator/consumer.h>
#include <linux/firmware.h>
#include <linux/regmap.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/slab.h>
#include <linux/syscalls.h>
#include <linux/fcntl.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>

#include "cx2092x.h"
#include "cx2092x-misc.h"

#define MODULE_NAME "cx2092x-misc"

extern int cx2092x_sendcmd(struct snd_soc_codec *codec,
               struct cx2092x_cmd *cmd);
extern struct cx2092x_priv* get_cx2092x_drvdata(void);


static int get_angle_of_arrival(struct cx2092x_priv *cx2092x, unsigned int  *angle)
{
    int ret = 0;
    struct cx2092x_cmd cmd = {
        .app_module_id = (CX2092X_APP_ID_CAPT | 30),
        .command_id = CX2092X_CMD_GET(CX2092X_SPATIAL_SEP2_CMD_INTERNAL_STATE),
        .reply = 0,
        .num_32b_words = 0,
    };

    if(!cx2092x->codec){
        pr_err("[%s] codec driver not set\n", __func__);
        return -1;
    }

    ret = cx2092x_sendcmd(cx2092x->codec, &cmd);
    if(ret < 0){
        pr_err("[%s] cx2092x_sendcmd() failed\n", __func__);
        return -1;
    }

    if(cmd.num_32b_words != 7){
        pr_err("[%s] cx2092x_sendcmd() failed - command has wrong number of return values:%d\n", __func__, cmd.num_32b_words);
        return -1;
    }

    *angle = cmd.data[2];

    pr_debug("[%s] num_32b_words:%d\n", __func__, cmd.num_32b_words);
    pr_debug("[%s] data[2]:%08x\n", __func__, cmd.data[2]);
    pr_debug("[%s] angle_of_arrival[2]:%d\n", __func__, ((int)(cmd.data[2]))/(1<<23));

    return 0;
}


static int cx2092x_get_mic_rms(struct cx2092x_priv *cx2092x, unsigned int type, struct cx2092_mic_rms *mic_rms)
{
    int ret = 0;

	struct cx2092x_cmd cmd;
	unsigned int mic1=0, mic2=0;

	if(!cx2092x->codec){
        pr_err("[%s] codec driver not set\n", __func__);
        return -1;
    }

	cmd.reply = 0,
	cmd.num_32b_words = 4;
	
	switch(type) {
	case CX2092X_IOCTL_GET_RMS_BE:
		cmd.app_module_id = 0xD3086328;
        cmd.command_id = 0x00000142;
		break;
    case CX2092X_IOCTL_GET_RMS_AF:
		cmd.app_module_id = 0xCB4C2313;
        cmd.command_id = 0x00000142;
		break;
	}
		
	ret = cx2092x_sendcmd(cx2092x->codec, &cmd);
    if(ret < 0){
        pr_err("[%s] cx2092x_sendcmd() failed\n", __func__);
        return -1;
    }

	switch(type) {
    case CX2092X_IOCTL_GET_RMS_BE:
		mic1 = cmd.data[0];
        mic2 = cmd.data[1];
		break;
	case CX2092X_IOCTL_GET_RMS_AF:
		mic1 = cmd.data[1];
        mic2 = cmd.data[2];
		break;
	}

	mic_rms->mic1 = mic1;
	mic_rms->mic2 = mic2;		

	pr_info("[%s] %08x %08x\n", __func__, mic1, mic2);
	pr_info("[%s] %08x %08x %08x\n", __func__, cmd.data[0], cmd.data[1], cmd.data[2]);
    return 0;
}



static ssize_t cx2092x_misc_read(struct file *file, char *buf,
                        size_t count, loff_t *ppos)
{
	return count;
}

static ssize_t cx2092x_misc_write(struct file *file, const char __user *buf,
                        size_t count, loff_t *ppos)
{
	
	return count;
}


static int cx2092x_misc_open(struct inode *inode, struct file *file)
{
	pr_info("[%s]\n", __func__);
	return 0;
}

static int cx2092x_misc_close(struct inode *inodep, struct file *file)
{
	pr_info("[%s]\n", __func__);
	return 0;
}


static long cx2092x_misc_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int err=0;
	struct cx2092x_priv *cx2092x = get_cx2092x_drvdata();


	if(!cx2092x) {
		pr_info("[%s] cx2092x_priv fail...\n", __func__);
		return -EFAULT;
	}

	/*
	 * extract the type and number bitfields, and don't decode
	 * wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok() 
	 */
	if (_IOC_TYPE(cmd) != CX2092X_IOCTL_MAGIC_NUMBER) return -ENOTTY;
	if (_IOC_NR(cmd) > CX2092X_IOCTL_MAXNR) return -ENOTTY;

	/*
	 * the direction is a bitmask, and VERIFY_WRITE catches R/W
	 * transfers. `Type' is user-oriented, while
	 * access_ok is kernel-oriented, so the concept of "read" and
	 * "write" is reversed
	 */
	if (_IOC_DIR(cmd) & _IOC_READ)
		err = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
	else if (_IOC_DIR(cmd) & _IOC_WRITE)
		err = !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
	if (err) return -EFAULT;


	switch(cmd){
	case CX2092X_IOCTL_GET_RMS_BE:
	case CX2092X_IOCTL_GET_RMS_AF:
		{
			struct cx2092_mic_rms mic_rms;
			if(cx2092x_get_mic_rms(cx2092x, cmd, &mic_rms) != 0)
				return -EFAULT;
			
			pr_info("[%s] mic1: %08x / mic2: %08x\n", __func__, mic_rms.mic1, mic_rms.mic2 );

			if (copy_to_user((void __user *)arg, &mic_rms, sizeof(mic_rms)))
                return -EFAULT;	
		}
		break;
	case CX2092X_IOCTL_GET_ANGLE_OF_ARRIVAL:
		{
			unsigned int angle = 0;
			if(get_angle_of_arrival(cx2092x, &angle) != 0)
				return -EFAULT;
			if (copy_to_user((void __user *)arg, &angle, sizeof(unsigned int)))
				return -EFAULT;
		}
		break;
	
	default:
		break;
	}

	return 0;
}

static const struct file_operations cx2092x_misc_fops = {
    .owner = THIS_MODULE,
    .read = cx2092x_misc_read,
    .write = cx2092x_misc_write,
	.unlocked_ioctl = cx2092x_misc_ioctl,
    .open = cx2092x_misc_open,
    .release = cx2092x_misc_close,
};


struct miscdevice cx2092x_misc_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = MODULE_NAME,
    .fops = &cx2092x_misc_fops,
};

static int __init cx2092x_misc_init(void)
{
    int ret = 0;

    ret = misc_register(&cx2092x_misc_device);
    if(ret) {
        pr_err("[%s] misc register fail...\n", __func__);
        return ret;
    }

    return 0;
}

static void __exit cx2092x_misc_exit(void)
{
    misc_deregister(&cx2092x_misc_device);
}

module_init(cx2092x_misc_init)
module_exit(cx2092x_misc_exit)

MODULE_DESCRIPTION("ASoC CX2092X MISC Driver");
MODULE_AUTHOR("Woo Seong Sim <wsshim@markt.co.kr>");
MODULE_LICENSE("GPL");

