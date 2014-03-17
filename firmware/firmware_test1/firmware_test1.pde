import processing.serial.*;
Serial serial;
int cur_port = 0;
int state = 0;

void setup() {
  size(500, 500);
}

void draw() {
  background(0);

  if (state == 0) {
    // select serial port
    String[] serials = Serial.list();
    int sel = floor((mouseY-14)/20.0);
    for (int i=0; i < serials.length; i++) {
      if (sel == i) {
        fill(255, 0, 0);
      } else {
        fill(255, 255, 255);
      }
      text(serials[i], 20, 20+i*20);
      if (mousePressed) {
        // selected, open port
        serial = new Serial(this, serials[i], 57600);
        // sequence to enable output
        serial.write('D');
        serial.write('G');
        frameRate(1);
        state = 1;
      }
    }
  } else {
    // activate motor at a time
    serial.write((1 << cur_port));
    text((cur_port+1), width/2, height/2);
    cur_port++;
    if (cur_port == 8) {
      cur_port = 0;
    }
  }
}
