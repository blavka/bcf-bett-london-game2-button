#include <application.h>

#define BATTERY_UPDATE_INTERVAL (60 * 60 * 1000)

// LED instance
bc_led_t led;

// Button instance
bc_button_t button;

// This function dispatches button events
void button_event_handler(bc_button_t *self, bc_button_event_t event, void *event_param)
{
    static bc_tick_t tick_button_press;

    if (event == BC_BUTTON_EVENT_PRESS)
    {
        tick_button_press = bc_tick_get();

        bc_led_set_mode(&led, BC_LED_MODE_ON);
    }
    else if (event == BC_BUTTON_EVENT_RELEASE)
    {
        int press_time = bc_tick_get() - tick_button_press;

        bc_radio_pub_value_int(BC_RADIO_PUB_VALUE_PRESS_TIME_BUTTON, &press_time);

        bc_led_set_mode(&led, BC_LED_MODE_OFF);
    }
}

// This function dispatches battery events
void battery_event_handler(bc_module_battery_event_t event, void *event_param)
{
    // Update event?
    if (event == BC_MODULE_BATTERY_EVENT_UPDATE)
    {
        float voltage;

        // Read battery voltage
        if (bc_module_battery_get_voltage(&voltage))
        {
            // Publish battery voltage
            bc_radio_pub_battery(&voltage);
        }
    }
}

void application_init(void)
{
    // Initialize LED
    bc_led_init(&led, BC_GPIO_LED, false, false);
    bc_led_set_mode(&led, BC_LED_MODE_OFF);

    // Initialize button
    bc_button_init(&button, BC_GPIO_BUTTON, BC_GPIO_PULL_DOWN, false);
    bc_button_set_event_handler(&button, button_event_handler, NULL);

    // Initialize battery
    bc_module_battery_init();
    bc_module_battery_set_event_handler(battery_event_handler, NULL);
    bc_module_battery_set_update_interval(BATTERY_UPDATE_INTERVAL);

    // Initialize radio
    bc_radio_init(BC_RADIO_MODE_NODE_SLEEPING);

    // Send radio pairing request
    bc_radio_pairing_request("game2-button", VERSION);

    // Pulse LED
    bc_led_pulse(&led, 2000);
}
