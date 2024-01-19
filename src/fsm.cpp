#include "headers.h"
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <Timezone.h>

WiFiUDP ntpUDP;
NTPClient ntp(ntpUDP, "pool.ntp.org", 0);
TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120}; // Central European Summer Time
TimeChangeRule CET = {"CET ", Last, Sun, Oct, 3, 60};   // Central European Standard Time
Timezone TZ(CEST, CET);

int OFFtime = -1;
int ONtime = -1;
float ONtemp = 18.0;
float OFFtemp = -1;

String status = "1";
String debug = "";
String debug2 = "";

void fsm_callback()
{
    if (ONtime > 1440 || ONtime < 0)
        ONtime = -1;
    if (OFFtime > 1440 || OFFtime < 0)
        OFFtime = -1;

    Serial.print("[FSM] ONtemp: ");
    Serial.print(ONtemp);
    Serial.print(" | OFFtemp: ");
    Serial.print(OFFtemp);
    Serial.print(" | ONtime: ");
    Serial.print(ONtime);
    Serial.print(" | OFFtime: ");
    Serial.println(OFFtime);

    if (OFFtime == -1 || ONtime == -1 || OFFtemp == -1)
    {
        control.set(ONtemp);
        mqtt.publish("home/heater/day/state", "heat", true);
        mqtt.publish("home/heater/night/state", "off", true);
        return;
    }

    ntp.update();
    time_t t = TZ.toLocal(ntp.getEpochTime());
    tm *t_struct = localtime(&t);
    int time = t_struct->tm_hour * 60 + t_struct->tm_min;
    Serial.print("[NTP] Time: ");
    Serial.println(time);
    mqtt.publish("home/heater/time", String(time).c_str(), true);

    int on = 0;

    if (time < OFFtime && time < ONtime && ONtime > OFFtime)
    {
        on = 1;
    }
    else if (time > ONtime && time < OFFtime)
    {
        on = 1;
    }
    else if (time > ONtime && time > OFFtime && ONtime > OFFtime)
    {
        on = 1;
    }

    if (on)
    {
        mqtt.publish("home/heater/day/state", "heat", true);
        mqtt.publish("home/heater/night/state", "off", true);
    }
    else
    {
        mqtt.publish("home/heater/day/state", "off", true);
        mqtt.publish("home/heater/night/state", "heat", true);
    }

    control.set(on ? ONtemp : OFFtemp);

    mqtt.publish("home/heater/thermostat", String(control.read(), 1).c_str(), true);
}