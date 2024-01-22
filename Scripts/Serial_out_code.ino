#include <HX711_ADC.h>
#include <EEPROM.h>
#include <Arduino.h>
#include <Thread.h>
#include <ThreadController.h>
#include <SPI.h>

const int HX711_dout = 4;
const int HX711_sck = 5;
const int calVal_eepromAdress = 0;
unsigned long t = 0;

float average = 0;

volatile unsigned int counter = 0;  // Counter variable for revolutions
unsigned long previousMillis = 0;  // Variable to store previous time

const int irSensorPin = 2;
unsigned int rpm;
volatile unsigned long count;

float cs_callibration_factor = -9.36;


HX711_ADC LoadCell(HX711_dout,HX711_sck);
ThreadController control = ThreadController();

Thread LC = Thread();
Thread CS = Thread();
Thread TM = Thread();

float vals[3];


void setup() {
  Serial.begin(9600); delay(10);
  //Serial.println();
  //Serial.println("Starting...");

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

  pinMode(irSensorPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(irSensorPin), IRinterrupt, FALLING);

  LC.onRun(LC_code);
  CS.onRun(CS_code);
  TM.onRun(TM_code);

  control.add(&LC);
  control.add(&CS);
  control.add(&TM);

}

void loop() {
  control.run();
  Serial.println(String(vals[0])+" "+String(vals[1])+" "+String(vals[2]));
  memset(vals, 0, sizeof(vals));
}

void IRinterrupt() {
  counter++;
}

void TM_code(){
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= 1000) {
    detachInterrupt(digitalPinToInterrupt(irSensorPin));
    rpm = (counter / 3) * 60;  // Calculate RPM
    counter = 0;
    attachInterrupt(digitalPinToInterrupt(irSensorPin), IRinterrupt, FALLING);
    previousMillis = currentMillis;
    vals[0] = rpm;
  }

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
  for(int i = 0; i < 500; i++) {
    average = average + (.044 * analogRead(A0) -3.78)/1000;
    //(.0264 * analogRead(A0) -13.51) ;//this is 
    //(.19 * analogRead(A0) -25) for 20A mode and 
    //(.044 * analogRead(A0) -3.78) for 30A mode
    delay(1);
  }
  vals[2] = (average + cs_callibration_factor)*100;
}

