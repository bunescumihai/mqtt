#ifndef DD_THERMISTOR_H
#define DD_THERMISTOR_H

// Include any necessary libraries here

class Thermistor
{
    public: 
    Thermistor(char thermistor_pin);
    char thermistor_pin;
    bool state = 0;
};

// Declare any global variables here

float dd_thermistor_get_temp_C(struct Thermistor *thermistor);
float dd_thermistor_get_temp_K(struct Thermistor *thermistor);

#endif // DD_LED_H