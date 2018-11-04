#include <SevSeg.h>

const long BAUD_RATE = 115200;
enum state { intro, clockSetup, countdown, beeping};
enum state currentState;

//////////////////////////////////////////
/// TURNABLE BUTTON
//////////////////////////////////////////

void initButton() {
  //TODO
}

bool hasButtonTurned() {
  //TODO
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

void initIntro() {
  //TODO
}

void handleIntro() {
  //TODO
}

void initSetup() {
  //TODO
}

void handleSetup() {
  //TODO
}

bool isSetupInactiveForLongEnough() {
  //TODO
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
