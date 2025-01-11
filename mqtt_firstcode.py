import paho.mqtt.client as mqttclient
import time

# Add callback_api_version
client_kwargs = {
    'client_id': "MQTT",
    'protocol': mqttclient.MQTTv5,
    'callback_api_version': mqttclient.CallbackAPIVersion.VERSION2
}

def on_connect(client, userdata, flags, rc, properties=None):
    global connected
    if rc == 0:
        print("Client is connected")
        connected = True
    else:
        print(f"Connection failed with code {rc}")
        connected = False

broker_address = "gull.rmq.cloudamqp.com"
port = 1883
user = "ejumsfuq:ejumsfuq"
password = "23apT7-ha1RDMnhhjNOSPUYlCcXZeURj"
connected = False

# Create client with updated parameters
client = mqttclient.Client(**client_kwargs)
client.username_pw_set(user, password=password)
client.on_connect = on_connect

try:
    client.connect(broker_address, port=port)
    client.loop_start()

    while connected != True:
        time.sleep(0.2)

    client.publish("mqtt/firstcode", "Smart Axiom")
    client.loop_stop()
except Exception as e:
    print(f"Error occurred: {e}")
    client.loop_stop()