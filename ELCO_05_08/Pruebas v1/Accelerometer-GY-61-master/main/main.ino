
#include "GY_61.h"

GY_61 accel;

void setup() {
    accel = GY_61(A5, A6, A7);
    Serial.begin(9600);
}
void loop() {
  Serial.print("X = ");
  Serial.print(accel.readx());
  Serial.print("     ");
  Serial.print(" Y = ");
  Serial.print(accel.ready());
  Serial.print("     ");
  Serial.print(" Z = ");
  Serial.print(accel.readz());
  Serial.print("     ");
  Serial.print(" Ac. Total = ");
  Serial.print(accel.acceltol());
  Serial.print("\n");
}
