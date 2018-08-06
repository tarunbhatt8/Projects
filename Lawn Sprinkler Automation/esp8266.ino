/*
 * This Code is for the ESP8266 WiFi Module. It builds a website running AJAX (Asynchronous JavaScript and XML)
 * which allows this webpage to update itself without having to refresh itself which may not be resource friendly
 */

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#define DEVICE_ID 1

ESP8266WebServer server(80);
const char* ssid="COLDSPOT";  // WiFi Access Point to connect to
const char* password="1234567890"; // Password for the Access Point
String webSite,javaScript,XML;
String coming;

void buildWebsite() {
  buildJavascript();
  webSite = "<!DOCTYPE HTML>\n";
  webSite += "<HEAD><META charset = 'UTF-8'></HEAD>\n";
  webSite += "<html lang = 'en-US'>\n";
  webSite += javaScript;
  webSite += "<BODY onload='process()'>\n";
  webSite += "<h1 align='center'><BR>ULTRASONIC SENSOR DATA:<BR><h1>\n";
  webSite += "STATUS = <A charset='UTF-8' id='runtime'></A>\n";
  webSite += "</BODY>\n";
  webSite += "</HTML>\n";
}

void buildJavascript() {
  javaScript = "<SCRIPT>\n";
  javaScript += "var xmlHttp=createXmlHttpObject();\n";
  javaScript += "function createXmlHttpObject(){\n";
  javaScript += " if(window.XMLHttpRequest){\n";
  javaScript += "    xmlHttp=new XMLHttpRequest();\n";
  javaScript += " }else{\n";
  javaScript += "    xmlHttp=new ActiveXObject('Microsoft.XMLHTTP');\n";
  javaScript += " }\n";
  javaScript += " return xmlHttp;\n";
  javaScript += "}\n";
  javaScript += "function process(){\n";
  javaScript += " if(xmlHttp.readyState==0 || xmlHttp.readyState==4){\n";
  javaScript += "   xmlHttp.open('PUT','xml',true);\n";
  javaScript += "   xmlHttp.onreadystatechange=handleServerResponse;\n"; // no brackets?????
  javaScript += "   xmlHttp.send(null);\n";
  javaScript += " }\n";
  javaScript += " setTimeout('process()',1000);\n";
  javaScript += "}\n";
  javaScript += "function handleServerResponse(){\n";
  javaScript += " if(xmlHttp.readyState==4 && xmlHttp.status==200){\n";
  javaScript += "   xmlResponse=xmlHttp.responseXML;\n";
  javaScript += "   xmldoc = xmlResponse.getElementsByTagName('response');\n";
  javaScript += "   message = xmldoc[0].firstChild.nodeValue;\n";
  javaScript += "   document.getElementById('runtime').innerHTML=message;\n";
  javaScript += " }\n";
  javaScript += "}\n";
  javaScript += "</SCRIPT>\n";
}

void buildXML() {
  XML = "<?xml version='1.0' encoding='UTF-8'?>";
  XML += "<response>";
  XML += DataFromArduino(); //incoming data from Arduino. It means it is the value of mapping function.
  XML += "</response>";
}
String DataFromArduino() {
  if(Serial.available()) {
    while(Serial.available()) {
      coming = Serial.readStringUntil('\n').toInt();
      delay(1);
    } // it is getting via serial port.
    return coming;
  } else {
    return coming;
  }
}

void handleWebsite() {
  buildWebsite();
  server.send(200,"text/html",webSite);
}

void handleXML() {
  buildXML();
  server.send(200,"text/xml",XML);
}

void setup() {
  Serial.begin(9600);  
  WiFi.begin(ssid,password);  //trying to connect the modem
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  WiFi.mode(WIFI_STA);
  Serial.println("\n. . . BOOTING ESP8266 . . .\n");
  Serial.print("DEVICE ID :");
  Serial.print(DEVICE_ID);
  Serial.print("\n");
  Serial.print("CONNECTED TO ACCESS POINT :");
  Serial.println(ssid);
  Serial.flush();
  Serial.println(WiFi.localIP());
  //it is sending the IP to Arduino's port. So I can see the IP on Serial Monitor
  Serial.flush();
  server.on("/", handleWebsite);
  //it is all about setting of server.
  server.on("/xml",handleXML);
  server.begin();  
}

void loop() {
  server.handleClient();
}
