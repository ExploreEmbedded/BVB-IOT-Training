#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager
#include <WiFiClient.h>
#include <PubSubClient.h>

char* mqttserver = "starling.exploreembedded.com";  
char message_buff[300];   // initialise storage buffer (i haven't tested to this capacity.)
char respBuffer[300];
char charBuf[50];
int8_t MQTT_Status;// return status of the mqtt connect

char macString[18] = {0};
char clientName[20] = {0};

String inputString; //  a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete


WiFiClient wifiClient;
PubSubClient client(mqttserver, 1883, callback, wifiClient); 


char* getMacString()
{
  uint8_t mac[6];
  WiFi.macAddress(mac);
  sprintf(macString, "%02X%02X%02X%02X%02X%02X\0", mac[0],  mac[1], mac[2], mac[3], mac[4], mac[5]);
  return (macString);
}

//Create client name wth mac address

char* getClientName()
{
  uint8_t mac[6];
  WiFi.macAddress(mac);
  sprintf(clientName, "*_%02X%02X%02X%02X%02X%02X\0", mac[0],  mac[1], mac[2], mac[3], mac[4], mac[5]);
  return (clientName);
}

void setup() {

    Serial.begin(9600);
    WiFiManager wifiManager;
    //wifiManager.resetSettings();// Use to clear all Wifi Settings
    wifiManager.autoConnect(getClientName(),"config123");

}

void loop() {

        MQTT_Status = client.state();
         
         switch(MQTT_Status){
              case -4: Serial.println("Connection timed out"); break;
              case -3: Serial.println("MQTT  connection lost"); break;
              case -2: break;  Serial.println("No internet connection, retrying in 1 min. . .");
              case -1: Serial.println("Client disconnected cleanly"); break;
              case 0: break;
              case 1: Serial.println("Bad Protocol"); break;
              case 2: Serial.println("Bad Client ID"); break;
              case 3: Serial.println("Connection unavailable"); break;
              case 4: Serial.println("Username or Password is incorrect"); break;
              case 5: Serial.println("Unauthorized connection"); break;
          }

        if(MQTT_Status<0)
        {
           //wait for 2 seconds, before trying again
           delay(1000);
           //client name and topic are kept the same. 
           Serial.println("NW_connecting_to_Starling_Server");
           if(client.connect(getClientName(),"starling","masterexplorer")) // device id, user id, password
            {
               Serial.println("NW_Conected");
               //client.publish("STAR0001","Starling is flying");
               client.subscribe("bvb");
               Serial.println("NW_subscribe_successful");
                        
            }
        }

      client.loop();

      //handle status read

    
    
    
}

void callback(char* topic, byte* payload, unsigned int length) {
  
    int i = 0;

  for(i=0; i<length; i++) {
    message_buff[i] = payload[i];
  }
  message_buff[i] = '\0';
  
  String msgString = String(message_buff);
  //Serial.println("Payload: " + msgString);
  Serial.println(msgString);

  if(msgString=="status")
  {
    Serial.println("reading device status");
    while(Serial.available()==0);
    inputString =  Serial.readString();
    Serial.println(inputString);
    inputString.toCharArray(charBuf, 50);   
    client.publish("bvb",charBuf);
  }
  
}




