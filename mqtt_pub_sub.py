import paho.mqtt.client as mqttclient
import time

# Configuration
PUBLISH_INTERVAL = 5  # Publish interval in seconds (you can change this)
BROKER_ADDRESS = "gull.rmq.cloudamqp.com"
PORT = 1883
USER = "ejumsfuq:ejumsfuq"
PASSWORD = "23apT7-ha1RDMnhhjNOSPUYlCcXZeURj"

# Global flags
connected = False
message_received = False

# Callback for when the client connects
def on_connect(client, userdata, flags, rc, properties=None):
    global connected
    if rc == 0:
        print("Client is connected")
        # Subscribe after connection
        client.subscribe("mqtt/test/receive")
        connected = True
    else:
        print(f"Connection failed with code: {rc}")
        connected = False

# Callback for when a message is received
def on_message(client, userdata, message, properties=None):
    print(f"Message received: {str(message.payload.decode('utf-8'))}")
    print(f"Topic: {message.topic}")

try:
    # Create client with callback API version
    client_kwargs = {
        'client_id': "MQTT_PubSub",
        'protocol': mqttclient.MQTTv5,
        'callback_api_version': mqttclient.CallbackAPIVersion.VERSION2
    }

    # Setup client
    client = mqttclient.Client(**client_kwargs)
    client.username_pw_set(USER, password=PASSWORD)
    client.on_connect = on_connect
    client.on_message = on_message

    # Connect to broker
    print("Connecting to broker...")
    client.connect(BROKER_ADDRESS, port=PORT)
    client.loop_start()

    # Wait for connection
    while not connected:
        time.sleep(0.1)

    # Counter for message number
    message_count = 0

    print("Starting publish/subscribe loop...")
    print(f"Publishing every {PUBLISH_INTERVAL} seconds. Press Ctrl+C to stop.")
    
    # Main loop
    while True:
        # Publish message
        message = f"Message #{message_count} at {time.strftime('%H:%M:%S')}"
        client.publish("mqtt/test/send", message)
        print(f"Published: {message}")
        
        message_count += 1
        time.sleep(PUBLISH_INTERVAL)

except KeyboardInterrupt:
    print("\nExiting...")
except Exception as e:
    print(f"Error occurred: {e}")
finally:
    client.loop_stop()
    client.disconnect()
    print("Disconnected from broker")