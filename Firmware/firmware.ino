#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

//display pins
#define TFT_SCLK 0 // labeled SCL on the screen
#define TFT_MOSI 1 // labeled SDA on the screen
#define TFT_RST 2
#define TFT_DC 3
#define TFT_CS 4
#define TFT_BL 5

//buzzer
#define BUZZER_PIN 6

//alarm time
int alarmHour = 10;
int alarmMinute = 30;
bool alarmTriggered = false;

//setup
class MyST7789 : public Adafruit_ST7789 {
public:
  MyST7789(int8_t cs, int8_t dc, int8_t mosi, int8_t sclk, int8_t rst)
    : Adafruit_ST7789(cs, dc, mosi, sclk, rst) {}
  void setOffsets(uint8_t col, uint8_t row) {
    _colstart = _colstart2 = col;
    _rowstart = _rowstart2 = row;
  }
};

MyST7789 tft(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

//init vars
unsigned long previousMillis = 0;

int hours = 0;
int minutes = 0;
int seconds = 0;

void setup() {
  Serial.begin(115200);

  // backlight
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, LOW);

  // init display
  tft.init(76, 284);
  tft.setOffsets(82, 18);
  tft.invertDisplay(false);
  tft.setRotation(1);
  tft.fillScreen(ST77XX_BLACK);

  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(4);

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);  

  drawClock();
}

void alarm() {
  for (int i = 0; i < 200; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(2);
    digitalWrite(BUZZER_PIN, LOW);
    delay(2);
  }
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 1000) {
    previousMillis = currentMillis;

    seconds++;

    if (seconds >= 60) {
      seconds = 0;
      minutes++;
    }

    if (minutes >= 60) {
      minutes = 0;
      hours++;
    }

    if (hours >= 24) {
      hours = 0;
      alarmTriggered = false;
    }

    // CHECK ALARM
    if (!alarmTriggered && hours == alarmHour && minutes == alarmMinute && seconds == 0) {
      alarmTriggered = true;
      alarm();
    }

    drawClock();
  }
}

void drawClock() {
  char timeString[9];
  char alarmString[16];

  sprintf(timeString, "%02d:%02d:%02d", hours, minutes, seconds);

  sprintf(alarmString, "ALARM %02d:%02d", alarmHour, alarmMinute);

  tft.fillScreen(ST77XX_BLACK);

  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(4);
  tft.setCursor(46, 5);
  tft.print(timeString);

  tft.setTextSize(1);
  tft.setCursor(105, 58);
  tft.print(alarmString);
}
