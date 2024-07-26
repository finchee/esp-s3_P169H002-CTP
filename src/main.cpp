#include <Arduino.h>
#include <TFT_eSPI.h>
#include <lvgl.h>
#include "User_Setup.h"
#include "demos/lv_demos.h"

TFT_eSPI tft = TFT_eSPI();

// put function declarations here:
void TFT_eSPI_setup(void);
void get_FPS(void);
void Screen_display(void);

unsigned long start_millis = 0;
int elapsed_time = 0;

void setup() {
  // put your setup code here, to run once:
  TFT_eSPI_setup();
}

void loop() {
  get_FPS();
  Screen_display();
}

// put function definitions here:
void TFT_eSPI_setup(void) {

  tft.init();
  tft.setRotation(0);
  tft.invertDisplay(1);
  
  // 先初始化, 后亮屏可以避免开机花屏
  ESP_LOGI("info","PWM start, TFT lighting...");
  analogWrite(10, 127);
}

void get_FPS(void)
{
  elapsed_time = 1000 / (millis() - start_millis);
  start_millis = millis();
}

void Screen_display(void)
{
  // Fill screen with grey so we can see the effect of printing with and without 
  // a background colour defined
  tft.fillScreen(TFT_DARKGREY);
  
  // Set "cursor" at top left corner of display (0,0) and select font 2
  // (cursor will move to next line automatically during printing with 'tft.println'
  //  or stay on the line is there is room for the text with tft.print)
  tft.setCursor(16, 16, 2);
  // Set the font colour to be white with a black background, set text size multiplier to 1
  tft.setTextColor(TFT_WHITE);  tft.setTextSize(1);
  // We can now plot text on screen using the "print" class
  tft.println("This is a temple display FPS");

  
   // Set the font colour to be blue with no background, set to font 4
  tft.setCursor(16, 32, 4);
  tft.setTextColor(TFT_YELLOW);  tft.setTextSize(2);
  tft.print("FPS:"); tft.println((int)elapsed_time, DEC); 
}