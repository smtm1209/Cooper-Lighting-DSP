#include <mosquitto.h>
#include <cstdio>

void callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message, const mosquitto_property *props)
{
    printf("Message Received!\n");
    printf("%s\n", message->payload);
}

int main(int argc, char *argv[])
{
    libmosq_EXPORT int returnStatus;

    void *obj;
    printf("Running\n");
    // mosquitto_lib_init();

    libmosq_EXPORT struct mosquitto *client = mosquitto_new(NULL, true, obj);

    returnStatus = mosquitto_connect(client,
                                     "192.168.137.39",
                                     1883,
                                     5);
    printf("%s\n", mosquitto_strerror(returnStatus));

    returnStatus = mosquitto_subscribe_v5(client,
                                          NULL,
                                          "test/message",
                                          1,
                                          0,
                                          NULL);

    printf("%s\n", mosquitto_strerror(returnStatus));

    mosquitto_message_v5_callback_set(client, callback);

    returnStatus = mosquitto_loop_forever(client,
                                          1000,
                                          100);
    printf("%s\n", mosquitto_strerror(returnStatus));

    return 0;
}