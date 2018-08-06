#include<DHT.h>//DHT Library
#include <SoftwareSerial.h>// For making virtual serial connection

#define DHTPIN 5 //Pin type and number
#define DHTTYPE DHT11
#define LEDIN 13
#define MOTORIN 7
#define RX 2
#define TX 3

const int VAL_PROBE = 0;
const int MOISTURE_LEVEL = 50;//Thresholds
const int TEMPERATURE_LEVEL = 30;
const int HUMIDITY_LEVEL = 50;

DHT dht(DHTPIN,DHTTYPE);
SoftwareSerial espSerial = SoftwareSerial(RX,TX);

float hum,temp,ftemp;
int moisture = -99,moisturep;
long previousMillis = 0;
unsigned long currentMillis = 0;
long interval = 16000;
String apiKey = "QNEG8U8IZ8T2WVGS";//Thingspeak Private API key
String ssid="COLDSPOT";//WiFi SSID
String password ="1234567890";//Password
boolean DEBUG=true;

boolean thingSpeakWrite(float value1, float value2, float value3)//uploads data to IOT Website(Thingspeak)
{
  String cmd = "AT+CIPSTART=\"TCP\",\"";//TCP Connection         
  cmd += "184.106.153.149";//api.thingspeak.com
  cmd += "\",80";// HTTP port
  espSerial.println(cmd);
  if (DEBUG) 
  Serial.println(cmd);
  if(espSerial.find("Error"))
  {
    if (DEBUG) 
    Serial.println("AT+CIPSTART error");//TCP Connection error
    return false;
  }
  
  String getStr = "GET /update?api_key=";//Generating GET URL string
  getStr += apiKey;
  getStr +="&field1=";
  getStr += String(value1);
  getStr +="&field2=";
  getStr += String(value2);
  getStr +="&field3=";
  getStr += String(value3);
  getStr += "\r\n\r\n";
  
  cmd = "AT+CIPSEND=";//Calculating length to send to server
  cmd += String(getStr.length());
  espSerial.println(cmd);
  if (DEBUG)  
  Serial.println(cmd);
  delay(100);
  
  if(espSerial.find(">"))
  {
    espSerial.print(getStr);
    if (DEBUG)  
    Serial.print(getStr);
  }
  else{
    espSerial.println("AT+CIPCLOSE");//Close TCP connection
    if (DEBUG)   
    Serial.println("AT+CIPCLOSE");// alert user
    return false;
  }
  return true;
}

void setup()
{
  DEBUG=true;
  bool status;
  Serial.begin(9600);//Serial communication at 9600 baud rate
  espSerial.begin(115200);//Serial communication at 115200 baud rate
  dht.begin();
  Serial.println("Booting up sensors.....");
  delay(500);
  moisture = analogRead(VAL_PROBE);
  Serial.println("Checking connection....");
  
  if(moisture == -99)
   {
    Serial.println("Error.....Check soil moisture sensor wiring!!!!");
    while(1);
   }
   
  pinMode(LEDIN, OUTPUT);
  pinMode(MOTORIN, OUTPUT);
  
  espSerial.println("AT+RST");
  delay(1000);
  
  espSerial.println("AT+CWMODE=1");
  delay(1000);

  espSerial.println("AT+CWJAP=\""+ssid+"\",\""+password+"\"");
  delay(5000);

   if (DEBUG)  
   Serial.println("Setup completed");
}

void LedState(int state)
{
  digitalWrite(13,state);
}

void MotorState(int state)
{
  digitalWrite(7,state);
}

void display()
{
  Serial.print("Moisture = ");
  Serial.print(moisturep);
  Serial.println("%");
  
  Serial.print("Humidity = ");
  Serial.print(hum);
  Serial.println("%");
  
  Serial.print("Temperature = ");
  Serial.print(temp);
  Serial.println("deg. C");
  
  Serial.print("Temperature = ");
  Serial.print(ftemp);
  Serial.println("deg. F");
  
  Serial.println();
}

void loop()
{
  currentMillis = millis();
  if(currentMillis - previousMillis > interval) 
  {
  previousMillis = currentMillis;
  moisture = analogRead(VAL_PROBE);
  moisturep = map(moisture,1023,200,0,100);
  hum = dht.readHumidity();
  temp = dht.readTemperature();
  if (isnan(temp) || isnan(hum)) 
  {
        if (DEBUG) 
        Serial.println("Failed to read from DHT");
      }
      else 
      {
  ftemp = temp*9.0/5.0+32;
  display();
  thingSpeakWrite(temp,hum,moisturep);
  }
  }
  
  if(moisturep < MOISTURE_LEVEL && temp > TEMPERATURE_LEVEL && hum < HUMIDITY_LEVEL)
  {
    LedState(HIGH);
    MotorState(HIGH);
  }
  else if (moisturep < MOISTURE_LEVEL && hum < HUMIDITY_LEVEL)
  {
    LedState(HIGH);
    MotorState(HIGH);
  }
  else if(moisturep < MOISTURE_LEVEL && temp > TEMPERATURE_LEVEL)
  {
    LedState(HIGH);
    MotorState(HIGH);
  }
  else if(temp > TEMPERATURE_LEVEL && hum < HUMIDITY_LEVEL)
  {
    LedState(HIGH);
    MotorState(HIGH);
  }
  else 
  {
    LedState(LOW);
    MotorState(LOW);
  }
}
