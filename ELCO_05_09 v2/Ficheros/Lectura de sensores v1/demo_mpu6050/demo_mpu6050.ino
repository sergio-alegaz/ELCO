/* Variable to select data displayed:
 * 0: accelerometer measures
 * 1: gyroscope measures
 */

byte dataDisplayed = 0;

#include "I2Cdev.h"
#include "MPU6050.h"
#include "Wire.h"

#define MY_SIZE 5
 
const int mpuAddress = 0x68;  // Puede ser 0x68 o 0x69
MPU6050 mpu(mpuAddress);
 
int ax, ay, az;
int axVect[MY_SIZE], ayVect[MY_SIZE], azVect[MY_SIZE];
int gx, gy, gz;
 
 
// Factores de conversion
const float accScale = 2.0 * 9.81 / 32768.0;
const float gyroScale = 250.0 / 32768.0;
 
void printTab()
{
   Serial.print(F("\t"));
}
 
// Mostrar medidas en Sistema Internacional
void printRAW(int ax, int ay, int az)
{
  if (dataDisplayed == 0) {     
     Serial.print(ax * accScale); printTab(); 
     Serial.print(ay * accScale); printTab(); 
     Serial.print(az * accScale); printTab(); Serial.print("|"); printTab();
  } else {
     Serial.print(gx * gyroScale);  printTab();
     Serial.print(gy * gyroScale);  printTab();
     Serial.println(gz * gyroScale);
  }
}
 
void setup()
{
   Serial.begin(9600);
   
   pinMode(2,OUTPUT);
   pinMode(3,OUTPUT);
   pinMode(4,OUTPUT);
   pinMode(5,OUTPUT);
   pinMode(6,OUTPUT);
   
   digitalWrite(2,LOW);
   digitalWrite(3,HIGH);
   digitalWrite(4,HIGH);
   digitalWrite(5,HIGH);
   digitalWrite(6,HIGH);   
   Wire.begin();
   mpu.initialize();
   Serial.println(mpu.testConnection() ? F("IMU iniciado correctamente") : F("Error al iniciar IMU"));
}
 
void loop()
{ 

  for (int i = 2; i < MY_SIZE + 2; i++) {
    for (int j = 2; j < MY_SIZE + 2; j++) {
      digitalWrite(j, HIGH);
    }
    
   digitalWrite(i, LOW);

   //delay(1);

    // Leer las aceleraciones y velocidades angulares
   mpu.getAcceleration(&ax, &ay, &az);
   //mpu.getRotation(&gx, &gy, &gz);

   axVect[i-2] = ax;
   ayVect[i-2] = ay;
   azVect[i-2] = az;
 
   printRAW(axVect[i-2], ayVect[i-2], azVect[i-2]);
    
  }

  Serial.println("");
          
}
