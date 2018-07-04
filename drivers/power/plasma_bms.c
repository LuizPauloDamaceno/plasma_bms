/*
 * Plasma BMS driver V1
 * 
 * by electronicslover
 * 
 * 
 */

#include <linux/init.h>
#include <linux/kernel.h>
#include <asm/segment.h>
#include <asm/uaccess.h>
#include <linux/plasma_bms.h>
#include <linux/module.h>
#include <linux/power_supply.h>
#include <linux/debugfs.h>


//First of all let me see if i can read some kernel parameters:

static ssize_t plasma_parameters_exib(struct class *dev,
				struct class_attribute *attr,
				char *buf);

static ssize_t plasma_parameters_save(struct class *dev,
		struct class_attribute *attr,
        const char *buf,
        size_t count);

enum
{
	CHARGING_SATE,
	CURRENT_FLOW,
	VOLTAGE,
	TEMPERATURE,
	PERCENT,
};

static struct class_attribute plasma_attributes[] = {
	__ATTR(charging_state, S_IRUGO, plasma_parameters_exib, NULL),
	__ATTR(current_flow, S_IRUGO, plasma_parameters_exib, NULL),
	__ATTR(voltage, S_IRUGO, plasma_parameters_exib, NULL),
	__ATTR(temperature, S_IRUGO, plasma_parameters_exib, NULL),
	__ATTR(capacity, S_IRUGO, plasma_parameters_exib, NULL),
	__ATTR_NULL,
};

static ssize_t plasma_parameters_exib(struct class *dev,
				struct class_attribute *attr,
				char *buf)
{
	const ptrdiff_t off = attr - plasma_attributes;

	switch(off)
	{
	case IS_CHARGING:
		return scnprintf(buf, PAGE_SIZE, "%d\n", battery_get_property(POWER_SUPPLY_PROP_STATUS).intval == POWER_SUPPLY_STATUS_CHARGING ? 1 : 0);
	case CURRENT:
		return scnprintf(buf, PAGE_SIZE, "%d\n", battery_get_property(POWER_SUPPLY_PROP_CURRENT_NOW).intval / 1000);
	case VOLTAGE:
		return scnprintf(buf, PAGE_SIZE, "%d\n", battery_get_property(POWER_SUPPLY_PROP_VOLTAGE_NOW).intval / 1000);
	case TEMPERATURE:
		return scnprintf(buf, PAGE_SIZE, "%d\n", battery_get_property(POWER_SUPPLY_PROP_TEMP).intval);
	case PERCENT:
		return scnprintf(buf, PAGE_SIZE, "%d\n", battery_get_property(POWER_SUPPLY_PROP_CAPACITY).intval);
	}

	return 0;
}

static struct class plasma_bms_class =
{
	.name = "plasma_bms",
	.owner = THIS_MODULE,
	.class_attrs = plasma_attributes
};

static int plasma_bms_init(void)
{
	class_register(&plasma_bms_class);
	return 0;
}
static void plasma_bms_exit(void)
{
	class_unregister(&plasma_bms_class);
}

module_init(plasma_bms_init);
module_exit(plasma_bms_exit);

MODULE_AUTHOR("electronicslover <luizpauloeletrico42@gmail.com>");
MODULE_DESCRIPTION("Plasma BMS V1");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("PLASMA");


//Verify if is charging or not


//If is charging, enter in dynamic clock control (redudce all cpu clock when screen is off)
//If is charging, regulate the maximum allowed charging current according charge level
//-> Temperature control, current control, voltage control.
//If not, default all CPUs and tune it according charge: 3 levels, 65%, 35% and 15%
