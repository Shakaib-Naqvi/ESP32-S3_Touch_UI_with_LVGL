#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <lvgl.h>
#include <TFT_eSPI.h>
#include "CST816D.h"
#include "ui.h"  // Include your SLS generated file
#include <esp_system.h>  // Include for setting CPU frequency

TFT_eSPI tft = TFT_eSPI();
CST816D cst816d(4, 5, 1, 0);  // Pin configuration that previously worked

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 240

BLEServer *pServer = NULL;
BLECharacteristic *pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;

#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

// Increase the size of the buffer to hold more lines for smoother drawing
static lv_color_t buf[SCREEN_WIDTH * 20]; // Increased buffer
static lv_disp_draw_buf_t draw_buf;

void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, area->x2 - area->x1 + 1, area->y2 - area->y1 + 1);
    tft.pushColors(&color_p->full, (area->x2 - area->x1 + 1) * (area->y2 - area->y1 + 1), true);
    tft.endWrite();
    lv_disp_flush_ready(disp);
}

void my_touch_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data) {
    uint16_t touchX = 0, touchY = 0;
    uint8_t gesture = 0;

    bool touched = cst816d.getTouch(&touchX, &touchY, &gesture);

    if (!touched) {
        data->state = LV_INDEV_STATE_REL;
    } else {
        data->state = LV_INDEV_STATE_PR;
        data->point.x = touchX;
        data->point.y = touchY;
    }
}

// Function to set the pointer position based on slider value
void setPointerPosition(int value) {
    int angle = map(value, 0, 100, 0, 180);  // Adjust as necessary
    lv_img_set_angle(ui_Screen2_Image2, angle * 10);
}

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        deviceConnected = true;
    }

    void onDisconnect(BLEServer* pServer) {
        deviceConnected = false;
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        std::string rxValue = pCharacteristic->getValue();
        if (rxValue.length() > 0) {
            int sliderValue = atoi(rxValue.c_str());
            setPointerPosition(sliderValue);
        }
    }
};

void setup() {
    setCpuFrequencyMhz(240);
    pinMode(3, OUTPUT);      // GPIO3 for Backlight
    digitalWrite(3, HIGH);   // Turn on the backlight

    // Setup BLE
    BLEDevice::init("DrSnuggles");
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    BLEService *pService = pServer->createService(SERVICE_UUID);
    pTxCharacteristic = pService->createCharacteristic(
                        CHARACTERISTIC_UUID_TX,
                        BLECharacteristic::PROPERTY_NOTIFY
                      );
    pTxCharacteristic->addDescriptor(new BLE2902());
    
    BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(
                        CHARACTERISTIC_UUID_RX,
                        BLECharacteristic::PROPERTY_WRITE
                      );
    pRxCharacteristic->setCallbacks(new MyCallbacks());

    pService->start();
    pServer->getAdvertising()->start();

    tft.begin();
    tft.setRotation(0);
    tft.fillScreen(TFT_BLACK);

    lv_init();
    lv_disp_draw_buf_init(&draw_buf, buf, NULL, SCREEN_WIDTH * 20);

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    disp_drv.hor_res = SCREEN_WIDTH;
    disp_drv.ver_res = SCREEN_HEIGHT;
    lv_disp_drv_register(&disp_drv);

    cst816d.begin();

    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touch_read;
    lv_indev_drv_register(&indev_drv);

    ui_init();  // Initialize UI from SLS
}

void loop() {
    lv_task_handler();
    lv_tick_inc(2);
    delay(2);

    // Manage BLE connection status
    if (!deviceConnected && oldDeviceConnected) {
        delay(500);
        pServer->startAdvertising();
        oldDeviceConnected = deviceConnected;
    }
    
    if (deviceConnected && !oldDeviceConnected) {
        oldDeviceConnected = deviceConnected;
    }
}
