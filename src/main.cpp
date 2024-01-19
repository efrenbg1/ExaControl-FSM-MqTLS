#include "headers.h"
#include "secret.h"

ExaControl control(subir, bajar);

WiFiClientSecure client;
PubSubClient mqtt(client);

String topic = WiFi.macAddress();

void callback(char *topic, byte *payload, unsigned int length)
{
    String msg;
    for (unsigned int i = 0; i < length && i < 50; i++)
    {
        msg += String((char)payload[i]);
    }
    if (strcmp(topic, "home/heater/day/thermostat") == 0)
    {
        ONtemp = msg.toFloat();
        fsm_callback();
    }
    if (strcmp(topic, "home/heater/night/thermostat") == 0)
    {
        OFFtemp = msg.toFloat();
        fsm_callback();
    }
    if (strcmp(topic, "home/heater/day/time") == 0)
    {
        ONtime = msg.substring(0, 2).toInt() * 60 + msg.substring(3, 5).toInt();
        fsm_callback();
    }
    if (strcmp(topic, "home/heater/night/time") == 0)
    {
        OFFtime = msg.substring(0, 2).toInt() * 60 + msg.substring(3, 5).toInt();
        fsm_callback();
    }
}

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(BMP180_VCC, OUTPUT);
    digitalWrite(BMP180_VCC, LOW);
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.begin(9600);
    Serial.println(read_temp());
    control.init();
    while (WiFi.status() != WL_CONNECTED)
        setup_wifi();
    client.setFingerprint(fingerprint);
    mqtt.setServer(mq_server, 8883);
    mqtt.setCallback(callback);
}

int timeout = 150;
String slot, msg;
void loop()
{
    while (!client.connected())
    {
        Serial.print("Attempting MQTT connection...");
        if (mqtt.connect(WiFi.macAddress().c_str(), mq_user, mq_pw))
        {
            Serial.println("connected");
            mqtt.subscribe("home/heater/day/thermostat");
            mqtt.subscribe("home/heater/day/time");
            mqtt.subscribe("home/heater/night/thermostat");
            mqtt.subscribe("home/heater/night/time");
        }
        else
        {
            Serial.println("fail");
            delay(5000);
        }
    }

    mqtt.loop();
    if (timeout == 0)
    {
        timeout = 150;

        // Read temp
        mqtt.publish("home/heater/temp", String(read_temp(), 1).c_str(), true);

        // Update thermostat temperature
        mqtt.publish("home/heater/thermostat", String(control.read(), 1).c_str(), true);

        // FSM
        fsm_callback();
    }
    timeout--;
    delay(200);
}