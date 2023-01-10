/**********************************************************************************
 *  TITLE: Blynk + Manual Switch (latched) control 4 Relays using ESP32 (Real time feedback + no WiFi control)
 *  by Tech StudyCell
 *  Preferences--> Aditional boards Manager URLs : 
 *  https://dl.espressif.com/dl/package_esp32_index.json, http://arduino.esp8266.com/stable/package_esp8266com_index.json
 *  
 *  Download Board ESP32 : https://github.com/espressif/arduino-esp32
 *
 *  Download the libraries 
 *  Blynk 1.0.1 Library:  https://github.com/blynkkk/blynk-library
 **********************************************************************************/

/* Fill-in your Template ID (only if using Blynk.Cloud) */
#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPLIriWTnN1"
#define BLYNK_DEVICE_NAME "all in one"
#define BLYNK_AUTH_TOKEN "CGzp0ksMJWCRzKKPdn1Ju_4PQG8s7XVU"

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "@Lhn";
char pass[] = "home1234";

bool fetch_blynk_state = false;  //true or false


#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

//define for fan control
#define Speed1 5 //21
#define Speed2 17 //19
#define Speed4 18

//for gas sensor
#define MQ2_SENSOR    35
#define RED_LED       25

// define the GPIO connected with Relays and switches
#define RelayPin1 23  //D23
#define RelayPin2 22  //D22
#define RelayPin3 21  //D21
#define RelayPin4 19  //D19

#define SwitchPin1 13  //D13
#define SwitchPin2 12  //D12
#define SwitchPin3 14  //D14
#define SwitchPin4 27  //D27

#define wifiLed   2   //D2

//Change the virtual pins according the rooms
#define VPIN_BUTTON_1    V1 
#define VPIN_BUTTON_2    V2
#define VPIN_BUTTON_3    V3 
#define VPIN_BUTTON_4    V4

// virtual pin for fan control
#define VPIN_BUTTON_0    V0

// Relay State
bool toggleState_1 = LOW; //Define integer to remember the toggle state for relay 1
bool toggleState_2 = LOW; //Define integer to remember the toggle state for relay 2
bool toggleState_3 = LOW; //Define integer to remember the toggle state for relay 3
bool toggleState_4 = LOW; //Define integer to remember the toggle state for relay 4

// Switch State
bool SwitchState_1 = LOW;
bool SwitchState_2 = LOW;
bool SwitchState_3 = LOW;
bool SwitchState_4 = LOW;

int wifiFlag = 0;

char auth[] = BLYNK_AUTH_TOKEN;

BlynkTimer timer;
//for smoke detection

int MQ2_SENSOR_Value = 0;

bool isconnected = false;

#define VPIN_BUTTON_1    V5

//for smoke dectection function
void getSensorData()
{
  MQ2_SENSOR_Value = map(analogRead(MQ2_SENSOR), 0, 4095, 0, 100);
  
  if (MQ2_SENSOR_Value > 50 ){
    digitalWrite(RED_LED, HIGH);
  }
  else{
    digitalWrite(RED_LED, LOW);
  }
}

void sendData()
{  
  Blynk.virtualWrite(VPIN_BUTTON_1, MQ2_SENSOR_Value);
  if (MQ2_SENSOR_Value > 50 )
  {
    Blynk.logEvent("gas", "Gas Detected!");
  }
}

// for fan control function
BLYNK_WRITE(V0)
{
  int fan_speed = param.asInt(); // assigning incoming value from pin V1 to a variable
  if (fan_speed == 0)
  {
    //All realys Off - Fan Off
    
    digitalWrite(Speed1, HIGH);
    digitalWrite(Speed2, HIGH);
    digitalWrite(Speed4, HIGH);
  }

  if (fan_speed == 1)
  {
    //Speed1 Relay On - Fan at speed 1
    
    digitalWrite(Speed1, HIGH);
    digitalWrite(Speed2, HIGH);
    digitalWrite(Speed4, HIGH);
    delay(500);
    digitalWrite(Speed1, LOW);
  }

  if (fan_speed == 2)
  {
    //Speed2 Relay On - Fan at speed 2
    
    digitalWrite(Speed1, HIGH);
    digitalWrite(Speed2, HIGH);
    digitalWrite(Speed4, HIGH);
    delay(500);
    digitalWrite(Speed2, LOW);
  }

  if (fan_speed == 3)
  {
    //Speed1 & Speed2 Relays On - Fan at speed 3
    
    digitalWrite(Speed1, HIGH);
    digitalWrite(Speed2, HIGH);
    digitalWrite(Speed4, HIGH);
    delay(500);
    digitalWrite(Speed1, LOW);
    digitalWrite(Speed2, LOW);
  }

  if (fan_speed == 4)
  {
    //Speed4 Relay On - Fan at speed 4
    
    digitalWrite(Speed1, HIGH);
    digitalWrite(Speed2, HIGH);
    digitalWrite(Speed4, HIGH);
    delay(500);
    digitalWrite(Speed4, LOW);
  }
  // process received value
}


BLYNK_WRITE(VPIN_BUTTON_1) {
  toggleState_1 = param.asInt();
  digitalWrite(RelayPin1, !toggleState_1);
}

BLYNK_WRITE(VPIN_BUTTON_2) {
  toggleState_2 = param.asInt();
  digitalWrite(RelayPin2, !toggleState_2);
}

BLYNK_WRITE(VPIN_BUTTON_3) {
  toggleState_3 = param.asInt();
  digitalWrite(RelayPin3, !toggleState_3);
}

BLYNK_WRITE(VPIN_BUTTON_4) {
  toggleState_4 = param.asInt();
  digitalWrite(RelayPin4, !toggleState_4);
}

void checkBlynkStatus() { // called every 2 seconds by SimpleTimer

  bool isconnected = Blynk.connected();
  if (isconnected == false) {
    //for gas control
    digitalWrite(wifiLed, LOW);
    Serial.println("Blynk Not Connected");
    //for bulb control
    wifiFlag = 1;
    Serial.println("Blynk Not Connected");
    digitalWrite(wifiLed, LOW);
  }
  if (isconnected == true) {
    //for gas detection
    digitalWrite(wifiLed, HIGH);
    sendData();
    //for bulb control
    wifiFlag = 0;
    if (!fetch_blynk_state){
    Blynk.virtualWrite(VPIN_BUTTON_1, toggleState_1);
    Blynk.virtualWrite(VPIN_BUTTON_2, toggleState_2);
    Blynk.virtualWrite(VPIN_BUTTON_3, toggleState_3);
    Blynk.virtualWrite(VPIN_BUTTON_4, toggleState_4);
    }
    digitalWrite(wifiLed, HIGH);
    Serial.println("Blynk Connected");
  }
}

BLYNK_CONNECTED() {
  // Request the latest state from the server
  if (fetch_blynk_state){
    Blynk.syncVirtual(VPIN_BUTTON_1);
    Blynk.syncVirtual(VPIN_BUTTON_2);
    Blynk.syncVirtual(VPIN_BUTTON_3);
    Blynk.syncVirtual(VPIN_BUTTON_4);
  }
}

void manual_control()
{
  if (digitalRead(SwitchPin1) == LOW && SwitchState_1 == LOW) {
    digitalWrite(RelayPin1, LOW);
    toggleState_1 = 1;
    SwitchState_1 = HIGH;
    Blynk.virtualWrite(VPIN_BUTTON_1, toggleState_1);
    Serial.println("Switch-1 on");
  }
  if (digitalRead(SwitchPin1) == HIGH && SwitchState_1 == HIGH) {
    digitalWrite(RelayPin1, HIGH);
    toggleState_1 = 0;
    SwitchState_1 = LOW;
    Blynk.virtualWrite(VPIN_BUTTON_1, toggleState_1);
    Serial.println("Switch-1 off");
  }
  if (digitalRead(SwitchPin2) == LOW && SwitchState_2 == LOW) {
    digitalWrite(RelayPin2, LOW);
    toggleState_2 = 1;
    SwitchState_2 = HIGH;
    Blynk.virtualWrite(VPIN_BUTTON_2, toggleState_2);
    Serial.println("Switch-2 on");
  }
  if (digitalRead(SwitchPin2) == HIGH && SwitchState_2 == HIGH) {
    digitalWrite(RelayPin2, HIGH);
    toggleState_2 = 0;
    SwitchState_2 = LOW;
    Blynk.virtualWrite(VPIN_BUTTON_2, toggleState_2);
    Serial.println("Switch-2 off");
  }
  if (digitalRead(SwitchPin3) == LOW && SwitchState_3 == LOW) {
    digitalWrite(RelayPin3, LOW);
    toggleState_3 = 1;
    SwitchState_3 = HIGH;
    Blynk.virtualWrite(VPIN_BUTTON_3, toggleState_3);
    Serial.println("Switch-3 on");
  }
  if (digitalRead(SwitchPin3) == HIGH && SwitchState_3 == HIGH) {
    digitalWrite(RelayPin3, HIGH);
    toggleState_3 = 0;
    SwitchState_3 = LOW;
    Blynk.virtualWrite(VPIN_BUTTON_3, toggleState_3);
    Serial.println("Switch-3 off");
  }
  if (digitalRead(SwitchPin4) == LOW && SwitchState_4 == LOW) {
    digitalWrite(RelayPin4, LOW);
    toggleState_4 = 1;
    SwitchState_4 = HIGH;
    Blynk.virtualWrite(VPIN_BUTTON_4, toggleState_4);
    Serial.println("Switch-4 on");
  }
  if (digitalRead(SwitchPin4) == HIGH && SwitchState_4 == HIGH) {
    digitalWrite(RelayPin4, HIGH);
    toggleState_4 = 0;
    SwitchState_4 = LOW;
    Blynk.virtualWrite(VPIN_BUTTON_4, toggleState_4);
    Serial.println("Switch-4 off");
  }
}  

void setup()
{
  Serial.begin(115200);
  // for smoke detection
  pinMode(MQ2_SENSOR, INPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(wifiLed, OUTPUT);

  digitalWrite(RED_LED, LOW);
  digitalWrite(wifiLed, LOW);
  

  //for fan control
  pinMode(Speed1, OUTPUT);
  pinMode(Speed2, OUTPUT);
  pinMode(Speed4, OUTPUT);

  //Initially the fan will be in OFF state
  digitalWrite(Speed1, HIGH);
  digitalWrite(Speed2, HIGH);
  digitalWrite(Speed4, HIGH);

// for bulb control
  pinMode(RelayPin1, OUTPUT);
  pinMode(RelayPin2, OUTPUT);
  pinMode(RelayPin3, OUTPUT);
  pinMode(RelayPin4, OUTPUT);

  pinMode(wifiLed, OUTPUT);

  pinMode(SwitchPin1, INPUT_PULLUP);
  pinMode(SwitchPin2, INPUT_PULLUP);
  pinMode(SwitchPin3, INPUT_PULLUP);
  pinMode(SwitchPin4, INPUT_PULLUP);

  //During Starting all Relays should TURN OFF
  digitalWrite(RelayPin1, !toggleState_1);
  digitalWrite(RelayPin2, !toggleState_2);
  digitalWrite(RelayPin3, !toggleState_3);
  digitalWrite(RelayPin4, !toggleState_4);

  digitalWrite(wifiLed, LOW);

  //Blynk.begin(auth, ssid, pass);
  WiFi.begin(ssid, pass);
  timer.setInterval(2000L, checkBlynkStatus); // check if Blynk server is connected every 2 seconds
  Blynk.config(auth);
  delay(1000);
  
  if (!fetch_blynk_state){
    Blynk.virtualWrite(VPIN_BUTTON_1, toggleState_1);
    Blynk.virtualWrite(VPIN_BUTTON_2, toggleState_2);
    Blynk.virtualWrite(VPIN_BUTTON_3, toggleState_3);
    Blynk.virtualWrite(VPIN_BUTTON_4, toggleState_4);
  }
}

void loop()
{ 
  getSensorData();
  manual_control();
  Blynk.run();
  timer.run();
}
