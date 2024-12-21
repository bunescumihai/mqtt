#include "dd_thermistor.h"
#include "Arduino.h"

float dd_thermistor_get_temp_K(Thermistor *thermistor)
{

    int analogValue = analogRead(thermistor->thermistor_pin);
    float voltage = analogValue * 5.0 / 1023.0;
    float temperature = ((voltage / 5.0) * 10000.0) / (1.0 - (voltage / 5.0));

    return 1.0 / ((1.0 / 298.15) + (1.0 / 3950.0) * log(temperature / 10000.0));
}

float dd_thermistor_get_temp_C(Thermistor *thermistor)
{
    return dd_thermistor_get_temp_K(thermistor) - 273.15;
}




Thermistor::Thermistor(char pin) : thermistor_pin(pin){}