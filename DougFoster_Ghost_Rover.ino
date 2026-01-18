/**
 * **********************************************************************
 *          Ghost Rover 3 - GNSS "invisible" rover (with RTK base/PP) for SW Maps.
 * **********************************************************************
 *
 * @author   D. Foster <doug@dougfoster.me>.
 * @since    3.0.11 [2026-01-18-03:45pm].
 * @see      https://github.com/doug-foster/DougFoster_Ghost_Rover.
 * @see      https://github.com/doug-foster/DougFoster_Ghost_Rover_BT_relay.
 * @see      https://github.com/doug-foster/DougFoster_Ghost_Rover_EVK_RTCM_relay.
 * @link     http://dougfoster.me.
 */

/**
 * ----------------------------------------------------------------------------
 *                          Comments.
 * ----------------------------------------------------------------------------
 * 
 * --- Description & operation. ---
 *     -- Primary use is ... // ToDo: complete.
 *
 * --- Major components - rover. ---
 *     -- Board: "Sparkfun ESP32-S3 Thing Plus" (~/Library/Arduino15/packages/esp32/hardware/esp32/3.2.0/boards.txt)
 *     -- primary MCU        https://www.sparkfun.com/sparkfun-thing-plus-esp32-c6.html (SparkFun Thing Plus - ESP32-C6).
 *        -- micro SD card.  https://www.amazon.com/dp/B0BDYVC5TD (SanDisk 128GB ImageMate microSDXC UHS-1 - Up to 140MB/s).
 *     -- secondary MCU      https://www.sparkfun.com/sparkfun-qwiic-pocket-development-board-esp32-c6.html (Sparkfun Qwiic Pocket Development Board - ESP32-C6 - I2C address 0x36).
 *     -- GNSS               https://www.sparkfun.com/sparkfun-gps-rtk-sma-breakout-zed-f9p-qwiic.html (SparkFun GPS-RTK-SMA Breakout - ZED-F9P (Qwiic) - I2C address 0x42).
 *     -- HC-12 RF radio     https://www.amazon.com/dp/B01MYTE1XR (HiLetgo HC-12 433Mhz SI4438).
 *     -- laser pointer      https://www.petsmart.com/cat/toys/interactive-and-electronic/whisker-city-thrills-and-chills-laser-cat-toy-84577.html.
 * 
 * --- Major components - base. ---
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
 *     -- IDE        VS Code & Arduino Maker Workshop 1.0.8 extension (uses Arduino CLI 1.4).
 *     -- GitHub     https://github.com/doug-foster/DougFoster_Ghost_Rover/.
 *     -- Platform   https://github.com/espressif/arduino-esp32/releases/tag/3.3.3 (Arduino Release v3.3.3 based on ESP-IDF v5.5.1+).
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
 *  * @link     https://www.build-electronic-circuits.com/arduino-laser-module-ky-008/.
 *
 * --- Code structure. ---
 *     -- Include libraries.
 *     -- Global vars.
 *     -- Setup functions.
 *     -- Task functions.
 *     -- Event handlers.
 *     -- Loop functions.
 *     -- Setup.
 *     -- Loop.
 */

/**
 * ============================================================================
 *                          Include libraries.
 * ============================================================================
 *
 * @since 3.0.9   [2025-12-01-05:15pm].
 * @since 3.0.11  [2026-01-08-10:30am] Browser initiated updates.
 * @link  Arduino https://docs.arduino.cc/libraries/.
 * @link  ESP32   https://docs.espressif.com/projects/arduino-esp32/en/latest/libraries.html.
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
#include <esp_system.h>                                    // https://github.com/pycom/pycom-esp-idf.
#include <esp_chip_info.h>                                 // https://github.com/pycom/pycom-esp-idf.

// --- Additional. ---                  
#include <AsyncTCP.h>                                      // https://github.com/ESP32Async/AsyncTCP (3.4.9).
#include <ESPAsyncWebServer.h>                             // https://github.com/ESP32Async/ESPAsyncWebServer (3.8.1).
#include <ArduinoJson.h>                                   // https://github.com/bblanchon/ArduinoJson (7.4.2).
#include <SparkFun_MAX1704x_Fuel_Gauge_Arduino_Library.h>  // https://github.com/sparkfun/SparkFun_MAX1704x_Fuel_Gauge_Arduino_Library (1.0.4).
#include <SparkFun_u-blox_GNSS_v3.h>                       // https://github.com/sparkfun/SparkFun_u-blox_GNSS_v3 (3.1.13).

/**
 * ============================================================================
 *                          Global vars.
 * ============================================================================
 * 
 * --- ESP32 (Arduino framework) data types. ---
 *
 * -- Unsigned integer. --
 * uint8_t                      %u       8 bits = 1 byte,  0 to 255.
 * uint16_t/unsigned short      %u      16 bits = 2 bytes, 0 to 65,535.
 * uint32_t/unsigned long       %u,%lu  32 bits = 4 bytes, 0 to 4,294,967,295.
 * size_t (size,length,count)   %zu     32 bits = 4 bytes, 0 to 4,294,967,295.
 * uint64_t/unsigned long long  %llu    64 bits = 8 bytes, 0 to 18,446,744,073,709,551,615.
 
 * -- Signed integer. --
 * int8_t             %d       8 bits = 1 byte,             -128 to 127.
 * int16_t/short      %d      16 bits = 2 bytes,         -32,768 to 32,767.
 * int32_t/int/long   %d,%ld  32 bits = 4 bytes,  -2,147,483,648 to 2,147,483,647.
 * int64_t/long long  %lld    64 bits = 8 bytes,       -9.22e+18 to 9.22e+18.
 *
 * -- Signed decimal/floating point. --
 * float               %f      32 bits = 4 bytes,   6-7 sig. digits (hardware),  -3.40e+38 to 3.40e+38).
 * double/long double  %f,%lf  64 bits = 8 bytes, 15-17 sig. digits (software), -1.79e+308 to 1.79e+308).
 *
 * -- Character/text. --
 * char (signed)  %c  8 bit = 1 byte, -128 to 127.
 * unsigned char  %c  8 bit = 1 byte,    0 to 255.
 *
 * -- Other. --
 *      bool    %d (0/1) or %s (text)  8 bit = 1 byte, true or false.
 *      void        n/a.
 *      array       n/a.
 *      string  %s
 *
 * @since 3.0.10 [2026-01-06-10:00pm].
 * @since 3.0.11 [2026-01-08-10:30am] Browser initiated updates.
 */

// -- Pin assignments. --
const uint8_t HC12_SET    = 7;                      // HC-12 SET {blue wire}.
const uint8_t LSR_TRIGGER = 15;                     // KY-008 trigger pin {yellow wire}.

// --- LED. ---
bool ws2812LedBlink = false;
enum ws2812_LED_COLOR {                             // WS2812 RGB STAT LED.
    OFF,                                            // 0.
    RED,                                            // 1.
    YELLOW,                                         // 2.
    GREEN,                                          // 3.
    BLUE,                                           // 4.
    WHITE                                           // 5.
} ws2812LedColor;

// --- Battery. ---
SFE_MAX1704X lipo(MAX1704X_MAX17048);               // LiPo battery.

// --- WiFi. ---
const char SSID[]     = "Ghost Rover";
const char PASSWORD[] = "snark217$";
const char AP_NAME[]  = "ghost";

// --- HTTP. ---
const char     WEBSOCKET_SERVER_NAME[] = "/ghostRover";
      uint8_t  clientId                = 0;         // HTTP WebSocket client ID # (+1 for each new connection).
AsyncWebServer httpServer(80);                      // HTTP AsyncWebServer object on port 80.
AsyncWebSocket ws(WEBSOCKET_SERVER_NAME);           // HTTP WebSocket object.

// --- GNSS. ---
SFE_UBLOX_GNSS roverGNSS;                           // GNSS object (uses I2C-1).

// --- Task handles. ---
TaskHandle_t taskLoopStatusLedHandle;               // Task: Loop status LED.

// --- Timing. ---
const int64_t  NMEA_TIMEOUT   = 3000000;            // Time (us) not to exceed for ACK-NMEA-BT received from MCU2 (3 sec).

// --- Operation. ---
enum CommandIndex {                                 //  Readable index for command array.
    TEST_RAD = 0,                                   //  0.
    DEBUG_RTCM,                                     //  1.
    DEBUG_GNSS,                                     //  2.
    DEBUG_NMEA,                                     //  3.
    DEBUG_BTN,                                      //  4.
    DEBUG_BT,                                       //  5.
    DEBUG_SER,                                      //  6.
    DEBUG_WIFI,                                     //  7.
    DEBUG_WS,                                       //  8.
    DEBUG_LIPO,                                     //  9.
    SHOW_UPTIME,                                    // 10.
    RESET,                                          // 11.
    CHECK_WIRE1,                                    // 12.
    NUM_COMMANDS                                    // 13 = automatic array length.
};      
const char* COMMAND[NUM_COMMANDS] = {               // Command strings; match CommandIndex.
    "testRad",                                      // TEST_RAD
    "debugRTCM",                                    // DEBUG_RTCM
    "debugGNSS",                                    // DEBUG_GNSS
    "debugNMEA",                                    // DEBUG_NMEA
    "debugBtn",                                     // DEBUG_BTN
    "debugBT",                                      // DEBUG_BT
    "debugSer",                                     // DEBUG_SER
    "debugWiFi",                                    // DEBUG_WIFI
    "debugWs",                                      // DEBUG_WS
    "debugLiPo",                                    // DEBUG_LIPO
    "showUpTime",                                   // SHOW_UPTIME
    "reset",                                        // RESET
    "checkWire1"                                    // CHECK_WIRE1
};      
static size_t  wsSendCount = 0;                         // # of WebSocket messages sent.
       bool    commandFlag[NUM_COMMANDS] = {false}; // Command flags.
       bool    ghostMode = false;                   // Flag, in Ghost mode (i.e. locked coordinates).
       bool    i2cUp = false;                       // Status: true if both Wire & Wire1 up, else false.
       bool    inLoop = false;                      // In loop() indicator.
       bool    RTCMin = false;                      // RTCM received within RTCM_TIMEOUT?
       bool    NMEAsentByMCU2 = false;              // Ack received from MCU #2 within NMEA_TIMEOUT? 
       bool    buttonGnssLock;                      // UI - // ToDo: implement.
       bool    buttonAltitudeLock;                  // UI - // ToDo: implement.
       bool    buttonLocationLock;                  // UI - // ToDo: implement.
       bool    buttonLaser;                         // UI - // ToDo: implement.
       bool    buttonUnlockAll;                     // UI - // ToDo: implement.
       char    serialState[4];                      // Serial state: [USB] [S0] [S1] [S2]; value = u, d, or -.
       char    operMode[2];                         // Operation mode (r=rover, b=base).
       char    operUnit[2];                         // Operation units (m=meter, f=feet).
       char    buffer[256] = {'\0'};                // Utility buffer. // ToDo: Move to rtcm3GetMessageType().
       int64_t startTime;                           // Boot time.

// --- Version. ---
const uint8_t MAJOR_VERSION = 3;
const uint8_t MINOR_VERSION = 0;
const uint8_t PATCH_VERSION = 11;
const char    NAME[]        = "Ghost Rover 3";
const char    BUILD_DATE[]  = "[2026-01-18-03:45pm]";

// --- Declaration. ---
// --- Test. ---

/**
 * ============================================================================
 *                          Setup functions.
 * ============================================================================
 *
 * @since 3.0.11 [2026-01-08-10:30am] Browser initiated updates.
 * @see   showBuild()            - Display build & processor info.
 * @see   startSerial()          - Start serial interfaces.
 * @see   initPins()             - Initialize pins & pin values.
 * @see   startI2C()             - Start I2C Wire interfaces.
 * @see   startLiPo()            - Start LiPo I2C interface.
 * @see   startWiFi()            - Start WiFi.
 * @see   startSD()              - Start & test microSD card reader.
 * @see   startHttpServer()      - Start HTTP server.
 * @see   startWebSocketServer() - Start WebSocket server.
 * @see   startAndConfigGNSS()   - Start GNSS, config ZED settings.
 * @see   startTasks()           - Start tasks.
 * @see   preLoop()              - Prepare for loop().
 */

 /**
 * ------------------------------------------------
 *      Display build & processor info.
 * ------------------------------------------------
 * 
 * Default pins for ESP32-S3 Thing Plus using Arduino core:
 *   GPIO 19 - Serial USB UART0 used as Communication Device Class interface D- (negative data line).
 *   GPIO 20 - Serial USB UART0 used as Communication Device Class interface D+ (positive data line).
 * 
 * @return void  No output is returned.
 * @since  3.0.10 [2025-12-30-02:00pm].
 * @since  3.0.10 [2026-01-07-09:45am] Local vars.
 * @see    setup().
 * @link   https://github.com/pycom/pycom-esp-idf.
 */
void showBuild() {

    // --- Local vars. ---
    const uint32_t SERIAL_USB_SPEED = 115200;   // Serial USB speed.
          esp_chip_info_t chip_info;

    // --- Run. ---
    startTime = esp_timer_get_time();
    Serial.begin(SERIAL_USB_SPEED);
    serialState[0] = 'u';   // Serial USB is up [u] [S0] [S1] [S2].
    esp_chip_info(&chip_info);
    Serial.printf("\n%s, Version: %u.%u.%u, Build date: %s\n", NAME, MAJOR_VERSION, MINOR_VERSION, PATCH_VERSION, BUILD_DATE);
    Serial.printf("Using %s, Rev %d, %d core(s), ID (MAC) %012llX.\n", ESP.getChipModel(), chip_info.revision, chip_info.cores, ESP.getEfuseMac());
    Serial.println("\nSetup() started.");
    Serial.printf("Serial  (USB) started        @ %u bps.\n", SERIAL_USB_SPEED);
}

/**
 * ------------------------------------------------
 *      Start serial interfaces.
 * ------------------------------------------------
 * 
 * Default pins for ESP32-S3 Thing Plus using Arduino core:
 *   GPIO 43 - Serial1    UART1 TX.
 *   GPIO 44 - Serial1    UART1 RX.
 *   GPIO 17 - Serial2    UART2 TX (also default for I2C1).
 *   GPIO 18 - Serial2    UART2 RX (also default for I2C1).
 *
 * @return void  No output is returned.
 * @since  3.0.3  [2025-10-13-01:00pm].
 * @since  3.0.10 [2025-12-27-06:00pm] Add Serial2.
 * @since  3.0.10 [2025-12-30-02:00pm] Add Serial USB.
 * @since  3.0.10 [2026-01-07-09:45am] Local vars.
 * @see    showBuild().
 * @see    setup().
 * @link   https://github.com/G6EJD/ESP32-Using-Hardware-Serial-Ports.
 * @link   https://randomnerdtutorials.com/esp32-uart-communication-serial-arduino/#esp32-custom-uart-pins.
 */
void startSerial() {

    // --- Local vars. ---
    const uint8_t  HC12_TX       =  5;      // HC-12 TXD     {white wire}.
    const uint8_t  HC12_RX       =  6;      // HC-12 RXD     {yellow wire}.
    const uint8_t  ZED_RX2       = 17;      // ZED UART2 RX2 {white wire}.
    const uint8_t  ZED_TX2       = 16;      // ZED UART2 TX2 {yellow wire} (not used).
    const uint32_t SERIAL1_SPEED = 9600;    // HC-12 default speed is 9600.
    const uint32_t SERIAL2_SPEED = 57600;   // ZED UART2 default speed is 38400.

    // --- Start serial interfaces. ---
    serialState[1] = '-';   // Serial0 is not used. [USB][-][S1][S2]
    Serial1.begin(SERIAL1_SPEED, SERIAL_8N1, HC12_TX, HC12_RX);     // UART1 object. RX, TX.
    serialState[2] = 'u';   // Serial1 is up [USB][S0][u][S2].
    Serial.printf("Serial1 (HC-12) started     @   %i bps.\n", SERIAL1_SPEED);
    Serial2.begin(SERIAL2_SPEED, SERIAL_8N1, ZED_RX2, ZED_TX2);     // UART2 object. RX, TX.
    serialState[3] = 'u';   // Serial2 is up [USB][S0][S1][u].
    Serial.printf("Serial2 (ZED UART2) started @  %i bps.\n", SERIAL2_SPEED);
}

/**
 * ------------------------------------------------
 *      Initialize pins & pin values.
 * ------------------------------------------------
 *
 * @return void No output is returned.
 * @since  3.0.3  [2025-10-13-01:00pm].
 * @since  3.0.10 [2025-12-27-06:00pm] Add HC12_SET & LSR_TRIGGER.
 * @see    setup().
 */
void initPins() {
    pinMode(HC12_SET, OUTPUT);          // HC-12 - set pin for AT command mode.
    digitalWrite(HC12_SET, HIGH);       // HC-12 - initially set pin for transparent mode.
    pinMode(LSR_TRIGGER, OUTPUT);       // KY-008 trigger pin.
    Serial.println("Init pins.");
}

/**
 * ------------------------------------------------
 *      Start I2C Wire interfaces.
 * ------------------------------------------------
 * 
 * Default pins for ESP32-S3 Thing Plus using Arduino core:
 *   GPIO  8 - SDA for I2C0 {Qwiic}.
 *   GPIO  9 - SCL for I2C0 {Qwiic}.
 *   GPIO 17 - SDA for I2C1 {PTH} (also default for Serial2 TX).
 *   GPIO 18 - SCL for I2C1 {PTH} (also default for Serial2 RX).
 *
 * @return void  No output is returned.
 * @since  3.0.9  [2025-12-05-05:00pm] New.
 * @since  3.0.10 [2025-12-27-07:00pm] Combine wire & wire1.
 * @since  3.0.10 [2026-01-07-10:00am] Local vars.
 * @see    setup().
 * @link   https://github.com/espressif/arduino-esp32/blob/master/libraries/Wire/src/Wire.h.
 * @link   https://docs.arduino.cc/language-reference/en/functions/communication/wire/. 
 */
void startI2C() {

    // --- Local vars. ---
    const uint8_t  I2C0_SDA   =  6;       // Primary I2C bus - data.
    const uint8_t  I2C0_SCL   =  7;       // Primary I2C bus - clock.
    const uint8_t  I2C1_SDA   = 14;       // Secondary I2C bus - data.
    const uint8_t  I2C1_SCL   = 10;       // Secondary I2C bus - clock.
    const uint16_t RETRY      = 500;      // Try restarting I2C interfaces.
    const uint32_t WIRE_SPEED = 400000;   // I2C Fast mode (4kHz).

    // --- Start interfaces. ---
    i2cUp = false;
    if ((Wire.begin()) && (Wire1.begin(I2C1_SDA,I2C1_SCL))) {
        Wire.setClock(WIRE_SPEED);
        Wire1.setClock(WIRE_SPEED);
        Serial.printf("Wire & Wire1 started @ 4kHz.\n");
        i2cUp = true;
    } else {
        Serial.println("Wire & Wire1 failed to start. Retrying.");
        delay(RETRY);
        startI2C();
    };

    // --- Register event functions. ---
}

/**
 * ------------------------------------------------
 *      Start LiPo I2C interface.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.7  [2025-11-09-10:15pm].
 * @since  3.0.10 [2026-01-06-11:15am]. Spelling, move lipo.enableDebugging().
 * @see    setup().
 * @link   https://github.com/sparkfun/SparkFun_MAX1704x_Fuel_Gauge_Arduino_Library.
 */
void startLiPo() {
    if (lipo.begin() == false) {    // Uses I2C0.
        Serial.println("LiPo not started. MAX17048 not detected.");
    } else {
        lipo.quickStart();          // Restart for a more accurate initial SOC guess.
        Serial.println("LiPo started.");
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
 * @link   https://github.com/espressif/arduino-esp32/tree/master/libraries/WiFi.
 * @link   https://docs.espressif.com/projects/arduino-esp32/en/latest/api/wifi.html.
 */
// void startWiFiClient() {
    // WiFi.mode(WIFI_STA);        // Station mode.
    // WiFi.config (staticIP);
    // WiFi.begin(SSID, PASSWORD);
    // Serial.printf("WiFi started.\nConnecting to SSID = %s ", SSID);
    // while (WiFi.status() != WL_CONNECTED) {
    //     Serial.print('.');
    //     delay(1000);            // Try again.
    // }
    // Serial.printf("\nConnected, IP = %s .\n", WiFi.localIP().toString().c_str());
// }

/**
 * ------------------------------------------------
 *      Start WiFi.
 * ------------------------------------------------
 *
 * ToDo: change to AP + STA
 *
 * @return void  No output is returned.
 * @since  3.0.7  [2025-11-20-12:30pm]. New.
 * @since  3.0.10 [2026-01-07-11:00am] Local vars.
 * @see    setup().
 * @link   https://github.com/espressif/arduino-esp32/tree/master/libraries/WiFi.
 * @link   https://docs.espressif.com/projects/arduino-esp32/en/latest/api/wifi.html.
 * @link   https://docs.espressif.com/projects/arduino-esp32/en/latest/api/network.html.
 */
void startWiFi() {

    // --- Local vars. ---
    // ToDo: change vars to const.
    IPAddress ap_local_IP(192, 168, 23, 1);     // ESP32 access point (AP) IP address.
    IPAddress ap_gateway(192, 168, 23, 1);      // Gateway address.
    IPAddress ap_subnet(255, 255, 255, 0);      // Subnet mask.

    // --- Run. ---
    if (!WiFi.softAPConfig(ap_local_IP, ap_gateway, ap_subnet)) {       // Configure IP.
        Serial.println("Soft AP - config failed.");
    }
    if (!WiFi.softAP(SSID)) {                                           // Set SSID. No password.
        Serial.println("Soft AP - create failed. Freezing.");
        while (true);
    }
    WiFi.softAPsetHostname(AP_NAME);                                    // Set hostname.
    IPAddress ip = WiFi.softAPIP();                                     // Start WiFi & check status (get IP).
    Serial.printf("WiFi AP %d.%d.%d.%d \"%s\" started.\n", ip[0], ip[1], ip[2], ip[3], AP_NAME);
    WiFi.onEvent(onWiFiEvent);                                          // Add WiFiEvent() as event handler.
}

/**
 * ------------------------------------------------
 *      Start & test microSD card reader.
 * ------------------------------------------------
 *
 * Using SanDisk 128GB ImageMate microSDXC UHS-1 - Up to 140MB/s.
 * 
 * Default pins for ESP32-S3 Thing Plus using Arduino core:
 *   GPIO 33 - SDIO3.
 *   GPIO 34 - SDIO_CMD.
 *   GPIO 38 - SDIO_CLK.
 *   GPIO 39 - SDIO0.
 *   GPIO 40 - SDIO1.
 *   GPIO 47 - SDIO2.
 *   GPIO 48 - SDIO_~{DET}.
 * 
 * @return void  No output is returned.
 * @since  3.0.3 [2025-10-13-01:00pm].
 * @since  3.0.10 [2026-01-07-11:30am] Local vars.
 * @see    setup().
 * @link   https://github.com/espressif/arduino-esp32/tree/master/libraries/SD.
 * @link   https://github.com/espressif/arduino-esp32/tree/master/libraries/FS.
 * @link   https://github.com/espressif/arduino-esp32/tree/master/libraries/SPI.
 * @link   https://docs.sparkfun.com/SparkFun_Thing_Plus_ESP32-S3/hardware_overview/#sd-card-slot.
 * @link   https://randomnerdtutorials.com/arduino-ide-2-install-esp32-littlefs/.
 */
void startSD() {

    // --- Local vars. ---
    const uint8_t  SPI_CS      = 33;                // SPI chip select.
    const uint8_t  SPI_PICO    = 34;                // microSD SDI.
    const uint8_t  SPI_SCK     = 38;                // ESP32-S3 Thing+ SPI serial clock.
    const uint8_t  SPI_POCI    = 39;                // microSD SDO.
    const uint16_t STARTUP     = 750;               // Allow time for SDIO to start up.
    const char     TEST_FILE[] = "/index.html";     // Test file (should always exist).

    // --- Start SDIO interface. ---
    if (!SPI.begin(SPI_SCK, SPI_POCI, SPI_PICO, SPI_CS)) {
        Serial.println("SDIO not started. Freezing.");
        while (true);
    }
    Serial.println("SDIO started.");

    // --- Start SD reader. ---
    delay(STARTUP);
    if (!SD.begin(SPI_CS)) {
        Serial.println("SD card not started. Freezing.");
        while (true);
    }
    Serial.println("SD card started.");

    // --- Test card. ---
    delay(STARTUP);
    Serial.print("SD card test - ");
    File file = SD.open(TEST_FILE, "r");
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
 * @since  3.0.10 [2026-01-07-11:30am] Local vars.
 * @see    setup().
 * @see    onHttpFileUpload().
 * @link   https://github.com/ESP32Async/ESPAsyncWebServer/wiki#get-post-and-file-parameters.
 * @link   https://github.com/ESP32Async/AsyncTCP.
 * @link   https://github.com/ESP32Async/ESPAsyncWebServer.
 */
void startHttpServer() {

    // --- Local vars. ---
    const char PAGE_ROOT[]     = "/";
    const char PAGE_UPLOAD[]   = "/upload";
    const char PAGE_DOWNLOAD[] = "/download";

    // --- Set root/home page endpoint. ---
    httpServer.on(PAGE_ROOT, HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SD, "/index.html", "text/html");      // Set root.
        Serial.println("httpServer requested \"/\", sent \"/index.html\"");
    }) ;

    // --- Set upload page endpoint. ---   
    httpServer.on(PAGE_UPLOAD, HTTP_POST, [](AsyncWebServerRequest *req) {
        req->send(200, "text/plain", "Upload complete");
        Serial.println("httpServer upload complete.");
    }, onHttpFileUpload);                                   // Register endpoint handler.

    // --- Download page endpoint. ---
    httpServer.on(PAGE_DOWNLOAD, HTTP_GET, [](AsyncWebServerRequest *request) { 
        if (request->hasParam("file")) {                    // Process request.
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

    // --- Start server. ---
    httpServer.serveStatic(PAGE_ROOT, SD, PAGE_ROOT);       // File system root ("/") is on SD card.
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
 * @since  3.0.10 [2026-01-07-12:00pm] WEBSOCKET_SERVER_NAME.
 * @see    setup().
 * @see    onWebSocketEvent().
 * @link   https://randomnerdtutorials.com/esp32-websocket-server-arduino/.
 * @link   https://shawnhymel.com/1882/how-to-create-a-web-server-with-websockets-using-an-esp32-in-arduino/.
 */
void startWebSocketServer() {
    ws.onEvent(onWebSocketEvent);
    httpServer.addHandler(&ws);     // startWebServer() must run first.
    Serial.printf("WebSocket server \"%s\" started.\n", WEBSOCKET_SERVER_NAME);
}

/**
 * ------------------------------------------------
 *      Start GNSS, config ZED settings.
 * ------------------------------------------------
 * 
 * Uses library SparkFun_u-blox_GNSS_v3 for UBX-CFG-VALGET & UBX-CFG-VALSET binary commands.
 * 
 * SW Maps needs these (5) sentences:
 *   Position & time: GNGGA, GNRMC.
 *   Skyplot display: GNGSA, GPGSV (significant bandwidth).
 *   Accuracy: GNGST.
 *
 * @return void No output is returned.
 * @since  0.1.0  [2025-04-24-12:00pm] New.
 * @since  3.0.7  [2025-11-14-04:00pm] Import from Ghost Rover V2.
 * @since  3.0.11 [2026-01-14-10:45am] Cleanup.
 * @see    Global vars: GNSS.
 * @see    startSerial().
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

    // --- Local vars. ---
    const uint8_t NAV_FREQ = 5;                                     // Generate a solution 5 times per second (interval = 200ms).
    
    // --- Start GNSS interface on I2C-1. ---
    if (roverGNSS.begin() == false) {
        Serial.println("Start roverGNSS failed. Freezing ...");     // Something is wrong, freeze.
        while (true);                                               // Infinite loop.
    } else {
        roverGNSS.setNavigationFrequency(NAV_FREQ);                 // Set solution interval.
        Serial.printf("roverGNSS started @ %u solutions per second.\n", roverGNSS.getNavigationFrequency());
    }

    // --- Configure ZED. ---
    roverGNSS.newCfgValset(VAL_LAYER_RAM);                          // New config template.
    // roverGNSS.newCfgValset(VAL_LAYER_RAM_BBR);
    roverGNSS.addCfgValset(UBLOX_CFG_NMEA_HIGHPREC,          1);    // High precision NMEA.  9 decimal places.
    roverGNSS.setI2COutput(COM_TYPE_NMEA | COM_TYPE_UBX);           // Enable both UBX & NMEA messages.
    roverGNSS.addCfgValset(UBLOX_CFG_MSGOUT_UBX_NAV_PVT_I2C, 1);    // Output solutions periodically on I2C.
    roverGNSS.addCfgValset(UBLOX_CFG_MSGOUT_NMEA_ID_GGA_I2C, 1);    // Enable GGA messages on I2C @ 1 per 1 solution.
    roverGNSS.addCfgValset(UBLOX_CFG_MSGOUT_NMEA_ID_RMC_I2C, 1);    // Enable RMC messages on I2C @ 1 per 1 solution.
    roverGNSS.addCfgValset(UBLOX_CFG_MSGOUT_NMEA_ID_GSA_I2C, 1);    // Enable GSA messages on I2C @ 1 per 1 solution.
    roverGNSS.addCfgValset(UBLOX_CFG_MSGOUT_NMEA_ID_GSV_I2C, 5);    // Enable GSV messages on I2C @ 1 per 5 solutions (minimize bandwidth).
    roverGNSS.addCfgValset(UBLOX_CFG_MSGOUT_NMEA_ID_GST_I2C, 1);    // Enable GSA messages on I2C @ 1 per 1 solution.
    roverGNSS.addCfgValset(UBLOX_CFG_MSGOUT_NMEA_ID_GLL_I2C, 0);    // Disable GLL messages on I2C.
    roverGNSS.addCfgValset(UBLOX_CFG_MSGOUT_NMEA_ID_VTG_I2C, 0);    // Disable GLL messages on I2C.
                                                                    // ZDA & GNS sentences are off by default.
    // roverGNSS.addCfgValset(UBLOX_CFG_I2CINPROT_UBX,       1);    // I2C - Turn on UBX protocol in. Default is on.
    // roverGNSS.addCfgValset(UBLOX_CFG_I2COUTPROT_UBX,      1);    // I2C - Turn on UBX protocol out. Default is on.
    // roverGNSS.addCfgValset(UBLOX_CFG_I2CINPROT_NMEA,      0);    // I2C - Turn off NMEA protocol in. Default is on.
    // roverGNSS.addCfgValset(UBLOX_CFG_I2COUTPROT_NMEA,     0);    // I2C - Turn on NMEA protocol out. Default is on.
    // roverGNSS.addCfgValset(UBLOX_CFG_I2CINPROT_RTCM3X,    0);    // I2C - Turn off RTCM3 protocol in. Default is ?.
    // roverGNSS.addCfgValset(UBLOX_CFG_I2COUTPROT_RTCM3X,   0);    // I2C - Turn off RTCM3 protocol out. Default is ?.
    // roverGNSS.addCfgValset(UBLOX_CFG_UART1_ENABLED,       0);    // UART1 - Disable. Default is enabled.
    roverGNSS.sendCfgValset() ? Serial.println("roverGNSS configured using valset keys.") : Serial.println("roverGNSS config failed."); // Send the config.
    // roverGNSS.saveConfiguration();                               // Save current settings to BBR/Flash.
    // roverGNSS.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT);        // Save port settings to flash and BBR.
    // roverGNSS.enableDebugging();                                 // Debug - all messages over Serial (default).
}

/**
 * ------------------------------------------------
 *      Start tasks.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.7  [2025-11-14-04:30pm].
 * @since  3.0.11 [2026-01-08-10:30am] Remove taskSendGnss() & taskSendBatteryStatus().
 * @see    Global vars: Task handles.
 * @see    setup().
 * @link   https://www.freertos.org/Documentation/02-Kernel/04-API-references/01-Task-creation/01-xTaskCreate.
 */
void startTasks() {

    // --- Loop status LED. ---
    xTaskCreate(taskLoopStatusLed, "Loop status LED", 2048, NULL, 2, &taskLoopStatusLedHandle);
    Serial.println("Task \"Loop status LED\" started.");
}

/**
 * ------------------------------------------------
 *      Prepare for loop().
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.7 [2025-11-07-12:30pm].
 * @since  3.0.7 [2025-11-21-06:00pm] Added inLoop.
 * @see    setup().
 */
void preLoop() {
    ws2812LedColor = RED;
    ws2812LedBlink = true;
    operMode[0]    = 'b';
    operUnit[0]    = 'm';
    inLoop         = true;
    Serial.println("Loop() starting.");
}

/**
 * ============================================================================
 *                          Task functions.
 * ============================================================================
 *
 * @since 3.0.11 [2026-01-08-10:30am] Browser initiated updates.
 * @see   startTasks()        - Start tasks in setup().
 * @see   taskLoopStatusLed() - Status LED for loop().
 */

/**
 * ------------------------------------------------
 *      Task: Loop() status LED.
 * ------------------------------------------------
 * 
 * Default pins for ESP32-S3 Thing Plus using Arduino core:
 *   GPIO 46 - green Status (STAT) LED.
 *   GPIO  2 - WS2812 LED.
 *   GPIO 23 - RGB BUILTIN LED.
 *
 * @param  void  * pvParameters Pointer to task parameters.
 * @return void  No output is returned.
 * @since  3.0.3  [2025-11-09-10:30am] New.
 * @since  3.0.10 [2026-01-07-09:00am] Local vars.
 * @since  3.0.11 [2026-01-08-02:30pm] Remove debug.
 * @see    startTasks().
 * @link   https://www.freertos.org/Documentation/02-Kernel/04-API-references/02-Task-control/06-vTaskSuspend.
 */
void taskLoopStatusLed(void * pvParameters) {

    // --- Local vars. ---
    const uint8_t    BRIGHT        = 18;                        // 0-255.
    const TickType_t TIME_ON       = 100/portTICK_PERIOD_MS;    // Timer (ms) =  0.1 seconds.
    const TickType_t TASK_INTERVAL = 1000/portTICK_PERIOD_MS;   // Timer (ms) =  1.0 seconds.

    // --- Loop. ---
    while(true) {
        switch (ws2812LedColor) {                               // Set color.
            case RED:                                           // 1.
                rgbLedWrite(LED_BUILTIN, BRIGHT, 0, 0);         // Pin, Red, green, blue.
                break;
            case YELLOW:                                        // 2.
                rgbLedWrite(LED_BUILTIN, BRIGHT, BRIGHT, 0);
                break;
            case GREEN:                                         // 3.
                rgbLedWrite(LED_BUILTIN, 0, BRIGHT, 0);
                break;
            case BLUE:                                          // 4.
                rgbLedWrite(LED_BUILTIN, 0, 0, BRIGHT);
                break;
            case WHITE:                                         // 5.
                rgbLedWrite(LED_BUILTIN, 0, 0, 0);
                break;
        }
        vTaskDelay(TIME_ON);                                    // LED remains on: 0.1 seconds.
        if (ws2812LedBlink) {
            rgbLedWrite(LED_BUILTIN, 0, 0, 0);                  // LED off.
        }
        vTaskDelay(TASK_INTERVAL);                              // Task interval: every 1.0 seconds.
    }
}

/**
 * ============================================================================
 *                          Event handlers.
 * ============================================================================
 *
 * @since 3.0.11 [2026-01-12-06:00pm] Browser initiated updates.
 * @see   onWiFiEvent()               - WiFi event handler.
 * @see   onHttpFileUpload()          - HTTP server endpoint handler.
 * @see   onWebSocketEvent()          - WebSocket event handler.
 * @see   onWebSocketMessage()        - WebSocket message event handler.
 * @see   DevUBLOXGNSS::processNMEA() - DevUBLOXGNSS: process NMEA bytes.
 */

 /**
 * ------------------------------------------------
 *      WiFi task - event handler.
 * ------------------------------------------------
 *
 * @param  WiFiEvent_t event WiFi event object.
 * @return void No output is returned.
 * @since  3.0.8 [2025-11-21] New.
 * @see    startWiFiSoftAP().
 * @link   https://docs.espressif.com/projects/arduino-esp32/en/latest/api/wifi.html.
 */
void onWiFiEvent(WiFiEvent_t event) {
    if (commandFlag[DEBUG_WIFI]) {
        Serial.printf("[WiFi-event] event %d - ", event);
        switch (event) {
            case ARDUINO_EVENT_WIFI_READY:              Serial.println("WiFi interface ready"); break;
            case ARDUINO_EVENT_WIFI_SCAN_DONE:          Serial.println("Completed scan for access points"); break;
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
}

/**
 * ------------------------------------------------
 *      httpServer endpoint "/upload".
 * ------------------------------------------------
 *
 * Upload a file to the SD card.
 *
 * @return void   No output is returned.
 * @since  3.0.7  [2025-11-11-06:00pm].
 * @since  3.0.10 [2026-01-07-12:00pm] Local vars.
 * @see    startHttpServer().
 * @link   https://randomnerdtutorials.com/esp32-async-web-server-espasyncwebserver-library/.
 */
void onHttpFileUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {

    // --- Local vars. ---
    static File uploadFile;                                 // HTTP upload file.

    // --- Begin. ---
    if (index == 0) {                                       // Start.
        Serial.println("\nhttpServer endpoint \"/upload\".\nonHttpFileUpload() running.");
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

    // --- Continue (write data to SD). ---
    if (len) {                                              // Data chunk.                                            
        uploadFile.write(data, len);                        // Write received data to file.
        Serial.printf("%u total bytes written.\n", (unsigned int)(index + len));
    }

    // --- Finish. ---
    if (final) {                                            // Complete.
        uploadFile.close();
        Serial.printf("%s closed on SD.\n", filename.c_str());
        request->send(200, "text/plain", "Upload complete. File saved to SD.");
    }
}

/**
 * ------------------------------------------------
 *      Event - WebSocket event handler.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.3 [2025-11-08-03:15pm] New.
 * @since  3.0.8 [2025-12-01-05:15pm] Changed color & blink status.
 * @see    startWebSocketServer().
 * @link   https://randomnerdtutorials.com/esp32-websocket-server-arduino/.
 * @link   https://shawnhymel.com/1882/how-to-create-a-web-server-with-websockets-using-an-esp32-in-arduino/.
 */
void onWebSocketEvent(AsyncWebSocket *httpServer, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    clientId = client->id();
    switch (type) {
        case WS_EVT_CONNECT:
            Serial.printf("WS #%u: %s connected to server.\n", clientId, client->remoteIP().toString().c_str());
            ws2812LedColor = BLUE;                          // Loop status indicator LED.
            ws2812LedBlink = true;
            wsSendCount    = 0;                             // Reset counter.
            break;
        case WS_EVT_DISCONNECT:
            Serial.printf("WS #%u: disconnected.\n\n", clientId);
            ws2812LedColor = RED;
            ws2812LedBlink = true;
            wsSendCount    = 0;                             // Reset counter.
            break;
        case WS_EVT_DATA:
            onWebSocketMessage(arg, data, len);             // Register WebSocket message handler.
            break;
        case WS_EVT_PONG:
        case WS_EVT_ERROR:
            break;
    }
}

/**
 * ------------------------------------------------
 *      Event - WebSocket message.
 * ------------------------------------------------
 * 
 * serializeJson(jsonDocToClient, Serial); // Debug.
 * Serial.println();
 *
 * @return void  No output is returned.
 * @since  3.0.7  [2025-11-10-12:00pm].
 * @since  3.0.10 [2026-01-07-02:30pm] Change {"operate":"ready"} to {"operate":"update"}.
 * @since  3.0.10 [2026-01-08-09:30am] Shortened keywords (e.g. latitude to lat).
 * @since  3.0.11 [2026-01-08-10:30am] Browser initiated updates.
 * @see    startWebSocketServer().
 * @see    onWebSocketEvent().
 * @see    Global vars.
 * @link   https://randomnerdtutorials.com/esp32-websocket-server-arduino/.
 * @link   https://randomnerdtutorials.com/esp32-websocket-server-sensor/.
 * @link   https://shawnhymel.com/1882/how-to-create-a-web-server-with-websockets-using-an-esp32-in-arduino/.
 * @link   https://arduinojson.org/v6/api/json/deserializejson/.
 * @link   https://arduinojson.org/v6/doc/deserialization/.
 * @link   https://arduinojson.org/v7/api/jsonvariant/.
 * @link   https://github.com/espressif/arduino-esp32/blob/master/libraries/SD/examples/SD_Test/SD_Test.ino.
 */

void onWebSocketMessage(void *arg, uint8_t *data, size_t len) {

    // --- Local vars. ---
    const size_t       WS_LIPO_SEND_COUNT       = 20;
    const int8_t       MIN_SATELLITE_THRESHHOLD =  2;   // Minimum SIV for reliable coordinate information.
          char         JSONbuffer[256];
          char         numberbuffer[64];
          uint32_t     lastGNGGA;                       // Last time a $GNGGA was received.  // ToDo: remove, not used?
          AwsFrameInfo *info                    = (AwsFrameInfo*)arg;
          JsonDocument jsonDocFromClient;               // JSON document received from client.
          JsonDocument jsonDocToClient;                 // JSON document sent to client.

    // --- Read WebSocket JSON message. ---
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {

        // -- Deserialize the JSON data into jsonDocFromClient. --
        jsonDocFromClient.clear();
        DeserializationError error = deserializeJson(jsonDocFromClient, data);
        if (error) {
            Serial.printf("JSON deserialize failed: %s\n", error.f_str());
            return;
        }

        // -- Create JSON key/value pairs. --
        memset(JSONbuffer, '\0', sizeof(JSONbuffer));
        serializeJson(jsonDocFromClient, JSONbuffer, sizeof(JSONbuffer));

        // -- Debug. --
        if (commandFlag[DEBUG_WS]) {
            Serial.printf("WS #%u: browser --> %s\n", clientId, JSONbuffer);
        }
        
        // -- Process each JSON key/value pair. --
        for (JsonPair kv : jsonDocFromClient.as<JsonObject>()) {
            jsonDocToClient.clear();

            // --------------------------------------------
            //  browser --> {"operate":"update"}.
            //  browser <-- {"ver":"3.0.3","mode":"b","unit":"m","fix":"1","siv":"28","lat":"35.5536111","lon":"-78.7713888","alt":"100.05"},"hac":"0.016","vac":"0.014","bat":"97.50","batc":"-3.1"}.
            // --------------------------------------------
            if ((strcmp(kv.key().c_str(), "operate") == 0) && (strcmp(kv.value().as<const char*>(), "update") == 0 )) {

                  // NMEA_GGA_data_t data;
                  // NMEA_VTG_data_t dataVTG;
                  // uint8_t result = 0;
                  // result = roverGNSS.getLatestNMEAGNGGA(&data);
                  // Serial.printf("%u %s",result, (const char *)data.nmea);
                  // result = roverGNSS.getLatestNMEAGNVTG(&dataVTG);
                  // Serial.printf("%u %s",result, (const char *)dataVTG.nmea);
                  // return;
                
                // - Send version, mode, & units only on first {"operate":"update"} message. -
                if (wsSendCount == 0) {
                    sprintf(JSONbuffer, "%u.%u.%u", MAJOR_VERSION, MINOR_VERSION, PATCH_VERSION);
                    jsonDocToClient["ver"] = JSONbuffer;
                    jsonDocToClient["mode"] = operMode;
                    jsonDocToClient["unit"] = operUnit;
                }

                // - Send battery status every WS_LIPO_SEND_COUNT {"operate":"update"} messages. -
                if (wsSendCount % WS_LIPO_SEND_COUNT == 0) {
                    memset(numberbuffer, '\0', sizeof(numberbuffer));
                    sprintf(numberbuffer, "%.2f", lipo.getSOC());
                    jsonDocToClient["bat"] = numberbuffer;
                    memset(numberbuffer, '\0', sizeof(numberbuffer));
                    sprintf(numberbuffer, "%.1f", lipo.getChangeRate());
                    jsonDocToClient["batc"] = numberbuffer;                   
                }

                // - Send GNSS data for evey {"operate":"update"} message. -
                if (roverGNSS.getSIV() < MIN_SATELLITE_THRESHHOLD) {        // Enough satellites?
                    jsonDocToClient["fix"]  = 0;                            // GNSS down. 
                    jsonDocToClient["siv"]  = 0;
                    jsonDocToClient["lat"]  = 0;
                    jsonDocToClient["long"] = 0;
                    jsonDocToClient["alt"]  = 0;
                    jsonDocToClient["hac"]  = 0;
                    jsonDocToClient["vac"]  = 0;
                } else if (roverGNSS.getHPPOSLLH() == true) {               // New high precision GNSS data is available.
                    // Fix type.
                    if (roverGNSS.getFixType() == 3) {
                        jsonDocToClient["fix"] = 1;                         // Single.
                    } else if (roverGNSS.getCarrierSolutionType() == 1 ) {
                        jsonDocToClient["fix"] = 2;                         // RTK-float.
                    } else if (roverGNSS.getCarrierSolutionType() == 2 ) {
                        jsonDocToClient["fix"] = 3;                         // RTK-fix.
                    }
                    // Satellites in view.
                    jsonDocToClient["siv"] = roverGNSS.getSIV();            
                    // Latitude.
                    int32_t latitude   = roverGNSS.getHighResLatitude();    // Degrees * 10^-7.
                    int8_t  latitudeHp = roverGNSS.getHighResLatitudeHp();  // High precision component: degrees * 10^-9.
                    double  lat        = latitude / 10000000.0;             // Convert to to 64 bit double - degrees (8 decimal places).
                    lat += latitudeHp / 1000000000.0;                       // Add high precision component.
                    memset(numberbuffer, '\0', sizeof(numberbuffer));
                    sprintf(numberbuffer, "%.8f", lat);
                    jsonDocToClient["lat"] = numberbuffer;
                    // Longitude.
                    int32_t longitude   = roverGNSS.getHighResLongitude();
                    int8_t  longitudeHp = roverGNSS.getHighResLongitudeHp();
                    double  lon         = longitude / 10000000.0;
                    lon += longitudeHp / 1000000000.0;
                    memset(numberbuffer, '\0', sizeof(numberbuffer));
                    sprintf(numberbuffer, "%.8f", lon);
                    jsonDocToClient["lon"] = numberbuffer;
                    // Altitude.
                    int32_t msl   = roverGNSS.getMeanSeaLevel();            // mm. 
                    int8_t  mslHp = roverGNSS.getMeanSeaLevelHp();          // mm * 10^-1.
                    float   alt   = (msl * 10 + mslHp) / 10000.0;           // Convert to meters.
                    memset(numberbuffer, '\0', sizeof(numberbuffer));
                    sprintf(numberbuffer, "%.2f", alt);
                    jsonDocToClient["alt"] = numberbuffer;
                    // Ellipsoid height.  // ToDo: Not used. allow config change.
                      // int32_t ellipsoid  = roverGNSS.getElipsoid();
                      // int8_t ellipsoidHp = roverGNSS.getElipsoidHp();
                      // float altEllipsoid = (ellipsoid * 10 + ellipsoidHp) / 10000.0;
                    // Horizontal accuracy.
                    memset(numberbuffer, '\0', sizeof(numberbuffer));
                    sprintf(numberbuffer, "%.3f", (roverGNSS.getHorizontalAccuracy() / 10000.0));
                    jsonDocToClient["hac"] = numberbuffer;
                    // Vertical accuracy.
                    memset(numberbuffer, '\0', sizeof(numberbuffer));
                    sprintf(numberbuffer, "%.3f", (roverGNSS.getVerticalAccuracy() / 10000.0));
                    jsonDocToClient["vac"] = numberbuffer;
                }
            }

            // --------------------------------------------
            //  browser --> {"main/files/config":"ready"}.
            //  browser <-- {"main/files/config":"ready"}.
            // --------------------------------------------
            else if (strcmp(kv.value().as<const char*>(), "ready") == 0 ) {
                jsonDocToClient["server"] = kv.value().as<const char*>();   // Echo ready.
            }

            // --------------------------------------------
            //  browser --> {"main/files/config":"leaving"}.
            //  browser <-- No reply.
            // --------------------------------------------
            else if (strcmp(kv.value().as<const char*>(), "leaving") == 0 ) {
                return;
            }

            // --------------------------------------------
            // browser --> {"altitude"/laser/location:"lock/unlock"}.
            // browser <-- {"altitude"/laser/location:"TBD"}.
            // --------------------------------------------
            else if ((strcmp(kv.value().as<const char*>(), "lock") == 0 ) ||
                     (strcmp(kv.value().as<const char*>(), "unlock") == 0 )) {
                if ((strcmp(kv.key().c_str(), "altitude") == 0) ||
                    (strcmp(kv.key().c_str(), "laser") == 0) ||
                    (strcmp(kv.key().c_str(), "location") == 0)) {

                    // - Reply with value sent + "ed". -
                    memset(JSONbuffer, '\0', sizeof(JSONbuffer));
                    strcpy(JSONbuffer, kv.value().as<const char*>());
                    strcat(JSONbuffer, "ed");
                    jsonDocToClient[kv.key().c_str()] = JSONbuffer;
                }
            }

            // --------------------------------------------
            // browser --> {"listFiles":""}.
            // browser <-- {"listFiles":"file1.ext, file2.ext, ..."}.
            // --------------------------------------------
            if (strcmp(kv.key().c_str(), "listFiles") == 0) {
                char output[2048];
                memset(output, '\0', sizeof(output));
                File root = SD.open("/");
                File file = root.openNextFile();
                while(file) {
                    if (strlen(output) + strlen(file.name()) + 2 < sizeof(output)) {       
                        if ((file.name()[0] != '.') && (file.name() != "") && (!file.isDirectory())) {
                            // TODO: Flat fs for now, add directories & recursive call.
                            strcat(output, "/");
                            strcat(output, file.name());
                            strcat(output, ",");
                        }
                    }
                    file = root.openNextFile();
                }
                jsonDocToClient[kv.key().c_str()] = output;
            }

            // --------------------------------------------
            // browser --> {"deleteFile":"filename"}.
            // browser <-- {"deleteFile":"fileDeleted/fileNOTdeleted"}.
            // --------------------------------------------
            if (strcmp(kv.key().c_str(), "deleteFile") == 0) {
                if (SD.remove(kv.value().as<const char*>())) {  // Delete file.
                    jsonDocToClient["fileDeleted"] = kv.value().as<const char*>();
                } else {
                    jsonDocToClient["fileNOTdeleted"] = kv.value().as<const char*>();
                }
            }

            // --------------------------------------------
            // browser --> {"echo":"hello/etc"}.
            // browser <-- {"echo":"hello/etc"}.
            // --------------------------------------------
            else if (strcmp(kv.key().c_str(), "echo") == 0) {
                jsonDocToClient[kv.key().c_str()] = kv.value().as<const char*>();
            }

            // --------------------------------------------
            // browser --> {"mcu":"restart"}.
            // browser <-- No reply.
            // --------------------------------------------
            else if ((strcmp(kv.key().c_str(), "mcu") == 0) && (strcmp(kv.value().as<const char*>(), "restart") == 0 )) {
                Serial.println("Restarting ...\n");
                esp_restart();  // Restart.
            }

            // - Send reply JSON doc. -
            memset(JSONbuffer, '\0', sizeof(JSONbuffer));
            serializeJson(jsonDocToClient, JSONbuffer, sizeof(JSONbuffer));
            ws.textAll(JSONbuffer);
            wsSendCount++;

            // - Debug. -
            if (commandFlag[DEBUG_WS]) {
                Serial.printf("WS #%u: browser <-- %s\n", clientId, JSONbuffer);
            }
        }
    }
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
 * @link   https://github.com/sparkfun/SparkFun_u-blox_GNSS_v3/tree/main/examples/Basics/Example2_NMEAParsing.
 */
void DevUBLOXGNSS::processNMEA(char incoming) {

    // --- Local vars. ---
    static size_t  nmeaCount        = 0;
    static int64_t lastNMEAsendTime = 0;                             // Last time (us) when NMEA was sent to Wire1.
    static char    nmeaBuffer[120]  = {'\0'};                        // Buffer for NMEA sentence.

    // --- Loop. ---
    if (inLoop) {
        strncat(nmeaBuffer, &incoming, 1);                          // Add NMEA byte from RTK-SMA to outbound buffer.
        if ((incoming == '\n') && (nmeaBuffer[0] == '$')) {         // We have a full sentence.
            if (i2cUp) {                                            // Slave is up.
                if (commandFlag[DEBUG_NMEA]) {                      // Debug.
                    if (strncmp("$GNGGA", nmeaBuffer, 6) == 0) {
                        Serial.printf("\n%u ", nmeaCount);
                    } else {
                        Serial.printf("%u ", nmeaCount);
                    }
                }
                Wire1.beginTransmission(8);                         // Prepare to send on I2C1.
                for (int i = 0; i < strlen(nmeaBuffer); i++) {      // Add bytes to output queue.
                    Wire1.write(nmeaBuffer[i]);
                    if (commandFlag[DEBUG_NMEA]) {                  // Debug.
                        Serial.printf("%c",nmeaBuffer[i]);
                    }
                }
                Wire1.endTransmission(8);                           // Send sentence on I2C1.
                i2cUp = true;                                       // Succesful I2C write. Slave is up.
                lastNMEAsendTime = esp_timer_get_time();            // Last time (us) when NMEA was sent to Wire1.
                nmeaCount++;
            }
            memset(nmeaBuffer, '\0', sizeof(nmeaBuffer));
        }
    }
}

/**
 * ============================================================================
 *                          Loop functions.
 * ============================================================================
 * 
 * Check task functions and event handlers. These are independent of loop().
 * 
 * @since 3.0.11 [2026-01-12-06:00pm] Browser initiated updates.
 * @see relaySerial1toSerial2()   - RTCM - Relay from Serial1 (HC-12 radio) to Serial2 (ZED UART2).
 * @see checkZED()                - NMEA - Check ZED to trigger DevUBLOXGNSS::processNMEA() send to MCU #2.
 * @see checkSerialUSB()          - Check serial USB for input.
 * @see checkNMEAoutBT()          - // ToDo: add check for NMEA ACK from MCU2 within NMEA_TIMEOUT window. See logic in relaySerial1toSerial2().
 * @see checkGnssLockButton()     - Check GNSS lock button. // ToDo: implement.
 * @see checkLaserPointerButton() - Check Laser pointer button. // ToDo: implement.
 * @see roverGNSS.checkUblox()    - New data? Process bytes as they come in. Not needed, why?
 * @see ws.cleanupClients()       - HTTP WebSocket cleanup.
 * @see debug()                   - Display debug.
 */

 /**
 * ------------------------------------------------
 *      RTCM - Relay from Serial1 (HC-12 radio) to Serial2 (ZED UART2).
 * ------------------------------------------------
 * 
 * RTCM preamble = '11010011 000000xx' = 0xd3 0x00.
 *
 *  ESP32-S3 Serial1 (HC12) is set to 9,600 bps (default speed) in Global Vars.
 *  ESP32-S3 Serial2 (ZED UART2) is set to 57,600 bps in Global Vars.
 *  RTK-SMA (ZED UART2) is set to 57,600 bps by default or startAndConfigGNSS()?  // ToDo: check - is this correct?
 *
 * @return void No output is returned.
 * @since  0.3.6  [2025-05-07-03:45] New.
 * @since  0.3.7  [2025-05-09-04:30pm] Add loop() throttle.
 * @since  0.4.5  [2025-05-15-05:00pm] Refactor.
 * @since  0.4.7  [2025-05-21-11:45am] Switch Radio & BT LEDs. Cleanup.
 * @since  0.5.1  [2025-06-07-06:45pm] Removed gotbits. Tweked debug.
 * @since  0.6.3  [2025-07-19-09:45am] Tweaked.
 * @since  3.0.10 [2025-12-30-04:30pm] Version 3.
 * @since  3.0.11 [2026-01-14-09:00am] Global serialChar to local inputChar.
 * @since  3.0.11 [2026-01-15-12:45pm] RTCM_TIMEOUT changed from ms to us.
 * @see    Global vars: Serial.
 * @see    startSerialInterfaces().
 * @see    loop().
 * @link   https://github.com/sparkfun/SparkFun_u-blox_GNSS_v3/blob/main/examples/ZED-F9P/Example3_StartRTCMBase/Example3_StartRTCMBase.ino.
 * @link   https://www.use-snip.com/kb/knowledge-base/an-rtcm-message-cheat-sheet/.
 * @link   https://www.use-snip.com/kb/knowledge-base/rtcm-3-message-list/.
 * @link   https://www.singularxyz.com/blog_detail/11.
 */
void relaySerial1toSerial2() {
    
    // -- Local vars. --
    const  uint16_t RTCM_TIMEOUT      = 3000000;            // Time (us) not to exceed for RTCM input received (3 sec).
    static uint8_t  preamble          =       0;
    static uint16_t byteCount         =       0;
    static int64_t  lastRTCMtime      =       0;            // Last time (us) when RTCM input received.
    static char     rtcmSentence[300] =  {'\0'};            // RTCM3 sentence buffer.
           uint16_t msg_type          =       0;

    // -- Check for Radio down. Set RTCMin state. --
    if ((esp_timer_get_time()-lastRTCMtime) > RTCM_TIMEOUT) {       // RTCM received within RTCM_TIMEOUT?
        RTCMin = false;
    }

    // -- Read from Serial1 (HC-12), write to Serial2 (ZED UART2). --
    if (Serial1.available() > 0) {                                  // HC-12 data to read?
        char inputChar = Serial1.read();                            // Read a character from Serial1 (HC-12) @ SERIAL1_SPEED.
        Serial2.write(inputChar);                                   // Write a character to Serial2 (ZED UART2) @ SERIAL2_SPEED.
        // Serial.printf("[%x]",inputChar);
        if (inputChar == 0xd3) {                                    // Look for preamble (beginning of RTCM3 sentence).
            preamble = (preamble == 0) ? 1 : 2;                     // First (1) or new (2) preamble?
        }
        if (preamble == 1) {                                        // First preamble.
            rtcmSentence[byteCount] = inputChar;                    // Add byte to sentence buffer.
            byteCount++;                                            // Increment byte counter.
            RTCMin = true;                                          // Set state.
        } else if (preamble == 2) {                                 // New Preamble.
            if (commandFlag[DEBUG_RTCM]) {                          // Debug.
                msg_type = rtcm3GetMessageType(rtcmSentence);       // Parse message type.
                uint16_t RTCMinterval = ((esp_timer_get_time()-lastRTCMtime)/1000);
                Serial.printf("\nRTCM3 %ld (%u ms): %i bytes.\n", msg_type, RTCMinterval, byteCount);
                for (size_t i = 0; i < byteCount; i++) {
                    Serial.printf("%02x ", rtcmSentence[i]);
                }
                Serial.println();
            }
            byteCount = 0;
            preamble  = 0;
            memset(rtcmSentence, '\0', sizeof(rtcmSentence));       // Clear the RTCM3 sentence buffer.
            lastRTCMtime = esp_timer_get_time();                    // Used to check for timeout.
        }
    }
}

/**
 * ------------------------------------------------
 *      Return RTCM3 message type.
 * ------------------------------------------------
 * 
 * RTCM3 message structure:
 *   Byte 0: Preamble (0xD3).
 *   Byte 1-2: Reserved (6 bits) + Message length (10 bits).
 *   Byte 3-4: Message type (12 bits) + rest of message.
 *      - Message type starts at bit 24 (byte 3) and is 12 bits long.
 *      - It occupies the upper 8 bits of byte 3 and upper 4 bits of byte 4.
 *
 * @param  array RTCM3 sentence.
 * @return uint16_t Message type.
 * @since  0.8.7 [2025-12-16-06:00pm] New.
 * @see    checkRTCMtoRadio().
 * @link   https://portal.u-blox.com/s/question/0D52p0000C7MwDfCQK/can-you-find-out-the-message-type-of-a-given-rtcm3-message.
 */
uint16_t rtcm3GetMessageType(const char *buffer) {
    // Serial.printf("[%02x] [%02x] [%02x] [%02x] [%02x]\n", buffer[0],  buffer[1], buffer[2], buffer[3], buffer[3]);
    if (buffer[0] != 0xD3) {    // Check if preamble is correct
        return 0;               // Invalid preamble.
    }
    uint16_t message_type = ((uint16_t)buffer[3] << 4) | (buffer[4] >> 4);
    return message_type;
}

/**
 * ------------------------------------------------
 *      Check ZED to trigger DevUBLOXGNSS::processNMEA() send to MCU #2.
 * ------------------------------------------------
 * 
 * checkZED() will trigger the DevUBLOXGNSS::processNMEA() event handler which reads NMEA characters from the ZED read buffer
 *  to build a NMEA sentence. A full NMEA sentence is then sent out Wire1 to MCU #2 for Bluetooth output to SWMaps, etc.
 * checkZED() does not run when the "Operate" web page is loaded since the onWebSocketMessage browser --> {"operate":"update"}
 *  message calls roverGNSS.xxx (where xxx = getSIV, ...) which also queries the ZED and triggers DevUBLOXGNSS::processNMEA().
 *
 * @return void  No output is returned.
 * @since  3.0.11  [2026-01-17-08:00pm] New.
 * @see    DevUBLOXGNSS::processNMEA().
 */
void checkZED() {

    // -- Local vars. --
    const  uint16_t CHECK_ZED_INTERVAL = 250000;                            // Time (ms) not to exceed for RTCM input received (3 sec).
           int64_t  lastCheckZEDtime   = 0;

    // --- Throttle checkUblox() calls. ---
    if ((esp_timer_get_time()-lastCheckZEDtime) < CHECK_ZED_INTERVAL) {     // Time to run?
        return;
    }
    // ToDo: do not run when the "Operate" web page is loaded.
    lastCheckZEDtime = esp_timer_get_time();
    roverGNSS.checkUblox();
}

/**
 * ------------------------------------------------
 *      Check serial USB for input.
 * ------------------------------------------------
 *
 * @return void No output is returned.
 * @since  3.0.9  [2025-12-17-06:00pm] New.
 * @since  3.0.10 [2025-12-30-01:15pm] Refactor.
 * @since  3.0.10 [2026-01-06-11:15am] Add LiPo.
 * @since  3.0.11 [2026-01-12-02:00pm] Refactor.
 * @since  3.0.11 [2026-01-16-08:40pm] if (Serial.available() == 0).
 * @see    loop().
 */
void checkSerialUSB() {

    // --- Local vars. ---
    static size_t posn        = 0;                              // Input position for command buffer.
    static char   command[20] = {'\0'};                         // Serial USB command buffer.
    static char   inputChar   = '\0';

    if (Serial.available() == 0) {                                   // Nothing to see, move on.
        return;
    }

    // --- Fill command buffer. ---
    while ((Serial.available() > 0) )  {
        inputChar = Serial.read();                              // Read char from USB Serial.
        if ((inputChar != '\n') && (inputChar != '\r')) {
            command[posn] = inputChar;                          // Add input to buffer.
            posn++;
        }
    }

    // --- Process command. ---
    if (inputChar == '\n')  {
        if ((command[0]) == '?') {                              // List commands.
            Serial.print("\nGR-MCU1 commands:");
            for (size_t i = 0; i <= NUM_COMMANDS-1; i++) {
                Serial.printf(" %s", COMMAND[i]);
            }
            Serial.println('.');
        } else if ((command[0]) == '!') {                       // Disable all debugs.
            for (size_t i = 0; i <= NUM_COMMANDS; i++) {
                commandFlag[i] = false;
            }
            Serial.println("All debugging disabled.");
        } else {                                                // Possible command.
            size_t i;
            for (i = 0; i < NUM_COMMANDS; i++) {
                if (strcmp(COMMAND[i], command) == 0) {
                    break;
                }
            }
            if (i == NUM_COMMANDS) {                            // Invalid command.
                Serial.printf("%s is not a command. \n", command);
            } else {
                commandFlag[i] = !commandFlag[i];               // Toggle the debug flag.
                Serial.printf("%s %s\n", COMMAND[i], (commandFlag[i]  ? "enabled." : "disabled."));
            }
        }
        posn = 0;                                               // Prepare for next command.
        memset(command, '\0', sizeof(command));
        inputChar = 0;
    }
}

/**
 * ------------------------------------------------
 *      Display debug.
 * ------------------------------------------------
 *
 * // ToDo: rework for V3. Will not compile.
 * @return void No output is returned.
 * @since  0.3.3 [2025-05-02-12:00pm] New.
 * @since  0.3.7 [2025-05-09-04:30pm] Add loop() throttle.
 * @since  0.5.1 [2025-06-07-03:45pm] Removed gotbits.
 * @since  0.6.1 [2025-07-13-08:00am] Added debugNMEA.
 * @since  3.0.11 [2026-01-12-03:30pm] Refactor.
 * @since  3.0.11 [2026-01-12-10:00pm] Added checkWire1.
 * @since  3.0.11 [2026-01-15-10:45am] Moved THROTTLE_DEBUG from global to local var.
 * @see    checkSerialUSB().
 */
void debug() {

    // --- Local vars. ---
    const int64_t  THROTTLE_DEBUG = 1000000;                            // Time (us) between debug() = (every 1 sec).
    static int64_t lastThrottleTime = esp_timer_get_time();             // Throttle. Initialize only once, then persist.
           int64_t lastTime;

    // --- Throttle loop() calls. ---
    if ((esp_timer_get_time() - lastThrottleTime) < THROTTLE_DEBUG) {   // Not time to run.
        return; 
    }
    lastThrottleTime = esp_timer_get_time();                // Time to run. Reset timer.

    // --- Test radio. ---
    if (commandFlag[TEST_RAD]) {

        // -- Local vars. --
        static size_t posn        = 0;                      // Input position for command buffer.
        static char   command[20] = {'\0'};                 // Serial USB command buffer.

        // -- HC-12 into command mode. --
        digitalWrite(HC12_SET, LOW);
        Serial1.write('\n');                                // Clear garbage.
        delay(200);
        while (Serial1.available() > 0) {
            Serial1.read();                          
        }
        Serial.println("\nHC-12 command mode enabled (! to exit)"); // Display instructions.
        Serial.println("Don't forget, the HC-12 needs LiPo power!");
        Serial.println("  AT, AT+Bxxxx, AT+Cxxx, AT+FUx, AT+Px,");
        Serial.println("  AT+Ry (AT+RB, AT+RC, AT+RF, AT+RP, AT+RX),");
        Serial.println("  (y = B=baudrate, C=channel, F=mode, P=power),");
        Serial.println("  AT+Udps, AT+V, AT+SLEEP, AT+DEFAULT, AT+UPDATE.");
        Serial.println("  See https://www.datsi.fi.upm.es/docencia/DMC/HC-12_v2.3A.pdf\n");

        // -- Interact with HC-12. --
        while (true) {

            // - Fill command buffer. -
            while (Serial.available() > 0)  {
                char inputChar = Serial.read();         // Read char from USB Serial.
                if ((inputChar != '\n') && (inputChar != '\r')) {
                    command[posn] = toupper(inputChar); // Add input to buffer.
                    posn++;
                } else if (inputChar == '\n') {
                    if (command[posn-1] == '!') {
                        digitalWrite(HC12_SET, HIGH);       // Reset pin.
                        commandFlag[TEST_RAD] = false;      // Clear test flag.
                        Serial.println("\nHC-12 command mode disabled.\n");
                        return;
                    } else {
                        Serial1.write(command);             // Write command.
                        posn = 0;                           // Prepare for next command.
                        memset(command, '\0', sizeof(command));
                    }
                }
            }
            if (Serial1.available() > 0) {
                while (Serial1.available() > 0)  {
                    char outoutChar = Serial1.read();
                    if (((int) outoutChar > 31) && ((int) outoutChar < 128)) {
                        Serial.printf("%c",outoutChar);     // Display character from HC-12.
                        lastTime = esp_timer_get_time();
                    }
                }
            }
        }
    }

    // --- debug RTCM. ---
    // @see relaySerial1toSerial2().

    // --- GNSS. ---
    if (commandFlag[DEBUG_GNSS]) {
        roverGNSS.enableDebugging();    // "Pipe all NMEA sentences to serial USB."
    }
    if (!commandFlag[DEBUG_GNSS]) {
        roverGNSS.disableDebugging();
    }

    // --- NMEA. ---
    // see "if (debugNMEA)" in checkNMEAin() in loop().

    // --- Buttons. ---
    // if (debugBtn)  {
    //     // - GNSS lock button state (0,1). -
    //     Serial.print("GNSS lock button position = ");
    //     (UIstate[0] == '0') ? Serial.println("up.") : Serial.println("down.");
    // }

    // --- Serial. ---
    if (commandFlag[DEBUG_SER]) {
        // - Serial state (d,u). -
        Serial.printf(
            "USB %c  Serial0 %c  serial1 %c  serial2 %c\n",
            serialState[0],        // Serial USB.
            serialState[1],        // Serial0.
            serialState[2],        // Serial1.
            serialState[3]         // Serial2.
            );
    }

    // --- WiFi. ---
    // --- WS. ---

    // --- LiPo. ---
    if (commandFlag[DEBUG_LIPO]) {
        lipo.enableDebugging();
    } else {
        lipo.disableDebugging();
    }

    // --- Uptime. ---
    if (commandFlag[SHOW_UPTIME]) {
        int32_t seconds = (esp_timer_get_time() - startTime)/1000000;
        int32_t minutes = seconds / 60;
        int32_t hours = minutes / 60;
        Serial.printf("Uptime: %u hrs %u min %u sec\n", hours % 24, minutes % 60, seconds % 60);
    }

    // --- Reset. ---
    if (commandFlag[RESET]) {
        Serial.println("Restarting ...");
        esp_restart();
    }

    // --- Wire1. ---
    if (commandFlag[CHECK_WIRE1]) {
        Wire1.beginTransmission(8);                             // Test Wire1. Receiver is device #8.
        uint8_t status = Wire1.endTransmission(8);                      // Test Wire1. Is device up?
        Serial.print("Wire 1 is ");
        if (status != 0) {
            Serial.printf("down. Error = %i. \n", status);
            i2cUp = false;                                      // Slave is down.
            startI2C();                                         // Restart Wire1.
        } else {
            i2cUp = true;                                       // Slave is up.
            Serial.println("up.");
        }
    }
}

/**
 * ------------------------------------------------
 *      Check GNSS lock button (upPosition or downPosition).
 * ------------------------------------------------
 *
 * // ToDo: implment.
 * @return void No output is returned.
 * @since  0.1.0 [2025-04-24-12:00pm] New.
 * @since  0.3.3 [2025-05-02-08:00am] Refactored.
 * @since  0.3.8 [2025-05-10-09:30am] Set state.
 * @since  0.4.2 [2025-05-15-07:00am] Refactored.
 * @since  0.4.7 [2025-05-21-07:30pm] Switch Radio & BT LEDs.
 * @link   https://roboticsbackend.com/arduino-turn-led-on-and-off-with-button/.
 */
void checkGnssLockButton() {

    static bool lastButtonPos = false;

    // -- Set state of GNSS lock button. --
    if (digitalRead(buttonGnssLock) == true) {
        // UIstate[0] = '0';                   // GNSS lock button is in upPosition.
        if (lastButtonPos == 1) {           // Only true if lock button was in downPosition and now is in upPosition.
            // updateLEDs('-','-','2');        // Overide BT LED.
            lastButtonPos = 0;              // Reset lock button position.
        }
    } else {
        // updateLEDs('-','-','1');            // Overide BT LED.
        // UIstate[0] = '1';                   // GNSS lock button is in downPosition.
        lastButtonPos = 1;                  // Last lock button position.
        ghostMode = true;                   // Flag for checkNMEAin().
    }
}

/**
 * ------------------------------------------------
 *      Check Laser pointer button (locked or unlocked).
 * ------------------------------------------------
 *
 * // ToDo: implement.
 * @return void No output is returned.
 * @since  0.6.3 [2025-07-19-05:45pm] New.
 * @link https://www.build-electronic-circuits.com/arduino-laser-module-ky-008/.
 * @link https://docs.sparkfun.com/SparkFun_Thing_Plus_ESP32-S3/arduino_example/#rgb-led.
 */
void checkLaserPointerButton() {

    // -- Set state of GNSS lock button. --
    if (buttonLaser) {                          // Laser pointer is on.
        digitalWrite(LSR_TRIGGER, LOW);       // Turn laser pointer off.
    } else {
        digitalWrite(LSR_TRIGGER, HIGH);        // Laser is off.
    }
}

/**
 * ============================================================================
 *                              Setup.
 * ============================================================================
 *
 * @since  3.0.3 [2025-10-13-01:00pm] New.
 * @since  3.0.8 [2025-11-21-12:15pm] Removed startWiFiClient(), added startWiFiSoftAP().
 * @see    Global vars.
 */

void setup() {
    showBuild();                // Display build & processor info.
    startSerial();              // Start serial interfaces.
    initPins();                 // Initialize pin modes & pin values.
    startI2C();                 // Start I2C Wire interfaces.
    startLiPo();                // Start LiPo I2C interface.
    startWiFi();                // Start WiFi.
    startSD();                  // Start & test microSD card reader.
    startHttpServer();          // Start HTTP server.
    startWebSocketServer();     // Start WebSocket server.
    startAndConfigGNSS();       // Start GNSS, config ZED settings.
    startTasks();               // Start tasks.
    preLoop();                  // Prepare for loop().
}

/**
 * ============================================================================
 *                              Loop.
 * ============================================================================
 * 
 * @since 3.0.10 [2025-12-27-08:00pm] New.
 * @see   startTasks().
 * @see   Task functions.
 * @see   Event handlers.
 */

void loop() {
    relaySerial1toSerial2();            // RTCM - Relay from Serial1 (HC-12 radio) to Serial2 (ZED UART2).
    checkZED();                         // NMEA - Check ZED to trigger DevUBLOXGNSS::processNMEA() send to MCU #2.
    checkSerialUSB();                   // Check serial USB for input.
    // checkNMEAoutBT() // ToDo: add check for NMEA ACK from MCU2 within NMEA_TIMEOUT window. See logic in relaySerial1toSerial2().
    // checkGnssLockButton();           // Check GNSS lock button. // ToDo: implement.
    // checkLaserPointerButton();       // Check Laser pointer button. // ToDo: implement.
    ws.cleanupClients();                // HTTP WebSocket cleanup.
    debug();                            // Display debug.
}
