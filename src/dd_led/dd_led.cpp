#include "dd_led.h"
#include "Arduino.h"

void dd_led_turn_on(Led *led)
{
    digitalWrite(led->led_pin, HIGH);
    led->state = 1;
}

void dd_led_turn_off(Led *led)
{
    digitalWrite(led->led_pin, LOW);
    led->state = 0;
}

int dd_led_is_on(Led *led){
 return led->state == 1;
}

void dd_led_toggle(Led *led)
{
    if(led->state == 1){
        led->state = 0;
        dd_led_turn_off(led);
    }else{
        dd_led_turn_on(led);
        led->state = 1;
    }
}

Led::Led(char pin) : led_pin(pin){
    state = 0;
    pinMode(led_pin, OUTPUT);  // Set the pin as OUTPUT
    dd_led_turn_off(this);
}