const int irSensorPin = 2;  // Connect the IR sensor to digital pin 2
unsigned int rpm;
unsigned long timeOld;
volatile unsigned long count;

void setup() {
  pinMode(irSensorPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(irSensorPin), countPulses, FALLING);
  Serial.begin(9600);
  count = 0;
  rpm = 0;
  timeOld = 0;
}

void loop() {
  delay(1000);  // Update every second
  detachInterrupt(digitalPinToInterrupt(irSensorPin));
  rpm = (count * 60) / 3;  // Calculate RPM
  Serial.print("RPM: ");
  Serial.println(rpm);
  count = 0;
  attachInterrupt(digitalPinToInterrupt(irSensorPin), countPulses, FALLING);
}

void countPulses() {
  count++;
}
