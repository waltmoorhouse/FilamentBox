#include <TimerOne.h>

#define COMMAND_MAX_SIZE 50

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

const int READINGS_PER_SECOND = 1;
int maxHumidity = 5;
int maxTemp = 80;

float w1 = 1.2345;
float w2 = 2.3456;
float w3 = 3.4567;
float w4 = 4.5678;
  
bool heaterPowerOn = true;
bool toggle = true;

void setup() {
  Serial.begin(115200);
  pinMode(HEATER, OUTPUT);

  Timer1.initialize(1000000 / READINGS_PER_SECOND);
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
          w1 = 0.00;
        } else if (strcmp(command, "2") == 0) {
          w2 = 0.00;
        } else if (strcmp(command, "3") == 0) {
          w3 = 0.00;
        } else if (strcmp(command, "4") == 0) {
          w4 = 0.00;
        } else {
          Serial.print(F("ERROR: Bad Scale Number: "));
          Serial.println(command);
        }
      } else {
        Serial.print(F("ERROR: Bad Command: "));
        Serial.println(command);
      }
    }
  }
}



void report(void) {
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = 15.123;
  // Read temperature as Celsius
  float t = 42.536;

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println(F("ERROR: Failed to read from DHT sensor!"));
    return;
  }

  if (t > maxTemp) {
    digitalWrite(HEATER, LOW);
    heaterPowerOn = false;
  } else {
    if (h > maxHumidity) {
      digitalWrite(HEATER, HIGH);
      heaterPowerOn = true;
    }
  }

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
  toggle = !toggle;
}
