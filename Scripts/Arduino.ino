#include <HX711_ADC.h>
#include <EEPROM.h>
#include <Arduino.h>
#include <Thread.h>
#include <Servo.h>
#include <ThreadController.h>
#define ANALOG_IN_PIN A1

const int HX711_dout = 4;
const int HX711_sck = 5;
const int calVal_eepromAdress = 0;
unsigned long t = 0;

float average = 0;

volatile unsigned int counter = 0;  // Counter variable for revolutions
unsigned long previousMillis = 0;  // Variable to store previous time

const int LaserSensor = 2;
unsigned int rpm;
volatile unsigned long count;

float cs_callibration_factor;
unsigned long start;
int Speed; 

float adc_voltage = 0.0;
float in_voltage = 0.0;
float R1 = 30000.0;
float R2 = 7500.0; 
int adc_value = 0;
float ref_voltage = 5.0;

HX711_ADC LoadCell(HX711_dout,HX711_sck);
ThreadController control = ThreadController();
Servo ESC; 

Thread LC = Thread();
Thread CS = Thread();
Thread TM = Thread();
Thread VS = Thread();

float vals[4];


void setup() {
  Serial.begin(9600); delay(10);
  ESC.attach(6,1000,2000);
  ESC.write(0);
  start = millis();
  callibration();
  average = 0;

  LoadCell.begin();
  //LoadCell.setReverseOutput(); //uncomment to turn a negative output value to positive
  float calibrationValue;
  calibrationValue = 700.1;
  unsigned long stabilizingtime = 2000;
  boolean _tare = true;


  count = 0;
  rpm = 0;
 

  LoadCell.start(stabilizingtime, _tare);

  if (LoadCell.getTareTimeoutFlag()) {
    Serial.println("Timeout, check MCU>HX711 wiring and pin designations");
    while (1);
  }
  else {
    LoadCell.setCalFactor(calibrationValue);
        //Serial.println("Startup is complete");
  }

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
  Speed = analogRead(A3);
  Speed = map(Speed, 0, 1023, 0, 180);
  ESC.write(Speed);
  control.run();
  Serial.println(String(vals[0])+" "+String(vals[1])+" "+String(vals[2])+" "+String(vals[3]));
  memset(vals, 0, sizeof(vals));
}



void IRinterrupt() {
  counter++;
}



void TM_code(){
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= 1000) {
    detachInterrupt(digitalPinToInterrupt(LaserSensor));
    rpm = (counter / 3) * 60;  // Calculate RPM
    counter = 0;
    attachInterrupt(digitalPinToInterrupt(LaserSensor), IRinterrupt, FALLING);
    previousMillis = currentMillis;
    vals[0] = rpm;
  }

}



void VS_code(){
  adc_value = analogRead(ANALOG_IN_PIN);
  adc_voltage  = (adc_value * ref_voltage) / 1024.0; 
  in_voltage = adc_voltage / (R2/(R1+R2)) ; 
  vals[3] = in_voltage;
}



void LC_code(){
  static boolean newDataReady = 0;
  const int serialPrintInterval = 0;

  // check for new data/start next conversion:
  if (LoadCell.update()) newDataReady = true;

  // get smoothed value from the dataset:
  if (newDataReady) {
    if (millis() > t + serialPrintInterval) {
      float i = LoadCell.getData();
      vals[1] = i;
      newDataReady = 0;
      t = millis();
    }
  }
  LoadCell.tareNoDelay();

}



void CS_code(){
  float average = 0;
  for(int i = 0; i < 1000; i++) {
    average = average + (.044 * analogRead(A2) -3.78)/1000;
    //(.0264 * analogRead(A0) -13.51) ;//this is 
    //(.19 * analogRead(A0) -25) for 20A mode and 
    //(.044 * analogRead(A0) -3.78) for 30A mode
    delay(1);
  }
  vals[2] = (average - cs_callibration_factor);
}



void callibration(){
  while ((millis() - start) <= 5500){
    average = 0;
    for(int i = 0; i < 1000; i++) {
    average = average + (.044 * analogRead(A2) -3.78)/1000;
    delay(1);
  }
  cs_callibration_factor = cs_callibration_factor + average;
  }
  cs_callibration_factor = cs_callibration_factor/5;
//  Serial.print("Callibration factor : ");
//  Serial.println(cs_callibration_factor);
}
