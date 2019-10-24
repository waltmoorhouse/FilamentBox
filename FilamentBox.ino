#include <TimerOne.h>
#include <DHT.h>
#include <DHT_U.h>
#include <HX711.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#define COMMAND_MAX_SIZE 25

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

//define LCD_SDA A4
//define LCD_SCL A5
#define LCD_LINES   4
#define LCD_CHARS   20

HX711 scale1;
HX711 scale2;
HX711 scale3;
HX711 scale4;

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, LCD_CHARS, LCD_LINES);

const int READINGS_PER_SECOND = 1;
int maxHumidity = 5;
int maxTemp = 80;

//These values obtained by using the SparkFun_HX711_Calibration sketch
float scale1_calibration_factor = -7050;
float scale2_calibration_factor = -7050;
float scale3_calibration_factor = -7050;
float scale4_calibration_factor = -7050;

bool heaterPowerOn = true;
bool toggle = true;

void setup() {
  Serial.begin(115200);
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
  
  Timer1.initialize(1000000/READINGS_PER_SECOND);
  Timer1.attachInterrupt(report);
}

void loop() {
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
            Serial.print(F("ERROR: Bad Setting: "));
            Serial.println(setting);
          }
        }
      } else if (strcmp(command, "TARE") == 0) {
        // Get the next part
        command = strtok(0, " ");
        if (strcmp(command, "1") == 0) {
          scale1.tare();
        } else if (strcmp(command, "2") == 0) {
          scale2.tare();
        } else if (strcmp(command, "3") == 0) {
          scale3.tare();
        } else if (strcmp(command, "4") == 0) {
          scale4.tare();
        } else {
          Serial.print(F("ERROR: Bad Scale Number: "));
          Serial.println(command);
        }
      } else {
        Serial.print(F("ERROR: Bad Command: "));
        Serial.println(input);
      }
    }
  }
}


void report(void) {
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius
  float t = dht.readTemperature();
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println(F("ERROR: Failed to read from DHT sensor!"));
    return;
  }

  if (t > maxTemp) {
    digitalWrite(HEATER, LOW);
    heaterPowerOn = false;
    lcd.noBacklight();
  } else {
    if (h > maxHumidity) {
      digitalWrite(HEATER, HIGH);
      heaterPowerOn = true;
      lcd.backlight();
    }
  }

  float w1 = scale1.get_units();
  float w2 = scale2.get_units();
  float w3 = scale3.get_units();
  float w4 = scale4.get_units();

  // Report to Serial
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
  Serial.print(F("kg P:"));
  if (heaterPowerOn) {
    Serial.println(F("ON"));
  } else {
    Serial.println(F("OFF"));
  }

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
  toggle = !toggle;
}
