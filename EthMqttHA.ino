#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

// Function prototypes
void subscribeReceive(char* topic, byte* payload, unsigned int length);

// Set your MAC address and IP address here
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 2, 240);

// Make sure to leave out the http and slashes!
const char* server = "";
const char* mqttUser = "";
const char* mqttPassword = "";

// Ethernet and MQTT related objects
EthernetClient ethClient;
PubSubClient mqttClient(ethClient);

const int PIRinput = 4;

// Variables will change:
int buttonPushCounter = 0;   // counter for the number of button presses
int buttonState = 0;         // current state of the button
int lastButtonState = 0;     // previous state of the button

void subscribeReceive(char* topic, byte* payload, unsigned int length)
{
  // Print the topic
  Serial.print("Topic: ");
  Serial.println(topic);

  // Print the message
  Serial.print("Message: ");
  for (int i = 0; i < length; i ++)
  {
    Serial.print(char(payload[i]));
  }

  // Print a newline
  Serial.println("");
}


void setup()
{
  // Useful for debugging purposes
  Serial.begin(9600);

  //pinmodes
  pinMode(PIRinput, INPUT);

  // Start the ethernet connection
  Ethernet.begin(mac, ip);

  // Ethernet takes some time to boot!
  delay(3000);

  // Set the MQTT server to the server stated above ^
  mqttClient.setServer(server, 1883);

  // Attempt to connect to the server with the ID "myClientID"
  if (mqttClient.connect("myClientID", mqttUser, mqttPassword))
  {
    Serial.println("Connection has been established, well done");

    // Establish the subscribe event
    mqttClient.setCallback(subscribeReceive);
  }
  else
  {
    Serial.println("Looks like the server connection failed...");
  }

  mqttClient.subscribe("EthMqtt/state");

}

void loop()
{
  mqttClient.loop();

  buttonState = digitalRead(PIRinput);

  // compare the buttonState to its previous state
  if (buttonState != lastButtonState) {
    // if the state has changed, increment the counter
    if (buttonState == HIGH) {
      // if the current state is HIGH then the button went from off to on:
      buttonPushCounter++;
      Serial.println("on");
      Serial.print("number of button pushes: ");
      Serial.println(buttonPushCounter);
      mqttClient.publish("EthMqtt/state", "ON");
    } else {
      // if the current state is LOW then the button went from on to off:
      Serial.println("off");
      mqttClient.publish("EthMqtt/state", "OFF");
    }
    // Delay a little bit to avoid bouncing
    delay(50);
  }

  // save the current state as the last state, for next time through the loop
  lastButtonState = buttonState;
}
