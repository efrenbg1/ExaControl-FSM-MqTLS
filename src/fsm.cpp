#include "headers.h"
#include <WiFiUdp.h>
#include <NTPClient.h>

WiFiUDP ntpUDP;
NTPClient ntp(ntpUDP, "es.pool.ntp.org", 3600);

int OFFtime = -1;
int ONtime = -1;
float ONtemp = 18.0;
float OFFtemp = -1;

String status = "1";
String debug = "";
String debug2 = "";

void fsm_retrieve()
{
    String retrieve;
    int done = mqtls.retrieve(topic, t_ONtemp, &retrieve);
    debug2 = String(done);
    if (done == 2 && !retrieve.equals(""))
        ONtemp = retrieve.toFloat();

    done = mqtls.retrieve(topic, t_OFFtemp, &retrieve);
    debug2 += String(done);
    if (done == 2 && !retrieve.equals(""))
        OFFtemp = retrieve.toFloat();

    done = mqtls.retrieve(topic, t_ONtime, &retrieve);
    debug2 += String(done);
    if (done == 2 && !retrieve.equals(""))
        ONtime = retrieve.toInt();

    done = mqtls.retrieve(topic, t_OFFtime, &retrieve);
    debug2 += String(done);
    if (done == 2 && !retrieve.equals(""))
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

    mqtls.publish(topic, t_status, "1");

    if (OFFtime == -1 || ONtime == -1 || OFFtemp == -1)
    {
        control.set(ONtemp);
        mqtls.publish(topic, t_target, String(control.read(), 1));
        return;
    }

    if (!ntp.update())
    {
        mqtls.publish(topic, t_status, "2");
        return;
    }

    int time = ntp.getHours() * 60 + ntp.getMinutes();
    Serial.print("[NTP] Time: ");
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

    if (on == 0)
    {
        debug = debug2 + "ONtemp: " + String(ONtemp) + " | OFFtemp: " + String(OFFtemp) + " | ONtime: " + String(ONtime) + " | OFFtime: " + String(OFFtime) + " | Clock: " + String(time);
    }
    mqtls.publish(topic, t_debug, debug);
    control.set(on ? ONtemp : OFFtemp);
    mqtls.publish(topic, t_target, String(control.read(), 1));
}