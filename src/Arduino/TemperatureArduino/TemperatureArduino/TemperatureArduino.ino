#include <ESP8266WiFi.h>
#include <Thermistor.h>
#include <PubSubClient.h>


const char *ssid =  "Connectify-BOYD";   // cannot be longer than 32 characters!
const char *pass =  "aphogeschool";

const char *mqtt_server = "m21.cloudmqtt.com";
const int mqtt_port = 12452;
const char *mqtt_user = "xnkcayag";
const char *mqtt_pass = "DtCGtuL2kVfk";
const char *mqtt_client_name = "Weemos_Temp"; // Client connections cant have the same connection name

String thisDevice = "Android/kamertemperatuur"; // Subscribe to this topic and publish with this as context
String verwarming = "Android/verwarming";

WiFiClient wclient;
PubSubClient client(wclient, mqtt_server, mqtt_port);

#define BUFFER_SIZE 100

Thermistor temp(0);


void setup() {
  pinMode(A0, OUTPUT);
  pinMode(D2, OUTPUT);
  // Setup console
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.println();
}
void loop() {
  int temperature = temp.getTemp();
    if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting to ");
    Serial.print(ssid);
    Serial.println("...");
    delay(50);
      Serial.print("Client name is: ");
      Serial.println(mqtt_client_name);
          delay(50);
    WiFi.begin(ssid, pass);

    if (WiFi.waitForConnectResult() != WL_CONNECTED)
      return;
    Serial.println("WiFi connected");
  }

  if (WiFi.status() == WL_CONNECTED) {
    if (!client.connected()) {
      Serial.print("Connecting to: "); Serial.println(mqtt_server);
      if (client.connect(MQTT::Connect(mqtt_client_name)
                         .set_auth(mqtt_user, mqtt_pass))) {
        Serial.println("Connected to MQTT server");
        client.subscribe(thisDevice);
        Serial.println("Subscribed to: " + thisDevice);
        client.subscribe(verwarming);
        Serial.println("Subscribed to: " + verwarming);
      } else {
        Serial.println("Could not connect to MQTT server");   
      }
    }
    
    if (client.connected()){
      temperature = temperature -8;
      if(temperature >=22){
      digitalWrite(D2, LOW);
      Serial.println("LOW");
      Serial.println(temperature);
      }
      else if(temperature<22){
        digitalWrite(D2, HIGH);
        Serial.println("HIGH");
        Serial.println(temperature);
        }
      Receive();
      Send(temperature);
      delay(10000);
      client.loop();  
    }
  }
}
void Send(int data){

   Serial.println("Sending data");
   client.publish(thisDevice, String(data) );

  }


  void Receive(){
                    
      Serial.println("Receiving");
      client.set_callback(callback);       
  }
    
      
  
  void callback(const MQTT::Publish& pub) {
    
    String tmp = (pub.payload_string());
    int temperature = temp.getTemp();

    temperature = temperature-8;


      if (tmp.toInt() >= 22 || temperature >=22) {
        client.publish(verwarming, "off" );
      }
      
      else if (tmp.toInt() < 22 || temperature < 22) {
      client.publish(verwarming, "on" );
      }   
  }
