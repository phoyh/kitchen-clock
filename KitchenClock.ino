#include <Encoder.h>
#include <SevSeg.h>

const long BAUD_RATE = 115200;
enum state { intro, clockSetup, countdown, beeping};
enum state currentState;

//////////////////////////////////////////
/// TURNABLE BUTTON
//////////////////////////////////////////
const byte CLK = A0;
const byte DT = A1;
Encoder meinEncoder(DT,CLK);

int newPosition;
int oldPosition;
int offset;

void initButton() {
  //TODO
}

bool hasButtonTurned() {
  newPosition = meinEncoder.read() / 4 * 3 + offset;
  if (newPosition != oldPosition) {
    return true;
  }
  return false;
}

int getPosition() {
  Serial.println(meinEncoder.read());
  newPosition = meinEncoder.read() / 4 * 3 + offset;
  if (newPosition < 0) {
    offset += 1;
  }
  oldPosition = newPosition;
  return newPosition;
}

//////////////////////////////////////////
/// DIGIT DISPLAY
//////////////////////////////////////////

const byte DIGITNUMBER = 4;
const byte DIGITPINS[] = {2, 3, 4, 5};
const byte SEGMENTPINS[] = {6, 7, 8, 9, 10, 11, 12, 13};
SevSeg display;

void initDisplay() {
  display.begin(COMMON_CATHODE, DIGITNUMBER, DIGITPINS, SEGMENTPINS);
}

bool hasSetupInactiveForLongEnough() {
  //TODO
}

//////////////////////////////////////////
/// STATE HANDLERS
//////////////////////////////////////////
unsigned long introtimerOffset;
const long INTRO_DELAY = 250;
byte introPosition;
char helloText[] = "    Hello Grandma";
char currentIntroText[8];

long timerOffset;

const int DIPLAYSECONDS_TURN = 2000;

long time;

void initIntro() {
  timerOffset = millis() + INTRO_DELAY;
  introPosition = 0;
}

void handleIntro() {
  if (millis() > (timerOffset + INTRO_DELAY)) {
      setCurrentIntroText();
      display.setChars(currentIntroText);
      introPosition++;
      timerOffset = millis();
  }
}

void setCurrentIntroText() {
  if (introPosition + 4 > strlen(helloText)) {
    return;
  }
  for (byte i = 0; i < 4; i++) {
    currentIntroText[i] = helloText[i + introPosition];
  }
}

void initSetup() {
  timerOffset = millis();
}

void handleSetup() {
  if (hasButtonTurned()) {
    time = getPosition() * 10;//DIPLAYSECONDS_TURN * 10;
    time = convertSecondToDisplaySecond(time);
    display.setNumber(time, 2);
  }
}

long convertSecondToDisplaySecond(long t) {
  return t / 60 * 100 + t % 60;
}

bool isSetupInactiveForLongEnough() {
  return false;
}

void handleCountdown() {
  //TODO
}

void initCountdown() {
  //TODO
}

bool isCountdownCompleted() {
  //TODO
}

void initBeeping() {
  //TODO
}

void handleBeeping() {
  //TODO
}

//////////////////////////////////////////
/// MAIN FUNCTIONS
//////////////////////////////////////////

void setup() {
  Serial.begin(BAUD_RATE);
  initDisplay();
  initButton();
  currentState = intro;
  initIntro();
}

void loop() {
  switch (currentState) {
    case intro:
      handleIntro();
      if (hasButtonTurned()) {
        currentState = clockSetup;
        initSetup();
      }
      break;
    case clockSetup:
      handleSetup();
      if (isSetupInactiveForLongEnough()) {
        currentState = countdown;
        initCountdown();
      }
      break;
    case countdown:
      handleCountdown();
      if (hasButtonTurned()) {
        currentState = clockSetup;
        initSetup();
      } else if (isCountdownCompleted()) {
        currentState = beeping;
        initBeeping();
      }
      break;
    case beeping:
      handleBeeping();
      if (hasButtonTurned()) {
        currentState = clockSetup;
        initSetup();
      }
      break;
  }
  display.refreshDisplay();
}
