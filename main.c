#include <stdio.h>

#include <wifi.h>

#define AP_SSID              "WRTnode_Schumy"
#define AP_PASS              "63483550"
#define AP_SEC               WICED_SECURITY_WPA2_MIXED_PSK

#include <board.h>
#define LED_PORT         GPIOC
#define LED_PIN          GPIO_Pin_7
#define LED_PORT_CLK     RCC_AHB1Periph_GPIOC

#define led_hw_on()   GPIO_SetBits(LED_PORT,LED_PIN)
#define led_hw_off()  GPIO_ResetBits(LED_PORT,LED_PIN)

int main(int argc, char** argv)
{
    wiced_result_t result;

    printf("Hello RT-Thread!\n");

    {
        GPIO_InitTypeDef gpio_init_structure;

        RCC_AHB1PeriphClockCmd(LED_PORT_CLK, ENABLE );
	
        gpio_init_structure.GPIO_Speed = GPIO_Speed_2MHz;
        gpio_init_structure.GPIO_Mode = GPIO_Mode_OUT;
        gpio_init_structure.GPIO_OType = GPIO_OType_PP;
        gpio_init_structure.GPIO_PuPd = GPIO_PuPd_NOPULL;

        gpio_init_structure.GPIO_Pin = LED_PIN;

        GPIO_Init(LED_PORT, &gpio_init_structure);
    
        led_hw_on();
        //led_hw_off();
    }

    if(1)
    {
        led_hw_on();
        rt_thread_delay(RT_TICK_PER_SECOND/3);
        led_hw_off();
        rt_thread_delay(RT_TICK_PER_SECOND/3);
    }

    result = wiced_management_init(WICED_COUNTRY_CHINA, RT_NULL);
    if(result != WICED_SUCCESS)
    {
        rt_kprintf("wiced_management_init failed:%d\n", result);
        return -1;
    }

    bcm_hw_wifi_init();

#ifdef WLAN_USING_AP
    wiced_wifi_start_ap(WIFI_AP_SSID, WIFI_AP_SEC, WIFI_AP_PASS, rt_strlen(WIFI_AP_PASS), WIFI_AP_CHANNEL);
#else
    /* Attempt to join the Wi-Fi network */
    WPRINT_APP_INFO(("Joining : " AP_SSID "\r\n"));
    while ( wiced_wifi_join( AP_SSID, AP_SEC, (rt_uint8_t*) AP_PASS, sizeof( AP_PASS ) - 1, NULL ) != WICED_SUCCESS )
    {
        WPRINT_APP_INFO(("Failed to join  : " AP_SSID "   .. retrying\r\n"));
    }
    WPRINT_APP_INFO(("Successfully joined : " AP_SSID "\r\n"));
#endif

    /* power save setting. */
    {
        wiced_result_t res;

        res = wiced_wifi_enable_powersave_with_throughput( 10 );
        printf("wiced_wifi_enable_powersave_with_throughput: %d\n", res);
    }

    return 0;
}

