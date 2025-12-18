#line 1 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover_2/DougFoster_Ghost_Rover_2.ino"
/**
 * ***********************************
 *      Ghost Rover 2.
 * ***********************************
 * 
 * Now you see me, now you don't. But you know exactly where I was.
 *
 * @author   D. Foster <doug@dougfoster.me>.
 * @since    0.7.0 [2025-09-28-11:15am] Version 2.
 * @link     http://dougfoster.me.
 * @link     https://www.build-electronic-circuits.com/arduino-laser-module-ky-008/.
 *
 * ===================================
 *      Comments.
 * ===================================
 * 
 * --- Description & operation. ---
 *     -- Primary use is ...
 *
 * --- Rover, major components. ---
 *     -- primary MCU        https://www.sparkfun.com/sparkfun-thing-plus-esp32-s3.html (SparkFun Thing Plus - ESP32-S3).
 *        -- micro SD card.  https://www.amazon.com/dp/B0BDYVC5TD (SanDisk 128GB ImageMate microSDXC UHS-1 - Up to 140MB/s).
 *     -- secondary MCU      https://www.amazon.com/dp/B0D2NKVB34 (Seeed Studio XIAO ESP32C6).
 *     -- GNSS               https://www.sparkfun.com/sparkfun-gps-rtk-sma-breakout-zed-f9p-qwiic.html (SparkFun GPS-RTK-SMA Breakout - ZED-F9P (Qwiic)).
 *     -- HC-12 RF radio     https://www.amazon.com/dp/B01MYTE1XR (HiLetgo HC-12 433Mhz SI4438).
 *     -- laser pointer      
 * 
 * --- Base station, major components. ---
 *     -- base station     https://www.sparkfun.com/sparkfun-rtk-evk.html (SparkFun RTK EVK).
 *     -- RTCM relay MCU   https://www.sparkfun.com/sparkfun-thing-plus-esp32-s3.html (SparkFun Thing Plus - ESP32-S3).
 *     -- HC-12 RF radio   https://www.amazon.com/dp/B01MYTE1XR (HiLetgo HC-12 433Mhz SI4438).
 *
 * --- Other components. ---
 *     -- Rover, GNSS antenna. --
 *        - GNSS antenna (L1/L2/L5, TNC-F)             https://www.sparkfun.com/gnss-multi-band-l1-l2-l5-surveying-antenna-tnc-spk6618h.html.
 *        - adapter (TNC-M to SMA-M)                   https://www.amazon.com/dp/B0BGPJP3J3.
 *        - adapter (SMA-M to SMA-F)                   https://www.amazon.com/dp/B00VHAZ0KW.
 *        - cable (SMA-F bulkhead to SMA-M, 6" RG316)  https://www.amazon.com/dp/B081BHHPHQ.
 *     -- Rover, RF radio. --
 *        - radio (433.4-473.0 MHz, 100mW, U.FL)       https://www.amazon.com/dp/B01MYTE1XR (HiLetgo HC-12 433Mhz SI4438).
 *        - antenna (UHF 400-960 MHz, BNC-M)           https://www.amazon.com/dp/B07R4PGZK3.
 *        - cable (BNC-F bulkhead to U.FL, 8" RG178)   https://www.amazon.com/dp/B098HX6NFH.
 *     -- Rover, Misc. -- 
 *        - I2C Qwiic cable kit                        https://www.amazon.com/dp/B08HQ1VSVL.
 *        - 26 AWG stranded wire                       https://www.amazon.com/dp/B089CQJHQC.
 *        - push button switch (12mm latching)         https://www.amazon.com/dp/B0CGTXMLKL.
 *        - push button switch (7mm momentary)         https://www.amazon.com/dp/B07RV1D98T.
 *        - power switch (15mm on/off latching toggle) https://www.amazon.com/dp/B09XMDXKTR.
 *        - LEDs (5mm)                                 https://www.amazon.com/dp/B0739RYXVC.
 *        - LED covers (5mm LED bulb socket)           https://www.amazon.com/dp/B07CQ6TH14.
 *        - battery (+5V 2.4A max, 8000 mAh)           https://www.walmart.com/ip/onn-8000mAh-Portable-Battery-Power-Bank-with-USB-A-to-C-Charging-Cable-LED-Indicator-Black/5266111773
 *        - enclosure (Pelican Micro Case 1040)        https://www.rei.com/product/778220/pelican-micro-case-1040-with-carabiner.
 *        - pistol grip handle                         https://www.amazon.com/dp/B01FUEXLGU.
 *        - tripod legs                                https://www.amazon.com/dp/B07GST1C2Z.
 *        - magnetic phone/tripod mount                https://www.amazon.com/dp/B0D21RP69C.
 *        - GNSS antenna thread adapter                https://www.sparkfun.com/antenna-thread-adapter-1-4in-to-5-8in.html
 *        - other: nuts, bolts, 1/4-10 bolt, washers, USB-A power cable, heat shrink tubing.
 *     -- Base, Misc. --
 *        - mini tripod                                https://www.amazon.com/dp/B0CQ6WTRW6.
 *        - laser pointer                              https://www.petsmart.com/cat/toys/interactive-and-electronic/whisker-city-thrills-and-chills-laser-cat-toy-84577.html.
 *        - battery/smartphone holder                  https://www.amazon.com/dp/B07S8TTH34
 *        - 3.5mm/ 0.14 in. pitch 10 pin pluggable PCB screw terminal block connector (female)  https://www.amazon.com/dp/B0BPHLZ8XN.
 *        - same as rover: battery, GNSS antenna/adapters/cable, HC-12 radio/cabl/antenna, LED/holders, primary MCU
 *        - other: nuts, 1/4" thread rod, 1.25" round bubble level.
 * 
 * --- Misc. references. ---
 *     -- EVK         https://docs.sparkfun.com/SparkFun_RTK_EVK/introduction/.
 *     -- HC-12       https://www.elecrow.com/download/HC-12.pdf.
 *     -- PyGPSClient https://github.com/semuconsulting/PyGPSClient.
 *     -- SW Maps     https://aviyaantech.com/swmaps/.
 *     -- RTK         https://learn.sparkfun.com/tutorials/what-is-gps-rtk/all.
 *     -- NMEA        https://cdn.sparkfun.com/assets/a/3/2/f/a/NMEA_Reference_Manual-Rev2.1-Dec07.pdf.
 *                    https://swairlearn.bluecover.pt/nmea_analyser.
 *     -- SparkFun    https://learn.sparkfun.com/tutorials/tags/gnss.
 *     -- Pin config  https://roboticsbackend.com/arduino-uno-pins-a-complete-practical-guide/.
 * 
 * --- Dev environment. ---
 *     -- IDE        VS Code & Arduino Maker Workshop 1.0.5 extension.
 *     -- GitHub     https://github.com/doug-foster/DougFoster_Ghost_Rover/.
 *     -- Platform   https://github.com/espressif/arduino-esp32/releases/tag/3.3.0 (Arduino Release v3.3.0 based on ESP-IDF v5.5.0).
 *     -- Libraries  (see // Include libraries. below ).
 *     -- Build      Arduino CLI v1.3.1 (2025-08-28T13:51:41Z).
 * 
 * --- Caveats. ---
 *     -- SoftwareSerial library is not supported on ESP32-S3 (does work on ESP32-C6).
 *     -- 0.5.1 -> 0.6.1 builds: Moved BLE relay from primary MCU to secondary MCU since BleSerial library is a space pig.
 *
 * --- TODO: ---
 *     1. Add WiFi.
 *     2. Add web server for remote data display, operation, and config.
 *     3. Add stand-alone mode using PP.
 *     4. Add "vector to coordinates" (navigate to a location) function.
 *
 * --- Code organization. ---
 *     -- Include libraries.
 *     -- Global vars: define vars, set constants, declarations.
 *     -- Functions: init, config, begin, start, check, display, callback, operation, tasks, test.
 *     -- Setup.
 *     -- Loop.
 */

// ===================================
//      Include libraries.
// ===================================

// -- Reference. --
// Arduino   https://docs.arduino.cc/libraries/.
// ESP32     https://docs.espressif.com/projects/arduino-esp32/en/latest/libraries.html.
// LittleFS  https://randomnerdtutorials.com/arduino-ide-2-install-esp32-littlefs/.

// -- ESP32 core board libraries. --
#include <Arduino.h>                                       // https://github.com/espressif/arduino-esp32.
#include <WiFi.h>                                          // https://github.com/espressif/arduino-esp32/tree/master/libraries/WiFi.
// #include <SD.h>                                            // https://github.com/espressif/arduino-esp32/tree/master/libraries/SD.
#include <SPI.h>                                           // https://github.com/espressif/arduino-esp32/tree/master/libraries/SPI.
#include <Wire.h>                                          // https://github.com/espressif/arduino-esp32/blob/master/libraries/Wire/src/Wire.h
#include <HardwareSerial.h>                                // https://github.com/espressif/arduino-esp32/blob/master/cores/esp32/HardwareSerial.cpp.
#include <esp_system.h>                                    // https://github.com/pycom/pycom-esp-idf.
                  
// -- Additional libraries. --                  
#include <AsyncTCP.h>                                      // https://github.com/ESP32Async/AsyncTCP (Async TCP - v3.4.8).
#include <ESPAsyncWebServer.h>                             // https://github.com/ESP32Async/ESPAsyncWebServer (ESP Async WebServer - v3.8.1).
#include <Arduino_JSON.h>                                  // https://github.com/arduino-libraries/Arduino_JSON (Arduino_JSON - v0.2.0).
#include <SparkFun_MAX1704x_Fuel_Gauge_Arduino_Library.h>  // https://github.com/sparkfun/SparkFun_MAX1704x_Fuel_Gauge_Arduino_Library (SparkFun MAX1704x Fuel Gauge Arduino Library - 1.0.4)
#include <SparkFun_u-blox_GNSS_v3.h>                       // https://github.com/sparkfun/SparkFun_u-blox_GNSS_v3 (SparkFun u-blox GNSS v3 - 3.1.9).
#include <SdFat.h>                                         // https://github.com/espressif/arduino-esp32/tree/master/libraries/SD.


// ===================================
//      Global vars.
// ===================================

// -- Version. --
const char BUILD_DATE[]   = "2025-09-28-13:15";     // 24hr format, need to fit max (16) characters.
const char MAJOR_VERSION  = '0';
const char MINOR_VERSION  = '7';
const char PATCH_VERSION  = '0';
const char NAME[]         = "Ghost Rover 2";

// -- Communication port usage. --
// Serial monitor (USB).
//
// Serial 0 (UART0 RX).
// Serial 0 (UART0 TX).
//
// Serial 1 (UART1 RX).
// Serial 1 (UART1 TX).
//
// Serial 2 (UART2 RX).
// Serial 2 (UART2 TX).
//
// I2C-1 bus {Qwicc}: ESP32-S3 <-> RTK-SMA GNSS (UBX).

// -- Pin (pth) definitions. --
const uint8_t LED_STATUS    = 0;                // Thing Plus status LED.
const uint8_t HC12_RX       = 43;               // See above.
const uint8_t HC12_TX       = 44;               // See above.
const uint8_t HC12_SET      = 42;               // ESP32-S3 Thing+ <-> HC-12 SET             {blue wire}.

const uint8_t I2C_SDA       =  8;               // ESP32-S3 Thing+ <-> I2C-1 data pin        {Qwiic} - SDA.
const uint8_t I2C_SCL       =  9;               // ESP32-S3 Thing+ <-> I2C-1 clock pin       {Qwiic} - SCL.

// const uint8_t PTH_S0_TX =  1;                // Default S0 (UART0, USB) TX - not used.
// const uint8_t PTH_S0_RX =  3;                // Default S0 (UART0, USB) RX - not used.

// -- Serial USB monitor. --
const  uint32_t SERIAL_MON_SPEED = 115200;      // Serial USB monitor speed.
       char     inputChar;                      // Input/output character.

// -- Serial 0. --
// -- Serial 1. --
// -- Serial 2. --
// -- I2C-1. --

// -- WiFi. --
// const char* ssid = "C&D";
// const char* password = "wewerewatchinghousetonight2!";
const char* ssid = "Doug";
const char* password = "snark217$";
IPAddress staticIP(172, 20, 10, 2);  // Verizon cell phone hot spot.

// -- microSD card reader. --
SdFat32 sd;

// -- Web. --
AsyncWebServer server(80);  // AsyncWebServer object on port 80.
AsyncWebSocket ws("/ws");   // WebSocket object.

// Json Variable to Hold Sensor Readings.
// JSONVar readings;

// Timer variables.
// unsigned long lastTime = 0;
// unsigned long timerDelay = 30000;

// -- Timing. --
const TickType_t LED_TIME_FLASH_ON  = 100/portTICK_PERIOD_MS;  // Time (ms).
const TickType_t LED_TIME_FLASH_OFF = 1000/portTICK_PERIOD_MS;  // Time (ms).

// -- Task handles. --
TaskHandle_t loopLEDtaskHandle;               // Loop LED task handle.

// -- Lipo battery. --
SFE_MAX1704X lipo(MAX1704X_MAX17048);
double voltage = 0;     // LiPo voltage.
double soc     = 0;     // LiPo state-of-charge (SOC)
bool   alert;           // Alert triggered?

// -- GNSS. --
// -- I/O. --
// -- Operation. --
// -- Commands. --
// -- State. --
// -- Declarations. --                          // Eliminate compiler scope error due to definition order.

// -- Test. --

// ===================================
//             Functions
// ===================================

// --- Init. ---

/**
 * ------------------------------------------------
 *      Initialize global vars.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  0.7.0 [2025-09-28-11:15am] Version 2.
 */
#line 232 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover_2/DougFoster_Ghost_Rover_2.ino"
void initVars();
#line 258 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover_2/DougFoster_Ghost_Rover_2.ino"
void initPins();
#line 281 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover_2/DougFoster_Ghost_Rover_2.ino"
void startSerialMonitor();
#line 299 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover_2/DougFoster_Ghost_Rover_2.ino"
void startSerialInterfaces();
#line 310 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover_2/DougFoster_Ghost_Rover_2.ino"
void startI2C();
#line 327 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover_2/DougFoster_Ghost_Rover_2.ino"
void startLIPO();
#line 348 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover_2/DougFoster_Ghost_Rover_2.ino"
void startWiFi();
#line 370 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover_2/DougFoster_Ghost_Rover_2.ino"
void startSD();
#line 387 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover_2/DougFoster_Ghost_Rover_2.ino"
void startWebServer();
#line 404 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover_2/DougFoster_Ghost_Rover_2.ino"
void startWebSocket();
#line 421 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover_2/DougFoster_Ghost_Rover_2.ino"
void startTasks();
#line 441 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover_2/DougFoster_Ghost_Rover_2.ino"
void loopLEDtask(void * pvParameters);
#line 468 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover_2/DougFoster_Ghost_Rover_2.ino"
void testSD();
#line 492 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover_2/DougFoster_Ghost_Rover_2.ino"
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);
#line 517 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover_2/DougFoster_Ghost_Rover_2.ino"
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len);
#line 566 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover_2/DougFoster_Ghost_Rover_2.ino"
void checkLIPO();
#line 578 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover_2/DougFoster_Ghost_Rover_2.ino"
void setup();
#line 597 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover_2/DougFoster_Ghost_Rover_2.ino"
void loop();
#line 232 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover_2/DougFoster_Ghost_Rover_2.ino"
void initVars() {

    Serial.println("setup() running.");
    Serial.print("Initialize global vars");

    // -- Serial USB monitor. --
    inputChar = '\0';

    // -- Timing. --
    // -- GNSS. --
    // -- I/O. --
    // -- Operation. --
    // -- Commands. --
    // -- State. --

    Serial.println(".");
}

/**
 * ------------------------------------------------
 *      Initialize pin modes & pin values.
 * ------------------------------------------------
 *
 * @return void No output is returned.
 * @since  0.7.0 [2025-09-28-11:15am]. Version 2.
 */
void initPins() {

    // -- Pin modes. --
    Serial.print("Config pins");
    pinMode(0, OUTPUT);
    pinMode(HC12_SET, OUTPUT);          // HC-12 - set pin for AT command mode.

    // -- Pin values. --
    digitalWrite(HC12_SET, HIGH);       // HC-12 - initially set pin for transparent mode.

    Serial.println(".");
}

// --- Start. ---

/**
 * ------------------------------------------------
 *      Start serial USB monitor.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  0.7.0 [2025-09-28-11:15am]. Version 2.
 */
void startSerialMonitor() {

    Serial.begin(SERIAL_MON_SPEED);
    delay(1000);
    Serial.printf("\nProject: %s, Version: %c.%c.%c, Build date: %s\n", NAME, MAJOR_VERSION, MINOR_VERSION, PATCH_VERSION, BUILD_DATE);
    Serial.printf("\nSerial USB monitor started @ %i bps.\n", SERIAL_MON_SPEED);
}

/**
 * ------------------------------------------------
 *      Start serial interfaces.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  0.7.0 [2025-09-28-11:15am]. Version 2.
 * @link   https://github.com/G6EJD/ESP32-Using-Hardware-Serial-Ports.
 * @link   https://randomnerdtutorials.com/esp32-uart-communication-serial-arduino/#esp32-custom-uart-pins.
 */
void startSerialInterfaces() {
}

/**
 * ------------------------------------------------
 *      Start I2C interface.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  0.7.0 [2025-09-28-11:15am]. Version 2.
 */
void startI2C() {

    if (Wire.begin() == false) {
        Serial.println("Start I2C failed. Freezing.");
        while (true);
    }
    Serial.println("I2C started.");
}

/**
 * ------------------------------------------------
 *      Start LIPO I2C interface.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  0.7.0 [2025-09-28-11:15am]. Version 2.
 */
void startLIPO() {

    // lipo.enableDebugging();
    if (lipo.begin() == false) { // Default wire port.
        Serial.println("LIPO MAX17048 not detected. Freezing.");
        while (true);
    }
    
    lipo.quickStart();      // Restart for a more accurate initial SOC guess.
    lipo.setThreshold(20);  // Interupt threshold (1% - 32%) for SOC alert.
    Serial.println("LIPO started.");
}

/**
 * ------------------------------------------------
 *      Start WiFi.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  0.7.0 [2025-09-28-11:15am]. Version 2.
 */
void startWiFi() {

    WiFi.mode(WIFI_STA);
    WiFi.config (staticIP);
    WiFi.begin(ssid, password);
    Serial.println("WiFi started.");
    Serial.printf("Looking for SSID = %s\n.", ssid);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print('.');
        delay(1000);  // Try again.
    }
    Serial.printf(" Connected to %s, IP = %s.\n", ssid, WiFi.localIP().toString().c_str());
}

/**
 * ------------------------------------------------
 *      Start microSD card reader.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  0.7.0 [2025-09-28-11:15am]. Version 2.
 */
void startSD() {

  if (!sd.begin()) {
    Serial.println("microSD card reader not started. Freezing.");
    while (true);
  }
  Serial.println("microSD card reader sarted.");  // SanDisk 128GB ImageMate microSDXC UHS-1 - Up to 140MB/s.
}

/**
 * ------------------------------------------------
 *      Start web server.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  0.7.0 [2025-09-28-11:15am]. Version 2.
 */
void startWebServer() {
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) { // Set root.
        request->send(sd, "/index.html", "text/html");
    });
    server.serveStatic("/", sd, "/");
    server.begin();
    Serial.println("Web server started.");
}

/**
 * ------------------------------------------------
 *      Start WebSocket.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  0.7.0 [2025-09-28-11:15am]. Version 2.
 */
void startWebSocket() {

    ws.onEvent(onEvent);
    server.addHandler(&ws);
    Serial.println("WebSocket started.");
}

/**
 * ------------------------------------------------
 *      Start tasks.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  0.7.0 [2025-09-28-11:15am]. Version 2.
 * @see    Global vars: Task handles.
 * @link   https://www.freertos.org/Documentation/02-Kernel/04-API-references/01-Task-creation/01-xTaskCreate.
 */
void startTasks() {

    xTaskCreate(loopLEDtask,      "LOOP_LED_task",         2048, NULL, 2, &loopLEDtaskHandle);  // Create task.
    // vTaskSuspend(loopLEDtaskHandle);                                                         // Suspend task.
    Serial.println("Tasks started.");
}

// --- Utility. ---

/**
 * ------------------------------------------------
 *      Loop active task.
 * ------------------------------------------------
 *
 * @param  void  * pvParameters Pointer to task parameters.
 * @return void  No output is returned.
 * @since  0.7.0 [2025-09-28-11:15am]. Version 2.
 * @see    startTasks().
 * @link   https://www.freertos.org/Documentation/02-Kernel/04-API-references/02-Task-control/06-vTaskSuspend.
 */
void loopLEDtask(void * pvParameters) {

    while(true) {
        digitalWrite(LED_STATUS, HIGH);         // LED on.
        vTaskDelay(LED_TIME_FLASH_ON);          // LED remains on.
        digitalWrite(LED_STATUS, LOW);          // LED off.
        vTaskDelay(LED_TIME_FLASH_OFF);         // LED remains off (ms).
    }
}

// Get Sensor Readings and return JSON object
// String getSensorReadings(){
//   readings["temperature"] = String(bme.readTemperature());
//   readings["humidity"] =  String(bme.readHumidity());
//   readings["pressure"] = String(bme.readPressure()/100.0F);
//   String jsonString = JSON.stringify(readings);
//   return jsonString;
// }

/**
 * ------------------------------------------------
 *      Test built-in microSD card reader. Read & print a file.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  0.7.0 [2025-09-28-11:15am]. Version 2.
 */
void testSD() {
    Serial.println("Testing built-in microSD card reader. Open file /index.html.");
    File32 file = sd.open("/index.html", "r");
    if (file == false){
        Serial.println("Failed. Freezing.");
        while (true);
    }
    Serial.println("File Content:");
    while (file.available()) {
        Serial.write(file.read());
    }
    file.close();
}

// --- Messaging. ---

/**
 * ------------------------------------------------
 *      WebSocket event handler.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  0.7.0 [2025-09-28-11:15am]. Version 2.
 */
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    switch (type) {
        case WS_EVT_CONNECT:
            Serial.printf("WebSocket client #%u connected from %s.\n", client->id(), client->remoteIP().toString().c_str());
            break;
        case WS_EVT_DISCONNECT:
            Serial.printf("WebSocket client #%u disconnected.\n", client->id());
            break;
      case WS_EVT_DATA:
            handleWebSocketMessage(arg, data, len);
            break;
      case WS_EVT_PONG:
      case WS_EVT_ERROR:
            break;
    }
}

/**
 * ------------------------------------------------
 *      WebSocket message handler.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  0.7.0 [2025-09-28-11:15am]. Version 2.
 */
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
        data[len] = 0;
        String message = (char*)data;
        // Check if the message is "getReadings"
        if (strcmp((char*)data, "getReadings") == 0) {
        //if it is, send current sensor readings
        //   String sensorReadings = getSensorReadings();
        //   Serial.print(sensorReadings);
        //   notifyClients(sensorReadings);
        }
    }
}

/**
 * ------------------------------------------------
 *      Notify clients.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  0.7.0 [2025-09-28-11:15am]. Version 2.
 */
// void notifyClients(String sensorReadings) {
//     ws.textAll(sensorReadings);
// }

// --- Check. ---

/**
 * ------------------------------------------------
 *      Check serial monitor (USB) for input.
 * ------------------------------------------------
 *
 * @return void No output is returned.
 * @return void  No output is returned.
 * @since  0.7.0 [2025-09-28-11:15am]. Version 2.
 */
void checkSerialMonitor(char print = ' ') {
}

/**
 * ------------------------------------------------
 *      Check LIPO status.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  0.7.0 [2025-09-28-11:15am]. Version 2.
 */
void checkLIPO() {

    voltage = lipo.getVoltage();
    soc = lipo.getSOC();      // State of charge (e.g. 79%).
    alert = lipo.getAlert();  // 0 = alert not triggered, 1 = triggered.
    Serial.printf("LIPO: %.2f V, %.2f %, alert(%d)\n", voltage, soc, alert);
}

// ===================================
//               Setup.
// ===================================

void setup() {
    startSerialMonitor();               // Start Serial (USB) for monitor.
    initVars();                         // Initialize global vars.
    initPins();                         // Initialize pin modes & pin values.
    startSerialInterfaces();            // Start serial interfaces.
    startI2C();                         // Start I2C interface.
    startLIPO();                        // Start LIPO I2C interface.
    startWiFi();                        // Start WiFi.
    startSD();                          // Start built-in microSD card reader.
    testSD();                           // Test microSD card reader. Read & print a file.
    startWebServer();                   // Start web server.
    startWebSocket();                   // Start WebSocket.
    startTasks();                       // Start tasks.
}

// =================================== 
//               Loop.
// ===================================

void loop() {
    checkSerialMonitor();               // Check serial monitor (USB) for input.
    checkLIPO();                        // Check LIPO status.
    delay(1000);

//     if ((millis() - lastTime) > timerDelay) {
//     String sensorReadings = getSensorReadings();
//     Serial.print(sensorReadings);
//     notifyClients(sensorReadings);
//     lastTime = millis();
//   }
//   ws.cleanupClients();

}

