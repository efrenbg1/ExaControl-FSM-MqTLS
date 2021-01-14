#ifndef NTPSYNC_H
#define NTPSYNC_H

#include <Arduino.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

class NTPsync
{

public:
    NTPsync(const char *, int);
    int get();

private:
    WiFiUDP ntpUDP;
    NTPClient ntp;
    unsigned long last = 0;
    int time = -1;
};

#endif