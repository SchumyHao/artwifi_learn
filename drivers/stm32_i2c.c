#include <rtthread.h>
#include <board.h>
#include <rtdevice.h>

#if defined(RT_USING_I2C) && defined(RT_USING_I2C_BITOPS)

/* GPIO define
SCL: I2C1_SCL PC2
SDA: I2C1_SDA PC3
*/
#define GPIO_PORT_I2C1_SCL   GPIOC
#define RCC_I2C1_SCL         RCC_AHB1Periph_GPIOC
#define PIN_I2C1_SCL         GPIO_Pin_2

#define GPIO_PORT_I2C1_SDA   GPIOC
#define RCC_I2C1_SDA         RCC_AHB1Periph_GPIOC
#define PIN_I2C1_SDA         GPIO_Pin_3

struct stm32_i2c_bit_data
{
    struct {
        GPIO_TypeDef *port;
        uint16_t     pin;
    } scl, sda;
};

static void gpio_set_sda(void *data, rt_int32_t state)
{
    struct stm32_i2c_bit_data *bd = data;

    if (state)
    {
        GPIO_SetBits(bd->sda.port, bd->sda.pin);
    }
    else
    {
        GPIO_ResetBits(bd->sda.port, bd->sda.pin);
    }
}

static void gpio_set_scl(void *data, rt_int32_t state)
{
    struct stm32_i2c_bit_data *bd = data;

    if (state)
    {
        GPIO_SetBits(bd->scl.port, bd->scl.pin);
    }
    else
    {
        GPIO_ResetBits(bd->scl.port, bd->scl.pin);
    }
}

static rt_int32_t gpio_get_sda(void *data)
{
    struct stm32_i2c_bit_data *bd = data;

    return GPIO_ReadInputDataBit(bd->sda.port, bd->sda.pin);
}

static rt_int32_t gpio_get_scl(void *data)
{
    struct stm32_i2c_bit_data *bd = data;

    return GPIO_ReadInputDataBit(bd->scl.port, bd->scl.pin);
}

static void gpio_udelay(rt_uint32_t us)
{
    volatile rt_int32_t i;
    for (; us > 0; us--)
    {
        i = 10;
        while (i--);
    }
}

static const struct stm32_i2c_bit_data _i2c1_bdata = {
    /* SCL */
    {GPIO_PORT_I2C1_SCL, PIN_I2C1_SCL},
    /* SDA */
    {GPIO_PORT_I2C1_SDA, PIN_I2C1_SDA},
};

static const struct rt_i2c_bit_ops _i2c1_bit_ops =
{
    (void*)&_i2c1_bdata,
    gpio_set_sda,
    gpio_set_scl,
    gpio_get_sda,
    gpio_get_scl,

    gpio_udelay,

    5,
    100
};

static struct rt_i2c_bus_device i2c1_device;
int rt_hw_i2c_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_I2C1_SCL | RCC_I2C1_SDA, ENABLE);

    /* config SCL PIN */
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;

    GPIO_InitStructure.GPIO_Pin = PIN_I2C1_SCL;
    GPIO_SetBits(GPIO_PORT_I2C1_SCL, PIN_I2C1_SCL);
    GPIO_Init(GPIO_PORT_I2C1_SCL, &GPIO_InitStructure);

    /* config SDA PIN */
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;

    GPIO_InitStructure.GPIO_Pin = PIN_I2C1_SDA;
    GPIO_SetBits(GPIO_PORT_I2C1_SDA, PIN_I2C1_SDA);
    GPIO_Init(GPIO_PORT_I2C1_SDA, &GPIO_InitStructure);

    rt_memset((void *)&i2c1_device, 0, sizeof(i2c1_device));
    i2c1_device.priv = (void *)&_i2c1_bit_ops;
    rt_i2c_bit_add_bus(&i2c1_device, "i2c1");

    return 0;
}
INIT_BOARD_EXPORT(rt_hw_i2c_init);

#endif

