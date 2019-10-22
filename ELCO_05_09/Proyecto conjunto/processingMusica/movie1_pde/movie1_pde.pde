
import processing.video.*;
import processing.serial.*;
import ddf.minim.*;
import ddf.minim.ugens.*;

Minim minim;
AudioOutput out;
AudioPlayer player;
Movie myMovie;
Movie myMovie2;

Capture cam;
char letter;
String words = "El gesto que has hecho se corresponde con...";
String letrafinal;
String video;
String cancion;
Serial myPort;  // Create object from Serial class
int val;      // Data received from the serial port
int iteration;
String recibido;
String anterior;
int frecuencia;
int count0;

void setup() {
  
  count0 = 0;
  size(1200, 800);
   String portName = Serial.list()[0];
  myPort = new Serial(this, portName, 115200);  
  textFont(createFont("SourceCodePro-Regular.ttf", 36));

 
 
// println(portName);
  String[] cameras = Capture.list();

  if (cameras == null) {
    println("Failed to retrieve the list of available cameras, will try the default...");
    cam = new Capture(this, 100, 100);
  } if (cameras.length == 0) {
    println("There are no cameras available for capture.");
    exit();
  } else {
    println("Available cameras:");
    printArray(cameras);

    // The camera can be initialized directly using an element
    // from the array returned by list():
    cam = new Capture(this, cameras[0]);
    // Or, the settings can be defined based on the text in the list
    //cam = new Capture(this, 640, 480, "Built-in iSight", 30);
    
    // Start capturing the images from the camera
    cam.start();
  }
  //sonido o musica
  minim = new Minim(this);
  out = minim.getLineOut();
  

  

  
 

  
}

void draw() {
  //CAMARA
  image(cam, 0, 300, width, height);
  
   if (cam.available() == true) {
    cam.read();
  }

  //TEXTOS
  textSize(25);
  text(words, 600, 50, 540, 300);
  

  //PUERTO SERIE  
  if ( myPort.available() > 0) {
    recibido = myPort.readString();

  
  }   
  

  for (TableRow row : table.findRows(recibido, "codigo")) {
  
    // video= row.getString("video");
     //myMovie = new Movie(this, video);
     //myMovie.speed(0.05);
     //myMovie.loop();
    // image(myMovie, 0, 0);
     
    frecuencia= row.getInt("frecuencia");
    out.playNote(0.0, 0.75, frecuencia );
    
     
      


    
     letrafinal=row.getString("nota");
     words = words + letrafinal;
     recibido="00000000";
     
     println(row.getString("video") + " " + row.getString("letra") + " " + row.getInt("frecuencia"));

  }
  



}



void movieEvent(Movie m) {
  m.read();
}
