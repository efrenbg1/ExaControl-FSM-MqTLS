#include "headers.h"
#include "secret.h"

ExaControl control(subir, bajar);
MqTLS mqtls(fingerprint, mq_address, mq_port, mq_user, mq_pw);

String topic = WiFi.macAddress();

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(BMP180_VCC, OUTPUT);
    digitalWrite(BMP180_VCC, LOW);
    Serial.begin(9600);
    Serial.println(read_temp());
    control.init();
    while (WiFi.status() != WL_CONNECTED)
        setup_wifi();
    fsm_init();
    mqtls.lastwill(topic, t_status, "9");
    mqtls.watch(topic);
    fsm_retrieve();
    fsm_callback();
}

int timeout = 150;
String slot, msg;
void loop()
{
    if (mqtls.callback(&slot, &msg) == 5)
    {
        if (slot == t_ONtemp)
            ONtemp = msg.toFloat();
        if (slot == t_OFFtemp)
            OFFtemp = msg.toFloat();
        if (slot == t_ONtime)
            ONtime = msg.toInt();
        if (slot == t_OFFtime)
            OFFtime = msg.toInt();
        if (slot != t_status)
            fsm_callback();
    }
    if (timeout == 0)
    {
        timeout = 150;

        // Read temp
        mqtls.publish(topic, t_temp, String(read_temp(), 1));

        // FSM
        fsm_retrieve();
        fsm_callback();
    }
    timeout--;
    delay(200);
}