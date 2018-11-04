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

int currentPosition;
int oldPosition;

void initButton() {
  currentPosition = getCurrentButtonPosition();
}

bool hasButtonTurned() {
  oldPosition = currentPosition;
  currentPosition = getCurrentButtonPosition();
  return oldPosition != currentPosition;
}

int getCurrentButtonPosition() {
  return meinEncoder.read() / -4;
}

int getButtonPositionBeforeLastTurnCheck() {
  return oldPosition;
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
const long BUTTON_TURN_SECONDS = 30;
byte introPosition;
char helloText[] = "    Hello Grandma";
char currentIntroText[8];

long nextEventMillis;

const int COUNTDOWN_STARTS_AFTER_MILLIS = 2000;

long currentTime;

int buttonSetupInitPosition;
long setupInitTime;

void initIntro() {
  currentTime = 0;
  nextEventMillis = millis();
  introPosition = 0;
}

void handleIntro() {
  if (millis() > nextEventMillis) {
      setCurrentIntroText();
      display.setChars(currentIntroText);
      introPosition++;
      nextEventMillis = millis() + INTRO_DELAY;
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

void displayCurrentTime() {
  display.setNumber(convertSecondToDisplaySecond(currentTime), 2);
}

long convertSecondToDisplaySecond(long t) {
  return t / 60 * 100 + t % 60;
}

long getNewCurrentTime() {
  return (getCurrentButtonPosition() - buttonSetupInitPosition) * BUTTON_TURN_SECONDS + setupInitTime;
}

void initSetup() {
  nextEventMillis = millis() + COUNTDOWN_STARTS_AFTER_MILLIS;
  setupInitTime = currentTime;
  buttonSetupInitPosition = getButtonPositionBeforeLastTurnCheck();
  currentTime = getNewCurrentTime();
  displayCurrentTime();
}

void handleSetup() {
  int currentButtonPosition = getCurrentButtonPosition();
  if (hasButtonTurned()) {
    currentTime = getNewCurrentTime();
    bool isBeyondLimits = false;
    if (currentTime < 0) {
      currentTime = 0;
      isBeyondLimits = true;
    }
    if (currentTime > 5999) {
      currentTime = 5999;
      isBeyondLimits = true;
    }
    if (isBeyondLimits) {
      setupInitTime = currentTime;
      buttonSetupInitPosition = currentButtonPosition;
    }
    displayCurrentTime();
    nextEventMillis = millis() + COUNTDOWN_STARTS_AFTER_MILLIS;
  }
}

bool isSetupInactiveForLongEnough() {
  return millis() > nextEventMillis;
}

void initCountdown() {
  nextEventMillis = millis();
}

void handleCountdown() {
  if (millis() > nextEventMillis) {
    nextEventMillis = millis() + 1000;
    currentTime--;
    if (currentTime < 0) currentTime = 0;
    displayCurrentTime();
  }
}

bool isCountdownCompleted() {
  return currentTime == 0;
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
