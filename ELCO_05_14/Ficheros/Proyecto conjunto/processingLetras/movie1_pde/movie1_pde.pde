import processing.sound.*;
import processing.video.*;
import processing.serial.*;
import ddf.minim.*;
import ddf.minim.ugens.*;


Minim minim;
AudioOutput out;
AudioPlayer player;
Movie myMovie;
Movie myMovie2;
SoundFile file;
SoundFile file2;
SoundFile file3;
SoundFile file4;


char letter;
String words ="";
String letrafinal;
String video;
String cancion;
Serial myPort;  // Create object from Serial class
int val;      // Data received from the serial port
String recibido;
int frecuencia;

/* Starting to receive flag */
int startToReceive;
int pressed;




void setup() {
  print("Processing started. \n\n");
  background(0,0,0);
  size(1200, 800);  
  String portName = Serial.list()[2];
  myPort = new Serial(this, portName, 115200);  
  textFont(createFont("SourceCodePro-Regular.ttf", 36));
  file = new SoundFile(this, "aplausos.mp3");
  file2 = new SoundFile(this, "grillo.mp3");
  file3 = new SoundFile(this, "pistola.mp3");
  file4 = new SoundFile(this, "amigos.mp3");
 
  //sonido o musica
  minim = new Minim(this);
  out = minim.getLineOut();
  
  startToReceive = 0;    
  pressed = 0;  
  
}

void draw() {

  //TEXTOS
  textSize(40);
  text(words, 50, 50, 540, 300);
    
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
      letrafinal=row.getString("letra");
      if(letrafinal != null && letrafinal !="***"){
        words = words + letrafinal;
      }
     
     recibido="000000000";

     if(letrafinal=="***"){
       background(0,0,0);
      if(words.length() > 0 ){
          words = words.substring(0,words.length()-1);
       }
     } else if (letrafinal == "**") {
        background(0,0,0);
        words = "";
     } else if (letrafinal == "¡MUCHAS GRACIAS!") {       
       file.play();       
     }else if (letrafinal == "ESTOESTO") {       
       file4.play();       
     }else if (letrafinal == "GRILLO") {       
       file2.play();     
    }else if (letrafinal == "LASER") {       
       file3.play();     
  }    
 
  }
 
}

}

void keyPressed() {  
  myPort.write(65);         
}

void movieEvent(Movie m) {
  m.read();
}
