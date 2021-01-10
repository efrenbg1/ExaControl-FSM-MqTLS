#include "MqTLS.h"

// -3 protocol error
// -2 timed out
// -1 not connected to server
//  4 watch done
int MqTLS::watch(String topic)
{
    watchTopic = topic;

    String line = "";
    String command = "MQS4" + enc(&topic);
    lwatch = true;
    int status = communicate(&command, &line);
    lwatch = false;
    if (status < 0)
        return status;
    if (line.indexOf("MQS4") == 0)
        return 4;
    return -3;
}

// -3 protocol error
// -2 timed out
// -1 not connected to server
//  5 watch retrieve done
//  7 no data received
int MqTLS::callback(String *slot, String *payload)
{
    *payload = "";
    String line = "";

    // Check secondary buffer first
    int i = watchBuff.indexOf('\n');
    if (i > -1)
    {
        //Serial.print("Secondary: ");
        //Serial.println(watchBuff);
        line = watchBuff.substring(0, i + 1);
        watchBuff = watchBuff.substring(i + 1);
    }
    else // Check buffer for data
    {
        if (client.available() == 0)
            return 7;

        line = client.readStringUntil('\n');
        //Serial.println(line);
        if (line.indexOf("MQS5") != 0)
            return -3;
    }
    if (line.length() < 7)
        return -3;
    *slot = String(line.substring(4, 5)).toInt();
    int pay_end = String(line.substring(5, 7)).toInt() + 7;
    if (line.length() < (unsigned int)pay_end)
        return -3;
    *payload = line.substring(7, pay_end);
    Serial.println(*payload);
    return 5;
}
