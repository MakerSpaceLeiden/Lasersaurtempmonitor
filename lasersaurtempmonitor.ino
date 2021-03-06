#include <OneWire.h>
#include <DallasTemperature.h>
#include <EEPROM.h>
  
// using the promicro...

// pin 5 is connected to the relay through an GP NPN transistor, ie BC547
// so setting the pin LOW will OPEN the loop and DISABLE laser
//    setting the pin HIGH will CLOSE the loop an ENABLE laser


// http://www.milesburton.com/?title=Dallas_Temperature_Control_Library#Download
// http://www.hobbytronics.co.uk/ds18b20-arduino

// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 10  
#define TEMPERATURE_PRECISION 9
#define BUTTON 4
#define RELAY 5
#define LEDDEAD 6
#define THRESHOLD 45
#define HYST 5

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

DeviceAddress dummy;

//this is kinda ugly, but with the eeprom write fnction we can fix this
//anyway, replace with your own...
//press button 4, connect one ds18 and read the name on serial
DeviceAddress site0 = {0x28, 0xAB, 0xDD, 0x20, 0x05, 0x00, 0x00, 0x4D};
DeviceAddress site1 = {0x28, 0x33, 0x3C, 0x21, 0x05, 0x00, 0x00, 0xC5};
DeviceAddress site2 = {0x28, 0x8D, 0xB4, 0x20, 0x05, 0x00, 0x00, 0xAB};
DeviceAddress site3 = {0x28, 0x51, 0xFD, 0x20, 0x05, 0x00, 0x00, 0x08};
DeviceAddress site4 = {0x28, 0x6F, 0x40, 0x21, 0x05, 0x00, 0x00, 0xDA};
DeviceAddress ambient = {0x28, 0xFF, 0x21, 0x27, 0x71, 0x15, 0x01, 0x82};


bool hot0 = false;
bool hot1 = false;
bool hot2 = false;
bool hot3 = false;
bool hot4 = false;

bool irreversible_hot = false;

int buttonState = 0;

float t0, t1, t2, t3, t4, t5;

void setup(void)
{
  // start serial port
  Serial.begin(9600);
  Serial.println("Lasersaur temp monitor");

  // Start up the library
  sensors.begin(); // IC Default 9 bit. If you have troubles consider upping it 12. Ups the delay giving the IC more time to process the temperature measurement

  //n = sensors.getDeviceCount();
  //present0 = sensors.getAddress(site0, 0);

  pinMode(BUTTON, INPUT);
  pinMode(RELAY, OUTPUT);
  pinMode(LEDDEAD, OUTPUT);
}

// function to print a device address
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    Serial.print(" 0x");
    // zero pad the address if necessary
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
  Serial.println();
  digitalWrite(LED, LOW);
}

void compare(float t, float amb, bool *hot)
{
  float delta;

  delta = (t - amb);

  if (delta > THRESHOLD) *hot = true;
  if (delta < THRESHOLD-HYST) *hot = false;
  
}

void loop(void)
{ 
  // call sensors.requestTemperatures() to issue a global temperature 
  // request to all devices on the bus
  //Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  //Serial.println("DONE");

  buttonState = digitalRead(BUTTON);
  if (buttonState==HIGH){
    
    Serial.print("name of sensors:");
    sensors.getAddress(dummy, 0);
    printAddress(dummy);
    //EEPROM.put(0, dummy);
    Serial.println();
    while(buttonState==HIGH){
      buttonState = digitalRead(BUTTON);
    };
  }
  

  t0 = sensors.getTempC(site0);
  t1 = sensors.getTempC(site1);
  t2 = sensors.getTempC(site2);
  t3 = sensors.getTempC(site3);
  t4 = sensors.getTempC(site4);
  t5 = sensors.getTempC(ambient);

  compare(t0, t5, &hot0);
  compare(t1, t5, &hot1);
  compare(t2, t5, &hot2);
  compare(t3, t5, &hot3);
  compare(t4, t5, &hot4);
   
  if (hot0||hot1||hot2||hot3||hot4){
    digitalWrite(RELAY, LOW);
    irreversible_hot = true;
  }
  else{
    digitalWrite(RELAY, HIGH);
  }

  if (irreversible_hot){
    //do something that does not recover
    //after cool off
    digitalWrite(LEDDEAD, HIGH);
  } else {
    digitalWrite(LEDDEAD, LOW);
  }
  
  delayMicroseconds(1000);
  Serial.print("0, ");
  //Serial.print(sensors.getTempCByIndex(0)); // Why "byIndex"? You can have more than one IC on the same bus. 0 refers to the first IC on the wire
  Serial.print(t0); // Why "byIndex"? You can have more than one IC on the same bus. 0 refers to the first IC on the wire
  Serial.print(", 1, ");
  Serial.print(t1); 
  Serial.print(", 2, ");
  Serial.print(t2); 
  Serial.print(", 3, ");
  Serial.print(t3); 
  Serial.print(", 4, ");
  Serial.print(t4); 
  Serial.print(", Ambient, ");
  Serial.print(t5); 
    
  Serial.println();
  
}
