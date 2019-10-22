/* ---------- MÚSICA ---------- */

import processing.video.*;
import processing.serial.*;
import ddf.minim.*;
import ddf.minim.ugens.*;

Minim minim;
AudioOutput out;
AudioPlayer player;
Movie myMovie;
Movie myMovie2;


char letter;
String words = "";
String nota;
Serial myPort;  // Create object from Serial class
String recibido;
int frecuencia;


/* Starting to receive flag */
int startToReceive;
int pressed;

void setup() {

  size(1200, 800);
   String portName = Serial.list()[0];
  myPort = new Serial(this, portName, 115200);  
  textFont(createFont("SourceCodePro-Regular.ttf", 36));


  //sonido o musica
  minim = new Minim(this);
  out = minim.getLineOut();
  
  startToReceive = 0;    
  pressed = 0; 
  
}

void draw() {


  //TEXTOS
  textSize(25);
  text(words, 600, 50, 540, 300);
  

  if (startToReceive == 0) {     
     if (myPort.available() != 0) {
       delay(100);                                                /* Necesario para garantizar que se lee el String completo y no se fragmenta en varias partes. */
       recibido = myPort.readString();
       if (recibido.length() == 5) {
         startToReceive = 1;
       } else {
         print(recibido);                  
       }
     }
       
     
  } 
  
  /* When ready to receive data... */
  if (startToReceive == 1) {
    
    /* When serial port has new data... */  
    if ( myPort.available() > 0) {
      delay(20);                                                  /* Necesario para garantizar que la longitud de la palabra recibida es 8. */
      recibido = myPort.readString();                              
      println(recibido);
    }   
  
     for (TableRow row : table.findRows(recibido, "codigo")) {
   
       
      frecuencia= row.getInt("frecuencia");
      nota= row.getString("nota");
      out.playNote(0.0, 0.75, frecuencia );
      words = words + nota + " ";
  
    }
    
    recibido = "00000000";
    
  }
  
}

void keyPressed() {  
  myPort.write(65);         
}


void movieEvent(Movie m) {
  m.read();
}
