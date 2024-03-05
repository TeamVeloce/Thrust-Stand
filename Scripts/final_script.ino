#include "HX711.h"
#include <EEPROM.h>
#include <Arduino.h>
#include <Thread.h>
#include <Servo.h>
#include <ThreadController.h>

// PIN ASSIGNMENT
#define VS_IN_PIN A1   // VS
#define DOUT_PIN  7    // LC
#define CLK_PIN   8    // LC
#define CS_IN_PIN A2   // CS
#define NO_OF_BLADES 2 // TM
#define LaserOut 3     // TM
#define LaserSensor 2  // TM


// FOR LOAD CELL
float calibrationFactor;

// FOR TACHOMETER
unsigned int rpm=0;
volatile unsigned int counter = 0;
unsigned long previousMillis = 0;

// FOR CS
float cs_callibration_factor;

// FOR VS
float adc_voltage = 0.0;
float in_voltage = 0.0;
float R1 = 30000.0;
float R2 = 7500.0; 
int adc_value = 0;
float ref_voltage = 5.0;

HX711 scale;
Servo ESC;
ThreadController control = ThreadController();

Thread LC = Thread();
Thread CS = Thread();
Thread TM = Thread();
Thread VS = Thread();

float vals[4];

void setup() {
  Serial.begin(9600); delay(10);
  pinMode(LaserOut,OUTPUT);
  digitalWrite(LaserOut,HIGH);
  scale.begin(DOUT_PIN, CLK_PIN);
  scale.set_scale();
  ESC.attach(6,1000,2000);
  ESC.write(0);
  callibration();

  pinMode(LaserSensor, INPUT);
  attachInterrupt(digitalPinToInterrupt(LaserSensor), IRinterrupt, FALLING);

  LC.onRun(LC_code);
  CS.onRun(CS_code);
  TM.onRun(TM_code);
  VS.onRun(VS_code);

  control.add(&LC);
  control.add(&CS);
  control.add(&TM);
  control.add(&VS);
}

void loop() {
  for (int i = 0; i<=10;i++){
    run_cycle(18*i);
  }
  ESC.write(0);
  delay(5000);
  memset(vals, 0, sizeof(vals));
}



void IRinterrupt() {
  counter++;
}



void TM_code(){
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 1000) {
    detachInterrupt(digitalPinToInterrupt(LaserSensor));
    rpm = (counter / NO_OF_BLADES) * 60;
    counter = 0;
    attachInterrupt(digitalPinToInterrupt(LaserSensor), IRinterrupt, FALLING);
    previousMillis = currentMillis;
    vals[0] = rpm;
  }

}

void VS_code(){
  adc_value = analogRead(VS_IN_PIN);
  adc_voltage  = (adc_value * ref_voltage) / 1024.0; 
  in_voltage = adc_voltage / (R2/(R1+R2)) ; 
  vals[3] = in_voltage;
}

void LC_code(){
  long rawValue = scale.read();
  float calibrationFactor = 0.00332;
  float massInGrams = rawValue * calibrationFactor;
  float final_mass = 270 - massInGrams;
  vals[1] = final_mass;
  delay(100);
}

void CS_code(){
  unsigned int x=0;
  float AcsValue=0.0,Samples=0.0,AvgAcs=0.0,AcsValueF=0.0;
  for (int x = 0; x < 150; x++){
    AcsValue = analogRead(CS_IN_PIN);
    Samples = Samples + AcsValue;
    delay (3);
  }
  AvgAcs=Samples/150.0;
  AcsValueF = (2.5 - (AvgAcs * (5.0 / 1024.0)) )/0.066;
  vals[2] = AcsValueF+0.35 - cs_callibration_factor;
  delay(50);
}

void callibration(){
  Serial.println("Callibration ... ");
  scale.tare();
  Serial.println("Tare Complete ... ");
  unsigned int x=0;
  float AcsValue=0.0,Samples=0.0,AvgAcs=0.0,AcsValueF=0.0;
  for (int x = 0; x < 150; x++){
    AcsValue = analogRead(CS_IN_PIN);
    Samples = Samples + AcsValue;
    delay(3);
  }
  AvgAcs=Samples/150.0;
  AcsValueF = (2.5 - (AvgAcs * (5.0 / 1024.0)) )/0.066;
  cs_callibration_factor = AcsValueF+0.35;
  Serial.println("Current sensor callibrated ... ");
  delay(500);
  Serial.println("Callibration Complete !");
  Serial.print("Please connect battery to thrust stand in ");
  Serial.print("5...");
  delay(1000);  Serial.print("4...");  delay(1000);  Serial.print("3...");
  delay(1000);  Serial.print("2...");  delay(1000);  Serial.println("1...");
  delay(2000);

}

void run_cycle(int percentage){
  ESC.write(percentage);
  delay(50);
  control.run();
  Serial.println(String(vals[0])+" "+String(vals[1])+" "+String(vals[2])+" "+String(vals[3]));
  delay(1000);
}
