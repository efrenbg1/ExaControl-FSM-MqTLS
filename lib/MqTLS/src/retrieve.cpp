#include "MqTLS.h"

// -3 protocol error
// -2 timed out
// -1 not connected to server
//  2 retrieve done
//  7 no data in topic
//  8 no valid acls
//  9 error auth
int MqTLS::retrieve(String topic, String slot, String *payload)
{
    *payload = "";
    String line = "";
    String command = "MQS2" + enc(&topic) + slot;
    int status = communicate(&command, &line);
    if (status < 0)
        return status;
    if (line.indexOf("MQS7") == 0)
        return 7;
    if (line.indexOf("MQS8") == 0)
        return 8;

    if (line.indexOf("MQS2") != 0)
        return -3;
    if (line.length() < 6)
        return -3;

    int pay_end = String(line.substring(4, 6)).toInt() + 6;
    if (line.length() < (unsigned int)pay_end)
        return -3;
    *payload = line.substring(6, pay_end);
    return 2;
}