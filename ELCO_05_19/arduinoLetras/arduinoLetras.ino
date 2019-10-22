#include "TimerOne.h"
#include "I2Cdev.h"
#include "MPU6050.h"
#include "Wire.h"

#define numIterations 100       // number of iterations for sensor calibration

String str0, str1;

/* ================================================== */
/*             SAMPLE COUNTING VARIABLES              */
/* ================================================== */
int cnt_trama = 0;
const int trama_threshold = 20;
String trama_actual = "000000000";
String enviado = "000000000";
int sent = 0;
int cnt_reposo = 0;

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
#define epsilon 3.5
#define gravity 9.8

const int mpuAddress = 0x68;  // Puede ser 0x68 o 0x69
MPU6050 mpu(mpuAddress);
 
int ax, ay, az;
int acceleration;
int gx, gy, gz; 
 
/* Conversion values */
const float accScale = 2.0 * 9.81 / 32768.0;
const float gyroScale = 250.0 / 32768.0;

void setup() {
    
  Serial.begin(115200);    

  str0 = "";
  str1 = "";

  /* ================================================== */
  /*               MPU6050 INITIALIZATION               */
  /* ================================================== */
    
  Wire.begin();
  mpu.initialize();
  Serial.print(mpu.testConnection() ? F("IMU started correctly") : F("Error when starting IMU")); Serial.print("\n\n");

  /* ================================================== */
  /*           FLEX SENSOR CALIBRATION: START           */
  /* ================================================== */

  Serial.print("To begin FLEX sensors configuration, press any key.");
  Serial.print("\n\n");
  
  while (Serial.available() == 0)  {    
  }
  Serial.readString();

  Serial.print("Step 1: Keep your fingers in a relaxed position. Press any key when you are ready.");  
  Serial.print("\n\n");
  
  while (Serial.available() == 0)  {    
  }
  Serial.readString();

  Serial.print("Processing...");
  Serial.print("\n\n");

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
  Serial.print("\n\n");  

  Serial.print("Congratulations! Step 1 succesfully completed.");
  Serial.print("\n\n");  
  Serial.print("Step 2: Keep your fingers forming 90 degrees with your hand. Press any key when you are ready.");
  Serial.print("\n\n");
  
  while (Serial.available() == 0)  {    
  }
  Serial.readString();

  Serial.print("Processing...");
  Serial.print("\n\n");

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
  Serial.print("\n\n");  

  Serial.print("Congratulations! Step 2 succesfully completed.");
  Serial.print("\n\n"); 
  Serial.print("Step 3: Flex your fingers as much as possible, trying to touch your hand with your fingerprints. Press any key when you are ready."); 
  Serial.print("\n\n");  
  
  while (Serial.available() == 0)  {    
  }  
  Serial.readString();

  Serial.print("Processing...");
  Serial.print("\n\n");  

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
  Serial.print("\n\n");  

  thresholdFlex[0][0] = (voltageValuesFlex[0][0]+voltageValuesFlex[0][1])/2.0/1023*5;
  thresholdFlex[0][1] = (voltageValuesFlex[0][1]+voltageValuesFlex[0][2])/2.0/1023*5;

  thresholdFlex[1][0] = (voltageValuesFlex[1][0]+voltageValuesFlex[1][1])/2.0/1023*5;
  thresholdFlex[1][1] = (voltageValuesFlex[1][1]+voltageValuesFlex[1][2])/2.0/1023*5;

  thresholdFlex[2][0] = (voltageValuesFlex[2][0]+voltageValuesFlex[2][1])/2.0/1023*5;
  thresholdFlex[2][1] = (voltageValuesFlex[2][1]+voltageValuesFlex[2][2])/2.0/1023*5;

  thresholdFlex[3][0] = (voltageValuesFlex[3][0]+voltageValuesFlex[3][1])/2.0/1023*5;
  thresholdFlex[3][1] = (voltageValuesFlex[3][1]+voltageValuesFlex[3][2])/2.0/1023*5;

  Serial.print("THRESHOLD FLEX VALUES:");
  Serial.print("\n\n");  

  Serial.print("thresholdFlex[0][0]: ");
  Serial.print(thresholdFlex[0][0]);
  Serial.print("   ");
  Serial.print("thresholdFlex[0][1]: ");
  Serial.print(thresholdFlex[0][1]);
  Serial.print("\n");    
  Serial.print("thresholdFlex[1][0]: ");
  Serial.print(thresholdFlex[1][0]);
  Serial.print("   ");
  Serial.print("thresholdFlex[1][1]: ");
  Serial.print(thresholdFlex[1][1]);
  Serial.print("\n");  
  Serial.print("thresholdFlex[2][0]: ");
  Serial.print(thresholdFlex[2][0]);
  Serial.print("   ");
  Serial.print("thresholdFlex[2][1]: ");
  Serial.print(thresholdFlex[2][1]);
  Serial.print("\n");  
  Serial.print("thresholdFlex[3][0]: ");
  Serial.print(thresholdFlex[3][0]);
  Serial.print("   ");
  Serial.print("thresholdFlex[3][1]: ");
  Serial.print(thresholdFlex[3][1]);
  Serial.print("\n\n");  


  delay(1000);

  Serial.print("FLEX sensors configuration succesfully completed. You will start receiving information in a few moments.");  
  Serial.print("\n\n");  
  
  /* ================================================== */
  /*           FLEX SENSOR CALIBRATION: END             */
  /* ================================================== */    

  delay(1000);
  Serial.print("start");  
  delay(1000);
  
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

  /* Add preasure values to str0 */
  str0 = str0 + preasureValue0;
  str0 = str0 + preasureValue1;
  str0 = str0 + preasureValue2;
  str0 = str0 + preasureValue3;

  /* Add FLEX values to str0 */
  str0 = str0 + flexValue0;
  str0 = str0 + flexValue1;
  str0 = str0 + flexValue2;
  str0 = str0 + flexValue3;    

  mpu.getAcceleration(&ax, &ay, &az);
  acceleration = getAccelerometerPosition(ax*accScale, ay*accScale, az*accScale);

  /* Add acceleration value to str0 */
  str0 = str0 + acceleration;

  /* Save str0 in str1. str1 will be the sent word */
  str1 = str0;

  /* ---------- Symbol Decision ---------- */
  if (str1 == trama_actual) {
    cnt_trama = cnt_trama + 1;
  } else {
    trama_actual = str1;
    cnt_trama = 1;
  }

  if (cnt_trama >= trama_threshold && trama_actual != enviado) {
    enviado = str1;
    Serial.print(enviado);
  }
  /* ---------- Symbol Decision ---------- */
  
  delay(20);  
  
}

/* ================================================== */
/*                  MPU6050 FUNCTIONS                 */
/* ================================================== */

int getAccelerometerPosition(int ax, int ay, int az) {

  if ((ax < gravity + epsilon) && (ax > gravity - epsilon)) {
    return 1;
  }

  else if ((ay < gravity + epsilon) && (ay > gravity - epsilon)) {
    return 2;
  }

  else if ((az < gravity + epsilon) && (az > gravity - epsilon)) {
    return 3;
  }

  else if ((ax*(-1) < gravity + epsilon) && (ax*(-1) > gravity - epsilon)) {    
    return 4;
  }

  else if ((ay*(-1) < gravity + epsilon) && (ay*(-1) > gravity - epsilon)) {
    return 5;
  }

  else if ((az*(-1) < gravity + epsilon) && (az*(-1) > gravity - epsilon)) {
    return 6;
  }

  else {
    return 0;
  }
  
}  
