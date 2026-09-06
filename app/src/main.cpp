#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

/* The devicetree node identifier for the "led0" alias. */
#define LED_NODE DT_ALIAS(led0)

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED_NODE, gpios);

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

int main(void)
{
#if !IS_ENABLED(CONFIG_LED_SUBSYSTEM)
    LOG_INF("LED Subsystem is disabled in build configuration.");
    return 0;
#else
    bool led_state = true;

    if (!gpio_is_ready_dt(&led)) return 0;

    if (gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE) < 0) return 0;

    LOG_INF("LED Subsystem initialized.");

#if IS_ENABLED(CONFIG_ADVANCED_LED_SETTINGS)
    LOG_INF("Advanced Settings active | Brightness: %d%% | Fade Duration: %d ms",
            CONFIG_LED_BRIGHTNESS, CONFIG_LED_FADE_DURATION);
#endif

    while (1) {
#if IS_ENABLED(CONFIG_LED_CUSTOM_PATTERN)
        // Custom pattern: Double flash burst
        gpio_pin_set_dt(&led, 1);
        k_msleep(100);
        gpio_pin_set_dt(&led, 0);
        k_msleep(100);
        gpio_pin_set_dt(&led, 1);
        k_msleep(100);
        gpio_pin_set_dt(&led, 0);

#if IS_ENABLED(CONFIG_LED_DEBUG)
        LOG_DBG("[DEBUG] Custom blink pattern cycle executed");
#endif

#else
        // Standard toggle
        if (gpio_pin_toggle_dt(&led) < 0) return 0;

        led_state = !led_state;
        LOG_INF("LED state: %s", led_state ? "ON" : "OFF");
#endif

#if IS_ENABLED(CONFIG_LED_DEBUG)
        LOG_DBG("[DEBUG] Sleeping for %d ms", CONFIG_LED_BLINK_SLEEP_MS);
#endif

        // Sleep time driven directly from Kconfig value
        k_msleep(CONFIG_LED_BLINK_SLEEP_MS);
    }

    return 0;
#endif // CONFIG_LED_SUBSYSTEM
}
