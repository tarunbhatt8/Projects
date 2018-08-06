#include<DHT.h>

#define DHTPIN 5
#define DHTTYPE DHT11
#define LEDIN 13
#define MOTORIN 7

const int VAL_PROBE = 0;
const int MOISTURE_LEVEL = 50;
const int TEMPERATURE_LEVEL = 30;
const int HUMIDITY_LEVEL = 50;

DHT dht(DHTPIN,DHTTYPE);

float hum,temp,ftemp;
int moisture = -99,moisturep;
long previousMillis = 0;
unsigned long currentMillis = 0;
long interval = 10000;

void setup()
{
  Serial.begin(9600);
  Serial.println("Booting up sensors.....");
  dht.begin();
  delay(500);
  moisture = analogRead(VAL_PROBE);
  Serial.println("Checking connection....\n");

  if(moisture == -99)
   {
    Serial.println("Error.....Check soil moisture sensor wiring!!!!");
    while(1);
   }
   
  pinMode(LEDIN, OUTPUT);
  pinMode(MOTORIN, OUTPUT);
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
  ftemp = temp*9.0/5.0+32;
  display();
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
