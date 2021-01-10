#include "MqTLS.h"

// -3 protocol error
// -2 timed out
// -1 not connected to server
//  1 publish done
int MqTLS::publish(String topic, String slot, String payload)
{
    String line = "";
    String command = "MQS1" + enc(&topic) + slot + enc(&payload);
    int status = communicate(&command, &line);
    if (status < 0)
        return status;
    if (line.indexOf("MQS1") == 0)
        return 1;
    return -3;
}