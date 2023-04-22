// Juilee Kotnis & Sushrut Thakur
#include <Servo.h>
#include "config.h"
#include <Arduino.h>
#include "HX711.h"
#define SOUND_VELOCITY 0.034
#define CM_TO_INCH 0.393701

Servo servo;
HX711 scale;

const int trigPin = 15;
const int echoPin = 14;
long duration;
float distanceCm;
float distanceInch;
int x = 0;
float percent = 0;
double percent_trash = 0;
const int dry = 849;
const int wet = 395;
const int LOADCELL_DOUT_PIN = 12;
const int LOADCELL_SCK_PIN = 13;

AdafruitIO_Feed *Wetness_Garbage = io.feed("Wetness_Garbage");
AdafruitIO_Feed *Trash_Lid = io.feed("Trash_Lid");
AdafruitIO_Feed *Percentage_Trash = io.feed("Percentage_Trash");

void sendSensor()
{

  x = analogRead(A0); 
  percent = map(x, wet, dry, 100, 0);  
  Wetness_Garbage->save(percent);
  delay(500);
  Trash_Lid->save(distanceCm);
  delay(3000);
  Percentage_Trash->save(percent_trash);
  delay(2000);

}

void setup() {

  Serial.begin(115200);
  while(! Serial);
  Serial.print("Connecting to Adafruit IO");

  io.connect(); 
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println();
  Serial.println(io.statusText());

  servo.attach(2); //D4
  servo.write(0);
  delay(2000);
  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT);

  Serial.println("HX711 Demo");
  Serial.println("Initializing the scale");
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  Serial.println("Before setting up the scale:");
  Serial.print("read: \t\t");
  Serial.println(scale.read());      
  Serial.print("read average: \t\t");
  Serial.println(scale.read_average(20)); 
  Serial.print("get value: \t\t");
  Serial.println(scale.get_value(5));  
  Serial.print("get units: \t\t");
  Serial.println(scale.get_units(5), 1);       
  scale.set_scale(1517);
  scale.tare();              
  Serial.println("After setting up the scale:");
  Serial.print("read: \t\t");
  Serial.println(scale.read());                
  Serial.print("read average: \t\t");
  Serial.println(scale.read_average(20));       
  Serial.print("get value: \t\t");
  Serial.println(scale.get_value(5));   
  Serial.print("get units: \t\t");
  Serial.println(scale.get_units(5), 1);        
  Serial.println("Readings:");

}

void loop() {
  
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distanceCm = duration * SOUND_VELOCITY/2;
  Serial.println(distanceCm);
  if(distanceCm < 10.0)
  {
    Serial.println("Open the Bin");
    servo.write(180);
    delay(1000);
 
  } 
  else
  {
    servo.write(0);
    delay(2000);
  }

  double weight = scale.get_units();
  percent_trash = (weight/11400)*100;
  Serial.print('\n'); 
  Serial.print(weight);
  Serial.print('\n');
  Serial.print(percent_trash); 
  scale.power_down();             // put the ADC in sleep mode
  delay(500);
  scale.power_up();

  io.run();
  sendSensor();
  delay(200);

}
