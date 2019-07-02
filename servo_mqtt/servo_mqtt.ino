#include <WiFi101.h>
#include <Servo.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

#define PIN_SERVO 10

char ssid[] = "ssid";
char pass[] = "pass";
char host[] = "address";
char topic[] = "topic";
int port = 1883;

WiFiClient wificlient;
Servo servo;
PubSubClient mqttClient(wificlient);
StaticJsonBuffer<200> jsonBuffer;

void setup() {
  Serial.begin(9600);
  servo.attach( PIN_SERVO );
  mqttClient.setServer(host, port);
  mqttClient.setCallback(callback);
  
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

void servo_angle(int angle) {
  servo.write( angle );
  delay(1000);
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
  
  JsonObject& servo_json = jsonBuffer.parseObject(payload_json);
  Serial.print("angle : ");
  int angle = servo_json["angle"];
  Serial.println(angle);
  servo_angle(angle);  
  jsonBuffer.clear();
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
