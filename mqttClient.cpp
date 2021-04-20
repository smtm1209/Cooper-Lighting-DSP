#include <mosquitto.h>
#include <cstdio>
#include "constants.h"
#include <mutex>

void callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message, const mosquitto_property *props)
{
    printf("Message Received: %s\n", message->payload);

    if (message->payload == "volumeUp")
    {
        printf("Volumn Up\n");
        obj->dbMutex.lock();
        db += 1.0;
        obj->dbMutex.unlock();
    }
    else if (message->payload == "volumeDown")
    {
        printf("Volumn Down\n");
        obj->dbMutex.lock();
        db -= 1.0;
        obj->dbMutex.unlock();
    }
    else if (message->payload == "on")
    {
        printf("On\n");
        obj->powerMutex.lock();
        power = true;
        obj->powerMutex.unlock();
    }
    else if (message->payload == "off")
    {
        printf("Off\n");
        obj->powerMutex.lock();
        power = false;
        obj->powerMutex.unlock();
    }
    else
    {
        printf("Unknown Message\n");
    }
}

struct Data
{
    double &db;
    mutex &dbMutex;
    bool &power;
    mutex &powerMutex;
};

void parseMQTT(double &db, mutex &dbMutex, bool &power, mutex &powerMutex)
{
    libmosq_EXPORT int returnStatus;

    struct Data data = {db, dbMutex, power, powerMutex};
    void *obj = data;
    printf("Running\n");
    libmosq_EXPORT struct mosquitto *client = mosquitto_new(NULL, true, obj);

    returnStatus = mosquitto_connect(client,
                                     "192.168.137.39",
                                     1883,
                                     5);
    printf("Connection Response: %s\n", mosquitto_strerror(returnStatus));

    returnStatus = mosquitto_subscribe_v5(client,
                                          NULL,
                                          "test/message",
                                          1,
                                          0,
                                          NULL);
    printf("Subscribe Response: %s\n", mosquitto_strerror(returnStatus));

    mosquitto_message_v5_callback_set(client, callback);

    returnStatus = mosquitto_loop_forever(client,
                                          1000,
                                          100);
    printf("%s\n", mosquitto_strerror(returnStatus));
}