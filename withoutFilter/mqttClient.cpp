#include <mosquitto.h>
#include <cstdio>
#include "constants.h"
#include <mutex>
struct Data
{
    double &db;
    mutex &dbMutex;
    bool &power;
    mutex &powerMutex;
};
void callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message, const mosquitto_property *props)
{
    printf("Message Received: %s\n", (char*)message->payload);
    struct Data *data = (struct Data*)obj;

    if (strcmp((char*)message->payload,"volumeUp") == 0)
    {
        printf("Volumn Up\n");

        data->dbMutex.lock();
        data->db += 1.0;
        data->dbMutex.unlock();
    }
    else if (strcmp((char*)message->payload,"volumeDown") == 0)
    {
        printf("Volumn Down\n");
        data->dbMutex.lock();
        data->db -= 1.0;
        data->dbMutex.unlock();
    }
    //else if (message->payload == "on")
    else if (strcmp((char*)message->payload,"on") == 0)
    {
        printf("On\n");
        data->powerMutex.lock();
        data->power = true;
        data->powerMutex.unlock();
    }
    else if (strcmp((char*)message->payload,"off") == 0)
    {
        printf("Off\n");
        data->powerMutex.lock();
        data->power = false;
        data->powerMutex.unlock();
    }
    else
    {
        printf("Unknown Message\n");
    }
}



void parseMQTT(double &db, mutex &dbMutex, bool &power, mutex &powerMutex)
{
    libmosq_EXPORT int returnStatus;

    struct Data data = {db, dbMutex, power, powerMutex};
    void *obj = &data;
    printf("Running\n");
    libmosq_EXPORT struct mosquitto *client = mosquitto_new(NULL, true, obj);

    returnStatus = mosquitto_connect(client,
                                     "10.211.55.4",
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