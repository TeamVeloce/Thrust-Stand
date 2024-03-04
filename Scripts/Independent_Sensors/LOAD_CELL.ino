#include "HX711.h"

#define DOUT_PIN  7
#define CLK_PIN   8

HX711 scale;

void setup() {
  Serial.begin(9600);
  
  // Initialize the HX711 ADC
  scale.begin(DOUT_PIN, CLK_PIN);
  
  // Set the scale to gain 128 (default is 128)
  scale.set_scale();
  
  // Tare the scale
  scale.tare();
}

void loop() {
  // Read the raw ADC value
  long rawValue = scale.read();
  //Serial.println(rawValue);

  // Convert the raw value to mass in grams
  // Use the calibration factor for your specific load cell
  // You can obtain the calibration factor by dividing the known weight in grams by the raw value
  // For example, if a known weight of 100 grams produces a raw value of 1000, then the calibration factor would be 100 / 1000 = 0.1
  float calibrationFactor = 0.00332; // Adjust this value according to your calibration
  float massInGrams = rawValue * calibrationFactor;
  float final_mass = 270 - massInGrams;

  // Print the mass in grams
  Serial.print("Mass: ");
  Serial.print(final_mass);
  Serial.println(" grams");

  delay(1000); // Delay for stability
}
