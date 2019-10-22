// Programa : Teste Giroscopio L3G4200D
// Adaptacoes : Arduino e Cia

#include <Wire.h>

/* Change this value to select data displayed */
/*
 * 0: Accelerometer values
 * 1: Compass values
 * 2: Temperature, preasure and altitude values 
 * 3: Angular speed values
 */

const int dataDisplayed = 0; 

/* ======================================== */
/*     GYROSCOPE REGISTERS AND VARIABLES    */
/* ======================================== */
#define CTRL_REG1 0x20
#define CTRL_REG2 0x21
#define CTRL_REG3 0x22
#define CTRL_REG4 0x23
#define CTRL_REG5 0x24

//Endereco I2C do L3G4200D
int L3G4200D_Address = 105;

int x;
int y;
int z;

/* ======================================== */
/*   ACCELEROMETER REGISTERS AND VARIABLES  */
/* ======================================== */
#define Register_ID 0
#define Register_2D 0x2D
#define Register_X0 0x32
#define Register_X1 0x33
#define Register_Y0 0x34
#define Register_Y1 0x35
#define Register_Z0 0x36
#define Register_Z1 0x37

// Endereco I2C do sensor : 83 em decimal ou 0x53
int ADXAddress = 0x53;  // the default 7-bit slave address
int reading = 0;
int val=0;
int X0,X1,X_out;
int Y0,Y1,Y_out;
int Z1,Z0,Z_out;
double Xg,Yg,Zg;

/* ======================================== */
/*     COMPASS REGISTERS AND VARIABLES      */
/* ======================================== */
// Define o endereco do HMC5883 - 0x1E ou 30 em decimal
#define Compass_address 0x1E 


/* ======================================== */
/*    TEMPERATURE REGISTERS AND VARIABLES   */
/* ======================================== */
//Define o endereco I2C do BMP085 - 0x77 ou 119 em decimal
#define BMP085_ADDRESS 0x77  

const unsigned char OSS = 0;  // Oversampling Setting

// Calibration values
int ac1;
int ac2;
int ac3;
unsigned int ac4;
unsigned int ac5;
unsigned int ac6;
int b1;
int b2;
int mb;
int mc;
int md;

// b5 is calculated in bmp085GetTemperature(...), this 
// variable is also used in bmp085GetPressure(...)
// so ...Temperature(...) must be called before ...Pressure(...).
long b5;

void setup()
{
  Wire.begin();
  Serial.begin(9600);

  /* GYROSCOPE INITIALIZATION */
  Serial.println("Inicializando o L3G4200D");
  // Configura o L3G4200 para 200, 500 ou 2000 graus/seg
  setupL3G4200D(2000); 
  // Aguarda a resposta do sensor
  delay(1500);

  /* ACCELEROMETER INITIALIZATION */
  delay(100);
  // enable to measute g data
  Wire.beginTransmission(ADXAddress);
  Wire.write(Register_2D);
  Wire.write(8);                //measuring enable
  Wire.endTransmission();     // stop transmitting

  /* COMPASS INITIALIZATIONION */
  // Inicializa o HMC5883
  Wire.beginTransmission(Compass_address);
  // Seleciona o modo
  Wire.write(0x02); 
  // Modo de medicao continuo
  Wire.write(0x00); 
  Wire.endTransmission();

  /* TEMPERATURE INITIALIZATION */
  bmp085Calibration();
   
}

void loop()
{
  
  /* ACCELEROMETER VALUES */
  //--------------X
  Wire.beginTransmission(ADXAddress); // transmit to device
  Wire.write(Register_X0);
  Wire.write(Register_X1);
  Wire.endTransmission();
  Wire.requestFrom(ADXAddress,2); 
  if(Wire.available()<=2)   
  {
    X0 = Wire.read();
    X1 = Wire.read(); 
    X1=X1<<8;
    X_out=X0+X1;   
  }

  //------------------Y
  Wire.beginTransmission(ADXAddress); // transmit to device
  Wire.write(Register_Y0);
  Wire.write(Register_Y1);
  Wire.endTransmission();
  Wire.requestFrom(ADXAddress,2); 
  if(Wire.available()<=2)   
  {
    Y0 = Wire.read();
    Y1 = Wire.read(); 
    Y1=Y1<<8;
    Y_out=Y0+Y1;
  }
  //------------------Z
  Wire.beginTransmission(ADXAddress); // transmit to device
  Wire.write(Register_Z0);
  Wire.write(Register_Z1);
  Wire.endTransmission();
  Wire.requestFrom(ADXAddress,2); 
  if(Wire.available()<=2)   
  {
    Z0 = Wire.read();
    Z1 = Wire.read(); 
    Z1=Z1<<8;
    Z_out=Z0+Z1;
  }
  //
  Xg=X_out/256.0*9.81;
  Yg=Y_out/256.0*9.81;
  Zg=Z_out/256.0*9.81;

  if (dataDisplayed == 0) {
    Serial.print("Xa = ");
    Serial.print(Xg);
    Serial.print("   ");
    Serial.print("Ya = ");
    Serial.print(Yg);
    Serial.print("   ");
    Serial.print("Za = ");
    Serial.print(Zg);
    Serial.println(""); 
  }    

  /* COMPASS VALUES */
  int xComp,yComp,zComp; //triple axis data
  
  // Indica ao HMC5883 para iniciar a leitura
  Wire.beginTransmission(Compass_address);
  Wire.write(0x03); //select register 3, X MSB register
  Wire.endTransmission();
 
  // Le os dados de cada eixo, 2 registradores por eixo
  Wire.requestFrom(Compass_address, 6);
  if(6<=Wire.available())
  {
    xComp = Wire.read()<<8; //X msb
    xComp |= Wire.read(); //X lsb
    zComp = Wire.read()<<8; //Z msb
    zComp |= Wire.read(); //Z lsb
    yComp = Wire.read()<<8; //Y msb
    yComp |= Wire.read(); //Y lsb
  }

  if (dataDisplayed == 1) {
    // Imprime os vaores no serial monitor
    Serial.print("Xc = ");
    Serial.print(xComp/1090.0);
    Serial.print("   ");
    Serial.print("Yc = ");
    Serial.print(yComp/1090.0);
    Serial.print("   ");
    Serial.print("Zc = ");
    Serial.print(zComp/1090.0);
    Serial.println("");
  }
  
  
  /* TEMPERATURE VALUES */
  // Chama a rotina que calcula a temperatura
  // Esta rotina DEVE ser executada primeiro
  float temperature = bmp085GetTemperature(bmp085ReadUT());
  // Chama a rotina que calcula a pressao
  float pressure = bmp085GetPressure(bmp085ReadUP());
  // Chama a rotina que calcula atmosfera
  float atm = pressure / 101325; 
  // Chama a rotina que calcula a altitude
  float altitude = calcAltitude(pressure); 

  if (dataDisplayed == 2) {
    Serial.print("T = ");
    // Mostra a temperatura com 2 casas decimais
    Serial.print(temperature, 2); 
    Serial.print(" C");
    Serial.print("   ");
  
    Serial.print("P = ");
    Serial.print(pressure, 0); 
    Serial.print(" Pa");
    Serial.print("   ");
    
  
    Serial.print("P(atm) = ");
    // Mostra o valor com 4 casas decimais
    Serial.print(atm, 4); //display 4 decimal places
    Serial.print("   ");
  
    Serial.print("Alt: ");
    // Mostra o valor com 2 casas decimais
    Serial.print(altitude, 2); 
    Serial.print(" M");
    Serial.println("");
  }  

  /* GYROSCOPE VALUES */
  // Atualiza os valores de X, Y e Z
  getGyroValues();  

  if (dataDisplayed == 3) {
    // Mostra os valores no serial monitor
    Serial.print("vAngX = ");
    Serial.print(x);
    Serial.print("   ");
  
    Serial.print("vAngY = ");
    Serial.print(y);
    Serial.print("   ");
  
    Serial.print("vAngZ = ");
    Serial.print(z);
    Serial.println("");  
  }    
  
  
}


/* ======================================== */
/*          GYROSCOPE FUNCTIONS             */
/* ======================================== */
void getGyroValues()
{
  // Rotina para leitura dos valores de X, Y e Z
  byte xMSB = readRegister(L3G4200D_Address, 0x29);
  byte xLSB = readRegister(L3G4200D_Address, 0x28);
  x = ((xMSB << 8) | xLSB);

  byte yMSB = readRegister(L3G4200D_Address, 0x2B);
  byte yLSB = readRegister(L3G4200D_Address, 0x2A);
  y = ((yMSB << 8) | yLSB);

  byte zMSB = readRegister(L3G4200D_Address, 0x2D);
  byte zLSB = readRegister(L3G4200D_Address, 0x2C);
  z = ((zMSB << 8) | zLSB);
}

int setupL3G4200D(int scale)
{
  //From  Jim Lindblom of Sparkfun's code

  // Enable x, y, z and turn off power down:
  writeRegister(L3G4200D_Address, CTRL_REG1, 0b00001111);

  // If you'd like to adjust/use the HPF, you can edit the line below to configure CTRL_REG2:
  writeRegister(L3G4200D_Address, CTRL_REG2, 0b00000000);

  // Configure CTRL_REG3 to generate data ready interrupt on INT2
  // No interrupts used on INT1, if you'd like to configure INT1
  // or INT2 otherwise, consult the datasheet:
  writeRegister(L3G4200D_Address, CTRL_REG3, 0b00001000);

  // CTRL_REG4 controls the full-scale range, among other things:
  if(scale == 250){
    writeRegister(L3G4200D_Address, CTRL_REG4, 0b00000000);
  }else if(scale == 500){
    writeRegister(L3G4200D_Address, CTRL_REG4, 0b00010000);
  }else{
    writeRegister(L3G4200D_Address, CTRL_REG4, 0b00110000);
  }

  // CTRL_REG5 controls high-pass filtering of outputs, use it
  // if you'd like:
  writeRegister(L3G4200D_Address, CTRL_REG5, 0b00000000);
}

void writeRegister(int deviceAddress, byte address, byte val) 
{
    Wire.beginTransmission(deviceAddress); // start transmission to device 
    Wire.write(address);       // send register address
    Wire.write(val);         // send value to write
    Wire.endTransmission();     // end transmission
}

int readRegister(int deviceAddress, byte address)
{
    int v;
    Wire.beginTransmission(deviceAddress);
    Wire.write(address); // register to read
    Wire.endTransmission();

    Wire.requestFrom(deviceAddress, 1); // read a byte

    while(!Wire.available()) 
    {
        // waiting
    }
    v = Wire.read();
    return v;
}

/* ======================================== */
/*          TEMPERATURE FUNCTIONS           */
/* ======================================== */
// Armazena todos os valores de calibracao do BMP085 em 
// variaveis globais. Valores de calibracao sao exigidos para
// calcular temperatura e pressao
// Esta funcao deve ser chamada/acionada no inicio do programa
void bmp085Calibration()
{
  ac1 = bmp085ReadInt(0xAA);
  ac2 = bmp085ReadInt(0xAC);
  ac3 = bmp085ReadInt(0xAE);
  ac4 = bmp085ReadInt(0xB0);
  ac5 = bmp085ReadInt(0xB2);
  ac6 = bmp085ReadInt(0xB4);
  b1 = bmp085ReadInt(0xB6);
  b2 = bmp085ReadInt(0xB8);
  mb = bmp085ReadInt(0xBA);
  mc = bmp085ReadInt(0xBC);
  md = bmp085ReadInt(0xBE);
}

// Calcula a temperatura em graus C
float bmp085GetTemperature(unsigned int ut)
{
  long x1, x2;

  x1 = (((long)ut - (long)ac6)*(long)ac5) >> 15;
  x2 = ((long)mc << 11)/(x1 + md);
  b5 = x1 + x2;

  float temp = ((b5 + 8)>>4);
  temp = temp /10;

  return temp;
}

// Calcula a pressao. Os valores de calibracao devem ser conhecidos
// b5 tambem eh exigido, logo a funcao bmp085GetTemperature(...) deve
// ser executada primeiro.
// Funcao retorna pressao em unidades de Pa
long bmp085GetPressure(unsigned long up){
  long x1, x2, x3, b3, b6, p;
  unsigned long b4, b7;

  b6 = b5 - 4000;
  // Calcula B3
  x1 = (b2 * (b6 * b6)>>12)>>11;
  x2 = (ac2 * b6)>>11;
  x3 = x1 + x2;
  b3 = (((((long)ac1)*4 + x3)<<OSS) + 2)>>2;

  // Calcula B4
  x1 = (ac3 * b6)>>13;
  x2 = (b1 * ((b6 * b6)>>12))>>16;
  x3 = ((x1 + x2) + 2)>>2;
  b4 = (ac4 * (unsigned long)(x3 + 32768))>>15;

  b7 = ((unsigned long)(up - b3) * (50000>>OSS));
  if (b7 < 0x80000000)
    p = (b7<<1)/b4;
  else
    p = (b7/b4)<<1;

  x1 = (p>>8) * (p>>8);
  x1 = (x1 * 3038)>>16;
  x2 = (-7357 * p)>>16;
  p += (x1 + x2 + 3791)>>4;

  long temp = p;
  return temp;
}

// Read 1 byte from the BMP085 at 'address'
char bmp085Read(unsigned char address)
{
  unsigned char data;

  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(address);
  Wire.endTransmission();

  Wire.requestFrom(BMP085_ADDRESS, 1);
  while(!Wire.available())
    ;

  return Wire.read();
}

// Read 2 bytes from the BMP085
// First byte will be from 'address'
// Second byte will be from 'address'+1
int bmp085ReadInt(unsigned char address)
{
  unsigned char msb, lsb;

  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(address);
  Wire.endTransmission();

  Wire.requestFrom(BMP085_ADDRESS, 2);
  while(Wire.available()<2)
    ;
  msb = Wire.read();
  lsb = Wire.read();

  return (int) msb<<8 | lsb;
}

// Read the uncompensated temperature value
unsigned int bmp085ReadUT(){
  unsigned int ut;

  // Write 0x2E into Register 0xF4
  // This requests a temperature reading
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(0xF4);
  Wire.write(0x2E);
  Wire.endTransmission();

  // Wait at least 4.5ms
  delay(5);

  // Read two bytes from registers 0xF6 and 0xF7
  ut = bmp085ReadInt(0xF6);
  return ut;
}

// Read the uncompensated pressure value
unsigned long bmp085ReadUP(){

  unsigned char msb, lsb, xlsb;
  unsigned long up = 0;

  // Write 0x34+(OSS<<6) into register 0xF4
  // Request a pressure reading w/ oversampling setting
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(0xF4);
  Wire.write(0x34 + (OSS<<6));
  Wire.endTransmission();

  // Wait for conversion, delay time dependent on OSS
  delay(2 + (3<<OSS));

  // Read register 0xF6 (MSB), 0xF7 (LSB), and 0xF8 (XLSB)
  msb = bmp085Read(0xF6);
  lsb = bmp085Read(0xF7);
  xlsb = bmp085Read(0xF8);

  up = (((unsigned long) msb << 16) | ((unsigned long) lsb << 8) | (unsigned long) xlsb) >> (8-OSS);

  return up;
}

float calcAltitude(float pressure)
{
  float A = pressure/101325;
  float B = 1/5.25588;
  float C = pow(A,B);
  C = 1 - C;
  C = C /0.0000225577;

  return C;
}
