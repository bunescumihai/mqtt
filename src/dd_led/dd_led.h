#ifndef DD_LED_H
#define DD_LED_H

// Include any necessary libraries here

class Led
{
    public: 
    Led(char led_pin);
    char led_pin;
    bool state = 0;
};

// Declare any global variables here

void dd_led_turn_on(struct Led *led);
void dd_led_turn_off(struct Led *led);
void dd_led_toggle(struct Led *led);
int dd_led_is_on(struct Led *led);

#endif // DD_LED_H