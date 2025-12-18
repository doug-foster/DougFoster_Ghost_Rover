/**
 * **********************************************************************
 *          Ghost Rover 3 - GNSS "invisible" rover (with RTK base/PP) for SW Maps.
 * **********************************************************************
 *
 * @author   D. Foster <doug@dougfoster.me>.
 * @since    3.0.9 [2025-12-03-05:15pm].
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
 *     -- Board: "Sparkfun ESP32-S3 Thing Plus" (~/Library/Arduino15/packages/esp32/hardware/esp32/3.2.0/boards.txt)
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
 *     -- Platform   https://github.com/espressif/arduino-esp32/releases/tag/3.3.3 (Arduino Release v3.3.3 based on ESP-IDF v5.5.1+).
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
 * @since 3.0.9             [2025-12-01-05:15pm].
 * @link  Arduino           https://docs.arduino.cc/libraries/.
 * @link  ESP32             https://docs.espressif.com/projects/arduino-esp32/en/latest/libraries.html.
 * @link  I2C               https://docs.arduino.cc/language-reference/en/functions/communication/wire/. 
 * @link  LittleFS          https://randomnerdtutorials.com/arduino-ide-2-install-esp32-littlefs/.
 * @link  WiFi/WiFiAP       https://docs.espressif.com/projects/arduino-esp32/en/latest/api/wifi.html.
 * @link  NetworkClient     https://docs.espressif.com/projects/arduino-esp32/en/latest/api/network.html.
 */

// --- Core. ---
#include <Arduino.h>                                       // https://github.com/espressif/arduino-esp32.
#include <WiFi.h>                                          // https://github.com/espressif/arduino-esp32/tree/master/libraries/WiFi.
#include <WiFiAP.h>                                        // https://github.com/espressif/arduino-esp32/tree/master/libraries/WiFi.
#include <SD.h>                                            // https://github.com/espressif/arduino-esp32/tree/master/libraries/SD.
#include <FS.h>                                            // https://github.com/espressif/arduino-esp32/tree/master/libraries/FS.
#include <SPI.h>                                           // https://github.com/espressif/arduino-esp32/tree/master/libraries/SPI.
#include <Wire.h>                                          // https://github.com/espressif/arduino-esp32/blob/master/libraries/Wire/src/Wire.h.
#include <time.h>                                          // https://github.com/espressif/arduino-esp32/blob/master/cores/esp32/esp32-hal-time.c#L47.
#include <HardwareSerial.h>                                // https://github.com/espressif/arduino-esp32/blob/master/cores/esp32/HardwareSerial.cpp.
#include <esp_system.h>                                    // https://github.com/pycom/pycom-esp-idf.
#include <esp_chip_info.h>                                 // https://github.com/pycom/pycom-esp-idf.

// --- Additional. ---                  
#include <AsyncTCP.h>                                      // https://github.com/ESP32Async/AsyncTCP (Async TCP - v3.4.9).
#include <ESPAsyncWebServer.h>                             // https://github.com/ESP32Async/ESPAsyncWebServer (ESP Async WebServer - v3.8.1).
#include <ArduinoJson.h>                                   // https://github.com/bblanchon/ArduinoJson (ArduinoJSON - v7.4.2).
#include <SparkFun_MAX1704x_Fuel_Gauge_Arduino_Library.h>  // https://github.com/sparkfun/SparkFun_MAX1704x_Fuel_Gauge_Arduino_Library (SparkFun MAX1704x Fuel Gauge Arduino Library - 1.0.4)
#include <SparkFun_u-blox_GNSS_v3.h>                       // https://github.com/sparkfun/SparkFun_u-blox_GNSS_v3 (SparkFun u-blox GNSS v3 - 3.1.13).

/**
 * ============================================================================
 *                          Global vars.
 * ============================================================================
 *
 * @since 3.0.9 [2025-12-01-05:15pm].
 */

// --- Pin. --- 
// const uint8_t LED_STATUS   =  0;             // ESP-32-S3 Thing Plus - pin for status LED.
// const uint8_t I2C_SDA      =  8;             // ESP32-S3 Thing+ <-> I2C-1 data pin        {Qwiic} - SDA.
// const uint8_t I2C_SCL      =  9;             // ESP32-S3 Thing+ <-> I2C-1 clock pin       {Qwiic} - SCL.

const uint8_t SPI_SCK          = 38;            // ESP32-S3 Thing+ SPI serial clock
const uint8_t SPI_PICO         = 34;            // microSD SDI 
const uint8_t SPI_POCI         = 39;            // microSD SDO
const uint8_t SPI_CS           = 33;            // SPI chip select

// const uint8_t HC12_RX      = 43;             // See above.
// const uint8_t HC12_TX      = 44;             // See above.
// const uint8_t HC12_SET     = 42;             // ESP32-S3 Thing+ <-> HC-12 SET             {blue wire}.
// const uint8_t PTH_S0_TX =  1;                // Default S0 (UART0, USB) TX - not used.
// const uint8_t PTH_S0_RX =  3;                // Default S0 (UART0, USB) RX - not used.

// --- LED. ---
enum ws2812_LED_COLOR {                         // WS2812 RGB STAT LED, pin=RGB_BUILTIN=IO23.
    OFF,                                        // 0.
    RED,                                        // 1.
    YELLOW,                                     // 2.
    GREEN,                                      // 3.
    BLUE,                                       // 4.
    WHITE                                       // 5.
} ws2812LedColor;
bool ws2812LedBlink;

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
const uint32_t SERIAL_MON_SPEED = 115200;       // Serial USB monitor speed.
      char     inputChar;                       // Input/output character.
const uint8_t  I2C0_SDA_PIN = 6;                // Primary I2C bus - data.
const uint8_t  I2C0_SCL_PIN = 7;                // Primary I2C bus - clock.
const uint8_t  I2C1_SDA_PIN = 14;               // Secondary I2C bus - data.
const uint8_t  I2C1_SCL_PIN = 10;               // Secondary I2C bus - clock.
      bool     wireStatus;                      // Status: up (true), down (false).
      bool     wire1Status;                     // Status: up (true), down (false).

// --- BLE (Bluetooth Low Energy). ---

// --- WiFi. ---
const char      *ssid = "Ghost Rover";
const char      *password = "snark217$";
      IPAddress ap_local_IP(192, 168, 23, 1);       // ESP32 access point (AP) IP address.
      IPAddress ap_gateway(192, 168, 23, 1);        // Gateway address.
      IPAddress ap_subnet(255, 255, 255, 0);        // Subnet mask.
const char      *ap_name = "ghost";                 // AP name.

// --- Timing. ---
const TickType_t LED_TIME_FLASH_ON  = 100/portTICK_PERIOD_MS;       // Timer (ms) =  0.1 seconds.
const TickType_t LED_TIME_FLASH_OFF = 1000/portTICK_PERIOD_MS;      // Timer (ms) =  1.0 seconds.
const TickType_t SEND_GNSS_PAUSE    = 250/portTICK_PERIOD_MS;       // Timer (ms) =  0.3 seconds.
const TickType_t SEND_BATTERY_PAUSE = 5000/portTICK_PERIOD_MS;      // Timer (ms) =  5.0 seconds.

// --- Task handles. ---
TaskHandle_t loopStatusLEDtaskHandle;           // Task: Loop status LED.
TaskHandle_t sendGnssTaskHandle;                // Task: send GNSS data.
TaskHandle_t sendBatteryStatusTaskHandle;       // Task: send battery status.

// --- Operation. ---

// --- JSON. ---
JsonDocument jsonDocFromClient;                 // JSON document received from client.
JsonDocument jsonDocToClient;                   // JSON document sent to client.

// --- HTTP. ---
AsyncWebServer httpServer(80);                  // HTTP AsyncWebServer object on port 80.
AsyncWebSocket ws("/ghostRover");               // HTTP WebSocket object.
uint8_t        clientId;                        // HTTP WebSocket client ID #.

// --- GNSS. ---
      uint16_t  GNSS_SOLN_MS             = 250; // Ms between solution calculations.
      uint32_t  lastGNGGA;                      // Last time a $GNGGA was received.
const int8_t    MIN_SATELLITE_THRESHHOLD = 2;   // Minimum SIV for reliable coordinate information.
      int8_t    siv;                            // Satellites in view.
      char      fixStatus[7];                   // Fix status: down, single, float, fix.
      float     alt;                            // Altitude.
      double    lat;                            // Latitude degrees * 10^-9 (high precision).
      double    lon;                            // Longitude degrees * 10^-9 (high precision).
      float     hAcc;                           // Horizontal accuracy.
      float     vAcc;                           // Vertical accuracy.
      char      nmeaBuffer[120];                // Buffer for NMEA sentence.
SFE_UBLOX_GNSS  roverGNSS;                      // GNSS object (uses I2C-1).

// --- General. ---
esp_chip_info_t chip_info;                      // Chip info.
char            operMode[5];                    // Operation mode (rover, base).
char            operUnits[7];                   // Operation units (meter, feet).
char            buffer[256];                    // Utility buffer.
bool            inLoop = false;                 // In loop indicator.

// --- Version. ---
const char BUILD_DATE[]  = "[2025-12-01-05:30pm]";
const char MAJOR_VERSION = '3';
const char MINOR_VERSION = '0';
const char PATCH_VERSION = '9';
const char NAME[]        = "Ghost Rover 3";

// --- Declaration. ---

// --- Test. ---

/**
 * ============================================================================
 *                              Functions.
 * ============================================================================
 */

// ==================================
//         Setup functions.
// ==================================

 /**
 * ------------------------------------------------
 *      Start serial USB monitor.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.7 [2025-11-14-07:00am].
 */
void startSerialUsbMonitor() {
    Serial.begin(SERIAL_MON_SPEED);
    delay(1000);
    Serial.printf("\n%s, Version: %c.%c.%c, Build date: %s\n", NAME, MAJOR_VERSION, MINOR_VERSION, PATCH_VERSION, BUILD_DATE);
    chipInfo();     // Display processor info.
    Serial.println("\nSetup() started.");
    Serial.printf("Serial USB monitor started @ %i bps.\n", SERIAL_MON_SPEED);
}

/**
 * ------------------------------------------------
 *      Display processor info.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.7 [2025-11-14-07:00am].
 * @see    startSerialMonitor().
 */
void chipInfo() {
    esp_chip_info(&chip_info);
    Serial.printf("Using %s, Rev %d,  %d core(s), ID (MAC) %012llX\n",
    ESP.getChipModel(), chip_info.revision, chip_info.cores, ESP.getEfuseMac());
}

/**
 * ------------------------------------------------
 *      Initialize global vars.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.7 [2025-11-07-12:30pm].
 * @see    setup().
 */
void initVars() {
    Serial.print("Init global vars");
    inputChar = '\0';           // Serial USB monitor.
    ws2812LedColor = RED;       // Task - loop status indicator LED.
    ws2812LedBlink = false;     // Task - loop status indicator LED.
    clientId  = 0;              // HTTP WebSocket client ID #.
    lastGNGGA = 0;              // Last time a $GNGGA was received.
    memset(buffer, '\0', sizeof(buffer));
    memset(nmeaBuffer, '\0', sizeof(nmeaBuffer));
    strcpy(operMode, "base");
    strcpy(operUnits, "meters");
    Serial.println(".");
}

/**
 * ------------------------------------------------
 *      Initialize pins & pin values.
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
 *      Start I2C Wire interface.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.9 [2025-12-05-05:00pm] New. 
 * @see    setup().
 */
void startWire() {
    if (wireStatus == 0) {      // Wire is down.
        if (Wire.begin() == false) {
            Serial.println("Wire start failed. Retrying.");
            delay(500);
            startWire();
        } else {
            Wire.setClock(400000);
            Serial.println("Wire started @ 4kHz.");
            wireStatus = 1;
        }
    } else {
        // Wire was up, now down.
        Wire.end();
        Serial.println("Wire down. Restarting ...");
        wireStatus = 0;
        delay(500);
        startWire();
    }
}

/**
 * ------------------------------------------------
 *      Start I2C Wire1 interface.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.9 [2025-12-05-05:00pm] New. 
 * @see    setup().
 */
void startWire1() {
    if (wire1Status == 0) {      // Wire is down.
        if (Wire1.begin(I2C1_SDA_PIN,I2C1_SCL_PIN) == false) {
            Serial.println("Wire1 start failed. Retrying.");
            delay(500);
            startWire1();
        } else {
            Wire1.setClock(400000);
            Serial.println("Wire1 started @ 4kHz.");
            wire1Status = 1;
        }
    } else {
        Wire1.end();            // Wire was up, now down. Restart.
        Serial.println("Wire1 down. Restarting ...");
        wire1Status = 0;
        delay(1000);
        startWire1();
    }
}

// /**
//  * ------------------------------------------------
//  *      Start I2C interface.
//  * ------------------------------------------------
//  *
//  * @return void  No output is returned.
//  * @since  3.0.3 [2025-10-16-03:00pm].
//  * @since  3.0.8 [2025-11-21-11:30am] Wire.setClock() does not return a value.
//  * @since  3.0.8 [2025-11-30-09:15am] Multiple I2C buses. 
//  * @see    setup().
//  */
// void startI2C() {
//     if (Wire.begin() == false) {
//         Serial.println("Start Wire0 failed. Freezing.");
//         while (true);
//     }
//     Wire.setClock(400000);
//     Serial.println("Wire0 started @ 4kHz.");
//     wireStatus = 1;
//     if (Wire1.begin(I2C1_SDA_PIN,I2C1_SCL_PIN) == false) {
//         Serial.println("Start Wire1 failed. Freezing.");
//         while (true);
//     }
//     Wire1.setClock(400000);
//     Serial.println("Wire1 started @ 4kHz.");
//     wire1Status = 1;
// }

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
    // lipo.enableDebugging();
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
 *      Start WiFi client.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.3 [2025-10-13-03:15pm].
 * @since  3.0.7 [2025-11-20-12:00pm] Renamed function.
 * @since  3.0.8 [2025-11-29-11:15am] Deprecated.
 * @see    setup().
 */
// void startWiFiClient() {
    // WiFi.mode(WIFI_STA);        // Station mode.
    // WiFi.config (staticIP);
    // WiFi.begin(ssid, password);
    // Serial.printf("WiFi started.\nConnecting to SSID = %s ", ssid);
    // while (WiFi.status() != WL_CONNECTED) {
    //     Serial.print('.');
    //     delay(1000);            // Try again.
    // }
    // Serial.printf("\nConnected, IP = %s .\n", WiFi.localIP().toString().c_str());
// }

/**
 * ------------------------------------------------
 *      Start WiFi soft AP.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.7 [2025-11-20-12:30pm]. New.
 * @see    setup().
 */
void startWiFiSoftAP() {
    if (!WiFi.softAPConfig(ap_local_IP, ap_gateway, ap_subnet)) {       // Configure IP.
        Serial.println("Soft AP - config failed.");
    }
    if (!WiFi.softAP(ssid)) {                                           // Set SSID. No password.
        Serial.println("Soft AP - create failed. Freezing.");
        while (true);
    }
    WiFi.softAPsetHostname("ghost");                                    // Set hostname.
    IPAddress ip = WiFi.softAPIP();                                     // Start WiFi & check status (get IP).
    Serial.printf("Soft AP %d.%d.%d.%d (aka %s) started.\n", ip[0], ip[1], ip[2], ip[3], ap_name);
    WiFi.onEvent(WiFiEvent);                                            // Add WiFiEvent() as event handler.
}

/**
 * ------------------------------------------------
 *      Start SDIO for microSD reader.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.3 [2025-10-13-01:00pm].
 * @see    setup().
 */
void startSDIO() {
    if (!SPI.begin(SPI_SCK, SPI_POCI, SPI_PICO, SPI_CS)) {
        Serial.println("SDIO not started. Freezing.");
        while (true);
    }
    delay(1000);
    Serial.println("SDIO started.");  // SanDisk 128GB ImageMate microSDXC UHS-1 - Up to 140MB/s.
}

/**
 * ------------------------------------------------
 *      Start built-in microSD card reader.
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
 *      Start HTTP server. 
 * ------------------------------------------------
 *
 * Set endpoints & start.
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
 *      Start WebSocket server.
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
 *      Start GNSS, config ZED settings.
 * ------------------------------------------------
 * 
 * Uses library SparkFun_u-blox_GNSS_v3 for UBX-CFG-VALGET & UBX-CFG-VALSET binary commands.
 *
 * @return void No output is returned.
 * @since  0.1.0 [2025-04-24-12:00pm] New.
 * @since  3.0.7 [2025-11-14-04:00pm] Import from Ghost Rover V2.
 * @see    Global vars: GNSS.
 * @see    beginSerialInterfaces().
 * @see    beginI2C().
 * @link   https://github.com/sparkfun/SparkFun_u-blox_GNSS_v3/blob/main/examples/Example1_PositionVelocityTime/Example1_PositionVelocityTime.ino.
 * @link   https://github.com/sparkfun/SparkFun_u-blox_GNSS_v3/blob/main/src/u-blox_config_keys.h.
 * @link   GNGGA = PVT, fix quality, SIV, HDOP, ...           https://receiverhelp.trimble.com/alloy-gnss/en-us/NMEA-0183messages_GGA.html.
 * @link   GPGSV = # Sats visible, sat info, ...              https://receiverhelp.trimble.com/alloy-gnss/en-us/NMEA-0183messages_RMC.html.
 * @link   GNGSA = PRN # for active sat, PDOP/HDOP/VDOP, ...  https://receiverhelp.trimble.com/alloy-gnss/en-us/NMEA-0183messages_GSA.html.
 * @link   GNRMC = PVT, ...                                   https://receiverhelp.trimble.com/alloy-gnss/en-us/NMEA-0183messages_GSV.html.
 * @link   GNGST = Position error statistics, ...             https://receiverhelp.trimble.com/alloy-gnss/en-us/NMEA-0183messages_GST.html.
 * @link   GNGLL = Position fix & status                      https://receiverhelp.trimble.com/alloy-gnss/en-us/NMEA-0183messages_GLL.html.
 * @link   GNVTG = Tracking.                                  https://receiverhelp.trimble.com/alloy-gnss/en-us/NMEA-0183messages_VTG.html.
 */
void startAndConfigGNSS() {
    
    // -- Start GNSS interface on I2C-1. --
    Serial.print("startAndConfigGNSS() - roverGNSS: start");                            // Print status.
    if (roverGNSS.begin() == false) {
        Serial.println(" failed. Freezing ...");                                        // Something is wrong, freeze.
        while (true);                                                                   // Infinite loop.
    } else {
        Serial.println(".");
    }

    // -- Configure interfaces. --
    Serial.print("startAndConfigGNSS() - roverGNSS: valset keys");
    roverGNSS.newCfgValset(VAL_LAYER_RAM);                                              // New config template.
    roverGNSS.addCfgValset(UBLOX_CFG_NMEA_HIGHPREC,            1);                      // High precision NMEA.  9 decimal places.
    roverGNSS.addCfgValset(UBLOX_CFG_RATE_MEAS,     250);                      // Ms between solution calculations.
    roverGNSS.addCfgValset(UBLOX_CFG_MSGOUT_UBX_NAV_PVT_I2C,   1);                      // I2C - Output solutions periodically.
    roverGNSS.addCfgValset(UBLOX_CFG_I2CINPROT_UBX,            1);                      // I2C - Turn on UBX protocol in.
    roverGNSS.addCfgValset(UBLOX_CFG_I2COUTPROT_UBX,           1);                      // I2C - Turn on UBX protocol out.
    roverGNSS.addCfgValset(UBLOX_CFG_I2CINPROT_NMEA,           0);                      // I2C - Turn off NMEA protocol in.
    roverGNSS.addCfgValset(UBLOX_CFG_I2COUTPROT_NMEA,          1);                      // I2C - Turn on NMEA protocol out.
    roverGNSS.addCfgValset(UBLOX_CFG_I2CINPROT_RTCM3X,         0);                      // I2C - Turn off RTCM3 protocol in.
    roverGNSS.addCfgValset(UBLOX_CFG_I2COUTPROT_RTCM3X,        0);                      // I2C - Turn off RTCM3 protocol out.
    roverGNSS.addCfgValset(UBLOX_CFG_UART1_ENABLED,            0);                      // UART1 - Disable.
    roverGNSS.addCfgValset(UBLOX_CFG_UART2_ENABLED,            1);                      // UART2 - Enable.     
    // roverGNSS.addCfgValset(UBLOX_CFG_MSGOUT_NMEA_ID_GGA_UART2, 1);                      // UART2 - Turn on GGA sentences for SW Maps.
    // roverGNSS.addCfgValset(UBLOX_CFG_MSGOUT_NMEA_ID_GSV_UART2, 1);                      // UART2 - Turn on GSV sentences for SW Maps.
    // roverGNSS.addCfgValset(UBLOX_CFG_MSGOUT_NMEA_ID_GSA_UART2, 1);                      // UART2 - Turn on GSA sentences for SW Maps.
    // roverGNSS.addCfgValset(UBLOX_CFG_MSGOUT_NMEA_ID_RMC_UART2, 1);                      // UART2 - Turn on RMC sentences for SW Maps.
    // roverGNSS.addCfgValset(UBLOX_CFG_MSGOUT_NMEA_ID_GST_UART2, 1);                      // UART2 - Turn on GST sentences for SW Maps.
    // roverGNSS.addCfgValset(UBLOX_CFG_MSGOUT_NMEA_ID_GLL_UART2, 0);                      // UART2 - Turn off GLL sentences.
    // roverGNSS.addCfgValset(UBLOX_CFG_MSGOUT_NMEA_ID_VTG_UART2, 0);                      // UART2 - Turn off VTG sentences.
    roverGNSS.sendCfgValset() ? Serial.println(".") : Serial.println(". Failed.");      // Send the config.
    // - ZDA & GNS sentences are off by default. -
    // roverGNSS.saveConfiguration();                                                   // Save current settings to BBR/Flash.
    // roverGNSS.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT);                            // Save port settings to flash and BBR.
    // roverGNSS.setNMEAOutputPort(ble);                                                // Debug - pipe all NMEA sentences to serial USB.
    // roverGNSS.enableDebugging();                                                     // Debug - all messages over Serial (default).
}

/**
 * ------------------------------------------------
 *      Start tasks.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.7 [2025-11-14-04:30pm].
 * @see    Global vars: Task handles.
 * @see    setup().
 * @link   https://www.freertos.org/Documentation/02-Kernel/04-API-references/01-Task-creation/01-xTaskCreate.
 */
void startTasks() {

    // -- LOOP status LED. --
    xTaskCreate(loopStatusLedTask, "LOOP status LED", 2048, NULL, 2, &loopStatusLEDtaskHandle);
    Serial.println("Task started: \"loop() LED.\".");

    // -- Send GNSS numbers. --
    xTaskCreate(sendGnssTask, "Send GNSS data", 2048, NULL, 2, &sendGnssTaskHandle);
    Serial.println("Task started: \"send GNSS data\".");

    // -- Send battery status. --
    xTaskCreate(sendBatteryStatusTask, "Send battery status", 2048, NULL, 2, &sendBatteryStatusTaskHandle);
    Serial.println("Task started: \"Send battery status\".");
}

/**
 * ------------------------------------------------
 *      Start loop().
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.7 [2025-11-07-12:30pm].
 * @since  3.0.7 [2025-11-21-06:00pm] Added inLoop.
 * @see    setup().
 */
void startLoop() {
    ws2812LedColor = RED;
    ws2812LedBlink = true;
    Serial.println("Loop() started.\n");
    inLoop = true;
}

// ==================================
//         Loop functions.
// ==================================

/**
 * ------------------------------------------------
 *      Check serial monitor (USB) for input.
 * ------------------------------------------------
 *
 * @return void No output is returned.
 * @since  3.0.3 [2025-10-13-01:00pm].
 * @see    loop().
 */
void checkSerialMonitor(char print = ' ') {
}

/**
 * ------------------------------------------------
 *      Check I2C Wire1 interface.
 * ------------------------------------------------
 *
 * @return void No output is returned.
 * @since  3.0.9 [2025-12-05-06:15pm].
 * @see    loop(), DevUBLOXGNSS::processNMEA().
 */
void checkWire1() {
    u8_t status;
    Wire1.beginTransmission(8);                             // Test Wire1. Receiver is device #8.
    status = Wire1.endTransmission(8);                      // Test Wire1. Is device up?
    if (status != 0) {
        Serial.printf("\nI2C error = %i. \n", status);      // Slave is down.
        startWire1();                                       // Restart Wire1.
    } else {
        wire1Status = 1;                                    // Slave is up.
        Serial.print("u");
    }
}

// ==================================
//         Task functions.
// ==================================

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
void loopStatusLedTask(void * pvParameters) {
    u8_t bright = 18;                                   // 0-255.
    while(true) {
        switch (ws2812LedColor) {
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
            case WHITE:
                rgbLedWrite(LED_BUILTIN, 0, 0, 0);
                break;
        }
        vTaskDelay(LED_TIME_FLASH_ON);                  // LED remains on.
        if (ws2812LedBlink == true) {
            rgbLedWrite(LED_BUILTIN, 0, 0, 0);          // LED off.
            vTaskDelay(LED_TIME_FLASH_OFF);             // LED remains off.
        }
    }
}

/**
 * ------------------------------------------------
 *      Task - send GNSS numbers.
 * ------------------------------------------------
 *
 * Uses SparkFun_u-blox_GNSS_v3 library.
 * 
 * @param  void  * pvParameters Pointer to task parameters.
 * @return void  No output is returned.
 * @since  3.0.7 [2025-11-14-04:00pm] New.
 * @since  3.0.8 [2025-11-21-01:00pm] Display {GNSS update}.
 * @see    startTasks().
 * @link   https://www.freertos.org/Documentation/02-Kernel/04-API-references/02-Task-control/06-vTaskSuspend.
 * @link   https://github.com/sparkfun/SparkFun_u-blox_GNSS_v3/blob/main/examples/ZED-F9P/Example10_GetHighPrecisionPositionAndAccuracy/Example10_GetHighPrecisionPositionAndAccuracy.ino
 *         // TODO: check GNSS lock button is in upPosition (GNSS unlocked).
 */
void sendGnssTask(void * pvParameters) {
    while(true) {
        if (roverGNSS.getSIV() < MIN_SATELLITE_THRESHHOLD) {                    // Enough satellites?
            strcpy(fixStatus, "down");
            siv      = 0;
            lat      = 0;
            lon      = 0;
            alt      = 0;
            hAcc     = 0;
            vAcc     = 0;
        } else if (roverGNSS.getHPPOSLLH() == true) {                           // New high precision GNSS data is available.
            
            // Fix type: 0=none, 1=dead reckoning, 2=2D, 3=3D, 4=GNSS, 5=Time.
            int8_t fixType       = roverGNSS.getFixType();
            // Carrier solution: 0=none, 1=RTK float, 2=RTK fixed.
            int8_t solnType      = roverGNSS.getCarrierSolutionType();
            // Fix status.
            if (fixType == 3) {
                strcpy(fixStatus, "single");
            } else if (solnType == 1 ) {
                strcpy(fixStatus, "float");
            } else if (solnType == 2 ) {
                strcpy(fixStatus, "fix");
            }

            // Number of satellites in view.
            siv = roverGNSS.getSIV();

            // High precision latitude. 
            int32_t latitude = roverGNSS.getHighResLatitude();                  // degrees * 10^-7.
            int8_t latitudeHp = roverGNSS.getHighResLatitudeHp();               // degrees * 10^-9 (high precision component).
            lat = latitude / 10000000.0;                                        // Convert to to 64 bit double - degrees (8 decimal places).
            lat += latitudeHp / 1000000000.0;                                   // Add high precision component.

            // High precision longitude.
            int32_t longitude = roverGNSS.getHighResLongitude();
            int8_t longitudeHp = roverGNSS.getHighResLongitudeHp();
            lon = longitude / 10000000.0;
            lon += longitudeHp / 1000000000.0;

            // Altitude.
            int32_t ellipsoid = roverGNSS.getElipsoid();                        // mm.
            int8_t ellipsoidHp = roverGNSS.getElipsoidHp();                     // mm * 10^-1.
            float altEllipsoid = (ellipsoid * 10 + ellipsoidHp) / 10000.0;      // Convert to meters.
            int32_t msl = roverGNSS.getMeanSeaLevel();                          // mm.
            int8_t mslHp = roverGNSS.getMeanSeaLevelHp();                       // mm * 10^-1.
            float altMSL = (msl * 10 + mslHp) / 10000.0;                        // Convert to meters.
            // TODO: allow config change
            alt = altMSL;                                                       // float.

            // Accuracy.
            uint32_t horizontalAccuracy = roverGNSS.getHorizontalAccuracy() ;   // mm * 10^-1
            hAcc = horizontalAccuracy / 10000.0;                                // Convert to meters.
            uint32_t verticalAccuracy = roverGNSS.getVerticalAccuracy();
            vAcc = verticalAccuracy / 10000.0;
        }

        // -- Build reply. --
        jsonDocToClient.clear();
        jsonDocToClient["fix"]       = fixStatus;
        jsonDocToClient["siv"]       = siv;
        jsonDocToClient["altitude"]  = alt;
        jsonDocToClient["latitude"]  = lat;
        jsonDocToClient["longitude"] = lon;
        jsonDocToClient["hac"]       = hAcc;
        jsonDocToClient["vac"]       = vAcc;

        memset(buffer, '\0', sizeof(buffer));
        serializeJson(jsonDocToClient, buffer, sizeof(buffer));

        // -- Send reply. --
        ws.textAll(buffer);

        // -- Debug. -- 
        // Serial.printf("WebSocket #%u - browser <-- server %s\n", clientId, buffer);
        // Serial.printf("WebSocket #%u - browser <-- server {GNSS update}\n", clientId);
    }

    // -- Pause. --
    vTaskDelay(SEND_GNSS_PAUSE);               // Update interval.
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
 *      WiFi task - event handler.
 * ------------------------------------------------
 *
 * WARNING: This function is called from a separate FreeRTOS task (thread).
 *
 * @param  WiFiEvent_t event WiFi event object.
 * @return void No output is returned.
 * @since  3.0.8 [2025-11-21] New.
 * @see    startWiFiSoftAP().
 * @link   https://docs.espressif.com/projects/arduino-esp32/en/latest/api/wifi.html.
 */
void WiFiEvent(WiFiEvent_t event) {
  Serial.printf("[WiFi-event] event %d - ", event);
    switch (event) {
    case ARDUINO_EVENT_WIFI_READY:               Serial.println("WiFi interface ready"); break;
    case ARDUINO_EVENT_WIFI_SCAN_DONE:           Serial.println("Completed scan for access points"); break;
    case ARDUINO_EVENT_WIFI_AP_START:           Serial.println("WiFi access point started"); break;
    case ARDUINO_EVENT_WIFI_AP_STOP:            Serial.println("WiFi access point  stopped"); break;
    case ARDUINO_EVENT_WIFI_AP_STACONNECTED:    Serial.println("Client connected."); break;
    case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED: Serial.println("Client disconnected"); break;
    case ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED:   Serial.println("IP address assigned to client."); break;
    case ARDUINO_EVENT_WIFI_AP_PROBEREQRECVED:  Serial.println("Received probe request"); break;
    case ARDUINO_EVENT_WIFI_AP_GOT_IP6:         Serial.println("AP IPv6 is preferred"); break;
    default:                                    break;
  }
  Serial.printf("[WiFi-event] Clients connected: %i\n", WiFi.softAPgetStationNum());
}

/**
 * ------------------------------------------------
 *      DevUBLOXGNSS task - process NMEA bytes
 * ------------------------------------------------
 *
 * Send NMEA sentence to MCU #2 for BLE out.
 * 
 * roverGNSS.checkUblox() is not used in loop().
 * Error return values from Wire1.beginTransmission():
 *   1: Data too long to fit in transmit buffer.
 *   2: Received NACK on transmit of address: slave device at the specified address did not respond.
 *   3: Received NACK on transmit of data: slave device acknowledged its address but did not acknowledge the data sent.
 *   4: Other error. This could indicate a bus error, lost arbitration, etc.
 *
 * @param  char incoming character from checkUblox().
 * @return void No output is returned.
 * @since  3.0.8 [2025-11-21] New.
 * @since  3.0.9 [2025-12-02] Reworked.
 * @see    
 * @link   https://docs.espressif.com/projects/arduino-esp32/en/latest/api/wifi.html.
 */
void DevUBLOXGNSS::processNMEA(char incoming) {
    if (inLoop) {
        strncat(nmeaBuffer, &incoming, 1);                          // Add NMEA byte from RTK-SMA to outbound buffer.
        if ((incoming == '\n') && (nmeaBuffer[0] == '$')) {         // We have a full sentence.
            checkWire1();
            if (wire1Status == 1) {                                 // Slave is up.
                Wire1.beginTransmission(8);                         // Prepare to send.
                for (int i = 0; i <= strlen(nmeaBuffer); i++) {
                    Wire1.write(nmeaBuffer[i]);                     // Add bytes to output queue.
                }
                Wire1.endTransmission(8);                           // Send sentence.
                wire1Status = 1;                                    // Succesful I2C write. Slave is up.
                Serial.print('-');
            }
            memset(nmeaBuffer, '\0', sizeof(nmeaBuffer));
        }
    }
}


// ==================================
//         General functions.
// ==================================

/**
 * ------------------------------------------------
 *      HTTP endpoint - /upload.
 * ------------------------------------------------
 *
 * Send files to SD.
 *
 * @return void  No output is returned.
 * @since  3.0.7 [2025-11-11-06:00pm].
 * @see    startHttpServer().
 */
void handleFileUpload( AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
    static File uploadFile;                                 // HTTP upload file.

    // --- Start. ---
    if (index == 0) {
        Serial.println("\nhttpServer endpoint \"/upload\".\nhandleFileUpload() running.");
        SD.remove("/" + filename);                          // Delete file.
        Serial.printf("%s deleted on SD.\n", filename.c_str());
        uploadFile = SD.open("/" + filename, FILE_WRITE);   // Open file for writing.
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
 *      WebSocket event handler.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.3 [2025-11-08-03:15pm] New.
 * @since  3.0.8 [2025-12-01-05:15pm] Changed color & blink status.
 * @see    startWebSocketServer().
 */
void onWebSocketEvent(AsyncWebSocket *httpServer, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    clientId = client->id();
    switch (type) {
        case WS_EVT_CONNECT:
            Serial.printf("WebSocket #%u - %s connected to server \n", clientId, client->remoteIP().toString().c_str());
            ws2812LedColor = BLUE;                         // Loop status indicator LED.
            ws2812LedBlink = true;
            break;
        case WS_EVT_DISCONNECT:
            Serial.printf("WebSocket #%u - disconnected\n\n", clientId);
            ws2812LedColor = RED;
            ws2812LedBlink = true;
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
 * @link   https://arduinojson.org/v7/api/jsonvariant/.
 * @link   https://github.com/espressif/arduino-esp32/blob/master/libraries/SD/examples/SD_Test/SD_Test.ino.
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

            // --------------------------------------------
            // -- {"operate":"ready"}.
            // --------------------------------------------
            if ((strcmp(kv.key().c_str(), "operate") == 0) && (strcmp(kv.value().as<const char*>(), "ready") == 0 )) {

                // - Resume tasks. - 
                vTaskResume(sendGnssTaskHandle);
                vTaskResume(sendBatteryStatusTaskHandle);

                // - Reply with version, mode, & units. -
                sprintf(buffer, "%c.%c.%c", MAJOR_VERSION, MINOR_VERSION, PATCH_VERSION);
                jsonDocToClient["version"] = buffer;
                jsonDocToClient["mode"]    = operMode;
                jsonDocToClient["units"]   = operUnits;
            }

            // --------------------------------------------
            // -- {"main":"ready"}.
            // -- {"files":"ready"}.
            // -- {"config":"ready"}.
            // --------------------------------------------
            else if (strcmp(kv.value().as<const char*>(), "ready") == 0 ) {

                // - Suspend tasks. -
                vTaskSuspend(sendGnssTaskHandle);
                vTaskSuspend(sendBatteryStatusTaskHandle);

                // - Reply with ready. -
                jsonDocToClient["server"] = kv.value().as<const char*>();
            }

            // --------------------------------------------
            // -- {"main":"leaving"}.
            // -- {"files":"leaving"}
            // -- {"config":"leaving"}.
            // --------------------------------------------
            else if (strcmp(kv.value().as<const char*>(), "leaving") == 0 ) {

                // - No reply. -
                return;
            }

            // --------------------------------------------
            // -- {"altitude":"lock/unlock"}.
            // -- {"laser":"lock/unlock"}.
            // -- {"location":"lock/unlock"}.
            // --------------------------------------------
            else if ((strcmp(kv.value().as<const char*>(), "lock") == 0 ) ||
                     (strcmp(kv.value().as<const char*>(), "unlock") == 0 )) {
                if ((strcmp(kv.key().c_str(), "altitude") == 0) ||
                    (strcmp(kv.key().c_str(), "laser") == 0) ||
                    (strcmp(kv.key().c_str(), "location") == 0)) {

                    // - Reply with value sent + "ed". -
                    memset(buffer, '\0', sizeof(buffer));
                    strcpy(buffer, kv.value().as<const char*>());
                    strcat(buffer, "ed");
                    jsonDocToClient[kv.key().c_str()] = buffer;
                }
            }

            // --------------------------------------------
            // -- {"listFiles":""}.
            // --------------------------------------------
            if (strcmp(kv.key().c_str(), "listFiles") == 0) {

                // - Reply with list of SD files. -
                char output[2048];
                memset(output, '\0', sizeof(output));
                sdListFiles(output, sizeof(output), "/");                               // List files.
                jsonDocToClient[kv.key().c_str()] = output;
            }

            // --------------------------------------------
            // -- {"deleteFile":"filename"}.
            // --------------------------------------------
            if (strcmp(kv.key().c_str(), "deleteFile") == 0) {

                // - Reply with "deleted" or "NOT deleted". -
                if (SD.remove(kv.value().as<const char*>())) {                          // Delete file.
                    jsonDocToClient["fileDeleted"] = kv.value().as<const char*>();      // Success.
                } else {
                    jsonDocToClient["fileNOTdeleted"] = kv.value().as<const char*>();   // Fail.
                }
            }

            // --------------------------------------------
            // -- {"echo":"hello"}.
            // --------------------------------------------
            else if (strcmp(kv.key().c_str(), "echo") == 0) {

                // - Reply with value sent. -
                jsonDocToClient[kv.key().c_str()] = kv.value().as<const char*>();
            }

            // --------------------------------------------
            // -- {"mcu":"restart"}.
            // --------------------------------------------
            else if ((strcmp(kv.key().c_str(), "mcu") == 0) && (strcmp(kv.value().as<const char*>(), "restart") == 0 )) {

                // - Restart. -
                Serial.println("Restarting ...\n");
                esp_restart();                                                          // Restart.
            }

            // -- Send reply. --
            memset(buffer, '\0', sizeof(buffer));
            serializeJson(jsonDocToClient, buffer, sizeof(buffer));
            ws.textAll(buffer);
            Serial.printf("WebSocket #%u - browser <-- server %s\n", clientId, buffer);
        }
    }
}

// ==================================
//         Test functions.
// ==================================

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
char* simLatitude() {
    float num = (355530000 + random_in_range(6050, 6250))/10000000.0;
    memset(buffer, '\0', sizeof(buffer));
    sprintf(buffer, "%.7f", num);
    return buffer;       // E.g. 35.5536111).
}
char* simLongitude() {
    float num = (-787710000.0 - random_in_range(3500, 4500))/10000000.0;
    memset(buffer, '\0', sizeof(buffer));
    sprintf(buffer, "%.7f", num);
    return buffer;      // E.g. -78.7713888.
}
char* simAltitude() {
    float num = 100 + random_in_range(0, 50)/100.0;
    memset(buffer, '\0', sizeof(buffer));
    sprintf(buffer, "%.2f", num);
    return buffer;      // E.g. 103.56.
}
char* simHac() {
    float num = (random_in_range(10, 25))/100.0;
    memset(buffer, '\0', sizeof(buffer));
    sprintf(buffer, "%.2f", num);
    return buffer;      // E.g. 0.190.
}
char* simVac() {
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
 * @since  3.0.3 [2025-10-13-01:00pm] New.
 * @since  3.0.8 [2025-11-21-12:15pm] Removed startWiFiClient(), added startWiFiSoftAP().
 * @since  3.0.9 [2025-12-05-05:00pm] Split startI2C() into startWire() & startWire1().
 */

void setup() {
    startSerialUsbMonitor();            // Start serial USB monitor.
    initVars();                         // Initialize global vars.
    initPins();                         // Initialize pin modes & pin values.
    startSerialInterfaces();            // Start serial interfaces.
    startWire();                        // Start I2C Wire interface.
    startWire1();                       // Start I2C Wire1 interface.
    startLIPO();                        // Start LIPO I2C interface.
    startWiFiSoftAP();                  // Start WiFi soft AP.
    startSDIO();                        // Start SDIO for microSD reader.
    startSD();                          // Start built-in microSD card reader.
    testSD();                           // Test built-in microSD reader.
    startHttpServer();                  // Start HTTP server.
    startWebSocketServer();             // Start WebSocket server.
    startAndConfigGNSS();               // Start GNSS, config ZED settings.
    startTasks();                       // Start tasks.
    startLoop();                        // On to loop().
}

/**
 * ============================================================================
 *                              Tasks.
 * ============================================================================
 *
 * loopStatusLedTask()        Loop status LED - LED_TIME_FLASH_ON, LED_TIME_FLASH_OFF.
 * sendGnssTask()             Send GNSS data - SEND_GNSS_PAUSE.
 * sendBatteryStatusTask()    Send battery status - SEND_BATTERY_PAUSE.
 * WiFiEvent()                WiFi events.
 *
 * @since 3.0.8 [2025-11-14-02:00pm] New.
 */

/**
 * ============================================================================
 *                              Loop.
 * ============================================================================
 *
 * @since  3.0.3 [2025-10-13-01:00pm] New.
 */

void loop() {
    checkWire1();                       // Check I2C Wire1 interface.
    checkSerialMonitor();               // Check serial monitor (USB) for input.
    ws.cleanupClients();

    // New data is available? Process bytes as they come in.
    // roverGNSS.checkUblox();

    Serial.println("\npausing");
    delay(GNSS_SOLN_MS);  // Pause to match the solution rate.
}
