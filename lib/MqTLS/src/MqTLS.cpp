#include "MqTLS.h"

//#define debug

MqTLS::MqTLS(String _thumbprint, String _address, int _port, String _user, String _pw)
{
	client.setTimeout(3000);
	client.setFingerprint(_thumbprint.c_str());
	address = _address;
	port = _port;
	user = _user;
	pw = _pw;
}

String MqTLS::enc(String *var)
{
	if (var->length() < 10)
	{
		return "0" + String(var->length()) + *var;
	}
	else if (var->length() < 100)
	{
		return String(var->length()) + *var;
	}
	return "00";
}

// 0 not connected to server
// 1 send done
bool MqTLS::send(String *data)
{
	String flush;
	while (client.available())
		receive(&flush);

	if (!client.connected())
		return false;

	//Serial.println(*data);
	client.print(*data + '\n');

#ifdef debug
	Serial.print("[MqTLS] Sent: ");
	Serial.println(*data);
#endif

	return true;
}

// -3 protocol error
// -2 timed out
// -1 not connected to server
//  1 received done
int MqTLS::receive(String *data)
{
	*data = "";
	String buff;
	unsigned long start = millis();
	while (client.connected())
	{
		if (client.available())
		{
			buff = client.readStringUntil('\n');

#ifdef debug
			Serial.print("[MqTLS] Read: ");
			Serial.println(buff);
#endif

			if (buff.indexOf("MQS") != 0)
				return -3;
			if (buff.indexOf("MQS5") == 0)
			{

#ifdef debug
				Serial.print("[MqTLS] WatchBuff += ");
				Serial.println(buff);
#endif

				if (watchBuff.length() > 1000)
					continue;
				watchBuff += buff + '\n';
				continue;
			}
			else
			{
				*data = buff;
				return 1;
			}
		}
		if ((millis() - start) > 2000)
			return -2;
		delay(1);
	}
	return -1;
}

// -3 protocol error
// -2 timed out
// -1 not connected to server
//  1 received done
int MqTLS::communicate(String *command, String *data)
{
	*data = "";

	if (!send(command))
	{
		int status = connect();
		if (status < 0)
			return status;
		if (!send(command))
			return -1;
	}

	int retries = 1;
	while (retries > 0)
	{
		int status = receive(data);
		if (status < -1)
			return status;
		if (status == -1)
		{
			int status = connect();
			if (status < 0)
				return status;
			if (!send(command))
				return -1;
			retries--;
			continue;
		}
		return 1;
	}
	return -1;
}

// -3 protocol error
// -2 response timed out (2s)
// -1 connection error (server not available)
//  0 connected
//  9 error auth
int MqTLS::connect()
{

#ifdef debug
	Serial.print("[MqTLS] Connecting to: ");
	Serial.println(address);
#endif

	String command = "MQS0" + enc(&user) + enc(&pw) + "1";
	while (!send(&command))
	{
		if (!client.connect(address, port))
			return -1;
	}

	String line = "";
	while (true)
	{
		int status = receive(&line);
		if (status < 0)
			return status;
		if (line.indexOf("MQS0") == 0)
			break;
	}

	// If connected:
	if (!lwTopic.equals("") && !llw)
	{
		int status = lastwill(lwTopic, lwSlot, lwPayload);
		if (status < 0)
			return status;
	}
	if (!watchTopic.equals("") && !lwatch)
	{
		int status = watch(watchTopic);
		if (status < 0)
			return status;
	}
	return 0;
}

void MqTLS::disconnect(void)
{
	client.stop();
}