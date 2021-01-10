#include "ExaControl.h"

ExaControl::ExaControl(int increse_pin, int decrease_pin)
{
    increase = increse_pin;
    decrease = decrease_pin;
    pinMode(increase, OUTPUT);
    pinMode(decrease, OUTPUT);
    digitalWrite(increase, HIGH);
    digitalWrite(decrease, HIGH);

    //ticker.once_ms(0, std::bind(&ExaControl::init, this));
}

void ExaControl::init()
{
    Serial.print("\n[ExaControl] Setting up ExaControl...");

    // Wake devide
    digitalWrite(decrease, LOW);
    delay(hold_timeout);
    digitalWrite(decrease, HIGH);
    delay(wait_timeout);

    // Go to lowest temp possible
    digitalWrite(decrease, LOW);
    delay(lowest_temp_timeout);
    digitalWrite(decrease, HIGH);

    // Last update timestamp
    last_update = millis();

    Serial.println("done");
}

void ExaControl::off()
{
    //Serial.println("off");
    digitalWrite(decrease, HIGH);
    digitalWrite(increase, HIGH);
    /*Serial.print(temp);
    Serial.print(" | ");
    Serial.println(target);*/
    if (temp != target)
        ticker.once_ms(wait_timeout, std::bind(&ExaControl::on, this));
    if (target2 != -1)
    {
        target = target2;
        target2 = -1;
        ticker.once_ms(wait_timeout, std::bind(&ExaControl::on, this));
    }
    last_update = millis();
}

void ExaControl::on()
{
    //Serial.print("on ");
    if (temp > target)
    {
        temp -= 0.5;
        digitalWrite(decrease, LOW);
    }
    else if (temp < target)
    {
        temp += 0.5;
        digitalWrite(increase, LOW);
    }
    ticker.once_ms(hold_timeout, std::bind(&ExaControl::off, this));
}

void ExaControl::set(float _target)
{
    // Round number
    int u = (int)(_target * 100) % 100;
    _target = (float)((int)_target);
    if (u >= 25 && u < 75)
        _target += 0.5;
    else if (u > 75)
        _target += 1;

    // Check limits
    if (_target > 25.0)
        _target = 25.0;
    if (_target < 15.0)
        _target = 15.0;

    Serial.print("[ExaControl] Setting target temp to ");
    Serial.print(_target);
    Serial.println(" ºC");

    if (temp != target)
    {
        //Serial.println("Updated target2");
        target2 = _target;
        return;
    }
    target = _target;
    if (temp == target)
        return;

    float timestamp = millis() - last_update;
    if (timestamp > max_screen_timeout)
    {
        //Serial.println("Over sleep");
        temp += temp > target ? 0.5 : -0.5;
        on();
    }
    else if (timestamp < min_screen_timeout)
    {
        //Serial.println("Under sleep");
        on();
    }
    else
    {
        //Serial.println("Wait for over sleep");
        float interval = max_screen_timeout - timestamp;
        temp += temp > target ? 0.5 : -0.5;
        ticker.once_ms(interval, std::bind(&ExaControl::on, this));
    }
}

float ExaControl::read()
{
    return target;
}