//includes first

#include <Arduino.h>
#include <ESP8266SAM.h>
#include <AudioOutputI2S.h>
//this is from remote example: to do test without
//#include "AudioOutputI2SNoDAC.h"
//from remote example. toypostserver code might help to get rid of useless h files
#include <ESP8266mDNS.h>
#include <ESP8266NetBIOS.h>
#include <ESP8266SSDP.h> //Library for SSDP (Show ESP in Network on Windows)
#include <ESP8266WebServer.h> //Library for WebServer
#include <ESP8266WiFi.h>
#include <uri/UriBraces.h>


//declarations
//version without remote
AudioOutputI2S *out = NULL;
//remote shit version
//AudioOutputI2SNoDAC *out = NULL;

//Server, Wifi
ESP8266WebServer server(80); //Web Server on port 80
const char* NAME = "SAM";
const char *ssid = "FILTERNET"; 
const char *pass = "";

ESP8266SAM *sam = new ESP8266SAM;
//---------------------------------SETUP--------------------

void setup()
{
  Serial.begin(115200);
  //shitty remote
 // out = new AudioOutputI2SNoDAC();
  //out->begin();
  
  //working without remote
  out = new AudioOutputI2S();
  out->begin();

//wifi stuff
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
//Wifi stuff I dont understand
  MDNS.begin(NAME);
  MDNS.addService("http", "tcp", 80);
  NBNS.begin(NAME);
//void SetSpeed(20)
 //the thing with the Brackets: accept everything after it
 server.on(UriBraces("/say/{}"), []() {
  //nimmt den path, was macht urldecode?
    String message_encoded = server.pathArg(0);
    String message_decoded = urldecode(message_encoded);
    const char* message = message_decoded.c_str();
//shows the message
    Serial.println(message_encoded);
    Serial.println(message_decoded);
    Serial.println(message);
    //the saying is happening
     sam->SetSpeed(120);
    // sam-> SetThroat(120);
    sam->Say(out, message);
    delay(500);
    server.send(200, "text/plain", "Type Message");

//
  /*   sam->Say(out, "Can you hear me now?");
  delay(500);
  sam->Say(out, "I can't hear you!");
  ESP8266SAM *sam = new ESP8266SAM;
  delete sam;*/

  });
  
//SSDP thing blabla
  server.on("/description.xml", HTTP_GET, []() {
    SSDP.schema(server.client());
  });
  server.begin();
  ssdp();
}  

//Function with lots of secrets
void ssdp() {
  //Simple Service Discovery Protocol : Display ESP in Windows Network Tab
  SSDP.setSchemaURL("description.xml");
  SSDP.setHTTPPort(80);
  SSDP.setName(NAME);
  SSDP.setDeviceType("upnp: rootdevice");
  SSDP.setSerialNumber("000000000001");
  SSDP.setURL("/say/connected");
  SSDP.setModelName("ESP8266SAM");
  SSDP.setModelNumber("0000000000001");
  SSDP.setModelURL("https://github.com/earlephilhower/ESP8266SAM");
  SSDP.setManufacturer("earlephilhower");
  SSDP.setManufacturerURL("https://github.com/earlephilhower/");
  SSDP.begin();
}  
 


//---------------------------------LOOP-----------------------
void loop()
{
 server.handleClient();

//working without remote  
 /* ESP8266SAM *sam = new ESP8266SAM;
  *  
  sam->Say(out, "Can you hear me now?");
  delay(500);
  sam->Say(out, "I can't hear you!");
  delete sam;*/

}

//Functions that do the encoding of the url
char* string2char(String command) {
  if (command.length() != 0) {
    char *p = const_cast<char*>(command.c_str());
    return p;
  } else {
    return "";
  }
}

unsigned char h2int(char c) {
  if (c >= '0' && c <= '9') {
    return ((unsigned char)c - '0');
  }
  if (c >= 'a' && c <= 'f') {
    return ((unsigned char)c - 'a' + 10);
  }
  if (c >= 'A' && c <= 'F') {
    return ((unsigned char)c - 'A' + 10);
  }
  return (0);
}

String urldecode(String str)
{

  String encodedString = "";
  char c;
  char code0;
  char code1;
  for (int i = 0; i < str.length(); i++) {
    c = str.charAt(i);
    if (c == '+') {
      encodedString += ' ';
    } else if (c == '%') {
      i++;
      code0 = str.charAt(i);
      i++;
      code1 = str.charAt(i);
      c = (h2int(code0) << 4) | h2int(code1);
      encodedString += c;
    } else {

      encodedString += c;
    }

    yield();
  }

  return encodedString;
}
