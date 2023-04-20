
//#define DEBUG

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/pm.h>
#include <linux/clk.h>
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
#include <linux/workqueue.h>
#include <linux/uaccess.h>
#include <linux/interrupt.h>

#include "ledanim.h"

#if defined(CONFIG_TARGET_PRODUCT_IF_S1200N)
#define LED_PIN_COUNT 3
#else
#define LED_PIN_COUNT 12
#endif

//#define ENABLE_CALIBRATE_PWM
//#define ENABLE_SECOND_LINE
#define USE_PM_QOS
//#define PRINT_LED_UPDATE_LATENCY

#if defined(USE_PM_QOS)
#define PM_QOS_VALUE	(200)
#include <linux/pm_qos.h>
#endif

// LED On/Off Register
#define ET6312B_CHIPCTR_ADDR				0x00	// enable/disable shutdown
#define ET6312B_SOFTDN_EN_ENABLE			0x00
#define ET6312B_SOFTDN_EN_DISABLE			0x10

#define ET6312B_LEDXMD_RGB_LED1_REG_ADDR	0x16
#define ET6312B_LEDXMD_RGB_LED2_REG_ADDR	0x17
#define ET6312B_LEDXMD_RGB_LED3_REG_ADDR	0x18
#define ET6312B_LEDXMD_RGB_LED4_REG_ADDR	0x19
#define ET6312B_LEDXMD_RGB_ON				0x25

#define ET6312B_LED1_CURT_REG_ADDR			0x1A

/*
// Page
#define IS3737_PAGE_LED_CONTROL_REGISTER	0
#define IS3737_PAGE_PWM_REGISTER			1
#define IS3737_PAGE_ABM_REGISTER			2
#define IS3737_PAGE_FUNCTION_REGISTER		3

// REGISTER
#define IS3737_COMMAND_REG_ADDR				0xFD
#define IS3737_COMMAND_REG_LOCK_ADDR		0xFE
#define IS3737_INTERRUPT_MASK_REG_ADDR		0xF0
#define IS3737_INTERRUPT_STATUS_REG_ADDR	0xF1

// UNLOCK
#define IS3737_COMMAND_REG_UNLOCK			0xC5

// LED On/Off Register
#define IS3737_LED_ON_OFF_REG_START_ADDR	0x00
#define IS3737_LED_ON_OFF_REG_COUNT			24

// Function register
#define IS3737_CONFIG_REG_ADDR				0x00
#define IS3737_GLOBAL_CURRENT_REG_ADDR		0x01
#define IS3737_SWY_PULLUP_REG_ADDR			0x0F
#define IS3737_CSX_PULLDOWN_REG_ADDR		0x10

#define IS3737_GLOBAL_CURRENT_VALUE			0xFF

#define MAX_SIZE 4096

static unsigned char led_enable_map_for_champ[IS3737_LED_ON_OFF_REG_COUNT] = {
#if defined(ENABLE_SECOND_LINE)
	0x3f, 0x00, 0x3f, 0x00, 0x3f, 0x00, 0x3f, 0x00,
	0x3f, 0x00, 0x3f, 0x00, 0x3f, 0x00, 0x3f, 0x00,
	0x3f, 0x00, 0x3f, 0x00, 0x3f, 0x00, 0x3f, 0x00	
#else
	0x3f, 0x00, 0x00, 0x00, 0x3f, 0x00, 0x00, 0x00,
	0x3f, 0x00, 0x00, 0x00, 0x3f, 0x00, 0x00, 0x00,
	0x3f, 0x00, 0x00, 0x00, 0x3f, 0x00, 0x00, 0x00
#endif
};*/

//static unsigned char led_pwm_reg_map_for_champ[LED_PIN_COUNT] = {
//	0x02, 0x22, 0x42,	/* LED505 */
//	0x01, 0x21, 0x41,	/* LED503 */
//	0x00, 0x20, 0x40,	/* LED501 */
//	0x60, 0x80, 0xA0,	/* LED500 */
//	0x61, 0x81, 0xA1,	/* LED502 */
//	0x62, 0x82, 0xA2,	/* LED504 */
//	0x63, 0x83, 0xA3,	/* LED506 */
//	0x64, 0x84, 0xA4,	/* LED508 */
//	0x65, 0x85, 0xA5,	/* LED5010 */
//	0x05, 0x25, 0x45,	/* LED5011 */
//	0x04, 0x24, 0x44,	/* LED509 */
//	0x03, 0x23, 0x43	/* LED507 */
//};

static unsigned char calpwm_tbl_red[] = { /* 0xBF */
	  0,   1,   1,   2,   3,   4,   4,   5,   6,   7,   7,   8,   9,  10,  10,  11, 
	 12,  13,  13,  14,  15,  16,  16,  17,  18,  19,  19,  20,  21,  22,  22,  23, 
	 24,  25,  25,  26,  27,  28,  28,  29,  30,  31,  31,  32,  33,  34,  34,  35, 
	 36,  37,  37,  38,  39,  40,  40,  41,  42,  43,  43,  44,  45,  46,  46,  47, 
	 48,  49,  49,  50,  51,  52,  52,  53,  54,  55,  55,  56,  57,  58,  58,  59, 
	 60,  61,  61,  62,  63,  64,  64,  65,  66,  67,  67,  68,  69,  70,  70,  71, 
	 72,  73,  73,  74,  75,  76,  76,  77,  78,  79,  79,  80,  81,  82,  82,  83, 
	 84,  85,  85,  86,  87,  88,  88,  89,  90,  91,  91,  92,  93,  94,  94,  95, 
	 96,  96,  97,  98,  99,  99, 100, 101, 102, 102, 103, 104, 105, 105, 106, 107, 
	108, 108, 109, 110, 111, 111, 112, 113, 114, 114, 115, 116, 117, 117, 118, 119, 
	120, 120, 121, 122, 123, 123, 124, 125, 126, 126, 127, 128, 129, 129, 130, 131, 
	132, 132, 133, 134, 135, 135, 136, 137, 138, 138, 139, 140, 141, 141, 142, 143, 
	144, 144, 145, 146, 147, 147, 148, 149, 150, 150, 151, 152, 153, 153, 154, 155, 
	156, 156, 157, 158, 159, 159, 160, 161, 162, 162, 163, 164, 165, 165, 166, 167, 
	168, 168, 169, 170, 171, 171, 172, 173, 174, 174, 175, 176, 177, 177, 178, 179, 
	180, 180, 181, 182, 183, 183, 184, 185, 186, 186, 187, 188, 189, 189, 190, 191
};
static unsigned char calpwm_tbl_green[] = { /* 0xBF */
	  0,   1,   1,   2,   3,   4,   4,   5,   6,   7,   7,   8,   9,  10,  10,  11, 
	 12,  13,  13,  14,  15,  16,  16,  17,  18,  19,  19,  20,  21,  22,  22,  23, 
	 24,  25,  25,  26,  27,  28,  28,  29,  30,  31,  31,  32,  33,  34,  34,  35, 
	 36,  37,  37,  38,  39,  40,  40,  41,  42,  43,  43,  44,  45,  46,  46,  47, 
	 48,  49,  49,  50,  51,  52,  52,  53,  54,  55,  55,  56,  57,  58,  58,  59, 
	 60,  61,  61,  62,  63,  64,  64,  65,  66,  67,  67,  68,  69,  70,  70,  71, 
	 72,  73,  73,  74,  75,  76,  76,  77,  78,  79,  79,  80,  81,  82,  82,  83, 
	 84,  85,  85,  86,  87,  88,  88,  89,  90,  91,  91,  92,  93,  94,  94,  95, 
	 96,  96,  97,  98,  99,  99, 100, 101, 102, 102, 103, 104, 105, 105, 106, 107, 
	108, 108, 109, 110, 111, 111, 112, 113, 114, 114, 115, 116, 117, 117, 118, 119, 
	120, 120, 121, 122, 123, 123, 124, 125, 126, 126, 127, 128, 129, 129, 130, 131, 
	132, 132, 133, 134, 135, 135, 136, 137, 138, 138, 139, 140, 141, 141, 142, 143, 
	144, 144, 145, 146, 147, 147, 148, 149, 150, 150, 151, 152, 153, 153, 154, 155, 
	156, 156, 157, 158, 159, 159, 160, 161, 162, 162, 163, 164, 165, 165, 166, 167, 
	168, 168, 169, 170, 171, 171, 172, 173, 174, 174, 175, 176, 177, 177, 178, 179, 
	180, 180, 181, 182, 183, 183, 184, 185, 186, 186, 187, 188, 189, 189, 190, 191
};
static unsigned char calpwm_tbl_blue[] = { /* 0xBF */
	  0,   1,   1,   2,   3,   4,   4,   5,   6,   7,   7,   8,   9,  10,  10,  11, 
	 12,  13,  13,  14,  15,  16,  16,  17,  18,  19,  19,  20,  21,  22,  22,  23, 
	 24,  25,  25,  26,  27,  28,  28,  29,  30,  31,  31,  32,  33,  34,  34,  35, 
	 36,  37,  37,  38,  39,  40,  40,  41,  42,  43,  43,  44,  45,  46,  46,  47, 
	 48,  49,  49,  50,  51,  52,  52,  53,  54,  55,  55,  56,  57,  58,  58,  59, 
	 60,  61,  61,  62,  63,  64,  64,  65,  66,  67,  67,  68,  69,  70,  70,  71, 
	 72,  73,  73,  74,  75,  76,  76,  77,  78,  79,  79,  80,  81,  82,  82,  83, 
	 84,  85,  85,  86,  87,  88,  88,  89,  90,  91,  91,  92,  93,  94,  94,  95, 
	 96,  96,  97,  98,  99,  99, 100, 101, 102, 102, 103, 104, 105, 105, 106, 107, 
	108, 108, 109, 110, 111, 111, 112, 113, 114, 114, 115, 116, 117, 117, 118, 119, 
	120, 120, 121, 122, 123, 123, 124, 125, 126, 126, 127, 128, 129, 129, 130, 131, 
	132, 132, 133, 134, 135, 135, 136, 137, 138, 138, 139, 140, 141, 141, 142, 143, 
	144, 144, 145, 146, 147, 147, 148, 149, 150, 150, 151, 152, 153, 153, 154, 155, 
	156, 156, 157, 158, 159, 159, 160, 161, 162, 162, 163, 164, 165, 165, 166, 167, 
	168, 168, 169, 170, 171, 171, 172, 173, 174, 174, 175, 176, 177, 177, 178, 179, 
	180, 180, 181, 182, 183, 183, 184, 185, 186, 186, 187, 188, 189, 189, 190, 191
};

struct et6312b_priv {
	struct device *dev;
	struct i2c_client *client;
	struct regmap *regmap;

	int led_enable_gpio;

	char is_power_on;
	char request_power_on;

	unsigned char led_brightness[LED_PIN_COUNT];

	struct work_struct	led_update_work;
	struct work_struct	led_power_enable_work;

	// unsigned char max_current; // del
	struct mutex mutex;

#if defined(USE_PM_QOS)
	struct pm_qos_request pm_qos_request;
#endif
};



static bool et6312b_volatile(struct device *dev, unsigned int reg)
{
	return true;
}

static bool et6312b_writeable(struct device *dev, unsigned int reg)
{
	return true;
}

static const struct regmap_config et6312b_i2c_regmap = {
	.reg_bits = 8,
	.val_bits = 8,
	.writeable_reg = et6312b_writeable,
	.volatile_reg = et6312b_volatile,
	.cache_type = REGCACHE_NONE,
	.max_register = 128,
};

static int et6312b_i2c_write_device(struct et6312b_priv *priv,
				    unsigned char reg,
				    unsigned char value)
{
	int rc = 0;
	
	rc = regmap_write(priv->regmap, reg, value);
	//rc = i2c_smbus_write_byte_data(priv->client, reg, value);
	if (rc < 0) {
		dev_err(priv->dev, "%s Error reg:0x%02x\n", __func__, reg);
	} else {
		dev_dbg(priv->dev, "%s OK reg:0x%02x value:%d OK\n", __func__, reg, value);
	}

	return rc;
}

static int et6312b_set_led_on(struct et6312b_priv *priv, int on)
{
	int i = 0;

	mutex_lock(&priv->mutex);
	
	if(on){
		et6312b_i2c_write_device(priv, ET6312B_CHIPCTR_ADDR, ET6312B_SOFTDN_EN_DISABLE);

		for(i = 0; i < (LED_PIN_COUNT / 3); i++){
			et6312b_i2c_write_device(priv, ET6312B_LEDXMD_RGB_LED1_REG_ADDR + i, ET6312B_LEDXMD_RGB_ON);
		}

		/*et6312b_i2c_write_device(priv, 0x16, 0x25);*/

		/*et6312b_i2c_write_device(priv, 0x1A, 0x0);
		et6312b_i2c_write_device(priv, 0x1B, 0xBF);
		et6312b_i2c_write_device(priv, 0x1C, 0x0);*/
	} else {
		et6312b_i2c_write_device(priv, ET6312B_CHIPCTR_ADDR, ET6312B_SOFTDN_EN_ENABLE);
	}
	mutex_unlock(&priv->mutex);
	return 0;
}

static int et6312b_set_led_pwm(struct et6312b_priv *priv)
{
	int i;
	uint8_t rgb = 0x0;

	mutex_lock(&priv->mutex);

	for(i = 0 ; i < LED_PIN_COUNT; i+=3){
		et6312b_i2c_write_device(priv, ET6312B_LED1_CURT_REG_ADDR + i, calpwm_tbl_red[priv->led_brightness[i]]);
		et6312b_i2c_write_device(priv, ET6312B_LED1_CURT_REG_ADDR + (i+1), calpwm_tbl_green[priv->led_brightness[i+1]]);
		et6312b_i2c_write_device(priv, ET6312B_LED1_CURT_REG_ADDR + (i+2), calpwm_tbl_blue[priv->led_brightness[i+2]]);
	}

	for(i = 0; i < (LED_PIN_COUNT / 3); i++){
		if (calpwm_tbl_red[priv->led_brightness[i]] == 0x0) {
			rgb = rgb | (0 << 0);
		}
		else {
			rgb = rgb | (1 << 0);
		}

		if (calpwm_tbl_green[priv->led_brightness[i+1]] == 0x0) {
			rgb = rgb | (0 << 2);
		}
		else {
			rgb = rgb | (1 << 2);
		}

		if (calpwm_tbl_blue[priv->led_brightness[i+2]] == 0x0) {
			rgb = rgb | (0 << 4);
		}
		else {
			rgb = rgb | (2 << 4);
		}

		et6312b_i2c_write_device(priv, ET6312B_LEDXMD_RGB_LED1_REG_ADDR + i, rgb);
	}

	mutex_unlock(&priv->mutex);
	return 0;
}

static int et6312b_get_led_brightness(struct device *dev, int index, unsigned char * value)
{	
	struct et6312b_priv * priv = dev_get_drvdata(dev);

	if(index < 0 || index >= LED_PIN_COUNT)
		return -1;

	*value = priv->led_brightness[index];
	return 0;
}

static int et6312b_set_led_brightness(struct device *dev, int index, unsigned char value)
{	
	struct et6312b_priv * priv = dev_get_drvdata(dev);

	if(index < 0 || index >= LED_PIN_COUNT)
		return -1;

	priv->led_brightness[index] = value;
	return 0;
}

static int et6312b_led_update(struct device *dev)
{
	struct et6312b_priv * priv = dev_get_drvdata(dev);

	if(priv)
		schedule_work(&priv->led_update_work);

	return 0;
}

static int et6312b_led_power_enable(struct device *dev, int enable)
{
	struct et6312b_priv * priv = dev_get_drvdata(dev);
	if(priv) {
		priv->request_power_on = (enable ? 1 : 0);
		schedule_work(&priv->led_power_enable_work);
	}
	return 0;
}


static void et6312b_led_update_work(struct work_struct *work)
{
	int rc;
/*#ifdef PRINT_LED_UPDATE_LATENCY
	int64_t latency; 
#endif*/
	struct et6312b_priv *priv = container_of(work, struct et6312b_priv, led_update_work);

	if(priv->is_power_on == 0){
		dev_err(priv->dev, "ET6312B is not power on\n");
		return;
	}

#if defined(USE_PM_QOS)
	pm_qos_update_request(&priv->pm_qos_request, PM_QOS_VALUE);
#endif

/*#ifdef PRINT_LED_UPDATE_LATENCY
	latency = ktime_to_ns(ktime_get());
#endif*/

	rc = et6312b_set_led_pwm(priv);
	if(rc != 0) {
#if defined(USE_PM_QOS)
		pm_qos_update_request(&priv->pm_qos_request, PM_QOS_DEFAULT_VALUE);
#endif
		dev_err(priv->dev, "ET6312B et6312b_set_led_pwm() failed\n");
		return;
	}

/*#ifdef PRINT_LED_UPDATE_LATENCY
	latency = ktime_to_ns(ktime_get()) - latency;
	latency += (NSEC_PER_MSEC / 2);
	do_div(latency, NSEC_PER_MSEC);
	dev_err(priv->dev, "IS3737 led_update_work latency:%llu msec\n", latency);
#endif	*/

#if defined(USE_PM_QOS)
	pm_qos_update_request(&priv->pm_qos_request, PM_QOS_DEFAULT_VALUE);
#endif
}


static int _et6312b_led_power_enable(struct i2c_client *client, int enable) 
{
	struct et6312b_priv * priv = i2c_get_clientdata(client);	

	if(enable){	
		if(priv->is_power_on){
			dev_dbg(priv->dev, "ET6312B already powered on\n");
			return 0;
		}
		dev_dbg(priv->dev, "led-enable gpio set to high\n");
		gpio_direction_output(priv->led_enable_gpio, 1);
		
		if(et6312b_set_led_on(priv, 1) == 0) {
			dev_dbg(priv->dev, "ET6312B power on success\n");
			priv->is_power_on = 1;
		} else {
			dev_err(priv->dev, "ET6312B power on failed");
			priv->is_power_on = 0;
			gpio_direction_output(priv->led_enable_gpio, 0);
			return -1;
		}
	} else {
		if(priv->is_power_on == 0){
			dev_dbg(priv->dev, "ET6312B already powered off\n");
			return 0;
		}
		
		et6312b_set_led_on(priv, 0);
		gpio_direction_output(priv->led_enable_gpio, 0);
		priv->is_power_on = 0;
	}

	return 0;
}

static void et6312b_led_power_enable_work(struct work_struct *work)
{
	struct et6312b_priv *priv = container_of(work, struct et6312b_priv, led_power_enable_work);
	_et6312b_led_power_enable(priv->client, (priv->request_power_on ? 1 : 0));
}


////////////////////////////////////////////////////////////////////////////
// sysfs attr
/*static ssize_t is3737_max_current_show(struct device *dev,
				 struct device_attribute *attr, char *buf)
{
	struct is3737_priv * priv = dev_get_drvdata(dev);
	if(!priv) return 0;

        return snprintf(buf, MAX_SIZE, "%u\n", priv->max_current);
}

static ssize_t is3737_max_current_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t len)
{
	struct is3737_priv * priv = dev_get_drvdata(dev);
	unsigned long value;

	if(!priv) 
		return -EINVAL;

	if (strict_strtoul(buf, 0, &value))
		return -EINVAL;

	if (value > 255)
		return -EINVAL;

	if((unsigned char)value != priv->max_current){
		priv->max_current = (unsigned char)value;
		if(priv->is_power_on){
			//is3737_apply_max_current(priv);  //del
			is3737_led_update(priv->dev);
		}
	}


	return len;
}
static DEVICE_ATTR(max_current, S_IRUGO | S_IWUSR,
		is3737_max_current_show,
		is3737_max_current_store);

static struct attribute *sysfs_attrs[] = {
	&dev_attr_max_current.attr,
	NULL
};

static struct attribute_group is3737_attribute_group = {
	.attrs = sysfs_attrs
};*/
////////////////////////////////////////////////////////////////////////////

static struct leddrv_func leddrv_ops = {
	.num_leds = LED_PIN_COUNT,
	.get_led_brightness = et6312b_get_led_brightness,
	.set_led_brightness = et6312b_set_led_brightness,
	.update = et6312b_led_update,
	.power_enable = et6312b_led_power_enable,
	//.get_max_current = is3737_get_max_current, // del
	//.set_max_current = is3737_set_max_current  // del
};


static int et6312b_i2c_probe(struct i2c_client *client,
			     const struct i2c_device_id *id)
{	
	struct et6312b_priv * priv;
	int rc;

	priv = devm_kzalloc(&client->dev, sizeof(struct et6312b_priv), GFP_KERNEL);

	if (!priv) {
		dev_err(&client->dev, " -ENOMEM\n");
		rc = -ENOMEM;
		goto err;
	}

	priv->client = client;
	priv->dev = &client->dev;
	i2c_set_clientdata(client, priv);
	dev_set_drvdata(&client->dev, priv);
	mutex_init(&priv->mutex);

	priv->regmap = devm_regmap_init_i2c(client, &et6312b_i2c_regmap);

	if (IS_ERR(priv->regmap)) {
		rc = PTR_ERR(priv->regmap );
		dev_err(&client->dev, "Failed to allocate register map: %d\n", rc);
		goto err;
	}

	if (client->dev.of_node) {
		priv->led_enable_gpio = of_get_named_gpio(client->dev.of_node, "infr,led-enable-gpio", 0);
		if (priv->led_enable_gpio != 0 && gpio_is_valid(priv->led_enable_gpio)){
			rc = gpio_request(priv->led_enable_gpio, "led-enable"); 
			if(rc){
				dev_err(&client->dev, "request led-enable gpio failed, rc=%d\n", rc); 
				goto err;
			}			
		} else {
			dev_err(&client->dev, "invalid led-enable gpio \n"); 
			rc = -EINVAL;
			goto err;
		}	
	}
	rc = 0;

	INIT_WORK(&priv->led_update_work, et6312b_led_update_work);
	INIT_WORK(&priv->led_power_enable_work, et6312b_led_power_enable_work);
	

	if(_et6312b_led_power_enable(client, 1) == 0){
		leddrv_ops.dev = priv->dev;
		if(register_leddrv_func(&leddrv_ops) != 0) {
			dev_err(&client->dev, "register_leddrv_func() failed\n"); 
		}
	}

	/*rc = sysfs_create_group(&client->dev.kobj, &is3737_attribute_group);
	if (rc < 0) {
		dev_err(&client->dev, "sysfs registration failed\n");
		goto err;
	}*/

#if defined(USE_PM_QOS)
	pm_qos_add_request(&priv->pm_qos_request, PM_QOS_CPU_DMA_LATENCY, PM_QOS_DEFAULT_VALUE);
#endif

	// _et6312b_led_power_enable(client, 1); // del

	return rc;

err:
	if (priv->led_enable_gpio != 0 && gpio_is_valid(priv->led_enable_gpio)){
		gpio_free(priv->led_enable_gpio);
		priv->led_enable_gpio = 0;
	}

	return rc;
}

static int et6312b_i2c_remove(struct i2c_client *client)
{
	struct et6312b_priv * priv = i2c_get_clientdata(client);

	dev_info(priv->dev, "%s\n", __func__);

#if defined(USE_PM_QOS)
	pm_qos_remove_request(&priv->pm_qos_request);
#endif

	//sysfs_remove_group(&priv->dev->kobj, &is3737_attribute_group);

	if (priv->led_enable_gpio != 0 && gpio_is_valid(priv->led_enable_gpio)){
		gpio_free(priv->led_enable_gpio);
		priv->led_enable_gpio = 0;
	}
	
	return 0;
}

#ifdef CONFIG_PM_SLEEP
static int et6312b_suspend(struct device *dev)
{
	struct i2c_client *client = to_i2c_client(dev);
	if(client){
		_et6312b_led_power_enable(client, 0);
	}
	return 0;
}

static int et6312b_resume(struct device *dev)
{
	struct i2c_client *client = to_i2c_client(dev);
	if(client){
		_et6312b_led_power_enable(client, 1);
	}
	return 0;
}
#endif

static const struct i2c_device_id et6312b_i2c_id[] = {
	{"et6312b", 0},
	{}
};

MODULE_DEVICE_TABLE(i2c, et6312b_i2c_id);

#ifdef CONFIG_PM_SLEEP
static const struct dev_pm_ops et6312b_pm_ops = {
	SET_SYSTEM_SLEEP_PM_OPS(et6312b_suspend, et6312b_resume)
};
#endif

#if defined(CONFIG_OF)
static const struct of_device_id et6312b_of_match[] = {
	{.compatible = "infr,et6312b"},
	{},
};

MODULE_DEVICE_TABLE(of, et6312b_of_match);
#endif

static struct i2c_driver et6312b_i2c_driver = {
	.driver = {
		.name = "et6312b",
		.owner = THIS_MODULE,
#ifdef CONFIG_PM_SLEEP
		.pm = &et6312b_pm_ops,
#endif
#if defined(CONFIG_OF)
		.of_match_table = of_match_ptr(et6312b_of_match),
#endif
	},
	.probe = et6312b_i2c_probe,
	.remove = et6312b_i2c_remove,
	.id_table = et6312b_i2c_id,
};
module_i2c_driver(et6312b_i2c_driver);

MODULE_AUTHOR("");
MODULE_DESCRIPTION("ET6312B LED IC control driver");
MODULE_LICENSE("GPL v2");
