// BEGIN BMP180_WTV020\BMP180_WTV020.ino
// Pro Mini atmega328 3.3V 8M: http://www.ebay.com/itm/200914924969?ssPageName=STRK:MEWNX:IT&_trksid=p3984.m1439.l2649
//
// BMP180 Digital Barometric Pressure Sensor:  http://www.ebay.com/itm/200915895472?ssPageName=STRK:MEWNX:IT&_trksid=p3984.m1439.l2649
// <BMP180.h>: http://www.loveelectronics.co.uk/Tutorials/20/bmp180-arduino-library-tutorial
// backup#1 bmp085 http://www.ebay.com/sch/i.html?_nkw=bmp085
// backup#1 library: https://github.com/adafruit/Adafruit_BMP085_Unified
// backup#2 MS5611-01BA03 http://www.ebay.com/sch/i.html?_nkw=MS5611-01BA03

// U-disk audio player SD card Sound module WTV020-SD-16P http://www.ebay.com/itm/310629013078?ssPageName=STRK:MEWNX:IT&_trksid=p3984.m1439.l2649
// arduino library, wiring & pinout: http://forum.arduino.cc/index.php?topic=117009.0
// add to Wtv020sd16p.h:
//    void setVolume1to7(int voiceLevel);
// add to Wtv020sd16p.cpp:
//    void Wtv020sd16p::setVolume1to7(int voiceLevel){
//      int encoded = VOLUME_MIN + voiceLevel;
//       sendCommand(encoded);
//    }
// wav files synthesized using http://www2.research.att.com/~ttsweb/tts/demo.php
// I used UsbRecorder to convert wav=>ad4 but the file with number "50" sounded louder than other numbers;
// now please check the comment inside getFileDuration() below;
// other resources on Wtv020sd16p voice module:
// MP3 mode circuit- WTV020SD-16P Tutorial http://www.buildcircuit.com/mp3-mode-circuit-wtv020sd-16p-tutorial/
// How to use WTV020SD music module with Arduino http://www.buildcircuit.com/how-to-use-wtv020sd-music-module-with-arduino/
// Wtv020-SD-16P http://matthew-morris.com/wiki/index.php?title=Wtv020-SD-16P
// WTV020-SD-16P - Help & solution tree http://forum.arduino.cc/index.php?PHPSESSID=dk0kb5qk818c9dqp57p7csq5e2&topic=115411.0
// English PDF: http://www.elechouse.com/elechouse/images/product/MP3%20Sound%20Mini%20SD%20Card%20Module/MP3%20Mini%20SD%20Card%20Module.pdf
// I used 128Mb, 256Mb and 512Mb TF cards (at arduino's 3.3V from VCC pin) from this seller: http://www.ebay.com/usr/liayufen http://stores.ebay.com/Cardword/_i.html?_nkw=micro+sd&submit=Search&_sid=1000775052

// voice module alternative (untested) (alice couldn't say how much memory is onboard, but my 176 ad4 files total up to 2Mb):
// High-quality WT588D-16p voice module Sound modue audio player for Arduino http://www.ebay.com/itm/400463493756?ssPageName=STRK:MEWNX:IT&_trksid=p3984.m1439.l2649

// speaker: http://www.ebay.com/sch/i.html?_nkw=8+ohm+0.5+w+40+mm+speaker
// alternative speaker (untested): 1pcs Round Micro Speaker Diameter 23mm 8Ohm 8R 1W SPEAKERS (261318155089) http://www.ebay.com/itm/261318155089

// FTDI USB to TTL Serial Adapter Module for Arduino Mini 3.3V 5.5V FTDI232: http://www.ebay.com/itm/111221076351?ssPageName=STRK:MEWNX:IT&_trksid=p3984.m1439.l2649
// wiring: http://www.youtube.com/watch?v=Vawhrr4COjI

// box: http://www.ebay.com/sch/i.html?_nkw=aa+aaa+plastic+box
// potential amplifier (untested): 20 Times gain 5V-12V LM386 Audio Amplifier Module with 10K Adjustable Resistance http://www.ebay.com/itm/181008031850?ssPageName=STRK:MEWNX:IT&_trksid=p3984.m1439.l2649
// potential in-flight mode switcher: Voice Recognition Module -Arduino Compatible http://www.ebay.com/itm/280793964171?ssPageName=STRK:MEWNX:IT&_trksid=p3984.m1439.l2649

#include <Wire.h>
#include <EEPROM.h>
#include <BMP180.h>
#include <Wtv020sd16p.h>
#include <Switch.h>   // http://www.avdweb.nl/arduino/hardware-interfacing/simple-switch-debouncer.html

// uncomment those and watch your Serial Monitor (Ctrl-Shift-M in Arduino IDE)
// when all uncommented:
//    Sketch uses 20,894 bytes (68%) of program storage space. Maximum is 30,720 bytes.
//    Global variables use 2,154 bytes (105%) of dynamic memory, leaving -106 bytes for local variables. Maximum is 2,048 bytes.
// when all commented out:
//    Sketch uses 15,756 bytes (51%) of program storage space. Maximum is 30,720 bytes.
//    Global variables use 524 bytes (25%) of dynamic memory, leaving 1,524 bytes for local variables. Maximum is 2,048 bytes.
//#define DEBUG_SETUP 1
//#define DEBUG_OPERATIONS 1
//#define DEBUG_VOICE 1
//#define DEBUG_VOICE_SELFTEST 1
//#define DEBUG_EEPROM 1
//#define DEBUG_DEBOUNCE 1

#ifdef DEBUG_SETUP
#define DEBUG_SETUP_PRINT(x)    Serial.print (x)
#define DEBUG_SETUP_PRINTDEC(x) Serial.print (x, DEC)
#define DEBUG_SETUP_PRINTLN(x)  Serial.println (x)
#else
#define DEBUG_SETUP_PRINT(x)
#define DEBUG_SETUP_PRINTDEC(x)
#define DEBUG_SETUP_PRINTLN(x)
#endif

#ifdef DEBUG_OPERATIONS
#define DEBUG_OPERATIONS_PRINT(x)    Serial.print (x)
#define DEBUG_OPERATIONS_PRINTDEC(x) Serial.print (x, DEC)
#define DEBUG_OPERATIONS_PRINTLN(x)  Serial.println (x)
#else
#define DEBUG_OPERATIONS_PRINT(x)
#define DEBUG_OPERATIONS_PRINTDEC(x)
#define DEBUG_OPERATIONS_PRINTLN(x)
#endif

#ifdef DEBUG_VOICE
#define DEBUG_VOICE_PRINT(x)    Serial.print (x)
#define DEBUG_VOICE_PRINTDEC(x) Serial.print (x, DEC)
#define DEBUG_VOICE_PRINTLN(x)  Serial.println (x)
#else
#define DEBUG_VOICE_PRINT(x)
#define DEBUG_VOICE_PRINTDEC(x)
#define DEBUG_VOICE_PRINTLN(x)
#endif

#ifdef DEBUG_EEPROM
#define DEBUG_EEPROM_PRINT(x)    Serial.print (x)
#define DEBUG_EEPROM_PRINTDEC(x) Serial.print (x, DEC)
#define DEBUG_EEPROM_PRINTLN(x)  Serial.println (x)
#else
#define DEBUG_EEPROM_PRINT(x)
#define DEBUG_EEPROM_PRINTDEC(x)
#define DEBUG_EEPROM_PRINTLN(x)
#endif

#ifdef DEBUG_DEBOUNCE
#define DEBUG_DEBOUNCE_PRINT(x)    Serial.print (x)
#define DEBUG_DEBOUNCE_PRINTDEC(x) Serial.print (x, DEC)
#define DEBUG_DEBOUNCE_PRINTLN(x)  Serial.println (x)
#else
#define DEBUG_DEBOUNCE_PRINT(x)
#define DEBUG_DEBOUNCE_PRINTDEC(x)
#define DEBUG_DEBOUNCE_PRINTLN(x)
#endif

int pinVoiceReset = 11;  // The pin number of the reset pin.
int pinVoiceClock = 10;  // The pin number of the clock pin.
int pinVoiceData = 9;  // The pin number of the data pin.
int pinVoiceBusy = 8;  // The pin number of the busy pin.

Wtv020sd16p wtv020sd16p(pinVoiceReset, pinVoiceClock, pinVoiceData, pinVoiceBusy);

int pinLed = 13;
int requestAltitudeGround = 2;
byte requestAltitudePin = 4;

int longPushDelay = 600;
Switch button = Switch(requestAltitudePin, INPUT_PULLUP, LOW, 50, longPushDelay); // debounceTime 50ms
//Switch button = Switch(requestAltitudePin);

BMP180 barometer;
float seaLevelPressure = 101325;

int	temperature = 0;
int	altitudeAboveMeanSeaLevel = 0;
int	altitudeAboveGroundLevel = 0;

int	prevAltitudeAGL = 0;
long	prevAltitudeAGLTimestamp = 0;

int	prevTemperature = 0;
long	prevTemperatureTimestamp = 0;

int	altitudeAboveMeanSeaLevelAtTakeoff = 0;
int	altitudeAboveMeanSeaLevelAtTakeoffEepromOffset = 0;

int	modeTalk = 1;		// 0=pushToSay, 1=2m sensitivity, 2=5m sensitivity, 3=10m sensitivity, 4=10 seconds delay
int	modeTalkEepromOffset = 4;

void setup() {
  Serial.begin(9600);
  pinMode(pinLed, OUTPUT);
  initButton();
  initSpeaker();
  initBarometer();
  doubleBlink();
}

boolean barometerReportedAtLeastOnce = false;
void initBarometer() {
  Wire.begin();
  barometer = BMP180();
  barometer.SetResolution(BMP180_Mode_UltraHighResolution, false);
  if (barometer.EnsureConnected() == false) return;	// connection failure handled/reconnected in loop()
  DEBUG_SETUP_PRINTLN("Connected to BMP180");

  barometer.SoftReset();
  barometer.Initialize();

  //altitudeAboveMeanSeaLevel = barometer.GetAltitude(seaLevelPressure);
  altitudeAboveMeanSeaLevel = getAltitudeAverage();
  temperature = barometer.GetTemperature();

  if (barometerReportedAtLeastOnce == true) return;
  
  seaLevelReadEeprom();

  //if (altitudeAboveMeanSeaLevelAtTakeoff == 0 && altitudeAboveMeanSeaLevel > 0) {
  //"if you don't have isnan(), you can compare it to itself with the == operator - if(x==x) - only a NaN will fail this test."
  if (isnan(altitudeAboveMeanSeaLevelAtTakeoff)) {
    altitudeAboveMeanSeaLevelAtTakeoff = altitudeAboveMeanSeaLevel;
    seaLevelWriteEeprom();		// first time ever turned on - no questions about resetting to zero
#ifdef DEBUG_EEPROM
    seaLevelReadEeprom();
#endif
  }
}
void initSpeaker() {
  wtv020sd16p.reset();
  //wtv020sd16p.unmute();
  //delay(100);
  // at level=7 letter "s" drops too much voltage and Adruino resets itself in a loop
  wtv020sd16p.setVolume1to7(6);
  
  modeTalkReadEeprom();
  if (modeTalk == -1) {
    modeTalk = 1;
    modeTalkWriteEeprom();		// first time ever turned on - no questions about resetting to zero
#ifdef DEBUG_EEPROM
    modeTalkReadEeprom();
#endif
  }

#ifdef DEBUG_VOICE_SELFTEST
  DEBUG_VOICE_PRINTLN();
  DEBUG_VOICE_PRINTLN();
  saySelftestOkay();
  delay(1000);
  sayNumber(1111);
  //delay(5000);
  sayNumber(-1111);
  //delay(5000);
  sayNumber(-1111.11);
  //delay(5000);
  sayNumber(22);
  //delay(5000);
  sayNumber(2222);
  //delay(5000);
  sayNumber(-2222);
  //delay(5000);
  sayNumber(-2222.22);
  //delay(5000);
  sayNumber(672.92f);
  //delay(5000);
  sayNumber(70);
  sayNumber(70.17);
  //delay(5000);
  sayNumber(-500);
  //delay(5000);
  sayNumber(3000);
  //delay(5000);
  sayNumber(124002);
#endif
}

void initButton() {
  //button = Switch(requestAltitudePin, INPUT_PULLUP, LOW, 50); // debounceTime 50ms
  pinMode(requestAltitudeGround, OUTPUT);
  digitalWrite(requestAltitudeGround, LOW);
  digitalWrite(pinLed, LOW);
}

void sayCurrentAGLThresholds(int thresholdToPreviousAGL = 5, int delayToPreviousAGL = 0) {
  DEBUG_OPERATIONS_PRINTLN();
  DEBUG_OPERATIONS_PRINTLN();
  DEBUG_OPERATIONS_PRINT("sayCurrentAGLThresholds(thresholdToPreviousAGL=");
  DEBUG_OPERATIONS_PRINTDEC(thresholdToPreviousAGL);
  DEBUG_OPERATIONS_PRINT(", delayToPreviousAGL=");
  DEBUG_OPERATIONS_PRINTDEC(delayToPreviousAGL);
  DEBUG_OPERATIONS_PRINTLN("): ");
  
  long timeCurrent = millis();
  if (prevAltitudeAGLTimestamp == 0) prevAltitudeAGLTimestamp = timeCurrent - delayToPreviousAGL - 100;
  if (delayToPreviousAGL > 0) {
    long timePassed = timeCurrent - prevAltitudeAGLTimestamp;
    DEBUG_OPERATIONS_PRINT(" prevAltitudeAGLTimestamp=");
    DEBUG_OPERATIONS_PRINT(prevAltitudeAGLTimestamp);
    DEBUG_OPERATIONS_PRINT(" timePassed=");
    DEBUG_OPERATIONS_PRINT(timePassed);
    if (timePassed < delayToPreviousAGL) {
      long timeLeft = delayToPreviousAGL - timePassed;
      DEBUG_OPERATIONS_PRINT(" timeLeft=");
      DEBUG_OPERATIONS_PRINTLN(timeLeft);
      //delay(timeLeft);
      return;
    }
  }
  prevAltitudeAGLTimestamp = timeCurrent;

  //altitudeAboveMeanSeaLevel = barometer.GetAltitude(seaLevelPressure);
  altitudeAboveMeanSeaLevel = getAltitudeAverage();
  altitudeAboveGroundLevel = altitudeAboveMeanSeaLevel - altitudeAboveMeanSeaLevelAtTakeoff;
  //DEBUG_OPERATIONS_PRINT(" altitudeAboveMeanSeaLevel=");
  //DEBUG_OPERATIONS_PRINT(altitudeAboveMeanSeaLevel);
  DEBUG_OPERATIONS_PRINT(" altitudeAboveGroundLevel=");
  DEBUG_OPERATIONS_PRINT(altitudeAboveGroundLevel);

  if (prevAltitudeAGL == 0) prevAltitudeAGL = altitudeAboveGroundLevel;
  float difference = altitudeAboveGroundLevel - prevAltitudeAGL;
  //DEBUG_OPERATIONS_PRINT(" altitudeAboveGroundLevel=");
  //DEBUG_OPERATIONS_PRINT(altitudeAboveGroundLevel);
  //DEBUG_OPERATIONS_PRINT(" prevAltitudeAGL=");
  //DEBUG_OPERATIONS_PRINT(prevAltitudeAGL);
  //delay(1000);
  if (abs(difference) < thresholdToPreviousAGL) {
    DEBUG_OPERATIONS_PRINT(" abs(difference=[");
    DEBUG_OPERATIONS_PRINTDEC(difference);
    DEBUG_OPERATIONS_PRINT("]) < thresholdToPreviousAGL[");
    DEBUG_OPERATIONS_PRINTDEC(thresholdToPreviousAGL);
    DEBUG_OPERATIONS_PRINTLN("]; skipping to next cycle");
    return;
  }
  prevAltitudeAGL = altitudeAboveGroundLevel;

  sayNumber(altitudeAboveGroundLevel);
  sayTemperatureIfDifferentThanPrevious();
}


boolean firstLoop = true;
bool    canSkipHundreds = false;
boolean chanceToResetAGL = false;
long 	chanceToResetAGLTimestamp = 0;
long 	chanceToResetAGLDurationMillis = 4000;
boolean waitForBarometer = true;

void loop() {
  canSkipHundreds = true;
  if (firstLoop) {
    firstLoop = false;
    sayFirstLoopSeaLevelTemperature();

    // yesterday(70.4) != today(70.9)
    if (altitudeAboveMeanSeaLevelAtTakeoff != altitudeAboveMeanSeaLevel) {
      sayCurrentAGLThresholds(0);
      chanceToResetAGL = true;
      //sayInstructionsToReset();
      //sayCountdownAsync();
      sayInstructionsToResetAsync();
      chanceToResetAGLTimestamp = millis();
    }
    return;
  }

  if (chanceToResetAGL) {
    if (millis() - chanceToResetAGLDurationMillis > chanceToResetAGLTimestamp) {
      chanceToResetAGL = false;
      return;
    }
    handleResetAGL();
    return;
  }

  if (barometerReportedAtLeastOnce == false) {
    sayCurrentTalkingMode();
    sayCurrentAGLThresholds(0);
  }

  barometerReportedAtLeastOnce = true;
  switch (modeTalk) {
//    case 0:  flightOperationPushToSay(); break;
    case 0:  break;
    case 1:  sayCurrentAGLThresholds(2, 200); break;
    case 2:  sayCurrentAGLThresholds(5, 200); break;
    case 3:  sayCurrentAGLThresholds(10, 200); break;
    case 4:  sayCurrentAGLThresholds(0, 10000); break;
    default: sayUnknownTalkingMode(); delay(1000);
  }
//  if (modeTalk == 0) return;
//  button.poll();
//  if (button.pushed()) rotateModeNext();
  flightOperationPushToSay();
}

void flightOperationPushToSay() {
  button.poll();
  if (button.pushed() == false) return;
  long timestampLongPush = millis() + longPushDelay;
  //for (int i=0; i<longPushDelay; i+=50) {
  while (millis() < timestampLongPush) {
    button.poll();
    if (button.released()) {
      DEBUG_DEBOUNCE_PRINT("pushedShorterThanLongPress(): ");
      //DEBUG_DEBOUNCE_PRINTDEC(i);
      DEBUG_DEBOUNCE_PRINTLN();
      sayCurrentAGLThresholds(0, 200);
      return;
    }
    delay(50);
  }
  DEBUG_DEBOUNCE_PRINT("longPress(): ");
  DEBUG_DEBOUNCE_PRINTDEC(button.longPress());
  DEBUG_DEBOUNCE_PRINTLN();
  rotateModeNext();
}

void rotateModeNext() {
  switch (modeTalk) {
    case 0:  modeTalk = 1; break;
    case 1:  modeTalk = 2; break;
    case 2:  modeTalk = 3; break;
    case 3:  modeTalk = 4; break;  //4
    case 4:  modeTalk = 0; break;
    default: sayUnknownTalkingMode();
  }
  modeTalkWriteEeprom();
#ifdef DEBUG_EEPROM
  modeTalkReadEeprom();
#endif
  sayCurrentTalkingMode();
}

void sayCurrentTalkingMode() {
  switch (modeTalk) {
    case 0:  sayPushToSay();            break;
    case 1:  saySensitivity2meters();   break;
    case 2:  saySensitivity5meters();   break;
    case 3:  saySensitivity10meters();  break;
    case 4:  sayDelay10sec();           break;
    default: sayUnknownTalkingMode();
  }
}

void sayPushToSay() {
  DEBUG_VOICE_PRINT("sayPushToSay(): ");
  sayAsyncWait(10);
  DEBUG_VOICE_PRINTLN();
}
void saySensitivity2meters() {
  DEBUG_VOICE_PRINT("saySensitivity2meters(): ");
  sayAsyncWait(11);
  DEBUG_VOICE_PRINTLN();
}
void saySensitivity3meters() {
  DEBUG_VOICE_PRINT("saySensitivity3meters(): ");
  sayAsyncWait(15);
  DEBUG_VOICE_PRINTLN();
}
void saySensitivity5meters() {
  DEBUG_VOICE_PRINT("saySensitivity5meters(): ");
  sayAsyncWait(12);
  DEBUG_VOICE_PRINTLN();
}
void saySensitivity10meters() {
  DEBUG_VOICE_PRINT("saySensitivity10meters(): ");
  sayAsyncWait(13);
  DEBUG_VOICE_PRINTLN();
}
void sayDelay10sec() {
  DEBUG_VOICE_PRINT("sayDelay10sec(): ");
  sayAsyncWait(14);
  DEBUG_VOICE_PRINTLN();
}

void sayFirstLoopSeaLevelTemperature() {
  //altitudeAboveMeanSeaLevel = barometer.GetAltitude(seaLevelPressure);
  altitudeAboveMeanSeaLevel = getAltitudeAverage();
  DEBUG_OPERATIONS_PRINT("altitudeAboveMeanSeaLevel: ");
  DEBUG_OPERATIONS_PRINT(altitudeAboveMeanSeaLevel);
  sayNumber(altitudeAboveMeanSeaLevel);
  sayMetersAboveSeaLevel();

  temperature = barometer.GetTemperature();
  DEBUG_OPERATIONS_PRINT("\tTemperature: ");
  DEBUG_OPERATIONS_PRINT(temperature);
  DEBUG_OPERATIONS_PRINTLN("C");
  prevTemperature = temperature;
  sayNumber(temperature);
  sayDegreesCelcium();
}

void sayTemperatureIfDifferentThanPrevious() {
  temperature = barometer.GetTemperature();
  temperature = round(temperature);
  if (prevTemperature == 0) prevTemperature = temperature;
  if (temperature != prevTemperature) {
    prevTemperature = temperature;
    prevTemperatureTimestamp = millis();
    sayNumber(temperature);
    sayDegreesCelcium();
  }
}

void handleResetAGL() {
  button.poll();
  if (button.pushed() == false) return;
  altitudeAboveMeanSeaLevelAtTakeoff = altitudeAboveMeanSeaLevel;
  seaLevelWriteEeprom();
#ifdef DEBUG_EEPROM
  seaLevelReadEeprom();	// check if next startup we will read the same HAMSL as AGL=0
#endif
  //sayAltitudeHasBeenResetToZero();
  //delay(1000);	// delay before flightMode, avoiding "has been reset reset to zero <this1secDelay> zero" confuse
  chanceToResetAGL = false;
}

void sayInstructionsToReset() {
  DEBUG_VOICE_PRINT("sayInstructionsToReset(): ");
  sayAsyncWait(50);
  DEBUG_VOICE_PRINTLN();
}

void sayInstructionsToResetAsync() {
  DEBUG_VOICE_PRINT("sayInstructionsToResetAsync(): ");
  sayAsync(50);
  DEBUG_VOICE_PRINTLN();
}

void sayCountdownAsync() {
  DEBUG_VOICE_PRINT("sayCountdownAsync(): ");
  sayAsync(51);
  DEBUG_VOICE_PRINTLN();
}

void sayAltitudeHasBeenResetToZero() {
  DEBUG_VOICE_PRINT("sayAltitudeHasBeenResetToZero(): ");
  sayAsyncWait(52);
  DEBUG_VOICE_PRINTLN();
}

void saySelftestOkay() {
  DEBUG_VOICE_PRINT("saySelftestOkay(): ");
  sayAsyncWait(70);
  DEBUG_VOICE_PRINTLN();
}

void sayUnknownTalkingMode() {
  DEBUG_VOICE_PRINT("sayUnknownTalkingMode(): ");
  sayAsyncWait(85);
  DEBUG_VOICE_PRINTLN();
}

#ifdef DEBUG_SETUP
#ifndef DEBUG_OPERATIONS
int currentColumn = 0;
#endif
#endif

int getAltitudeSingle() {
  boolean barometerGotDisconnected = false;
  while (barometer.EnsureConnected() == false) {
    DEBUG_SETUP_PRINTLN("Barometer not found, delay 50ms");
    //sayBarometerFailure();
    sayBarometerNotResponding();
    delay(50);
    barometerGotDisconnected = true;
    //initBarometer();
    barometer.SoftReset();
    barometer.Initialize();
    //firstLoop = true;
    //canSkipHundreds = false;
    //return;
  }
  if (barometerGotDisconnected) {
    DEBUG_SETUP_PRINTLN("RE-Connected to BMP180");
    sayBarometerConnected();
    barometerGotDisconnected = false;
  }

  int ret = barometer.GetAltitude(seaLevelPressure);
  //ret = round(ret);
  DEBUG_OPERATIONS_PRINT("getAltitudeAverage(): ");
  DEBUG_OPERATIONS_PRINTDEC(ret);
  DEBUG_OPERATIONS_PRINTLN();
  
  #ifdef DEBUG_SETUP
  #ifndef DEBUG_OPERATIONS
  DEBUG_SETUP_PRINT(".");
  if (currentColumn++ > 80) {
    currentColumn = 0;
    DEBUG_SETUP_PRINTLN();
  }
  #endif
  #endif
  
  return ret;
}

float getAltitudeAverage() {
  float ret1 = getAltitudeSingle();
  return ret1;
  delay(200);
  float ret2 = getAltitudeSingle();
  delay(200);
  float ret3 = getAltitudeSingle();
  ret1 = ret1 * 100 / 100;
  ret2 = ret2 * 100 / 100;
  ret3 = ret3 * 100 / 100;
  if (ret1 <= 0 || ret2 <= 0 || ret3 <= 0) sayBarometerNegativeAltitude();
  float ret = (ret1 + ret2 + ret3) / 3;
  DEBUG_OPERATIONS_PRINTLN();
  DEBUG_OPERATIONS_PRINT("getAltitudeAverage(): ");
  DEBUG_OPERATIONS_PRINTDEC(ret);
  DEBUG_OPERATIONS_PRINT(" (");
  DEBUG_OPERATIONS_PRINTDEC(ret1);
  DEBUG_OPERATIONS_PRINT(", ");
  DEBUG_OPERATIONS_PRINTDEC(ret2);
  DEBUG_OPERATIONS_PRINT(", ");
  DEBUG_OPERATIONS_PRINTDEC(ret3);
  DEBUG_OPERATIONS_PRINTLN(")");
  return ret;
}

void sayNumber(float number) {
  float absNumber = abs(number);
  int hundreds = ((int)(absNumber / 100)) * 100;
  int tens = round(absNumber - hundreds);
  int decimals = (absNumber - (float) hundreds - (float)tens) * 100;

  DEBUG_VOICE_PRINTLN();
  DEBUG_VOICE_PRINT("sayNumber(");
  DEBUG_VOICE_PRINTDEC(number);
  DEBUG_VOICE_PRINTLN("): ");

  if (number < 0) sayMinus();

  bool skipHundreds = shouldSkipHundreds(hundreds, tens);
  if (skipHundreds) {
    DEBUG_VOICE_PRINT("\tSkipHundreds");
  } else {
    if (hundreds > 0) {
      sayHundreds(hundreds, (tens > 0));
    } else {
      DEBUG_VOICE_PRINT("\tNoHundreds");
    }
  }
  if (tens > 0) {
    sayTens(tens, (decimals > 0));
  } else {
    if (hundreds > 0) {
      DEBUG_VOICE_PRINT("\tNoTens");
    } else {  //forcibly say ZERO
      sayTens(tens, (decimals > 0));
    }
  }
  if (decimals > 0) {
    sayDecimals(decimals, (hundreds == 0 && tens == 0));
  } else {
    DEBUG_VOICE_PRINT("\tNoDecimals");
  }

  DEBUG_VOICE_PRINTLN("//");
}

void sayHundreds(int hundreds, bool raisingIntonation) {
  if (hundreds > 5000) {
    DEBUG_OPERATIONS_PRINTLN();
    DEBUG_OPERATIONS_PRINT("ERROR: sayHundreds(hundreds=");
    DEBUG_OPERATIONS_PRINTDEC(hundreds);
    DEBUG_OPERATIONS_PRINT(", raisingIntonation=");
    DEBUG_OPERATIONS_PRINTDEC(raisingIntonation);
    DEBUG_OPERATIONS_PRINTLN("): hundreds > 5000; must be positive 100 ... 5000 in whole hundreds");
    return;
  }

  if (hundreds % 100 > 0) {
    DEBUG_OPERATIONS_PRINTLN();
    DEBUG_OPERATIONS_PRINT("ERROR: sayHundreds(hundreds=");
    DEBUG_OPERATIONS_PRINTDEC(hundreds);
    DEBUG_OPERATIONS_PRINT(", raisingIntonation=");
    DEBUG_OPERATIONS_PRINT(raisingIntonation);
    DEBUG_OPERATIONS_PRINTLN("): hundreds % 100 > 0; must be positive 100 ... 5000 in whole hundreds");
    return;
  }

  if (hundreds <= 0) {
    DEBUG_OPERATIONS_PRINTLN();
    DEBUG_OPERATIONS_PRINT("ERROR: sayHundreds(hundreds=");
    DEBUG_OPERATIONS_PRINTDEC(hundreds);
    DEBUG_OPERATIONS_PRINT(", raisingIntonation=");
    DEBUG_OPERATIONS_PRINTDEC(raisingIntonation);
    DEBUG_OPERATIONS_PRINTLN("): hundreds <= 0; must be positive 100 ... 5000 in whole hundreds");
    return;
  }

  DEBUG_VOICE_PRINT("\t");
  DEBUG_VOICE_PRINT(hundreds);
  if (raisingIntonation) DEBUG_VOICE_PRINT("+ ");

  if (raisingIntonation) hundreds++;
  sayAsyncWait(hundreds);
}

void sayTens(int tens, bool raisingIntonation) {
  if (tens > 99) {
    DEBUG_OPERATIONS_PRINTLN();
    DEBUG_OPERATIONS_PRINT("ERROR: sayTens(tens=");
    DEBUG_OPERATIONS_PRINTDEC(tens);
    DEBUG_OPERATIONS_PRINT(", raisingIntonation=");
    DEBUG_OPERATIONS_PRINTDEC(raisingIntonation);
    DEBUG_OPERATIONS_PRINTLN("): tens > 99; must be positive 0 ... 99");
    return;
  }

  if (tens < 0) {
    DEBUG_OPERATIONS_PRINTLN();
    DEBUG_OPERATIONS_PRINT("ERROR: sayTens(tens=");
    DEBUG_OPERATIONS_PRINTDEC(tens);
    DEBUG_OPERATIONS_PRINT(", raisingIntonation=");
    DEBUG_OPERATIONS_PRINTDEC(raisingIntonation);
    DEBUG_OPERATIONS_PRINTLN("): tens < 0; must be positive 0 ... 99");
    return;
  }

  DEBUG_VOICE_PRINT("\t");
  DEBUG_VOICE_PRINT(tens);
  DEBUG_VOICE_PRINT("=");
  if (tens >= 0 && tens <= 19) {
    int oneDigit = tens * 10 + 9000;	// 1 => 9010
    if (raisingIntonation) {
      oneDigit++;	// 1 => 9011
      DEBUG_VOICE_PRINT("+");
    }
    sayAsyncWait(oneDigit);
    return;
  }

  int higherDecimal = ((int)tens / 10 ) * 10;
  int lowerDecimal = tens - higherDecimal;
  int higherDecimalFile =  higherDecimal * 10 + 9000;	// 40 => 9400
  int lowerDecimalFile = lowerDecimal * 10 + 9000;	//  2 => 9020
  if (lowerDecimal > 0) {
    if (higherDecimal > 0) {
      higherDecimalFile++;		// 40 => 9401
      DEBUG_VOICE_PRINT(higherDecimal);
      DEBUG_VOICE_PRINT("+");
    }
    DEBUG_VOICE_PRINT(lowerDecimal);
    if (raisingIntonation) {
      lowerDecimalFile++;							//  2 => 9021
      DEBUG_VOICE_PRINT("+L ");
    }
  } else {
    DEBUG_VOICE_PRINT(higherDecimal);
    if (raisingIntonation) {
      higherDecimalFile++;						// 40 => 9401
      DEBUG_VOICE_PRINT("+H ");
    }
  }
  if (higherDecimal > 0) sayAsyncWait(higherDecimalFile);
  if (lowerDecimal > 0) sayAsyncWait(lowerDecimalFile);
}

void sayDecimals(int decimals, bool leadingZeroNeeded) {
  if (decimals > 99) {
    DEBUG_OPERATIONS_PRINTLN();
    DEBUG_OPERATIONS_PRINT("sayDecimals(decimals=");
    DEBUG_OPERATIONS_PRINTDEC(decimals);
    DEBUG_OPERATIONS_PRINT(", leadingZeroNeeded=");
    DEBUG_OPERATIONS_PRINTDEC(leadingZeroNeeded);
    DEBUG_OPERATIONS_PRINTLN("): tens > 99");
    return;
  }

  if (leadingZeroNeeded) sayAsyncWait(9000);
  sayPoint();
  sayTens(decimals, false);
}

void sayMinus() {
  DEBUG_VOICE_PRINT("\tMINUS");
  sayAsyncWait(1);
}

void sayPoint() {
  DEBUG_VOICE_PRINT("\tPOINT");
  sayAsyncWait(2);
}

void sayMeters() {
  DEBUG_VOICE_PRINT("\tMETERS");
  sayAsyncWait(3);
}

void sayDegreesCelcium() {
  DEBUG_VOICE_PRINT("\tDEGREES_CELCIUM");
  sayAsyncWait(5);
  DEBUG_VOICE_PRINTLN();
}

void sayMetersAboveSeaLevel() {
  DEBUG_VOICE_PRINT("\tMETERS_ABOVE_SEA_LEVEL");
  sayAsyncWait(6);
  DEBUG_VOICE_PRINTLN();
}

void sayBarometerFailure() {
  DEBUG_VOICE_PRINT("sayBarometerFailure(): ");
  sayAsyncWait(80);
  DEBUG_VOICE_PRINTLN();
}

void sayBarometerNegativeAltitude() {
  DEBUG_VOICE_PRINT("sayBarometerNegativeAltitude(): ");
  sayAsyncWait(81);
  DEBUG_VOICE_PRINTLN();
}

void sayBarometerNotResponding() {
  DEBUG_VOICE_PRINT("sayBarometerNotResponding(): ");
  sayAsyncWait(82);
  DEBUG_VOICE_PRINTLN();
}

void sayBarometerConnected() {
  DEBUG_VOICE_PRINT("sayBarometerConnected(): ");
  sayAsyncWait(83);
  DEBUG_VOICE_PRINTLN();
}
void sayAsync(int fourDigitFileNumber) {
  sayAsyncWaitBlock(fourDigitFileNumber, false);
}

void sayAsyncWait(int fourDigitFileNumber) {
  // imitating Wtv020sd16p::playVoice with measured delay(),
  // I hear waiting for unBUSY signal takes extra 100ms?... didn't measure, though
  sayAsyncWaitBlock(fourDigitFileNumber, true);
}

void sayAsyncWaitBlock(int fourDigitFileNumber, boolean waitToBlock) {
  DEBUG_VOICE_PRINT(" {");
  DEBUG_VOICE_PRINT(fourDigitFileNumber);
  DEBUG_VOICE_PRINT(".ad4");
  digitalWrite(pinLed, HIGH);

  wtv020sd16p.asyncPlayVoice(fourDigitFileNumber);
  delay(580);		// I guess WTV020 needs ~200..500ms to read the file from microSD
  if (waitToBlock) {
    delay(getFileDuration(fourDigitFileNumber));
    // failed attempt to skip any message by button press
/*  int exitAt = millis() + getFileDuration(fourDigitFileNumber);
    while (millis() <= exitAt) {
      delay(10);
      button.poll();
      if (button.pushed()) {
        //wtv020sd16p.stopVoice();
        wtv020sd16p.resetFast();
        break;
      }
    }*/
  }

  digitalWrite(pinLed, LOW);
  DEBUG_VOICE_PRINT("}");
}

int getFileDuration(int fourDigitFileNumber) {
  // 1) wav_duration.bat BEGIN
  // @rem http://stackoverflow.com/questions/7105433/windows-batch-echo-without-new-line
  // echo|set /p=%1 >> wav_durations.txt
  // soxi -D %1 >> wav_durations.txt
  // wav_duration.bat END
  // 2) C:\TTS4barometer> copy sox.exe soxi.exe
  // 3) C:\TTS4barometer> for %i in (*.wav) do wav_duration %i
  // 4) open wav_durations.txt (0001.wav	0.482688) in excel
  // 5) add one column with "=LEFT(A1,4)"; "0005.wav" will become ".wav"
  // 6) add another column with "=ROUND(B1,3)*1000"; "0.430688" will become "430"
  // 7) remove two original columns, export two new columns to wav_cases.txt
  // 8) open wav_cases.txt in Notepad++
  // 9) Find what: (\d+)\t(\d+) ; Replace with: \tcase $1:\treturn\($2\)
  // 10) 4 times: Find what: <space>0 ; Replace with: 0
  // 11) you'll get the content to paste here into switch(fourDigitFileNumber)
  switch (fourDigitFileNumber) {
    case    1:	return	482;
    case    2:	return	230;
    case    3:	return	538;
    case    5:	return	884;
    case    6:	return	1388;
    case   10:	return	4705;
    case   11:	return	1722;
    case   12:	return	1571;
    case   13:	return	1522;
    case   14:	return	1245;
    case   15:	return	1730;
    case   50:	return	2137;
    case   51:	return	2747;
    case   52:	return	2127;
    case   70:	return	2574;
    case   80:	return	8037;
    case   81:	return	2390;
    case   82:	return	2177;
    case   83:	return	663;
    case   85:	return	1414;
    case  100:	return	608;
    case  101:	return	575;
    case  200:	return	670;
    case  201:	return	600;
    case  300:	return	676;
    case  301:	return	576;
    case  400:	return	719;
    case  401:	return	638;
    case  500:	return	691;
    case  501:	return	631;
    case  600:	return	721;
    case  601:	return	678;
    case  700:	return	867;
    case  701:	return	799;
    case  800:	return	640;
    case  801:	return	638;
    case  900:	return	748;
    case  901:	return	661;
    case 1000:	return	614;
    case 1001:	return	667;
    case 1100:	return	855;
    case 1101:	return	806;
    case 1200:	return	735;
    case 1201:	return	678;
    case 1300:	return	867;
    case 1301:	return	861;
    case 1400:	return	862;
    case 1401:	return	848;
    case 1500:	return	941;
    case 1501:	return	907;
    case 1600:	return	1066;
    case 1601:	return	987;
    case 1700:	return	999;
    case 1701:	return	979;
    case 1800:	return	915;
    case 1801:	return	842;
    case 1900:	return	963;
    case 1901:	return	941;
    case 2000:	return	803;
    case 2001:	return	818;
    case 2100:	return	952;
    case 2101:	return	861;
    case 2200:	return	1005;
    case 2201:	return	952;
    case 2300:	return	1015;
    case 2301:	return	907;
    case 2400:	return	1033;
    case 2401:	return	922;
    case 2500:	return	1035;
    case 2501:	return	993;
    case 2600:	return	1070;
    case 2601:	return	978;
    case 2700:	return	1096;
    case 2701:	return	1013;
    case 2800:	return	1048;
    case 2801:	return	1035;
    case 2900:	return	1027;
    case 2901:	return	976;
    case 3000:	return	765;
    case 3001:	return	812;
    case 3100:	return	964;
    case 3101:	return	829;
    case 3200:	return	958;
    case 3201:	return	930;
    case 3300:	return	1043;
    case 3301:	return	851;
    case 3400:	return	1074;
    case 3401:	return	940;
    case 3500:	return	1013;
    case 3501:	return	946;
    case 3600:	return	1031;
    case 3601:	return	899;
    case 3700:	return	1082;
    case 3701:	return	1005;
    case 3800:	return	1001;
    case 3801:	return	855;
    case 3900:	return	1019;
    case 3901:	return	914;
    case 4000:	return	833;
    case 4001:	return	812;
    case 4100:	return	887;
    case 4101:	return	869;
    case 4200:	return	956;
    case 4201:	return	863;
    case 4300:	return	952;
    case 4301:	return	849;
    case 4400:	return	1003;
    case 4401:	return	898;
    case 4500:	return	1004;
    case 4501:	return	912;
    case 4600:	return	977;
    case 4601:	return	888;
    case 4700:	return	1067;
    case 4701:	return	967;
    case 4800:	return	934;
    case 4801:	return	908;
    case 4900:	return	964;
    case 4901:	return	863;
    case 5000:	return	716;
    case 5001:	return	3099;
    case 9000:	return	635;
    case 9001:	return	457;
    case 9010:	return	481;
    case 9011:	return	245;
    case 9020:	return	460;
    case 9021:	return	205;
    case 9030:	return	438;
    case 9031:	return	232;
    case 9040:	return	500;
    case 9041:	return	303;
    case 9050:	return	431;
    case 9051:	return	282;
    case 9060:	return	581;
    case 9061:	return	353;
    case 9070:	return	535;
    case 9071:	return	450;
    case 9080:	return	359;
    case 9081:	return	240;
    case 9090:	return	576;
    case 9091:	return	299;
    case 9100:	return	370;
    case 9101:	return	244;
    case 9110:	return	597;
    case 9111:	return	414;
    case 9120:	return	534;
    case 9121:	return	353;
    case 9130:	return	565;
    case 9131:	return	410;
    case 9140:	return	625;
    case 9141:	return	422;
    case 9150:	return	625;
    case 9151:	return	485;
    case 9160:	return	776;
    case 9161:	return	678;
    case 9170:	return	845;
    case 9171:	return	666;
    case 9180:	return	698;
    case 9181:	return	491;
    case 9190:	return	784;
    case 9191:	return	589;
    case 9200:	return	410;
    case 9201:	return	330;
    case 9300:	return	381;
    case 9301:	return	354;
    case 9400:	return	536;
    case 9401:	return	303;
    case 9500:	return	511;
    case 9501:	return	291;
    case 9600:	return	578;
    case 9601:	return	512;
    case 9700:	return	651;
    case 9701:	return	541;
    case 9800:	return	412;
    case 9801:	return	299;
    case 9900:	return	584;
    case 9901:	return	417;
    default:    return  3000;
  }
  DEBUG_OPERATIONS_PRINTLN();
  DEBUG_OPERATIONS_PRINTLN();
  DEBUG_OPERATIONS_PRINT("getFileDuration(fourDigitFileNumber = ");
  DEBUG_OPERATIONS_PRINTDEC(fourDigitFileNumber);
  DEBUG_OPERATIONS_PRINTLN("): NO ENTRY IN DURATION LOOKUP TABLE, returned 5 seconds");
  return 5000;
}

//http://forum.arduino.cc/index.php/topic,41497.0.html
//http://rexpirando.blogspot.ca/2011/01/volt-amperimetro-com-arduino-parte-1.html
// compiler error: ISO C++ forbids declaration of 'T' with no type
/*template <class T> int EEPROM_writeAnything(int ee, const T& value) {
  const byte* p = (const byte*)(const void*)&value;
  int i;
  for (i = 0; i < sizeof(value); i++) EEPROM.write(ee++, *p++);
  return i;
}
template <class T> int EEPROM_readAnything(int ee, T& value) {
  byte* p = (byte*)(void*)&value;
  int i;
  for (i = 0; i < sizeof(value); i++) *p++ = EEPROM.read(ee++);
  return i;
}
*/
//"unwrapped" template<class T> above for floats and ints
int EEPROM_writeFloat(int ee, const float & value) {
  const byte* p = (const byte*)(const void*)&value;
  int i;
  for (i = 0; i < sizeof(value); i++) EEPROM.write(ee++, *p++);
  return i;
}
int EEPROM_readFloat(int ee, float & value) {
  byte* p = (byte*)(void*)&value;
  int i;
  for (i = 0; i < sizeof(value); i++) *p++ = EEPROM.read(ee++);
  return i;
}
int EEPROM_writeInt(int ee, const int & value) {
  const byte* p = (const byte*)(const void*)&value;
  int i;
  for (i = 0; i < sizeof(value); i++) EEPROM.write(ee++, *p++);
  return i;
}
int EEPROM_readInt(int ee, int & value) {
  byte* p = (byte*)(void*)&value;
  int i;
  for (i = 0; i < sizeof(value); i++) *p++ = EEPROM.read(ee++);
  return i;
}

void seaLevelReadEeprom() {
  int bytesRead = EEPROM_readInt(
                    altitudeAboveMeanSeaLevelAtTakeoffEepromOffset, altitudeAboveMeanSeaLevelAtTakeoff);
  DEBUG_EEPROM_PRINT("seaLevelReadEeprom(): altitudeAboveMeanSeaLevelAtTakeoff=");
  DEBUG_EEPROM_PRINT(altitudeAboveMeanSeaLevelAtTakeoff);
  DEBUG_EEPROM_PRINT("; bytesRead=");
  DEBUG_EEPROM_PRINTDEC(bytesRead);
  DEBUG_EEPROM_PRINT("; EepromOffset=");
  DEBUG_EEPROM_PRINTLN(altitudeAboveMeanSeaLevelAtTakeoffEepromOffset);
}

void seaLevelWriteEeprom() {
  int bytesWritten = EEPROM_writeInt(
                       altitudeAboveMeanSeaLevelAtTakeoffEepromOffset, altitudeAboveMeanSeaLevelAtTakeoff);
  DEBUG_EEPROM_PRINT("seaLevelWriteEeprom(): altitudeAboveMeanSeaLevelAtTakeoff=");
  DEBUG_EEPROM_PRINTDEC(altitudeAboveMeanSeaLevelAtTakeoff);
  DEBUG_EEPROM_PRINT("; bytesWritten=");
  DEBUG_EEPROM_PRINTDEC(bytesWritten);
  DEBUG_EEPROM_PRINT("; EepromOffset=");
  DEBUG_EEPROM_PRINTLN(altitudeAboveMeanSeaLevelAtTakeoffEepromOffset);
}

void modeTalkReadEeprom() {
  int bytesRead = EEPROM_readInt(modeTalkEepromOffset, modeTalk);
  DEBUG_EEPROM_PRINT("modeTalkReadEeprom(): modeTalk=");
  DEBUG_EEPROM_PRINT(modeTalk);
  DEBUG_EEPROM_PRINT("; bytesRead=");
  DEBUG_EEPROM_PRINT(bytesRead);
  DEBUG_EEPROM_PRINT("; EepromOffset=");
  DEBUG_EEPROM_PRINTLN(modeTalkEepromOffset);
}

void modeTalkWriteEeprom() {
  int bytesWritten = EEPROM_writeInt(modeTalkEepromOffset, modeTalk);
  DEBUG_EEPROM_PRINT("modeTalkWriteEeprom(): modeTalk=");
  DEBUG_EEPROM_PRINTDEC(modeTalk);
  DEBUG_EEPROM_PRINT("; bytesWritten=");
  DEBUG_EEPROM_PRINTDEC(bytesWritten);
  DEBUG_EEPROM_PRINT("; EepromOffset=");
  DEBUG_EEPROM_PRINTLN(modeTalkEepromOffset);
}

void doubleBlink() { // double-blink means "DONE REPORTING"
  digitalWrite(pinLed, LOW);
  delay(100);
  digitalWrite(pinLed, HIGH);
  delay(50);
  digitalWrite(pinLed, LOW);
  delay(100);
  digitalWrite(pinLed, HIGH);
  delay(50);
  digitalWrite(pinLed, LOW);
}

int lastHundreds = 0;
int lastHundredsSkipped = 0;
int lastHundredsSkippedLimit = 4;

bool shouldSkipHundreds(int hundreds, int tens) {
  if (canSkipHundreds == false) return false;
  if (tens == 0) return false;
  if (lastHundreds == 0) {
    lastHundreds = hundreds;
    return false;
  }
  if (hundreds != lastHundreds) {
    lastHundreds = hundreds;
    lastHundredsSkipped = 0;
    return false;
  }
  if (lastHundredsSkipped < lastHundredsSkippedLimit) {
    lastHundredsSkipped++;
    return true;
  }
  lastHundredsSkipped = 0;
  return false;
}

// END BMP180_WTV020\BMP180_WTV020.ino
