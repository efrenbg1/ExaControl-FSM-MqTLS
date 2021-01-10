#include "headers.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085.h>

Adafruit_BMP085 BMP180;

float read_temp()
{
    float temp = -1;
    digitalWrite(BMP180_VCC, HIGH);
    delay(150);
    if (BMP180.begin())
        temp = BMP180.readTemperature();
    digitalWrite(BMP180_VCC, LOW);
    return temp;
}