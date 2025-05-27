#include "Arduino.h"
#include "LedControl.h"
#include "Delay.h"

const byte MATRIX_A = 1;
const byte MATRIX_B = 0;

const int ACC_LOW = 300;
const int ACC_HIGH = 360;

const byte PIN_DATAIN = 5;
const byte PIN_CLK = 4;
const byte PIN_LOAD = 6;

const byte PIN_X = A1;
const byte PIN_Y = A2;

const byte PIN_A = 2;
const byte PIN_B = 3;
const byte PIN_BP = 8;

const int ROTATION_OFFSET = 90;

const int DELAY_FRAME = 100;

const byte MODE_HOURGLASS = 0;
const byte MODE_SETMINUTES = 1;
const byte MODE_SETHOURS = 2;

volatile unsigned long last_time = -100;

byte delayHours = 0;
byte delayMinutes = 1;
int mode = MODE_HOURGLASS;
int gravity;
LedControl lc = LedControl(PIN_DATAIN, PIN_CLK, PIN_LOAD, 2);
NonBlockDelay d;


long getDelayDrop() {
  return delayMinutes + delayHours * 60;
}



coord getDown(int x, int y) {
  coord xy;
  xy.x = x-1;
  xy.y = y+1;
  return xy;
}
coord getLeft(int x, int y) {
  coord xy;
  xy.x = x-1;
  xy.y = y;
  return xy;
}
coord getRight(int x, int y) {
  coord xy;
  xy.x = x;
  xy.y = y+1;
  return xy;
}



bool canGoLeft(int addr, int x, int y) {
  if (x == 0) return false;
  return !lc.getXY(addr, getLeft(x, y));
}
bool canGoRight(int addr, int x, int y) {
  if (y == 7) return false;
  return !lc.getXY(addr, getRight(x, y));
}
bool canGoDown(int addr, int x, int y) {
  if (y == 7) return false;
  if (x == 0) return false;
  if (!canGoLeft(addr, x, y)) return false;
  if (!canGoRight(addr, x, y)) return false;
  return !lc.getXY(addr, getDown(x, y));
}



void goDown(int addr, int x, int y) {
  lc.setXY(addr, x, y, false);
  lc.setXY(addr, getDown(x,y), true);
}
void goLeft(int addr, int x, int y) {
  lc.setXY(addr, x, y, false);
  lc.setXY(addr, getLeft(x,y), true);
}
void goRight(int addr, int x, int y) {
  lc.setXY(addr, x, y, false);
  lc.setXY(addr, getRight(x,y), true);
}



bool moveParticle(int addr, int x, int y) {
  if (!lc.getXY(addr,x,y)) {
    return false;
  }

  bool can_GoLeft = canGoLeft(addr, x, y);
  bool can_GoRight = canGoRight(addr, x, y);

  if (!can_GoLeft && !can_GoRight) {
    return false;
  }

  bool can_GoDown = canGoDown(addr, x, y);

  if (can_GoDown) {
    goDown(addr, x, y);
  } else if (can_GoLeft&& !can_GoRight) {
    goLeft(addr, x, y);
  } else if (can_GoRight && !can_GoLeft) {
    goRight(addr, x, y);
  } else if (random(2) == 1) {
    goLeft(addr, x, y);
  } else {
    goRight(addr, x, y);
  }
  return true;
}



void fill(int addr, int maxcount) {
  int n = 8;
  byte x,y;
  int count = 0;
  for (byte slice = 0; slice < 2*n-1; ++slice) {
    byte z = slice<n ? 0 : slice-n + 1;
    for (byte j = z; j <= slice-z; ++j) {
      y = 7-j;
      x = (slice-j);
      lc.setXY(addr, x, y, (++count <= maxcount));
    }
  }
}



int getGravity() {
  int x = analogRead(PIN_X);
  int y = analogRead(PIN_Y);
  if (y < ACC_LOW)  {return 0;}
  if (x > ACC_HIGH) {return 90;}
  if (y > ACC_HIGH) {return 180;}
  if (x < ACC_LOW)  {return 270;}
}


int getTopMatrix() {
  return (getGravity() == 90) ? MATRIX_A : MATRIX_B;
}
int getBottomMatrix() {
  return (getGravity() != 90) ? MATRIX_A : MATRIX_B;
}



void resetTime() {
  for (byte i=0; i<2; i++) {
    lc.clearDisplay(i);
  }
  fill(getTopMatrix(), 60);
  d.Delay(getDelayDrop() * 1000);
}



bool updateMatrix() {
  int n = 8;
  bool somethingMoved = false;
  byte x,y;
  bool direction;
  for (byte slice = 0; slice < 2*n-1; ++slice) {
    direction = (random(2) == 1);
    byte z = slice<n ? 0 : slice-n + 1;
    for (byte j = z; j <= slice-z; ++j) {
      y = direction ? (7-j) : (7-(slice-j));
      x = direction ? (slice-j) : j;
      if (moveParticle(MATRIX_B, x, y)) {
        somethingMoved = true;
      };
      if (moveParticle(MATRIX_A, x, y)) {
        somethingMoved = true;
      }
    }
  }
  return somethingMoved;
}



boolean dropParticle() {
  if (d.Timeout()) {
    d.Delay(getDelayDrop() * 1000);
    if (gravity == 0 || gravity == 180) {
      if ((lc.getLed(MATRIX_A, 0, 0) && !lc.getLed(MATRIX_B, 7, 7)) ||
          (!lc.getLed(MATRIX_A, 0, 0) && lc.getLed(MATRIX_B, 7, 7))
      ) {
        lc.invertRawXY(MATRIX_A, 0, 0);
        lc.invertRawXY(MATRIX_B, 7, 7);
        return true;
      }
    }
  }
  return false;
}



void displayLetter(char letter, int matrix) {
  lc.clearDisplay(matrix);
  lc.setXY(matrix, 1,4, true);
  lc.setXY(matrix, 2,3, true);
  lc.setXY(matrix, 3,2, true);
  lc.setXY(matrix, 4,1, true);

  lc.setXY(matrix, 3,6, true);
  lc.setXY(matrix, 4,5, true);
  lc.setXY(matrix, 5,4, true);
  lc.setXY(matrix, 6,3, true);

  if (letter == 'M') {
    lc.setXY(matrix, 4,2, true);
    lc.setXY(matrix, 4,3, true);
    lc.setXY(matrix, 5,3, true);
  }
  if (letter == 'H') {
    lc.setXY(matrix, 3,3, true);
    lc.setXY(matrix, 4,4, true);
  }
}



void renderSetMinutes() {
  fill(getTopMatrix(), delayMinutes);
  displayLetter('M', getBottomMatrix());
}
void renderSetHours() {
  fill(getTopMatrix(), delayHours);
  displayLetter('H', getBottomMatrix());
}


volatile boolean lastEncoderA = LOW;
void update_encoder() {
  boolean encoderA = digitalRead(PIN_A);

  if ((lastEncoderA == HIGH) && (encoderA == LOW)) {
    if (digitalRead(PIN_B) == LOW) {
      reduce_time();
    }
    else {
      add_time();
    }
  }
  lastEncoderA = encoderA;
}

void add_time() {
  if (digitalRead(PIN_BP)==LOW) {return;}
  if (millis()-last_time<=100) {last_time = millis();return;}
  last_time=millis();
  if (mode == MODE_SETHOURS) {
    delayHours = constrain(delayHours+1, 0, 64);
    renderSetHours();
  } else if(mode == MODE_SETMINUTES) {
    delayMinutes = constrain(delayMinutes+1, 0, 64);
    renderSetMinutes();
  }
  Serial.print("Delay: ");
  Serial.println(getDelayDrop());
}

void reduce_time() {  
  if (digitalRead(PIN_BP)==LOW) {return;}
  if (millis()-last_time<=100) {last_time = millis();return;}
  last_time=millis();
  if (mode == MODE_SETHOURS) {
    delayHours = constrain(delayHours-1, 0, 64);
    renderSetHours();
  } else if (mode == MODE_SETMINUTES) {
    delayMinutes = constrain(delayMinutes-1, 0, 64);
    renderSetMinutes();
  }
  Serial.print("Delay: ");
  Serial.println(getDelayDrop());
}



void pciSetup(byte pin) {
  *digitalPinToPCMSK(pin) |= bit (digitalPinToPCMSKbit(pin));
  PCIFR  |= bit (digitalPinToPCICRbit(pin));
  PCICR  |= bit (digitalPinToPCICRbit(pin));
}

ISR (PCINT0_vect) {
  if (digitalRead(PIN_BP)==LOW) {return;}
  if (millis()-last_time<=100) {last_time = millis();return;}
  last_time=millis();
  buttonPush();
 }


void buttonPush() {
  mode = (mode+1) % 3;
  Serial.print("Switched mode to: ");
  Serial.println(mode);

  if (mode == MODE_SETMINUTES) {
    lc.backup();
    renderSetMinutes();
  }
  if (mode == MODE_SETHOURS) {
    renderSetHours();
  }
  if (mode == MODE_HOURGLASS) {
    lc.clearDisplay(0);
    lc.clearDisplay(1);
    lc.restore();
    resetTime();
  }
}



void setup() {
  Serial.begin(9600);

  pinMode(PIN_A, INPUT_PULLUP);
  pinMode(PIN_B, INPUT_PULLUP);
  pinMode(PIN_BP, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_A), update_encoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PIN_B), update_encoder, CHANGE);
  pciSetup(PIN_BP);

  randomSeed(analogRead(A0));

  for (byte i=0; i<2; i++) {
    lc.shutdown(i,false);
    lc.setIntensity(i,0);
  }

  resetTime();
}

void loop() {
  delay(DELAY_FRAME);
  gravity = getGravity();
  lc.setRotation((ROTATION_OFFSET + gravity) % 360);

  if (mode == MODE_SETMINUTES) {
    renderSetMinutes(); return;
  } else if (mode == MODE_SETHOURS) {
    renderSetHours(); return;
  }
  
  updateMatrix();
  dropParticle();
}