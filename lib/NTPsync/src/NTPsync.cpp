#include <NTPsync.h>

NTPsync::NTPsync(const char *server, int offset) : ntp(ntpUDP, server, offset)
{
    ntp.begin();
};

int NTPsync::get()
{
    ntp.update();
    int nettime = ntp.getHours() * 60 + ntp.getMinutes();
    if (time == -1)
    {
        time = nettime;
        last = millis();
    }

    time += (int)((millis() - last) / 60000);

    Serial.print("[NTP] Time: ");
    Serial.print(time);
    Serial.print(" | Nettime: ");
    Serial.println(nettime);

    if (abs(nettime - time) > (1440 - 5))
        time = nettime;
    if (abs(nettime - time) < 5)
        time = nettime;

    last = millis();
    return time;
}