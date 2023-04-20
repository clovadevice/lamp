

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/io.h>
#include <linux/mm.h>
#include <linux/spinlock.h>
#include <linux/device.h>
#include <linux/ctype.h>
#include <linux/miscdevice.h>
#include <linux/interrupt.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/init.h>

#include "ledanim.h"
#include "clova_modelname.h"


static int _get_cmdline_param(const char * param, char * value, int len)
{
	char * start, *end;
	int param_length;

	start = strstr(saved_command_line, param);
	if(start == NULL){
		printk(KERN_ERR "not found '%s' in saved_command_line\n", param);
		return -EINVAL;
	}

    start += strlen(param);
	end = strchr(start, ' ');

    if(end == NULL){
		return -EINVAL;		
	}

	param_length = (int)end - (int)start; 
	if(param_length > len)
		param_length = len;

	strncpy(value, start, param_length);
	return 0;
}


int get_clova_modelname(char * modelname, int len)
{
	return _get_cmdline_param("androidboot.model=", modelname, len);
}
EXPORT_SYMBOL(get_clova_modelname);


int get_clova_board_rev(unsigned char * rev)
{
	char buf[32] = {0,};
	unsigned long value;
	unsigned char major,minor;
	char * p;
	int rc;

	rc = _get_cmdline_param("androidboot.board_rev=", buf, sizeof(buf) - 1);
	if(rc != 0){
		return rc;
	}

	p = strchr(buf, '.');
	if(!p){
		printk(KERN_ERR "Invalid board revision:%s\n", buf);
		return -EINVAL;
	}

	*p = '\0';
	if (strict_strtoul(buf, 0, &value)){
		printk(KERN_ERR "Invalid board revision:%s\n", buf);
		return -EINVAL;
	}
	major = (unsigned char)value;
	p++;
	if (strict_strtoul(p, 0, &value)){
		printk(KERN_ERR "Invalid board revision:%s\n", buf);
		return -EINVAL;
	}
	minor = (unsigned char)value;
	*rev = (((major & 0xF) << 4) | (minor & 0xF));
	return 0;
}
EXPORT_SYMBOL(get_clova_board_rev);

