/*
  Analog Input

  Demonstrates analog input by reading an analog sensor on analog pin 0 and
  turning on and off a light emitting diode(LED) connected to digital pin 13.
  The amount of time the LED will be on and off depends on the value obtained
  by analogRead().

  The circuit:
  - potentiometer
    center pin of the potentiometer to the analog input 0
    one side pin (either one) to ground
    the other side pin to +5V
  - LED
    anode (long leg) attached to digital output 13
    cathode (short leg) attached to ground

  - Note: because most Arduinos have a built-in LED attached to pin 13 on the
    board, the LED is optional.

  created by David Cuartielles
  modified 30 Aug 2011
  By Tom Igoe

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/AnalogInput
*/

#define numIterations 500

int sensorPin = A0;          // select the input pin for the potentiometer
int sensorValue = 0;         // variable to store the value coming from the sensor

int outputValue;             // 0, 1 or 2, depending on sensors position
double voltageValues[3];     // Equivalent voltage values to each of the flex sensor positions from Analog Inputs [0,1023]. Probably needs conversion. 
double threshold[2];         // To decide sensor's position
double v0, rFlex;

void setup() {

  Serial.begin(9600);

  /* ================================================== */
  /*           FLEX SENSOR CALIBRATION: START           */
  /* ================================================== */

  Serial.println("Para comenzar a calibrar el sensor, pulse cualquier tecla.");
  Serial.println();
  
  while (Serial.available() == 0)  {    
  }
  Serial.readString();

  Serial.println("Comenzaremos calibrando la posición en reposo. Por favor, mantenga el dedo en una posición relajada. Pulse cualquier tecla cuando desee empezar el proceso.");  
  Serial.println();
  
  while (Serial.available() == 0)  {    
  }
  Serial.readString();

  Serial.println("Procesando...");
  Serial.println();

  double sum = 0;

  for (int i = 0; i < numIterations; i++) {
    sum += (double)(analogRead(sensorPin));
    delay(10);
  }

  /* Voltage value [0,1023] equivalent to flat position */  
  voltageValues[0] = sum/numIterations;

  Serial.print("voltageValues[0]: ");
  Serial.print(voltageValues[0]/1023.0*5.0);
  Serial.println();
  Serial.println();

  Serial.println("¡Enhorabuena! Hemos calibrado la primera posición, solo quedan 2 más.");
  Serial.println();
  Serial.println("Ahora flexione el dedo a 90º y manténgalo flexionado hasta nuevo aviso. Pulse cualquier tecla para comenzar.");
  Serial.println();
  
  while (Serial.available() == 0)  {    
  }
  Serial.readString();

  Serial.println("Procesando...");
  Serial.println();

  sum = 0.0;

  for (int i = 0; i < numIterations; i++) {
    sum += (double)(analogRead(sensorPin));
    delay(10);
  }

  /* Voltage value [0,1023] equivalent to 90º position */
  voltageValues[1] = sum/numIterations;

  Serial.print("voltageValues[1]: ");
  Serial.print(voltageValues[1]/1023.0*5.0);
  Serial.println();
  Serial.println();

  Serial.println("¡Enhorabuena! Hemos calibrado la segunda posición, solo quedan 1 más.");
  Serial.println();
  Serial.println("Ahora flexione todo lo que pueda (¡Sin llegar a rompérselo!) y manténgalo ahí hasta nuevo aviso. Pulse cualquier tecla para comenzar.");
  Serial.println();
  
  while (Serial.available() == 0)  {    
  }  
  Serial.readString();

  Serial.println("Procesando...");
  Serial.println();

  sum = 0.0;

  for (int i = 0; i < numIterations; i++) {
    sum += (double)(analogRead(sensorPin));
    delay(10);
  }

  /* Voltage value [0,1023] equivalent to completely bent position */
  voltageValues[2] = sum/numIterations;

  Serial.print("voltageValues[2]: ");
  Serial.print(voltageValues[2]/1023.0*5.0);
  Serial.println();
  Serial.println();

  threshold[0] = (voltageValues[0]+voltageValues[1])/2.0/1023*5;
  threshold[1] = (voltageValues[1]+voltageValues[2])/2.0/1023*5;

  Serial.print("threshold[0]: ");
  Serial.print(threshold[0]);
  Serial.println();
  Serial.println();

  Serial.print("threshold[1]: ");
  Serial.print(threshold[1]);
  Serial.println();
  Serial.println();


  delay(5000);

  Serial.println("Ha completado con éxito el proceso. En unos instantes comenzará a recibir información de los sensores");
  
  /* ================================================== */
  /*           FLEX SENSOR CALIBRATION: END           */
  /* ================================================== */
  
}

void loop() {
  // read the value from the sensor: 
  
  sensorValue = analogRead(sensorPin);  

  v0 = ((double)sensorValue)/1023.0*5.0;

  if (v0 > threshold[0]) {
    outputValue = 0;
  } else if (v0 > threshold[1] && v0 <= threshold[0]) {
    outputValue = 1;
  } else {
    outputValue = 2;
  }

  rFlex = 38390.0*(5.0/v0 - 1);         // rAux = 38.39 kOhms

  //Serial.print("sensorValue = ");
  //Serial.print(sensorValue);
  //Serial.print("     ");

  //Serial.print("v0 = ");
  //Serial.print(v0);
  //Serial.print("     ");
  
  Serial.print("rFlex = ");
  Serial.print(rFlex);
  Serial.print("     ");

  Serial.print("outputValue = ");
  Serial.print(outputValue);
  Serial.println();
  
  // stop the program for <sensorValue> milliseconds:
  delay(500);
}
