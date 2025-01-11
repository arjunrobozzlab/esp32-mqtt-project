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
        print("Client is not connected")
        connected = False

def on_message(client, userdata, message, properties=None):
    global message_received
    print("Message received: " + str(message.payload.decode("utf-8")))
    print("Topic: " + str(message.topic))
    message_received = True

broker_address = "gull.rmq.cloudamqp.com"
port = 1883
user = "ejumsfuq:ejumsfuq"
password = "23apT7-ha1RDMnhhjNOSPUYlCcXZeURj"

connected = False
message_received = False

# Create client with updated parameters
client = mqttclient.Client(**client_kwargs)
client.username_pw_set(user, password=password)
client.on_connect = on_connect
client.on_message = on_message

try:
    client.connect(broker_address, port=port)
    client.loop_start()
    
    while connected != True:
        time.sleep(0.2)
    
    client.subscribe("mqtt/secondcode")  # Subscribe to the same topic we published to
    
    print("Waiting for messages...")
    while message_received != True:
        time.sleep(0.2)
        
except Exception as e:
    print(f"Error occurred: {e}")
finally:
    client.loop_stop()