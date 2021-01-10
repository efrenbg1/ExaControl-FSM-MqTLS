#ifndef MQTLS_H
#define MQTLS_H

#include <Arduino.h>
#include <WiFiClientSecure.h>

class MqTLS
{
    WiFiClientSecure client;
    String watchBuff = "", watchTopic = "", address = "", user = "", pw = "";
    String lwTopic = "", lwSlot = "", lwPayload = "";
    int port;
    bool lwatch = false;
    bool llw = false;

public:
    MqTLS(String, String, int, String, String);
    int publish(String, String, String);
    int lastwill(String, String, String);
    int watch(String);
    int retrieve(String, String, String *);
    int callback(String *, String *);
    void disconnect(void);

private:
    int connect();
    String enc(String *);
    bool send(String *);
    int receive(String *);
    int communicate(String *, String *);
};

#endif