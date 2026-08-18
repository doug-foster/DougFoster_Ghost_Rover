/**
 * *************************************************************************
 *  Ghost Rover 3 - "Invisible" GNSS rover
 * *************************************************************************
 *
 * @author D. Foster <doug@dougfoster.me>.
 * @since  3.1.0 [2026-03-02-05:00pm] Stable 3.0 version.
 * @since  3.1.0 [2026-03-10-11:30am] Add pole height preference.
 * @since  3.1.1 [2026-06-25-10:30pm] Regroup. Cleanup.
 * @since  3.1.1 [2026-06-26-12:30pm] Cleanup formatting.
 * @since  3.1.1 [2026-06-26-06:00pm] Change reset to restart.
 * @since  3.1.1 [2026-06-26-06:00pm] Change checkZED to checkZedTriggerUpdate.
 * @since  3.1.1 [2026-07-03-10:30am] General cleanup.
 * @since  3.1.2 [2026-07-03-06:15pm] New, GhostRover FreeRTOS task taskRtcmRelay() replaced relaySerial1toSerial2() in loop().
 * @since  3.1.2 [2026-07-03-07:30pm] Address rtcmSentence buffer overflow.
 * @since  3.1.2 [2026-07-15-04:45pm] Add NTRIP preferences.
 * @since  3.1.2 [2026-07-16-09:00am] Changed int16_t prfInstrHgt to uint16_t.
 * @since  3.1.2 [2026-07-18-03:00pm] NTRIP.
 * @since  3.2.1 [2026-07-24-03:30pm] Refactor JSON.
 * @since  3.2.1 [2026-07-25-11:00am] Removed wsKey().
 * @since  3.2.1 [2026-07-25-05:00pm] Convert NTRIP keys from alpha to numeric.
 * @since  3.2.1 [2026-07-27-01:45pm] Add sendDataToBrowser(), refactor to consolidate JSON.
 * @since  3.2.1 [2026-07-28-04:45pm] Removed NMEA out switch & preference.
 * @since  3.2.1 [2026-07-31-09:30am] Add WiFi client for NTRIP access.
 * @since  3.2.1 [2026-08-02-04:30pm] Remove connectWiFiclient().
 * @since  3.2.1 [2026-08-03-10:00am] Removed jsonObj["21'] & jsonObj["21'].
 * @since  3.2.2 [2026-08-09-11:45am] Add NTRIP client.
 * @since  3.2.2 [2026-08-09-02:00pm] Updated platform from 3.3.10 to 3.3.11. Updated AsyncTCP & ESPAsyncWebServer libraries.
 * @since  3.2.2 [2026-08-09-05:30pm] Only print for DEBUG_WS.
 * @since  3.2.3 [2026-08-10-09:45am] Add TCP to replace BLE.
 * @since  3.2.3 [2026-08-10-10:15pm] Refactor rtcm3GetMessageType(), relayRtcmByte(), & taskRtcmRelay() to properly handle RTCM sentences.
 * @since  3.3.0 [2026-08-13-12:00pm] Replaced checkZedTriggerUpdate() with checkTimers().
 * @since  3.3.0 [2026-08-13-12:30pm] Replaced ntripPushGGA() with checkTimers().
 * @since  3.3.0 [2026-08-13-01:00pm] Replaced debug timer with checkTimers().
 * @since  3.3.0 [2026-08-17-09:15am] Refactored card test in startOutputs() to use log file. Changed position in setup().
 * @since  3.3.1 [2026-08-16-12:00pm] Changed from SD (SPI) to SD_MMC (SDIO).
 * @since  3.3.1 [2026-08-16-05:30pm] Add logPrint().
 * @see    https://github.com/doug-foster/DougFoster_Ghost_Rover.
 * @see    https://github.com/doug-foster/DougFoster_Ghost_Rover_BT_relay.
 * @see    https://github.com/doug-foster/DougFoster_Ghost_Rover_EVK_RTCM_relay.
 * @link   http://dougfoster.me.
 */

 // ToDo: Remove i2cUp & Wire1 since being replaced by TCP.

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
 *     -- GR-MCU board       https://www.sparkfun.com/sparkfun-thing-plus-esp32-s3.html (SparkFun Thing Plus - ESP32-S3).
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
 *     -- IDE            VS Code & Arduino Maker Workshop 1.1.9 extension (uses Arduino CLI 1.2.0).
 *                       https://marketplace.visualstudio.com/items?itemName=TheLastOutpostWorkshop.arduino-maker-workshop.
 *     -- Platform       https://github.com/espressif/arduino-esp32/releases/latest (Arduino Release v3.3.11 based on ESP-IDF v5.5.5).
 *     -- Release notes  https://github.com/espressif/arduino-esp32/releases.
 * 
 * --- Caveats. ---
 *     -- SoftwareSerial library is not supported on ESP32-S3 (does work on ESP32-C6).
 *     -- 0.5.1 -> 0.6.1 builds: Moved BLE relay from primary MCU to secondary MCU since BleSerial library is a space pig.
 *
 * --- TODO: ---
 *     - Replace BLE with TCP for NTRIP.
 *     - Add NTRIP bridge mode.
 *     - Add RTCM page.
 *     - Offset height/NMEA by instrument height.
 *     - Button lock (laser/height/position).
 *     - Update RTKEverywhere for base station.
 *     - Verify RTK-FIX. Check serial2 tx data.
 *     - Operate.js/operate.html page - add ability to select coordinates (lat/lon, ECEF, UTM northing & easting)  
 */

/**
 * -------------------------------------------------------------------------
 *  Code structure.
 * -------------------------------------------------------------------------
 * 
 * @since 3.1.1 [2026-06-25-01:00pm] New.
 * @since 3.1.2 [2026-07-03-06:15pm] New, GhostRover FreeRTOS task taskRtcmRelay() replaced relaySerial1toSerial2() in loop().
 * @since 3.2.1 [2026-07-30-07:45am] Implement GhostRover FreeRTOS queues: refactor onWebSocketMessage() into processJsonActivity().
 * @since 3.2.2 [2026-08-09-11:45am] Add NTRIP client: add relayRtcmByte(), add ntripBeginClient(), & ntripPushGGA().
 * @since 3.2.3 [2026-08-10-09:45am] Add startTcpServer(), add checkTcpClient().
 * @since 3.3.1 [2026-08-17-09:15am] Changed position of startOutputs() in setup().
 * @since 3.3.1 [2026-08-16-05:30pm] Add logPrint().
 *
 *  --- Docs. ---
 *
 *  --- Include libraries. ---
 *      -- Core.
 *      -- Additional.
 *
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
 *
 *  --- General functions. ---
 *      -- statusLedOn()               - Turn on status LED.
 *      -- prefUtility()               - Preference utility.
 *      -- buildOperData()             - Build data for operate page.
 *      -- sendDataToBrowser()         - Send data to browser.
 *      -- rtcm3GetMessageType()       - Return RTCM3 message type to taskRtcmRelay().
 *      -- relayRtcmByte()             - Relay RTCM byte to Serial2 (ZED UART2), tracking stats.
 *      -- ntripBeginClient()          - Connect to NTRIP caster & validate credentials.
 *      -- logPrint                    - Save a message to log.txt, print to Serial if available.
 *
 *  --- Setup functions. ---
 *      -- startOutputs()              - Start serial & microSD card reader. 
 *      -- buildInfo()                 - Build & processor info.
 *      -- startSerial()               - Start serial interfaces.
 *      -- initPins()                  - Initialize pins & pin values.
 *      -- startI2C()                  - Start I2C wire interfaces.
 *      -- startLiPo()                 - Start LiPo I2C interface.
 *      -- startWiFiServer()           - Start WiFi server.
 *      -- startTcpServer()            - Start TCP server for GNSS Master (NMEA out / RTCM in bridge).
 *      -- startHttpServer()           - Start HTTP server.
 *      -- startWebSocketServer()      - Start WebSocket server.
 *      -- startAndConfigGNSS()        - Start GNSS, config ZED settings.
 *      -- startQueues()               - Start GhostRover FreeRTOS queues.
 *      -- startTasks()                - Start GhostRover FreeRTOS tasks.
 *      -- preLoop()                   - Prepare for loop().
 *
 *  --- GhostRover FreeRTOS functions. ---
 *      -- taskLoopStatusLed()         - GhostRover FreeRTOS task - Set Loop() status LED to blink or solid.
 *      -- taskRtcmRelay()             - GhostRover FreeRTOS task - Relay RTCM from Serial1 (HC-12) to -> Serial2 (ZED UART2).
 *
 *  --- Event handlers for core/additional library processes. ---
 *      -- onWiFiEvent()               - <WiFi.h> & <WiFiAP.h> WiFi event handler (WiFiEvent_t).
 *      -- onHttpFileUpload()          - <ESPAsyncWebServer.h> HTTP endpoint ("/upload") event handler (AsyncWebServerRequest).
 *      -- onWebSocketEvent()          - <ESPAsyncWebServer.h> WebSocket event handler (AsyncWebSocket).
 *      -- DevUBLOXGNSS::processNMEA() - <SparkFun_u-blox_GNSS_v3.h> DevUBLOXGNSS::processNMEA event handler (char incoming).
 *
 *  --- Loop functions. ---
 *      -- checkTimers()               - Check all process timers.
 *      -- processJsonActivity()       - Process queued WS messages & pending status updates. All JSON activity lives here.
 *      -- checkSerialUSB()            - Check serial USB for input.
 *      -- // checkGnssLockButton()    - Check GNSS lock button (upPosition or downPosition). // ToDo: Implement.
 *      -- checkTcpClient()            - Check TCP server for new/dropped client (GNSS Master, ..).
 *      -- debug()                     - Display debug.
 *
 *  --- Setup. ---
 *
 *  --- Loop. ---
 */

/**
 * -------------------------------------------------------------------------
 *  Code Operation.
 * -------------------------------------------------------------------------
 *
 * @since 3.1.1 [2026-06-25-01:00pm] New.
 * @since 3.2.1 [2026-07-30-07:45am] Implement FreeRTOS queues: refactor onWebSocketMessage() into processJsonActivity().
 * @since 3.2.3 [2026-08-10-09:45am] Add startTcpServer(), add checkTcpClient().
 * @since 3.3.1 [2026-08-17-09:00am] Changed position of startOutputs() in setup().
 * 
 * --- Boot. ---
 *     Include libraries.
 *     Define global vars.
 *     Define general functions.
 *     Define setup() functions.
 *     Define FreeRTOS functions.
 *     Define event handlers.
 *     Define loop() functions.
 *
 * --- Run setup(). ---
 *     startOutputs()                 - Start serial & microSD card reader. Status LED is YELLOW then WHITE.
 *     buildInfo()                    - Build & processor info. Status LED is YELLOW then WHITE.
 *     prefUtility(PREF_INIT)         - Get preferences.
 *     startSerial()                  - Start serial interfaces.
 *     initPins()                     - Initialize pin modes & pin values.
 *     startI2C()                     - Start I2C wire interfaces.
 *     startLiPo()                    - Start LiPo I2C interface.
 *     startWiFiServer()              - Start WiFi.
 *     startTcpServer()               - Start TCP server for GNSS Master (NMEA out / RTCM in bridge).
 *     startHttpServer()              - Start HTTP server.
 *     startWebSocketServer()         - Start WebSocket server.
 *     startAndConfigGNSS()           - Start GNSS, config ZED settings.
 *     startQueues()                  - Start GhostRover FreeRTOS queues.
 *     startTasks()                   - Start GhostRover FreeRTOS tasks.
 *     preLoop()                      - Prepare for loop().
 *
 * --- Run loop(). ---    
 *     checkTimers()        - Check all timers, check ZED to trigger DevUBLOXGNSS::processNMEA().
 *       - After THROTTLE_CHECK_ZED expires:
 *         - Run roverGNSS.checkUblox().
 *         - Call buildOperData() to set GNSS global vars.
 *         - Set flag to send RTCM if page is "ntrip".
 *         - Set pending browser update flag.
 *     processJsonActivity()          - @see "Operation summary" in description for processJsonActivity().
 *       - Process one incoming WebSocket message, if queued.
 *         - Remove message from queue.
 *         - Deserialize JSON.
 *         - Set page name.
 *         - Depending on page (config, files, nmea, operate, ntrip, ...):
 *           - Fill jsonDocToBrowser[] with page specific data. Run specific functions for some pages.
 *              - If "nmea" page, do nothing. Processing is loop() -> checkTimers() -> DevUBLOXGNSS::processNMEA().
 *           - Send data to browser.
 *           - If periodic status update is pending, send to browser page.
 *           - If NTRIP status update is pending, send to browser page.
 * 
 *     checkSerialUSB()               - Check serial USB for input.
 *     // checkGnssLockButton()       - Check GNSS lock button.
 *     checkTcpClient()               - Check TCP server for new/dropped client (GNSS Master, ..).
 *     ws.cleanupClients()            - HTTP WebSocket cleanup.
 *     debug()                        - Display debug.
 *
 * --- GhostRover FreeRTOS functions. ---
 *     taskLoopStatusLed()            - GhostRover FreeRTOS task - Set Loop() status LED to blink or solid.
 *     taskRtcmRelay()                - GhostRover FreeRTOS task - Relay RTCM from source to -> Serial2 (ZED UART2).
 *     rtcm3GetMessageType()          - Called by taskRtcmRelay - return RTCM3 message type.
 *     relayRtcmByte()                - Called by taskRtcmRelay - read byte from NTRIP client, write to Serial2 (ZED UART2).
 *     ntripBeginClient()             - Called by taskRtcmRelay - connect to NTRIP caster.
 *
 * --- Event handlers for core/additional library processes. ---
 *     -- onWiFiEvent()               -- <WiFi.h> & <WiFiAP.h> WiFi event handler (WiFiEvent_t).
 *        - if commandFlag[DEBUG_WIFI]), print WiFi status.
 *     -- onHttpFileUpload()          -- <ESPAsyncWebServer.h> HTTP endpoint ("/upload") event handler (AsyncWebServerRequest).
 *        - write file to SD, print upload status.
 *     -- onWebSocketEvent()          -- <ESPAsyncWebServer.h> WebSocket event handler (AsyncWebSocket).
 *        - cases: WS_EVT_CONNECT, WS_EVT_DISCONNECT,WS_EVT_DATA,WS_EVT_PONG,WS_EVT_ERROR.
 *        - print status, set LED color.
 *        - if WS_EVT_DATA, push (xQueueSend) JSON struct (data & length) into GhostRover FreeRTOS QueueHandle_t wsRxQueue.
 *     -- DevUBLOXGNSS::processNMEA() -- <SparkFun_u-blox_GNSS_v3.h> DevUBLOXGNSS::processNMEA event handler (char incoming).
 *        - Track counts of NMEA sentences (all & each type) for operate page, status section.
 *        - Set status LED red if I2C (Wire1) is down, call startI2C() to restart.
 */

/**
 * -------------------------------------------------------------------------
 *  Board LED status.
 * -------------------------------------------------------------------------
 *
 * @since 3.1.1 [2026-06-25-01:00pm] New.
 * @since 3.2.1 [2026-06-25-01:00pm] Updated GR-MCU LED status.
 *
 *   ws2812LedColor = RED, YELLOW, GREEN, BLUE, WHITE.
 *   ws2812LedBlink = true, false.
 *   "Good" LED transition from power-on to connect will be: YELLOW -> WHITE -> BLUE -> GREEN.
 * 
 * --- GR-MCU ----
 *     -- setup(). --
 *        - solid YELLOW: startup delay.
 *        - solid  WHITE: setup() started & running ok.
 *        - solid    RED: startWiFiServer() error,
 *                        startOutputs() error,
 *                        startAndConfigGNSS() error.
 *     -- loop(). --  
 *        - solid   BLUE: loop running ok with no websocket connection.
 *        - solid  GREEN: loop running ok with onWebSocketEvent(WS_EVT_CONNECT) connection.
 *        - blink  GREEN: relaySerial1toSerial2() RTCM in Serial1 out Serial2. 
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
 * @since 3.2.2   [2026-08-09-11:45am] Add NTRIP client: add base64.h.
 * @since 3.2.2   [2026-08-09-01:45pm] Updated library <AsyncTCP.h>                from 3.4.10  to 3.5.0.
 * @since 3.2.2   [2026-08-09-01:45pm] Updated library <ESPAsyncWebServer.h>       from 3.11.1  to 3.12.0.
 * @since 3.3.1   [2026-08-16-05:30pm] Changed SD card reader library from <SD.h> to <SD_MMC.h>.
 * @link  Arduino https://docs.arduino.cc/libraries/.
 * @link  ESP32   https://docs.espressif.com/projects/arduino-esp32/en/latest/libraries.html.
 */

// --- Core. ---
#include <Arduino.h>                                       // https://github.com/espressif/arduino-esp32.
#include <WiFi.h>                                          // https://github.com/espressif/arduino-esp32/tree/master/libraries/WiFi.
#include <WiFiAP.h>                                        // https://github.com/espressif/arduino-esp32/tree/master/libraries/WiFi.
#include <SD_MMC.h>                                        // https://github.com/espressif/arduino-esp32/tree/master/libraries/SD_MMC.
#include <FS.h>                                            // https://github.com/espressif/arduino-esp32/tree/master/libraries/FS.
#include <SPI.h>                                           // https://github.com/espressif/arduino-esp32/tree/master/libraries/SPI.
#include <Wire.h>                                          // https://github.com/espressif/arduino-esp32/blob/master/libraries/Wire/src/Wire.h.
#include <time.h>                                          // https://github.com/espressif/arduino-esp32/blob/master/cores/esp32/esp32-hal-time.c#L47.
#include <esp_system.h>                                    // https://github.com/pycom/pycom-esp-idf.
#include <esp_chip_info.h>                                 // https://github.com/pycom/pycom-esp-idf.
#include <Preferences.h>                                   // https://github.com/espressif/arduino-esp32/tree/master/libraries/Preferences/.
#include "base64.h"                                        // https://github.com/espressif/arduino-esp32/tree/master/cores/esp32.

// --- Additional. ---                  
#include <AsyncTCP.h>                                      // https://github.com/ESP32Async/AsyncTCP (3.5.0).
#include <ESPAsyncWebServer.h>                             // https://github.com/ESP32Async/ESPAsyncWebServer (3.12.0).
#include <ArduinoJson.h>                                   // https://github.com/bblanchon/ArduinoJson (7.4.3).
#include <SparkFun_MAX1704x_Fuel_Gauge_Arduino_Library.h>  // https://github.com/sparkfun/SparkFun_MAX1704x_Fuel_Gauge_Arduino_Library (1.0.4).
#include <SparkFun_u-blox_GNSS_v3.h>                       // https://github.com/sparkfun/SparkFun_u-blox_GNSS_v3 (3.1.14).            

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
 * @since 3.1.2  [2026-07-16-10:00am] Moved MAJOR, MINOR, PATCH from buildInfo() to "Operation" section.
 * @since 3.2.1  [2026-07-24-03:30pm] Refactor JSON.
 * @since 3.2.1  [2026-07-28-04:45pm] Removed NMEA out switch & preference.
 * @since 3.2.2  [2026-08-09-11:45am] Add NTRIP client.
 * @since 3.2.3  [2026-08-10-09:45am] Add TCP to replace BLE.
 * @since 3.3.0  [2026-08-13-01:30pm] Added global debugFlag.
 * @since 3.3.1  [2026-08-16-05:30pm] Add LOG_FILE, outputBuffer.
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

// --- TCP (GNSS Master: NMEA out, RTCM in for "bridge" mode). ---
const uint16_t TCP_SERVER_PORT = 9099;                    // GNSS Master (Android) connects here.
WiFiServer     gnssTcpServer(TCP_SERVER_PORT);            // TCP server object.
WiFiClient     gnssTcpClient;                             // Single TCP client (GNSS Master). Single-client by design.
bool           tcpClientConnected = false;                // Status: TCP client connected.

// --- HTTP. ---
const char     WEBSOCKET_SERVER_NAME[] = "/ghostRover";
uint8_t        clientId                = 0;               // HTTP WebSocket client ID # (+1 for each new connection).
AsyncWebServer httpServer(80);                            // HTTP AsyncWebServer object on port 80.
AsyncWebSocket ws(WEBSOCKET_SERVER_NAME);                 // HTTP WebSocket object.

// --- WebSocket. ---
const uint8_t WS_RX_QUEUE_LEN     = 5;                    // Max # of WebSocket queued incoming messages.
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

// --- NTRIP client. ---
WiFiClient ntripClient;                                    // WiFi connection to NTRIP caster.
                                                           // Owned only by taskRtcmRelay() (core 0) for connect()/stop()/read().
                                                           // Requests/status cross task boundary as flags, same pattern as browserUpdatePendingFlag.                                                            
bool       ntripConnected             = false;             // Status: connected to caster. Set only by taskRtcmRelay().
bool       ntripConnectRequest        = false;             // Set by processJsonActivity(), cleared by taskRtcmRelay().
bool       ntripDisconnectRequest     = false;             // Set by processJsonActivity(), cleared by taskRtcmRelay().
bool       ntripStatusPending         = false;             // New ntripStatusMsg ready to forward to browser.
bool       ntripsendRtcmSentenceCount = false;             // Flag to send rtcmSentenceCount for ntrip page. Triggered by checkTimers().
char       ntripStatusMsg[100]        = {'\0'};            // Latest status line for "connectNtripCasterResp".
char       lastGGA[100]               = {'\0'};            // Last complete GGA sentence (any page). @see DevUBLOXGNSS::processNMEA().

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
    DEBUG_NTRIP,                                          // 16.
    DEBUG_TIMERS,                                          // 17.
    NUM_COMMANDS                                          // 18 = automatic array length.
};     
const char* COMMAND[NUM_COMMANDS] = {                     // Command strings; match CommandIndex.
    "testRad",                                            // TEST_RAD.
    "debugRTCM",                                          // DEBUG_RTCM.
    "debugGNSS",                                          // DEBUG_GNSS.
    "debugNMEA",                                          // DEBUG_NMEA.
    "debugBtn",                                           // DEBUG_BTN.
    "debugSer",                                           // DEBUG_SER.
    "debugWiFi",                                          // DEBUG_WIFI.
    "debugWS",                                            // DEBUG_WS.
    "debugLiPo",                                          // DEBUG_LIPO.
    "showUpTime",                                         // SHOW_UPTIME.
    "restart",                                            // RESTART.
    "checkWire1",                                         // CHECK_WIRE1.
    "debugTemp",                                          // DEBUG_TEMP.
    "debugNMEAhex",                                       // DEBUG_NMEA_HEX.
    "debugNMEAcounts",                                    // DEBUG_NMEA_COUNTS.
    "debugPrefs",                                         // DEBUG_PREFS.
    "debugNTRIP",                                          // DEBUG_NTRIP.
    "debugTimers"                                         // DEBUG_TIMERS.
};     
const bool    RW_MODE                   = false;          // Open preference name space as read/write.
const bool    RO_MODE                   = true;           // Open preference name space as read only.
const char    LOG_FILE[]                = "/log.txt";     // Log file.
const uint8_t MAJOR_VERSION             = 3;              // Current major build version (@see buildInfo()).
const uint8_t MINOR_VERSION             = 3;              // Current minor build version (@see buildInfo()).
const uint8_t PATCH_VERSION             = 1;              // Current patch build version (@see buildInfo()).
const uint8_t MIN_SATELLITE_THRESHHOLD  = 2;              // Minimum SIV for reliable coordinate information.      
bool          ghostMode                 = false;          // Flag, in Ghost mode (i.e. locked coordinates).
bool          i2cUp                     = false;          // Status: true if both Wire & Wire1 up, else false.
bool          inLoop                    = false;          // In loop() indicator.
bool          RTCMin                    = false;          // RTCM being received within RTCM_TIMEOUT.
bool          NMEAout                   = false;          // NMEA being sent OUT to MCU #2.
bool          zeroStatusCounters        = false;          // Flag to zero status counters.
bool          todoRestartGrMCU          = false;          // Flag to restart GR-MCU.
bool          buttonGnssLock;                             // UI - // ToDo: Implement.
bool          buttonAltitudeLock;                         // UI - // ToDo: Implement.
bool          buttonPositionLock;                         // UI - // ToDo: Implement.
bool          buttonLaser;                                // UI button to turn laser pointer on/off.
bool          buttonUnlockAll;                            // UI - // ToDo: Implement.
bool          commandFlag[NUM_COMMANDS] = {false};        // Command flags.
bool          debugFlag                 = false;          // Debug flag.
char          uptime[20]                = {'\0'};         // 01h 03m 12s.
char          operMode[2]               = {'\0'};         // Operation mode (r=rover, b=base).
char          debugTemp[250]            = {'\0'};         // Various debug scenarios.
char          whichPage[10]             = {'\0'};         // Current browser page served by startHttpServer().
char          buildString[40]           = {'\0'};         // Build string (build version on date at time). e.g. 3.0.12 - Feb 19 2026 @ 12:23:13
char          serialState[4];                             // Serial state: '-', 'u', or 'd'.
                                                          // serialState[0] = USB interface.
                                                          // serialState[1] = Serial0 interface, not used.
                                                          // serialState[2] = Serial1 interface, RTCM in from HC-12.
                                                          // serialState[3] = Serial2 interface, RTCM out to ZED UART2.
                                                          // prfRtcmInSource (char[6]) RTCM in source: off, radio, ntrip, ...
                                                          // RTCM (bool) being received within RTCM_TIMEOUT.
                                                          // NMEA (bool) being sent OUT to MCU #2.
char          nmeaBuffer[120]           = {'\0'};         // Buffer for NMEA sentence. @see DevUBLOXGNSS::processNMEA().  // ToDo: Move to local var?
char          operBuffer[24]            = {'\0'};         // Buffer for Operate data.
char          startOutputsResults[300]  = {'\0'};         // Buffered output from startOutputs().
char          buildInfoResults[200]     = {'\0'};         // Buffered output from buildInfo().
char          outputBuffer[200]         = {'\0'};         // Buffer for output data.
size_t        wsSendCount               = 0;              // # of WebSocket messages sent.
size_t        rtcmSentenceCount         = 0;              // # of RTCM sentences in.
u_int8_t      numSatInView              = 0;              // GNSS - # OF satellites in view.
u_int8_t      fixType                   = 0;              // GNSS - type of fix (single, RTK-float, RTK-fix).
int64_t       bootTime;                                   // Boot time.
float         rtcmKbps                  = 0;              // RTCM kbps (average).
float         heightEllipsoid           = 0;              // GNSS - ellipsoid height.
float         heightOrthometric         = 0;              // GNSS - orthometric height.
float         accuracyHorizontal        = 0;              // GNSS - horizontal accuracy.
float         accuracyVertical          = 0;              // GNSS - vertical accuracy.
float         batterySoc                = 0;              // Battery State Of Charge (SOC).
float         batteryChangeRate         = 0;              // Battery charge - rate of change.
double        lat                       = 0;              // GNSS - latitude.
double        lon                       = 0;              // GNSS - longitude.

// --- Flags. ---
bool          browserUpdatePendingFlag  = false;          // Update ready to send to browser page (operate, nmea, ...).

// --- Preferences. ---
const uint16_t NTRIP_CAST_ATTR_LEN      = 512;            // Length of character array for NTRIP caster attibute profile.
char           prfUnt[6];                                 // Distance units: meter/feet (used only in browser).
char           prfRtcmInSource[8];                        // RTCM in source: off, radio, ntrip, ...
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
int64_t nmeaRate           = 0;
int64_t lastRTCMtime       = esp_timer_get_time();      // Last time (us) when RTCM input received.

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
 * @since 3.2.2  [2026-08-09-11:45am] Add NTRIP client: add relayRtcmByte(), add ntripBeginClient(), & ntripPushGGA().
 * @since 3.3.0  [2026-08-13-12:30pm] Replaced ntripPushGGA() with checkTimers().
 * @since 3.3.1  [2026-08-16-05:30pm] Add logPrint().
 * @see   logPrint()              - Save a message to LOG_FILE, print to Serial if available.
 * @see   statusLedOn()           - Turn on status LED.
 * @see   prefUtility()           - Preference utility.
 * @see   buildOperData()         - Build data for operate page.
 * @see   sendDataToBrowser()     - Send jsonDocToBrowser.
 * @see   rtcm3GetMessageType()   - Return RTCM3 message type to taskRtcmRelay().
 * @see   relayRtcmByte()         - Relay RTCM byte to Serial2 (ZED UART2), tracking stats.
 * @see   ntripBeginClient()      - Connect to NTRIP caster & validate credentials.
 */

 /**
 * -------------------------------------------------------------------------
 *  Print text to USB if Serial available. Add text to LOG_FILE.
 * -------------------------------------------------------------------------
 *
 * @param  char* textToBuffer Text to log/print.
 * @return void No output is returned.
 * @since  3.3.1 [2026-08-16-05:15pm] New.
 */
void logPrint(const char* textToLogPrint = NULL) {

    if (textToLogPrint == NULL)  {
        return;
    } else {

        // --- Print to Serial. ---
        if (serialState[0] == 'u') {
            Serial.print(textToLogPrint);
        }

        // --- Send to log file. ---
        File file = SD_MMC.open(LOG_FILE, FILE_APPEND);
        if (file) {
            file.print(textToLogPrint);
            file.close();
        }
    }
}

/**
 * -------------------------------------------------------------------------
 *  Turn on status LED.
 * -------------------------------------------------------------------------
 * 
 * @return void  No output is returned.
 * @since  3.0.12 [2026-02-10-10:45pm] New.
 * @see buildInfo(), startWiFiServer(), startOutputs(), startAndConfigGNSS(), taskLoopStatusLed().
 */
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
 * Keys for preference values are sent in WebSocket as "1", "2", etc. but stored in NVS as "prfUnt", "prfRtcmInSource", ... .
 * 
 * @param  enum    prefAction PREF_INIT, PREF_READ, PREF_SET, PREF_RESET, PREF_PRINT, PREF_SET_NTRIP.
 * @param  array   key WebSocket JSON key.
 * @param  array   value WebSocket JSON value.
 * @return void    No output is returned.
 * @since  3.0.12 [2026-02-07-10:30am] New.
 * @since  3.0.12 [2026-02-14-06:15pm] Remove prfRqsPvtInt.
 * @since  3.0.12 [2026-02-18-06:00pm] Add buildString.
 * @since  3.0.12 [2026-02-23-01:00pm] Shorten RTCM & NMEA status.
 * @since  3.0.12 [2026-02-28-02:45pm] Fix bugs: prfRtcmInSource, jsonDocToBrowser.clear().
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
    const char      DEF_RTC_IN[]            = "off";            // Default control RTCM in: off, radio, ntrip, ...                                         2 - Matching global var: char     prfRtcmInSource[6].
    const char      DEF_HOT_SSI[]           = "";               // Default WiFi hotspot client: network SSID.                                        4 - Matching global var: char     prfHotSsi[20].
    const char      DEF_HOT_PASS[]          = "";               // Default WiFi hotspot client: password.                                            5 - Matching global var: char     prfHotPas[30].
    const char      DEF_NTRIP_CAST_ATTR_1[] = "{\"43\":\"1\",\"44\":\"\",\"45\":\"\",\"46\":\"\",\"47\":\"2101\",\"48\":\"1\",\"49\":\"\",\"50\":\"\",\"51\":\"1\"}";
    const char      DEF_NTRIP_CAST_ATTR_2[] = "{\"43\":\"2\",\"44\":\"\",\"45\":\"\",\"46\":\"\",\"47\":\"2101\",\"48\":\"1\",\"49\":\"\",\"50\":\"\",\"51\":\"1\"}";
    const char      DEF_NTRIP_CAST_ATTR_3[] = "{\"43\":\"3\",\"44\":\"\",\"45\":\"\",\"46\":\"\",\"47\":\"2101\",\"48\":\"1\",\"49\":\"\",\"50\":\"\",\"51\":\"1\"}";
                                                                // Default NTRIP caster attribute profile 1.                                         6 - Matching global var: char     prfNtripCastAttr[0].
                                                                // Default NTRIP caster attribute profile 2.                                         7 - Matching global var: char     prfNtripCastAttr[1].
                                                                // Default NTRIP caster attribute profile 3.                                         8 - Matching global var: char     prfNtripCastAttr[2].
    const char      DEF_NTRIP_CAST_ACT[]    = "1";              // Default NTRIP caster profile being used.                                         10 - Matching global var: char     prfNtripCastAct.
    const uint8_t   DEF_GNS_NAV_RAT         = 2;                // Default ZED rate (times/interval): OUTPUT a new solution.                         9 - Matching global var: uint8_t  prfGnsNavRat.
    const uint16_t  DEF_GNS_MSR_INT         = 100;              // Default ZED interval (ms): CREATE a new solution.                                11 - Matching global var: uint16_t prfGnsMsrInt.
    const uint16_t  DEF_INSTR_HGT           = 128;              // Default instrument height (mm - includes rover height [128] + pole height [0]).  12 - Matching global var: uint16_t prfInstrHgt.
    const uint16_t  NUM_PREFS               = 12;               // Number of preferences being used.
    size_t remaining                        = 0;                // Number of bytes remaining that can be written to char array. 
    bool            hasKey                  = false;
    char diagMsg[100]                       = {'\0'};
    DeserializationError ntripError;

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
            snprintf(diagMsg, sizeof(diagMsg), "NVS namespace %s using %u entries with %u available.\n", NAMESPACE, NUM_PREFS, roverPrefs.freeEntries());
            logPrint(diagMsg);
            break;

        case PREF_READ:

            // -- Open name space. --
            roverPrefs.begin(NAMESPACE, RO_MODE);

            // -- Set global vars from NVS preferences. --
            roverPrefs.getString("prfUnt",          prfUnt,              sizeof(prfUnt));    // Preference stored as "prfUnt".
            roverPrefs.getString("prfRtcmInSource", prfRtcmInSource,     sizeof(prfRtcmInSource));
            roverPrefs.getString("prfHotSsi",       prfHotSsi,           sizeof(prfHotSsi));
            roverPrefs.getString("prfHotPas",       prfHotPas,           sizeof(prfHotPas));
            roverPrefs.getString("prfNtripCaster1", prfNtripCastAttr[0], NTRIP_CAST_ATTR_LEN);
            roverPrefs.getString("prfNtripCaster2", prfNtripCastAttr[1], NTRIP_CAST_ATTR_LEN);
            roverPrefs.getString("prfNtripCaster3", prfNtripCastAttr[2], NTRIP_CAST_ATTR_LEN);
            roverPrefs.getString("prfNtripCastAct", prfNtripCastAct,     sizeof(prfNtripCastAct));
            prfGnsNavRat = roverPrefs.getUShort("prfGnsNavRat");           
            prfGnsMsrInt = roverPrefs.getUShort("prfGnsMsrInt");
            prfInstrHgt  = roverPrefs.getUShort("prfInstrHgt");

            // - Create NTRIP caster JSON doc from embedded JSON string for active caster. -
            // Embedded JSON string allows attributes for an NTRIP caster to be stored as a single preference. 
            JsonDocNtrip.clear();
            ntripError = deserializeJson(JsonDocNtrip, prfNtripCastAttr[atoi(prfNtripCastAct)-1]);
            if (ntripError) {
                snprintf(diagMsg, sizeof(diagMsg), "JSON deserialize failed: %s\n", ntripError.f_str());
                logPrint(diagMsg);
                return;
            }
            // Serial.print("prfNtripCastAct=");
            // Serial.println(prfNtripCastAct);
            // Serial.print("prfNtripCastAttr[atoi(prfNtripCastAct)]=");
            // Serial.println(prfNtripCastAttr[atoi(prfNtripCastAct)]);

            // - Set global vars from JSON values. -
            strlcpy(ntripCaster.name,  JsonDocNtrip["44"],  sizeof(ntripCaster.name));
            strlcpy(ntripCaster.url,   JsonDocNtrip["45"],   sizeof(ntripCaster.url));
            strlcpy(ntripCaster.mount, JsonDocNtrip["46"], sizeof(ntripCaster.mount));
            strlcpy(ntripCaster.user,  JsonDocNtrip["49"],  sizeof(ntripCaster.user));
            strlcpy(ntripCaster.pass,  JsonDocNtrip["50"],  sizeof(ntripCaster.pass));
            ntripCaster.id      = atoi(JsonDocNtrip["43"]);
            ntripCaster.port    = atoi(JsonDocNtrip["47"]);
            ntripCaster.version = atoi(JsonDocNtrip["48"]);
            ntripCaster.sendGga = JsonDocNtrip["51"].as<bool>();

            // -- Close name space. --
            roverPrefs.end();
            logPrint("Preferences read.\n");
            break;

        case PREF_SET:

            // -- Open name space. --
            roverPrefs.begin("config", RW_MODE);

            // - Set NVS preferences from global vars. -
            roverPrefs.putString("prfUnt",          prfUnt);                // Store as "prfUnt"              (sent/rcvd as "1").
            roverPrefs.putString("prfRtcmInSource", prfRtcmInSource);       // Store as "prfRtcmInSource"     (sent/rcvd as "2").
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
            logPrint("Preferences saved.\n");
            todoRestartGrMCU = true;
            logPrint("\nGR-MCU will restart.\n");
            break;

        case PREF_RESET:

            // -- Copy default values to global vars. --
            strlcpy(prfUnt,              DEF_UNT,               sizeof(prfUnt));
            strlcpy(prfRtcmInSource,     DEF_RTC_IN,            sizeof(prfRtcmInSource));
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
            logPrint("Resetting all preferences.\n");
            todoRestartGrMCU = true;
            logPrint("\nGR-MCU will restart.\n");
            break;

        case PREF_PRINT:

            // -- Open name space. --
            roverPrefs.begin(NAMESPACE, RO_MODE);

            // -- Print values. --
            Serial.println("---                    Default, Global, NVS. ---");
            Serial.printf( "prfUnt                 \"%s\", \"%s\", \"%s\"\n", DEF_UNT,            prfUnt,          roverPrefs.getString("prfUnt"));
            Serial.printf( "prfRtcmInSource        \"%s\", \"%s\", \"%s\"\n", DEF_RTC_IN,         prfRtcmInSource, roverPrefs.getString("prfRtcmInSource"));
            Serial.printf( "prfHotSsi              \"%s\", \"%s\", \"%s\"\n", DEF_HOT_SSI,        prfHotSsi,       roverPrefs.getString("prfHotSsi"));
            Serial.printf( "prfHotPas              \"%s\", \"%s\", \"%s\"\n", DEF_HOT_PASS,       prfHotPas,       roverPrefs.getString("prfHotPas"));
            Serial.printf( "prfGnsNavRat           %u, %u, %u\n",             DEF_GNS_NAV_RAT,    prfGnsNavRat,    roverPrefs.getUShort("prfGnsNavRat"));
            Serial.printf( "prfGnsMsrInt           %u, %u, %u\n",             DEF_GNS_MSR_INT,    prfGnsMsrInt,    roverPrefs.getUShort("prfGnsMsrInt"));

            Serial.printf( "DEF_NTRIP_CAST_ATTR_1  \"%s\"\n", DEF_NTRIP_CAST_ATTR_1);
            Serial.printf( "DEF_NTRIP_CAST_ATTR_2  \"%s\"\n", DEF_NTRIP_CAST_ATTR_2);
            Serial.printf( "DEF_NTRIP_CAST_ATTR_3  \"%s\"\n", DEF_NTRIP_CAST_ATTR_3);

            Serial.printf( "prfNtripCastAttr[0]    \"%s\"\n", prfNtripCastAttr[0]);
            Serial.printf( "prfNtripCastAttr[1]    \"%s\"\n", prfNtripCastAttr[1]);
            Serial.printf( "prfNtripCastAttr[2]    \"%s\"\n", prfNtripCastAttr[2]);

            roverPrefs.getString("prfNtripCaster1", prfNtripCastAttr[3], NTRIP_CAST_ATTR_LEN);      // [0,1,2] are used, [3] is for scratch.
            Serial.printf( "prfNtripCaster1        \"%s\"\n", prfNtripCastAttr[3]);
            roverPrefs.getString("prfNtripCaster2", prfNtripCastAttr[3], NTRIP_CAST_ATTR_LEN);
            Serial.printf( "prfNtripCaster2        \"%s\"\n", prfNtripCastAttr[3]);
            roverPrefs.getString("prfNtripCaster3", prfNtripCastAttr[3], NTRIP_CAST_ATTR_LEN);
            Serial.printf( "prfNtripCaster3        \"%s\"\n", prfNtripCastAttr[3]);
            memset(prfNtripCastAttr[3], '\0', NTRIP_CAST_ATTR_LEN);

            Serial.printf( "prfNtripCastAct        \"%s\", \"%s\", \"%s\"\n", DEF_NTRIP_CAST_ACT, prfNtripCastAct, roverPrefs.getString("prfNtripCastAct"));
            Serial.printf( "ntripCaster.name     = \"%s\"\n", ntripCaster.name);
            Serial.printf( "ntripCaster.url      = \"%s\"\n", ntripCaster.url);
            Serial.printf( "ntripCaster.mount    = \"%s\"\n", ntripCaster.mount);
            Serial.printf( "ntripCaster.user     = \"%s\"\n", ntripCaster.user);
            Serial.printf( "ntripCaster.pass     = \"%s\"\n", ntripCaster.pass);
            Serial.printf( "ntripCaster.id.      = \"%d\"\n", ntripCaster.id);
            Serial.printf( "ntripCaster.version  = \"%d\"\n", ntripCaster.version);
            Serial.printf( "ntripCaster.port     = \"%d\"\n", ntripCaster.port);
            Serial.printf( "ntripCaster.sendGga  = \"%d\"\n", ntripCaster.sendGga);

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
            logPrint("NTRIP preference set.\n");
            todoRestartGrMCU = true;
            logPrint("\nGR-MCU will restart.\n");
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
 * @since  3.2.3  [2026-08-11-09:00am] Moved browserUpdatePendingFlag to DevUBLOXGNSS::processNMEA().
 * @since  3.3.1  [2026-08-17-09:15pm] Changed from Serial.print() to logPrint().
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
        int32_t seconds = (esp_timer_get_time() - bootTime)/1000000;
        int32_t minutes = seconds / 60;
        int32_t hours = minutes / 60;
        snprintf(uptime, sizeof(uptime), "%uh %um %us", hours % 24, minutes % 60, seconds % 60);

        if (commandFlag[DEBUG_TIMERS]) {
            Serial.print("buildOperData() executed.\n");
        }
    }
}

/**
 * -------------------------------------------------------------------------
 *  Send data to browser.
 * -------------------------------------------------------------------------
 *
 * @return void No output is returned.
 * @since  3.2.1 [2026-07-26-06:30pm] New.
 * @since  3.2.1 [2026-07-30-10:30am] jsonDocToBrowser["NMEA"] '= lastNmea' was '= nmeaBuffer'.
 * @since  3.2.1 [2026-07-31-01:30pm] Moved "Wrap up" section from processJsonActivity() to here.
 * @since  3.2.2 [2026-08-09-05:30pm] Only print for DEBUG_WS.
 * @since  3.3.0 [2026-08-15-12:00pm] Moved restarts to checkToDoFlags().
 * @see    checkTimers(), processJsonActivity(), DevUBLOXGNSS::processNMEA().
 * @see    processJsonActivity() for description of exchange protocol.
 */
void sendDataToBrowser() {      // Browser sets state: 1) respond to from browser or 2)periodic update (nmea, operate) to browser, 

    // --- NMEA page. ---
    if (strcmp(whichPage, "nmea") == 0) {
        jsonDocToBrowser["NMEA"] = lastNmea;
        browserUpdatePendingFlag = true;
    }

    // --- NTRIP page. ---
    if ((strcmp(whichPage, "ntrip") == 0) && (ntripsendRtcmSentenceCount))  {
        jsonDocToBrowser["37"] = rtcmSentenceCount;
        ntripsendRtcmSentenceCount = false;
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
            jsonDocToBrowser["16"] = (RTCMin)  ? true : false;      // Up (true), down (false). @see taskRtcmRelay().
            jsonDocToBrowser["17"] = (NMEAout) ? true : false;      // Up (true), down (false). @see DevUBLOXGNSS::processNMEA().
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
 * @since  3.2.3 [2026-08-10-10:15pm] Defensive cast (uint8_t) since Xtensa/ESP32 defaults char to unsigned.
 * @see    checkRTCMtoRadio().
 * @link   https://portal.u-blox.com/s/question/0D52p0000C7MwDfCQK/can-you-find-out-the-message-type-of-a-given-rtcm3-message.
 */
uint16_t rtcm3GetMessageType(const char* rtcmSentence) {
    // Serial.printf("[%02x] [%02x] [%02x] [%02x] [%02x]\n", rtcmSentence[0],  rtcmSentence[1], rtcmSentence[2], rtcmSentence[3], rtcmSentence[3]);
    if ((uint8_t)rtcmSentence[0] != 0xD3) {    // Check if preamble is correct
        return 0;               // Invalid preamble.
    }
    uint16_t message_type = ((uint16_t)(uint8_t)rtcmSentence[3] << 4) | ((uint8_t)rtcmSentence[4] >> 4);
    return message_type;
}

// /**. // ToDo: Delete after verifying new version.
//  * -------------------------------------------------------------------------
//  *  Relay RTCM byte to Serial2 (ZED UART2), tracking stats.
//  * -------------------------------------------------------------------------
//  *
//  * Extracted so the "ntrip" branch of taskRtcmRelay() can reuse the same
//  * preamble-detection/stats logic as the "radio" branch without duplicating
//  * it. The "radio" branch itself is left as-is for now.
//  *
//  * @param  char      inputChar     Byte to relay.
//  * @param  char*     rtcmSentence  Sentence buffer (caller-owned, sized 1030).
//  * @param  uint16_t  &byteCount    Caller-owned running byte count.
//  * @param  uint16_t  &msg_type     Caller-owned last parsed message type.
//  * @return void No output is returned.
//  * @since  3.2.2 [2026-08-09-12:00pm] New.
//  * @see    taskRtcmRelay(), rtcm3GetMessageType().
//  */
// void relayRtcmByte(char inputChar, char* rtcmSentence, uint16_t &byteCount, uint16_t &msg_type) {
//     Serial2.write(inputChar);
//     if (byteCount < 1030 - 1) {                                     // Bounds check.
//         rtcmSentence[byteCount] = inputChar;
//     }
//     RTCMin = true;
//     ws2812LedColor = GREEN;
//     ws2812LedBlink = true;

//     if (inputChar == (char)0xd3) {                                  // Start of new sentence.
//         rtcmSentenceCount++;
//         msg_type = rtcm3GetMessageType(rtcmSentence);
//         int64_t RTCMintervalUs = esp_timer_get_time() - lastRTCMtime;
//         if (RTCMintervalUs > 0) {
//             rtcmKbps = ((float)byteCount * 8.0f * 1000.0f) / (float)RTCMintervalUs;
//         }
//         if (commandFlag[DEBUG_RTCM]) {
//             Serial.printf("\nRTCM3 (%s) active(%d) #%zu Type:%u bytes:%u kbps:%.2f\n\nd3 ",
//                 prfRtcmInSource, RTCMin, rtcmSentenceCount, msg_type, byteCount, rtcmKbps);
//         }
//         lastRTCMtime = esp_timer_get_time();
//         memset(rtcmSentence, '\0', 1030);
//         rtcmSentence[0] = 0xd3;
//         byteCount = 1;
//     } else {
//         if (commandFlag[DEBUG_RTCM]) {
//             Serial.printf("%02x ", inputChar);
//         }
//         byteCount++;
//     }
// }

/**
 * -------------------------------------------------------------------------
 *  Relay RTCM byte to Serial2 (ZED UART2), tracking stats.
 * -------------------------------------------------------------------------
 *
 * RTCM3 framing: [0xD3][6 reserved bits + 10-bit length][length bytes payload][3-byte CRC24Q].
 * The payload is arbitrary binary data and CAN legitimately contain the byte
 * value 0xD3 - RTCM3 uses length-prefixed framing, not byte-stuffing. A message
 * boundary can only be found by counting down the declared length; treating
 * every 0xD3 in the stream as a new preamble mis-frames any message whose
 * payload happens to contain that byte, corrupting the parsed type and count.
 *
 * @param  char      inputChar          Byte to relay.
 * @param  char*     rtcmSentence       Sentence buffer (caller-owned, sized 1030).
 * @param  uint16_t  &byteCount         Caller-owned byte position within the current frame.
 * @param  uint16_t  &bytesLeftInFrame  Caller-owned countdown; 0 means "expecting a preamble byte", 0xFFFF means "header length not yet known".
 * @param  uint16_t  &msg_type          Caller-owned last parsed message type.
 * @return void      No output is returned.
 * @since  3.2.3     [2026-08-10] Refactored to use length-based framing instead of "any 0xD3 = new message".
 * @see    taskRtcmRelay(), rtcm3GetMessageType().
 * @link   https://www.use-snip.com/kb/knowledge-base/an-rtcm-message-cheat-sheet/.
 */
void relayRtcmByte(char inputChar, char* rtcmSentence, uint16_t &byteCount, uint16_t &bytesLeftInFrame, uint16_t &msg_type) {
    Serial2.write(inputChar);               // Always relay immediately - framing state never affects this.

    // --- Idle: expecting the next preamble. ---
    if (bytesLeftInFrame == 0) {
        if ((uint8_t)inputChar != 0xd3) {
            if (commandFlag[DEBUG_RTCM]) {
                Serial.printf("RTCM3 desync - expected preamble, got %02x\n", (uint8_t)inputChar);
            }
            return;                         // Drop stray byte from parsing; relay above already happened.
        }
        RTCMin = true;
        ws2812LedColor = GREEN;
        ws2812LedBlink = true;
        memset(rtcmSentence, '\0', 1030);
        rtcmSentence[0] = 0xd3;
        byteCount = 1;
        bytesLeftInFrame = 0xFFFF;          // Sentinel: length not known until 2 more header bytes arrive.
        return;
    }

    // --- Mid-frame: buffer the byte. ---
    if (byteCount < 1030 - 1) {             // Bounds check.
        rtcmSentence[byteCount] = inputChar;
    }
    byteCount++;

    // --- Compute payload length & remaining frame size from bytes 0-2. ---
    if ((byteCount == 3) && (bytesLeftInFrame == 0xFFFF)) {
        uint16_t payloadLen = ((uint16_t)((uint8_t)rtcmSentence[1] & 0x03) << 8) | (uint8_t)rtcmSentence[2];
        bytesLeftInFrame = payloadLen + 3;  // Remaining: payload + 3-byte CRC24Q.
        return;
    }

    // --- Continue filling header length bytes (byteCount 1 or 2). ---
    if (bytesLeftInFrame == 0xFFFF) {
        return;
    }

    // --- Count down payload+CRC. ---
    bytesLeftInFrame--;
    if (bytesLeftInFrame > 0) {
        return;
    }

    // --- Frame complete. ---
    rtcmSentenceCount++;
    msg_type = rtcm3GetMessageType(rtcmSentence);
    int64_t RTCMintervalUs = esp_timer_get_time() - lastRTCMtime;
    if (RTCMintervalUs > 0) {
        rtcmKbps = ((float)byteCount * 8.0f * 1000.0f) / (float)RTCMintervalUs;
    }
    if (commandFlag[DEBUG_RTCM]) {
        Serial.printf("RTCM3 (%s) active(%d) #%zu Type:%u bytes:%u kbps:%.2f\n",
            prfRtcmInSource, RTCMin, rtcmSentenceCount, msg_type, byteCount, rtcmKbps);
    }
    lastRTCMtime = esp_timer_get_time();
    // bytesLeftInFrame is already 0 - next byte in is treated as the next preamble.
}

/**
 * -------------------------------------------------------------------------
 *  Connect to NTRIP caster & validate credentials.
 * -------------------------------------------------------------------------
 *
 * Only called from taskRtcmRelay() (core 0) in response to ntripConnectRequest.
 * Credentials come from the active ntripCaster profile (NVS preferences).
 *
 * @return bool true if caster responded 200 OK, false otherwise.
 * @since  3.2.2 [2026-08-09-12:30pm] New.
 * @since  3.3.0 [2026-08-13-12:30pm] Replaced ntripPushGGA() with checkTimers().
 * @see    taskRtcmRelay(), checkTimers(), prefUtility(), Global vars: NTRIP client.
 * @link   https://github.com/sparkfun/SparkFun_u-blox_GNSS_Arduino_Library/blob/main/examples/ZED-F9P/Example17_NTRIPClient_With_GGA_Callback/Example17_NTRIPClient_With_GGA_Callback.ino.
 * @link   https://www.use-snip.com/kb/knowledge-base/ntrip-rev1-versus-rev2-formats/.
 * @link   https://www.use-snip.com/kb/knowledge-base/subtle-issues-with-using-ntrip-client-nmea-183-strings/.
 */
bool ntripBeginClient() {

    // --- Local vars. ---
    const uint16_t REQUEST_BUF_LEN  = 512;
    const uint16_t RESPONSE_BUF_LEN = 512;
    const int64_t  CASTER_TIMEOUT   = 5000000;                   // Time (us) to wait for caster response (5 sec).
    char           serverRequest[REQUEST_BUF_LEN]   = {'\0'};
    char           credentials[REQUEST_BUF_LEN]     = {'\0'};
    char           casterResponse[RESPONSE_BUF_LEN] = {'\0'};
    size_t         responseSpot     = 0;
    int            connectionResult = 0;
    int64_t        startTime;

    // --- Open socket. ---
    if (commandFlag[DEBUG_NTRIP]) {
        Serial.printf("Opening socket to %s:%u.\n", ntripCaster.url, ntripCaster.port);
    }

    // --- Socket connect failed. ---
    if (ntripClient.connect(ntripCaster.url, ntripCaster.port) == false) {
        strlcpy(ntripStatusMsg, "FAILED: connect to url:port.", sizeof(ntripStatusMsg));
        if (commandFlag[DEBUG_NTRIP]) {
            Serial.println(ntripStatusMsg);        
        }
        ntripStatusPending = true;
        return false;
    }
    if (commandFlag[DEBUG_NTRIP]) {
        Serial.printf("Connected to %s:%u.\n", ntripCaster.url, ntripCaster.port);
    }

    // --- Build GET request. ---
    snprintf(serverRequest, REQUEST_BUF_LEN,
        "GET /%s HTTP/1.0\r\nUser-Agent: NTRIP GhostRover Client v1.0\r\n", ntripCaster.mount);

    // --- Build & base64-encode credentials (Basic Auth), if provided. ---
    if (strlen(ntripCaster.user) == 0) {
        strlcpy(credentials, "Accept: */*\r\nConnection: close\r\n", sizeof(credentials));
    } else {
        char userCredentials[sizeof(ntripCaster.user) + sizeof(ntripCaster.pass) + 1];  // ':' takes a spot.
        snprintf(userCredentials, sizeof(userCredentials), "%s:%s", ntripCaster.user, ntripCaster.pass);
        base64 b;                                                                       // Built-in ESP32 lib returns String.
        String strEncodedCredentials = b.encode(userCredentials);                       // converted to char[] immediately below.
        char   encodedCredentials[strEncodedCredentials.length() + 1];
        strEncodedCredentials.toCharArray(encodedCredentials, sizeof(encodedCredentials));
        snprintf(credentials, sizeof(credentials), "Authorization: Basic %s\r\n", encodedCredentials);
    }
    strlcat(serverRequest, credentials, REQUEST_BUF_LEN);
    strlcat(serverRequest, "\r\n", REQUEST_BUF_LEN);

    // --- Send request. ---
    if (commandFlag[DEBUG_NTRIP]) {
        Serial.printf("Requesting mount point \"%s\".\n", ntripCaster.mount);
    }
    ntripClient.write(serverRequest, strlen(serverRequest));

    // --- Wait for response. ---
    startTime = esp_timer_get_time();       // Localized timeout watch, does not belong in checkTimers().
    while (ntripClient.available() == 0) {

        // -- Timed out waiting for caster's HTTP response. --
        if ((esp_timer_get_time() - startTime) > CASTER_TIMEOUT) {
            strlcpy(ntripStatusMsg, "FAILED: Time out.", sizeof(ntripStatusMsg));
            if (commandFlag[DEBUG_NTRIP]) {
                Serial.println(ntripStatusMsg);
            }
            ntripStatusPending = true;
            ntripClient.stop();
            return false;
        }
        vTaskDelay(1);                                            // Yield - runs inside taskRtcmRelay().
    }

    // --- Check reply: look for OK (200). Unauthorized is (401). ---
    while (ntripClient.available()) {
        if (responseSpot == RESPONSE_BUF_LEN - 1) {
            break;                                                // Bounds check.
        }
        casterResponse[responseSpot++] = ntripClient.read();
        if ((connectionResult == 0) && (strstr(casterResponse, "200") != NULL)) {
            connectionResult = 200;
        }
        if ((connectionResult == 0) && (strstr(casterResponse, "401") != NULL)) {
            connectionResult = 401;
        }
    }
    casterResponse[responseSpot] = '\0';

    if (connectionResult == 200) {

        // --- Success. ---
        strlcpy(ntripStatusMsg, "SUCCESS: ready to receive RTCM.", sizeof(ntripStatusMsg));
        if (commandFlag[DEBUG_NTRIP]) {
            Serial.println(ntripStatusMsg);
        }
        rtcmSentenceCount = 0;
        ntripStatusPending = true;
        return true;
    } else {

        // --- Something went wrong. ---
        snprintf(ntripStatusMsg, sizeof(ntripStatusMsg), "REJECTED: %s.\n", casterResponse);
        if (commandFlag[DEBUG_NTRIP]) {
            Serial.print(ntripStatusMsg);
        }
        ntripStatusPending = true;
        ntripClient.stop();
        return false;
    }
}

/**
 * =========================================================================
 *  Setup functions.
 * =========================================================================
 *
 * @since 3.0.11 [2026-01-08-10:30am] Browser initiated updates.
 * @since 3.2.3  [2026-08-10-09:45am] Add startTcpServer().
 * @since 3.3.1  [2026-08-17-09:00am] Changed position of startOutputs() in setup().
 * @see   startOutputs()         - Start serial & microSD card reader.
 * @see   buildInfo()            - Display build & processor info.
 * @see   prefUtility(PREF_INIT) - Preference utility (get preferences).
 * @see   startSerial()          - Start serial interfaces.
 * @see   initPins()             - Initialize pins & pin values.
 * @see   startI2C()             - Start I2C wire interfaces.
 * @see   startLiPo()            - Start LiPo I2C interface.
 * @see   startWiFiServer()      - Start WiFi server.
 * @see   startTcpServer()       - Start TCP server for GNSS Master (NMEA out / RTCM in bridge).
 * @see   startHttpServer()      - Start HTTP server.
 * @see   startWebSocketServer() - Start WebSocket server.
 * @see   startAndConfigGNSS()   - Start GNSS, config ZED settings.
 * @see   startQueues()          - Start GhostRover FreeRTOS queues.
 * @see   startTasks()           - Start GhostRover FreeRTOS tasks.
 * @see   preLoop()              - Prepare for loop().
 */

/**
 * -------------------------------------------------------------------------
 *  Start serial & microSD card reader.
 * -------------------------------------------------------------------------
 *
 * Using SanDisk 128GB ImageMate microSDXC UHS-1 - Up to 140MB/s.
 * 
 * Default pins for ESP32-S3 Thing Plus using Arduino core:
 *   GPIO 19 - Serial USB UART0 used as Communication Device Class interface D- (negative data line).
 *   GPIO 20 - Serial USB UART0 used as Communication Device Class interface D+ (positive data line).
 *   GPIO 33 - SDIO3.
 *   GPIO 34 - SDIO_CMD.
 *   GPIO 38 - SDIO_CLK.
 *   GPIO 39 - SDIO0.
 *   GPIO 40 - SDIO1.
 *   GPIO 47 - SDIO2.
 *   GPIO 48 - SDIO_~{DET}.
 * 
 * @return void  No output is returned.
 * @since  3.0.3  [2025-10-13-01:00pm].
 * @since  3.0.10 [2026-01-07-11:30am] Local vars.
 * @since  3.3.0  [2026-08-16-12:00pm] Refactored test to use log file.
 * @since  3.3.0  [2026-08-16-12:00pm] Changed from SD (SPI) to SD_MMC (SDIO).
 * @since  3.3.1  [2026-08-17-02:15pm] Refactored for logPrint().
 * @see    buildInfo(), setup().
 * @link   https://github.com/espressif/arduino-esp32/tree/master/libraries/SD_MMC.
 * @link   https://github.com/espressif/arduino-esp32/tree/master/libraries/FS.
 * @link   https://github.com/espressif/arduino-esp32/tree/master/libraries/SPI.
 * @link   https://docs.sparkfun.com/SparkFun_Thing_Plus_ESP32-S3/hardware_overview/#sd-card-slot.
 * @link   https://github.com/sparkfun/SparkFun_Thing_Plus_ESP32-S3/blob/main/Firmware/SD_SDIO_Benchmark/SD_SDIO_Benchmark.ino.
 * @link   https://github.com/espressif/arduino-esp32/blob/master/libraries/SD_MMC/examples/SDMMC_Test/SDMMC_Test.ino.
 * @link   https://randomnerdtutorials.com/arduino-ide-2-install-esp32-littlefs/.
 */
void startOutputs() {

    // --- Local vars. ---
    const uint8_t  SDIO_CLK         = 38;
    const uint8_t  SDIO_CMD         = 34;
    const uint8_t  SDIO_D0          = 39;
    const uint8_t  SDIO_D1          = 40;
    const uint8_t  SDIO_D2          = 47;
    const uint8_t  SDIO_D3          = 33;
    const uint32_t SERIAL_USB_SPEED = 115200;   // Serial USB speed.
    size_t remaining                = 0;        // Number of bytes remaining that can be written to char array. 
    bool   startedSd                = true;

    // --- Set initial state. ---
    ws2812LedColor = YELLOW;
    ws2812LedBlink = false;
    statusLedOn();
    memset(startOutputsResults, '\0', sizeof(startOutputsResults));

     // --- Start Serial USB. ---
    Serial.begin(SERIAL_USB_SPEED);
    if (Serial) {
        serialState[0] = 'u';   // USB interface is up.
        snprintf(startOutputsResults, sizeof(startOutputsResults), "Serial USB started @ %d.\n", SERIAL_USB_SPEED);
    } else {
        serialState[0] = 'd';   // USB interface is down.
        snprintf(startOutputsResults, sizeof(startOutputsResults), "USB not connected. Serial NOT started.\n");
    };

    // --- Assign SDIO pins for SD card reader. ---
    if (SD_MMC.setPins(SDIO_CLK, SDIO_CMD, SDIO_D0, SDIO_D1, SDIO_D2, SDIO_D3)) {
        remaining = sizeof(startOutputsResults) - strlen(startOutputsResults) - 1;
        strncat(startOutputsResults, "SDIO pins assigned.\n", remaining);
    } else {
        startedSd = false;
    }

    // --- Mount SD card. ---
    if (SD_MMC.begin()) {
        remaining = sizeof(startOutputsResults) - strlen(startOutputsResults) - 1;
        strncat(startOutputsResults, "SD card started.\n", remaining);
    } else {
        startedSd = false;
    };

    // --- Test SD card & reader by writing new LOG_FILE. ---
    if (SD_MMC.exists(LOG_FILE)) {                          // Create a fresh log file for every boot.
        SD_MMC.remove(LOG_FILE);
    }
    File file = SD_MMC.open(LOG_FILE, FILE_WRITE);          // FILE_WRITE to write to log file, FILE_APPEND to append to log file.
    if (file) {
        char diagMsg[100] = {'\0'};
        snprintf(diagMsg, sizeof(diagMsg), "Log file created.\n");
        remaining = sizeof(startOutputsResults) - strlen(startOutputsResults) - 1;
        strncat(startOutputsResults, diagMsg, remaining);
        file.print("#");
        file.close();
    } else  {
        startedSd = false;
    }

    // --- Error starting SD? ---
    if (!startedSd)  {
        ws2812LedColor = RED;
        ws2812LedBlink = true;
        statusLedOn();

        // -- SD card contains UI files, do not continue. --
        if (Serial) {
            Serial.print("ERROR SD card. Freezing.");
        }
        while (true);
    }

    // --- Continue. ---
    // Display startOutputsResults[] at end of buildInfo().
}

/**
 * -------------------------------------------------------------------------
 *  Build & processor info. Status LED is YELLOW then WHITE.
 * -------------------------------------------------------------------------
 * 
 * @return void  No output is returned.
 * @since  3.0.10 [2025-12-30-02:00pm].
 * @since  3.0.10 [2026-01-07-09:45am] Local vars.
 * @since  3.1.1  [2026-06-25-01:00pm] Updated version, added startup delay, transition LED YELLOW->WHITE.
 * @since  3.1.2  [2026-07-16-10:00am] Moved MAJOR, MINOR, PATCH from buildInfo() to "Operation" section.
 * @since  3.3.1. [2026-08-17-11:00am] Refactored. Renamed from showBuild() to buildInfo().
 * @since  3.3.1  [2026-08-17-02:15pm] Refactored for logPrint().
 * @see    startOutputs(), setup().
 * @link   https://github.com/pycom/pycom-esp-idf.
 */
void buildInfo() {

    // --- Local vars. ---
    const char NAME[] = "Ghost Rover 3";
    esp_chip_info_t chip_info;

    // --- Set build message. ---
    esp_chip_info(&chip_info);
    memset(buildInfoResults, '\0', sizeof(buildInfoResults));
    snprintf(buildInfoResults, sizeof(buildInfoResults),
        "\n%s\n"
        "%u.%u.%u - built on %s @ %s\n"
        "Using %s, Rev %d, %d core(s), ID (MAC) %012llX.\n",
        NAME,
        MAJOR_VERSION, MINOR_VERSION, PATCH_VERSION, __DATE__, __TIME__,
        ESP.getChipModel(), chip_info.revision, chip_info.cores, ESP.getEfuseMac()
    );

    // --- Continue. ---
    logPrint(buildInfoResults);
    logPrint(startOutputsResults);
    ws2812LedColor = WHITE;
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
 * @since  3.3.1  [2026-08-17-02:45pm] Changed from Serial.print() to logPrint().
 * @see    buildInfo(), setup().
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
    char diagMsg[100] = {'\0'};

    // --- Start serial interfaces. ---
    serialState[1] = '-';
    logPrint("Serial0 is not used.\n");

    if (strncmp(prfRtcmInSource, "radio", sizeof(prfRtcmInSource)) == 0) {
        Serial1.begin(SERIAL1_SPEED, SERIAL_8N1, HC12_TX, HC12_RX); // UART1 object. RX, TX.
        serialState[2] = 'u';
        snprintf(diagMsg, sizeof(diagMsg), "Serial1 started @ %i bps", SERIAL1_SPEED);
        logPrint(diagMsg);
    } else {        // prfRtcmInSource is other than radio.
        serialState[2] = '-';
        logPrint("Serial1 not started");
    }
    snprintf(diagMsg, sizeof(diagMsg), " (RTCM in = \"%s\" TCP).\n", prfRtcmInSource);
    logPrint(diagMsg);
    if (strncmp(prfRtcmInSource, "off", sizeof(prfRtcmInSource)) == 0) {
        RTCMin = false;
    }

    // -- RTCM out is always over Serial2 (to ZED UART2).
    Serial2.begin(SERIAL2_SPEED, SERIAL_8N1, ZED_RX2, ZED_TX2);     // UART2 object. RX, TX.  
    serialState[3] = 'u';
    snprintf(diagMsg, sizeof(diagMsg), "Serial2 started @ %i bps (RTCM out to ZED UART2).\n", SERIAL2_SPEED);
    logPrint(diagMsg);
}

/**
 * -------------------------------------------------------------------------
 *  Initialize pins modes & pin values.
 * -------------------------------------------------------------------------
 *
 * @return void No output is returned.
 * @since  3.0.3  [2025-10-13-01:00pm].
 * @since  3.0.10 [2025-12-27-06:00pm] Add HC12_SET & LSR_TRIGGER.
 * @since  3.3.1  [2026-08-17-02:45pm] Changed from Serial.print() to logPrint().
 * @see    setup().
 */
void initPins() {
    pinMode(HC12_SET, OUTPUT);          // HC-12 - set pin for AT command mode.
    digitalWrite(HC12_SET, HIGH);       // HC-12 - initially set pin for transparent mode.
    pinMode(LSR_TRIGGER, OUTPUT);       // KY-008 trigger pin.
    logPrint("Init HC-12 & laser pins.\n");
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
 * @since  3.3.1  [2026-08-17-02:45pm] Changed from Serial.print() to logPrint().
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
        logPrint("Wire & Wire1 started @ 4kHz.\n");
        i2cUp = true;
    } else {
        logPrint("Wire & Wire1 failed to start. Retrying.\n");
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
 * @since  3.3.1  [2026-08-17-02:45pm] Changed from Serial.print() to logPrint().
 * @see    setup().
 * @link   https://github.com/sparkfun/SparkFun_MAX1704x_Fuel_Gauge_Arduino_Library.
 */
void startLiPo() {
    if (lipo.begin() == false) {    // Uses I2C0.
        logPrint("LiPo not started. MAX17048 not detected.\n");
    } else {
        lipo.quickStart();          // Restart for a more accurate initial SOC guess.
        logPrint("LiPo started.\n");
    }
}

/**
 * -------------------------------------------------------------------------
 *  Start WiFi server.
 * -------------------------------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.7  [2025-11-20-12:30pm]. New.
 * @since  3.0.10 [2026-01-07-11:00am] Local vars.
 * @since  3.0.12 [2026-01-27-04:00pm] Refactor from AP mode to AP+Station mode.
 * @since  3.0.12 [2026-02-01-05:30pm] Use preferences.
 * @since  3.2.1  [2026-07-31-12:30pm] Add WiFi client for NTRIP access. Refactor.
 * @since  3.3.1  [2026-08-17-02:45pm] Changed from Serial.print() to logPrint().
 * @see    setup(), prefUtility().
 * @link   https://github.com/espressif/arduino-esp32/tree/master/libraries/WiFi.
 * @link   https://docs.espressif.com/projects/arduino-esp32/en/latest/api/wifi.html.
 * @link   https://docs.espressif.com/projects/arduino-esp32/en/latest/api/network.html.
 */
void startWiFiServer() {

    // --- Local Vars. ---
    const char AP_SSID[] = "Ghost Rover";                           // Local ESP32 Access Point (AP) network.
    const char AP_NAME[] = "ghost";                                 // AP name.
    const IPAddress AP_LOCAL_IP(192, 168, 23, 1);                   // AP host address.
    const IPAddress AP_GATEWAY(192, 168, 23, 1);                    // AP gateway address.
    const IPAddress AP_SUBNET(255, 255, 255, 0);                    // AP subnet mask.      

    // --- Set WiFi mode to WIFI_AP_STA for both WiFi server (WIFI_AP) & client (WIFI_STA). ---
    WiFi.mode(WIFI_AP_STA);

    // --- Config & start WiFi server (Access Point) for easy browser access. ---
    if (!WiFi.softAPConfig(AP_LOCAL_IP, AP_GATEWAY, AP_SUBNET)) {   // Configure IP network.
        logPrint("Soft AP - config failed.\n");
        while (true) {
            ws2812LedColor = RED;                                   // Indicates error during setup(). 
            ws2812LedBlink = false;
            statusLedOn();
        };
    }
    if (!WiFi.softAP(AP_SSID)) {                                    // Open access point - set SSID, omit password.
        logPrint("Soft AP - create failed. Freezing.");
        while (true) {
            ws2812LedColor = RED;                                   // Indicates error during setup(). 
            ws2812LedBlink = false;
            statusLedOn();
        };
    }
    WiFi.softAPsetHostname(AP_NAME);                                // Set hostname.
    WiFi.onEvent(onWiFiEvent);                                      // Add event handler WiFiEvent().
    IPAddress ip = WiFi.softAPIP();                                 // Start WiFi & check status (get IP).
    snprintf(localIp, sizeof(localIp), "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
    char diagMsg[100] = {'\0'};
    snprintf(diagMsg, sizeof(diagMsg), "WiFi server \"%s\" started @ %s.\n", AP_SSID, localIp);
    logPrint(diagMsg);
}

/**
 * -------------------------------------------------------------------------
 *  Start TCP server for GNSS Master (NMEA out / RTCM in bridge).
 * -------------------------------------------------------------------------
 *
 * Listens on both WiFi "server" and WiFi "client" interfaces.
 *
 * @return void No output is returned.
 * @since  3.2.3 [2026-08-10-10:45am] New.
 * @since  3.3.1 [2026-08-17-02:45pm] Changed from Serial.print() to logPrint().
 * @see    setup(), checkTcpClient().
 */
void startTcpServer() {
    gnssTcpServer.begin();
    gnssTcpServer.setNoDelay(true);                        // Disable Nagle - don't batch NMEA/RTCM bytes.
    char diagMsg[100] = {'\0'};
    snprintf(diagMsg, sizeof(diagMsg), "TCP server started on port %u.\n", TCP_SERVER_PORT);
    logPrint(diagMsg);
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
 * @since  3.3.1 [2026-08-17-02:45pm] Changed from Serial.print() to logPrint().
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
        char diagMsg[100]          = {'\0'};
        snprintf(diagMsg, sizeof(diagMsg), "httpServer - Page \"%s\" requested.\n", request->url().c_str());
        logPrint(diagMsg);
        request->send(SD_MMC, "/index.html", "text/html");      // Set root.
    }) ;

    // --- Route: file upload. ---   
    httpServer.on(PAGE_UPLOAD, HTTP_POST, [](AsyncWebServerRequest *req) {
        req->send(200, "text/plain", "Upload complete");
        logPrint("httpServer - File upload complete.\n");
    }, onHttpFileUpload);                                   // Register endpoint handler.

    // --- Route: file download. ---
    httpServer.on(PAGE_DOWNLOAD, HTTP_GET, [](AsyncWebServerRequest *request) { 
        if (request->hasParam("file")) {                    // Process request.
            String filename = request->getParam("file")->value();
            String filepath = "/" + filename;
            char diagMsg[100]          = {'\0'};
            if (SD_MMC.exists(filepath)) {
                request->send(SD_MMC, filepath, "application/octet-stream", true);
                snprintf(diagMsg, sizeof(diagMsg), "httpServer - Downloading file: %s\n", filename.c_str());
                logPrint(diagMsg);
            } else {
                request->send(404, "text/plain", "File not found");
                snprintf(diagMsg, sizeof(diagMsg), "File not found: %s\n", filename.c_str());
                logPrint(diagMsg);
            }
        } else {
            request->send(400, "text/plain", "File parameter required");
        }
    });

    // --- Start server. ---
    httpServer.serveStatic(PAGE_ROOT, SD_MMC, PAGE_ROOT);       // File system root ("/") is on SD card.
    httpServer.begin();
    logPrint("httpServer started.\n");
}

/**
 * -------------------------------------------------------------------------
 *  Start WebSocket server.
 * -------------------------------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.3  [2025-10-13-01:00pm].
 * @since  3.0.10 [2026-01-07-12:00pm] WEBSOCKET_SERVER_NAME.
 * @since  3.0.12 [2026-02-14-06:00pm] Add softwareResetGNSSOnly().
 * @since  3.3.1  [2026-08-17-02:45pm] Changed from Serial.print() to logPrint().
 * @see    setup(), onWebSocketEvent().
 * @link   https://randomnerdtutorials.com/esp32-websocket-server-arduino/.
 * @link   https://shawnhymel.com/1882/how-to-create-a-web-server-with-websockets-using-an-esp32-in-arduino/.
 */
void startWebSocketServer() {
    ws.onEvent(onWebSocketEvent);
    httpServer.addHandler(&ws);     // startWebServer() must run first.
    char diagMsg[100] = {'\0'};
    snprintf(diagMsg, sizeof(diagMsg), "WebSocket server \"%s\" started.\n", WEBSOCKET_SERVER_NAME);
    logPrint(diagMsg);
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
 * @since  3.3.1  [2026-08-17-02:45pm] Changed from Serial.print() to logPrint().
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

    // --- Local vars. ---
    char diagMsg[100] = {'\0'};

    // --- Start GNSS interface on I2C-1. ---
    if (roverGNSS.begin() == false) {
        logPrint("Start roverGNSS failed. Freezing ...");           // Something is wrong, freeze.
        ws2812LedColor = RED;
        ws2812LedBlink = false;
        statusLedOn();
        while (true);                                               // Infinite loop.
    } else {

        // -- Software reset. --
        roverGNSS.softwareResetGNSSOnly();
        logPrint("RoverGNSS started.\nEnumerating satellite constellations.\n");
        delay(1000); // Short delay to allow the module to complete the reset process.

        // uint16_t    prfGnsMsrInt;  // ZED: MEASURE every Y (e.g. 100) ms.
        // uint8_t     prfGnsNavRat;  // ZED: OUTPUT every X (e.g. 5) MEASURE intervals every (e.g. 5*100=500) ms.
        // roverGNSS.setNavigationFrequency(2) will produce 1 solution every 500ms, but only uses 2 (not 5) measurements per second.
        roverGNSS.setNavigationRate(prfGnsNavRat, VAL_LAYER_RAM);
        roverGNSS.setMeasurementRate(prfGnsMsrInt, VAL_LAYER_RAM);
        snprintf(diagMsg, sizeof(diagMsg), "Solution output every (%u * %u) ms.\n", prfGnsNavRat, prfGnsMsrInt);
        logPrint(diagMsg);
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
    roverGNSS.sendCfgValset() ? logPrint("roverGNSS configured using valset keys.\n") : logPrint("roverGNSS config failed!\n");

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
 * @since  3.3.1  [2026-08-17-02:45pm] Changed from Serial.print() to logPrint().
 * @see    setup(), onWebSocketEvent(), processJsonActivity().
 */
void startQueues() {
    wsRxQueue = xQueueCreate(WS_RX_QUEUE_LEN, sizeof(WsQueueItem));
    if (wsRxQueue == NULL) {
        logPrint("Failed to create wsRxQueue. Freezing.");
        ws2812LedColor = RED;
        ws2812LedBlink = false;
        statusLedOn();
        while (true);
    }
    logPrint("GhostRover FreeRTOS queue \"wsRxQueue\" created.\n");
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
 * @since  3.3.1  [2026-08-17-02:45pm] Changed from Serial.print() to logPrint().
 * @see    Global vars: FreeRTOS handles.
 * @see    setup().
 * @link   https://www.freertos.org/Documentation/02-Kernel/04-API-references/01-Task-creation/01-xTaskCreate.
 */
void startTasks() {

    // --- Loop status LED. ---
    xTaskCreate(taskLoopStatusLed, "Loop status LED", 2048, NULL, 2, &taskLoopStatusLedHandle);
    logPrint("GhostRover FreeRTOS task \"Loop status LED\" started.\n");

    // --- RTCM relay. ---
    // Arduino-ESP32 core 0 defaults: WiFi/BT.
    // Arduino-ESP32 core 1 defaults: Arduino loop(), WiFi/I2C.
    // Pin taskRtcmRelay() to core 0 for parallel execution instead of round-robin in loop() since I2C calls block and don't yield.
    if (strncmp(prfRtcmInSource, "off", sizeof(prfRtcmInSource)) != 0) {    // Relay RTCM, skip for "off".
        xTaskCreatePinnedToCore(taskRtcmRelay, "RTCM_Relay", 8192, NULL, 2, &taskRtcmRelayHandle, 0);
        logPrint("GhostRover FreeRTOS task \"RTCM relay\" started.\n");
    }
    if (strncmp(prfRtcmInSource, "off", sizeof(prfRtcmInSource)) != 0) {
        RTCMin = false;
    }
}

/**
 * -------------------------------------------------------------------------
 *  Prepare for loop().
 * -------------------------------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.7 [2025-11-21-06:00pm] Added inLoop.
 * @since  3.3.1 [2026-08-17-03:45pm] Changed from Serial.print() to logPrint().
 * @see    setup().
 */
void preLoop() {

    ws2812LedColor    = BLUE;
    ws2812LedBlink    = false;
    operMode[0]       = 'r';
    inLoop            = true;
    float seconds     = (esp_timer_get_time() - bootTime)/1000000.;
    char diagMsg[100] = {'\0'};
    snprintf(diagMsg, sizeof(diagMsg), "Loop starting. Boot time: %.2f seconds.\n", seconds);
    logPrint(diagMsg);
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
 * RTCM preamble = '11010011 000000xx' = 0xd3 0x00.  // ToDo: cleanup after new code inserted.
 *
 *  ESP32-S3 Serial1 (HC12) is set to 9,600 bps (default speed) in Global Vars.
 *  ESP32-S3 Serial2 (ZED UART2) is set to 57,600 bps in Global Vars.
 *  RTK-SMA (ZED UART2) is set to 57,600 bps by default (could change in startAndConfigGNSS() ).
 * 
 * Runs independently of loop() so blocking I2C calls in checkZedTriggerUpdate(). // ToDo: Update description.
 * (NMEA-over-I2C forwarding) can't starve the RTCM relay. Drains Serial1 fully
 * on every wake so any backlog from a stall clears immediately instead of
 * trickling out one byte per loop() pass.
 * 
 * RTCM types: 1004/1005/1006/1012/1019/1033/1074/1077/1084/1087/1094/1097/1124/1127/1230/4072.
 *
 * @param  void * pvParameters Pointer to FreeRTOS task parameters.
 * @return void   No output is returned (infinite loop).
 * @since  3.1.2  [2026-07-03-06:15pm] New, replaced relaySerial1toSerial2() in loop().
 * @since  3.1.2  Added if (byteCount < sizeof(rtcmSentence) - 1) to check for rtcmSentence overflow.
 * @since  3.2.1  [2026-07-29-09:30am] Added guard to prevent rtcmKbps form calculating as null.
 * @since  3.2.2  [2026-08-09-12:45pm] Add NTRIP client: add logic for "// prfRtcmInSource preference is set to "ntrip."
 * @since  3.2.3  [2026-08-10-10:30pm] Add bytesLeftInFrame, unify "radio" branch onto relayRtcmByte() helper.
 * @since  3.3.0 [2026-08-13-12:30pm] Replaced ntripPushGGA() with checkTimers().
 * @see    startTasks().
 * @see    rtcm3GetMessageType().
 * @see    Global vars: Serial, startSerialInterfaces(), loop().
 * @link   https://github.com/sparkfun/SparkFun_u-blox_GNSS_v3/blob/main/examples/ZED-F9P/Example3_StartRTCMBase/Example3_StartRTCMBase.ino.
 * @link   https://www.use-snip.com/kb/knowledge-base/an-rtcm-message-cheat-sheet/.
 * @link   https://www.use-snip.com/kb/knowledge-base/rtcm-3-message-list/.
 * @link   https://www.singularxyz.com/blog_detail/11.
 * @link   https://github.com/sparkfun/SparkFun_u-blox_GNSS_Arduino_Library/blob/main/examples/ZED-F9P/Example15_NTRIPClient/Example15_NTRIPClient.ino.
 * @link   https://github.com/sparkfun/SparkFun_u-blox_GNSS_Arduino_Library/blob/main/examples/ZED-F9P/Example17_NTRIPClient_With_GGA_Callback/Example17_NTRIPClient_With_GGA_Callback.ino.
 */
void taskRtcmRelay(void *pvParameters) {

    // --- Local vars. ---
    char     rtcmSentence[1030] = {'\0'};                               // RTCM3 sentence buffer.
    uint16_t byteCount          = 0;
    uint16_t bytesLeftInFrame   = 0;                                    // @since 3.2.4 - framing state for relayRtcmByte().
    uint16_t msg_type           = 0;

    // --- Loop. ---
    for (;;) {
        vTaskDelay(1);                                                  // Yield 1 tick when idle - keeps watchdog/other tasks fed.

        // -- prfRtcmInSource preference is set to "radio." --
        if (strncmp(prfRtcmInSource, "radio", sizeof(prfRtcmInSource)) == 0) {
            while (Serial1.available() > 0) {
                char inputChar = Serial1.read();
                relayRtcmByte(inputChar, rtcmSentence, byteCount, bytesLeftInFrame, msg_type);
            }
        }

        // -- prfRtcmInSource preference is set to "ntrip." --
        if (strncmp(prfRtcmInSource, "ntrip", sizeof(prfRtcmInSource)) == 0) {

            static int64_t lastNtripRtcmTime = 0;                        // Hangup timeout - persists across task passes.
            const  int64_t NTRIP_RTCM_TIMEOUT = 10000000;                // Time (us), matches Ex15/17's maxTimeBeforeHangup_ms.

            // - Handle connect/disconnect requests from browser (via processJsonActivity()). -
            if (ntripConnectRequest) {
                ntripConnectRequest = false;
                ntripConnected = ntripBeginClient();
                if (ntripConnected) {
                    lastNtripRtcmTime = esp_timer_get_time();
                }
            }
            if (ntripDisconnectRequest) {
                ntripDisconnectRequest = false;
                if (ntripClient.connected()) {
                    ntripClient.stop();
                }

                // Browser requested disconnect.
                ntripConnected = false;
                RTCMin = false;
                strlcpy(ntripStatusMsg, "DISCONNECTED: browser request.", sizeof(ntripStatusMsg));
                if (commandFlag[DEBUG_NTRIP]) {
                    Serial.println(ntripStatusMsg);
                }
                ntripStatusPending = true;
            }

            // - Relay RTCM & push GGA while connected. -
            if (ntripConnected) {
                if (ntripClient.connected()) {
                    while (ntripClient.available() > 0) {
                        char inputChar = ntripClient.read();
                        relayRtcmByte(inputChar, rtcmSentence, byteCount, bytesLeftInFrame, msg_type);
                        lastNtripRtcmTime = esp_timer_get_time();
                    }
                    if ((esp_timer_get_time() - lastNtripRtcmTime) > NTRIP_RTCM_TIMEOUT) {

                        // RTCM hangup timeout.
                        ntripClient.stop();
                        ntripConnected = false;
                        RTCMin = false;
                        strlcpy(ntripStatusMsg, "DISCONNECTED: RTCM timeout.", sizeof(ntripStatusMsg));
                        if (commandFlag[DEBUG_NTRIP]) {
                            Serial.println(ntripStatusMsg);
                        }
                        ntripStatusPending = true;
                    }
                } else {

                    // Caster closed the socket.
                    ntripConnected = false;
                    RTCMin = false;
                    strlcpy(ntripStatusMsg, "DISCONNECTED: dropped by caster.", sizeof(ntripStatusMsg));
                    if (commandFlag[DEBUG_NTRIP]) {
                        Serial.println(ntripStatusMsg);
                    }
                    ntripStatusPending = true;
                }
            }
        }

        // -- prfRtcmInSource preference is set to "bridge". --
        // RTCM from GNSS Master's own NTRIP client, over TCP).
        if (strncmp(prfRtcmInSource, "bridge", sizeof(prfRtcmInSource)) == 0) {
            if (tcpClientConnected) {
                while (gnssTcpClient.available() > 0) {
                    char inputChar = gnssTcpClient.read();
                    relayRtcmByte(inputChar, rtcmSentence, byteCount, bytesLeftInFrame, msg_type);
                }
            }
        }
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
 * @since  3.3.1 [2026-08-17-03:45pm] Changed from Serial.print() to logPrint().
 * @see    startWiFiServer().
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
        char diagMsg[100] = {'\0'};
        snprintf(diagMsg, sizeof(diagMsg), "[WiFi-event] Clients connected: %i\n", WiFi.softAPgetStationNum());
        logPrint(diagMsg);
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
 * @since  3.3.1  [2026-08-17-03:45pm] Changed from Serial.print() to logPrint().
 * @see    startHttpServer().
 * @link   https://randomnerdtutorials.com/esp32-async-web-server-espasyncwebserver-library/.
 */
void onHttpFileUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {

    // --- Local vars. ---
    static File uploadFile;                                 // HTTP upload file.
    char diagMsg[100] = {'\0'};

    // --- Begin. ---
    if (index == 0) {                                       // Start.
        logPrint("\nhttpServer endpoint \"/upload\".\nonHttpFileUpload() running.\n");
        SD_MMC.remove("/" + filename);                          // Delete file.
        snprintf(diagMsg, sizeof(diagMsg), "%s deleted on SD.\n", filename.c_str());
        logPrint(diagMsg);
        uploadFile = SD_MMC.open("/" + filename, FILE_WRITE);   // Open file for writing.
        if (uploadFile) {
            snprintf(diagMsg, sizeof(diagMsg), "%s opened on SD.\n", filename.c_str());
            logPrint(diagMsg);
        } else {
            request->send(500, "text/plain", "Cannot open file for writing on SD.");
            snprintf(diagMsg, sizeof(diagMsg), "Cannot open %s on SD for writing.\n", filename.c_str());
            logPrint(diagMsg);
            return;  
        }
    }

    // --- Continue (write data to SD). ---
    if (len) {                                              // Data chunk.                                            
        uploadFile.write(data, len);                        // Write received data to file.
        snprintf(diagMsg, sizeof(diagMsg), "%u total bytes written.\n", (unsigned int)(index + len));
        logPrint(diagMsg);
    }

    // --- Finish. ---
    if (final) {                                            // Complete.
        uploadFile.close();
        snprintf(diagMsg, sizeof(diagMsg), "%s closed on SD.\n", filename.c_str());
        logPrint(diagMsg);
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

    char diagMsg[100] = {'\0'};

    clientId = client->id();
    switch (type) {
        case WS_EVT_CONNECT:

            snprintf(diagMsg, sizeof(diagMsg), "WS #%u: %s connected to server.\n", clientId, client->remoteIP().toString().c_str());
            logPrint(diagMsg);
            ws2812LedColor = GREEN;                         // Loop status indicator LED.
            ws2812LedBlink = false;
            wsSendCount    = 0;                             // Reset counter.
            break;
        case WS_EVT_DISCONNECT:
            snprintf(diagMsg, sizeof(diagMsg), "WS #%u: disconnected.\n\n", clientId);
            logPrint(diagMsg);
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
                        logPrint("wsRxQueue full, message dropped.");
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
 * @since  3.2.1  [2026-07-30-10:30am] Global browserUpdatePendingFlag added.
 * @since  3.2.2  [2026-08-09-11:45am] Add NTRIP client: add char lastGGA[100].
 * @since  3.2.3  [2026-08-14-03:15pm] Refactor from Wire1 to TCP.
 * @see    nmeaBuffer[] in Operation section of Global vars.
 * @link   https://docs.espressif.com/projects/arduino-esp32/en/latest/api/wifi.html.
 * @link   https://github.com/sparkfun/SparkFun_u-blox_GNSS_v3/tree/main/examples/Basics/Example2_NMEAParsing.
 */
void DevUBLOXGNSS::processNMEA(char incoming) {

    // --- Local vars. ---
    // char nmeaBuffer[120] is a global var, holds (1) NMEA sentence.
    static size_t nmeaSentenceCounter  = 0;
    static size_t nmeaBytesCounter     = 0;
    int64_t       countBeginTime       = esp_timer_get_time();
    const  size_t nmeaSentenceCountMax = 80;

    // --- Loop. ---
    if (inLoop) {
        strncat(nmeaBuffer, &incoming, 1);                              // Add NMEA byte from RTK-SMA to outbound buffer.
        nmeaBytesCounter++;
        if ((incoming == '\n') && (nmeaBuffer[0] == '$')) {             // Full sentence.

            // ToDo: Modify the NMEA sentence here for height (+instrument) and position lock.

            // -- Output full NMEA sentence to TCP client (aka "GNSS Master" Android app).
            size_t bytesWritten = 0;
            if (tcpClientConnected) {
                bytesWritten = gnssTcpClient.write((const uint8_t*)nmeaBuffer, strlen(nmeaBuffer));
            }

            // -- Did TCP client receive the sentence? --
            if (bytesWritten > 0) {
                NMEAout = true;
            } else {
                NMEAout = false;
            }

            // -- Track stats for NMEA generated by ZED-F9P. ---
            nmeaSentenceCounter++;
            nmeaCountAll++;
            if (strncmp(&nmeaBuffer[3], "GGA", 3) == 0) {
                nmeaCountGGA++;
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
            if (zeroStatusCounters) {
                nmeaCountAll = 0; nmeaCountGGA = 0; nmeaCountRMC = 0; nmeaCountGSA = 0;
                nmeaCountGSV = 0; nmeaCountGST = 0; nmeaCountTXT = 0; nmeaCountOther = 0;
                zeroStatusCounters = false;
            }
            if (nmeaSentenceCounter == nmeaSentenceCountMax) {
                nmeaRate = nmeaBytesCounter / (esp_timer_get_time() - countBeginTime) * 8. * 1000000.;  // bps = (bytes/us) * 8bits/byte * 1000000us/1sec.
                nmeaBytesCounter    = 0;
                nmeaSentenceCounter = 0;
                countBeginTime      = esp_timer_get_time();
            }

            // -- Debug NMEA generated by ZED-F9P. ---
            if (commandFlag[DEBUG_NMEA_COUNTS]) {
                Serial.printf("All=%u, GGA=%u, RMC=%u, GSA=%u, GSV=%u, GST=%u, TXT=%u, other=%u.\n",
                    nmeaCountAll, nmeaCountGGA, nmeaCountRMC, nmeaCountGSA, nmeaCountGSV, nmeaCountGST, nmeaCountTXT, nmeaCountOther);
            }
            if (commandFlag[DEBUG_NMEA]) {
                if (strncmp("$GNGGA", nmeaBuffer, 6) == 0) {
                    Serial.print('\n');
                }
                Serial.printf("%u %s", nmeaCountAll, nmeaBuffer);
            }
            if (commandFlag[DEBUG_NMEA_HEX]) {
                if (strncmp("$GNGGA", nmeaBuffer, 6) == 0) {
                    Serial.println('\n');
                }
                Serial.printf("%u %s", nmeaCountAll, nmeaBuffer);
                for (int i = 0; i < strlen(nmeaBuffer); i++) {
                    Serial.printf("[\"%c\" 0x%02X] ", nmeaBuffer[i], nmeaBuffer[i]);
                }
                Serial.println('\n');
            }

            // -- If on NMEA page, save sentence for processJsonActivity() to send to browser. --
            if (strcmp(whichPage, "nmea") == 0) {
                strlcpy(lastNmea, nmeaBuffer, sizeof(lastNmea));
                browserUpdatePendingFlag = true;
            }

            // -- Clear the buffer. --
            memset(nmeaBuffer, '\0', sizeof(nmeaBuffer));

        }   // End of full sentence.
    }       // End of if in loop().
}

/**
 * =========================================================================
 *  Loop functions.
 * =========================================================================
 * 
 * @since 3.0.11 [2026-01-12-06:00pm] Browser initiated updates.
 * @since 3.3.0  [2026-08-13-12:00pm] Replaced checkZedTriggerUpdate() with checkTimers().
 * @see checkTimers()             - Check all process timers.
 * @see checkSerialUSB()          - Check serial USB for input.
 * @see debug()                   - Display debug.
 * @see checkGnssLockButton()     - Check GNSS lock button. // ToDo: Implement.
 * @see checkTcpClient()          - Check TCP server for new/dropped GNSS Master client.
 * @see ws.cleanupClients()       - HTTP WebSocket cleanup.
 */

 /**
 * -------------------------------------------------------------------------
 *  Check all processing timers.
 * -------------------------------------------------------------------------
 * 
 * // ToDo: DevUBLOXGNSS::processNMEA()  - NMEA setences on nme page
 * // ToDo: relayRtcmByte(), taskRTCMRelay(), 
 *   - FreeRTOS: send RTCM sentence count, TBD ...
 * 
 * Does not include timeouts, those stay in each specific function.
 *
 * @return void No output is returned.
 * @since  3.3.0 [2026-08-13-12:00pm] New. Replaces checkZedTriggerUpdate().
 * @since  3.3.0 [2026-08-13-03:30pm] Refactored.
 * @see    DevUBLOXGNSS::processNMEA().
 * @see    processJsonActivity().
 */
void checkTimers() {

    // --- Local vars. ---
    const  int64_t CHECK_UBLOX_WAIT   = (prfGnsNavRat * prfGnsMsrInt) * 1000;   // Convert from (ms) to (us), time between checkZedTriggerUpdate().
    const  int64_t SEND_GGA_INTERVAL  = 10000000;                               // Time (us) between GGA sends (10 sec, matches Ex17).
    const  int64_t DEBUG_INTERVAL     = 1000000;                                // Time (us) between debug() = (every 1 sec).
    static int64_t lastCheckUbloxTime = esp_timer_get_time();                   // Initialize only once, then persist.
    static int64_t lastSendGgaTime    = esp_timer_get_time();
    static int64_t lastDebugTime      = esp_timer_get_time();

    // --- Snapshot now time. ---
    int64_t espTime = esp_timer_get_time();

    // --- Check timer for operate and nmea pages. ---
    if ((strcmp(whichPage, "operate") == 0) || (strcmp(whichPage, "nmea") == 0)) {
        if ((espTime - lastCheckUbloxTime) > CHECK_UBLOX_WAIT) {

            // -- Debug timer. --
            if (commandFlag[DEBUG_TIMERS]) {
                Serial.printf("CHECK_UBLOX_WAIT (%lld) expired: espTime(%lld) - lastCheckUbloxTime(%lld) = %lld.\n",
                    CHECK_UBLOX_WAIT, espTime, lastCheckUbloxTime, (espTime-lastCheckUbloxTime));
            }

            // -- Reset last check time. --
            lastCheckUbloxTime = esp_timer_get_time();

            // -- Force ZED update. --
            roverGNSS.checkUblox();

            // --- Build data for operate page. ---
            if (strcmp(whichPage, "operate") == 0) {
                buildOperData();
            }

            // -- Send to browser. --
             browserUpdatePendingFlag = true;
        }
    }

    // --- Check timer for ntrip page - push last $GGA to caster. ---
    if (strcmp(whichPage, "ntrip") == 0) {

        // -- Only if preference set & a sentence is available & timer has expired. --
        if ((ntripCaster.sendGga == true) && (lastGGA[0] != '\0')) {
            if ((espTime - lastSendGgaTime) > SEND_GGA_INTERVAL) {

                // - Debug timer. -
                if (commandFlag[DEBUG_TIMERS]) {
                    Serial.printf("SEND_GGA_INTERVAL (%lld) expired: espTime(%lld) - lastSendGgaTime(%lld) = %lld.\n",
                    SEND_GGA_INTERVAL, espTime, lastSendGgaTime, (espTime-lastSendGgaTime));
                }
                
                // - Reset last send time. -
                lastSendGgaTime = esp_timer_get_time();

                // - Push GGA to caster. -
                ntripClient.print(lastGGA);
                if ((commandFlag[DEBUG_RTCM]) || (commandFlag[DEBUG_NTRIP])) {
                    Serial.printf("Pushed to NTRIP caster: %s", lastGGA);
                }

                // - Flag to send rtcmSentenceCount. -
                ntripsendRtcmSentenceCount = true;
                browserUpdatePendingFlag       = true;
            }
        }
    }

    // --- Check timer for debug serial print. ---
    if (debugFlag) {
        if ((espTime - lastDebugTime) > DEBUG_INTERVAL) {

            // - Debug timer. -
            if (commandFlag[DEBUG_TIMERS]) {
                Serial.printf("DEBUG_INTERVAL (%lld) expired: espTime(%lld) - lastDebugTime(%lld) = %lld.\n",
                DEBUG_INTERVAL, espTime, lastDebugTime, (espTime-lastDebugTime));
            }

            // - Reset last send time. -
            lastDebugTime = esp_timer_get_time();

            // - Debug. -
            debug();
        }
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
 *  7. If preferences changed, restart ESP32 (todoRestartGrMCU).
 *  8. sendDataToBrowser().
 *     8.1. Fill jsonDocToBrowser if browser page is constantly updated (operate, nmea, ...). 
 *     8.2  WebSocket send.
 *  9. If jsonDocFromBrowser["restartGR-MCU"], restart ESP32 (todoRestartGrMCU).
 * 10. If browserUpdatePendingFlag is true, sendDataToBrowser() & flip flag.
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
 *     2  = RTCM in source                  (char     prfRtcmInSource[6]).
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
 *     21 = Not used.
 *     22 = Not used.
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
 *     34 = WiFi hot spot address           (char     lhotspotIp[16]).
 *     35 = WebSocket client/session id     (uint8_t  clientId).
 *     36 = Instrument height               (uint16_t prfInstrHgt).
 *     37 = RTCM sentence count             (size_t   rtcmSentenceCount).
 *     38 = RTCM rate                       (float    rtcmKbps).
 *     39 = NTRIP caster #1 attributes      (char     prfNtripCastAttr[0][512]).
 *     40 = NTRIP caster #2 attributes      (char     prfNtripCastAttr[1][512]).
 *     41 = NTRIP caster #3 attributes      (char     prfNtripCastAttr[2][512]).
 *     42 = NTRIP caster active [1/2/3]     (char     prfNtripCastAct[2]).
 *     43 = NTRIP caster id                 (struct ntripCasterProfile ntripCaster.id      - uint8_t).
 *     44 = NTRIP caster name               (struct ntripCasterProfile ntripCaster.name    - char name[48]).
 *     45 = NTRIP caster url                (struct ntripCasterProfile ntripCaster.url     - char url[48]).
 *     46 = NTRIP caster mount point        (struct ntripCasterProfile ntripCaster.mount   - char mount[24]).
 *     47 = NTRIP caster port               (struct ntripCasterProfile ntripCaster.port    - uint16_t).
 *     48 = NTRIP caster version            (struct ntripCasterProfile ntripCaster.version - uint8_t).
 *     49 = NTRIP caster user               (struct ntripCasterProfile ntripCaster.user    - char user[48]).
 *     50 = NTRIP caster password           (struct ntripCasterProfile ntripCaster.pass    - char user[48]).
 *     51 = NTRIP caster sendGga            (struct ntripCasterProfile ntripCaster.sendGga - bool ).
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
 *       browser (sends)    --> {"page:"menu/nmea/files/config/operate/ntrip/rtcm/restart","sendPrefs":""}.
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
 *       browser (sends)    --> {"page":"menu","restartGR-MCU":""}.
 *       browser (receives) <-- {"restartGR-MCUResp":"GR-MCU will restart."}.
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
 *  -- NTRIP page. --
 *     - Hello: WiFi client already connected. -
 *       browser (sends)    --> {"page:"ntrip","sendPrefs":""}.
 *       browser (receives) <-- {"sendPrefsResp":"Preferences sent.",ALL PREFERENCES}.
 *       browser (receives) <-- {"connectWifiClientResp":"WiFi CONNECTED: x.x.x.x"}. If WiFi client already connected.
 * 
 *     - Hello: NTRIP client already connected. -
 *       browser (sends)    --> {"page:"ntrip","sendPrefs":""}.
 *       browser (receives) <-- {"sendPrefsResp":"Preferences sent.",ALL PREFERENCES}.
 *       browser (receives) <-- {"connectNtripCasterResp":"NTRIP CONNECTED: url:port @ mount"}.
 * 
 *     - Connect/disconnect WiFi client. -
 *       browser (sends)    --> {"connectWifiClient":""}.
 *       browser (receives) <-- {"connectWifiClientResp":"Connecting WiFi client"}.
 *       browser (receives) <-- {"connectWifiClientResp":"Connecting to SSID \"xxxx\""}.
 *       browser (receives) <-- {"connectWifiClientResp":"Attempt x of y"}. Repeat for each "x" of max "y" attempts.
 *       browser (receives) <-- {"connectWifiClientResp":"Connect ABORTED"}. Max attempts exceeded. Connect failed.
 *       browser (receives) <-- {"connectWifiClientResp":"CONNECTED as x.x.x.x"}. Connect success.
 *       browser (sends)    --> {"disconnectWifiClient":""}.
 *       browser (sends)    --> {"disconnectWifiClientResp":"WiFi client disconnected."}
 * 
 *     - Connect/disconnect to NTRIP caster. -
 *       browser (sends)    --> {"connectNtripCaster":""}.
 *       browser (receives) <-- {"connectNtripCasterResp":"Connecting to NTRIP caster \n name \n url:port \n mount (version x)"}.
 *       browser (receives) <-- {"connectNtripCasterResp":"FAILED: connect to url:port."} Attempt to connect to caster.url @ caster.port failed.
 *       browser (receives) <-- {"connectNtripCasterResp":"FAILED: Time out."} No reponse from caster within timeout window.
 *       browser (receives) <-- {"connectNtripCasterResp":"REJECTED: caster response."}. 
 *       browser (receives) <-- {"connectNtripCasterResp":"SUCCESS: ready to receive RTCM."}. Connect success.
 *       browser (sends)    --> {"disconnectNtripCaster":""}.
 *       browser (receives) <-- {"disconnectNtripCasterResp":"Disconnecting from NTRIP caster."}
 *
 *  -- Restart page. --
 *     - Hello. -
 *
 *     - Restart GR-MCU. -
 *       browser (sends)    --> {"page":"restart","restartGR-MCU":""}'.
 *       browser (receives) <-- {"restartGR-MCUResp":"GR-MCU will restart."}
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
 * @since 3.2.1  [2026-07-31-01:30pm] Moved "Wrap up" section from here to sendDataToBrowser().
 * @since 3.2.1  [2026-08-03-10:00am] Removed jsonObj["21'] & jsonObj["21'].
 * @since 3.2.2  [2026-08-09-01:00pm] Add NTRIP client: add NTRIP Connect/Disconnect logic, add logic for "// Step 3/3: Forward pending NTRIP status update to browser.""
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
 void  processJsonActivity() {      // From browser, to browser, periodic.

    // --- Debug. ---
    // Cannot use print()/println()/printf() to debug JSON doc value. Use these lines:
    // serializeJson(jsonDocToBrowser, Serial); // Debug.
    // Serial.println();

    // --- Local vars. ---
    // jsonDocFromBrowser, jsonDocToBrowser, &  JsonDocNtrip are global vars.
    char diagMsg[100] = {'\0'};
    WsQueueItem item;

    // --- If periodic status update is pending, send to browser page. ---
    if (browserUpdatePendingFlag) {
        memset(response, '\0', sizeof(response));
        jsonDocToBrowser.clear();       // Globals are set & will be sent. Ensure clean JSON doc for al periodic updates.
        if (ntripStatusPending) {
            jsonDocToBrowser["connectNtripCasterResp"] = ntripStatusMsg;
            ntripStatusPending = false;
        }
        sendDataToBrowser();
        browserUpdatePendingFlag = false;
        return;
    }


    // -------------------------------------------------------------------------
    // -- If incoming WebSocket message is NOT queued, return.
    // -------------------------------------------------------------------------
    if (xQueueReceive(wsRxQueue, &item, 0) != pdTRUE) {     // WebSocket message from browser?
        return;
    }

    // -- Debug. Print data received. --
    if (commandFlag[DEBUG_WS]) {
        Serial.printf("WS #%u: browser --> %s\n", clientId, item.data);
    }

    // -- WebSocket message - deserialize the JSON data into a JSON document (jsonDocFromBrowser). --
    jsonDocFromBrowser.clear();
    DeserializationError error = deserializeJson(jsonDocFromBrowser, item.data, item.len);

    // -- Begin. --
    if (error) {
        snprintf(diagMsg, sizeof(diagMsg), "JSON deserialize failed: %s\n", error.f_str());
        logPrint(diagMsg);
        return;
    }

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
        jsonDocToBrowser["2"]  = prfRtcmInSource;
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
        strlcpy(prfRtcmInSource, jsonDocFromBrowser["2"],  sizeof(prfRtcmInSource));
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
        File root = SD_MMC.open("/");
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
        strcpy(response, fileName);
        strcat(response, SD_MMC.remove(fileName) ? " deleted." : " NOT deleted.");
        // - Set response. -
        jsonDocToBrowser["deleteFileResp"] = response;
    }

    // -------------------------------------------------------------------------
    // -- Menu page. Restart GRMCU-1. --
    // -------------------------------------------------------------------------
    if (jsonDocFromBrowser["restartGR-MCU"].is<JsonVariant>()) {

        // - Set response. -
        strcpy(response, "GR-MCU will restart.");
        jsonDocToBrowser["restartGR-MCUResp"] = response;
        todoRestartGrMCU = true;
    }

    // -------------------------------------------------------------------------
    // -- NMEA page. NMEA sentences. --
    // -------------------------------------------------------------------------
    // loop() -> checkTimers() -> DevUBLOXGNSS::processNMEA() sets browserUpdatePendingFlag = true; -> sendDataToBrowser().

    // -------------------------------------------------------------------------
    // -- Operate page. GNSS data. --
    // -------------------------------------------------------------------------
    // loop() -> checkTimers() -> buildOperData() sets browserUpdatePendingFlag = true; -> sendDataToBrowser().

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
        snprintf(diagMsg, sizeof(diagMsg), "%s.\n", response);
        logPrint(diagMsg);
    }
    if (jsonDocFromBrowser["laserOff"].is<JsonVariant>()) {
        digitalWrite(LSR_TRIGGER, LOW);         // Turn laser off.

        // - Set response. -
        strcpy(response, "Laser off.");
        jsonDocToBrowser["laserOffResp"] = response;
        snprintf(diagMsg, sizeof(diagMsg), "%s.\n", response);
        logPrint(diagMsg);
    }

    // -------------------------------------------------------------------------
    // -- Operate page. Height lock/unlock button. --
    // -------------------------------------------------------------------------
    if (jsonDocFromBrowser["heightLock"].is<JsonVariant>()) {
        // ToDo: Implement.

        // - Set response. -
        strcpy(response, "Height locked.");
        jsonDocToBrowser["heightLockResp"] = response;
        snprintf(diagMsg, sizeof(diagMsg), "%s.\n", response);
        logPrint(diagMsg);
    }
    if (jsonDocFromBrowser["heightUnlock"].is<JsonVariant>()) {
        // ToDo: Implement.

        // - Set response. -
        strcpy(response, "Height unlocked.");
        jsonDocToBrowser["heightUnlockResp"] = response;
        snprintf(diagMsg, sizeof(diagMsg), "%s.\n", response);
        logPrint(diagMsg);
    }

    // -------------------------------------------------------------------------
    // -- Operate page. Position lock/unlock button. --
    // -------------------------------------------------------------------------
    if (jsonDocFromBrowser["positionLock"].is<JsonVariant>()) {
        // ToDo: Implement.

        // - Set response. -
        strcpy(response, "Position locked.");
        jsonDocToBrowser["positionLockResp"] = response;
        snprintf(diagMsg, sizeof(diagMsg), "%s.\n", response);
        logPrint(diagMsg);
    }
    if (jsonDocFromBrowser["positionUnlock"].is<JsonVariant>()) {
        // ToDo: Implement.

        // - Set response. -
        strcpy(response, "Position unlocked.");
        jsonDocToBrowser["positionUnlockResp"] = response;
        snprintf(diagMsg, sizeof(diagMsg), "%s.\n", response);
        logPrint(diagMsg);
    }

    // -------------------------------------------------------------------------
    // -- NTRIP page. Was on NTRIP page, connected WiFi client, left, returned, WiFi client still connected.
    // -------------------------------------------------------------------------
    if (strncmp(whichPage,"ntrip", sizeof(whichPage)) == 0) {
        if ((WiFi.status() == WL_CONNECTED) && (jsonDocFromBrowser["sendPrefs"].is<JsonVariant>())) {
            sendDataToBrowser();                                                    // Send prefs WS message.
            jsonDocToBrowser.clear();
            snprintf(response, sizeof(response), "WiFi CONNECTED: %s", hotspotIp);     // Second WS message. Triggers UI.
            jsonDocToBrowser["connectWifiClientResp"] = response;
        }
    }
    
    // -------------------------------------------------------------------------
    // -- NTRIP page. Connect WiFi client. --
    // -------------------------------------------------------------------------
    if (jsonDocFromBrowser["connectWifiClient"].is<JsonVariant>()) {

        // - Local vars. --
        size_t numWifiTrys = 1;                         // Count connect attempts to STA_SSID.
        size_t maxWifiTrys = 10;                        // Max # of trys to connect to STA_SSID, one per second.
        IPAddress STA_IP(172, 20, 10, 2);               // Request this IP address.

        // - Begin. -
        strcpy(response, "Connecting WiFi client");
        jsonDocToBrowser["connectWifiClientResp"] = response;
        sendDataToBrowser();

        snprintf(response, sizeof(response), "Connecting to SSID \"%s\"", prfHotSsi); 
        jsonDocToBrowser["connectWifiClientResp"] = response;
        sendDataToBrowser();

        // - Configure & start WiFi client for RTCMin via Internet NTRIP caster. -
        WiFi.disconnect();
        WiFi.begin(prfHotSsi, prfHotPas);
        for (numWifiTrys; numWifiTrys <= maxWifiTrys; numWifiTrys++) {
            snprintf(response, sizeof(response), "Attempt %d of %d", numWifiTrys, maxWifiTrys);
            jsonDocToBrowser["connectWifiClientResp"] = response;
            sendDataToBrowser();
            if (WiFi.status() == WL_CONNECTED) {
                break;
            }
            delay(1000);                                // Try again.
        }
        if (WiFi.status() == WL_CONNECTED) {
            strlcpy(hotspotIp, WiFi.localIP().toString().c_str(), sizeof(hotspotIp));
            snprintf(response, sizeof(response), "WiFi CONNECTED:  %s", hotspotIp);
            jsonDocToBrowser["connectWifiClientResp"] = response;
            ws2812LedColor = WHITE;                             // Indicates no error during setup(). 
            ws2812LedBlink = false;
            statusLedOn();
        } else {
            snprintf(response, sizeof(response), "Connect ABORTED");
            jsonDocToBrowser["connectWifiClientResp"] = response;
            memset(hotspotIp, '\0', sizeof(hotspotIp));
            WiFi.disconnect();
        }
    }

    // -------------------------------------------------------------------------
    // -- NTRIP page. Disconnect WiFi client. --
    // -------------------------------------------------------------------------
    if (jsonDocFromBrowser["disconnectWifiClient"].is<JsonVariant>()) {
        WiFi.disconnect();
        strcpy(response, "WiFi client disconnected.");
        jsonDocToBrowser["disconnectWifiClientResp"] = response;
    }

    // -------------------------------------------------------------------------
    // -- NTRIP page. Was on NTRIP page, connected to NTRIP caster, left, returned, NTRIP caster still connected.
    // -------------------------------------------------------------------------
    if (strncmp(whichPage,"ntrip", sizeof(whichPage)) == 0) {
        if ((WiFi.status() == WL_CONNECTED) && (ntripClient.connected()) && (jsonDocToBrowser["connectWifiClientResp"].is<JsonVariant>())) {
            snprintf(response, sizeof(response), "NTRIP CONNECTED: %s:%d@ %s ", ntripCaster.url, ntripCaster.port, ntripCaster.mount);  // Trigger UI.
            jsonDocToBrowser["connectNtripCasterResp"] = response;
            ntripConnected = true;
            ntripConnectRequest = false;  // taskRtcmRelay() picks this up next pass.
        }
    }

    // -------------------------------------------------------------------------
    // -- NTRIP page. Connect to NTRIP caster. --
    // -------------------------------------------------------------------------
    if (!ntripClient.connected() && jsonDocFromBrowser["connectNtripCaster"].is<JsonVariant>()) {
        snprintf(response, sizeof(response), "Connecting to NTRIP caster\n%s\n%s:%d\n%s (version %d)",
            ntripCaster.name, ntripCaster.url, ntripCaster.port, ntripCaster.mount, ntripCaster.version);
        jsonDocToBrowser["connectNtripCasterResp"] = response;
        ntripConnected = false;
        ntripConnectRequest = true;  // taskRtcmRelay() picks this up next pass.
    }

    // -------------------------------------------------------------------------
    // -- NTRIP page. Disconnect NTRIP caster. --
    // -------------------------------------------------------------------------
    if (jsonDocFromBrowser["disconnectNtripCaster"].is<JsonVariant>()) {
        strcpy(response, "Disconnecting from NTRIP caster.");
        jsonDocToBrowser["disconnectNtripCasterResp"] = response;
        ntripDisconnectRequest = true;
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

    // -------------------------------------------------------------------------
    // -- Send data to browser
    // -------------------------------------------------------------------------
    sendDataToBrowser();
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
 * @since  3.3.0  [2026-08-13-01:30pm] Added logic for global debugFlag.
 * @see    loop().
 */
void checkSerialUSB() {

    if (Serial.available() == 0) {                      // No serial USB input.
        return;
    }

    // --- Local vars. ---
    static size_t posn        = 0;                      // Input position for command buffer.
    static char   command[20] = {'\0'};                 // Serial USB command buffer.
    static char   inputChar   = '\0';

    // --- Fill command buffer. ---
    while ((Serial.available() > 0) )  {
        inputChar = Serial.read();                      // Read char from USB Serial.
        if ((inputChar != '\n') && (inputChar != '\r')) {
            command[posn] = inputChar;                  // Add input to buffer.
            posn++;
        }
    }

    // --- Process command. ---
    if (inputChar == '\n')  {
        if ((command[0]) == '?') {                      // List commands.
            Serial.print("\nGR-MCU:\n\"?\" Print commands.\n\"!\" Disable all debug.\n\"z\" Zero status counters.\nCommands:");
            for (size_t i = 0; i <= NUM_COMMANDS-1; i++) {
                Serial.printf(" %s", COMMAND[i]);
            }
            Serial.println('.');
        } else if ((command[0]) == '!') {               // Disable all debugs.
            for (size_t i = 0; i <= NUM_COMMANDS; i++) {
                commandFlag[i] = false;
            }
            Serial.println("All debug disabled.");
            debugFlag = false;
        } else if ((command[0]) == 'z') {               // Zero all status counters.
            Serial.println("Zero all counters.");
            zeroStatusCounters = true;
        } else {                                        // Possible command.
            size_t i;
            for (i = 0; i < NUM_COMMANDS; i++) {
                if (strcmp(COMMAND[i], command) == 0) {
                    break;
                }
            }
            if (i == NUM_COMMANDS) {                    // Invalid command.
                Serial.printf("%s is not a command. \n", command);
            } else {
                commandFlag[i] = !commandFlag[i];       // Toggle the specific debug flag.
                Serial.printf("%s %s\n", COMMAND[i], (commandFlag[i]  ? "enabled." : "disabled."));
                commandFlag[i] ? (debugFlag = true) : (debugFlag = false);  // Set the global debug flag.
            }
        }
        posn = 0;                                       // Prepare for next command.
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
 *  Check TCP server for new/dropped GNSS Master client.
 * -------------------------------------------------------------------------
 *
 * Single-client design: an incoming connection always replaces whatever
 * client is currently held, so a reconnect (e.g. phone WiFi toggled)
 * doesn't get stuck behind a dead socket.
 *
 * @return void No output is returned.
 * @since  3.2.2 [2026-08-10-10:15am] New.
 * @see    loop(), startTcpServer().
 */
void checkTcpClient() {

    char diagMsg[100] = {'\0'};

    // --- Accept new client, replacing any existing one. ---
    if (gnssTcpServer.hasClient()) {
        if (gnssTcpClient) {
            gnssTcpClient.stop();                          // Drop old client.
        }
        gnssTcpClient = gnssTcpServer.available();
        gnssTcpClient.setNoDelay(true);
        tcpClientConnected = true;
        snprintf(diagMsg, sizeof(diagMsg), "TCP client connected: %s\n", gnssTcpClient.remoteIP().toString().c_str());
        logPrint(diagMsg);
    }

    // --- Detect disconnect. ---
    if (tcpClientConnected && !gnssTcpClient.connected()) {
        tcpClientConnected = false;
        gnssTcpClient.stop();
        NMEAout = false;
        logPrint("TCP client disconnected.");
    }
}

/**
 * -------------------------------------------------------------------------
 *  "To do tasks" flagged in other functions or FreeRTOS tasks.
 * -------------------------------------------------------------------------
 *
 * @return void No output is returned.
 * @since  3.3.0 [2026-08-15-12:00pm] New.
 * @see    loop().
 */
void checkToDoFlags() {

    // --- Restart MCU. ---
    if (todoRestartGrMCU) {
        delay(2000);
        esp_restart();
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
 * @since  3.3.0  [2026-08-13-01:45pm] Move timer logic to checkTimers().
 * @see    checkSerialUSB().
 */
void debug() {

    if (!Serial) {                                          // Nothing to see, move on.
        return;
    }

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
            serialState[0], serialState[1], serialState[2], serialState[3]);
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
        int32_t seconds = (esp_timer_get_time() - bootTime)/1000000;
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

    // --- NTRIP. ---
    // @see "if (commandFlag[DEBUG_NTRIP])" in ntripBeginClient(), called from taskRtcmRelay() FreeRTOS task.
}

/**
 * =========================================================================
 *  Setup.
 * =========================================================================
 *
 * @since  3.0.3 [2025-10-13-01:00pm] New.
 * @since 3.2.3  [2026-08-10-09:45am] Add startTcpServer().
 * @since 3.3.0  [2026-08-17-09:00am] Changed position of startOutputs() in setup().
 * @see    Global vars.
 */
void setup() {
    startOutputs();             // Start serial & microSD card reader.
    buildInfo();                // Build & processor info.
    prefUtility(PREF_INIT);     // Get preferences.
    startSerial();              // Start serial interfaces.
    initPins();                 // Initialize pin modes & pin values.
    startI2C();                 // Start I2C wire interfaces.
    startLiPo();                // Start LiPo I2C interface.
    startWiFiServer();          // Start WiFi server.
    startTcpServer();           // Start TCP server for GNSS Master (NMEA out / RTCM in bridge).
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
 * @since 3.3.0  [2026-08-13-12:00pm] Replaced checkZedTriggerUpdate() with checkTimers().
 * @since  3.3.0 [2026-08-13-01:00pm] Replaced debug timer with checkTimers().
 * @see   startTasks().
 * @see   GhostRover FreeRTOS functions.
 * @see   Event handlers.
 */
void loop() {
    // *** Proposed sequence. ***
    // checkTimeOuts();           // ntripBeginClient() timeout, taskRtcmRelay() timeout, relayRtcmByte() timeout.
    // processJsonIn();           //  Event based (queued WebSocket message from browser).
    // processJsonOut();       
    // sendToBrowser();           // Send out JSON if flags are set.

    processJsonActivity();      // Process queued WS messages & pending status updates. All JSON activity lives here.
    checkTimers();              // Check all processing timers.
    checkSerialUSB();           // Check serial USB for input.
    checkToDoFlags();           // "To do tasks" flagged in other functions or FreeRTOS tasks.
    // checkGnssLockButton();   // Check GNSS lock button.  // ToDo: Implement.
    checkTcpClient();           // Check TCP server for new/dropped client (GNSS Master, ..).
    ws.cleanupClients();        // HTTP WebSocket cleanup.
    vTaskDelay(1);
}
