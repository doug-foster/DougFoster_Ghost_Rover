#line 1 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
/**
 * ***********************************
 *          Ghost Rover 3.
 * ***********************************
 * 
 * Now you see me, now you don't. But you know exactly where I was.
 *
 * @author   D. Foster <doug@dougfoster.me>.
 * @since    3.0.7 [2025-11-11-05:45pm].
 * @link     http://dougfoster.me.
 * @link     https://www.build-electronic-circuits.com/arduino-laser-module-ky-008/.
 *
 */

 /** TBD
 * 
 * ===================================
 *             Comments.
 * ===================================
 * 
 * --- Description & operation. ---
 *     -- Primary use is ...
 *
 * --- Rover, major components. ---
 *     -- primary MCU        https://www.sparkfun.com/sparkfun-thing-plus-esp32-c6.html (SparkFun Thing Plus - ESP32-C6).
 *        -- micro SD card.  https://www.amazon.com/dp/B0BDYVC5TD (SanDisk 128GB ImageMate microSDXC UHS-1 - Up to 140MB/s).
 *     -- secondary MCU      https://www.sparkfun.com/sparkfun-qwiic-pocket-development-board-esp32-c6.html (Sparkfun Qwiic Pocket Development Board - ESP32-C6 - I2C address 0x36).
 *     -- GNSS               https://www.sparkfun.com/sparkfun-gps-rtk-sma-breakout-zed-f9p-qwiic.html (SparkFun GPS-RTK-SMA Breakout - ZED-F9P (Qwiic) - I2C address 0x42).
 *     -- HC-12 RF radio     https://www.amazon.com/dp/B01MYTE1XR (HiLetgo HC-12 433Mhz SI4438).
 *     -- laser pointer      https://www.petsmart.com/cat/toys/interactive-and-electronic/whisker-city-thrills-and-chills-laser-cat-toy-84577.html.
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
 *     1. Add items to config page.
 *     2. Add debug/details page.
 *     3. Add stand-alone mode using PP.
 *     4. Add "vector to coordinates" (navigate to a location) function.
 *
 * --- Code organization. ---
 *     -- Include libraries
 *     -- Global vars
 *     -- Functions
 *     -- Setup.
 *     -- Loop.
 */

/**
 * ============================================================================
 *                          Include libraries.
 * ============================================================================
 *
 * @since 3.0.7     [2025-11-10-11:00am].
 * @link  Arduino   https://docs.arduino.cc/libraries/.
 * @link  ESP32     https://docs.espressif.com/projects/arduino-esp32/en/latest/libraries.html.
 * @link  LittleFS  https://randomnerdtutorials.com/arduino-ide-2-install-esp32-littlefs/.
 */

// --- Core. ---
#include <Arduino.h>                                       // https://github.com/espressif/arduino-esp32.
#include <WiFi.h>                                          // https://github.com/espressif/arduino-esp32/tree/master/libraries/WiFi.
#include <SD.h>                                            // https://github.com/espressif/arduino-esp32/tree/master/libraries/SD.
#include <FS.h>                                            // https://github.com/espressif/arduino-esp32/tree/master/libraries/FS.
#include <SPI.h>                                           // https://github.com/espressif/arduino-esp32/tree/master/libraries/SPI.
#include <Wire.h>                                          // https://github.com/espressif/arduino-esp32/blob/master/libraries/Wire/src/Wire.h.
#include <time.h>                                          // https://github.com/espressif/arduino-esp32/blob/master/cores/esp32/esp32-hal-time.c#L47.
#include <HardwareSerial.h>                                // https://github.com/espressif/arduino-esp32/blob/master/cores/esp32/HardwareSerial.cpp.
#include <esp_system.h>                                    // https://github.com/pycom/pycom-esp-idf.
#include <esp_chip_info.h>                                 // https://github.com/pycom/pycom-esp-idf.

// --- Additional. ---                  
#include <AsyncTCP.h>                                      // https://github.com/ESP32Async/AsyncTCP (Async TCP - v3.4.8).
#include <ESPAsyncWebServer.h>                             // https://github.com/ESP32Async/ESPAsyncWebServer (ESP Async WebServer - v3.8.1).
#include <ArduinoJson.h>                                   // https://github.com/bblanchon/ArduinoJson (ArduinoJSON - v7.4.2).
#include <SparkFun_MAX1704x_Fuel_Gauge_Arduino_Library.h>  // https://github.com/sparkfun/SparkFun_MAX1704x_Fuel_Gauge_Arduino_Library (SparkFun MAX1704x Fuel Gauge Arduino Library - 1.0.4)
#include <SparkFun_u-blox_GNSS_v3.h>                       // https://github.com/sparkfun/SparkFun_u-blox_GNSS_v3 (SparkFun u-blox GNSS v3 - 3.1.9).

/**
 * ============================================================================
 *                          Global vars.
 * ============================================================================
 *
 * @since 3.0.7 [2025-11-11-04:30pm].
 */

// --- Pin. --- 
// const uint8_t LED_STATUS   =  0;             // ESP-32-S3 Thing Plus - pin for status LED.
// const uint8_t I2C_SDA      =  8;             // ESP32-S3 Thing+ <-> I2C-1 data pin        {Qwiic} - SDA.
// const uint8_t I2C_SCL      =  9;             // ESP32-S3 Thing+ <-> I2C-1 clock pin       {Qwiic} - SCL.
const uint8_t SPI_SCK          = 19;            // ESP32-C6 Thing+ SPI serial clock
const uint8_t SPI_POCI         = 21;            // SPI Peripheral Out / Controller In (aka MISO) microSD SDO
const uint8_t SPI_PICO         = 20;            // SPI Peripheral In / Controller Out (aka MOSI) microSD SDI 
const uint8_t SPI_CS           = 18;            // SPI chip select
// const uint8_t HC12_RX      = 43;             // See above.
// const uint8_t HC12_TX      = 44;             // See above.
// const uint8_t HC12_SET     = 42;             // ESP32-S3 Thing+ <-> HC-12 SET             {blue wire}.
// const uint8_t PTH_S0_TX =  1;                // Default S0 (UART0, USB) TX - not used.
// const uint8_t PTH_S0_RX =  3;                // Default S0 (UART0, USB) RX - not used.

// --- LED. ---
enum WS2818_LED_COLOR {                         // WS2812 RGB STAT LED, pin=RGB_BUILTIN=IO23.
    OFF,                                        // 0.
    RED,                                        // 1.
    YELLOW,                                     // 2.
    GREEN,                                      // 3.
    BLUE                                        // 4.
} ws2818LedColor;
bool ws2818LedBlink;

// --- Battery. ---
SFE_MAX1704X lipo(MAX1704X_MAX17048);           // LIPO battery.

// --- Serial. --- 
// Serial 0 (UART0 RX).
// Serial 0 (UART0 TX).
// Serial 1 (UART1 RX).
// Serial 1 (UART1 TX).
// Serial 2 (UART2 RX).
// Serial 2 (UART2 TX).
// I2C.
const  uint32_t SERIAL_MON_SPEED = 115200;      // Serial USB monitor speed.
       char     inputChar;                      // Input/output character.

// --- WiFi. --- 
// const char* ssid = "C&D";                       // Home network.
// const char* password = "wewerewatchinghousetonight2!";
// IPAddress staticIP(192, 168, 4, 200);
const char* ssid = "Doug";                   // Verizon cell phone hot spot.
const char* password = "snark217$";
IPAddress staticIP(172, 20, 10, 2);
// const char* ssid = "Maules Poinr";           // Starlink Maules Point.
// const char* password = "HAS111@Heritage";
// IPAddress staticIP(192, 168, 1, 100);

// -- Timing. --
const TickType_t LED_TIME_FLASH_ON  = 100/portTICK_PERIOD_MS;       // Timer (ms) =  0.1 seconds.
const TickType_t LED_TIME_FLASH_OFF = 1000/portTICK_PERIOD_MS;      // Timer (ms) =  1.0 seconds.
const TickType_t SEND_NUMBERS_PAUSE = 500/portTICK_PERIOD_MS;       // Timer (ms) =  0.5 seconds.
const TickType_t SEND_BATTERY_PAUSE = 5000/portTICK_PERIOD_MS;      // Timer (ms) =  5.0 seconds.

// -- Task handles. --
TaskHandle_t loopStatusLEDtaskHandle;           // Task: Loop status LED.
TaskHandle_t sendPositionTaskHandle;            // Task: send position.
TaskHandle_t sendBatteryStatusTaskHandle;       // Task: send battery status.

// --- JSON. ---
JsonDocument jsonDocFromClient;                 // JSON document received from client.
JsonDocument jsonDocToClient;                   // JSON document sent to client.

// --- HTTP. --- 
AsyncWebServer httpServer(80);                  // HTTP AsyncWebServer object on port 80.
AsyncWebSocket ws("/ghostRover");               // HTTP WebSocket object.
uint8_t clientId;                               // HTTP WebSocket client ID #.

// --- General. ---
esp_chip_info_t chip_info;                      // Chip info.
char operMode[5];                               // Operation mode (rover, base).
char operUnits[7];                              // Operation units (meter, feet).
char buffer[256];                               // Utility buffer.

// --- Version. ---
const char BUILD_DATE[]   = "[2025-11-10-11:00am]";
const char MAJOR_VERSION  = '3';
const char MINOR_VERSION  = '0';
const char PATCH_VERSION  = '7';
const char NAME[]         = "Ghost Rover 3";

// --- Declaration. ---

// --- Test. ---

/**
 * ============================================================================
 *                              Functions.
 * ============================================================================
 */

/**
 * ------------------------------------------------
 *      Initialize global vars.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.7 [2025-11-07-12:30pm].
 * @see    setup().
 */
#line 245 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void initVars();
#line 266 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void initPins();
#line 282 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void startSerialMonitor();
#line 302 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void startSerialInterfaces();
#line 314 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void startI2C();
#line 331 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void startLIPO();
#line 351 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void startWiFi();
#line 371 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void startSPI();
#line 389 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void startSD();
#line 406 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void startWebSocketServer();
#line 422 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void startTasks();
#line 445 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void startLoop();
#line 473 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void chipInfo();
#line 488 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void testSD();
#line 510 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void loopStatusLEDtask(void * pvParameters);
#line 546 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void sendPositionTask(void * pvParameters);
#line 581 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void sendBatteryStatusTask(void * pvParameters);
#line 613 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void sdListFiles(char* output, size_t outputMaxSize, char* path);
#line 639 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void startHttpServer();
#line 684 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void handleFileUpload( AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final);
#line 723 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void onWebSocketEvent(AsyncWebSocket *httpServer, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);
#line 758 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len);
#line 870 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void scanI2C();
#line 911 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
float random_in_range(int min, int max);
#line 914 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
char * latitude();
#line 920 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
char * longitude();
#line 926 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
char * elevation();
#line 932 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
char * hac();
#line 938 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
char * vac();
#line 953 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void setup();
#line 978 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void loop();
#line 245 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void initVars() {
    Serial.print("Init global vars");
    inputChar = '\0';           // Serial USB monitor.
    ws2818LedColor = RED;       // Task - loop status indicator LED.
    ws2818LedBlink = false;     // Task - loop status indicator LED.
    clientId = 0;               // HTTP WebSocket client ID #.
    memset(buffer, '\0', sizeof(buffer));
    strcpy(operMode, "base");
    strcpy(operUnits, "meters");
    Serial.println(".");
}

/**
 * ------------------------------------------------
 *      Initialize pins.
 * ------------------------------------------------
 *
 * @return void No output is returned.
 * @since  3.0.3 [2025-10-13-01:00pm].
 * @see    setup().
 */
void initPins() {
    Serial.print("Config pins");
    // pinMode(0, OUTPUT);
    // pinMode(HC12_SET, OUTPUT);          // HC-12 - set pin for AT command mode.
    // digitalWrite(HC12_SET, HIGH);       // HC-12 - initially set pin for transparent mode.
    Serial.println(".");
}

/**
 * ------------------------------------------------
 *      Start serial USB monitor.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.3 [2025-10-13-01:00pm].
 */
void startSerialMonitor() {
    Serial.begin(SERIAL_MON_SPEED);
    delay(1000);
    Serial.printf("\n%s, Version: %c.%c.%c, Build date: %s\n", NAME, MAJOR_VERSION, MINOR_VERSION, PATCH_VERSION, BUILD_DATE);
    chipInfo();
    Serial.println("\nSetup() started.");
    Serial.printf("Serial USB monitor started @ %i bps.\n", SERIAL_MON_SPEED);
}

/**
 * ------------------------------------------------
 *      Start serial interfaces.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.3 [2025-10-13-01:00pm].
 * @link   https://github.com/G6EJD/ESP32-Using-Hardware-Serial-Ports.
 * @link   https://randomnerdtutorials.com/esp32-uart-communication-serial-arduino/#esp32-custom-uart-pins.
 * @see    setup().
 */
void startSerialInterfaces() {
}

/**
 * ------------------------------------------------
 *      Start I2C interface.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.3 [2025-10-13-01:00pm].
 * @see    setup().
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
 * @since  3.0.7 [2025-11-09-10:15pm].
 * @see    setup().
 */
void startLIPO() {
    lipo.enableDebugging();
    if (lipo.begin() == false) {    // Uses default I2C (wire) port.
        Serial.println("LIPO not started. MAX17048 not detected.");
    } else {
        lipo.quickStart();          // Restart for a more accurate initial SOC guess.
        Serial.println("LIPO started.");
        delay(1000);
    }
}

/**
 * ------------------------------------------------
 *      Start WiFi.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.3 [2025-10-13-03:15pm].
 * @see    setup().
 */
void startWiFi() {
    WiFi.mode(WIFI_STA);        // Station mode.
    WiFi.config (staticIP);
    WiFi.begin(ssid, password);
    Serial.printf("WiFi started.\nConnecting to SSID = %s ", ssid);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print('.');
        delay(1000);            // Try again.
    }
    Serial.printf("\nConnected, IP = %s .\n", WiFi.localIP().toString().c_str());
}
/**
 * ------------------------------------------------
 *      Start SPI for microSD reader.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.3 [2025-10-13-01:00pm].
 * @see    setup().
 */
void startSPI() {
    if (!SPI.begin(SPI_SCK, SPI_POCI, SPI_PICO, SPI_CS)) {
        Serial.println("SPI not started. Freezing.");
        while (true);
    }
    delay(1000);
    Serial.println("SPI started.");  // SanDisk 128GB ImageMate microSDXC UHS-1 - Up to 140MB/s.
}

/**
 * ------------------------------------------------
 *      Start microSD card reader.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.3 [2025-10-13-01:00pm].
 * @see    setup().
 */
void startSD() {
    if (!SD.begin(SPI_CS)) {
        Serial.println("SD card not started. Freezing.");
        while (true);
    }
    Serial.println("SD card started.");  // SanDisk 128GB ImageMate microSDXC UHS-1 - Up to 140MB/s.
}

/**
 * ------------------------------------------------
 *      Start WebSocket.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.3 [2025-10-13-01:00pm].
 * @see    setup().
 */
void startWebSocketServer() {
    ws.onEvent(onWebSocketEvent);
    httpServer.addHandler(&ws);     // startWebServer() must run first.
    Serial.println("WebSocket server \"ghostRover\" started.");
}

/**
 * ------------------------------------------------
 *      Start tasks.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.7 [2025-11-10-10:30am].
 * @see    setup().
 * @link   https://www.freertos.org/Documentation/02-Kernel/04-API-references/01-Task-creation/01-xTaskCreate.
 */
void startTasks() {
    // -- LOOP status LED. --
    xTaskCreate(loopStatusLEDtask, "LOOP status LED", 2048, NULL, 2, &loopStatusLEDtaskHandle);
    Serial.println("Task \"LOOP LED\" started.");

    // -- Send position. --
    xTaskCreate(sendPositionTask, "Send position", 2048, NULL, 2, &sendPositionTaskHandle);
    Serial.println("Task \"Send position\" started.");

    // -- Send battery status. --
    xTaskCreate(sendBatteryStatusTask, "Send battery status", 2048, NULL, 2, &sendBatteryStatusTaskHandle);
    Serial.println("Task \"Send battery status\" started.");
}

/**
 * ------------------------------------------------
 *      Start loop().
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.7 [2025-11-07-12:30pm].
 * @see    setup().
 */
void startLoop() {
    ws2818LedColor = YELLOW;
    ws2818LedBlink = false;
    Serial.println("Loop() started.\n");
}

/**
 * ------------------------------------------------
 *      Check serial monitor (USB) for input.
 * ------------------------------------------------
 *
 * @return void No output is returned.
 * @return void  No output is returned.
 * @since  3.0.3 [2025-10-13-01:00pm].
 * @see    loop().
 */
void checkSerialMonitor(char print = ' ') {
}

/**
 * ------------------------------------------------
 *      Dicplay MCU processor info.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.3 [2025-10-13-01:00pm].
 * @see    startSerialMonitor().
 */
void chipInfo() {
    esp_chip_info(&chip_info);
    Serial.printf("Using %s, Rev %d,  %d core(s), ID (MAC) %012llX\n",
        ESP.getChipModel(), chip_info.revision, chip_info.cores, ESP.getEfuseMac());
}

/**
 * ------------------------------------------------
 *      Test built-in microSD reader.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.3 [2025-10-13-01:00pm].
 * @see    setup().
 */
void testSD() {
    Serial.print("SD card test - ");
    File file = SD.open("/index.html", "r");
    if (file == false){
        Serial.println("failed. Freezing.");
        while (true);
    }
    Serial.println("OK.");
    file.close();
}

/**
 * ------------------------------------------------
 *      Task - loop status LED.
 * ------------------------------------------------
 *
 * @param  void  * pvParameters Pointer to task parameters.
 * @return void  No output is returned.
 * @since  3.0.3 [2025-11-09-10:30am].
 * @see    startTasks().
 * @link   https://www.freertos.org/Documentation/02-Kernel/04-API-references/02-Task-control/06-vTaskSuspend.
 */
void loopStatusLEDtask(void * pvParameters) {
    u8_t bright = 18;                                   // 0-255.
    while(true) {
        switch (ws2818LedColor) {
            case RED:
                rgbLedWrite(LED_BUILTIN, bright, 0, 0);  // red, green, blue.
                break;
            case YELLOW:
                rgbLedWrite(LED_BUILTIN, bright, bright, 0);
                break;
            case GREEN:
                rgbLedWrite(LED_BUILTIN, 0, bright, 0);
                break;
            case BLUE:
                rgbLedWrite(LED_BUILTIN, 0, 0, bright);
                break;
        }
        vTaskDelay(LED_TIME_FLASH_ON);                  // LED remains on.
        if (ws2818LedBlink == true) {
            rgbLedWrite(LED_BUILTIN, 0, 0, 0);          // LED off.
            vTaskDelay(LED_TIME_FLASH_OFF);             // LED remains off.
        }
    }
}

/**
 * ------------------------------------------------
 *      Task - Send position.
 * ------------------------------------------------
 *
 * @param  void  * pvParameters Pointer to task parameters.
 * @return void  No output is returned.
 * @since  3.0.7 [2025-11-09-09:00am].
 * @see    startTasks().
 * @link   https://www.freertos.org/Documentation/02-Kernel/04-API-references/02-Task-control/06-vTaskSuspend.
 */
void sendPositionTask(void * pvParameters) {
    while(true) {

        // -- Build reply. --
        jsonDocToClient.clear();
        jsonDocToClient["elevation"] = elevation();
        jsonDocToClient["latitude"]  = latitude();
        jsonDocToClient["longitude"] = longitude();
        jsonDocToClient["hac"]       = hac();
        jsonDocToClient["vac"]       = vac();
        memset(buffer, '\0', sizeof(buffer));
        serializeJson(jsonDocToClient, buffer, sizeof(buffer));

        // -- Send reply. --
        ws.textAll(buffer);

        // -- Debug. -- 
        // Serial.printf("WebSocket #%u - browser <-- server %s\n", clientId, buffer);

        // -- Pause. --
        vTaskDelay(SEND_NUMBERS_PAUSE);               // Update interval = 2x per second.
    }
}

/**
 * ------------------------------------------------
 *      Task - Send battery status.
 * ------------------------------------------------
 *
 * @param  void  * pvParameters Pointer to task parameters.
 * @return void  No output is returned.
 * @since  3.0.7 [2025-11-10-11:00am].
 * @see    startTasks().
 * @link   https://www.freertos.org/Documentation/02-Kernel/04-API-references/02-Task-control/06-vTaskSuspend.
 */
void sendBatteryStatusTask(void * pvParameters) {
    while(true) {
        
        // -- Build reply. --
        jsonDocToClient.clear();
        jsonDocToClient["batterySoc"]    = (double) lipo.getSOC();
        jsonDocToClient["batteryChange"] = (double) lipo.getChangeRate();
        memset(buffer, '\0', sizeof(buffer));
        serializeJson(jsonDocToClient, buffer, sizeof(buffer));

        // -- Send reply. --
        ws.textAll(buffer);

        // -- Debug. --
        // Serial.printf("LIPO: %.2f V, %.2f %, %.2f %\n", lipo.getVoltage(), lipo.getSOC(), lipo.getChangeRate());
    
        // -- Pause. --
        vTaskDelay(SEND_BATTERY_PAUSE);               // Update interval.
    }
}

/**
 * ------------------------------------------------
 *      HTTP server - /list.
 * ------------------------------------------------
 *
 * Create SD file list.
 *
 * @return void  No output is returned.
 * @since  3.0.3 [2025-11-10-02:00pm].
 * @see    onListFiles().
 */
void sdListFiles(char* output, size_t outputMaxSize, char* path) {
    File root = SD.open(path);
    File file = root.openNextFile();
    while(file) {
        if (strlen(output) + strlen(file.name()) + 2 < outputMaxSize) {       
            if ((file.name()[0] != '.') && (file.name() != "") && (!file.isDirectory())) {
                // TODO: Flat fs for now, add directories & recursive call.
                strcat(output, "/");
                strcat(output, file.name());
                strcat(output, ",");
            }
        }
      file = root.openNextFile();
    }
}

/**
 * ------------------------------------------------
 *      HTTPS server - set endpoints & start.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.7 [2025-11-11-06:15pm].
 * @see    setup().
 * @link   https://github.com/ESP32Async/ESPAsyncWebServer/wiki#get-post-and-file-parameters.
 */
void startHttpServer() {

    // --- Home page. ---
    httpServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {       // Set root.
        request->send(SD, "/index.html", "text/html");
        Serial.println("httpServer requested \"/\", sent \"/index.html\"");}) ;

    httpServer.on("/upload", HTTP_POST, [](AsyncWebServerRequest *req) {
        req->send(200, "text/plain", "Upload complete");
        Serial.println("httpServer upload complete.");}, handleFileUpload);    // Register handler.

    // --- Download. TBD. ---
    httpServer.on("/download", HTTP_GET, [](AsyncWebServerRequest *request) { 
        if (request->hasParam("file")) {
            String filename = request->getParam("file")->value();
            String filepath = "/" + filename;
            if (SD.exists(filepath)) {
                request->send(SD, filepath, "application/octet-stream", true);
                Serial.printf("Sending file for download: %s\n", filename.c_str());
            } else {
                request->send(404, "text/plain", "File not found");
                Serial.printf("File not found: %s\n", filename.c_str());
            }
        } else {
            request->send(400, "text/plain", "File parameter required");
        }
    });

    // --- Start. ---
    httpServer.serveStatic("/", SD, "/");
    httpServer.begin();
    Serial.println("httpServer started.");
}

/**
 * ------------------------------------------------
 *      HTTP server - /upload.
 * ------------------------------------------------
 *
 * Send files to SD.
 *
 * @return void  No output is returned.
 * @since  3.0.7 [2025-11-11-06:00pm].
 * @see    startHttpServer().
 */
void handleFileUpload( AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
    static File uploadFile;                                // HTTP upload file.

    // --- Start. ---
    if (index == 0) {
        Serial.println("\nhttpServer endpoint \"/upload\".\nhandleFileUpload() running.");
        uploadFile = SD.open("/" + filename, FILE_WRITE);  // Open file for writing.
        if (uploadFile) {
            Serial.printf("%s opened on SD.\n", filename.c_str());
        } else {
            request->send(500, "text/plain", "Cannot open file for writing on SD.");
            Serial.printf("Cannot open %s on SD for writing.\n", filename.c_str());
            return;  
        }
    }

    // --- Data chunk. ---
    if (len) {                                                       
        uploadFile.write(data, len);  // Write received data to file.
        Serial.printf("%u total bytes written.\n", (unsigned int)(index + len));
    }
    
    // --- Complete. ---
    if (final) {
        uploadFile.close();
        Serial.printf("%s closed on SD.\n", filename.c_str());
        request->send(200, "text/plain", "Upload complete. File saved to SD.");
    }
}

/**
 * ------------------------------------------------
 *      WebSocket event handler.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.3 [2025-11-08-03:15pm].
 * @see    startWebSocketServer().
 */
void onWebSocketEvent(AsyncWebSocket *httpServer, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    clientId = client->id();
    switch (type) {
        case WS_EVT_CONNECT:
            Serial.printf("WebSocket #%u - connected to server (%s)\n", clientId, client->remoteIP().toString().c_str());
            ws2818LedColor = GREEN;                         // Loop status indicator LED.
            ws2818LedBlink = false;
            break;
        case WS_EVT_DISCONNECT:
            Serial.printf("WebSocket #%u - disconnected\n\n", clientId);
            ws2818LedColor = YELLOW;
            ws2818LedBlink = false;
            break;
        case WS_EVT_DATA:
            handleWebSocketMessage(arg, data, len);         // WebSocket message handler.
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
 * @since  3.0.7 [2025-11-10-12:00pm].
 * @see    onWebSocketEvent().
 * @link   https://arduinojson.org/v6/api/json/deserializejson/.
 * @link   https://arduinojson.org/v6/doc/deserialization/.
 * @link   https://arduinojson.org/v7/api/jsonvariant/
 */

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {

        // --- Deserialize the JSON message. ---
        jsonDocFromClient.clear();
        DeserializationError error = deserializeJson(jsonDocFromClient, data);
        if (error) {
            Serial.printf("JSON deserialize failed: %s\n", error.f_str());
            return;
        }

        // --- Create JSON key/value pairs. ---
        memset(buffer, '\0', sizeof(buffer));
        serializeJson(jsonDocFromClient, buffer, sizeof(buffer));
        Serial.printf("WebSocket #%u - browser --> server %s\n", clientId, buffer);

        // --- Process each JSON key/value pair. ---
        for (JsonPair kv : jsonDocFromClient.as<JsonObject>()) {

            jsonDocToClient.clear();

            // - {"operate":"ready"}. - 
            if ((strcmp(kv.key().c_str(), "operate") == 0) && (strcmp(kv.value().as<const char*>(), "ready") == 0 )) {
                vTaskResume(sendPositionTaskHandle);
                vTaskResume(sendBatteryStatusTaskHandle);

                // Build reply.
                sprintf(buffer, "%c.%c.%c", MAJOR_VERSION, MINOR_VERSION, PATCH_VERSION);
                jsonDocToClient["version"] = buffer;
                jsonDocToClient["mode"]    = operMode;
                jsonDocToClient["units"]   = operUnits;
            }

            // - {"main,files,config":"ready"}. -
            else if (strcmp(kv.value().as<const char*>(), "ready") == 0 ) {
                vTaskSuspend(sendPositionTaskHandle);
                vTaskSuspend(sendBatteryStatusTaskHandle);

                // Build reply.
                jsonDocToClient["server"] = kv.value().as<const char*>();
            }

            // - {"main,files,config":"leaving"}. -
            else if (strcmp(kv.value().as<const char*>(), "leaving") == 0 ) {
                return;
            }

            // - {"elevation,laser,location":"lock, unlock"}. -
            else if ((strcmp(kv.value().as<const char*>(), "lock") == 0 ) ||
                     (strcmp(kv.value().as<const char*>(), "unlock") == 0 )) {
                        
                // Build reply.
                if ((strcmp(kv.key().c_str(), "elevation") == 0) ||
                    (strcmp(kv.key().c_str(), "laser") == 0) ||
                    (strcmp(kv.key().c_str(), "location") == 0)) {
                    memset(buffer, '\0', sizeof(buffer));
                    strcpy(buffer, kv.value().as<const char*>());
                    strcat(buffer, "ed");
                    jsonDocToClient[kv.key().c_str()] = buffer;
                }
            }

            // - {"listFiles":""}. -
            if (strcmp(kv.key().c_str(), "listFiles") == 0) {
                char output[2048];
                memset(output, '\0', sizeof(output));
                sdListFiles(output, sizeof(output), "/");
                jsonDocToClient[kv.key().c_str()] = output;
            }

            // - {"deleteFile":"filename"}. -
            // https://github.com/espressif/arduino-esp32/blob/master/libraries/SD/examples/SD_Test/SD_Test.ino.
            if (strcmp(kv.key().c_str(), "deleteFile") == 0) {
                if (SD.remove(kv.value().as<const char*>())) {                          // Delete file.
                    jsonDocToClient["fileDeleted"] = kv.value().as<const char*>();      // Success.
                } else {
                    jsonDocToClient["fileNOTdeleted"] = kv.value().as<const char*>();   // Failed.
                }
            }

            // - {"echo":"hello"}. -
            else if (strcmp(kv.key().c_str(), "echo") == 0) {  
                jsonDocToClient[kv.key().c_str()] = kv.value().as<const char*>();
            }

            // - {"mcu":"restart"}. -
            else if ((strcmp(kv.key().c_str(), "mcu") == 0) && (strcmp(kv.value().as<const char*>(), "restart") == 0 )) {
                Serial.println("Restarting ...\n");
                esp_restart();
            }

            // - Send reply. -
            memset(buffer, '\0', sizeof(buffer));
            serializeJson(jsonDocToClient, buffer, sizeof(buffer));
            ws.textAll(buffer);
            Serial.printf("WebSocket #%u - browser <-- server %s\n", clientId, buffer);
        }
    }
}

// --- Test. ---

/**
 * ------------------------------------------------
 *      I2C scanner.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.7 [2025-11-11-07:00pm].
 * @link   .
 */
void scanI2C() {
    byte error, address;
    int nDevices;
    Serial.println("Scanning...");
    nDevices = 0;
    for(address = 1; address < 127; address++ ) {
        Wire.beginTransmission(address);
        error = Wire.endTransmission();
        if (error == 0) {
        Serial.print("I2C device found at address 0x");
        if (address<16) {
            Serial.print("0");
        }
        Serial.println(address,HEX);
        nDevices++;
        }
        else if (error==4) {
        Serial.print("Unknow error at address 0x");
        if (address<16) {
            Serial.print("0");
        }
        Serial.println(address,HEX);
        }    
    }
    if (nDevices == 0) {
        Serial.println("No I2C devices found\n");
    }
    else {
        Serial.println("done\n");
    }
    delay(5000);
}

/**
 * ------------------------------------------------
 *      Generate random test values.
 * ------------------------------------------------
 *
 * @return float  Random value.
 * @since  3.0.7 [2025-11-10-10:30am].
 */
float random_in_range(int min, int max) {
    return (rand() % (max - min + 1)) + min; // Generate a random integer in the range [min, max]
}
char* latitude() {
    float num = (355530000 + random_in_range(6050, 6250))/10000000.0;
    memset(buffer, '\0', sizeof(buffer));
    sprintf(buffer, "%.7f", num);
    return buffer;       // E.g. 35.5536111).
}
char* longitude() {
    float num = (-787710000.0 - random_in_range(3500, 4500))/10000000.0;
    memset(buffer, '\0', sizeof(buffer));
    sprintf(buffer, "%.7f", num);
    return buffer;      // E.g. -78.7713888.
}
char* elevation() {
    float num = 100 + random_in_range(0, 50)/100.0;
    memset(buffer, '\0', sizeof(buffer));
    sprintf(buffer, "%.2f", num);
    return buffer;      // E.g. 103.56.
}
char* hac() {
    float num = (random_in_range(10, 25))/100.0;
    memset(buffer, '\0', sizeof(buffer));
    sprintf(buffer, "%.2f", num);
    return buffer;      // E.g. 0.190.
}
char* vac() {
    float num = (random_in_range(15, 25))/100.0;
    memset(buffer, '\0', sizeof(buffer));
    sprintf(buffer, "%.2f", num);
    return buffer;      // E.g. 0.190.
}

/**
 * ============================================================================
 *                              Setup.
 * ============================================================================
 *
 * @since  3.0.3 [2025-10-13-01:00pm].
 */

void setup() {
    startSerialMonitor();               // Start Serial (USB) for monitor.
    initVars();                         // Initialize global vars.
    initPins();                         // Initialize pin modes & pin values.
    startSerialInterfaces();            // Start serial interfaces.
    startI2C();                         // Start I2C interface.
    startLIPO();                        // Start LIPO I2C interface.
    startWiFi();                        // Start WiFi.
    startSPI();                         // Based on SPI pin assignments.
    startSD();                          // Start built-in microSD card reader.
    testSD();                           // Test microSD card reader. Read & print a file.
    startHttpServer();                  // Start HTTP server.
    startWebSocketServer();             // Start WebSocket server.
    startTasks();                       // Start tasks.
    startLoop();                        // On to loop().
}

/**
 * ============================================================================
 *                              Loop.
 * ============================================================================
 *
 * @since  3.0.7 [2025-11-10-10:30am].
 */

void loop() {
    checkSerialMonitor();               // Check serial monitor (USB) for input.
    ws.cleanupClients();
    // scanI2C();
}

