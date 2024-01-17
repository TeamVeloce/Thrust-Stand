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

const int irSensorPin = 8;
unsigned int rpm;
unsigned long timeOld;
volatile unsigned long count;


HX711_ADC LoadCell(HX711_dout,HX711_sck);
ThreadController control = ThreadController();

Thread LC = Thread();
Thread CS = Thread();
Thread TM = Thread();


void setup() {
  Serial.begin(9600); delay(10);
  Serial.println();
  Serial.println("Starting...");

  pinMode(irSensorPin, INPUT);

  LoadCell.begin();
  //LoadCell.setReverseOutput(); //uncomment to turn a negative output value to positive
  float calibrationValue;
  calibrationValue = 700.1;
  unsigned long stabilizingtime = 2000;
  boolean _tare = true;
  count = 0;
  rpm = 0;
  timeOld = 0;

  LoadCell.start(stabilizingtime, _tare);

  if (LoadCell.getTareTimeoutFlag()) {
    Serial.println("Timeout, check MCU>HX711 wiring and pin designations");
    while (1);
  }
  else {
    LoadCell.setCalFactor(calibrationValue); // set calibration value (float)
    Serial.println("Startup is complete");
  }

  attachInterrupt(digitalPinToInterrupt(irSensorPin), countPulses, FALLING);

  LC.onRun(LC_code);
  CS.onRun(CS_code);
  TM.onRun(TM_code);

  control.add(&LC);
  control.add(&CS);
  control.add(&TM);

}

void loop() {
  control.run();

}



void TM_code(){
  delay(1000);  // Update every second
  detachInterrupt(digitalPinToInterrupt(irSensorPin));
  rpm = (count * 60) / 3;  // Calculate RPM
  Serial.print("RPM: ");
  Serial.println(rpm);
  count = 0;
  attachInterrupt(digitalPinToInterrupt(irSensorPin), countPulses, FALLING);
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
      Serial.print(i);
      newDataReady = 0;
      t = millis();
    }
  }

  // receive command from serial terminal, send 't' to initiate tare operation:
  if (Serial.available() > 0) {
    char inByte = Serial.read();
    if (inByte == 't') LoadCell.tareNoDelay();
  }

  // check if last tare operation is complete:
  if (LoadCell.getTareStatus() == true) {
    Serial.println("Tare complete");
  }
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
  Serial.print(" | ");
  Serial.println(average + 28.6);
}

void countPulses(){
  count++;
}