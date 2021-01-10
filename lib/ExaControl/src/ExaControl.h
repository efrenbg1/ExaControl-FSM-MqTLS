#ifndef EXACONTROL_H
#define EXACONTROL_H

#include <Arduino.h>
#include "Ticker.h"

#define lowest_temp_timeout 14000
#define hold_timeout 100
#define wait_timeout 600
#define max_screen_timeout 120 * 1000
#define min_screen_timeout 60 * 1000
#define interval_screen_timeout 60 * 1000

class ExaControl
{
    Ticker ticker;
    int increase, decrease;
    float temp = 5.0, target = 5.0, target2 = -1;
    float last_update = millis();

public:
    ExaControl(int, int);
    void init();
    void set(float);
    float read();

private:
    void on();
    void off();
};

#endif