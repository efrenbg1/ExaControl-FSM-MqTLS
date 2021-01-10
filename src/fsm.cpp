#include "headers.h"
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <Ticker.h>

WiFiUDP ntpUDP;
NTPClient nclock(ntpUDP, "es.pool.ntp.org", 3600);

int OFFtime = -1;
int ONtime = -1;
float ONtemp = 18.0;
float OFFtemp = -1;

String status = "1";

void fsm_init()
{
    nclock.begin();
}

void fsm_retrieve()
{
    String retrieve;
    if (mqtls.retrieve(topic, t_ONtemp, &retrieve) == 2 && !retrieve.equals(""))
        ONtemp = retrieve.toFloat();

    if (mqtls.retrieve(topic, t_OFFtemp, &retrieve) == 2 && !retrieve.equals(""))
        OFFtemp = retrieve.toFloat();

    if (mqtls.retrieve(topic, t_ONtime, &retrieve) == 2 && !retrieve.equals(""))
        ONtime = retrieve.toInt();

    if (mqtls.retrieve(topic, t_OFFtime, &retrieve) == 2 && !retrieve.equals(""))
        OFFtime = retrieve.toInt();
}

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

    if (!nclock.update())
    {
        mqtls.publish(topic, t_status, "2");
        return;
    }

    mqtls.publish(topic, t_status, "1");

    if (OFFtime == -1 || ONtime == -1 || OFFtemp == -1)
    {
        control.set(ONtemp);
        mqtls.publish(topic, t_target, String(control.read(), 1));
        return;
    }

    int time = nclock.getHours() * 60 + nclock.getMinutes();
    Serial.print("[FSM] Time: ");
    Serial.println(time);

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

    control.set(on ? ONtemp : OFFtemp);
    mqtls.publish(topic, t_target, String(control.read(), 1));
}