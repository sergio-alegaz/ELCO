#include "TimerOne.h"
#include "I2Cdev.h"
#include "MPU6050.h"
#include "Wire.h"

int dataDisplayed = 0;

#define numIterations 100       // number of iterations for sensor calibration

String str0, str1;

/* ================================================== */
/*                      DEBOUNCE                      */
/* ================================================== */
const int timeThreshold = 225;
const int intPin = 2;
volatile int ISRCounter = 0;
int counter = 0;
long startTime = 0;


/* ================================================== */
/*           PREASURE SENSORS DECLARATION             */
/* ================================================== */

/* Pins used */
int preasurePin0 = A0;
int preasurePin1 = A1;
int preasurePin2 = A2;
int preasurePin3 = A3;

/* analogRead() values */
double preasureVoltage0 = 0;
double preasureVoltage1 = 0;
double preasureVoltage2 = 0;
double preasureVoltage3 = 0;

/* Variables to detect preasure <===> 0/1 */
int preasureValue0 = 0;
int preasureValue1 = 0;
int preasureValue2 = 0;
int preasureValue3 = 0;

/* ================================================== */
/*           FLEX SENSORS DECLARATION                 */
/* ================================================== */

/* Pins used */
int flexPin0 = A4;
int flexPin1 = A5;
int flexPin2 = A6;
int flexPin3 = A7;

/* analogRead() values */
double flexVoltage0 = 0;
double flexVoltage1 = 0;
double flexVoltage2 = 0;
double flexVoltage3 = 0;

/* FLEX thresholds */
double voltageValuesFlex[4][3];                // Equivalent to each flex sensor position
double thresholdFlex[4][2];                    // To distinguish flex sensor positions

/* Variables to detect flexion <===> 0/1/2 */
int flexValue0 = 0;
int flexValue1 = 0;
int flexValue2 = 0;
int flexValue3 = 0;

/* ================================================== */
/*             MPU6050 SENSOR DECLARATION             */
/* ================================================== */
#define MY_SIZE 5
#define epsilon 4.5
#define gravity 9.8

const int mpuAddress = 0x68;  // Puede ser 0x68 o 0x69
MPU6050 mpu(mpuAddress);
 
int ax, ay, az;
//int axVect[MY_SIZE], ayVect[MY_SIZE], azVect[MY_SIZE];
int accel[MY_SIZE];
int acceleration = 0;
int gx, gy, gz;
 
 
// Factores de conversion
const float accScale = 2.0 * 9.81 / 32768.0;
const float gyroScale = 250.0 / 32768.0;

void setup() {
    
  Serial.begin(115200);  
  Serial3.begin(115200); 

  str0 = "";
  str1 = "";

  /* ================================================== */
  /*               MPU6050 INITIALIZATION               */
  /* ================================================== */

  pinMode(2,OUTPUT);
  pinMode(3,OUTPUT);
  pinMode(4,OUTPUT);
  pinMode(5,OUTPUT);
  pinMode(6,OUTPUT);

  pinMode(18,INPUT_PULLUP);
   
  digitalWrite(2,LOW);
  digitalWrite(3,HIGH);
  digitalWrite(4,HIGH);
  digitalWrite(5,HIGH);
  digitalWrite(6,HIGH);   
  Wire.begin();
  mpu.initialize();
  Serial.println(mpu.testConnection() ? F("IMU iniciado correctamente") : F("Error al iniciar IMU")); Serial.println();

  /* ================================================== */
  /*           FLEX SENSOR CALIBRATION: START           */
  /* ================================================== */

  Serial.println("Para comenzar a calibrar los sensores FLEX, teclee cualquier tecla en el terminal, y pulse ENTER.");
  Serial.println();
  
  while (Serial.available() == 0)  {    
  }
  Serial.readString();

  Serial.println("Comenzaremos calibrando la posición en reposo. Por favor, mantenga los dedos en una posición relajada. Pulse cualquier tecla y ENTER cuando desee empezar el proceso.");  
  Serial.println();
  
  while (Serial.available() == 0)  {    
  }
  Serial.readString();

  Serial.println("Procesando...");
  Serial.println();

  double sum[1][4] = {0,0,0,0};         // To calculate an average value of equivalent voltage for each position,
                                        // First, accumulates numIteration values, then estimates an average value.
                                        
  for (int i = 0; i < numIterations; i++) {
    sum[0][0] += (double)(analogRead(flexPin0));
    sum[0][1] += (double)(analogRead(flexPin1));
    sum[0][2] += (double)(analogRead(flexPin2));
    sum[0][3] += (double)(analogRead(flexPin3));
    delay(10);
  }

  /* Voltage value [0,1023] equivalent to flat position */  
  voltageValuesFlex[0][0] = sum[0][0]/numIterations;
  voltageValuesFlex[1][0] = sum[0][1]/numIterations;
  voltageValuesFlex[2][0] = sum[0][2]/numIterations;
  voltageValuesFlex[3][0] = sum[0][3]/numIterations;

  Serial.print("voltageValuesFlex[0][0]: ");
  Serial.print(voltageValuesFlex[0][0]/1023.0*5.0);
  Serial.print("     ");
  Serial.print("voltageValuesFlex[1][0]: ");
  Serial.print(voltageValuesFlex[1][0]/1023.0*5.0);
  Serial.print("     ");
  Serial.print("voltageValuesFlex[2][0]: ");
  Serial.print(voltageValuesFlex[2][0]/1023.0*5.0);
  Serial.print("     ");
  Serial.print("voltageValuesFlex[3][0]: ");
  Serial.print(voltageValuesFlex[3][0]/1023.0*5.0);
  Serial.print("     ");
  Serial.println();
  Serial.println();

  Serial.println("¡Enhorabuena! Hemos calibrado la primera posición, solo quedan 2 más.");
  Serial.println();
  Serial.println("Ahora flexione los dedos a 90º y manténgalos flexionado hasta nuevo aviso. Pulse cualquier tecla y ENTER para comenzar.");
  Serial.println();
  
  while (Serial.available() == 0)  {    
  }
  Serial.readString();

  Serial.println("Procesando...");
  Serial.println();

  /* Reinitialize sum */

  for (int i = 0; i < 4; i++) {
    sum[0][i] = 0.0;
  }

  for (int i = 0; i < numIterations; i++) {
    sum[0][0] += (double)(analogRead(flexPin0));
    sum[0][1] += (double)(analogRead(flexPin1));
    sum[0][2] += (double)(analogRead(flexPin2));
    sum[0][3] += (double)(analogRead(flexPin3));
    delay(10);
  }

  /* Voltage value [0,1023] equivalent to 90º position */
  voltageValuesFlex[0][1] = sum[0][0]/numIterations;
  voltageValuesFlex[1][1] = sum[0][1]/numIterations;
  voltageValuesFlex[2][1] = sum[0][2]/numIterations;
  voltageValuesFlex[3][1] = sum[0][3]/numIterations;

  Serial.print("voltageValuesFlex[0][1]: ");
  Serial.print(voltageValuesFlex[0][1]/1023.0*5.0);
  Serial.print("     ");
  Serial.print("voltageValuesFlex[1][1]: ");
  Serial.print(voltageValuesFlex[1][1]/1023.0*5.0);
  Serial.print("     ");
  Serial.print("voltageValuesFlex[2][1]: ");
  Serial.print(voltageValuesFlex[2][1]/1023.0*5.0);
  Serial.print("     ");
  Serial.print("voltageValuesFlex[3][1]: ");
  Serial.print(voltageValuesFlex[3][1]/1023.0*5.0);
  Serial.print("     ");
  Serial.println();
  Serial.println();

  Serial.println("¡Enhorabuena! Hemos calibrado la segunda posición, solo queda 1 más.");
  Serial.println();
  Serial.println("Ahora flexione todo lo que pueda (¡¡¡sin llegar a rompérselos!!!) y manténgalos ahí hasta nuevo aviso. Pulse cualquier tecla y ENTER para comenzar.");
  Serial.println();
  
  while (Serial.available() == 0)  {    
  }  
  Serial.readString();

  Serial.println("Procesando...");
  Serial.println();

  /* Reinitialize sum */

  for (int i = 0; i < 4; i++) {
    sum[0][i] = 0.0;
  }

  for (int i = 0; i < numIterations; i++) {
    sum[0][0] += (double)(analogRead(flexPin0));
    sum[0][1] += (double)(analogRead(flexPin1));
    sum[0][2] += (double)(analogRead(flexPin2));
    sum[0][3] += (double)(analogRead(flexPin3));
    delay(10);
  }

  /* Voltage value [0,1023] equivalent to completely bent position */
  voltageValuesFlex[0][2] = sum[0][0]/numIterations;
  voltageValuesFlex[1][2] = sum[0][1]/numIterations;
  voltageValuesFlex[2][2] = sum[0][2]/numIterations;
  voltageValuesFlex[3][2] = sum[0][3]/numIterations;

  Serial.print("voltageValuesFlex[0][2]: ");
  Serial.print(voltageValuesFlex[0][2]/1023.0*5.0);
  Serial.print("     ");
  Serial.print("voltageValuesFlex[1][2]: ");
  Serial.print(voltageValuesFlex[1][2]/1023.0*5.0);
  Serial.print("     ");
  Serial.print("voltageValuesFlex[2][2]: ");
  Serial.print(voltageValuesFlex[2][2]/1023.0*5.0);
  Serial.print("     ");
  Serial.print("voltageValuesFlex[3][2]: ");
  Serial.print(voltageValuesFlex[3][2]/1023.0*5.0);
  Serial.print("     ");
  Serial.println();
  Serial.println();

  thresholdFlex[0][0] = (voltageValuesFlex[0][0]+voltageValuesFlex[0][1])/2.0/1023*5;
  thresholdFlex[0][1] = (voltageValuesFlex[0][1]+voltageValuesFlex[0][2])/2.0/1023*5;

  thresholdFlex[1][0] = (voltageValuesFlex[1][0]+voltageValuesFlex[1][1])/2.0/1023*5;
  thresholdFlex[1][1] = (voltageValuesFlex[1][1]+voltageValuesFlex[1][2])/2.0/1023*5;

  thresholdFlex[2][0] = (voltageValuesFlex[2][0]+voltageValuesFlex[2][1])/2.0/1023*5;
  thresholdFlex[2][1] = (voltageValuesFlex[2][1]+voltageValuesFlex[2][2])/2.0/1023*5;

  thresholdFlex[3][0] = (voltageValuesFlex[3][0]+voltageValuesFlex[3][1])/2.0/1023*5;
  thresholdFlex[3][1] = (voltageValuesFlex[3][1]+voltageValuesFlex[3][2])/2.0/1023*5;

  Serial.println("THRESHOLD FLEX VALUES:");
  Serial.println();

  Serial.print("thresholdFlex[0][0]: ");
  Serial.print(thresholdFlex[0][0]);
  Serial.print("   ");
  Serial.print("thresholdFlex[0][1]: ");
  Serial.print(thresholdFlex[0][1]);
  Serial.println();
  Serial.print("thresholdFlex[1][0]: ");
  Serial.print(thresholdFlex[1][0]);
  Serial.print("   ");
  Serial.print("thresholdFlex[1][1]: ");
  Serial.print(thresholdFlex[1][1]);
  Serial.println();
  Serial.print("thresholdFlex[2][0]: ");
  Serial.print(thresholdFlex[2][0]);
  Serial.print("   ");
  Serial.print("thresholdFlex[2][1]: ");
  Serial.print(thresholdFlex[2][1]);
  Serial.println();
  Serial.print("thresholdFlex[3][0]: ");
  Serial.print(thresholdFlex[3][0]);
  Serial.print("   ");
  Serial.print("thresholdFlex[3][1]: ");
  Serial.print(thresholdFlex[3][1]);
  Serial.println();
  Serial.println();


  delay(2000);

  Serial.println("Ha completado con éxito el proceso de calibración de los sensores FLEX. En unos instantes comenzará a recibir información de los ellos.");
  Serial.println(); 
  
  /* ================================================== */
  /*           FLEX SENSOR CALIBRATION: END             */
  /* ================================================== */
  
  //Timer1.initialize(100000);
  //Timer1.attachInterrupt(printAllSensors);

  //attachInterrupt(digitalPinToInterrupt(18), printAllSensors, FALLING);
  delay(2000);
  
}

void loop() {

  str0 = "";  
  
  // put your main code here, to run repeatedly:
  preasureVoltage0 = ((double)analogRead(preasurePin0))/1023.0*5.0;  
  preasureVoltage1 = ((double)analogRead(preasurePin1))/1023.0*5.0;  
  preasureVoltage2 = ((double)analogRead(preasurePin2))/1023.0*5.0;  
  preasureVoltage3 = ((double)analogRead(preasurePin3))/1023.0*5.0;  

  flexVoltage0 = ((double)analogRead(flexPin0))/1023.0*5.0;  
  flexVoltage1 = ((double)analogRead(flexPin1))/1023.0*5.0;  
  flexVoltage2 = ((double)analogRead(flexPin2))/1023.0*5.0;  
  flexVoltage3 = ((double)analogRead(flexPin3))/1023.0*5.0;
  
/* If preasure sensor > 2.5 V, then preasure is detected */
  if (preasureVoltage0 < 2.5) {
    preasureValue0 = 0;
  } else {
    preasureValue0 = 1;
  }

  if (preasureVoltage1 < 2.5) {
    preasureValue1 = 0;
  } else {
    preasureValue1 = 1;
  }

  if (preasureVoltage2 < 2.5) {
    preasureValue2 = 0;
  } else {
    preasureValue2 = 1;
  }

  if (preasureVoltage3 < 2.5) {
    preasureValue3 = 0;
  } else {
    preasureValue3 = 1;
  }

/* Flex sensor position detection acording to thresholds previously established */
  if (flexVoltage0 > thresholdFlex[0][0]) {
    flexValue0 = 0;
  } else if (flexVoltage0 > thresholdFlex[0][1] && flexVoltage0 < thresholdFlex[0][0]) {
    flexValue0 = 1;
  } else {
    flexValue0 = 2;
  }

  if (flexVoltage1 > thresholdFlex[1][0]) {
    flexValue1 = 0;
  } else if (flexVoltage1 > thresholdFlex[1][1] && flexVoltage1 < thresholdFlex[1][0]) {
    flexValue1 = 1;
  } else {
    flexValue1 = 2;
  }

  if (flexVoltage2 > thresholdFlex[2][0]) {
    flexValue2 = 0;
  } else if (flexVoltage2 > thresholdFlex[2][1] && flexVoltage2 < thresholdFlex[2][0]) {
    flexValue2 = 1;
  } else {
    flexValue2 = 2;
  }

  if (flexVoltage3 > thresholdFlex[3][0]) {
    flexValue3 = 0;
  } else if (flexVoltage3 > thresholdFlex[3][1] && flexVoltage3 < thresholdFlex[3][0]) {
    flexValue3 = 1;
  } else {
    flexValue3 = 2;
  }

  
  /*Serial.print("p0: ");
  Serial.print(preasureValue0);
  printTab();  

  Serial.print("p1: ");
  Serial.print(preasureValue1);
  printTab();

  Serial.print("p2: ");
  Serial.print(preasureValue2);
  printTab();

  Serial.print("p3: ");
  Serial.print(preasureValue3);
  printTab();*/  

  str0 = str0 + preasureValue0;
  str0 = str0 + preasureValue1;
  str0 = str0 + preasureValue2;
  str0 = str0 + preasureValue3;

  //Serial.print("f0: ");
  //Serial.print(flexValue0);  
  //printTab();
  str0 = str0 + flexValue0;

  /*Serial.print("f1: ");
  Serial.print(flexValue1);
  printTab();*/
  str0 = str0 + flexValue1;

  /*Serial.print("f2: ");
  Serial.print(flexValue2);
  printTab();*/
  str0 = str0 + flexValue2;

  /*Serial.print("f3: ");
  Serial.print(flexValue3);
  printTab();*/
  str0 = str0 + flexValue3;

  //str0 = str0 + "0000";  

  /*for (int i = 2; i < MY_SIZE + 2; i++) {
    for (int j = 2; j < MY_SIZE + 2; j++) {
      digitalWrite(j, HIGH);
    }
    
   digitalWrite(i, LOW);*/

   //delay(1);

   // Leer las aceleraciones y velocidades angulares
   mpu.getAcceleration(&ax, &ay, &az);
   //mpu.getRotation(&gx, &gy, &gz);
   //axVect[i-2] = ax;
   //ayVect[i-2] = ay;
   //azVect[i-2] = az;

   acceleration = getAccelerometerPosition(ax*accScale, ay*accScale, az*accScale);

   //Serial.print("acc"); Serial.print(i-2); Serial.print(": ");
   //Serial.print(accel[i-2]); printTab();
 
   //printRAW(axVect[i-2], ayVect[i-2], azVect[i-2]);
    
  /*}*/ 
  
  /*if (counter != ISRCounter)
  
   {
      Serial.print("Counter: "); Serial.println(counter);
      Serial.print("ISRCounter: "); Serial.println(ISRCounter);
      counter = ISRCounter;
      Serial3.println(str1); 
   }*/

   str0 = str0 + acceleration;
   
   str1 = str0;

   Serial.println(str1);
  
  delay(20);  
  
}

/* ================================================== */
/*                  MPU6050 FUNCTIONS                 */
/* ================================================== */

void printTab()
{
   Serial.print("  ");
}

// Mostrar medidas en Sistema Internacional
void printRAW(int ax, int ay, int az)
{
  if (dataDisplayed == 0) {     
     Serial.print("|   ");
     Serial.print(ax * accScale); printTab(); 
     Serial.print(ay * accScale); printTab(); 
     Serial.print(az * accScale); printTab(); Serial.print("|");
  } else {
     Serial.print(gx * gyroScale);  printTab();
     Serial.print(gy * gyroScale);  printTab();
     Serial.println(gz * gyroScale);
  }
}

int getAccelerometerPosition(int ax, int ay, int az) {

  if ((ax < gravity + epsilon) && (ax > gravity - epsilon)) {
    return 1;
    /*if (abs(ay*az) < 2) {
      return 1;
    }*/
  }

  else if ((ay < gravity + epsilon) && (ay > gravity - epsilon)) {
    return 2;
    /*if (abs(ax*az) < 2) {
      return 2;
    }*/
  }

  else if ((az < gravity + epsilon) && (az > gravity - epsilon)) {
    return 3;
    /*if (abs(ax*ay) < 2) {
      return 3;
    }*/
  }

  else if ((ax*(-1) < gravity + epsilon) && (ax*(-1) > gravity - epsilon)) {    
    return 4;
    /*if (abs(ay*az) < 2) {
      return 4;
    }*/
  }

  else if ((ay*(-1) < gravity + epsilon) && (ay*(-1) > gravity - epsilon)) {
    return 5;
    /*if (abs(ax*az) < 2) {
      return 5;
    }*/
  }

  else if ((az*(-1) < gravity + epsilon) && (az*(-1) > gravity - epsilon)) {
    return 6;
    /*if (abs(ax*ay) < 2) {
      return 6;
    }*/
  }

  else {
    return 0;
  }
  
}
  
void printAllSensors() {

  if (millis() - startTime > timeThreshold)
   {
      ISRCounter++;
      startTime = millis();
   }     
  
}
