#ifndef HEADERS_H
#define HEADERS_H

#include <MqTLS.h>
#include <ExaControl.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>

#define t_status "0"
#define t_ONtemp "1"
#define t_OFFtemp "2"
#define t_target "3"
#define t_ONtime "4"
#define t_OFFtime "5"
#define t_temp "6"

#define BMP180_VCC 13
#define bajar 12
#define subir 14
#define LED 2

extern String topic;
extern MqTLS mqtls;
extern ExaControl control;
extern int OFFtime;
extern int ONtime;
extern float ONtemp;
extern float OFFtemp;

//void mq_init();
void mq_callback();
float read_temp();
void fsm_init();
void fsm_retrieve();
void fsm_callback();
bool setup_wifi();

#endif