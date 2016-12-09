/* vim: set filetype=cpp: */
                       /*
// date created 09 Dec 2016
import processing.serial.*;

Serial myPort;        // The serial port
int xPos = 1;         // horizontal position of the graph
int lastxPos=1;
int lastheight=height;
float inByte = 0;
String inString;

void setup () {
  // set the window size:
  size(768, 300);
  println(height);
  // List all the available serial ports
  // if using Processing 2.1 or later, use Serial.printArray()
  println(Serial.list());

  // I know that the first port in the serial list on my mac
  // is always my  Arduino, so I open Serial.list()[0].
  // Open whatever port is the one you're using.
  myPort = new Serial(this, "/dev/ttyUSB0",115200);

  // don't generatj a serialEvent() unless you get a newline character:
  myPort.bufferUntil('\n');

  // set inital background:
  background(0);
}
void draw () {
  // draw the line:
  //println(inByte);
  //drawing a line from last inByte to the new one
  if (xPos==0) {
      background(0);
  }
  if (xPos>lastxPos){
    stroke(127,34,255);// stroke color
    strokeWeight(4);//stroke width
    line(lastxPos, lastheight, xPos, height - inByte);
    lastxPos=xPos;
    lastheight=int(height-inByte);
  }

}


void serialEvent (Serial myPort) {
  // get the ASCII string:
  inString = myPort.readStringUntil('\n');
  inString=trim(inString); //<>//
  if (inString.equals("EOF")==false) {
      if(inString!= null){
         // convert to an int and map to the screen height:
         inByte = float(inString);

         inByte = map(inByte, -120, 0, 0, height);

         xPos++;
      }

  }
  else {
    xPos=0; //<>//
    lastxPos=0;
  }
}                     */
