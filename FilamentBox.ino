/**
 * FilamentBox Arduino code by Walt Moorhouse.
 * MOCK_MODE is for testing changes to the logic without having to connect all the sensor. It can be run on an Uno or a Mega.
 * You must have a Mega to run with actual hardware.
 */
#include <TimerOne.h>

// Comment out to run with actual hardware, unncomment to run in MOCK mode.
// #define MOCK_MODE true
// #define DEBUG_MODE true

#define COMMAND_MAX_SIZE 25
#define RESPONSE_MAX_SIZE 10

#ifndef MOCK_MODE
  #include <ClickEncoder.h>
  #include <DHT.h>
  #include <DHT_U.h>
  #include <HX711.h>
  #include <LiquidCrystal_I2C.h>
  #include <SD.h>
  #include <SPI.h>
  #include <TimerThree.h>
  #include <Wire.h>

  #define DHTTYPE       DHT22
  
  #define DHTPIN        2
  #define HEATER        3
  
  #define SCALE1_DOUT   4
  #define SCALE1_CLK    5
  
  #define SCALE2_DOUT   6
  #define SCALE2_CLK    7
  
  #define SCALE3_DOUT   8
  #define SCALE3_CLK    9
  
  #define SCALE4_DOUT   10
  #define SCALE4_CLK    11
  
  #define ROT1_DT       A0
  #define ROT1_CLK      A1
  #define ROT1_SW       20
  
  #define ROT2_DT       A2
  #define ROT2_CLK      A3
  #define ROT2_SW       22
  
  #define ROT3_DT       A6
  #define ROT3_CLK      A7
  #define ROT3_SW       24
  
  #define ROT4_DT       A8
  #define ROT4_CLK      A9
  #define ROT4_SW       26
  
  #define LCD_SDA       A4
  #define LCD_SCL       A5
  #define LCD_LINES     4
  #define LCD_CHARS     20

  #define SD_MISO       50
  #define SD_MOSI       51
  #define SD_SCLK       52
  #define SD_CS         53

  HX711 scale1;
  HX711 scale2;
  HX711 scale3;
  HX711 scale4;
  
  ClickEncoder *encoder1;
  ClickEncoder *encoder2;
  ClickEncoder *encoder3;
  ClickEncoder *encoder4;
  
  DHT dht(DHTPIN, DHTTYPE);
  LiquidCrystal_I2C lcd(0x27, LCD_CHARS, LCD_LINES);

#endif

const int READINGS_PER_MINUTE = 10;
const char * SETTINGS_FILENAME = "settings.dat";
int maxHumidity = 5;
int maxTemp = 80;

float scale1_calibration_factor = -421000;
float scale2_calibration_factor = -421000;
float scale3_calibration_factor = -421000;
float scale4_calibration_factor = -421000;

#ifndef MOCK_MODE
  float l1 = 0.00;
  float l2 = 0.00;
  float l3 = 0.00;
  float l4 = 0.00;
#else
  float w1 = 1.23;
  float w2 = 2.34;
  float w3 = 3.45;
  float w4 = 4.56;
  
  float l1 = 100.12;
  float l2 = 50.34;
  float l3 = 75.56;
  float l4 = 150.78;
#endif

bool heaterPowerOn = true;
bool toggle = true;
bool writingToSerial = false;

#ifndef MOCK_MODE
  void timerIsr() {
    encoder1->service();
    encoder2->service();
    encoder3->service();
    encoder4->service();
  }
#endif

void setup() {
  Serial.begin(115200);

#ifndef MOCK_MODE
    if (!SD.begin(SD_CS)) {
      Serial.println(F("ERROR: Could not initialize SD Card!"));
    }
    loadCalibrationSettingsFromSd();
  
    pinMode(HEATER, OUTPUT);
    dht.begin();
    
    scale1.begin(SCALE1_DOUT, SCALE1_CLK);
    scale1.set_scale(scale1_calibration_factor); 
    scale1.tare();
    
    scale2.begin(SCALE2_DOUT, SCALE2_CLK);
    scale2.set_scale(scale2_calibration_factor);
    scale2.tare();
    
    scale3.begin(SCALE3_DOUT, SCALE3_CLK);
    scale3.set_scale(scale3_calibration_factor);
    scale3.tare();
    
    scale4.begin(SCALE4_DOUT, SCALE4_CLK);
    scale4.set_scale(scale4_calibration_factor);
    scale4.tare();

    encoder1 = new ClickEncoder(ROT1_CLK, ROT1_DT, ROT1_SW);
    encoder2 = new ClickEncoder(ROT2_CLK, ROT2_DT, ROT2_SW);
    encoder3 = new ClickEncoder(ROT3_CLK, ROT3_DT, ROT3_SW);
    encoder4 = new ClickEncoder(ROT4_CLK, ROT4_DT, ROT4_SW);
    encoder1->setAccelerationEnabled(false);
    encoder3->setAccelerationEnabled(false);
    encoder3->setAccelerationEnabled(false);
    encoder4->setAccelerationEnabled(false);
    
    Timer3.initialize(1000);
    Timer3.attachInterrupt(timerIsr); 
#endif
  Serial.println(F("Waiting on Sensors to warm up before beginning..."));
  delay(10000);
  Serial.println(F("Sensors ready."));
  Timer1.initialize(60000000/READINGS_PER_MINUTE);
  Timer1.attachInterrupt(report);
}

void loop() {
#ifndef MOCK_MODE  
    l1 += encoder1->getValue();
    l2 += encoder2->getValue();
    l3 += encoder3->getValue();
    l4 += encoder4->getValue();
    
    ClickEncoder::Button b1 = encoder1->getButton();
    if (b1 == ClickEncoder::DoubleClicked) {
      l1 = 0.00;
    }
    ClickEncoder::Button b2 = encoder2->getButton();
    if (b2 == ClickEncoder::DoubleClicked) {
      l2 = 0.00;
    }
    ClickEncoder::Button b3 = encoder3->getButton();
    if (b3 == ClickEncoder::DoubleClicked) {
      l3 = 0.00;
    }
    ClickEncoder::Button b4 = encoder4->getButton();
    if (b4 == ClickEncoder::DoubleClicked) {
      l4 = 0.00;
    }
#endif
  if (Serial.available() > 0) {
    char input[COMMAND_MAX_SIZE + 1];
    // Get next command from Serial (add 1 for final 0)
    byte size = Serial.readBytes(input, COMMAND_MAX_SIZE);
    // Add the final 0 to end the C string
    input[size] = 0;
    
    char* command = strtok(input, " ");
    if (command != 0) {
      if (strcmp(command, "SET") == 0) {
        // Get the next part
        command = strtok(0, " ");
        // Split the command in two values
        char* separator = strchr(command, '=');
        if (separator != 0) {
          // Actually split the string in 2: replace '=' with 0
          *separator = 0;
          char* setting = command;
          ++separator;
          int newValue = atoi(separator);
          
          if  (strcmp(setting, "T") == 0) {
            maxTemp = newValue;
          } else if  (strcmp(setting, "H") == 0) {
            maxHumidity = newValue;
          } else {
            safeWrite(F("ERROR: Bad Setting: "), setting);
          }
        }
      } else if (strcmp(command, "TARE") == 0) {
        // Get the next part
        command = strtok(0, " ");
        if (strcmp(command, "1") == 0) {
#ifndef MOCK_MODE
          scale1.tare();
#endif
        } else if (strcmp(command, "2") == 0) {
#ifndef MOCK_MODE
          scale2.tare();
#endif
        } else if (strcmp(command, "3") == 0) {
#ifndef MOCK_MODE
          scale3.tare();
#endif
        } else if (strcmp(command, "4") == 0) {
#ifndef MOCK_MODE
          scale4.tare();
#endif
        } else {
          safeWrite(F("ERROR: Bad Scale Number: "), command);
        }
      } else if (strcmp(command, "ZERO") == 0) {
        // Get the next part
        command = strtok(0, " ");
        if (strcmp(command, "1") == 0) {
          l1 = 0.00;
        } else if (strcmp(command, "2") == 0) {
          l2 = 0.00;
        } else if (strcmp(command, "3") == 0) {
          l3 = 0.00;
        } else if (strcmp(command, "4") == 0) {
          l4 = 0.00;
        } else {
          safeWrite(F("ERROR: Bad Encoder Number: "), command);
        }
      } else if (strcmp(command, "CALI") == 0) {
        command = strtok(0, " ");
        calibrate(atoi(command));
      } else {
        safeWrite(F("ERROR: Bad Command: "), input);
      }
    }
  }
  delay(50);
}

void report(void) {
#ifdef DEBUG_MODE
  Serial.println(F("DEBUG: Begin report"));
#endif
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
#ifndef MOCK_MODE
    float h = dht.readHumidity();
    // Read temperature as Celsius
    float t = dht.readTemperature();
#else
    float h = 15.123;
    float t = 42.536;
#endif
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println(F("ERROR: Failed to read from DHT sensor!"));
    return;
  }

  if (t > maxTemp) {
    heaterPowerOn = false;
    #ifndef MOCK_MODE
      digitalWrite(HEATER, HIGH);
      lcd.noBacklight();
    #endif
  } else {
    if (h > maxHumidity) {
      heaterPowerOn = true;
      #ifndef MOCK_MODE
        digitalWrite(HEATER, LOW);
        lcd.backlight();
      #endif
    }
  }
  
  #ifndef MOCK_MODE
    float w1 = scale1.get_units();
    float w2 = scale2.get_units();
    float w3 = scale3.get_units();
    float w4 = scale4.get_units();
  #endif

  // Report to Serial
  while (writingToSerial) {
    delay(3);
  }
  writingToSerial = true;
  Serial.print(F("H:"));
  Serial.print(h);
  Serial.print(F("% T:"));
  Serial.print(t);
  Serial.print(F("C S1:"));
  Serial.print(w1);
  Serial.print(F("kg S2:"));
  Serial.print(w2);
  Serial.print(F("kg S3:"));
  Serial.print(w3);
  Serial.print(F("kg S4:"));
  Serial.print(w4);
  Serial.print(F("kg L1:"));
  Serial.print(l1);
  Serial.print(F("mm L2:"));
  Serial.print(l2);
  Serial.print(F("mm L3:"));
  Serial.print(l3);
  Serial.print(F("mm L4:"));
  Serial.print(l4);
  Serial.print(F("mm P:"));
  if (heaterPowerOn) {
    Serial.println(F("ON"));
  } else {
    Serial.println(F("OFF"));
  }
  writingToSerial = false;

#ifndef MOCK_MODE
    // update LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Humidity: "));
    lcd.print(h);
    lcd.print(F("%"));
    lcd.setCursor(0, 1);
    lcd.print(F("Temp: "));
    lcd.print(t);
    lcd.print(F(" C"));
    lcd.setCursor(0, 2);
    if (toggle) {
      lcd.print(F("Spool 1: "));
      lcd.print(w1);
      lcd.print(F("kg"));
      lcd.setCursor(0, 3);
      lcd.print(F("Spool 2: "));
      lcd.print(w2);
      lcd.print(F("kg"));
    } else {
      lcd.print(F("Spool 3: "));
      lcd.print(w3);
      lcd.print(F("kg"));
      lcd.setCursor(0, 3);
      lcd.print(F("Spool 4: "));
      lcd.print(w4);
      lcd.print(F("kg"));
    }
#endif
  toggle = !toggle;
  
#ifdef DEBUG_MODE
  Serial.println(F("DEBUG: End report"));
#endif
}

void calibrate(int scaleNum) {
  Timer1.stop();
  calibrationWrite(F("Entering calibration mode..."));
#ifndef MOCK_MODE
  HX711 scale;
  switch(scaleNum) {
    case 1:
      scale = scale1;
      break;
    case 2:
      scale = scale2;
      break;
    case 3:
      scale = scale3;
      break;
    case 4:
      scale = scale4;
      break;
    default:
       safeWrite(F("ERROR: Bad Scale: "), scaleNum);
       Timer1.start();
       return;
  }
  scale.set_scale();
  scale.tare();
  long zero_factor = scale.read_average();
#else
  long zero_factor = 0;
#endif
  calibrationDataWrite(F("Zero Factor = "), zero_factor);
  delay(1000);
  String answer = prompt(String(F("Place known weight on scale. Enter weight (in kg) and click OK.")));

#ifdef DEBUG_MODE
  Serial.print(F("DEBUG: String answer="));
  Serial.println(answer);
#endif
  if (strcmp(answer.c_str(), "CANCEL") == 0) {
    calibrationWrite(F("Calibration canceled."));
    Timer1.start();
    return;
  }
  float targetKg = atof(answer.c_str());
  calibrationDataWrite(F("Calibration grams = "), targetKg * 1000);
  
#ifndef MOCK_MODE
  float currentCalibrationFactor = getCalibrationValue(scaleNum);
  scale.set_scale(currentCalibrationFactor);
  float currentReading = scale.get_units();
  while (currentReading != targetKg) {
    String promptMsg(F("Enter new Calibration value and click OK. Current reading is "));
    promptMsg += String(currentReading, 4);
    promptMsg += '/';
    promptMsg += String(targetKg, 4);
    promptMsg += String(F(" Current Calibration Value is "));
    promptMsg += String(currentCalibrationFactor);
    answer = prompt(promptMsg);
    
    if (strcmp(answer.c_str(), "CANCEL") == 0) {
      calibrationWrite(F("Calibration canceled."));
      Timer1.start();
      return;
    } else if (strcmp(answer.c_str(), "ACCEPT") == 0) {
      setCalibrationValue(scaleNum, currentCalibrationFactor);
      calibrationWrite(F("Calibration complete."));
      Timer1.start();
      return;
    } else {
      currentCalibrationFactor = atof(answer.c_str());
      scale.set_scale(currentCalibrationFactor);
      currentReading = scale.get_units();
    }
  }
#else
  delay(2000);
#endif
  calibrationWrite(F("Target matched. Calibration complete."));
  Timer1.start();
}

float getCalibrationValue(int scaleNum) {
  switch(scaleNum) {
    case 1:
      return scale1_calibration_factor;
    case 2:
      return scale2_calibration_factor;
    case 3:
      return scale3_calibration_factor;
    case 4:
      return scale4_calibration_factor;
    default:
      return 0;
  }
}

void setCalibrationValue(int scaleNum, float newValue) {
  switch(scaleNum) {
    case 1:
      scale1_calibration_factor = newValue;
      break;
    case 2:
      scale2_calibration_factor = newValue;
      break;
    case 3:
      scale3_calibration_factor = newValue;
      break;
    case 4:
      scale4_calibration_factor = newValue;
      break;
  }
#ifndef MOCK_MODE
  saveCalibrationSettingsToSd();
#endif
}

String prompt(String msg) {
  writingToSerial = true;
  Serial.print(F("PROMPT:"));
  Serial.println(msg);
  writingToSerial = false;
  
  while (true) {
    if (Serial.available() > 0) {
      char input[RESPONSE_MAX_SIZE + 1];
      byte size = Serial.readBytes(input, RESPONSE_MAX_SIZE);
      input[size] = 0;
#ifdef DEBUG_MODE
      Serial.print(F("DEBUG: input[]="));
      Serial.println(input);
#endif
      return String(input);
    }
    delay(10);
  }
}

void calibrationWrite(const __FlashStringHelper* msg) {
  writingToSerial = true;
  Serial.print(F("CALIBRATION:"));
  Serial.print(scale1_calibration_factor);
  Serial.print(F(" "));
  Serial.print(scale2_calibration_factor);
  Serial.print(F(" "));
  Serial.print(scale3_calibration_factor);
  Serial.print(F(" "));
  Serial.print(scale4_calibration_factor);
  Serial.print(F(":"));
  Serial.println(msg);
  writingToSerial = false;
}

void calibrationDataWrite(const __FlashStringHelper* msg, long data) {
  writingToSerial = true;
  Serial.print(F("CALIBRATION:"));
  Serial.print(scale1_calibration_factor);
  Serial.print(F(" "));
  Serial.print(scale2_calibration_factor);
  Serial.print(F(" "));
  Serial.print(scale3_calibration_factor);
  Serial.print(F(" "));
  Serial.print(scale4_calibration_factor);
  Serial.print(F(":"));
  Serial.print(msg);
  Serial.println(data);
  writingToSerial = false;
}

void safeWrite(const __FlashStringHelper* msg, char* data) {
  while (writingToSerial) {
    delay(5);
  }
  writingToSerial = true;
  Serial.print(msg);
  Serial.println(data);
  writingToSerial = false;
}

#ifndef MOCK_MODE
  void saveCalibrationSettingsToSd() {
    File file = SD.open(SETTINGS_FILENAME, FILE_WRITE);
    if (file) {
      file.print(scale1_calibration_factor);
      file.print(F(" "));
      file.print(scale2_calibration_factor);
      file.print(F(" "));
      file.print(scale3_calibration_factor);
      file.print(F(" "));
      file.print(scale4_calibration_factor);
  
      file.close();
    } else {
      safeWrite(F("ERROR: Could not open file for writing: "), SETTINGS_FILENAME);
    }
  }
  
  void loadCalibrationSettingsFromSd() {
    File file = SD.open(SETTINGS_FILENAME);
    String contents = "";
    if (file) {
      while (file.available()) {
        contents += (char) file.read();
      }
      file.close();
      char* setting = strtok(contents.c_str(), " ");
      scale1_calibration_factor = atof(setting);
      setting = strtok(0, " ");
      scale2_calibration_factor = atof(setting);
      setting = strtok(0, " ");
      scale3_calibration_factor = atof(setting);
      setting = strtok(0, " ");
      scale4_calibration_factor = atof(setting);
    } else {
      safeWrite(F("ERROR: Could not open file for reading: "), SETTINGS_FILENAME);
    }
  }
#endif
