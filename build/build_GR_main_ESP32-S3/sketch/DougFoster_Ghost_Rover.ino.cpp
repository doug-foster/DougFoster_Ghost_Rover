#line 1 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
/**
 * *************************************************************************
 *  Ghost Rover 3 - "Invisible" GNSS rover
 * *************************************************************************
 *
 * @author D. Foster <doug@dougfoster.me>.
 * @since  3.1.0  [2026-03-02-05:00pm] Stable 3.0 version.
 * @since  3.1.0  [2026-03-10-11:30am] Add pole height preference.
 * @since  3.1.1  [2026-06-25-10:30pm] Regroup. Cleanup.
 * @since  3.1.1  [2026-06-26-12:30pm] Cleanup formatting.
 * @since  3.1.1  [2026-06-26-06:00pm] Change reset to restart.
 * @since  3.1.1  [2026-06-26-06:00pm] Change checkZED to checkZedTriggerUpdate.
 * @since  3.1.1  [2026-07-03-10:30am] General cleanup.
 * @since  3.1.2  [2026-07-03-06:15pm] New, GhostRover FreeRTOS task taskRtcmRelay() replaced relaySerial1toSerial2() in loop().
 * @since  3.1.2  [2026-07-03-07:30pm] Address rtcmSentence buffer overflow.
 * @since  3.1.2  [2026-07-15-04:45pm] Add NTRIP preferences.
 * @since  3.1.2  [2026-07-16-09:00am] Changed int16_t prfInstrHgt to uint16_t.
 * @since  3.1.2  [2026-07-18-03:00pm] NTRIP.
 * @since  3.2.1  [2026-07-24-03:30pm] Refactor JSON.
 * @since  3.2.1  [2026-07-25-11:00am] Removed wsKey().
 * @since  3.2.1  [2026-07-25-05:00pm] Convert NTRIP keys from alpha to numeric.
 * @since  3.2.1  [2026-07-27-01:45pm] Add sendDataToBrowser(), refactor to consolidate JSON.
 * @since  3.2.1  [2026-07-28-04:45pm] Removed NMEA out switch & preference.
 * @see    https://github.com/doug-foster/DougFoster_Ghost_Rover.
 * @see    https://github.com/doug-foster/DougFoster_Ghost_Rover_BT_relay.
 * @see    https://github.com/doug-foster/DougFoster_Ghost_Rover_EVK_RTCM_relay.
 * @link   http://dougfoster.me.
 */

/**
 * =========================================================================
 *  Docs.
 * =========================================================================
 * 
 * @since 3.1.1 [2026-06-25-01:00pm] New.
 * 
 * --- Comments. ---
 * --- Code structure. ---
 * --- Code operation. ---
 * --- Board LED status. ---
 * --- ESP32 (Arduino framework) data types. ---
 */

/**
 * -------------------------------------------------------------------------
 *  Comments.
 * -------------------------------------------------------------------------
 * 
 * @since 3.1.1 [2026-06-25-01:00pm] New.
 * 
 * --- Description & operation. ---
 *     -- Primary use is ... // ToDo: Complete.
 *
 * --- Major components: rover. ---
 *     -- FQBN               "Sparkfun ESP32-S3 Thing Plus" (~/Library/Arduino15/packages/esp32/hardware/esp32/3.3.10/boards.txt).
 *     -- GR-MCU1 board      https://www.sparkfun.com/sparkfun-thing-plus-esp32-s3.html (SparkFun Thing Plus - ESP32-S3).
 *        - micro SD card    https://www.amazon.com/dp/B0BDYVC5TD (SanDisk 128GB ImageMate microSDXC UHS-1 - Up to 140MB/s).
 *     -- GR-MCU2 board      https://www.sparkfun.com/sparkfun-thing-plus-esp32-s3.html (SparkFun Thing Plus - ESP32-S3).
 *     -- GNSS board         https://www.sparkfun.com/sparkfun-gps-rtk-sma-breakout-zed-f9p-qwiic.html (SparkFun GPS-RTK-SMA Breakout - ZED-F9P (Qwiic) - I2C address 0x42).
 *     -- HC-12 RF radio     https://www.amazon.com/dp/B01MYTE1XR (HiLetgo HC-12 433Mhz SI4438).
 *     -- laser pointer      https://www.petsmart.com/cat/toys/interactive-and-electronic/whisker-city-thrills-and-chills-laser-cat-toy-84577.html.
 * 
 * --- Major components: base. ---
 *     -- base station       https://www.sparkfun.com/sparkfun-rtk-evk.html (SparkFun RTK EVK).
 *     -- RTCM relay MCU     https://www.sparkfun.com/sparkfun-thing-plus-esp32-s3.html (SparkFun Thing Plus - ESP32-S3).
 *     -- HC-12 RF radio     https://www.amazon.com/dp/B01MYTE1XR (HiLetgo HC-12 433Mhz SI4438).
 *
 * --- Other components. ---
 *     -- Rover GNSS antenna. --
 *        - GNSS antenna (L1/L2/L5, TNC-F)              https://www.sparkfun.com/gnss-multi-band-l1-l2-l5-surveying-antenna-tnc-spk6618h.html.
 *        - adapter (TNC-M to SMA-M)                    https://www.amazon.com/dp/B0BGPJP3J3.
 *        - adapter (SMA-M to SMA-F)                    https://www.amazon.com/dp/B00VHAZ0KW.
 *        - cable (SMA-F bulkhead to SMA-M, 6" RG316)   https://www.amazon.com/dp/B081BHHPHQ.
 *     -- Rover RF radio. --
 *        - radio (433.4-473.0 MHz, 100mW, U.FL)        https://www.amazon.com/dp/B01MYTE1XR (HiLetgo HC-12 433Mhz SI4438).
 *        - antenna (UHF 400-960 MHz, BNC-M)            https://www.amazon.com/dp/B07R4PGZK3.
 *        - cable (BNC-F bulkhead to U.FL, 8" RG178)    https://www.amazon.com/dp/B098HX6NFH.
 *     -- Rover Misc. -- 
 *        - I2C Qwiic cable kit                         https://www.amazon.com/dp/B08HQ1VSVL.
 *        - 26 AWG stranded wire                        https://www.amazon.com/dp/B089CQJHQC.
 *        - push button switch (12mm latching)          https://www.amazon.com/dp/B0CGTXMLKL.
 *        - push button switch (7mm momentary)          https://www.amazon.com/dp/B07RV1D98T.
 *        - power switch (15mm on/off latching toggle)  https://www.amazon.com/dp/B09XMDXKTR.
 *        - LEDs (5mm)                                  https://www.amazon.com/dp/B0739RYXVC.
 *        - LED covers (5mm LED bulb socket)            https://www.amazon.com/dp/B07CQ6TH14.
 *        - battery (+5V 2.4A max, 8000 mAh)            https://www.walmart.com/ip/onn-8000mAh-Portable-Battery-Power-Bank-with-USB-A-to-C-Charging-Cable-LED-Indicator-Black/5266111773
 *        - enclosure (Pelican Micro Case 1040)         https://www.rei.com/product/778220/pelican-micro-case-1040-with-carabiner.
 *        - pistol grip handle                          https://www.amazon.com/dp/B01FUEXLGU.
 *        - tripod legs                                 https://www.amazon.com/dp/B07GST1C2Z.
 *        - magnetic phone/tripod mount                 https://www.amazon.com/dp/B0D21RP69C.
 *        - GNSS antenna thread adapter                 https://www.sparkfun.com/antenna-thread-adapter-1-4in-to-5-8in.html
 *        - other: nuts, bolts, 1/4-10 bolt, washers, USB-A power cable, heat shrink tubing.
 *     -- Base Misc. --
 *        - mini tripod                                 https://www.amazon.com/dp/B0CQ6WTRW6.
 *        - laser pointer                               https://www.petsmart.com/cat/toys/interactive-and-electronic/whisker-city-thrills-and-chills-laser-cat-toy-84577.html.
 *        - battery/smartphone holder                   https://www.amazon.com/dp/B07S8TTH34
 *        - 3.5mm/ 0.14 in. pitch 10 pin pluggable PCB screw terminal block connector (female)  https://www.amazon.com/dp/B0BPHLZ8XN.
 *        - same as rover: battery, GNSS antenna/adapters/cable, HC-12 radio/cabl/antenna, LED/holders, primary MCU
 *        - other: nuts, 1/4" thread rod, 1.25" round bubble level.
 * 
 * --- Misc. references. ---
 *     -- EVK         https://docs.sparkfun.com/SparkFun_RTK_EVK/introduction/.ard
 *     -- HC-12       https://www.elecrow.com/download/HC-12.pdf.
 *     -- KY-008      https://www.build-electronic-circuits.com/arduino-laser-module-ky-008/.
 *     -- PyGPSClient https://github.com/semuconsulting/PyGPSClient.
 *     -- SW Maps     https://aviyaantech.com/swmaps/.
 *     -- RTK         https://learn.sparkfun.com/tutorials/what-is-gps-rtk/all.
 *     -- NMEA        https://cdn.sparkfun.com/assets/a/3/2/f/a/NMEA_Reference_Manual-Rev2.1-Dec07.pdf.
 *                    https://swairlearn.bluecover.pt/nmea_analyser.
 *     -- SparkFun    https://learn.sparkfun.com/tutorials/tags/gnss.
 *     -- Pin config  https://roboticsbackend.com/arduino-uno-pins-a-complete-practical-guide/.
 * 
 * --- Dev environment. ---
 *     -- IDE         VS Code & Arduino Maker Workshop 1.1.5 extension (uses Arduino CLI 1.2.0).
 *     -- Platform    https://github.com/espressif/arduino-esp32/releases/latest (Arduino Release v3.3.10 based on ESP-IDF v5.5.4).
 * 
 * --- Caveats. ---
 *     -- SoftwareSerial library is not supported on ESP32-S3 (does work on ESP32-C6).
 *     -- 0.5.1 -> 0.6.1 builds: Moved BLE relay from primary MCU to secondary MCU since BleSerial library is a space pig.
 *
 * --- TODO: ---
 *     1. Add NTRIP client (use credential preferences).
 *     2. Add RTCM page.
 *     3. Offset height/NMEA by instrument height.
 *     4. Button lock (laser/height/position).
 *     5. Update RTKEverywhere for base station.
 *     6. Verify RTK-FIX.
 *     7. Operate.js/operate.html page - add ability to select coordinates (lat/lon, ECEF, UTM northing & easting)  
 */

/**
 * -------------------------------------------------------------------------
 *  Code structure.
 * -------------------------------------------------------------------------
 * 
 * @since 3.1.1 [2026-06-25-01:00pm] New.
 * @since 3.1.2 [2026-07-03-06:15pm] New, GhostRover FreeRTOS task taskRtcmRelay() replaced relaySerial1toSerial2() in loop().
 * @since 3.2.1 [2026-07-30-07:45am] Implement GhostRover FreeRTOS queues: refactor onWebSocketMessage() into processJsonActivity().
 *
 *  --- Docs. ---
 *  --- Include libraries. ---
 *      -- Core.
 *      -- Additional.
 *  --- Global vars.---
 *      -- Pin assignments.
 *      -- LED.
 *      -- Battery.
 *      -- WiFi.
 *      -- HTTP.
 *      -- WebSocket.
 *      -- GNSS.
 *      -- FreeRTOS handles.
 *      -- Operation.
 *      -- Preferences.
 *      -- Oper status.
 *      -- Declaration.
 *      -- Test.
 *  --- General functions. ---
 *      -- statusLedOn()               - Turn on status LED.
 *      -- prefUtility()               - Preference utility.
 *      -- buildOperData()             - Build data for operate page.
 *      -- sendDataToBrowser()         - Send data to browser.
 *      -- rtcm3GetMessageType()       - Return RTCM3 message type to taskRtcmRelay().
 *  --- Setup functions. ---
 *      -- showBuild()                 - Display build & processor info. Status LED is xxx.
 *      -- startSerial()               - Start serial interfaces.
 *      -- initPins()                  - Initialize pins & pin values.
 *      -- startI2C()                  - Start I2C wire interfaces.
 *      -- startLiPo()                 - Start LiPo I2C interface.
 *      -- startWiFi()                 - Start WiFi. Status LED is xxx.
 *      -- startSD()                   - Start & test microSD card reader.
 *      -- startHttpServer()           - Start HTTP server.
 *      -- startWebSocketServer()      - Start WebSocket server.
 *      -- startAndConfigGNSS()        - Start GNSS, config ZED settings.
 *      -- startQueues()               - Start GhostRover FreeRTOS queues.
 *      -- startTasks()                - Start GhostRover FreeRTOS tasks.
 *      -- preLoop()                   - Prepare for loop().
 *  --- GhostRover FreeRTOS functions. ---
 *      -- taskLoopStatusLed()         - GhostRover FreeRTOS task - Set Loop() status LED to blink or solid.
 *      -- taskRtcmRelay()             - GhostRover FreeRTOS task - Relay RTCM from Serial1 (HC-12) to -> Serial2 (ZED UART2).
 *  --- Event handlers for core/additional library processes. ---
 *      -- onWiFiEvent()               - <WiFi.h> & <WiFiAP.h> WiFi event handler (WiFiEvent_t).
 *      -- onHttpFileUpload()          - <ESPAsyncWebServer.h> HTTP endpoint ("/upload") event handler (AsyncWebServerRequest).
 *      -- onWebSocketEvent()          - <ESPAsyncWebServer.h> WebSocket event handler (AsyncWebSocket).
 *      -- DevUBLOXGNSS::processNMEA() - <SparkFun_u-blox_GNSS_v3.h> DevUBLOXGNSS::processNMEA event handler (char incoming).
 *  --- Loop functions. ---
 *      -- checkZedTriggerUpdate()     - Check ZED to trigger DevUBLOXGNSS::processNMEA().
 *      -- processJsonActivity()       - Process queued WS messages & pending status updates. All JSON activity lives here.
 *      -- checkSerialUSB()            - Check serial USB for input.
 *      -- // checkGnssLockButton()    - Check GNSS lock button (upPosition or downPosition). // ToDo: Implement.
 *      -- debug()                     - Display debug.
 *  --- Setup. ---
 *  --- Loop. ---
 */

/**
 * -------------------------------------------------------------------------
 *  Code Operation.
 * -------------------------------------------------------------------------
 *
 * @since 3.1.1 [2026-06-25-01:00pm] New.
 * @since 3.2.1 [2026-07-30-07:45am] Implement FreeRTOS queues: refactor onWebSocketMessage() into processJsonActivity().
 *
 * --- Boot. ---
 *     Include libraries.
 *     Define global vars.
 *     Define general functions.
 *     Define setup() functions.
 *     Define FreeRTOS functions.
 *     Define event handlers.
 *     Define loop() functions.
 * --- Run setup(). ---
 *     showBuild()                    // Display build & processor info.
 *     prefUtility(PREF_INIT)         // Get preferences.
 *     startSerial()                  // Start serial interfaces.
 *     initPins()                     // Initialize pin modes & pin values.
 *     startI2C()                     // Start I2C wire interfaces.
 *     startLiPo()                    // Start LiPo I2C interface.
 *     startWiFi()                    // Start WiFi.
 *     startSD()                      // Start & test microSD card reader.
 *     startHttpServer()              // Start HTTP server.
 *     startWebSocketServer()         // Start WebSocket server.
 *     startAndConfigGNSS()           // Start GNSS, config ZED settings.
 *     startQueues()                  // Start GhostRover FreeRTOS queues.
 *     startTasks()                   // Start GhostRover FreeRTOS tasks.
 *     preLoop()                      // Prepare for loop().
 * --- Run loop(). ---    
 *     checkZedTriggerUpdate()        // NMEA - Check ZED to trigger DevUBLOXGNSS::processNMEA().
 *     processJsonActivity()          // @see "Operation summary" in description for processJsonActivity().
 *     checkSerialUSB()               // Check serial USB for input.
 *     // checkGnssLockButton()       // Check GNSS lock button.
 *     ws.cleanupClients()            // HTTP WebSocket cleanup.
 *     debug()                        // Display debug.
 * --- GhostRover FreeRTOS functions. ---
 *     taskLoopStatusLed()            // GhostRover FreeRTOS task - Set Loop() status LED to blink or solid.
 *     taskRtcmRelay()                // GhostRover FreeRTOS task - Relay RTCM from Serial1 (HC-12) to -> Serial2 (ZED UART2).
 *     rtcm3GetMessageType()          // Called by taskRtcmRelay - return RTCM3 message type.
 * --- Event handlers for core/additional library processes. ---
 *     -- onWiFiEvent()               // <WiFi.h> & <WiFiAP.h> WiFi event handler (WiFiEvent_t).
 *        - if commandFlag[DEBUG_WIFI]), print WiFi status.
 *     -- onHttpFileUpload()          // <ESPAsyncWebServer.h> HTTP endpoint ("/upload") event handler (AsyncWebServerRequest).
 *        - write file to SD, print upload status.
 *     -- onWebSocketEvent()          // <ESPAsyncWebServer.h> WebSocket event handler (AsyncWebSocket).
 *        - cases: WS_EVT_CONNECT, WS_EVT_DISCONNECT,WS_EVT_DATA,WS_EVT_PONG,WS_EVT_ERROR.
 *        - print status, set LED color.
 *        - if WS_EVT_DATA, push (xQueueSend) JSON struct (data & length) into GhostRover FreeRTOS QueueHandle_t wsRxQueue.
 *     -- DevUBLOXGNSS::processNMEA() // <SparkFun_u-blox_GNSS_v3.h> DevUBLOXGNSS::processNMEA event handler (char incoming).
 *        - Gather NMEA bytes into sentences, send NMEA sentence over I2C (Wire1) to GR-MCU2.
 *        - Track counts of NMEA sentences (all & each type) for operate page, status section.
 *        - Set status LED red if I2C (Wire1) is down, call startI2C() to restart.
 */

/**
 * -------------------------------------------------------------------------
 *  Board LED status.
 * -------------------------------------------------------------------------
 *
 * @since 3.1.1 [2026-06-25-01:00pm] New.
 * @since 3.2.1 [2026-06-25-01:00pm] Updated GR-MCU2 LED status.
 *
 *   ws2812LedColor = RED, YELLOW, GREEN, BLUE, WHITE.
 *   ws2812LedBlink = true, false.
 * 
 * --- GR-MCU1 ----
 *     -- setup(). --
 *        - solid YELLOW: startup delay.
 *        - solid  WHITE: setup() started & running ok.
 *        - solid    RED: startWiFi() error,
 *                        startSD() error,
 *                        startAndConfigGNSS() error.
 *     -- loop(). --  
 *        - solid   BLUE: loop running ok with no websocket connection.
 *        - solid  GREEN: loop running ok with onWebSocketEvent(WS_EVT_CONNECT) connection.
 *        - blink  GREEN: relaySerial1toSerial2() RTCM in Serial1 out Serial2.
 *        - solid    RED: DevUBLOXGNSS::processNMEA() GRMCU1 <--> GRMCU2 I2C (NMEAout) error.
 * --- GR-MCU2 ----
 * @see DougFoster_Ghost_Rover_BT_relay.ino. 
 * 
 * --- GNSS ----
 *     -- https://learn.sparkfun.com/tutorials/gps-rtk2-hookup-guide#hardware-overview. --
 *        - PWR:   (red) receiving 3.3V over USB or Qwiic bus.
 *        - PPS:   (yellow) blinks each second when position lock has been achieved.
 *        - RTK:   (yellow) solid on power up, blinks if RTCM data received, off if RTK fix obtained.
 *        - FENCE: (blue) can be configured for geofencing.
 */

/**
 * -------------------------------------------------------------------------
 *  ESP32 (Arduino framework) data types.
 * -------------------------------------------------------------------------
 *
 * @since 3.0.12 [2026-02-20-09:00am] New.
 * 
 * --- Unsigned integer. ---
 *     uint8_t                      %u         8 bits = 1 byte,  0 to 255.
 *     uint16_t/unsigned short      %u        16 bits = 2 bytes, 0 to 65,535.
 *     uint32_t/unsigned long       %u,%lu    32 bits = 4 bytes, 0 to 4,294,967,295.
 *     size_t (size,length,count)   %zu       32 bits = 4 bytes, 0 to 4,294,967,295.
 *     uint64_t/unsigned long long  %llu      64 bits = 8 bytes, 0 to 18,446,744,073,709,551,615.
 *
 * --- Signed integer. ---
 *     int8_t                       %d         8 bits = 1 byte,            -128 to 127.
 *     int16_t/short                %d        16 bits = 2 bytes,        -32,768 to 32,767.
 *     int32_t/int/long             %d,%ld    32 bits = 4 bytes, -2,147,483,648 to 2,147,483,647.
 *     int64_t/long long            %lld      64 bits = 8 bytes,      -9.22e+18 to 9.22e+18.
 *
 * --- Signed decimal/floating point. ---
 *     float                        %.2f      32 bits = 4 bytes,   6-7 sig. digits (hardware),  -3.40e+38 to 3.40e+38), 2 decimal places.
 *     double/long double           %f,%lf    64 bits = 8 bytes, 15-17 sig. digits (software), -1.79e+308 to 1.79e+308).
 *
 * --- Character/text. ---
 *     char (signed)                %c         8 bit = 1 byte,  -128 to 127.
 *     unsigned char                %c         8 bit = 1 byte,     0 to 255.
 *
 * --- Other. ---
 *     bool                        %d (0/1)   8 bit = 1 byte,  true or false.
 *     bool                        %s (text)  8 bit = 1 byte,  true or false.
 *     void                        n/a.
 *     array                       n/a.
 *     string                      %s
 */

 /**
 * -------------------------------------------------------------------------
 *  WebSocket docs. 
 * -------------------------------------------------------------------------
 * 
 * @see comments in processJsonActivity().
 */

/**
 * =========================================================================
 *  Include libraries.
 * =========================================================================
 *
 * @since 3.0.9   [2025-12-01-05:15pm].
 * @since 3.0.11  [2026-01-08-10:30am] Browser initiated updates.
 * @since 3.0.11  [2026-01-26-04:15pm] Add preferences library.
 * @since 3.1.1   [2026-06-25-02:00pm] Updated library <AsyncTCP.h>                from 3.4.9  to 3.4.10.
 * @since 3.1.1   [2026-06-25-02:00pm] Updated library <ESPAsyncWebServer.h>       from 3.9.3  to 3.11.1.
 * @since 3.1.1   [2026-06-25-02:00pm] Updated library <ArduinoJson.h>             from 7.4.2  to 7.4.3.
 * @since 3.1.1   [2026-06-25-02:00pm] Updated library <SparkFun_u-blox_GNSS_v3.h> from 3.1.13 to 3.1.14.
 * @since 3.1.2   [2026-07-15-04:45pm] Add NTRIP preferences.
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
#include <Preferences.h>                                   // https://github.com/espressif/arduino-esp32/tree/master/libraries/Preferences/.

// --- Additional. ---                  
#include <AsyncTCP.h>                                      // https://github.com/ESP32Async/AsyncTCP (3.4.10).
#include <ESPAsyncWebServer.h>                             // https://github.com/ESP32Async/ESPAsyncWebServer (3.8.1).
#include <ArduinoJson.h>                                   // https://github.com/bblanchon/ArduinoJson (7.4.3).
#include <SparkFun_MAX1704x_Fuel_Gauge_Arduino_Library.h>  // https://github.com/sparkfun/SparkFun_MAX1704x_Fuel_Gauge_Arduino_Library (1.0.4).
#include <SparkFun_u-blox_GNSS_v3.h>                       // https://github.com/sparkfun/SparkFun_u-blox_GNSS_v3 (3.1.13).

/**
 * =========================================================================
 *  Global vars.
 * =========================================================================
 *
 * @since 3.0.10 [2026-01-06-10:00pm].
 * @since 3.0.11 [2026-01-08-10:30am] Browser initiated updates.
 * @since 3.0.12 [2026-02-01-09:30am] Add preferences.
 * @since 3.0.12 [2026-02-14-06:15pm] Remove prfRqsPvtInt.
 * @since 3.0.12 [2026-02-28-02:15pm] Add WS_SOCKET_NUM.
 * @since 3.1.0  [2026-03-20-11:45am] Add pole height preference.
 * @since 3.1.2  [2026-07-16-09:00am] Increase jsonBuffer[768] to 1024.
 * @since 3.1.2  [2026-07-16-09:00am] Changed int16_t prfInstrHgt to uint16_t.
 * @since 3.1.2  [2026-07-16-10:00am] Moved MAJOR, MINOR, PATCH from showBuild() to "Operation" section.
 * @since 3.2.1  [2026-07-24-03:30pm] Refactor JSON.
 * @since  3.2.1  [2026-07-28-04:45pm] Removed NMEA out switch & preference.
 */

// --- Pin assignments. ---
const uint8_t HC12_SET    = 7;                            // HC-12 SET {blue wire}.
const uint8_t LSR_TRIGGER = 15;                           // KY-008 trigger pin {yellow wire}.

// --- LED. ---
bool  ws2812LedBlink     = false;
const uint8_t LED_BRIGHT = 50;                            // 0-255. taskLoopStatusLed()
enum  ws2812_LED_COLOR {                                  // WS2812 RGB STAT LED.
    OFF,
    RED,
    YELLOW,
    GREEN,
    BLUE,
    WHITE
} ws2812LedColor;

// --- Battery. ---
SFE_MAX1704X lipo(MAX1704X_MAX17048);                     // LiPo battery.

// --- WiFi. ---
char localIp[16];
char hotspotIp[16];

// --- HTTP. ---
const char     WEBSOCKET_SERVER_NAME[] = "/ghostRover";
uint8_t        clientId                = 0;               // HTTP WebSocket client ID # (+1 for each new connection).
AsyncWebServer httpServer(80);                            // HTTP AsyncWebServer object on port 80.
AsyncWebSocket ws(WEBSOCKET_SERVER_NAME);                 // HTTP WebSocket object.

// --- WebSocket. ---
const uint8_t WS_RX_QUEUE_LEN     = 5;                    // Max # of WebSocket queued incoming messages.
bool         browserUpdatePending = false;                // Flag: update ready to send to browser page (operate, nmea, ...).
char         lastNmea[120]      = {'\0'};                 // Snapshot of last complete NMEA sentence. @see DevUBLOXGNSS::processNMEA(), sendDataToBrowser().
char         jsonBuffer[1024];                            // @see processJsonActivity().  // ToDo: Move to local var?
char         response[128];                               // WebSocket message response.
size_t       jsonPairNum;                                 // Track number of JSON KV pairs.
JsonDocument jsonDocToBrowser;                            // JSON document - send to browser. Used in processJsonActivity(), buildOperData(), & DevUBLOXGNSS::processNMEA().
JsonDocument jsonDocFromBrowser;                          // JSON document - received from browser. Used in processJsonActivity(). 
JsonDocument JsonDocNtrip;                                // JSON document - JSON NTRIP data inside jsonDocToBrowser or jsonDocFromBrowser.
struct WsQueueItem {                                      // Queued incoming WebSocket message.
    char   data[2048];                                    // Raw JSON text 2x jsonBuffer[1024]. Escaped NTRIP JSON attributes can run larger than outbound buffer.
    size_t len;                                           // Length of raw JSON data (not just null-terminated).
};

// --- GNSS. ---
SFE_UBLOX_GNSS roverGNSS;                                 // GNSS object (uses I2C-1).

// --- FreeRTOS handles. ---
TaskHandle_t taskLoopStatusLedHandle;                     // GhostRover FreeRTOS task: Loop status LED.
TaskHandle_t taskRtcmRelayHandle;                         // GhostRover FreeRTOS task: RTCM relay, Serial1 -> Serial2.
QueueHandle_t wsRxQueue;                                  // GhostRover FreeRTOS queue: AsyncTCP task -> loop().

// --- Operation. ---
enum CommandIndex {                                       //  Readable index for command array.
    TEST_RAD = 0,                                         //  0.
    DEBUG_RTCM,                                           //  1.
    DEBUG_GNSS,                                           //  2.
    DEBUG_NMEA,                                           //  3.
    DEBUG_BTN,                                            //  4.
    DEBUG_SER,                                            //  5.
    DEBUG_WIFI,                                           //  6.
    DEBUG_WS,                                             //  7.
    DEBUG_LIPO,                                           //  8.
    SHOW_UPTIME,                                          //  9.
    RESTART,                                              // 10.
    CHECK_WIRE1,                                          // 11.
    DEBUG_TEMP,                                           // 12.
    DEBUG_NMEA_HEX,                                       // 13.
    DEBUG_NMEA_COUNTS,                                    // 14.
    DEBUG_PREFS,                                          // 15.
    NUM_COMMANDS                                          // 16 = automatic array length.
};     
const char* COMMAND[NUM_COMMANDS] = {                     // Command strings; match CommandIndex.
    "testRad",                                            // TEST_RAD.
    "debugRTCM",                                          // DEBUG_RTCM.
    "debugGNSS",                                          // DEBUG_GNSS.
    "debugNMEA",                                          // DEBUG_NMEA.
    "debugBtn",                                           // DEBUG_BTN.
    "debugSer",                                           // DEBUG_SER.
    "debugWiFi",                                          // DEBUG_WIFI.
    "debugWs",                                            // DEBUG_WS.
    "debugLiPo",                                          // DEBUG_LIPO.
    "showUpTime",                                         // SHOW_UPTIME.
    "restart",                                            // RESTART.
    "checkWire1",                                         // CHECK_WIRE1.
    "debugTemp",                                          // DEBUG_TEMP.
    "debugNMEAhex",                                       // DEBUG_NMEA_HEX.
    "debugNMEAcounts",                                    // DEBUG_NMEA_COUNTS.
    "debugPrefs"                                          // DEBUG_PREFS.
};     
const bool    RW_MODE                   = false;          // Open preference name space as read/write.
const bool    RO_MODE                   = true;           // Open preference name space as read only.
const uint8_t MAJOR_VERSION             = 3;              // Current major build version (@see showBuild()).
const uint8_t MINOR_VERSION             = 2;              // Current minor build version (@see showBuild()).
const uint8_t PATCH_VERSION             = 1;              // Current patch build version (@see showBuild()).
const uint8_t MIN_SATELLITE_THRESHHOLD  =  2;             // Minimum SIV for reliable coordinate information.      
bool          ghostMode                 = false;          // Flag, in Ghost mode (i.e. locked coordinates).
bool          i2cUp                     = false;          // Status: true if both Wire & Wire1 up, else false.
bool          inLoop                    = false;          // In loop() indicator.
bool          RTCMin                    = false;          // RTCM received from NTRIP or radio within RTCM_TIMEOUT.
bool          NMEAout                   = false;          // NMEA sent OUT to MCU #2?
bool          zeroStatusCounters        = false;          // Flag to zero status counters.
bool          buttonGnssLock;                             // UI - // ToDo: Implement.
bool          buttonAltitudeLock;                         // UI - // ToDo: Implement.
bool          buttonPositionLock;                         // UI - // ToDo: Implement.
bool          buttonLaser;                                // UI button to turn laser pointer on/off.
bool          buttonUnlockAll;                            // UI - // ToDo: Implement.
bool          commandFlag[NUM_COMMANDS] = {false};        // Command flags.
char          uptime[20]                = {'\0'};         // 01h 03m 12s.
char          operMode[2]               = {'\0'};         // Operation mode (r=rover, b=base).
char          debugTemp[250]            = {'\0'};         // Various debug scenarios.
char          whichPage[10]             = {'\0'};         // Current browser page served by startHttpServer().
char          buildString[40]           = {'\0'};         // Build string (build version on date at time). e.g. 3.0.12 - Feb 19 2026 @ 12:23:13
char          serialState[4];                             // Serial state: [USB] [S0] [S1] [S2]; value = u, d, or -.
char          nmeaBuffer[120]           = {'\0'};         // Buffer for NMEA sentence. @see DevUBLOXGNSS::processNMEA().  // ToDo: Move to local var?
char          operBuffer[24]            = {'\0'};         // Buffer for Operate data.
size_t        wsSendCount               = 0;              // # of WebSocket messages sent.
size_t        rtcmSentenceCount         = 0;              // # of RTCM sentences in.
u_int8_t      numSatInView              = 0;              // GNSS - # OF satellites in view.
u_int8_t      fixType                   = 0;              // GNSS - type of fix (single, RTK-float, RTK-fix).
int64_t       startTime;                                  // Boot time.
float         rtcmKbps                  = 0;              // RTCM kbps (average).
float         heightEllipsoid           = 0;              // GNSS - ellipsoid height.
float         heightOrthometric         = 0;              // GNSS - orthometric height.
float         accuracyHorizontal        = 0;              // GNSS - horizontal accuracy.
float         accuracyVertical          = 0;              // GNSS - vertical accuracy.
float         batterySoc                = 0;              // Battery State Of Charge (SOC).
float         batteryChangeRate         = 0;              // Battery charge - rate of change.
double        lat                       = 0;              // GNSS - latitude.
double        lon                       = 0;              // GNSS - longitude.

// --- Preferences. ---
const uint16_t NTRIP_CAST_ATTR_LEN      = 512;            // Length of character array for NTRIP caster attibute profile.
char           prfUnt[6];                                 // Distance units: meter/feet (used only in browser).
char           prfRtcIn[6];                               // Control RTCM in: off/radio/ntrip.
char           prfHotSsi[20];                             // WiFi hotspot client: network SSID.
char           prfHotPas[30];                             // WiFi hotspot client: password.
char           prfNtripCastAttr[4][NTRIP_CAST_ATTR_LEN];  // 2D Array of (4) NTRIP caster attribute profiles (each is in JSON format).
char           prfNtripCastAct[2];                        // Which # NTRIP caster attribute profile is being used.
uint8_t        prfGnsNavRat;                              // ZED: OUTPUT every X (e.g. 5) MEASURE intervals every (e.g. 5*100=500) ms.

uint16_t       prfGnsMsrInt;                              // ZED: MEASURE every Y (e.g. 100) ms.
uint16_t       prfInstrHgt;                               // Instrument height (includes rover height + pole height).
Preferences    roverPrefs;                                // Rover's NVS preferences namespace.
enum           prefAction {                               // Readable index for preference actions.
    PREF_INIT,                                            // 0.
    PREF_READ,                                            // 1.
    PREF_SET,                                             // 2.
    PREF_RESET,                                           // 3.
    PREF_PRINT,                                           // 4.
    PREF_SET_NTRIP                                        // 5.
};
struct         ntripCasterProfile {                       // NTRIP caster attribute template.
    bool     sendGga;
    char     name[48];
    char     url[48];
    char     mount[24];
    char     user[48];
    char     pass[48];
    uint8_t  id;
    uint8_t  version;
    uint16_t port;
};
ntripCasterProfile ntripCaster = {};                      // NTRIP caster attribute profile being used.

// --- Oper status. ---
size_t  nmeaCountAll       = 0;
size_t  nmeaCountGGA       = 0;
size_t  nmeaCountRMC       = 0;
size_t  nmeaCountGSA       = 0;
size_t  nmeaCountGSV       = 0;
size_t  nmeaCountGST       = 0;
size_t  nmeaCountTXT       = 0;
size_t  nmeaCountOther     = 0;
int64_t lastGGAsendTime    = 0; 
int64_t nmeaRate           = 0;

// --- Declaration. ---
// --- Test. ---

/**
 * =========================================================================
 *  General functions.
 * =========================================================================
 *
 * @since 3.0.12 [2026-02-06-04:00pm] New.
 * @since 3.2.1  [2026-07-25-11:00am] Removed wsKey().
 * @since 3.2.1  [2026-07-26-09:00am] Add sendDataToBrowser().
 * @see   statusLedOn()           - Turn on status LED.
 * @see   prefUtility()           - Preference utility.
 * @see   buildOperData()         - Build data for operate page.
 * @see   sendDataToBrowser()     - Send jsonDocToBrowser.
 * @see   rtcm3GetMessageType()   - Return RTCM3 message type to taskRtcmRelay().
 */

/**
 * -------------------------------------------------------------------------
 *  Turn on status LED.
 * -------------------------------------------------------------------------
 * 
 * @return void  No output is returned.
 * @since  3.0.12 [2026-02-10-10:45pm] New.
 * @see showBuild(), startWiFi(), startSD(), startAndConfigGNSS(), taskLoopStatusLed().
 */
#line 590 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void statusLedOn();
#line 809 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void buildOperData();
#line 890 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void sendDataToBrowser();
#line 990 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
uint16_t rtcm3GetMessageType(const char* rtcmSentence);
#line 1038 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void showBuild();
#line 1087 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void startSerial();
#line 1119 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void initPins();
#line 1145 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void startI2C();
#line 1182 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void startLiPo();
#line 1206 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void startWiFi();
#line 1307 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void startSD();
#line 1369 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void startHttpServer();
#line 1424 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void startWebSocketServer();
#line 1459 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void startAndConfigGNSS();
#line 1533 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void startQueues();
#line 1558 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void startTasks();
#line 1581 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void preLoop();
#line 1620 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void taskLoopStatusLed(void * pvParameters);
#line 1665 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void taskRtcmRelay(void *pvParameters);
#line 1753 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void onWiFiEvent(arduino_event_id_t event);
#line 1785 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void onHttpFileUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final);
#line 1833 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void onWebSocketEvent(AsyncWebSocket *httpServer, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);
#line 2031 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void checkZedTriggerUpdate();
#line 2301 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void processJsonActivity();
#line 2628 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void checkSerialUSB();
#line 2699 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void checkGnssLockButton();
#line 2735 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void debug();
#line 2901 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void setup();
#line 2928 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void loop();
#line 590 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void statusLedOn() {
    switch (ws2812LedColor) {
        case RED:
            rgbLedWrite(LED_BUILTIN, LED_BRIGHT, 0, 0);         // red, green, blue.
            break;
        case YELLOW:
            rgbLedWrite(LED_BUILTIN, LED_BRIGHT, LED_BRIGHT, 0);
            break;
        case GREEN:
            rgbLedWrite(LED_BUILTIN, 0, LED_BRIGHT, 0);
            break;
        case BLUE:
            rgbLedWrite(LED_BUILTIN, 0, 0, LED_BRIGHT);
            break;
        case WHITE:
            rgbLedWrite(LED_BUILTIN, LED_BRIGHT, LED_BRIGHT, LED_BRIGHT);
            break;
    }
}

/**
 * -------------------------------------------------------------------------
 *  Preference utility.
 * -------------------------------------------------------------------------
 * 
 * Keys for preference values are sent in WebSocket as "1", "2", etc. but stored in NVS as "prfUnt", "prfRtcIn", ... .
 * 
 * @param  enum    prefAction PREF_INIT, PREF_READ, PREF_SET, PREF_RESET, PREF_PRINT, PREF_SET_NTRIP.
 * @param  array   key WebSocket JSON key.
 * @param  array   value WebSocket JSON value.
 * @return void    No output is returned.
 * @since  3.0.12 [2026-02-07-10:30am] New.
 * @since  3.0.12 [2026-02-14-06:15pm] Remove prfRqsPvtInt.
 * @since  3.0.12 [2026-02-18-06:00pm] Add buildString.
 * @since  3.0.12 [2026-02-23-01:00pm] Shorten RTCM & NMEA status.
 * @since  3.0.12 [2026-02-28-02:45pm] Fix bugs: prfRtcIn, jsonDocToBrowser.clear().
 * @since  3.1.0  [2026-03-20-11:45am] Add pole height preference.
 * @since  3.1.2  [2026-07-09-09:00pm] Add (3) NTRIP caster profiles.
 * @since  3.1.2  [2026-07-15-04:45pm] Refactor: NTRIP & cleanup.
 * @since  3.1.2  [2026-07-16-09:00am] Changed int16_t prfInstrHgt to uint16_t.
 * @since  3.1.2  [2026-07-20-03:15pm] NTRIP.
 * @since  3.2.1  [2026-07-24-03:30pm] Refactor JSON.
 * @since  3.2.1  [2026-07-28-04:45pm] Removed NMEA out switch & preference.
 * 
 * @see    Global vars: Preference defaults, setup().
 * @link   https://docs.espressif.com/projects/arduino-esp32/en/latest/tutorials/preferences.html.
 * @link   https://github.com/espressif/arduino-esp32/tree/master/libraries/Preferences/.
 */
void prefUtility(prefAction action, const char* key = NULL, const char* value = NULL) {

    // --- Local vars. ---
    const char      NAMESPACE[]             = "config";         // The preference namespace. 
    const char      DEF_UNT[]               = "meter";          // Default distance units: meter/feet (used only in browser).                        1 - Matching global var: char     prfUnt[6].
    const char      DEF_RTC_IN[]            = "radio";          // Default control RTCM in: off/radio/ntrip.                                         2 - Matching global var: char     prfRtcIn[6].
    const char      DEF_HOT_SSI[]           = "ssid";           // Default WiFi hotspot client: network SSID.                                        4 - Matching global var: char     prfHotSsi[20].
    const char      DEF_HOT_PASS[]          = "pass";           // Default WiFi hotspot client: password.                                            5 - Matching global var: char     prfHotPas[30].
    const char      DEF_NTRIP_CAST_ATTR_1[] = "{\"43\":\"1\",\"44\":\"name 1\",\"45\":\"x.com\",\"46\":\"ABC\",\"47\":\"2101\",\"48\":\"1\",\"49\":\"user1\",\"50\":\"pass1\",\"51\":\"1\"}";
    const char      DEF_NTRIP_CAST_ATTR_2[] = "{\"43\":\"2\",\"44\":\"name 2\",\"45\":\"y.com\",\"46\":\"DEF\",\"47\":\"2101\",\"48\":\"1\",\"49\":\"user2\",\"50\":\"pass2\",\"51\":\"1\"}";
    const char      DEF_NTRIP_CAST_ATTR_3[] = "{\"43\":\"3\",\"44\":\"name 3\",\"45\":\"z.com\",\"46\":\"GHI\",\"47\":\"2101\",\"48\":\"1\",\"49\":\"user3\",\"50\":\"pass3\",\"51\":\"1\"}";
                                                                // Default NTRIP caster attribute profile 1.                                         6 - Matching global var: char     prfNtripCastAttr[0].
                                                                // Default NTRIP caster attribute profile 2.                                         7 - Matching global var: char     prfNtripCastAttr[1].
                                                                // Default NTRIP caster attribute profile 3.                                         8 - Matching global var: char     prfNtripCastAttr[2].
    const char      DEF_NTRIP_CAST_ACT[]    = "1";              // Default NTRIP caster profile being used.                                         10 - Matching global var: char     prfNtripCastAct.
    const uint8_t   DEF_GNS_NAV_RAT         = 2;                // Default ZED rate (times/interval): OUTPUT a new solution.                         9 - Matching global var: uint8_t  prfGnsNavRat.
    const uint16_t  DEF_GNS_MSR_INT         = 100;              // Default ZED interval (ms): CREATE a new solution.                                11 - Matching global var: uint16_t prfGnsMsrInt.
    const uint16_t  DEF_INSTR_HGT           = 128;              // Default instrument height (mm - includes rover height [128] + pole height [0]).  12 - Matching global var: uint16_t prfInstrHgt.
    const uint16_t  NUM_PREFS               = 12;               // Number of preferences being used.
    bool            hasKey                  = false;

    // --- Which action? ---
    switch (action) {
        case PREF_INIT:                                         // Only called by setup().

            // -- Check namespace. --
            roverPrefs.begin(NAMESPACE, RW_MODE);               // Open NAMESPACE object for read/write. If it doesn't exist, create it.
            if(roverPrefs.isKey("prfUnt")) {
                prefUtility(PREF_READ);                         // Test preference exists, so they all should. Read values from NVS & set global vars.
            } else {
                prefUtility(PREF_RESET);                        // If the test preference doesn't exist, none of them do.
            }

            // -- Close name space. --
            roverPrefs.end();
            Serial.printf("NVS namespace %s using %u entries with %u available.\n", NAMESPACE, NUM_PREFS, roverPrefs.freeEntries());
            break;

        case PREF_READ:

            // -- Open name space. --
            roverPrefs.begin(NAMESPACE, RO_MODE);

            // -- Set global vars from NVS preferences. --
            roverPrefs.getString("prfUnt",          prfUnt,              sizeof(prfUnt));    // Preference stored as "prfUnt".
            roverPrefs.getString("prfRtcIn",        prfRtcIn,            sizeof(prfRtcIn));
            roverPrefs.getString("prfHotSsi",       prfHotSsi,           sizeof(prfHotSsi));
            roverPrefs.getString("prfHotPas",       prfHotPas,           sizeof(prfHotPas));
            roverPrefs.getString("prfNtripCaster1", prfNtripCastAttr[0], NTRIP_CAST_ATTR_LEN);
            roverPrefs.getString("prfNtripCaster2", prfNtripCastAttr[1], NTRIP_CAST_ATTR_LEN);
            roverPrefs.getString("prfNtripCaster3", prfNtripCastAttr[2], NTRIP_CAST_ATTR_LEN);
            roverPrefs.getString("prfNtripCastAct", prfNtripCastAct,     sizeof(prfNtripCastAct));
            prfGnsNavRat    = roverPrefs.getUShort("prfGnsNavRat");           
            prfGnsMsrInt    = roverPrefs.getUShort("prfGnsMsrInt");
            prfInstrHgt     = roverPrefs.getUShort("prfInstrHgt");

            // -- Close name space. --
            roverPrefs.end();
            Serial.println("Preferences read.");
            break;

        case PREF_SET:

            // -- Open name space. --
            roverPrefs.begin("config", RW_MODE);

            // - Set NVS preferences from global vars. -
            roverPrefs.putString("prfUnt",          prfUnt);                // Store as "prfUnt"              (sent/rcvd as "1").
            roverPrefs.putString("prfRtcIn",        prfRtcIn);              // Store as "prfRtcIn"            (sent/rcvd as "2").
            roverPrefs.putString("prfHotSsi",       prfHotSsi);             // Store as "prfHotSsi"           (sent/rcvd as "6").
            roverPrefs.putString("prfHotPas",       prfHotPas);             // Store as "prfHotPas"           (sent/rcvd as "7").
            roverPrefs.putString("prfNtripCaster1", prfNtripCastAttr[0]);   // Store as "prfNtripCastAttr[0]" (sent/rcvd as "39").
            roverPrefs.putString("prfNtripCaster2", prfNtripCastAttr[1]);   // Store as "prfNtripCastAttr[1]" (sent/rcvd as "40").
            roverPrefs.putString("prfNtripCaster3", prfNtripCastAttr[2]);   // Store as "prfNtripCastAttr[2]" (sent/rcvd as "41").
            roverPrefs.putString("prfNtripCastAct", prfNtripCastAct);       // Store as "prfNtripCastAct"     (sent/rcvd as "42").
            roverPrefs.putUShort("prfGnsNavRat",    prfGnsNavRat);          // Store as "prfGnsNavRat"        (sent/rcvd as "5").
            roverPrefs.putUShort("prfGnsMsrInt",    prfGnsMsrInt);          // Store as "prfGnsMsrInt"        (sent/rcvd as "4").
            roverPrefs.putUShort("prfInstrHgt",     prfInstrHgt);           // Store as "prfInstrHgt"         (sent/rcvd as "36" with value in mm, e.g. "165").

            // -- Close name space. --
            roverPrefs.end();
            Serial.println("Preferences saved.");
            break;

        case PREF_RESET:

            // -- Copy default values to global vars. --
            strlcpy(prfUnt,              DEF_UNT,               sizeof(prfUnt));
            strlcpy(prfRtcIn,            DEF_RTC_IN,            sizeof(prfRtcIn));
            strlcpy(prfHotSsi,           DEF_HOT_SSI,           sizeof(prfHotSsi));
            strlcpy(prfHotPas,           DEF_HOT_PASS,          sizeof(prfHotPas));
            strlcpy(prfNtripCastAttr[0], DEF_NTRIP_CAST_ATTR_1, NTRIP_CAST_ATTR_LEN);
            strlcpy(prfNtripCastAttr[1], DEF_NTRIP_CAST_ATTR_2, NTRIP_CAST_ATTR_LEN);
            strlcpy(prfNtripCastAttr[2], DEF_NTRIP_CAST_ATTR_3, NTRIP_CAST_ATTR_LEN);
            strlcpy(prfNtripCastAct,     DEF_NTRIP_CAST_ACT,    sizeof(prfNtripCastAct));
            prfGnsNavRat               = DEF_GNS_NAV_RAT;
            prfGnsMsrInt               = DEF_GNS_MSR_INT;
            prfInstrHgt                = DEF_INSTR_HGT;

            // -- Close name space. --
            roverPrefs.end();
            Serial.println("Resetting all preferences.");
            break;

        case PREF_PRINT:

            // -- Open name space. --
            roverPrefs.begin(NAMESPACE, RO_MODE);

            // -- Print values. --
            Serial.println("---                    Default, Global, NVS. ---");
            Serial.printf( "prfUnt                 \"%s\", \"%s\", \"%s\"\n", DEF_UNT,            prfUnt,          roverPrefs.getString("prfUnt"));
            Serial.printf( "prfRtcIn               \"%s\", \"%s\", \"%s\"\n", DEF_RTC_IN,         prfRtcIn,        roverPrefs.getString("prfRtcIn"));
            Serial.printf( "prfHotSsi              \"%s\", \"%s\", \"%s\"\n", DEF_HOT_SSI,        prfHotSsi,       roverPrefs.getString("prfHotSsi"));
            Serial.printf( "prfHotPas              \"%s\", \"%s\", \"%s\"\n", DEF_HOT_PASS,       prfHotPas,       roverPrefs.getString("prfHotPas"));
            Serial.printf( "prfGnsNavRat           %u, %u, %u\n",             DEF_GNS_NAV_RAT,    prfGnsNavRat,    roverPrefs.getUShort("prfGnsNavRat"));
            Serial.printf( "prfGnsMsrInt           %u, %u, %u\n",             DEF_GNS_MSR_INT,    prfGnsMsrInt,    roverPrefs.getUShort("prfGnsMsrInt"));
            Serial.printf( "prfNtripCastAct        \"%s\", \"%s\", \"%s\"\n", DEF_NTRIP_CAST_ACT, prfNtripCastAct, roverPrefs.getString("prfNtripCastAct"));
            Serial.printf( "DEF_NTRIP_CAST_ATTR_1  \"%s\"\n", DEF_NTRIP_CAST_ATTR_1);
            Serial.printf( "prfNtripCastAttr[0]    \"%s\"\n", prfNtripCastAttr[0]);
            roverPrefs.getString("prfNtripCaster1", prfNtripCastAttr[3], NTRIP_CAST_ATTR_LEN);
            Serial.printf( "prfNtripCaster1        \"%s\"\n", prfNtripCastAttr[3]);
            Serial.printf( "DEF_NTRIP_CAST_ATTR_2  \"%s\"\n", DEF_NTRIP_CAST_ATTR_2);
            Serial.printf( "prfNtripCastAttr[1]    \"%s\"\n", prfNtripCastAttr[1]);
            roverPrefs.getString("prfNtripCaster2", prfNtripCastAttr[3], NTRIP_CAST_ATTR_LEN);
            Serial.printf( "prfNtripCaster2        \"%s\"\n", prfNtripCastAttr[3]);
            Serial.printf( "DEF_NTRIP_CAST_ATTR_3  \"%s\"\n", DEF_NTRIP_CAST_ATTR_3);
            Serial.printf( "prfNtripCastAttr[2]    \"%s\"\n", prfNtripCastAttr[2]);
            roverPrefs.getString("prfNtripCaster3", prfNtripCastAttr[3], NTRIP_CAST_ATTR_LEN);
            Serial.printf( "prfNtripCaster3        \"%s\"\n", prfNtripCastAttr[3]);

            // -- Close name space. --
            roverPrefs.end();
            break;
        
        case PREF_SET_NTRIP:

            // -- Open name space. --
            roverPrefs.begin("config", RW_MODE);

            // - Set NVS preference from global var. -
            switch (ntripCaster.id) {
                case 1:
                    roverPrefs.putString("prfNtripCaster1", prfNtripCastAttr[0]);   // Store as "prfNtripCastAttr[0]" (sent/rcvd as "39").
                    break;
                case 2:
                    roverPrefs.putString("prfNtripCaster2", prfNtripCastAttr[1]);   // Store as "prfNtripCastAttr[1]" (sent/rcvd as "40").
                    break;
                case 3:
                    roverPrefs.putString("prfNtripCaster3", prfNtripCastAttr[2]);   // Store as "prfNtripCastAttr[2]" (sent/rcvd as "41").
                    break;
            }

            // -- Close name space. --
            roverPrefs.end();
            Serial.println("NTRIP preference set.");
            break;
    }
}

/**
 * -------------------------------------------------------------------------
 *  Build data for operate page.
 * -------------------------------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.10 [2026-01-08-01:30pm] New
 * @since  3.0.12 [2026-02-18-11:00pm] Shorten RTCM & NMEA status.
 * @since  3.2.1  [2026-07-26-06:30pm] Refactor.
 * @see    Global vars: WebSockets, setup().
 */
 void buildOperData() {

    // -- Satellites in view. --
    numSatInView = roverGNSS.getSIV();

    if (numSatInView > MIN_SATELLITE_THRESHHOLD) {                          // Enough satellites?

        // -- Fix type. --
        if (roverGNSS.getFixType() == 3) {
            fixType = 1;                                                    // Single.
        } else if (roverGNSS.getCarrierSolutionType() == 1 ) {
            fixType = 2;                                                    // RTK-float.
        } else if (roverGNSS.getCarrierSolutionType() == 2 ) {
            fixType = 3;                                                    // RTK-fix.
        }

        /**
         * -- Heights: --
         * H = Orthometric height (elevation above sea level). 
         * N = Geoid height/undulation (separation between ellipsoid and geoid) based on a chosen geoid model.
         * Ellipsoid height (h) = H + N.
         * u-blox receivers use EGM96 (Earth Gravitational Model 1996).
         * EGM96 is an irregular, gravity-based surface geoid model, based on a 10° x 10° grid, and interpolated to the receiver's position.
         * WGS84 is a mathematical ellipsoid (smooth, idealized shape).
         */

        // -- Height - ellipsoid (h). --
        int32_t ellipsoid     = roverGNSS.getElipsoid();                    // mm
        int8_t ellipsoidHp    = roverGNSS.getElipsoidHp();                  // mm * 10^-1.
        heightEllipsoid = (ellipsoid * 10 + ellipsoidHp) / 10000.0;         // Convert to meters.

        // -- Height - orthometric (H). --
        int32_t msl               = roverGNSS.getMeanSeaLevel();            // a.k.a getAltitudeMSL()?
        int8_t  mslHp             = roverGNSS.getMeanSeaLevelHp();
        heightOrthometric = (msl * 10 + mslHp) / 10000.0;

        // -- Latitude. --
        int32_t latitude   = roverGNSS.getHighResLatitude();                // Degrees * 10^-7.
        int8_t  latitudeHp = roverGNSS.getHighResLatitudeHp();              // High precision component: degrees * 10^-9.
        lat  = latitude / 10000000.0;                                       // Convert to to 64 bit double - degrees (8 decimal places).
        lat += latitudeHp / 1000000000.0;                                   // Add high precision component.

        // -- Longitude. --
        int32_t longitude   = roverGNSS.getHighResLongitude();
        int8_t  longitudeHp = roverGNSS.getHighResLongitudeHp();
        lon  = longitude / 10000000.0;
        lon += longitudeHp / 1000000000.0;

        // -- Horizontal & vertical accuracy. --
        accuracyHorizontal = roverGNSS.getHorizontalAccuracy() / 10000.0;
        accuracyVertical   = roverGNSS.getVerticalAccuracy() / 10000.0;

        // -- RTCM & BT status. --
        // @see sendDataToBrowser().

        // -- Battery. --
        batterySoc        = lipo.getSOC();
        batteryChangeRate = lipo.getChangeRate();

        // -- Status. --
        int32_t seconds = (esp_timer_get_time() - startTime)/1000000;
        int32_t minutes = seconds / 60;
        int32_t hours = minutes / 60;
        snprintf(uptime, sizeof(uptime), "%uh %um %us", hours % 24, minutes % 60, seconds % 60);
    }

    // -- Flag pending browser update. Global vars are sent as JSON by processJsonActivity(). --
    browserUpdatePending = true;
}

/**
 * -------------------------------------------------------------------------
 *  Send data to browser.
 * -------------------------------------------------------------------------
 *
 * @return void No output is returned.
 * @since  3.2.1 [2026-07-26-06:30pm] New.
 * @since  3.2.1 [2026-07-30-10:30am] jsonDocToBrowser["NMEA"] '= lastNmea' was '= nmeaBuffer'.
 * @see    checkZedTriggerUpdate(), processJsonActivity(), DevUBLOXGNSS::processNMEA().
 * @see    processJsonActivity() for description of exchange protocol.
 */
void sendDataToBrowser() {

    // --- NMEA page. ---
    if (strcmp(whichPage, "nmea") == 0) {
        jsonDocToBrowser["NMEA"] = lastNmea;
    }

    // --- Operate page. ---
    if (strcmp(whichPage, "operate") == 0) {
        if (numSatInView < MIN_SATELLITE_THRESHHOLD) {

            // --GNSS down. --
            jsonDocToBrowser["8"]  = 0;
            jsonDocToBrowser["9"]  = 0;
            jsonDocToBrowser["12"] = 0;
            jsonDocToBrowser["13"] = 0;
            jsonDocToBrowser["10"] = 0;
            jsonDocToBrowser["11"] = 0;
            jsonDocToBrowser["15"] = 0;
            jsonDocToBrowser["2"]  = 0;
            jsonDocToBrowser["3"]  = 0;
        } else {
            jsonDocToBrowser["8"] = fixType;
            jsonDocToBrowser["9"] = numSatInView;
            memset(operBuffer, '\0', sizeof(operBuffer));
            snprintf(operBuffer, sizeof(operBuffer), "%.2f", heightEllipsoid);
            jsonDocToBrowser["10"] = operBuffer;
            memset(operBuffer, '\0', sizeof(operBuffer));
            snprintf(operBuffer, sizeof(operBuffer), "%.2f", heightOrthometric);
            jsonDocToBrowser["11"] = operBuffer;
            memset(operBuffer, '\0', sizeof(operBuffer));
            snprintf(operBuffer, sizeof(operBuffer), "%.8f", lat);
            jsonDocToBrowser["12"] = operBuffer;
            memset(operBuffer, '\0', sizeof(operBuffer));
            snprintf(operBuffer, sizeof(operBuffer), "%.8f", lon);
            jsonDocToBrowser["13"] = operBuffer;
            memset(operBuffer, '\0', sizeof(operBuffer));
            snprintf(operBuffer, sizeof(operBuffer), "%.3f", accuracyHorizontal);
            jsonDocToBrowser["14"] = operBuffer;
            memset(operBuffer, '\0', sizeof(operBuffer));
            snprintf(operBuffer, sizeof(operBuffer), "%.3f", accuracyVertical);
            jsonDocToBrowser["15"] = operBuffer;
            jsonDocToBrowser["16"] = (RTCMin)  ? "u" : "d";         // Up, down. @see relaySerial1toSerial2().
            jsonDocToBrowser["17"] = (NMEAout) ? "u" : "d";         // Up, down. @see DevUBLOXGNSS::processNMEA().
            memset(operBuffer, '\0', sizeof(operBuffer));
            snprintf(operBuffer, sizeof(operBuffer), "%.2f", batterySoc);
            jsonDocToBrowser["18"] = operBuffer;
            memset(operBuffer, '\0', sizeof(operBuffer));
            snprintf(operBuffer, sizeof(operBuffer), "%.1f", batteryChangeRate);
            jsonDocToBrowser["19"] = operBuffer;
            jsonDocToBrowser["20"] = uptime;
            jsonDocToBrowser["23"] = nmeaCountGGA;
            jsonDocToBrowser["24"] = nmeaCountRMC;
            jsonDocToBrowser["25"] = nmeaCountGSA;
            jsonDocToBrowser["26"] = nmeaCountGSV;
            jsonDocToBrowser["27"] = nmeaCountGST;
            jsonDocToBrowser["28"] = nmeaCountTXT;
            jsonDocToBrowser["29"] = nmeaCountOther;
            jsonDocToBrowser["30"] = nmeaCountAll;
            jsonDocToBrowser["31"] = nmeaRate;
            jsonDocToBrowser["32"] = operMode;
            jsonDocToBrowser["33"] = localIp;
            jsonDocToBrowser["34"] = hotspotIp;
            jsonDocToBrowser["37"] = rtcmSentenceCount;
            jsonDocToBrowser["38"] = rtcmKbps;
        }
    }

    // --- All pages. ---
    memset(jsonBuffer, '\0', sizeof(jsonBuffer));
    serializeJson(jsonDocToBrowser, jsonBuffer, sizeof(jsonBuffer));
    ws.textAll(jsonBuffer);                         // Send WebSocket message.
    wsSendCount++;
    if (commandFlag[DEBUG_WS]) {                    // Debug.
        Serial.printf("WS #%u: browser <-- %s\n\n", clientId, jsonBuffer);
    } else {
        if (response[0] != '\0') {
            Serial.println(response);
        }
    }
}

/**
 * -------------------------------------------------------------------------
 *  Return RTCM3 message type to taskRtcmRelay().
 * -------------------------------------------------------------------------
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
uint16_t rtcm3GetMessageType(const char* rtcmSentence) {
    // Serial.printf("[%02x] [%02x] [%02x] [%02x] [%02x]\n", rtcmSentence[0],  rtcmSentence[1], rtcmSentence[2], rtcmSentence[3], rtcmSentence[3]);
    if (rtcmSentence[0] != 0xD3) {    // Check if preamble is correct
        return 0;               // Invalid preamble.
    }
    uint16_t message_type = ((uint16_t)rtcmSentence[3] << 4) | (rtcmSentence[4] >> 4);
    return message_type;
}

/**
 * =========================================================================
 *  Setup functions.
 * =========================================================================
 *
 * @since 3.0.11 [2026-01-08-10:30am] Browser initiated updates.
 * @see   showBuild()            - Display build & processor info.
 * @see   prefUtility(PREF_INIT) - Preference utility (get preferences).
 * @see   startSerial()          - Start serial interfaces.
 * @see   initPins()             - Initialize pins & pin values.
 * @see   startI2C()             - Start I2C wire interfaces.
 * @see   startLiPo()            - Start LiPo I2C interface.
 * @see   startWiFi()            - Start WiFi.
 * @see   startSD()              - Start & test microSD card reader.
 * @see   startHttpServer()      - Start HTTP server.
 * @see   startWebSocketServer() - Start WebSocket server.
 * @see   startAndConfigGNSS()   - Start GNSS, config ZED settings.
 * @see   startQueues()          - Start GhostRover FreeRTOS queues.
 * @see   startTasks()           - Start GhostRover FreeRTOS tasks.
 * @see   preLoop()              - Prepare for loop().
 */

/**
 * -------------------------------------------------------------------------
 *  Display build & processor info.
 * -------------------------------------------------------------------------
 * 
 * Default pins for ESP32-S3 Thing Plus using Arduino core:
 *   GPIO 19 - Serial USB UART0 used as Communication Device Class interface D- (negative data line).
 *   GPIO 20 - Serial USB UART0 used as Communication Device Class interface D+ (positive data line).
 * 
 * @return void  No output is returned.
 * @since  3.0.10 [2025-12-30-02:00pm].
 * @since  3.0.10 [2026-01-07-09:45am] Local vars.
 * @since  3.1.1  [2026-06-25-01:00pm] Updated version, added startup delay, transition LED YELLOW->WHITE.
 * @since  3.1.2  [2026-07-16-10:00am] Moved MAJOR, MINOR, PATCH from showBuild() to "Operation" section.
 * @see    Global vars: Version, setup().
 * @link   https://github.com/pycom/pycom-esp-idf.
 */
void showBuild() {

    // --- Local vars. ---
    const char      NAME[]           = "Ghost Rover 3";
    const uint32_t  SERIAL_USB_SPEED = 115200;   // Serial USB speed.
    const uint64_t  START_DELAY      = 4000000;  // 4 second startup delay.
    esp_chip_info_t chip_info;

    // --- Run. ---
    startTime = esp_timer_get_time();
    ws2812LedColor = YELLOW;
    ws2812LedBlink = false;
    statusLedOn();
    Serial.begin(SERIAL_USB_SPEED);
    serialState[0] = 'u';   // Serial USB is up [u] [S0] [S1] [S2].
    esp_chip_info(&chip_info);
    sprintf(buildString, "%u.%u.%u - %s @ %s", MAJOR_VERSION, MINOR_VERSION, PATCH_VERSION, __DATE__, __TIME__);
    while ((esp_timer_get_time() - startTime) < START_DELAY) {
        vTaskDelay(1);  // busy-wait; yield to RTOS if needed
    }
    ws2812LedColor = WHITE;
    // Serial.print("\033[2J");   // Clear screen before displaying boot messages.
    Serial.println('\n');         // Empty lines before displaying boot messages.
    Serial.printf("%s\n%s\n", NAME, buildString);
    Serial.printf("Using %s, Rev %d, %d core(s), ID (MAC) %012llX.\n", ESP.getChipModel(), chip_info.revision, chip_info.cores, ESP.getEfuseMac());
    Serial.println("setup() started.");
    Serial.printf("Serial (USB) started @ %u bps.\n", SERIAL_USB_SPEED);
}

/**
 * -------------------------------------------------------------------------
 *  Start serial interfaces.
 * -------------------------------------------------------------------------
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
 * @see    showBuild(), setup().
 * @link   https://github.com/G6EJD/ESP32-Using-Hardware-Serial-Ports.
 * @link   https://randomnerdtutorials.com/esp32-uart-communication-serial-arduino/#esp32-custom-uart-pins.
 */
void startSerial() {

    // --- Local vars. ---
    const uint8_t  HC12_TX       =  5;                              // HC-12 TXD     {white wire}.
    const uint8_t  HC12_RX       =  6;                              // HC-12 RXD     {yellow wire}.
    const uint8_t  ZED_RX2       = 17;                              // ZED UART2 RX2 {white wire}.
    const uint8_t  ZED_TX2       = 16;                              // ZED UART2 TX2 {yellow wire} (not used).
    const uint32_t SERIAL1_SPEED = 9600;                            // HC-12 default speed is 9600.
    // const uint32_t SERIAL2_SPEED = 57600;                        // ZED UART2 default speed is 38400.
    const uint32_t SERIAL2_SPEED = 38400;                           // ZED UART2 default speed is 38400.

    // --- Start serial interfaces. ---
    serialState[1] = '-';   // Serial0 is not used: [USB][-][S1][S2].
    Serial.println("Serial0 is not used.");
    Serial1.begin(SERIAL1_SPEED, SERIAL_8N1, HC12_TX, HC12_RX);     // UART1 object. RX, TX.
    serialState[2] = 'u';   // Serial1 is up [USB][S0][u][S2].
    Serial.printf("Serial1 (HC-12) started @ %i bps.\n", SERIAL1_SPEED);
    Serial2.begin(SERIAL2_SPEED, SERIAL_8N1, ZED_RX2, ZED_TX2);     // UART2 object. RX, TX.
    serialState[3] = 'u';   // Serial2 is up [USB][S0][S1][u].
    Serial.printf("Serial2 (ZED UART2) started @ %i bps.\n", SERIAL2_SPEED);
}

/**
 * -------------------------------------------------------------------------
 *  Initialize pins modes & pin values.
 * -------------------------------------------------------------------------
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
 * -------------------------------------------------------------------------
 *. Start I2C wire interfaces.
 * -------------------------------------------------------------------------
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
 * -------------------------------------------------------------------------
 *  Start LiPo I2C interface.
 * -------------------------------------------------------------------------
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
 * -------------------------------------------------------------------------
 *  Start WiFi.
 * -------------------------------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.7  [2025-11-20-12:30pm]. New.
 * @since  3.0.10 [2026-01-07-11:00am] Local vars.
 * @since  3.0.12 [2026-01-27-04:00pm] Refactor from AP mode to AP+Station mode.
 * @since  3.0.12 [2026-02-01-05:30pm] Use preferences.
 * @see    setup(), prefUtility().
 * @link   https://github.com/espressif/arduino-esp32/tree/master/libraries/WiFi.
 * @link   https://docs.espressif.com/projects/arduino-esp32/en/latest/api/wifi.html.
 * @link   https://docs.espressif.com/projects/arduino-esp32/en/latest/api/network.html.
 */
void startWiFi() {

    // --- Local ESP32 Access Point (AP) network. ---
        WiFi.mode(WIFI_AP_STA);                         // Enable dual mode

        // -- Local Vars. --
        const char AP_SSID[] = "Ghost Rover";           // Local ESP32 Access Point (AP) network.
        const char AP_NAME[] = "ghost";                 // AP name.
        const IPAddress AP_LOCAL_IP(192, 168, 23, 1);   // AP host address.
        const IPAddress AP_GATEWAY(192, 168, 23, 1);    // AP gateway address.
        const IPAddress AP_SUBNET(255, 255, 255, 0);    // AP subnet mask.

        // -- Global Vars. --
        // snprintf(localIp, sizeof(localIp), "%d.%d.%d.%d", AP_LOCAL_IP[0], AP_LOCAL_IP[1], AP_LOCAL_IP[2], AP_LOCAL_IP[3]);

        // -- Config & start AP (aka WiFi server). --
        if (!WiFi.softAPConfig(AP_LOCAL_IP, AP_GATEWAY, AP_SUBNET)) {   // Configure IP network.
            Serial.println("Soft AP - config failed.");
            while (true) {
                ws2812LedColor = RED;
                ws2812LedBlink = false;
                statusLedOn();
            };
        }
        if (!WiFi.softAP(AP_SSID)) {                                    // Set AP SSID & start. No password.
            Serial.println("Soft AP - create failed. Freezing.");
            while (true) {
                ws2812LedColor = RED;
                ws2812LedBlink = false;
                statusLedOn();
            };
        }
        WiFi.softAPsetHostname(AP_NAME);                                // Set hostname.
        WiFi.onEvent(onWiFiEvent);                                      // Add WiFiEvent() as event handler.
        IPAddress ip = WiFi.softAPIP();                                 // Start WiFi & check status (get IP).
        snprintf(localIp, sizeof(localIp), "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
        Serial.printf("WiFi server (AP mode) \"%s\" started @ %s.\n", AP_SSID, localIp);

    // --- Cellular hotspot client. ---

        // -- Local Vars. --
        size_t maxTrys = 20;                        // Max # of trys to connect to STA_SSID.
        IPAddress STA_IP(172, 20, 10, 2);           // Request to use this IP address.

        // -- Global Vars. --
        // snprintf(hotspotIp, sizeof(hotspotIp), "%d.%d.%d.%d", STA_IP[0], STA_IP[1], STA_IP[2], STA_IP[3]);

        // -- Config & start hotspot client. --
        // - char prfHotSsi[] = "ssid";  // WiFi hotspot client: network SSID. -
        // - char prfHotPas[] = "pass";  // WiFi hotspot client: password. -
        if ((strcmp(prfHotSsi, "ssid") != 0) && (strcmp(prfRtcIn, "ntrip") == 0)) {  // RTCMin by NTRIP requires Internet hotspot access, RTCMin by radio does not.
            Serial.printf("WiFi STA connecting to \"%s\" ", prfHotSsi);
            WiFi.config (STA_IP);
            WiFi.begin(prfHotSsi, prfHotPas);

            size_t numTrys;
            for (numTrys = 0; numTrys < maxTrys; numTrys++) {
                Serial.print('.');
                if (WiFi.status() == WL_CONNECTED) {
                    strlcpy(hotspotIp, WiFi.localIP().toString().c_str(), sizeof(hotspotIp));
                    Serial.printf(", connected with IP = %s.\n", hotspotIp);
                    ws2812LedColor = WHITE;
                    ws2812LedBlink = false;
                    statusLedOn();
                    break;
                }
                delay(1000);                            // Try again.
            }
            if (numTrys == maxTrys) {
                Serial.printf(", max trys exceeded, not connected.\n", prfHotSsi);
                strlcpy(hotspotIp, " ", sizeof(hotspotIp));
            }
        }
}

/**
 * -------------------------------------------------------------------------
 *  Start & test microSD card reader.
 * -------------------------------------------------------------------------
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
        while (true) {
            ws2812LedColor = RED;
            ws2812LedBlink = false;
            statusLedOn();
        };
    }
    Serial.println("SDIO started.");

    // --- Start SD reader. ---
    delay(STARTUP);
    if (!SD.begin(SPI_CS)) {
        Serial.println("SD card not started. Freezing.");
        ws2812LedColor = RED;
        ws2812LedBlink = false;
        statusLedOn();
        while (true);
    }
    Serial.println("SD card started.");

    // --- Test card. ---
    delay(STARTUP);
    Serial.print("SD card test - ");
    File file = SD.open(TEST_FILE, "r");
    if (file == false) {
        Serial.println("failed. Freezing.");
        ws2812LedColor = RED;
        ws2812LedBlink = false;
        statusLedOn();
        while (true);
    }
    Serial.println("OK.");
    file.close();
}

/**
 * -------------------------------------------------------------------------
 *  Start HTTP server. 
 * -------------------------------------------------------------------------
 *
 * Set endpoints & start.
 * 
 * @return void  No output is returned.
 * @since  3.0.7 [2025-11-11-06:15pm].
 * @since  3.0.10 [2026-01-07-11:30am] Local vars.
 * @see    setup(), onHttpFileUpload().
 * @link   https://github.com/ESP32Async/ESPAsyncWebServer/wiki#get-post-and-file-parameters.
 * @link   https://github.com/ESP32Async/AsyncTCP.
 * @link   https://github.com/ESP32Async/ESPAsyncWebServer.
 */
void startHttpServer() {

    // --- Local vars. ---
    const char PAGE_ROOT[]     = "/";
    const char PAGE_UPLOAD[]   = "/upload";
    const char PAGE_DOWNLOAD[] = "/download";

    // --- Route: root. ---
    httpServer.on(PAGE_ROOT, HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.printf("httpServer - Page \"%s\" requested.\n", request->url().c_str());
        request->send(SD, "/index.html", "text/html");      // Set root.
    }) ;

    // --- Route: file upload. ---   
    httpServer.on(PAGE_UPLOAD, HTTP_POST, [](AsyncWebServerRequest *req) {
        req->send(200, "text/plain", "Upload complete");
        Serial.println("httpServer - File upload complete.");
    }, onHttpFileUpload);                                   // Register endpoint handler.

    // --- Route: file download. ---
    httpServer.on(PAGE_DOWNLOAD, HTTP_GET, [](AsyncWebServerRequest *request) { 
        if (request->hasParam("file")) {                    // Process request.
            String filename = request->getParam("file")->value();
            String filepath = "/" + filename;
            if (SD.exists(filepath)) {
                request->send(SD, filepath, "application/octet-stream", true);
                Serial.printf("httpServer - Downloading file: %s\n", filename.c_str());
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
 * -------------------------------------------------------------------------
 *  Start WebSocket server.
 * -------------------------------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.3 [2025-10-13-01:00pm].
 * @since  3.0.10 [2026-01-07-12:00pm] WEBSOCKET_SERVER_NAME.
 * @since  3.0.12 [2026-02-14-06:00pm] Add softwareResetGNSSOnly().
 * @see    setup(), onWebSocketEvent().
 * @link   https://randomnerdtutorials.com/esp32-websocket-server-arduino/.
 * @link   https://shawnhymel.com/1882/how-to-create-a-web-server-with-websockets-using-an-esp32-in-arduino/.
 */
void startWebSocketServer() {
    ws.onEvent(onWebSocketEvent);
    httpServer.addHandler(&ws);     // startWebServer() must run first.
    Serial.printf("WebSocket server \"%s\" started.\n", WEBSOCKET_SERVER_NAME);
}

/**
 * -------------------------------------------------------------------------
 *  Start GNSS, config ZED settings.
 * -------------------------------------------------------------------------
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
 * @since  3.0.11 [2026-01-26-04:15pm] Rework config, see wiring diagram.
 * @since  3.0.12 [2026-02-01-12:15pm] Changed to prfGnsNavRat & prfGnsMsrInt.
 * @see    Global vars: GNSS, prefUtility(), startSerial(), beginI2C().
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

    // --- Start GNSS interface on I2C-1. ---
    if (roverGNSS.begin() == false) {
        Serial.println("Start roverGNSS failed. Freezing ...");     // Something is wrong, freeze.
        ws2812LedColor = RED;
        ws2812LedBlink = false;
        statusLedOn();
        while (true);                                                // Infinite loop.
    } else {

        // -- Software reset. --
        roverGNSS.softwareResetGNSSOnly();
        Serial.println("RoverGNSS started.\nEnumerating satellite constellations.");
        delay(1000); // Short delay to allow the module to complete the reset process.

        // uint16_t    prfGnsMsrInt;  // ZED: MEASURE every Y (e.g. 100) ms.
        // uint8_t     prfGnsNavRat;  // ZED: OUTPUT every X (e.g. 5) MEASURE intervals every (e.g. 5*100=500) ms.
        // roverGNSS.setNavigationFrequency(2) will produce 1 solution every 500ms, but only uses 2 (not 5) measurements per second.
        roverGNSS.setNavigationRate(prfGnsNavRat, VAL_LAYER_RAM);
        roverGNSS.setMeasurementRate(prfGnsMsrInt, VAL_LAYER_RAM);
        Serial.printf("Solution output every (%u * %u) ms.\n", prfGnsNavRat, prfGnsMsrInt);
    }

    // --- New config template. ---
    roverGNSS.newCfgValset(VAL_LAYER_RAM);                          // Save only to RAM.

    // --- Enable high precision mode. ---
    roverGNSS.addCfgValset(UBLOX_CFG_NMEA_HIGHPREC,          1);    // NMEA - High precision (7 instead of 5 decimal places for lat/lon in NMEA sentences).

    // --- Push solutions onto I2. ---
    roverGNSS.addCfgValset(UBLOX_CFG_MSGOUT_UBX_NAV_PVT_I2C, 1);    // Output solutions periodically on I2C.  

    // --- Minimize ZED processing: UART1 & SPI are not used. UART2 only uses RTCM in. UBX & NMEA over I2C, UBX & NMEA over USB for pygpsclient debugging. ---
    roverGNSS.addCfgValset(UBLOX_CFG_UART1_ENABLED,          0);    // UART1 - Disable (on by default).  SPI is off by default.
    roverGNSS.addCfgValset(UBLOX_CFG_UART2INPROT_SPARTN,     0);    // UART2 - Disable SPARTN in (on by default). Only RTCM3 in is needed.
    roverGNSS.addCfgValset(UBLOX_CFG_UART2INPROT_UBX,        0);    // UART2 - Disable UBX in (on by default). 
    roverGNSS.addCfgValset(UBLOX_CFG_UART2OUTPROT_UBX,       0);    // UART2 - Disable UBX in (on by default).
    roverGNSS.addCfgValset(UBLOX_CFG_UART2OUTPROT_RTCM3X,    0);    // UART2 - Disable RTCM3 out (on by default).
    roverGNSS.addCfgValset(UBLOX_CFG_I2CINPROT_RTCM3X,       0);    // I2C - Disable RTCM3 in (on by default).
    roverGNSS.addCfgValset(UBLOX_CFG_I2COUTPROT_RTCM3X,      0);    // I2C - Disable RTCM3 out (on by default).
    roverGNSS.addCfgValset(UBLOX_CFG_USBINPROT_RTCM3X,       0);    // USB - Disable RTCM3 in (on by default).
    roverGNSS.addCfgValset(UBLOX_CFG_USBOUTPROT_RTCM3X,      0);    // USB - Disable RTCM3 out (on by default).

    // --- Minimize I2C bandwidth. ---
    roverGNSS.addCfgValset(UBLOX_CFG_MSGOUT_NMEA_ID_GLL_I2C, 0);    // I2C messages - Disable GLL (on by default).
    roverGNSS.addCfgValset(UBLOX_CFG_MSGOUT_NMEA_ID_VTG_I2C, 0);    // I2C messages - Disable GLL (on by default).
    roverGNSS.addCfgValset(UBLOX_CFG_MSGOUT_NMEA_ID_GSA_I2C, 3);    // I2C messages - Reduce GSA to 1 per 3 solutions (default is 1 per 1 solution).
    roverGNSS.addCfgValset(UBLOX_CFG_MSGOUT_NMEA_ID_GSV_I2C, 5);    // I2C messages - Reduce GSV to 1 per 5 solutions (default is 1 per 1 solution).
    roverGNSS.addCfgValset(UBLOX_CFG_MSGOUT_NMEA_ID_GST_I2C, 3);    // I2C messages - Enable GSA to 1 per 3 solutions  (default is 1 per 1 solution).
                                                                    // ZDA & GNS sentences are off by default.
    // --- Send the config. ---
    roverGNSS.sendCfgValset() ? Serial.println("roverGNSS configured using valset keys.") : Serial.println("roverGNSS config failed!");

    // --- Not used. ---
    // roverGNSS.newCfgValset(VAL_LAYER_RAM_BBR);
    // roverGNSS.addCfgValset(UBLOX_CFG_MSGOUT_UBX_NAV_PVT_I2C, 1); // Output solutions periodically on I2C.
    // roverGNSS.addCfgValset(UBLOX_CFG_I2CINPROT_NMEA,      0);    // I2C - Turn off NMEA protocol in. Default is on.
    // roverGNSS.addCfgValset(UBLOX_CFG_I2COUTPROT_NMEA,     0);    // I2C - Turn on NMEA protocol out. Default is on.
    // roverGNSS.addCfgValset(UBLOX_CFG_I2CINPROT_RTCM3X,    0);    // I2C - Turn off RTCM3 protocol in. Default is ?.
    // roverGNSS.addCfgValset(UBLOX_CFG_I2COUTPROT_RTCM3X,   0);    // I2C - Turn off RTCM3 protocol out. Default is ?.
    // roverGNSS.saveConfiguration();                               // Save current settings to BBR/Flash.
    // roverGNSS.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT);        // Save port settings to flash and BBR.
    // roverGNSS.enableDebugging();                                 // Debug - all messages over Serial (default).
}
/**
 * -------------------------------------------------------------------------
 *  Start GhostRover FreeRTOS queues.
 * -------------------------------------------------------------------------
 *
 * @return void No output is returned.
 * @since  3.2.2 [2026-07-29] New. Threadsafe queues shared by FreeRTOS tasks and loop() functions.
 * @see    setup(), onWebSocketEvent(), processJsonActivity().
 */
void startQueues() {
    wsRxQueue = xQueueCreate(WS_RX_QUEUE_LEN, sizeof(WsQueueItem));
    if (wsRxQueue == NULL) {
        Serial.println("Failed to create wsRxQueue. Freezing.");
        ws2812LedColor = RED;
        ws2812LedBlink = false;
        statusLedOn();
        while (true);
    }
    Serial.println("GhostRover FreeRTOS queue \"wsRxQueue\" created.");
}

/**
 * -------------------------------------------------------------------------
 *  Start GhostRover FreeRTOS tasks.
 * -------------------------------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.7  [2025-11-14-04:30pm].
 * @since  3.0.11 [2026-01-08-10:30am] Remove taskSendGnss() & taskSendBatteryStatus().
 * @since  3.1.2  [2026-07-03-07:30pm] xTaskCreatePinnedToCore from 4096 to 8192.
 * @see    Global vars: FreeRTOS handles.
 * @see    setup().
 * @link   https://www.freertos.org/Documentation/02-Kernel/04-API-references/01-Task-creation/01-xTaskCreate.
 */
void startTasks() {

    // --- Loop status LED. ---
    xTaskCreate(taskLoopStatusLed, "Loop status LED", 2048, NULL, 2, &taskLoopStatusLedHandle);
    Serial.println("GhostRover FreeRTOS task \"Loop status LED\" started.");

    // --- RTCM relay. ---
    // Arduino-ESP32 core 0 defaults: WiFi/BT.
    // Arduino-ESP32 core 1 defaults: Arduino loop(), WiFi/I2C.
    // Pin taskRtcmRelay() to core 0 for parallel execution instead of round-robin in loop() since I2C calls block and don't yield.
    xTaskCreatePinnedToCore(taskRtcmRelay, "RTCM_Relay", 8192, NULL, 2, &taskRtcmRelayHandle, 0);
    Serial.println("GhostRover FreeRTOS task \"RTCM relay\" started.");
}

/**
 * -------------------------------------------------------------------------
 *  Prepare for loop().
 * -------------------------------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.7 [2025-11-21-06:00pm] Added inLoop.
 * @see    setup().
 */
void preLoop() {
    ws2812LedColor = BLUE;
    ws2812LedBlink = false;
    operMode[0]    = 'r';
    inLoop         = true;
    Serial.println("Loop() starting.");
}

/**
 * =========================================================================
 *  GhostRover FreeRTOS functions.
 * =========================================================================
 *
 * @since 3.0.11 [2026-01-08-10:30am] Browser initiated updates.
 * @see   startTasks()          - Start GhostRover FreeRTOS tasks in setup().
 * @see   taskLoopStatusLed()   - GhostRover FreeRTOS task - Set Loop() status LED to blink or solid.
 * @see   rtcm3GetMessageType() - Return RTCM3 message type to taskRtcmRelay().
 * @see   taskRtcmRelay()       - GhostRover FreeRTOS task - Relay RTCM from Serial1 (HC-12) to -> Serial2 (ZED UART2).
 */

/**
 * -------------------------------------------------------------------------
 *  GhostRover FreeRTOS task - Set Loop() status LED to blink or solid.
 * -------------------------------------------------------------------------
 * 
 * Default pins for ESP32-S3 Thing Plus using Arduino core:
 *   GPIO 46 - green Status (STAT) LED.
 *   GPIO  2 - WS2812 LED.
 *   GPIO 23 - RGB BUILTIN LED.
 *
 * @param  void  * pvParameters Pointer to FreeRTOS task parameters.
 * @return void  No output is returned.
 * @since  3.0.3  [2025-11-09-10:30am] New.
 * @since  3.0.10 [2026-01-07-09:00am] Local vars.
 * @since  3.0.11 [2026-01-08-02:30pm] Remove debug.
 * @since  3.0.12 [2026-02-10-10:45pm] Status LED changes.
 * @see    startTasks().
 * @link   https://www.freertos.org/Documentation/02-Kernel/04-API-references/02-Task-control/06-vTaskSuspend.
 */
void taskLoopStatusLed(void * pvParameters) {

    // --- Local vars. ---
    const TickType_t DELAY      = 40/portTICK_PERIOD_MS;            // Timer (ms) =  0.04 seconds.

    // --- Loop. ---
    while(true) {
        statusLedOn();
        vTaskDelay(DELAY);
        if (ws2812LedBlink == true) {
            rgbLedWrite(LED_BUILTIN, 0, 0, 0);                      // LED off.
            ws2812LedBlink = false;
        }
    }
}

/**
 * -------------------------------------------------------------------------
 *  GhostRover FreeRTOS task - Relay RTCM from Serial1 (HC-12) to -> Serial2 (ZED UART2).
 * -------------------------------------------------------------------------
 *
 * RTCM preamble = '11010011 000000xx' = 0xd3 0x00.
 *
 *  ESP32-S3 Serial1 (HC12) is set to 9,600 bps (default speed) in Global Vars.
 *  ESP32-S3 Serial2 (ZED UART2) is set to 57,600 bps in Global Vars.
 *  RTK-SMA (ZED UART2) is set to 57,600 bps by default (could change in startAndConfigGNSS() ).
 * 
 * Runs independently of loop() so blocking I2C calls in checkZedTriggerUpdate()
 * (NMEA-over-I2C forwarding) can't starve the RTCM relay. Drains Serial1 fully
 * on every wake so any backlog from a stall clears immediately instead of
 * trickling out one byte per loop() pass.
 *
 * @param  void * pvParameters Pointer to FreeRTOS task parameters.
 * @return void   No output is returned (infinite loop).
 * @since  3.1.2  [2026-07-03-06:15pm] New, replaced relaySerial1toSerial2() in loop().
 * @since  3.1.2  Added if (byteCount < sizeof(rtcmSentence) - 1) to check for rtcmSentence overflow.
 * @since  3.2.1  [2026-07-29-09:30am] Added guard to prevent rtcmKbps form calculating as null.
 * @see    startTasks().
 * @see    rtcm3GetMessageType().
 * @see    Global vars: Serial, startSerialInterfaces(), loop().
 * @link   https://github.com/sparkfun/SparkFun_u-blox_GNSS_v3/blob/main/examples/ZED-F9P/Example3_StartRTCMBase/Example3_StartRTCMBase.ino.
 * @link   https://www.use-snip.com/kb/knowledge-base/an-rtcm-message-cheat-sheet/.
 * @link   https://www.use-snip.com/kb/knowledge-base/rtcm-3-message-list/.
 * @link   https://www.singularxyz.com/blog_detail/11.
 */
void taskRtcmRelay(void *pvParameters) {

// --- Local vars. ---
    const  uint16_t RTCM_TIMEOUT      = 3000000;                        // Time (us) not to exceed for RTCM input received (3 sec).
           uint16_t byteCount         =       0;
           int64_t  lastRTCMtime      =       0;                        // Last time (us) when RTCM input received.
           char     rtcmSentence[1030] =  {'\0'};                        // RTCM3 sentence buffer.
           uint16_t msg_type          =       0;

    // --- Loop. ---
    for (;;) {

        // -- Check preference. --
        if (strcmp(prfRtcIn, "off") == 0) {
            vTaskDelay(pdMS_TO_TICKS(50));                              // Idle - nothing to relay.
            continue;
        }

        // -- Check for radio down. Set RTCMin state. --
        if ((esp_timer_get_time() - lastRTCMtime) > RTCM_TIMEOUT) {
            RTCMin = false;
            ws2812LedColor = GREEN;
            ws2812LedBlink = false;
        }

        // -- Drain Serial1 (HC-12) fully, write each byte to Serial2 (ZED UART2). --
        while (Serial1.available() > 0) {                               // Loop until caught up, not just once.
            char inputChar = Serial1.read();                            // Read a character from Serial1 (HC-12) @ SERIAL1_SPEED.
            Serial2.write(inputChar);                                   // Write a character to Serial2 (ZED UART2) @ SERIAL2_SPEED.
            if (byteCount < sizeof(rtcmSentence) - 1) {                 // Bounds check - prevent stack buffer overflow (relay above is unaffected either way).
                rtcmSentence[byteCount] = inputChar;                    // RTCM3 sentence buffer used to parse message type.
            }
            RTCMin = true;
            ws2812LedColor = GREEN;
            ws2812LedBlink = true;

            // - Stats. -
            if (inputChar == 0xd3) {                                    // Start of new sentence.
                rtcmSentenceCount++;
                msg_type = rtcm3GetMessageType(rtcmSentence);           // Parse message type.
                int64_t RTCMintervalUs = esp_timer_get_time() - lastRTCMtime;
                int64_t RTCMinterval = RTCMintervalUs / 1000;           // Ms for display only.
                if (RTCMintervalUs > 0) {
                    rtcmKbps = ((float)byteCount * 8.0f * 1000.0f) / (float)RTCMintervalUs;     // kbps = bits / ms.
                } else {
                        // Interval too short to measure meaningfully — hold last known value rather than emit garbage/null.
                }
                if (commandFlag[DEBUG_RTCM]) {          // Debug.
                    Serial.printf("\nRTCM3 #%zu Type:%u bytes:%u ms:%lld kbps:%.2f\n\nd3 ", rtcmSentenceCount, msg_type, byteCount, RTCMinterval, rtcmKbps);
                }
                lastRTCMtime = esp_timer_get_time();                    // Used to check for timeout.
                memset(rtcmSentence, '\0', sizeof(rtcmSentence));       // Clear the sentence buffer.
                rtcmSentence[0] = 0xd3;
                byteCount = 1;
            } else {
                if (commandFlag[DEBUG_RTCM]) {                          // Debug.
                    Serial.printf("%02x ", inputChar);
                }
                byteCount++;
            }
        }
        vTaskDelay(1);                                                  // Yield 1 tick when idle - keeps watchdog/other tasks fed.
    }
}

/**
 * =========================================================================
 *  Event handlers for core/additional library processes.
 * =========================================================================
 *
 * @since 3.0.11 [2026-01-12-06:00pm] Browser initiated updates.
 * @see   onWiFiEvent()               - <WiFi.h> & <WiFiAP.h> WiFi event handler (WiFiEvent_t).
 * @see   onHttpFileUpload()          - <ESPAsyncWebServer.h> HTTP endpoint ("/upload") event handler (AsyncWebServerRequest).
 * @see   onWebSocketEvent()          - <ESPAsyncWebServer.h> WebSocket event handler (AsyncWebSocket).
 * @see   DevUBLOXGNSS::processNMEA() - <SparkFun_u-blox_GNSS_v3.h> DevUBLOXGNSS::processNMEA event handler (char incoming).
 */

/**
 * -------------------------------------------------------------------------
 *  <WiFi.h> & <WiFiAP.h> WiFi event handler (WiFiEvent_t).
 * -------------------------------------------------------------------------
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
 * -------------------------------------------------------------------------
 *  <ESPAsyncWebServer.h> HTTP endpoint ("/upload") event handler (AsyncWebServerRequest).
 * -------------------------------------------------------------------------
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
 * -------------------------------------------------------------------------
 *  <ESPAsyncWebServer.h> WebSocket event handler (AsyncWebSocket).
 * -------------------------------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.3 [2025-11-08-03:15pm] New.
 * @since  3.0.8 [2025-12-01-05:15pm] Changed color & blink status.
 * @since  3.2.1  [2026-07-28-04:45pm] Removed NMEA out switch & preference.
 * @since  3.2.1  [2026-07-30-10:00am] Refactored case WS_EVT_DATA.
 * @see    startWebSocketServer().
 * @link   https://randomnerdtutorials.com/esp32-websocket-server-arduino/.
 * @link   https://shawnhymel.com/1882/how-to-create-a-web-server-with-websockets-using-an-esp32-in-arduino/.
 */
void onWebSocketEvent(AsyncWebSocket *httpServer, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    clientId = client->id();
    switch (type) {
        case WS_EVT_CONNECT:
            Serial.printf("WS #%u: %s connected to server.\n", clientId, client->remoteIP().toString().c_str());
            ws2812LedColor = GREEN;                         // Loop status indicator LED.
            ws2812LedBlink = false;
            wsSendCount    = 0;                             // Reset counter.
            break;
        case WS_EVT_DISCONNECT:
            Serial.printf("WS #%u: disconnected.\n\n", clientId);
            ws2812LedColor = BLUE;
            ws2812LedBlink = false;
            wsSendCount    = 0;                             // Reset counter.
            break;
        case WS_EVT_DATA: {
                AwsFrameInfo *info = (AwsFrameInfo*)arg;
                if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {   // Full message received.
                    WsQueueItem item;                                                                   // Struct - holds data & length.
                    item.len = (len < sizeof(item.data) - 1) ? len : sizeof(item.data) - 1;             // Bounds check.
                    memcpy(item.data, data, item.len);
                    item.data[item.len] = '\0';                                                         // For debug printing.
                    if (xQueueSend(wsRxQueue, &item, 0) != pdTRUE) {                                    // Non-blocking; drop if full.
                        Serial.println("wsRxQueue full, message dropped.");
                    }
                }
            }
            break;
        case WS_EVT_PONG:
        case WS_EVT_ERROR:
            // ws2812LedColor = RED;
            // ws2812LedBlink = false;
            break;
    }
}



/**
 * -------------------------------------------------------------------------
 *  <SparkFun_u-blox_GNSS_v3.h> DevUBLOXGNSS::processNMEA event handler (char incoming).
 * -------------------------------------------------------------------------
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
 * @since  3.0.8  [2025-11-21] New.
 * @since  3.0.9  [2025-12-02] Reworked.
 * @since  3.0.11 [2026-01-23-10:15am] Added startI2C(), DEBUG_NMEA_HEX.
 * @since  3.0.12 [2026-02-18-11:00pm] Shorten RTCM & NMEA status.
 * @since  3.2.1  [2026-07-28-04:45pm] Removed NMEA out switch & preference.
 * @since  3.2.1  [2026-07-30-10:30am] Global browserUpdatePending flag added.
 * @see    nmeaBuffer[] in Operation section of Global vars.
 * @link   https://docs.espressif.com/projects/arduino-esp32/en/latest/api/wifi.html.
 * @link   https://github.com/sparkfun/SparkFun_u-blox_GNSS_v3/tree/main/examples/Basics/Example2_NMEAParsing.
 */
void DevUBLOXGNSS::processNMEA(char incoming) {

    // --- Local vars. ---
    // nmeaBuffer[] is a global var.
    uint8_t writeStatus;                                                    // Return value from Wire.endTransmission().
    static  uint64_t nmeaSolutionLength        = 1;
    static  bool     nmeaSolutionBlockComplete = false;

    // --- Loop. ---
    if (inLoop) {
        strncat(nmeaBuffer, &incoming, 1);                                  // Add NMEA byte from RTK-SMA to outbound buffer.
        if ((incoming == '\n') && (nmeaBuffer[0] == '$')) {                 // We have a full sentence.
            // TODo: Here is where the NMEA sentence should get modified for instrument hieght and lock button.
            if (i2cUp) {                                                    // Slave is up.
                Wire1.beginTransmission(8);                                 // Prepare to send on I2C1.
                for (int i = 0; i < strlen(nmeaBuffer); i++) {              // Add bytes to output queue.
                    Wire1.write(nmeaBuffer[i]);
                }
                writeStatus = Wire1.endTransmission(8);                 // Send sentence on I2C1.
                if (writeStatus == 0) {                                     // Success: master (Wire1 on MCU #1) & slave (Wire on MCU #2) are both up.
                    nmeaCountAll++;                                         // Increment counter for all NMEA sentences sent.
                    if (strncmp(&nmeaBuffer[3], "GGA", 3) == 0) {           // We have a full GGA sentence.
                        lastGGAsendTime = esp_timer_get_time();             // Save time when GGA sentence was sent out.
                        nmeaCountGGA++;                                     // Increment counter for GGA sentences sent.
                        nmeaSolutionBlockComplete = true;                   // NMEA solution block is complete.
                    } else if (strncmp(&nmeaBuffer[3], "RMC", 3) == 0) {
                        nmeaCountRMC++;
                    } else if (strncmp(&nmeaBuffer[3], "GSA", 3) == 0) {
                        nmeaCountGSA++;
                    } else if (strncmp(&nmeaBuffer[3], "GSV", 3) == 0) {
                        nmeaCountGSV++;
                    } else if (strncmp(&nmeaBuffer[3], "GST", 3) == 0) {
                        nmeaCountGST++;
                    } else if (strncmp(&nmeaBuffer[3], "TXT", 3) == 0) {
                        nmeaCountTXT++;
                    } else {
                        nmeaCountOther++;
                        if (commandFlag[DEBUG_NMEA_COUNTS]) {
                            Serial.println(nmeaBuffer);
                        }
                    }
                    if (zeroStatusCounters) {                               // Zero all NMEA status counters.
                            nmeaCountAll       = 0;
                            nmeaCountGGA       = 0;
                            nmeaCountRMC       = 0;
                            nmeaCountGSA       = 0;
                            nmeaCountGSV       = 0;
                            nmeaCountGST       = 0;
                            nmeaCountTXT       = 0;
                            nmeaCountOther     = 0;
                            zeroStatusCounters = false;
                    }
                    if (commandFlag[DEBUG_NMEA_COUNTS]) {
                        Serial.printf("All=%u, GGA=%u, RMC=%u, GSA=%u, GSV=%u, GST=%u, TXT=%u, $other=%u.\n",
                        nmeaCountAll, nmeaCountGGA, nmeaCountRMC, nmeaCountGSA, nmeaCountGSV, nmeaCountGST, nmeaCountTXT, nmeaCountOther);
                    }
                    if (commandFlag[DEBUG_NMEA]) {                          // Debug - show NMEA sentence characters.
                        if (strncmp("$GNGGA", nmeaBuffer, 6) == 0) {
                            Serial.print('\n');
                        }
                        Serial.printf("%u %s", nmeaCountAll, nmeaBuffer);   // Display NMEA sentence (nmeaBuffer already ends with [CR][LF]).
                    }
                    if (commandFlag[DEBUG_NMEA_HEX]) {                      // Debug - show NMEA sentence characters in hex.
                        if (strncmp("$GNGGA", nmeaBuffer, 6) == 0) {
                            Serial.println('\n');
                        }
                        Serial.printf("%u %s", nmeaCountAll, nmeaBuffer);   // Display NMEA sentence (nmeaBuffer already ends with [CR][LF]).
                        for (int i = 0; i < strlen(nmeaBuffer); i++) {      // Display NMEA sentence characters in hex.
                            Serial.printf("[\"%c\" 0x%02X] ",nmeaBuffer[i], nmeaBuffer[i]);
                        }
                        Serial.println('\n');
                    }

                    // -- If on NMEA page, save sentence for processJsonActivity() call in next loop() & flag update. --
                    // NmeaBuffer gets memset (cleared), so it needs to be saved into lastNmea.
                    // This shifts the NMEA sentence's arrival at the browser by roughly one loop() pass (microseconds) which is negligable.
                    if (strcmp(whichPage, "nmea") == 0) {
                        strlcpy(lastNmea, nmeaBuffer, sizeof(lastNmea));
                        browserUpdatePending = true;
                    }

                    i2cUp = true;
                    NMEAout = true;                                         // NMEA sent out succesfully to MCU #2.

                    // -- Calculate NMEA status values for oper page. --
                    if (nmeaSolutionBlockComplete) {                        // For each solution block ...
                        nmeaRate = (nmeaSolutionLength * 1024) / (esp_timer_get_time() - lastGGAsendTime);          // Average kbps x 1000 per solution.
                        lastGGAsendTime = esp_timer_get_time();             // Save time when last GGA sent.
                        nmeaSolutionBlockComplete = false;                  // Start a new solution block.
                        nmeaSolutionLength = 0;                             // Reset counter for # of bytes in solution block.
                    }
                    nmeaSolutionLength += strlen(nmeaBuffer);               // Each NMEA sentence - add to total bytes for this solution block.
                } else {
                    i2cUp = false;                                          // Wire1 is down.
                    NMEAout = false;
                    ws2812LedColor = RED;
                    ws2812LedBlink = false;
                    startI2C();                                             // Restart Wire & Wire1.
                }
            }
            memset(nmeaBuffer, '\0', sizeof(nmeaBuffer));
        }
    }
}

/**
 * =========================================================================
 *  Loop functions.
 * =========================================================================
 * 
 * @since 3.0.11 [2026-01-12-06:00pm] Browser initiated updates.
 * @see checkZedTriggerUpdate()   - Check ZED to trigger DevUBLOXGNSS::processNMEA().
 * @see checkSerialUSB()          - Check serial USB for input.
 * @see debug()                   - Display debug.
 * @see checkGnssLockButton()     - Check GNSS lock button. // ToDo: Implement.
 * @see ws.cleanupClients()       - HTTP WebSocket cleanup.
 */

 /**
 * -------------------------------------------------------------------------
 *  Check ZED to trigger DevUBLOXGNSS::processNMEA().
 * -------------------------------------------------------------------------
 * 
 * Throttle roverGNSS.checkUblox() calls, which throttles DevUBLOXGNSS::processNMEA().
 * 
 * (prfGnsNavRat * prfGnsMsrInt) = interval (ms) to query ZED for PVT data.
 * 
 *
 * @return void No output is returned.
 * @since  3.0.12 [2026-02-08-05:00pm] New.
 * @since  3.0.12 [2026-02-14-06:15pm] Replace prfRqsPvtInt with (prfGnsNavRat * prfGnsMsrInt).
 * @since  3.2.1  [2026-07-30-11:15am] Moved jsonDocToBrowser.clear() to processJsonActivity().
 * @see    DevUBLOXGNSS::processNMEA().
 */
void checkZedTriggerUpdate() {

    // --- NMEA page. ---
    if (strcmp(whichPage, "nmea") == 0) {
        // -- Local vars. --
        const  int64_t THROTTLE_CHECK_ZED = (prfGnsNavRat * prfGnsMsrInt) * 1000;   // Convert from (us) to (ms), time between checkZedTriggerUpdate().
        static int64_t lastZedCheck = esp_timer_get_time();                         // Throttle. Initialize only once, then persist.
               int64_t lastTime;

        // -- Throttle loop() calls. --
        if ((esp_timer_get_time() - lastZedCheck) < THROTTLE_CHECK_ZED) {           // Not time to run.
            return; 
        }
        lastZedCheck = esp_timer_get_time();                                        // Time to run. Reset timer.

        // -- Check ZED. --
        roverGNSS.checkUblox();
        lastTime = esp_timer_get_time();
    }

    // --- Build data for operate page. ---
    if (strcmp(whichPage, "operate") == 0) {
        buildOperData();
    }
}

/**
 * -------------------------------------------------------------------------
 *  Process queued WS messages & pending status updates. All JSON activity lives here. 
 * -------------------------------------------------------------------------
 * 
 * Operation summary:
 *  1. Pull (xQueueReceive) JSON struct (data & length) from GhostRover FreeRTOS QueueHandle_t wsRxQueue.
 *     JSON struct was pushed (xQueueSend) into GhostRover FreeRTOS QueueHandle_t wsRxQueue by onWebSocketEvent().
 *  2. If data pulled from queue, deserialize into jsonDocFromBrowser.
 *  3. Clear jsonDocToBrowser & response.
 *  4. Save browser page name as global var.
 *  5. Set global vars. from jsonDocFromBrowser. Read/set preferences if on config page.
 *  6. Fill jsonDocToBrowser with simple response or data (depends on which browser page).
 *  7. If preferences changed, restart dependent processes (startWiFi, ...).
 *  8. sendDataToBrowser().
 *     8.1. Fill jsonDocToBrowser if browser page is constantly updated (operate, nmea, ...). 
 *     8.2  WebSocket send.
 *  9. If jsonDocFromBrowser["restartGR-MCU1"], restart ESP32.
 * 10. If browserUpdatePending flag is true, sendDataToBrowser() & flip flag.
 * 
 * jsonDocFromBrowser is ONLY touched by this function.
 * jsonDocToBrowser & response are ONLY touched by 1) this function and 2) sendDataToBrowser() (which is ONLY called by this function).
 * "which" browser page is a global var but ONLY set by this function.
 * jsonBuffer is ONLY touched by sendDataToBrowser() (which is ONLY called by this function). // ToDo: Move to local var?
 * 
 *  --- Notes. --- 
 *      1) NTRIP CASTER PREFERENCE is an embedded JSON string. Attributes for each NTRIP caster are sent/received (and stored in NVS) as a single JSON string.
 *      2) Numeric JSON keys are used to reduce JSON string length.
 * 
 * --- JSON key index. ---
 *     0  = Build info                      (buildString).
 *     1  = Units                           (char     prfUnt[6]).
 *     2  = RTCM in source                  (char     prfRtcIn[6]).
 *     3  = Not used.
 *     4  = GNSS measure interval           (uint16_t prfGnsMsrInt).
 *     5  = GNSS navigation rate            (uint8_t  prfGnsNavRat).
 *     6  = WiFi hot spot SSID              (char     prfHotSsi[20]).
 *     7  = WiFi hot spot password          (char     prfHotPas[30]).
 *     8  = GNSS fix                        (u_int8_t fixType).
 *     9  = GNSS satellites in view         (u_int8_t numSatInView).
 *     10 = GNSS ellipsoid height           (float    heightEllipsoid    -> operBuffer[24]).
 *     11 = GNSS orthometric height         (float    heightOrthometric  -> operBuffer[24]).
 *     12 = GNSS latitude                   (double   lat                -> operBuffer[24]).
 *     13 = GNSS longitude                  (double   lon                -> operBuffer[24]).
 *     14 = GNSS horizontal accuracy        (float    accuracyHorizontal -> operBuffer[24]).
 *     15 = GNSS vertical accuracy          (float    accuracyVertical   -> operBuffer[24]).
 *     16 = RTCM in status - up/down        (bool     RTCMin).
 *     17 = NMEA out status - up/down       (bool     NMEAout).
 *     18 = Battery State Of Charge (SOC)   (float    batterySoc         -> operBuffer[24]).
 *     19 = Battery change rate             (float    batteryChangeRate  -> operBuffer[24]).
 *     20 = Up time                         (char     uptime[20]).
 *     21 = RTCM in count all               Not used?
 *     22 = RTCM in rate                    Not used?
 *     23 = NMEA GGA out sentence count     (size_t   nmeaCountGGA).
 *     24 = NMEA RMC out sentence count     (size_t   nmeaCountRMC).
 *     25 = NMEA GSA out sentence count     (size_t   nmeaCountGSA).
 *     26 = NMEA GSV out sentence count     (size_t   nmeaCountGSV).
 *     27 = NMEA GST out sentence count     (size_t   nmeaCountGST).
 *     28 = NMEA TXT out sentence count     (size_t   nmeaCountTXT).
 *     29 = NMEA other out sentence count   (size_t   nmeaCountOther).
 *     30 = NMEA total out sentence count   (size_t   nmeaCountAll).
 *     31 = NMEA out rate                   (int64_t  nmeaRate).
 *     32 = Operational mode                (char     operMode[2]).
 *     33 = WiFi local network IP address   (char     localIp[16]).
 *     34 = WiFi hot spot address           Not used?
 *     35 = WebSocket client/session id     (uint8_t  clientId).
 *     36 = Instrument height               (uint16_t prfInstrHgt).
 *     37 = RTCM sentence count             (size_t   rtcmSentenceCount).
 *     38 = RTCM rate                       (float    rtcmKbps).
 *     39 = NTRIP caster #1 attributes      (char     prfNtripCastAttr[0][512]).
 *     40 = NTRIP caster #2 attributes      (char     prfNtripCastAttr[1][512]).
 *     41 = NTRIP caster #3 attributes      (char     prfNtripCastAttr[2][512]).
 *     42 = NTRIP caster active [1/2/3]     (char     prfNtripCastAct[2]).
 *     43 = NTRIP caster id                 (struct ntripCasterProfile caster[1/2/3].id      - uint8_t).
 *     44 = NTRIP caster name               (struct ntripCasterProfile caster[1/2/3].name    - char name[48]).
 *     45 = NTRIP caster url                (struct ntripCasterProfile caster[1/2/3].url     - char url[48]).
 *     46 = NTRIP caster mount point        (struct ntripCasterProfile caster[1/2/3].mount   - char mount[24]).
 *     47 = NTRIP caster port               (struct ntripCasterProfile caster[1/2/3].port    - uint16_t).
 *     48 = NTRIP caster version            (struct ntripCasterProfile caster[1/2/3].version - uint8_t).
 *     49 = NTRIP caster user               (struct ntripCasterProfile caster[1/2/3].user    - char user[48]).
 *     50 = NTRIP caster password           (struct ntripCasterProfile caster[1/2/3].pass    - char user[48]).
 *     51 = NTRIP caster sendGga            (struct ntripCasterProfile caster[1/2/3].sendGga - bool ).
 *
 *  --- Description of exchange protocol. ---
 *
 *  -- ALL PREFERENCES. --
 *       "0":"3.1.2 - Jul 15 2026 @ 17:20:35",
 *       "1":"meter",
 *       "2":"radio",
 *       "3":"on",
 *       "4":100,
 *       "5":2,
 *       "6":"ssid",
 *       "7":"pass",
 *       "35":22,
 *       "36":1596,
 *       "39":"{\"43\":\"1\",\"44\":\"PointPerfect (SparkPNT)\",\"45\":\"ppntrip.services.u-blox.com\",\"46\":\"NEAR-RTCM\",\"47\":\"2101\",\"48\":\"1\",\"49\":\"abcdefghijkl\",\"50\":\"abcdefghij\",\"51\":\"1\"}",
 *       "40":"{\"43\":\"2\",\"44\":\"name 2\",\"45\":\"\",\"46\":\"\",\"47\":\"\",\"48\":\"1\",\"49\":\"\",\"50\":\"\",\"51\":\"1\"}",
 *       "41":"{\"43\":\"3\",\"44\":\"name 3\",\"45\":\"\",\"46\":\"\",\"47\":\"\",\"48\":\"1\",\"49\":\"\",\"50\":\"\",\"51\":\"1\"}",
 *       "42":"1".
 *
 *  -- NTRIP CASTER PREFERENCE. --
 *       "setNtripCasterPref":"{\\"43\\":\\"1\\",\\"44\\":\\"name 1\\",\\"45\\":\\"x.com\\",\\"46\\":\\"ABC\\",\\"47\\":\\"2101\\",\\"48\\":\\"1\\",\\"49\\":\\"user1\\",\\"50\\":\\"pass1\\",\\"51\\":\\"1\\"}".
 *
 *  -- GNSS STATUS. --
 *       "8":1,
 *       "9":10,
 *       "10":"-40.68",
 *       "11":"-4.62",
 *       "12":"35.44418163",
 *       "13":"-76.92332881",
 *       "14":"8.464",
 *       "15":"10.229",
 *       "16":"d",
 *       "17":"u",
 *       "18":"101.30",
 *       "19":"2.5",
 *       "20":"4h 29m 52s",
 *       "30":526389,
 *       "31":160768,
 *       "23":77545,
 *       "24":77545,
 *       "25":129240,
 *       "26":216211,
 *       "27":25848,
 *       "28":0,
 *       "29":0,
 *       "32":"r",
 *       "33":"192.168.23.1",
 *       "34":"172.20.10.2",
 *       "35":30,
 *       "37":12,
 *       "38":89
 * 
 *   -- NMEA SENTENCE. --
 *     "NMEA":"$GLGSV,1,1,01,77,06,333,10,3*4F\r\n", etc.
 * 
 *  -- All pages. --
 *     - Hello. -
 *       browser (sends)    --> {"page:"menu/nmea/files/config/operate","sendPrefs":""}.
 *       browser (receives) <-- {"sendPrefsResp":"Preferences sent.",ALL PREFERENCES}.
 *
 *  -- Config page. --
 *     - Hello. -
 *
 *     - Set all preferences. -
 *       browser (sends)    --> {"page":"config","setPrefs":"",{ALL PREFERENCES}}.
 *       browser (receives) <-- {"setPrefsResp":"Preferences saved."}
 * 
 *     - Reset all preferences. -
 *       browser (sends)    --> {"page":"config","resetPrefs":""}.
 *       browser (receives) <-- {"prefsResetResp":"Preferences reset."}.
 *
 *     - Set NTRIP caster preference. -
 *       browser (sends)    --> {"page":"config",{NTRIP CASTER PREFERENCE}}.
 *       browser (receives) <-- {"setNtripCasterPrefResp":"Preference updated."}.
 *
 *  -- Files page. --
 *     - Hello. -
 *
 *     - List files. -
 *       browser (sends)    --> {"page":"files","listFiles":""}.
 *       browser (receives) <-- {"listFilesResp":"/index.html,/config.css,/config.html,/config.js,/upload-image-icon.png,/files.css,/files.html,
 *                              /files.js,/global.css,/global.js,/menu.css,/menu.html,/menu.js,/operate.css,/operate.js,/junk.txt,/operate.html,"}.
 *     - Delete files. -
 *       browser (sends)    --> {"deleteFile":"filename"}.
 *       browser (receives) <-- {"deleteFileResp":"File deleted./File NOT deleted"}.
 * 
 *     - Upload/view(download) files. -
 *       No websockets. Uses HTTP post from fetch() API in files.js. @ see startHttpServer().
 *
 *  -- Menu page. --
 *     - Hello. -
 *
 *     - Restart GRMCU-1. -
 *       browser (sends)    --> {"page":"menu","restartGR-MCU1":""}.
 *       browser (receives) <-- {"restartGR-MCU1Resp":"GR-MCU1 will restart."}.
 *
 *  -- Operate page. --
 *     - Hello. -
 * 
 *     - GNSS & status values. -
 *       browser (sends)    --> {"page:"operate","sendPrefs":""}.
 *       browser (receives) <-- {"sendPrefsResp":"Preferences sent.",ALL PREFERENCES,GNSS STATUS}.
 *       browser (receives) <-- {GNSS STATUS}. Continues in loop() until page is left.
 *
 *     - Laser on/off button. --
 *       browser (sends)    --> {"page":"operate",{"laserOn:""}.
 *       browser (receives) <-- {"laserOnResp":"Laser on."}.
 *       browser (sends)    --> {"page":"operate",{"laserOff:""}.
 *       browser (receives) <-- {"laserOffResp":"Laser off."}.
 *
 *     - Height lock/unlock button. --
 *       browser (sends)    --> {"page":"operate",{"heightLock:""}.
 *       browser (receives) <-- {"heightLockResp":"Height locked"}.
 *       browser (sends)    --> {"page":"operate",{"heightUnlock:""}.
 *       browser (receives) <-- {"heightUnlockResp":"Height unlocked."}.
 *
 *     - Position lock/unlock button. --
 *       browser (sends)    --> {"page":"operate",{"positionLock:""}.
 *       browser (receives) <-- {"positionLockResp":"Position locked"}.
 *       browser (sends)    --> {"page":"operate",{"positionUnlock:""}.
 *       browser (receives) <-- {"positionUnlockResp":"Position unlocked."}.
 *
 *  -- NMEA page. --
 *     - Hello. -
 *
 *     - NMEA sentences. - Triggered by "Hello" message exchange.
 *       browser (sends)    --> {"page:"operate","sendPrefs":""}.
 *       browser (receives) <-- {"sendPrefsResp":"Preferences sent.",ALL PREFERENCES}.
 *       browser (receives) <-- {NMEA SENTENCE}. Continues in loop() until page is left.
 *
 *  -- Test. --
 *     - Echo. -
 *       browser (sends)    --> {"page":"TBD","echo":"some text"}.
 *       browser (receives) <-- {"echo":"some text","echo":"Message echoed."}.
 *
 * @return void  No output is returned.
 * @since 3.0.7  [2025-11-10-12:00pm].
 * @since 3.0.10 [2026-01-07-02:30pm] Change {"opr":"ready"} to {"opr":"?"}.
 * @since 3.0.10 [2026-01-08-09:30am] Shortened keywords (e.g. latitude to lat).
 * @since 3.0.11 [2026-01-08-10:30am] Browser initiated updates.
 * @since 3.0.11 [2026-01-22-02:45pm] Add laser logic.
 * @since 3.0.12 [2026-02-06-06:15pm] Add preferences.
 * @since 3.0.12 [2026-02-07-07:30am] Check for {"page":"opr/cfg/menu/nmea"}.
 * @since 3.0.12 [2026-02-19-04:00pm] Removed leaving message.
 * @since 3.1.2  [2026-07-20-11:00am] Change jsonObjFromBrowser kv pair branching from "if" to "else if."
 * @since 3.2.1  [2026-07-24-03:30pm] Refactor JSON.
 * @since 3.2.1  [2026-07-25-05:00pm] Convert NTRIP keys from alpha to numeric.
 * @since 3.2.1  [2026-07-30-10:45am] Implement FreeRTOS queues: refactor onWebSocketMessage() into processJsonActivity().
 *                Fix cross-task race on shared JsonDocuments causing intermittent LoadProhibited/heap-corruption crashes.
 * @since 3.2.1  [2026-07-30-11:45am] Set page name global var. 
 * @see   Global vars: GNSS, prefUtility(), onWebSocketEvent(), startWebSocketServer().
 * @link  https://randomnerdtutorials.com/esp32-websocket-server-arduino/.
 * @link  https://randomnerdtutorials.com/esp32-websocket-server-sensor/.
 * @link  https://shawnhymel.com/1882/how-to-create-a-web-server-with-websockets-using-an-esp32-in-arduino/.
 * @link  https://arduinojson.org/v6/api/json/deserializejson/.
 * @link  https://arduinojson.org/v6/doc/deserialization/.
 * @link  https://arduinojson.org/v7/api/jsonvariant/.
 * @link  https://github.com/espressif/arduino-esp32/blob/master/libraries/SD/examples/SD_Test/SD_Test.ino.
 * @link  https://docs.espressif.com/projects/arduino-esp32/en/latest/tutorials/preferences.html.
 * @link  https://github.com/espressif/arduino-esp32/tree/master/libraries/Preferences/.
 *
 */
 void processJsonActivity() {

    // --- Debug. ---
    // serializeJson(jsonDocToBrowser, Serial); // Debug.
    // Serial.println();

    // --- Local vars. ---
    // jsonDocFromBrowser, jsonDocToBrowser, &  JsonDocNtrip are global vars.
    WsQueueItem item;

    // --- Step 1/2: Process one incoming WebSocket message, if queued. ---
    if (xQueueReceive(wsRxQueue, &item, 0) == pdTRUE) {

        // -- Debug. Print data received. --
        if (commandFlag[DEBUG_WS]) {
            Serial.printf("WS #%u: browser --> %s\n", clientId, item.data);
        }

        // -- WebSocket message - deserialize the JSON data into a JSON document (jsonDocFromBrowser). --
        jsonDocFromBrowser.clear();
        DeserializationError error = deserializeJson(jsonDocFromBrowser, item.data, item.len);

        // -- Begin. --
        if (error) {
            Serial.printf("JSON deserialize failed: %s\n", error.f_str());
            return;
        } else {

            // -- Process JSON. --
            memset(response, '\0', sizeof(response));
            jsonDocToBrowser.clear();

            // -- Set page name global var.
            if (jsonDocFromBrowser["page"].is<JsonVariant>()) {                     // Does key exist?
                strlcpy(whichPage, jsonDocFromBrowser["page"], sizeof(whichPage));  // Important global used in loop().
            }

            // -------------------------------------------------------------------------
            // -- All pages. Send all preferences to browser. --
            // -------------------------------------------------------------------------

            if (jsonDocFromBrowser["sendPrefs"].is<JsonVariant>()) {

                // - Set global vars from preferences. -
                prefUtility(PREF_READ);
                
                // - Set JSON values from global vars. -
                jsonDocToBrowser["0"]  = buildString;
                jsonDocToBrowser["1"]  = prfUnt;
                jsonDocToBrowser["2"]  = prfRtcIn;
                jsonDocToBrowser["4"]  = prfGnsMsrInt;
                jsonDocToBrowser["5"]  = prfGnsNavRat;
                jsonDocToBrowser["6"]  = prfHotSsi;
                jsonDocToBrowser["7"]  = prfHotPas;
                jsonDocToBrowser["35"] = clientId;
                jsonDocToBrowser["36"] = prfInstrHgt;
                jsonDocToBrowser["39"] = prfNtripCastAttr[0];
                jsonDocToBrowser["40"] = prfNtripCastAttr[1];
                jsonDocToBrowser["41"] = prfNtripCastAttr[2];
                jsonDocToBrowser["42"] = prfNtripCastAct;

                // - Set response. -
                strcpy(response, "Preferences sent.");
                jsonDocToBrowser["sendPrefsResp"] = response;
            }

            // -------------------------------------------------------------------------
            // -- Config page. Set all preferences. --
            // -------------------------------------------------------------------------
            if (jsonDocFromBrowser["setPrefs"].is<JsonVariant>()) {

                // - Set global vars from JSON values. -
                strlcpy(prfUnt,          jsonDocFromBrowser["1"],  sizeof(prfUnt));  // dst, src, sizeof(dest)
                strlcpy(prfRtcIn,        jsonDocFromBrowser["2"],  sizeof(prfRtcIn));
                strlcpy(prfHotSsi,       jsonDocFromBrowser["6"],  sizeof(prfHotSsi));
                strlcpy(prfHotPas,       jsonDocFromBrowser["7"],  sizeof(prfHotPas));
                strlcpy(prfNtripCastAct, jsonDocFromBrowser["42"], sizeof(prfNtripCastAct));
                prfGnsNavRat    = (uint8_t)  atoi(jsonDocFromBrowser["5"]);   // KV values are stored in NVS as int, but set to C-string in processJsonActivity() for code clarity.
                prfGnsMsrInt    = (uint16_t) atoi(jsonDocFromBrowser["4"]);
                prfInstrHgt     = (uint16_t) atoi(jsonDocFromBrowser["36"]);

                // - Set new preferences from global vars. -
                prefUtility(PREF_SET);

                // - Set response. -
                strcpy(response, "Preferences saved.");
                jsonDocToBrowser["setPrefsResp"] = response;
            }

            // -------------------------------------------------------------------------
            // -- Config page. Reset all preferences to defaults. --
            // -------------------------------------------------------------------------
            if (jsonDocFromBrowser["resetPrefs"].is<JsonVariant>()) {

                // - Set global vars to defaults. -
                prefUtility(PREF_RESET);

                // - Set response. -
                strcpy(response, "Preferences reset.");
                jsonDocToBrowser["resetPrefsResp"] = response;
            }

            // -------------------------------------------------------------------------
            // -- Config page. Set NTRIP preference. --
            // -------------------------------------------------------------------------
            if (jsonDocFromBrowser["setNtripCasterPref"].is<JsonVariant>()) {

                // - Create NTRIP caster JSON doc from embedded JSON string. -
                // Embedded JSON string allows attributes for an NTRIP caster to be stored as a single preference. 
                JsonDocNtrip.clear();
                DeserializationError ntripError = deserializeJson(JsonDocNtrip, jsonDocFromBrowser["setNtripCasterPref"]);
                if (ntripError) {
                    Serial.printf("JSON deserialize failed: %s\n", error.f_str());
                    return;
                }

                // - Set global vars to JSON values from browser. -
                strlcpy(ntripCaster.name,  JsonDocNtrip["44"],  sizeof(ntripCaster.name));
                strlcpy(ntripCaster.url,   JsonDocNtrip["45"],   sizeof(ntripCaster.url));
                strlcpy(ntripCaster.mount, JsonDocNtrip["46"], sizeof(ntripCaster.mount));
                strlcpy(ntripCaster.user,  JsonDocNtrip["49"],  sizeof(ntripCaster.user));
                strlcpy(ntripCaster.pass,  JsonDocNtrip["50"],  sizeof(ntripCaster.pass));
                ntripCaster.id      = atoi(JsonDocNtrip["43"]);
                ntripCaster.port    = atoi(JsonDocNtrip["47"]);
                ntripCaster.version = atoi(JsonDocNtrip["48"]);
                ntripCaster.sendGga = JsonDocNtrip["51"].as<bool>();

                // Set set global var for JSON NTRIP caster (e.g. prfNtripCastAttr[1]).
                strlcpy(prfNtripCastAttr[ntripCaster.id-1], jsonDocFromBrowser["setNtripCasterPref"], NTRIP_CAST_ATTR_LEN);

                // - Set new NTRIP preference. -
                prefUtility(PREF_SET_NTRIP);

                // - Set response. -
                strcpy(response, "Preference updated.");
                jsonDocToBrowser["setNtripCasterPrefResp"] = response;
            }

            // -------------------------------------------------------------------------
            // -- Files page. List files. --
            // -------------------------------------------------------------------------
            if (jsonDocFromBrowser["listFiles"].is<JsonVariant>()) {

                // - Set JSON value: list of files. -
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
                jsonDocToBrowser["fileList"] = output;

                // - Set response. -
                strcpy(response, "Files listed.");
                jsonDocToBrowser["listFilesResp"] = response;
            }

            // -------------------------------------------------------------------------
            // -- Files page. Delete file. --
            // -------------------------------------------------------------------------
            if (jsonDocFromBrowser["deleteFile"].is<JsonVariant>()) {

                // - Delete file. -
                const char* fileName = jsonDocFromBrowser["deleteFile"];
                strcpy(response, SD.remove(fileName) ? "File deleted." : "File NOT deleted.");

                // - Set response. -
                jsonDocToBrowser["deleteFileResp"] = response;
            }

            // -------------------------------------------------------------------------
            // -- Menu page. Restart GRMCU-1. --
            // -------------------------------------------------------------------------
            if (jsonDocFromBrowser["restartGR-MCU1"].is<JsonVariant>()) {

                // - Set response. -
                strcpy(response, "GR-MCU1 will restart.");
                jsonDocToBrowser["restartGR-MCU1Resp"] = response;
                Serial.println(response);
            }

            // -------------------------------------------------------------------------
            // -- NMEA page. NMEA sentences. --
            // -------------------------------------------------------------------------
            // loop() -> checkZedTriggerUpdate() -> DevUBLOXGNSS::processNMEA() sets browserUpdatePending = true; -> sendDataToBrowser().

            // -------------------------------------------------------------------------
            // -- Operate page. GNSS data. --
            // -------------------------------------------------------------------------
            // loop() -> checkZedTriggerUpdate() -> buildOperData() sets browserUpdatePending = true; -> sendDataToBrowser().

            // -------------------------------------------------------------------------
            // -- Operate page. Laser on/off button. --
            // -------------------------------------------------------------------------
            //   @link https://www.build-electronic-circuits.com/arduino-laser-module-ky-008/.
            //   @link https://docs.sparkfun.com/SparkFun_Thing_Plus_ESP32-S3/arduino_example/#rgb-led.
            if (jsonDocFromBrowser["laserOn"].is<JsonVariant>()) {
                digitalWrite(LSR_TRIGGER, HIGH);        // Turn laser on.

                // - Set response. -
                strcpy(response, "Laser on.");
                jsonDocToBrowser["laserOnResp"] = response;
                Serial.println(response);
            }
            if (jsonDocFromBrowser["laserOff"].is<JsonVariant>()) {
                digitalWrite(LSR_TRIGGER, LOW);         // Turn laser off.

                // - Set response. -
                strcpy(response, "Laser off.");
                jsonDocToBrowser["laserOffResp"] = response;
                Serial.println(response);
            }

            // -------------------------------------------------------------------------
            // -- Operate page. Height lock/unlock button. --
            // -------------------------------------------------------------------------
            if (jsonDocFromBrowser["heightLock"].is<JsonVariant>()) {
                // ToDo: Implement.

                // - Set response. -
                strcpy(response, "Height locked.");
                jsonDocToBrowser["heightLockResp"] = response;
                Serial.println(response);
            }
            if (jsonDocFromBrowser["heightUnlock"].is<JsonVariant>()) {
                // ToDo: Implement.

                // - Set response. -
                strcpy(response, "Height unlocked.");
                jsonDocToBrowser["heightUnlockResp"] = response;
                Serial.println(response);
            }

            // -------------------------------------------------------------------------
            // -- Operate page. Position lock/unlock button. --
            // -------------------------------------------------------------------------
            if (jsonDocFromBrowser["positionLock"].is<JsonVariant>()) {
                // ToDo: Implement.

                // - Set response. -
                strcpy(response, "Position locked.");
                jsonDocToBrowser["positionLockResp"] = response;
                Serial.println(response);
            }
            if (jsonDocFromBrowser["positionUnlock"].is<JsonVariant>()) {
                // ToDo: Implement.

                // - Set response. -
                strcpy(response, "Position unlocked.");
                jsonDocToBrowser["positionUnlockResp"] = response;
                Serial.println(response);
            }

            // -------------------------------------------------------------------------
            // -- Test. Echo. --
            // -------------------------------------------------------------------------
            if (jsonDocFromBrowser["echo"].is<JsonVariant>()) {

                // - Set JSON value. -
                jsonDocToBrowser["echo"] = jsonDocFromBrowser["echo"];

                // - Set response. -
                strcpy(response, "Message echoed.");
                jsonDocToBrowser["echoResp"] = response;
            }

            // -- Wrap up JSON processing. --
            if (inLoop) {
                if ((jsonDocFromBrowser["setPrefs"].is<JsonVariant>()) ||
                    (jsonDocFromBrowser["resetPrefs"].is<JsonVariant>()) ||
                    (jsonDocFromBrowser["setNtripCasterPref"].is<JsonVariant>())) {

                        // -- Rerun dependent functions. --
                        if (!jsonDocFromBrowser["setPrefs"].is<JsonVariant>()) {
                            prefUtility(PREF_SET);
                        }
                        Serial.println("Restarting dependent processes.");
                        startAndConfigGNSS();                                       // Uses prfGnsNavRat, prfGnsMsrInt.
                        if (strcmp(prfRtcIn, "ntrip") == 0) {
                            startWiFi();                                            // NTRIP uses prfHotSsi & prfHotPas.
                            // ToDo: // Implement, if NTRIP switch is on, start/restart NTRIP client
                        } 
                }
            }
        }

        // -- Send data to browser. --
        sendDataToBrowser();

        // -- Wrap up. --
        if (jsonDocFromBrowser["restartGR-MCU1"].is<JsonVariant>()) {
            esp_restart();  // Restart.
        }
    }

    // -- Step 2/2: Send periodic update to browser page (operate, nmea, ...) if pending. --
    if (browserUpdatePending) {
        memset(response, '\0', sizeof(response));
        jsonDocToBrowser.clear();       // Ensure a clean JSON doc for all browser pages (operate, nmea, ...).
        sendDataToBrowser();
        browserUpdatePending = false;
    }
}

/**
 * -------------------------------------------------------------------------
 *  Check serial USB for input.
 * -------------------------------------------------------------------------
 *
 * @return void No output is returned.
 * @since  3.0.9  [2025-12-17-06:00pm] New.
 * @since  3.0.10 [2025-12-30-01:15pm] Refactor.
 * @since  3.0.10 [2026-01-06-11:15am] Add LiPo.
 * @since  3.0.11 [2026-01-12-02:00pm] Refactor.
 * @since  3.0.11 [2026-01-16-08:40pm] if (Serial.available() == 0).
 * @since  3.0.12 [2026-02-15-05:00pm] Add "z" zero status counters.
 * @see    loop().
 */
void checkSerialUSB() {

    if (Serial.available() == 0) {                              // Nothing to see, move on.
        return;
    }

    // --- Local vars. ---
    static size_t posn        = 0;                              // Input position for command buffer.
    static char   command[20] = {'\0'};                         // Serial USB command buffer.
    static char   inputChar   = '\0';

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
            Serial.print("\nGR-MCU1:\n\"?\" Print commands.\n\"!\" Disable all debug.\n\"z\" Zero status counters.\nCommands:");
            for (size_t i = 0; i <= NUM_COMMANDS-1; i++) {
                Serial.printf(" %s", COMMAND[i]);
            }
            Serial.println('.');
        } else if ((command[0]) == '!') {                       // Disable all debugs.
            for (size_t i = 0; i <= NUM_COMMANDS; i++) {
                commandFlag[i] = false;
            }
            Serial.println("All debug disabled.");
        } else if ((command[0]) == 'z') {                       // Zero all status counters.
            Serial.println("Zero all counters.");
            zeroStatusCounters = true;
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
 * --------------------------------------------------------------------------------------------------
 *  Check GNSS lock button (upPosition or downPosition).
 * ---------------------------------------------------------------------------------------------------------------------------

 *
 * // ToDo: Implment.
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

    // --- Set state of GNSS lock button. ---
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
 * -------------------------------------------------------------------------
 *  Display debug.
 * -------------------------------------------------------------------------
 *
 * @return void No output is returned.
 * @since  0.3.3  [2025-05-02-12:00pm] New.
 * @since  0.3.7  [2025-05-09-04:30pm] Add loop() throttle.
 * @since  0.5.1  [2025-06-07-03:45pm] Removed gotbits.
 * @since  0.6.1  [2025-07-13-08:00am] Added debugNMEA.
 * @since  3.0.11 [2026-01-12-03:30pm] Refactor.
 * @since  3.0.11 [2026-01-12-10:00pm] Added checkWire1.
 * @since  3.0.11 [2026-01-15-10:45am] Moved THROTTLE_DEBUG from global to local var.
 * @since  3.0.11 [2026-01-22-02:00pm] Add DEBUG_TEMP.
 * @since  3.1.1  [2026-06-25-04:00pm] Change DEBUG_SER output.
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
        Serial.println("\nHC-12 command mode enabled (! to exit)"); // Display jsonObjFromBrowsers.
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

    // --- RTCM in. ---
    // @see task taskRtcmRelay().

    // --- GNSS. ---
    if (commandFlag[DEBUG_GNSS]) {
        roverGNSS.enableDebugging();    // "Pipe all NMEA sentences to serial USB."
    }
    if (!commandFlag[DEBUG_GNSS]) {
        roverGNSS.disableDebugging();
    }

    // --- NMEA out (sentences). ---
    // @see "if (commandFlag[DEBUG_NMEA])" in DevUBLOXGNSS::processNMEA() event handler.

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
            "Serial USB %c  Serial0 (not used) %c serial1 (RTCMin -> HC12) %c  serial2 (RTCMout -> ZED UART2) %c\n",
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
    if (commandFlag[RESTART]) {
        Serial.println("Restarting ...");
        esp_restart();
    }

    // --- Wire1. ---
    if (commandFlag[CHECK_WIRE1]) {
        Wire1.beginTransmission(8);                             // Test Wire1. Receiver is device #8.
        uint8_t status = Wire1.endTransmission(8);              // Test Wire1. Is device up?
        Serial.print("Wire 1 is ");
        if (status != 0) {
            Serial.printf("down. Error = %i. \n", status);
            i2cUp = false;                                      // Slave is down.
            startI2C();                                         // Restart Wire1.
        } else {                                                // 0 = success (slave ACKed).
            i2cUp = true;                                       // Slave is up, successful write.
            Serial.println("up.");
        }
    }
    // --- Temporary items. ---
    // memset(debugTemp, '\0', sizeof(debugTemp));
    // strcpy(debugTemp,numberbuffer);
    if (commandFlag[DEBUG_TEMP]) {
        Serial.printf("[%s]\n", debugTemp);
    }

    // --- NMEA (hex), NMEA (counts). ---
    // @see "if (commandFlag[DEBUG_NMEA_HEX])" in DevUBLOXGNSS::processNMEA() event handler.

    // --- Preferences. ---
    if (commandFlag[DEBUG_PREFS]) {
        prefUtility(PREF_PRINT);
        Serial.println();
    }
}

/**
 * =========================================================================
 *  Setup.
 * =========================================================================
 *
 * @since  3.0.3 [2025-10-13-01:00pm] New.
 * @since  3.0.8 [2025-11-21-12:15pm] Removed startWiFiClient(), added startWiFiSoftAP().
 * @see    Global vars.
 */
void setup() {
    showBuild();                // Display build & processor info.
    prefUtility(PREF_INIT);     // Get preferences.
    startSerial();              // Start serial interfaces.
    initPins();                 // Initialize pin modes & pin values.
    startI2C();                 // Start I2C wire interfaces.
    startLiPo();                // Start LiPo I2C interface.
    startWiFi();                // Start WiFi.
    startSD();                  // Start & test microSD card reader.
    startHttpServer();          // Start HTTP server.
    startWebSocketServer();     // Start WebSocket server.
    startAndConfigGNSS();       // Start GNSS, config ZED settings.
    startQueues();              // Start GhostRover FreeRTOS queues.
    startTasks();               // Start GhostRover FreeRTOS tasks.
    preLoop();                  // Prepare for loop().
}

/**
 * =========================================================================
 *  Loop.
 * =========================================================================
 * 
 * @since 3.0.10 [2025-12-27-08:00pm] New.
 * @see   startTasks().
 * @see   GhostRover FreeRTOS functions.
 * @see   Event handlers.
 */
void loop() {
    checkZedTriggerUpdate();    // Check ZED to trigger DevUBLOXGNSS::processNMEA().
    processJsonActivity();      // Process queued WS messages & pending status updates. All JSON activity lives here.
    checkSerialUSB();           // Check serial USB for input.
    // checkGnssLockButton();   // Check GNSS lock button.  // ToDo: Implement.
    ws.cleanupClients();        // HTTP WebSocket cleanup.
    debug();                    // Display debug.
}

