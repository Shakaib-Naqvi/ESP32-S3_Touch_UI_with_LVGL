#include <Arduino.h>
#include <Wire.h>
#define USER_SETUP_LOADED  // Verhindert, dass die Standardkonfiguration geladen wird
#include "User_Setup.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <lvgl.h>
#include "TFT_eSPI.h"
#include "CST816S.h"
#include "ui.h"
#include <esp_system.h>  // CPU-Frequenzeinstellungen


// Display und Touchscreen-Objekte
TFT_eSPI tft = TFT_eSPI();
CST816S cst816s(TOUCH_SDA, TOUCH_SCL, TOUCH_INT, TOUCH_RST);

// LVGL Display-Buffer
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf1[TFT_WIDTH * 10];

// LVGL Display-Flush Funktion
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, area->x2 - area->x1 + 1, area->y2 - area->y1 + 1);
    tft.pushColors((uint16_t*)&color_p->full, (area->x2 - area->x1 + 1) * (area->y2 - area->y1 + 1), true);
    tft.endWrite();
    lv_disp_flush_ready(disp);
}

// LVGL Touch-Input Funktion
void my_touch_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data) {
    uint16_t touchX, touchY;
    uint8_t gesture;
    bool touched = cst816s.getTouch(&touchX, &touchY, &gesture);
    if (!touched) {
        data->state = LV_INDEV_STATE_REL;
    } else {
        data->state = LV_INDEV_STATE_PR;
        data->point.x = touchX;
        data->point.y = touchY;
    }
}

void setup() {
    Serial.begin(115200);
    tft.begin();
    pinMode(TFT_RST, OUTPUT);
digitalWrite(TFT_RST, LOW);
delay(100);
digitalWrite(TFT_RST, HIGH);
delay(100);

    tft.setRotation(0);

    // TEST: Display-Farben prüfen
    tft.fillScreen(TFT_RED);
    delay(1000);
    tft.fillScreen(TFT_GREEN);
    delay(1000);
    tft.fillScreen(TFT_BLUE);
    delay(1000);

    lv_init();  // Jetzt erst LVGL starten

    lv_disp_draw_buf_init(&draw_buf, buf1, NULL, TFT_WIDTH * 10);

    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = TFT_WIDTH;
    disp_drv.ver_res = TFT_HEIGHT;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touch_read;
    lv_indev_drv_register(&indev_drv);

    ui_init();  // SquareLine Studio UI Initialisierung
}

void loop() {
    lv_timer_handler();
    delay(5);
}
