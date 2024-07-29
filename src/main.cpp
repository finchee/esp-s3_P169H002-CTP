#include <Arduino.h>
#include <TFT_eSPI.h>
#include <lvgl.h>
#include "User_Setup.h"
#include "lv_conf.h"
#include "demos/widgets/lv_demo_widgets.h"

/*Set to your screen resolution*/
static const uint16_t screenWidth  = 240;
static const uint16_t screenHeight = 280;

/*LVGL draw into this buffer, 1/10 screen size usually works well. The size is in bytes*/
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[ screenWidth * screenHeight / 10 ];

TFT_eSPI tft = TFT_eSPI();

// put function declarations here:
void TFT_eSPI_setup(void);
void get_FPS(void);
void Screen_display(void);
void my_disp_flush( lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p );

unsigned long start_millis = 0;
int elapsed_time = 0;

void setup() {
  // put your setup code here, to run once:
  TFT_eSPI_setup();

  Serial.begin( 115200 ); /* prepare for possible serial debug */

  String LVGL_Arduino = "Hello Arduino! ";
  LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();

  Serial.println( LVGL_Arduino );
  Serial.println( "I am LVGL_Arduino" );

  lv_init();

#if LV_USE_LOG != 0
    lv_log_register_print_cb( my_print ); /* register print function for debugging */
#endif

    tft.begin();          /* TFT init */
    tft.setRotation( 0 ); /* Landscape orientation, flipped */

    /*Set the touchscreen calibration data,
     the actual data for your display can be acquired using
     the Generic -> Touch_calibrate example from the TFT_eSPI library*/
    uint16_t calData[5] = { 275, 3620, 264, 3532, 1 };
    tft.setTouch( calData );

    lv_disp_draw_buf_init( &draw_buf, buf, NULL, screenWidth * screenHeight / 10 );

    /*Initialize the display*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init( &disp_drv );
    /*Change the following line to your display resolution*/
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register( &disp_drv );

    /*Initialize the (dummy) input device driver*/
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init( &indev_drv );
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    // indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register( &indev_drv );
  
    /* Create simple label */
    lv_obj_t *label = lv_label_create( lv_scr_act() );
    lv_label_set_text( label, "Hello Ardino and LVGL!");
    lv_obj_align( label, LV_ALIGN_CENTER, 0, 0 );
 
    /* Try an example. See all the examples 
     * online: https://docs.lvgl.io/master/examples.html
     * source codes: https://github.com/lvgl/lvgl/tree/e7f88efa5853128bf871dde335c0ca8da9eb7731/examples */
    //  lv_example_btn_1();
   
     /*Or try out a demo. Don't forget to enable the demos in lv_conf.h. E.g. LV_USE_DEMOS_WIDGETS*/
    // lv_demo_widgets();               
    // lv_demo_benchmark();          
    // lv_demo_keypad_encoder();     
    // lv_demo_music();              
    // lv_demo_printer();
    // lv_demo_stress();
    
    Serial.println( "Setup done" );
}

void loop() {
  // get_FPS();
  // Screen_display();

  lv_task_handler(); /* let the GUI do its work */
  delay(5); /* let this time pass */
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

/* LVGL calls it when a rendered image needs to copied to the display*/
void my_disp_flush( lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p )
{
    uint32_t w = ( area->x2 - area->x1 + 1 );
    uint32_t h = ( area->y2 - area->y1 + 1 );

    tft.startWrite();
    tft.setAddrWindow( area->x1, area->y1, w, h );
    tft.pushColors( ( uint16_t * )&color_p->full, w * h, true );
    tft.endWrite();

    lv_disp_flush_ready( disp_drv );
}