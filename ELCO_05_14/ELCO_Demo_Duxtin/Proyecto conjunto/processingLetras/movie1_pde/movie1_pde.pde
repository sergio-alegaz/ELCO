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

/* Sound variables */
SoundFile file, file1, file2, file3, file4, file5;

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
  String portName = Serial.list()[0];
  myPort = new Serial(this, portName, 115200);  
  textFont(createFont("SourceCodePro-Regular.ttf", 36));
  
  file = new SoundFile(this, "aplausos.mp3");
  file1 = new SoundFile(this, "amigos.mp3");
  file2 = new SoundFile(this, "gun.mp3");
  file3 = new SoundFile(this, "crickets.mp3");
  file4 = new SoundFile(this, "haha.mp3");
  file5 = new SoundFile(this, "cocking.mp3");
 
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
      /* video play */
      //video=row.getString("video");
      //myMovie = new Movie(this, video);      
      
      if(letrafinal != null && letrafinal !="***"){
        words = words + letrafinal;
        //image(myMovie,0,0);
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
     } else if (letrafinal == "¡FIN!") {       
       file1.play();       
     } else if (letrafinal == "SHOT! SHOT! SHOT!") {       
       file2.play();       
     } else if (letrafinal == "CRI... CRI... CRI...") {       
       file3.play();       
     } else if (letrafinal == "F*CK YOU!") {       
       file4.play();       
     } else if (letrafinal == "Reloading...") {       
       file5.play();       
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
