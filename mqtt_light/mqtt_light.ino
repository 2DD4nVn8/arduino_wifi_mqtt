#include <WiFi101.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

#define RELAY_PIN 6

char ssid[] = "ssid";
char pass[] = "pass";
char host[] = "address";
char topic[] = "topic";
int port = 1883;

boolean was_turned_on = false;

WiFiClient wificlient;
PubSubClient mqttClient(wificlient);


void setup() {
  Serial.begin(9600);
  mqttClient.setServer(host, port);
  mqttClient.setCallback(callback);
  pinMode(RELAY_PIN, OUTPUT);
  
  // connect WiFi
  while ( WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(10000);
  }

}

void loop() {
  if(!mqttClient.connected()) {
    if (mqttClient.connect(topic)) {
      Serial.println("Connected.");    
      mqttClient.subscribe(topic, 0);
      Serial.println("Subscribed.");
    }
    else {
      errorReport();
    }
  }

  mqttClient.loop();
}


void callback(char* topic, byte* payload, unsigned int length) {
  char payload_json[length];
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    //Serial.print((char)payload[i]);
  payload_json[i]=(char)payload[i];
  payload_json[i+1] = '\0';
  }
  //Serial.println();
  Serial.println(payload_json);

  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& recieved_data_JSON = jsonBuffer.parseObject(payload_json);
  if((int)(recieved_data_JSON["turn_on"]) == 1 && !was_turned_on) {
    Serial.println("OK, turn on");
    
    was_turned_on = !was_turned_on;
    digitalWrite(RELAY_PIN, HIGH);
  }
  else if((int)(recieved_data_JSON["turn_on"]) == 0 && was_turned_on) {
    Serial.println("OK, turn off");
   
    was_turned_on = !was_turned_on;
    digitalWrite(RELAY_PIN, LOW);
  }
  
}

void errorReport(){
  Serial.print("Failed. Error state = ");

  switch (mqttClient.state()) {
    case MQTT_CONNECT_UNAUTHORIZED:
      Serial.println("MQTT_CONNECT_UNAUTHORIZED");
      break;
    case MQTT_CONNECT_BAD_CREDENTIALS:
      Serial.println("MQTT_CONNECT_BAD_CREDENTIALS");
      break;
    case MQTT_CONNECT_UNAVAILABLE:
      Serial.println("MQTT_CONNECT_UNAVAILABLE");
      break;
    case MQTT_CONNECT_BAD_CLIENT_ID:
      Serial.println("MQTT_CONNECT_BAD_CLIENT_ID");
      break;
    case MQTT_CONNECT_BAD_PROTOCOL:
      Serial.println("MQTT_CONNECT_BAD_PROTOCOL");
      break;
    case MQTT_CONNECTED:
      Serial.println("MQTT_CONNECTED");
      break;
    case MQTT_DISCONNECTED:
      Serial.println("MQTT_DISCONNECTED");
      break;
    case MQTT_CONNECT_FAILED:
      Serial.println("MQTT_CONNECT_FAILED");
      break;
    case MQTT_CONNECTION_LOST:
      Serial.println("MQTT_CONNECTION_LOST");
      break;
    case MQTT_CONNECTION_TIMEOUT:
      Serial.println("MQTT_CONNECTION_TIMEOUT");
      break;
  }

  delay(5000); // Wait 5 seconds before retrying
}
