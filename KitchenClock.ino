#include <Encoder.h>
#include <SevSeg.h>
#include <pitches.h>

const long BAUD_RATE = 115200;
const byte SOUNDPIN = A2;
enum state { intro, clockSetup, countdown, beeping};
enum state currentState;

//////////////////////////////////////////
////TURNABLE BUTTON///////////////////////
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
////DIGIT DISPLAY/////////////////////////
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
////STATE HANDLERS////////////////////////
//////////////////////////////////////////
const long INTRO_DELAY = 250;
const long BUTTON_TURN_SECONDS = 60;
byte introPosition;
char helloText[] = "    Hello Grandma";
char currentIntroText[8];

long nextTimerMillis[3];

const long COUNTDOWN_STARTS_AFTER_MILLIS = 2000;
const long RESENSIBILIZE_AFTER_MILLIS = 500;

long currentTime;

int buttonSetupInitPosition;
long setupInitTime;

const int TONE_DURATION = 100;
int melody[] = { NOTE_C5, NOTE_E5, NOTE_C5, NOTE_G4 };
byte nextPlayedNoteIndex;

void setTimer(byte timerId, long millisUntilEvent) {
  nextTimerMillis[timerId] = millis() + millisUntilEvent;
}
bool isTimerReady(byte timerId) {
  return nextTimerMillis[timerId] < millis();
}
void turnOffTimer(byte timerId) {
  setTimer(timerId, 10000000);
}

void initIntro() {
  currentTime = 0;
  setTimer(0, 0);
  introPosition = 0;
}

void handleIntro() {
  if (isTimerReady(0)) {
      setCurrentIntroText();
      display.setChars(currentIntroText);
      introPosition++;
      setTimer(0, INTRO_DELAY);
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
  long turnUnits = getCurrentButtonPosition() - buttonSetupInitPosition;
  long turnSign;
  if (turnUnits > 0) {
    turnSign = 1;
  } else {
    turnSign = -1;
  }
  long unsignedTurnUnits = turnSign * turnUnits;
  long unsignedTimeUnits = unsignedTurnUnits * (unsignedTurnUnits + 1) / 2;
  return turnSign * unsignedTimeUnits * BUTTON_TURN_SECONDS + setupInitTime;
}

void initSetup() {
  setTimer(0, COUNTDOWN_STARTS_AFTER_MILLIS);
  setTimer(1, RESENSIBILIZE_AFTER_MILLIS);
  setupInitTime = currentTime;
  buttonSetupInitPosition = getButtonPositionBeforeLastTurnCheck();
  currentTime = getNewCurrentTime();
  displayCurrentTime();
}

void handleSetup() {
  int currentButtonPosition = getCurrentButtonPosition();
  bool isSetupInitRequired = false;
  if (hasButtonTurned()) {
    currentTime = getNewCurrentTime();
    if (currentTime < 0) {
      currentTime = 0;
      isSetupInitRequired = true;
    }
    if (currentTime > 5999) {
      currentTime = 5999;
      isSetupInitRequired = true;
    }
    displayCurrentTime();
    setTimer(0, COUNTDOWN_STARTS_AFTER_MILLIS);
    setTimer(1, RESENSIBILIZE_AFTER_MILLIS);
  }
  if (isTimerReady(1)) {
    turnOffTimer(1);
    isSetupInitRequired = true;
  }
  if (isSetupInitRequired) {
    setupInitTime = currentTime;
    buttonSetupInitPosition = currentButtonPosition;
  }
}

bool isSetupInactiveForLongEnough() {
  return isTimerReady(0);
}

void initCountdown() {
  setTimer(0, 0);
}

void handleCountdown() {
  if (isTimerReady(0)) {
    setTimer(0, 1000);
    currentTime--;
    if (currentTime < 0) currentTime = 0;
    displayCurrentTime();
  }
}

bool isCountdownCompleted() {
  return currentTime == 0;
}

void initBeeping() {
  setTimer(2, TONE_DURATION);
  nextPlayedNoteIndex = 0;
}

void handleBeeping() {
  if (isTimerReady(2)) {
    tone(SOUNDPIN, melody[nextPlayedNoteIndex++], TONE_DURATION);
    setTimer(2, TONE_DURATION + 10);
  }
  if (sizeof(melody) <= nextPlayedNoteIndex * 2) {
    nextPlayedNoteIndex = 0;
    setTimer(2, TONE_DURATION * 4);
  }
}

//////////////////////////////////////////
////MAIN FUNCTIONS////////////////////////
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
