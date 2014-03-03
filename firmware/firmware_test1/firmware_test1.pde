import processing.serial.*;
Serial serial;
int cur_port = 0;

void setup() {
  printArray(Serial.list());
  // change this to match the Serial port
  serial = new Serial(this, Serial.list()[0], 57600);
  // sequence to enable output
  serial.write('D');
  serial.write('G');
  frameRate(1);
}

void draw() {
  background(0);
  serial.write((1 << cur_port));
  text((cur_port+1), 50, 50);
  cur_port++;
  if (cur_port == 8) {
    cur_port = 0;
  }
}
