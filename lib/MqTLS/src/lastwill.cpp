#include "MqTLS.h"

// -3 protocol error
// -2 timed out
// -1 not connected to server
//  3 lastwill done
int MqTLS::lastwill(String topic, String slot, String payload)
{
    lwTopic = topic;
    lwSlot = slot;
    lwPayload = payload;

    String line = "";
    String command = "MQS3" + enc(&topic) + slot + enc(&payload);
    llw = true;
    int status = communicate(&command, &line);
    llw = false;
    if (status < 0)
        return status;
    if (line.indexOf("MQS3") == 0)
        return 3;
    return -3;
}
