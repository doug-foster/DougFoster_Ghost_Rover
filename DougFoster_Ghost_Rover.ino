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
 * @since  3.2.1 [2026-07-31-09:30am] Add Internet for NTRIP access.
 * @since  3.2.1 [2026-08-02-04:30pm] Remove connectInternet().
 * @since  3.2.1 [2026-08-03-10:00am] Removed jsonObj["21'] & jsonObj["21'].
 * @since  3.2.2 [2026-08-09-11:45am] Add NTRIP client.
 * @since  3.2.2 [2026-08-09-02:00pm] Updated platform from 3.3.10 to 3.3.11. Updated AsyncTCP & ESPAsyncWebServer libraries.
 * @since  3.2.2 [2026-08-09-05:30pm] Only print for DEBUG_WS_EVENTS.
 * @since  3.2.3 [2026-08-10-09:45am] Add TCP to replace BLE.
 * @since  3.2.3 [2026-08-10-10:15pm] Refactor rtcm3GetMessageType(), relayRtcmByte(), & taskRtcmRelay() to properly handle RTCM sentences.
 * @since  3.3.0 [2026-08-13-12:00pm] Replaced checkZedTriggerUpdate() with checkLoopTimers().
 * @since  3.3.0 [2026-08-13-12:30pm] Replaced ntripPushGGA() with checkLoopTimers().
 * @since  3.3.0 [2026-08-13-01:00pm] Replaced debug timer with checkLoopTimers().
 * @since  3.3.0 [2026-08-17-09:15am] Refactored card test in startOutputs() to use log file. Changed position in setup().
 * @since  3.3.1 [2026-08-16-12:00pm] Changed from SD (SPI) to SD_MMC (SDIO).
 * @since  3.3.1 [2026-08-16-05:30pm] Add logPrint().
 * @since  3.3.1 [2026-08-28-06:30pm] Add logic to manage nmeaBuffer if buffer gets out of sync.
 * @since  3.3.2 [2026-08-30-04:45pm] Moved char diagMsg[] to global vars.
 * @since  3.3.3 [2026-08-31-05:00pm] More cleanup.
 * @since  3.3.4 [2026-09-07-11:45am] Cleanup & memory management.
 * @since  3.3.5 [2026-09-07-01:15pm] Removed checkLoopTimers() in loop(), replaced with FreeRTOS tasks.
 * @since  3.4.0 [2026-09-13-05:30pm] Height/position lock/unlock.
 * @since  3.4.0 [2026-09-20-05:30pm] Add prfLckAvgInt.
 * @since  3.4.1 [2026-09-27-10:00pm] Added caster.crs.
 * @since  3.4.1 [2026-09-28-12:30pm] ntripCasterProfile.crs from char[24] to char[48].
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
 * --- // ToDo: ---
 *     - Update RTKEverywhere for base station.
 *     - Add RTCM page.
 *     - Operate.js/operate.html page - add ability to select coordinates (lat/lon, ECEF, UTM northing & easting)  
 */

/**
 * -------------------------------------------------------------------------
 *  Code structure.
 * -------------------------------------------------------------------------
 * 
 * @since 3.1.1 [2026-06-25-01:00pm] New.
 * @since 3.1.2 [2026-07-03-06:15pm] New, GhostRover FreeRTOS task taskRtcmRelay() replaced relaySerial1toSerial2() in loop().
 * @since 3.2.1 [2026-07-30-07:45am] Implement GhostRover FreeRTOS queues: refactor onWebSocketMessage() into processMessagesIn().
 * @since 3.2.2 [2026-08-09-11:45am] Add NTRIP client: add relayRtcmByte(), add ntripBeginClient(), & ntripPushGGA().
 * @since 3.2.3 [2026-08-10-09:45am] Add startTcpServer(), add checkTCPServer().
 * @since 3.3.1 [2026-08-17-09:15am] Changed position of startOutputs() in setup().
 * @since 3.3.1 [2026-08-16-05:30pm] Add logPrint().
 * @since 3.3.2 [2026-08-30-03:30pm] Add taskBuildOperData().
 * @since 3.4.0 [2026-09-12-07:15pm] Height/position lock/unlock: add nmeaChecksum(), nmeaSetFields(), decimalDegreesToNmea(), substituteLockedNmea().
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
 *      -- uptime()                    - Display time since boot.
 *      -- logPrint                    - Save a message to log.txt, print to Serial if available.
 *      -- statusLedOn()               - Turn on status LED.
 *      -- prefUtility()               - Preference utility.
 *      -- buildOperData()             - Build data for operate page.
 *      -- sendDataToBrowser()         - Send data to browser.
 *      -- rtcm3GetMessageType()       - Return RTCM3 message type to taskRtcmRelay().
 *      -- relayRtcmByte()             - Relay RTCM byte to Serial2 (ZED UART2), tracking stats.
 *      -- nmeaChecksum()              - Compute NMEA checksum (XOR of all bytes between '$' and '*').
 *      -- nmeaSetFields()             - Replace comma-delimited fields in NMEA sentence, recompute and append checksum.
 *      -- decimalDegreesToNmea()      - Convert decimal degrees to NMEA ddmm.mmmmmmm / dddmm.mmmmmmm format.
 *      -- substituteLockedNmea()      - Substitute locked position/height values into full NMEA sentence.
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
 *      -- taskBuildOperData()         - GhostRover FreeRTOS task - Build data for operate page, trigger DevUBLOXGNSS::processNMEA().
 *
 *  --- Event handlers for core/additional library processes. ---
 *      -- onWiFiEvent()               - <WiFi.h> & <WiFiAP.h> WiFi event handler (WiFiEvent_t).
 *      -- onHttpFileUpload()          - <ESPAsyncWebServer.h> HTTP endpoint ("/upload") event handler (AsyncWebServerRequest).
 *      -- onWebSocketEvent()          - <ESPAsyncWebServer.h> WebSocket event handler (AsyncWebSocket).
 *      -- DevUBLOXGNSS::processNMEA() - <SparkFun_u-blox_GNSS_v3.h> DevUBLOXGNSS::processNMEA event handler (char incoming).
 *
 *  --- Loop functions. ---
 *      -- processMessagesIn()         - Process queued WS messages. All JSON activity lives here & sendDataToBrowser().
 *      -- checkSerialUSB()            - Check serial USB for input.
 *      -- checkTCPServer()            - Check TCP server for new/dropped NMEA client.
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
 * @since 3.2.1 [2026-07-30-07:45am] Implement FreeRTOS queues: refactor onWebSocketMessage() into processMessagesIn().
 * @since 3.2.3 [2026-08-10-09:45am] Add startTcpServer(), add checkTCPServer().
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
 *     processMessagesIn()          - @see "Operation summary" in description for processMessagesIn().
 *       - Process one incoming WebSocket message, if queued.
 *         - Remove message from queue.
 *         - Deserialize JSON.
 *         - Set page name.
 *         - Depending on page (config, files, nmea, operate, ntrip, ...):
 *           - Fill jsonDocToBrowser[] with page specific data. Run specific functions for some pages.
 *              - If "nmea" page, do nothing. Processing is loop() -> checkLoopTimers() -> DevUBLOXGNSS::processNMEA().
 *           - Send data to browser.
 *           - If periodic status update is pending, send to browser page.
 *           - If NTRIP status update is pending, send to browser page.
 * 
 *     checkSerialUSB()               - Check serial USB for input.
 *     checkTCPServer()               - Check TCP server for new/dropped client.
 *     ws.cleanupClients()            - HTTP WebSocket cleanup.
 *     debug()                        - Display debug.
 *
 * --- GhostRover FreeRTOS functions. ---
 *     taskLoopStatusLed()            - GhostRover FreeRTOS task - Set Loop() status LED to blink or solid.
 *     taskBuildOperData()            - GhostRover FreeRTOS task - Build data for operate page, trigger DevUBLOXGNSS::processNMEA().
 *     taskRtcmRelay()                - GhostRover FreeRTOS task - Relay RTCM from source to -> Serial2 (ZED UART2).
 *     rtcm3GetMessageType()          - Called by taskRtcmRelay - return RTCM3 message type.
 *     relayRtcmByte()                - Called by taskRtcmRelay - read byte from NTRIP client, write to Serial2 (ZED UART2).
 *     ntripBeginClient()             - Called by taskRtcmRelay - connect to NTRIP caster.
 *
 * --- Event handlers for core/additional library processes. ---
 *     -- onWiFiEvent()               -- <WiFi.h> & <WiFiAP.h> WiFi event handler (WiFiEvent_t).
 *        - if commandFlag[DEBUG_WIFI_EVENTS]), print WiFi status.
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
 *        - solid  WHITE: setup() started & running ok.
 *        - solid YELLOW: startup delay.
 *        - solid    RED: startOutputs() error,
 *                        startI2C error,
 *                        startWiFiServer() error,
 *                        startAndConfigGNSS() error.
 *                        startQueues() error.
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
 * @see comments in processMessagesIn().
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
#include <esp_wifi.h>                                      // https://github.com/pycom/pycom-esp-idf.
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
 * @since 3.1.2  [2026-07-16-09:00am] Changed int16_t prfInstrHgt to uint16_t.
 * @since 3.1.2  [2026-07-16-10:00am] Moved MAJOR, MINOR, PATCH from buildInfo() to "Operation" section.
 * @since 3.2.1  [2026-07-24-03:30pm] Refactor JSON.
 * @since 3.2.1  [2026-07-28-04:45pm] Removed NMEA out switch & preference.
 * @since 3.2.2  [2026-08-09-11:45am] Add NTRIP client.
 * @since 3.2.3  [2026-08-10-09:45am] Add TCP to replace BLE.
 * @since 3.3.0  [2026-08-13-01:30pm] Added global debugFlag.
 * @since 3.3.1  [2026-08-16-05:30pm] Add LOG_FILE, outputBuffer.
 * @since 3.3.1  [2026-08-27-02:00pm] Add PREF_RESET_NO_REBOOT.
 * @since 3.3.1  [2026-08-29-03:00pm] New debug vars. Remove CHECK_WIRE1.
 * @since  3.4.0 [2026-09-12-07:15pm] Height/position lock/unlock.
 * @since  3.4.0 [2026-09-20-05:30pm] Add prfLckAvgInt.
 * @since  3.4.1 [2026-09-27-10:00pm] Added ntripCaster.crs.
 * @since  3.4.1 [2026-09-28-12:30pm] ntripCasterProfile.crs from char[24] to char[48].
 */

// --- Increase default ESP32 stack size from 8KB to 16KB.
SET_LOOP_TASK_STACK_SIZE(16 * 1024); 

// --- Pin assignments. ---
const uint8_t HC12_SET    = 7;                                  // HC-12 SET {blue wire}.
const uint8_t LSR_TRIGGER = 15;                                 // KY-008 trigger pin {yellow wire}.

// --- LED. ---
const uint8_t LED_BRIGHT     = 50;                              // 0-255. taskLoopStatusLed()
      bool    ws2812LedBlink = false;
      enum    ws2812_LED_COLOR {                                // WS2812 RGB STAT LED.
          OFF,
          RED,
          YELLOW,
          GREEN,
          BLUE,
          WHITE
      } ws2812LedColor;

// --- Battery. ---
SFE_MAX1704X lipo(MAX1704X_MAX17048);                           // LiPo battery.

// --- WiFi. ---
char localIp[16];
char hotspotIp[16];

// --- TCP (GNSS Master: NMEA out, RTCM in for "bridge" mode). ---
const uint16_t   TCP_SERVER_PORT    = 9099;                     // Remote app like "GNSS Master" connects here: always send NMEA out, can receive RTCM in.
      WiFiServer tcpServer(TCP_SERVER_PORT);                    // TCP server object.
      WiFiClient tcpClient;                                     // TCP client. Only one supported.

// --- HTTP. ---
const char           WEBSOCKET_SERVER_NAME[] = "/ghostRover";
      uint8_t        clientId                = 0;               // HTTP WebSocket client ID # (+1 for each new connection).
      AsyncWebServer httpServer(80);                            // HTTP AsyncWebServer object on port 80.
      AsyncWebSocket ws(WEBSOCKET_SERVER_NAME);                 // HTTP WebSocket object.

// --- WebSocket. ---
const uint8_t      WS_RX_QUEUE_LEN = 5;                         // Max # of WebSocket queued incoming messages.
      char         lastNmea[120]   = {'\0'};                    // Snapshot of last complete NMEA sentence. @see DevUBLOXGNSS::processNMEA(), sendDataToBrowser().
      JsonDocument jsonDocToBrowser;                            // JSON document - send to browser. Used in processMessagesIn(), buildOperData(), & DevUBLOXGNSS::processNMEA().
      JsonDocument jsonDocFromBrowser;                          // JSON document - received from browser. Used in processMessagesIn(). 
      JsonDocument JsonDocNtrip;                                // JSON document - JSON NTRIP data inside jsonDocToBrowser or jsonDocFromBrowser.
struct WsQueueItem {                                            // Queued incoming WebSocket message.
    char   data[2048];                                          // Raw JSON text 2x jsonBuffer[1024] in sendDataToBrowser(). Escaped NTRIP JSON attributes can run larger than outbound buffer.
    size_t len;                                                 // Length of raw JSON data (not just null-terminated).
};
WsQueueItem item;                                               // Queued incoming WebSocket message.

// --- NTRIP client. ---
char       ntripStatusMsg[300]        = {'\0'};                 // Latest status line for "connectNtripCasterResp".
char       lastGGA[100]               = {'\0'};                 // Last complete GGA sentence (any page). @see DevUBLOXGNSS::processNMEA().
WiFiClient ntripClient;                                         // WiFi connection to NTRIP caster.
                                                                // Owned only by taskRtcmRelay() (core 0) for connect()/stop()/read().
                                                                // Requests/status cross task boundary as flags, same pattern as browserUpdatePendingFlag. 

// --- GNSS. ---
int8_t  ellipsoidHp;
int8_t  mslHp;
int8_t  latitudeHp;
int8_t  longitudeHp;
int32_t ellipsoid;
int32_t msl;
int32_t latitude;   
int32_t longitude;
int32_t altitudeMSL;
float   heightEllipsoid    = 0;                                 // GNSS - ellipsoid height (meters).
float   heightOrthometric  = 0;                                 // GNSS - orthometric height (meters).
float   accuracyHorizontal = 0;                                 // GNSS - horizontal accuracy.
float   accuracyVertical   = 0;                                 // GNSS - vertical accuracy.
SFE_UBLOX_GNSS roverGNSS;                                       // GNSS object (uses I2C-1).

// --- Operation. ---
enum CommandIndex {                                             //  Readable index for command array.
    TEST_RAD = 0,                                               //  0.
    DEBUG_RTCM,                                                 //  1.
    DEBUG_GNSS,                                                 //  2.
    DEBUG_NMEA,                                                 //  3.
    DEBUG_BTNS,                                                 //  4.
    DEBUG_SER,                                                  //  5.
    DEBUG_WIFI_EVENTS,                                          //  6.
    DEBUG_WS_EVENTS,                                            //  7.
    DEBUG_LIPO,                                                 //  8.
    SHOW_UPTIME,                                                //  9.
    RESTART,                                                    // 10.
    DEBUG_TEMP,                                                 // 11.
    DEBUG_NMEA_HEX,                                             // 12.
    DEBUG_NMEA_COUNTS,                                          // 13.
    DEBUG_PREFS,                                                // 14.
    DEBUG_NTRIP,                                                // 15.
    DEBUG_TIMERS,                                               // 16.
    DEBUG_WS_TRAFFIC,                                           // 17.
    DEBUG_HEAP,                                                 // 18.
    DEBUG_NMEA_SENT,                                            // 19.
    DEBUG_LOOP_STACK,                                           // 20.
    DEBUG_FILE_SYSTEM,                                          // 21.
    NUM_COMMANDS                                                // 22 = automatic array length.
};     
const char* COMMAND[NUM_COMMANDS] = {                           // Command strings; match CommandIndex.
    "test-rad",                                                 // TEST_RAD.
    "debug-rtcm",                                               // DEBUG_RTCM.
    "debug-gnss",                                               // DEBUG_GNSS.
    "debug-nmea",                                               // DEBUG_NMEA.
    "debug-btns",                                               // DEBUG_BTNS.
    "debug-ser",                                                // DEBUG_SER.
    "debug-wifi-events",                                        // DEBUG_WIFI_EVENTS.
    "debug-ws-events",                                          // DEBUG_WS_EVENTS.
    "debug-lipo",                                               // DEBUG_LIPO.
    "show-uptime",                                              // SHOW_UPTIME.
    "restart",                                                  // RESTART.
    "debug-temp",                                               // DEBUG_TEMP.
    "debug-nmea-hex",                                           // DEBUG_NMEA_HEX.
    "debug-nmea-counts",                                        // DEBUG_NMEA_COUNTS.
    "debug-prefs",                                              // DEBUG_PREFS.
    "debug-ntrip",                                              // DEBUG_NTRIP.
    "debug-timers",                                             // DEBUG_TIMERS.
    "debug-ws-traffic",                                         // DEBUG_WS_TRAFFIC.
    "debug-heap",                                               // DEBUG_HEAP.
    "debug-nmea-sent",                                          // DEBUG_NMEA_SENT.
    "debug-loop-stack",                                         // DEBUG_LOOP_STACK.
    "debug-file-system",                                        // DEBUG_FILE_SYSTEM.
};     
const bool    RW_MODE                    = false;               // Open preference name space as read/write.
const bool    RO_MODE                    = true;                // Open preference name space as read only.
const char    LOG_FILE[]                 = "/log.txt";          // Log file.
const uint8_t MAJOR_VERSION              = 3;                   // Current major build version (@see buildInfo()).
const uint8_t MINOR_VERSION              = 4;                   // Current minor build version (@see buildInfo()).
const uint8_t PATCH_VERSION              = 1;                   // Current patch build version (@see buildInfo()).
const uint8_t MIN_SATELLITE_THRESHHOLD   = 2;                   // Minimum SIV for reliable coordinate information.      
      char     operMode[2]               = {'\0'};              // Operation mode (r=rover, b=base).
      char     debugTemp[250]            = {'\0'};              // Various debug scenarios.
      char     whichPage[10]             = {'\0'};              // Current browser page served by startHttpServer().
      char     buildString[50]           = {'\0'};              // Build string (build version on date at time). e.g. 3.0.12 - Feb 19 2026 @ 12:23:13
      char     serialState[4];                                  // Serial state: '-', 'u', or 'd'.
                                                                // serialState[0] = USB interface.
                                                                // serialState[1] = Serial0 interface, not used.
                                                                // serialState[2] = Serial1 interface, RTCM in from HC-12.
                                                                // serialState[3] = Serial2 interface, RTCM out to ZED UART2.
                                                                // prfRtcmInSource (char[6]) RTCM in source: off, radio, ntrip, ...
                                                                // RTCM (bool) being received within RTCM_TIMEOUT.
                                                                // NMEA (bool) being sent OUT to MCU #2.
      char     operBuffer[24]            = {'\0'};              // Buffer for Operate data.
      char     startOutputsResults[1024] = {'\0'};              // Buffered output from startOutputs().
      char     buildInfoResults[200]     = {'\0'};              // Buffered output from buildInfo().
      char     outputBuffer[1024]        = {'\0'};              // Buffer for output data.
      char     timeSinceBoot[20]         = {'\0'};              // Time since boot: 01h 03m 12s.
      size_t   wsSendCount               = 0;                   // # of WebSocket messages sent.
      size_t   rtcmSentenceCount         = 0;                   // # of RTCM sentences in.
      uint8_t  numSatInView              = 0;                   // GNSS - # OF satellites in view.
      uint8_t  fixType                   = 0;                   // GNSS - type of fix (single, RTK-float, RTK-fix).
//    uint16_t loopTaskStackSize         = 16384;               // Override Arduino-ESP32 weak default (8192). @see processMessagesIn().
      int64_t  bootTime;                                        // Boot time.
      float    rtcmKbps                  = 0;                   // RTCM kbps (average).
      float    batterySoc                = 0;                   // Battery State Of Charge (SOC).
      float    batteryChangeRate         = 0;                   // Battery charge - rate of change.
      double   lat                       = 0;                   // GNSS - latitude.
      double   lon                       = 0;                   // GNSS - longitude.

// --- Flags. ---
bool browserUpdatePendingFlag       = false;                    // Update ready to send to browser page (operate, nmea, ...).
bool restartGrMcuFlag               = false;                    // Restart MCU.
bool debugFlag                      = false;                    // Debug active.
bool inLoopFlag                     = false;                    // In loop() indicator.
bool zeroStatusCountersFlag         = false;                    // Zero out status counters.
bool NMEAoutFlag                    = false;                    // NMEA being sent out.
bool RTCMinFlag                     = false;                    // RTCM being received within RTCM_TIMEOUT.
bool tcpClientConnectedFlag         = false;                    // TCP client connected.
bool ntripStatusPendingFlag         = false;                    // New ntripStatusMsg ready to forward to browser.
bool ntripSendRtcmSentenceCountFlag = false;                    // Flag to send rtcmSentenceCount for ntrip page. Triggered by taskEvery1000Ms().
bool heightLockFlag                 = false;                    // Height currently locked - substituting NMEA.
bool positionLockFlag               = false;                    // Position currently locked - substituting NMEA.
bool heightLockAveragingFlag        = false;                    // Averaging window in progress for height lock.
bool positionLockAveragingFlag      = false;                    // Averaging window in progress for position lock.
bool laserOnFlag                    = false;                    // Laser state.
bool commandFlag[NUM_COMMANDS]      = {false};                  // Debug command flags.

// --- Preferences. ---
const char        DEF_NTRIP_CAST_ATTR_1[] = "{\"43\":1,\"44\":\"PointPerfect (SparkPNT)\",\"45\":\"ppntrip.services.u-blox.com\",\"46\":\"NEAR-RTCM\",\"47\":2101,\"48\":1,\"49\":\"tbd\",\"50\":\"tbd\",\"51\":1,\"53\":\"\"}";    // Newest pref.
const char        DEF_NTRIP_CAST_ATTR_2[] = "{\"43\":2,\"44\":\"\",\"45\":\"\",\"46\":\"\",\"47\":2101,\"48\":1,\"49\":\"\",\"50\":\"\",\"51\":1,\"53\":\"\"}";
const char        DEF_NTRIP_CAST_ATTR_3[] = "{\"43\":3,\"44\":\"\",\"45\":\"\",\"46\":\"\",\"47\":2101,\"48\":1,\"49\":\"\",\"50\":\"\",\"51\":1,\"53\":\"\"}";
const uint16_t    NTRIP_CAST_ATTR_LEN     = 512;                // Length of character array for NTRIP caster attibute profile.
      char        prfUnt[6];                                    // Distance units: meter/feet (used only in browser).
      char        prfRtcmInSource[8];                           // RTCM in source: off, radio, ntrip, ...
      char        prfHotSsi[20];                                // WiFi hotspot client: network SSID.
      char        prfHotPas[30];                                // WiFi hotspot client: password.
      char        prfNtripCastAttr[4][NTRIP_CAST_ATTR_LEN];     // 2D Array of (4) NTRIP caster attribute profiles (each is in JSON format).
      uint8_t     prfNtripCastAct;                              // Which # NTRIP caster attribute profile is being used.
      uint8_t     prfGnsNavRat;                                 // ZED: OUTPUT every X (e.g. 5) MEASURE intervals every (e.g. 5*100=500) ms.
      uint16_t    prfGnsMsrInt;                                 // ZED: MEASURE every Y (e.g. 100) ms.
      uint16_t    prfInstrHgt;                                  // Instrument height (includes rover height + pole height).
      uint16_t    prfLckAvgInt;                                 // GNSS lock averaging interval.
      Preferences roverPrefs;                                   // Rover's NVS preferences namespace.
      enum        prefAction {                                  // Readable index for preference actions.
          PREF_INIT,                                            // 0.
          PREF_READ,                                            // 1.
          PREF_SET,                                             // 2.
          PREF_RESET,                                           // 3.
          PREF_PRINT,                                           // 4.
          PREF_SET_NTRIP,                                       // 5.
          PREF_RESET_NO_REBOOT                                  // 6.
      };
struct ntripCasterProfile {                                     // NTRIP caster attribute template.
    bool     sendGga;                                           // Flag - send keepalive back to NTRIP caster.
    char     name[48];
    char     url[48];
    char     mount[24];
    char     user[48];
    char     pass[48];
    char     crs[48];                                           // Newest pref.
    uint8_t  id;
    uint8_t  version;
    uint16_t port;
};
ntripCasterProfile ntripCaster = {};                            // NTRIP caster attribute profile being used.

// --- Oper status. ---
size_t  nmeaCountAll   = 0;
size_t  nmeaCountGGA   = 0;
size_t  nmeaCountRMC   = 0;
size_t  nmeaCountGSA   = 0;
size_t  nmeaCountGSV   = 0;
size_t  nmeaCountGST   = 0;
size_t  nmeaCountTXT   = 0;
size_t  nmeaCountOther = 0;
int64_t nmeaRate       = 0;
int64_t lastRTCMtime   = esp_timer_get_time();                  // Last time (us) when RTCM input received.

// --- FreeRTOS handles. ---
extern TaskHandle_t  loopTaskHandle;                            // Handle for FreeRTOS task: loopTask (i.e. loop()). Declared in Arduino-ESP32 core's main.cpp
       TaskHandle_t  taskLoopStatusLedHandle;                   // Handle for GhostRover FreeRTOS task: Loop status LED.
       TaskHandle_t  taskRtcmRelayHandle;                       // Handle for GhostRover FreeRTOS task: RTCM relay, Serial1 -> Serial2.
       TaskHandle_t  taskBuildOperDataHandle;                   // Handle for GhostRover FreeRTOS task: buildOperData.
       TaskHandle_t  taskEvery1000MsHandle;                     // Handle for GhostRover FreeRTOS task: Do these every 1 second.
       TaskHandle_t  taskEvery10000MsHandle;                    // Handle for GhostRover FreeRTOS task: Do these every 10 seconds.
       QueueHandle_t wsRxQueue;                                 // Handle for GhostRover FreeRTOS queue: AsyncTCP task -> loop().

// --- FreeRTOS tasks. ---
const TickType_t LED_BLINK_INTERVAL     = 40/portTICK_PERIOD_MS;                             // LED "blink" time (ms) =  0.04 seconds.
const TickType_t EVERY_1000MS_INTERVAL  = 1000/portTICK_PERIOD_MS;                           // Do these every 1 second.
const TickType_t EVERY_10000MS_INTERVAL = 10000/portTICK_PERIOD_MS;                          // Do these every 1 second.
      TickType_t buildOperDataInterval  = (prfGnsNavRat * prfGnsMsrInt)/portTICK_PERIOD_MS;  // Time (ms) [pref based] between operate page updates.

// --- Lock/average state. ---
      int64_t  heightLockAveragingStart   = 0;
      int64_t  positionLockAveragingStart = 0;
      double   heightLockSumOrtho         = 0;
      double   heightLockSumGeoidSep      = 0;
      uint32_t heightLockSampleCount      = 0;
      double   positionLockSumLat         = 0;
      double   positionLockSumLon         = 0;
      uint32_t positionLockSampleCount    = 0;
      double   lockedLat                  = 0;
      double   lockedLon                  = 0;
      float    lockedHeightOrtho          = 0;
      float    lockedGeoidSep             = 0;       // heightEllipsoid - heightOrthometric, frozen at lock time.

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
 * @since 3.3.0  [2026-08-13-12:30pm] Replaced ntripPushGGA() with checkLoopTimers().
 * @since 3.3.1  [2026-08-16-05:30pm] Add logPrint().
 * @since 3.3.5  [2026-09-07-01:15pm] Replaced checkLoopTimers() in loop() with FreeRTOS tasks.
 * @see   uptime()                - Display time since boot.
 * @see   logPrint()              - Save a message to LOG_FILE, print to Serial if available.
 * @see   statusLedOn()           - Turn on status LED.
 * @see   prefUtility()           - Preference utility.
 * @see   buildOperData()         - Build data for operate page.
 * @see   sendDataToBrowser()     - Fill & send jsonDocToBrowser.
 * @see   rtcm3GetMessageType()   - Return RTCM3 message type to taskRtcmRelay().
 * @see   relayRtcmByte()         - Relay RTCM byte to Serial2 (ZED UART2), tracking stats.
 * @see   nmeaChecksum()          - Compute NMEA checksum (XOR of all bytes between '$' and '*').
 * @see   nmeaSetFields()         - Replace comma-delimited fields in NMEA sentence, recompute and append checksum.
 * @see   decimalDegreesToNmea()  - Convert decimal degrees to NMEA ddmm.mmmmmmm / dddmm.mmmmmmm format.
 * @see   substituteLockedNmea()  - Substitute locked position/height values into full NMEA sentence.
 */

  /**
 * -------------------------------------------------------------------------
 *  Display time since boot.
 * -------------------------------------------------------------------------
 *
 * @param  char*  dest       Output buffer.
 * @param  size_t max_len    Output buffer max length.
 * @param  bool   fractional Which version.
 * @return void   No output is returned.
 * @since  3.3.3 [2026-08-31-03:30pm] New.
 */
void uptime(char *dest, size_t max_len, bool fractional = true) {
    int64_t us            = esp_timer_get_time() - bootTime;
    int32_t total_seconds = us / 1000000;
    int32_t hours         = total_seconds / 3600;
    int32_t minutes       = (total_seconds / 60) % 60;
    float   secs          = (total_seconds % 60) + (us % 1000000) / 1000000.0f;
    if (fractional) {
        snprintf(dest, max_len, "%dh %dm %.2fs", hours % 24, minutes, secs);
    } else {
        snprintf(dest, max_len, "%dh %dm %ds", hours % 24, minutes, (int)secs);
    }
}

 /**
 * -------------------------------------------------------------------------
 *  Print text to USB if Serial available. Add text to LOG_FILE.
 * -------------------------------------------------------------------------
 *
 * @param  char* textToBuffer Text to log/print.
 * @param  bool  log Log text to SD card.
 * @param  bool  newline Use println() vs print().
 * @return void   No output is returned.
 * @since  3.3.1 [2026-08-16-05:15pm] New.
 * @since  3.3.2 [2026-08-16-05:15pm] Added log secondary parameter.
 * @since  3.3.3 [2026-09-04-09:30am] Added newline tertiary parameter.
 */
void logPrint(const char* textToLogPrint = NULL, bool log = true, bool newline = true) {

    if (textToLogPrint == NULL)  {
        return;
    } else {

        // --- Print to Serial. ---
        if (newline) {
            Serial.println(textToLogPrint);
        } else {
            Serial.print(textToLogPrint);
        }

        // --- Send to log file. ---
        if (log) {
            File file = SD_MMC.open(LOG_FILE, FILE_APPEND);
            if (file) {
                if (newline) {
                    file.println(textToLogPrint);
                } else {
                    file.print(textToLogPrint);
                }
                file.close();
            }
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
 * @param  enum    prefAction PREF_INIT, PREF_READ, PREF_SET, PREF_RESET, PREF_PRINT, PREF_SET_NTRIP, PREF_RESET_NO_REBOOT.
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
 * @since  3.3.1  [2026-08-27-02:30pm] ["JsonDocNtrip["43,47,48, 51"] from char to int.
 * @since  3.3.1  [2026-08-27-02:00pm] Add PREF_RESET_NO_REBOOT, changed PREF_RESET to call prefUtility(PRF_SET).
 * @since  3.4.0  [2026-09-20-05:30pm] Add prfLckAvgInt.
 * @since  3.4.1  [2026-09-27-10:00pm] Added ntripCaster.crs.
 * @see    Global vars: Preference defaults, setup().
 * @link   https://docs.espressif.com/projects/arduino-esp32/en/latest/tutorials/preferences.html.
 * @link   https://github.com/espressif/arduino-esp32/tree/master/libraries/Preferences/.
 */
void prefUtility(prefAction action, const char* key = NULL, const char* value = NULL) {

    // --- Local vars. ---
    // DEF_NTRIP_CAST_ATTR_1,2,3[] are globals.
    const char      NAMESPACE[]             = "config";         // The preference namespace. 
    const char      DEF_UNT[]               = "meter";          // Default distance units: meter/feet (used only in browser).                       - Matching global var: char     prfUnt[6].
    const char      DEF_RTC_IN[]            = "off";            // Default control RTCM in: off, radio, ntrip, ...                                  - Matching global var: char     prfRtcmInSource[6].
    const char      DEF_HOT_SSI[]           = "tbd";            // Default WiFi hotspot client: network SSID.                                       - Matching global var: char     prfHotSsi[20].
    const char      DEF_HOT_PASS[]          = "tbd";            // Default WiFi hotspot client: password.                                           - Matching global var: char     prfHotPas[30].
                                                                // Default NTRIP caster attribute profile 1.                                        - Matching global var: char     prfNtripCastAttr[0].
                                                                // Default NTRIP caster attribute profile 2.                                        - Matching global var: char     prfNtripCastAttr[1].
                                                                // Default NTRIP caster attribute profile 3.                                        - Matching global var: char     prfNtripCastAttr[2].
    const uint8_t   DEF_NTRIP_CAST_ACT      = 1;                // Default NTRIP caster profile being used.                                         - Matching global var: uint8_t  prfNtripCastAct.
    const uint8_t   DEF_GNS_NAV_RAT         = 2;                // Default ZED rate (times/interval): OUTPUT a new solution.                        - Matching global var: uint8_t  prfGnsNavRat.
    const uint16_t  DEF_GNS_MSR_INT         = 100;              // Default ZED interval (ms): CREATE a new solution.                                - Matching global var: uint16_t prfGnsMsrInt.
    const uint16_t  DEF_INSTR_HGT           = 128;              // Default instrument height (mm - includes rover height [128] + pole height [0]).  - Matching global var: uint16_t prfInstrHgt.
    const uint16_t  DEF_LCK_AVG_INT         = 3;                // Default GNSS lock averaging interval.  // Newest pref. 2.
    const uint16_t  NUM_PREFS               = 13;               // Number of preferences being used.
          size_t    remaining               = 0;                // Number of bytes remaining that can be written to char array. 
          bool      hasKey                  = false;
          DeserializationError ntripError;
    memset(prfNtripCastAttr[3], '\0', NTRIP_CAST_ATTR_LEN);     // Scratch buffer - not used.

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
            snprintf(outputBuffer, sizeof(outputBuffer), "NVS namespace %s using %u entries with %u available.", NAMESPACE, NUM_PREFS, roverPrefs.freeEntries());
            logPrint(outputBuffer);
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
            prfNtripCastAct = roverPrefs.getUShort("prfNtripCastAct");
            prfGnsNavRat    = roverPrefs.getUShort("prfGnsNavRat");           
            prfGnsMsrInt    = roverPrefs.getUShort("prfGnsMsrInt");
            prfInstrHgt     = roverPrefs.getUShort("prfInstrHgt");
            prfLckAvgInt    = roverPrefs.getUShort("prfLckAvgInt");     // Newest pref. 2.

            // - Create NTRIP caster JSON doc from embedded JSON string for active caster. -
            // Embedded JSON string allows attributes for an NTRIP caster to be stored as a single preference. 
            JsonDocNtrip.clear();
            ntripError = deserializeJson(JsonDocNtrip, prfNtripCastAttr[prfNtripCastAct-1]);
            if (ntripError) {
                snprintf(outputBuffer, sizeof(outputBuffer), "JSON deserialize failed: %s", ntripError.f_str());
                logPrint(outputBuffer);
                return;
            }

            // - Set global vars from JSON values. -
            strlcpy(ntripCaster.name,  JsonDocNtrip["44"],  sizeof(ntripCaster.name));
            strlcpy(ntripCaster.url,   JsonDocNtrip["45"],   sizeof(ntripCaster.url));
            strlcpy(ntripCaster.mount, JsonDocNtrip["46"], sizeof(ntripCaster.mount));
            strlcpy(ntripCaster.user,  JsonDocNtrip["49"],  sizeof(ntripCaster.user));
            strlcpy(ntripCaster.pass,  JsonDocNtrip["50"],  sizeof(ntripCaster.pass));
            // A newly added pref has never been saved to NVS. Test if it exists before trying to copy.  [2026-09-27]
            if (JsonDocNtrip["53"].is<JsonVariant>()) {
                strlcpy(ntripCaster.crs, JsonDocNtrip["53"], sizeof(ntripCaster.crs));      // Newest pref.
            }
            ntripCaster.id      = JsonDocNtrip["43"];
            ntripCaster.port    = JsonDocNtrip["47"];
            ntripCaster.version = JsonDocNtrip["48"];
            ntripCaster.sendGga = JsonDocNtrip["51"].as<bool>();

            // -- Close name space. --
            roverPrefs.end();
            if (commandFlag[DEBUG_PREFS]) {
                logPrint("Preferences read.");
            }
            break;

        case PREF_SET:

            // -- Open name space. --
            roverPrefs.begin("config", RW_MODE);

            // - Set NVS preferences from global vars. -
            roverPrefs.putString("prfUnt",          prfUnt);              // Store as "prfUnt"              (sent/rcvd as "1").
            roverPrefs.putString("prfRtcmInSource", prfRtcmInSource);     // Store as "prfRtcmInSource"     (sent/rcvd as "2").
            roverPrefs.putString("prfHotSsi",       prfHotSsi);           // Store as "prfHotSsi"           (sent/rcvd as "6").
            roverPrefs.putString("prfHotPas",       prfHotPas);           // Store as "prfHotPas"           (sent/rcvd as "7").
            roverPrefs.putString("prfNtripCaster1", prfNtripCastAttr[0]); // Store as "prfNtripCastAttr[0]" (sent/rcvd as "39").
            roverPrefs.putString("prfNtripCaster2", prfNtripCastAttr[1]); // Store as "prfNtripCastAttr[1]" (sent/rcvd as "40").
            roverPrefs.putString("prfNtripCaster3", prfNtripCastAttr[2]); // Store as "prfNtripCastAttr[2]" (sent/rcvd as "41").
            roverPrefs.putUShort("prfNtripCastAct", prfNtripCastAct);     // Store as "prfNtripCastAct"     (sent/rcvd as "42").
            roverPrefs.putUShort("prfGnsNavRat",    prfGnsNavRat);        // Store as "prfGnsNavRat"        (sent/rcvd as "5").
            roverPrefs.putUShort("prfGnsMsrInt",    prfGnsMsrInt);        // Store as "prfGnsMsrInt"        (sent/rcvd as "4").
            roverPrefs.putUShort("prfInstrHgt",     prfInstrHgt);         // Store as "prfInstrHgt"         (sent/rcvd as "36" with value in mm, e.g. "165").
            roverPrefs.putUShort("prfLckAvgInt",    prfLckAvgInt);        // Store as "prfLckAvgInt"        (sent/rcvd as "52").    // Newest pref. 2.

            // -- Close name space. --
            roverPrefs.end();
            if (commandFlag[DEBUG_PREFS]) {
                logPrint("Preferences saved.");
            }
            break;

        case PREF_RESET:
        case PREF_RESET_NO_REBOOT:

            // -- Copy default values to global vars. --
            strlcpy(prfUnt,              DEF_UNT,               sizeof(prfUnt));
            strlcpy(prfRtcmInSource,     DEF_RTC_IN,            sizeof(prfRtcmInSource));
            strlcpy(prfHotSsi,           DEF_HOT_SSI,           sizeof(prfHotSsi));
            strlcpy(prfHotPas,           DEF_HOT_PASS,          sizeof(prfHotPas));
            strlcpy(prfNtripCastAttr[0], DEF_NTRIP_CAST_ATTR_1, NTRIP_CAST_ATTR_LEN);
            strlcpy(prfNtripCastAttr[1], DEF_NTRIP_CAST_ATTR_2, NTRIP_CAST_ATTR_LEN);
            strlcpy(prfNtripCastAttr[2], DEF_NTRIP_CAST_ATTR_3, NTRIP_CAST_ATTR_LEN);
            prfNtripCastAct            = DEF_NTRIP_CAST_ACT;
            prfGnsNavRat               = DEF_GNS_NAV_RAT;
            prfGnsMsrInt               = DEF_GNS_MSR_INT;
            prfInstrHgt                = DEF_INSTR_HGT;
            prfLckAvgInt               = DEF_LCK_AVG_INT;   // Newest pref. 2.

            // -- Save to NVS. --
            logPrint("Preference globals reset to defaults.");
            prefUtility(PREF_SET);

            // -- Wrap up. --
            if (action == PREF_RESET) {
                restartGrMcuFlag = true;
                logPrint("\nGR-MCU will restart.");
            } else {
                prefUtility(PREF_READ);
            }
            break;

        case PREF_PRINT:

            // -- Open name space. --
            roverPrefs.begin(NAMESPACE, RO_MODE);

            // -- Print values. --
            logPrint("                       Default    NVS    Global     ");
            logPrint("                       -------    ---    ------     ");
            snprintf(outputBuffer, sizeof(outputBuffer), "prfUnt                 \"%s\"   \"%s\"   \"%s\"", DEF_UNT,            roverPrefs.getString("prfUnt"),          prfUnt);
            logPrint(outputBuffer);
            snprintf(outputBuffer, sizeof(outputBuffer), "prfRtcmInSource        \"%s\"   \"%s\"   \"%s\"", DEF_RTC_IN,         roverPrefs.getString("prfRtcmInSource"), prfRtcmInSource);
            logPrint(outputBuffer);
            snprintf(outputBuffer, sizeof(outputBuffer), "prfHotSsi              \"%s\"   \"%s\"   \"%s\"", DEF_HOT_SSI,        roverPrefs.getString("prfHotSsi"),       prfHotSsi);
            logPrint(outputBuffer);
            snprintf(outputBuffer, sizeof(outputBuffer), "prfHotPas              \"%s\"   \"%s\"   \"%s\"", DEF_HOT_PASS,       roverPrefs.getString("prfHotPas"),       prfHotPas);
            logPrint(outputBuffer);
            snprintf(outputBuffer, sizeof(outputBuffer), "prfGnsNavRat           %u   %u   %u",             DEF_GNS_NAV_RAT,    roverPrefs.getUShort("prfGnsNavRat"),    prfGnsNavRat);
            logPrint(outputBuffer);
            snprintf(outputBuffer, sizeof(outputBuffer), "prfGnsMsrInt           %u   %u   %u",             DEF_GNS_MSR_INT,    roverPrefs.getUShort("prfGnsMsrInt"),    prfGnsMsrInt);
            logPrint(outputBuffer);
            snprintf(outputBuffer, sizeof(outputBuffer), "prfLckAvgInt           %u   %u   %u",             DEF_LCK_AVG_INT,    roverPrefs.getUShort("prfLckAvgInt"),    prfLckAvgInt);
            logPrint(outputBuffer);
            snprintf(outputBuffer, sizeof(outputBuffer), "prfNtripCastAct        %u   %u   %u",             DEF_NTRIP_CAST_ACT, roverPrefs.getUShort("prfNtripCastAct"), prfNtripCastAct);
            logPrint(outputBuffer);
            logPrint(" ");
            logPrint("Default:");
            snprintf(outputBuffer, sizeof(outputBuffer), "DEF_NTRIP_CAST_ATTR_1  \"%s\"", DEF_NTRIP_CAST_ATTR_1);
            logPrint(outputBuffer);
            snprintf(outputBuffer, sizeof(outputBuffer), "DEF_NTRIP_CAST_ATTR_2  \"%s\"", DEF_NTRIP_CAST_ATTR_2);
            logPrint(outputBuffer);
            snprintf(outputBuffer, sizeof(outputBuffer), "DEF_NTRIP_CAST_ATTR_3  \"%s\"", DEF_NTRIP_CAST_ATTR_3);
            logPrint(outputBuffer);
            logPrint(" ");
            logPrint("NVS:");
            roverPrefs.getString("prfNtripCaster1", prfNtripCastAttr[3], NTRIP_CAST_ATTR_LEN);      // [0,1,2] are used, [3] is for scratch.
            snprintf(outputBuffer, sizeof(outputBuffer), "prfNtripCaster1        \"%s\"", prfNtripCastAttr[3]);
            logPrint(outputBuffer);
            roverPrefs.getString("prfNtripCaster2", prfNtripCastAttr[3], NTRIP_CAST_ATTR_LEN);
            snprintf(outputBuffer, sizeof(outputBuffer), "prfNtripCaster2        \"%s\"", prfNtripCastAttr[3]);
            logPrint(outputBuffer);
            roverPrefs.getString("prfNtripCaster3", prfNtripCastAttr[3], NTRIP_CAST_ATTR_LEN);
            snprintf(outputBuffer, sizeof(outputBuffer), "prfNtripCaster3        \"%s\"", prfNtripCastAttr[3]);
            logPrint(outputBuffer);
            memset(prfNtripCastAttr[3], '\0', NTRIP_CAST_ATTR_LEN);                                 // Scratch buffer - not used.
            logPrint(" ");
            logPrint("Global:");
            snprintf(outputBuffer, sizeof(outputBuffer), "prfNtripCastAttr[0]    \"%s\"", prfNtripCastAttr[0]);
            logPrint(outputBuffer);
            snprintf(outputBuffer, sizeof(outputBuffer), "prfNtripCastAttr[1]    \"%s\"", prfNtripCastAttr[1]);
            logPrint(outputBuffer);
            snprintf(outputBuffer, sizeof(outputBuffer), "prfNtripCastAttr[2]    \"%s\"", prfNtripCastAttr[2]);
            logPrint(outputBuffer);
            snprintf(outputBuffer, sizeof(outputBuffer), "ntripCaster.name       \"%s\"", ntripCaster.name);
            logPrint(outputBuffer);
            snprintf(outputBuffer, sizeof(outputBuffer), "ntripCaster.url        \"%s\"", ntripCaster.url);
            logPrint(outputBuffer);
            snprintf(outputBuffer, sizeof(outputBuffer), "ntripCaster.mount      \"%s\"", ntripCaster.mount);
            logPrint(outputBuffer);
            snprintf(outputBuffer, sizeof(outputBuffer), "ntripCaster.user       \"%s\"", ntripCaster.user);
            logPrint(outputBuffer);
            snprintf(outputBuffer, sizeof(outputBuffer), "ntripCaster.pass       \"%s\"", ntripCaster.pass);
            logPrint(outputBuffer);
            snprintf(outputBuffer, sizeof(outputBuffer), "ntripCaster.crs       \"%s\"", ntripCaster.crs);      // Newest pref.
            logPrint(outputBuffer);
            snprintf(outputBuffer, sizeof(outputBuffer), "ntripCaster.id         %d", ntripCaster.id);
            logPrint(outputBuffer);
            snprintf(outputBuffer, sizeof(outputBuffer), "ntripCaster.version    %d", ntripCaster.version);
            logPrint(outputBuffer);
            snprintf(outputBuffer, sizeof(outputBuffer), "ntripCaster.port       %d", ntripCaster.port);
            logPrint(outputBuffer);
            snprintf(outputBuffer, sizeof(outputBuffer), "ntripCaster.sendGga    %d", ntripCaster.sendGga);
            logPrint(outputBuffer);

            // -- Close name space. --
            roverPrefs.end();
            break;
        
        case PREF_SET_NTRIP:

            // -- Open name space. --
            roverPrefs.begin("config", RW_MODE);

            // - Create NTRIP caster JSON doc from embedded JSON string for active caster. -
            JsonDocNtrip.clear();
            ntripError = deserializeJson(JsonDocNtrip, jsonDocFromBrowser["setNtripCasterPref"].as<const char*>());
            if (ntripError) {
                snprintf(outputBuffer, sizeof(outputBuffer), "NTRIP caster JSON deserialize failed: %s", ntripError.f_str());
                logPrint(outputBuffer);
                roverPrefs.end();
                return;
            }

            // - Set global vars from JSON values. -
            ntripCaster.id = JsonDocNtrip["43"];
            strlcpy(prfNtripCastAttr[0], jsonDocFromBrowser["setNtripCasterPref"], NTRIP_CAST_ATTR_LEN);

            switch (ntripCaster.id) {
                case 1:
                    roverPrefs.putString("prfNtripCaster1", prfNtripCastAttr[0]);   // Store as "prfNtripCaster1" (sent/rcvd as "39").
                    break;
                case 2:
                    roverPrefs.putString("prfNtripCaster2", prfNtripCastAttr[0]);   // Store as "prfNtripCaster2" (sent/rcvd as "40").
                    break;
                case 3:
                    roverPrefs.putString("prfNtripCaster3", prfNtripCastAttr[0]);   // Store as "prfNtripCaster3" (sent/rcvd as "41").
                    break;
            }

            // -- Close name space. --
            roverPrefs.end();
            logPrint("NTRIP preference set.\nMCU will restart.");
            restartGrMcuFlag = true;
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
 * @since  3.4.0  [2026-09-13-04:00pm] Height/position lock/unlock.
 * @see    Global vars: WebSockets, setup().
 */
 void buildOperData() {

    // --- GNSS ready? ---
    if (!roverGNSS.getPVT()) {
        return;
    }

    // -- Satellites in view. --
    numSatInView = roverGNSS.getSIV();

    if (numSatInView > MIN_SATELLITE_THRESHHOLD) {                          // Enough satellites?

        // -- Fix type. --
        if (roverGNSS.getFixType() == 3) {
            fixType = 1;                                                    // Single.
        }
        if (roverGNSS.getCarrierSolutionType() == 1 ) {
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
         * prfInstrHgt is instrument height in mm.
         */

        // -- Height - ellipsoid (h). --
        ellipsoid       = roverGNSS.getElipsoid();                              // mm
        ellipsoidHp     = roverGNSS.getElipsoidHp();                            // mm * 10^-1.
        heightEllipsoid = (ellipsoid * 10) + ellipsoidHp;
        heightEllipsoid = heightEllipsoid / 10000.0;                            // Convert to meters.

        // -- Height - orthometric (H). --
        msl               = roverGNSS.getMeanSeaLevel();
        mslHp             = roverGNSS.getMeanSeaLevelHp();
        altitudeMSL       = roverGNSS.getAltitudeMSL();
        heightOrthometric = (msl * 10) + mslHp;
        heightOrthometric = heightOrthometric / 10000.0;      
        heightOrthometric = heightOrthometric - ((float)prfInstrHgt)/1000.0;    // Reduce by instrument height.

        // -- Latitude. --
        latitude   = roverGNSS.getHighResLatitude();                            // Degrees * 10^-7.
        latitudeHp = roverGNSS.getHighResLatitudeHp();                          // High precision component: degrees * 10^-9.
        lat        = latitude    / 10000000.0;                                  // Convert to to 64 bit double - degrees (8 decimal places).
        lat        += latitudeHp / 1000000000.0;                                // Add high precision component.

        // -- Longitude. --
        longitude   = roverGNSS.getHighResLongitude();
        longitudeHp = roverGNSS.getHighResLongitudeHp();
        lon         = longitude    / 10000000.0;
        lon         += longitudeHp / 1000000000.0;

        // -- Horizontal & vertical accuracy. --
        accuracyHorizontal = roverGNSS.getHorizontalAccuracy() / 10000.0;
        accuracyVertical   = roverGNSS.getVerticalAccuracy()   / 10000.0;

        // -- Height lock averaging. --
        if (heightLockAveragingFlag) {
            heightLockSumOrtho    += heightOrthometric;
            heightLockSumGeoidSep += (heightEllipsoid - heightOrthometric);
            heightLockSampleCount++;
            if ((esp_timer_get_time() - heightLockAveragingStart) >= prfLckAvgInt * 1000000) {
                lockedHeightOrtho       = heightLockSumOrtho / heightLockSampleCount;
                lockedGeoidSep          = heightLockSumGeoidSep / heightLockSampleCount;
                heightLockAveragingFlag = false;
                heightLockFlag          = true;
                snprintf(outputBuffer, sizeof(outputBuffer), "Height locked @ %.4f m (%u samples in %u secs).", lockedHeightOrtho, heightLockSampleCount, prfLckAvgInt);
                if (commandFlag[DEBUG_BTNS]) {                      // Debug.    
                    logPrint(outputBuffer);
                }
            }
        }
        if (heightLockFlag) {
            heightOrthometric = lockedHeightOrtho;
            heightEllipsoid   = lockedHeightOrtho + lockedGeoidSep;
        }

        // -- Position lock averaging. --
        if (positionLockAveragingFlag) {
            positionLockSumLat += lat;
            positionLockSumLon += lon;
            positionLockSampleCount++;
            if ((esp_timer_get_time() - positionLockAveragingStart) >= prfLckAvgInt * 1000000) {
                lockedLat                 = positionLockSumLat / positionLockSampleCount;
                lockedLon                 = positionLockSumLon / positionLockSampleCount;
                positionLockAveragingFlag = false;
                positionLockFlag          = true;
                snprintf(outputBuffer, sizeof(outputBuffer), "Position locked @ Lat %.9f, Lon %.9f (%u samples in %u secs).", lockedLat, lockedLon, positionLockSampleCount, prfLckAvgInt);
                if (commandFlag[DEBUG_BTNS]) {                      // Debug.
                    logPrint(outputBuffer);
                }
            }
        }

        if (positionLockFlag) {
            lat = lockedLat;
            lon = lockedLon;
        }

        // -- Battery. --
        batterySoc        = lipo.getSOC();
        batteryChangeRate = lipo.getChangeRate();
    }
}

/**
 * -------------------------------------------------------------------------
 *  Fill & send jsonDocToBrowser.
 * -------------------------------------------------------------------------
 *
 * @return void No output is returned.
 * @since  3.2.1 [2026-07-26-06:30pm] New.
 * @since  3.1.2 [2026-07-16-09:00am] Increase jsonBuffer[768] to 1024.
 * @since  3.2.1 [2026-07-30-10:30am] jsonDocToBrowser["NMEA"] '= lastNmea' was '= nmeaBuffer'.
 * @since  3.2.1 [2026-07-31-01:30pm] Moved "Wrap up" section from processMessagesIn() to here.
 * @since  3.2.2 [2026-08-09-05:30pm] Only print for DEBUG_WS_EVENTS.
 * @since  3.3.0 [2026-08-15-12:00pm] Moved restarts to checkFlags().
 * @since  3.3.5 [2026-09-08-10:30pm] Added check for ntripClient.connected().
 * @see    checkFlags().
 * @see    buildOperData().
 * @see    processMessagesIn(), also for description of exchange protocol.

 */
void sendDataToBrowser() {

    // --- Local vars. ---
    char    jsonBuffer[1024];

    // --- NMEA page. ---
    if (strcmp(whichPage, "nmea") == 0) {
        jsonDocToBrowser["NMEA"] = lastNmea;
        // browserUpdatePendingFlag = true;
    }

    // --- NTRIP page. ---
    if (strcmp(whichPage, "ntrip") == 0) {
        if (ntripSendRtcmSentenceCountFlag) {
            if (ntripClient.connected()) {
                jsonDocToBrowser["37"] = rtcmSentenceCount;
            }
            ntripSendRtcmSentenceCountFlag = false;
        }
        if (ntripStatusPendingFlag) {
            jsonDocToBrowser["connectNtripCasterResp"] = ntripStatusMsg;
            ntripStatusPendingFlag = false;
        }
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
            snprintf(operBuffer, sizeof(operBuffer), "%.2f", heightEllipsoid);
            jsonDocToBrowser["10"] = operBuffer;
            snprintf(operBuffer, sizeof(operBuffer), "%.2f", heightOrthometric);
            jsonDocToBrowser["11"] = operBuffer;
            snprintf(operBuffer, sizeof(operBuffer), "%.8f", lat);
            jsonDocToBrowser["12"] = operBuffer;
            snprintf(operBuffer, sizeof(operBuffer), "%.8f", lon);
            jsonDocToBrowser["13"] = operBuffer;
            snprintf(operBuffer, sizeof(operBuffer), "%.3f", accuracyHorizontal);
            jsonDocToBrowser["14"] = operBuffer;
            snprintf(operBuffer, sizeof(operBuffer), "%.3f", accuracyVertical);
            jsonDocToBrowser["15"] = operBuffer;
            jsonDocToBrowser["16"] = (RTCMinFlag)  ? true : false;      // Up (true), down (false). @see taskRtcmRelay().
            jsonDocToBrowser["17"] = (NMEAoutFlag) ? true : false;      // Up (true), down (false). @see DevUBLOXGNSS::processNMEA().
            snprintf(operBuffer, sizeof(operBuffer), "%.2f", batterySoc);
            jsonDocToBrowser["18"] = operBuffer;
            snprintf(operBuffer, sizeof(operBuffer), "%.1f", batteryChangeRate);
            jsonDocToBrowser["19"] = operBuffer;
            uptime(timeSinceBoot, sizeof(timeSinceBoot), false);
            snprintf(operBuffer, sizeof(operBuffer), "%s", timeSinceBoot);
            jsonDocToBrowser["20"] = operBuffer;
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
            if (laserOnFlag) {
                snprintf(operBuffer, sizeof(operBuffer), "Laser on.");
                jsonDocToBrowser["laserOnResp"] = operBuffer;
            }
            if (heightLockFlag) {
                snprintf(operBuffer, sizeof(operBuffer), "Height locked.");
                jsonDocToBrowser["heightLockResp"] = operBuffer;
            }
            if (positionLockFlag) {
                snprintf(operBuffer, sizeof(operBuffer), "Position locked.");
                jsonDocToBrowser["positionLockResp"] = operBuffer;
            }
        }
    }

    // --- All pages. ---
    memset(jsonBuffer, '\0', sizeof(jsonBuffer));
    serializeJson(jsonDocToBrowser, jsonBuffer, sizeof(jsonBuffer));
    if (jsonDocToBrowser.size() > 0) {
        ws.textAll(jsonBuffer);                         // Send WebSocket message.
        wsSendCount++;
        if (commandFlag[DEBUG_WS_TRAFFIC]) {            // Debug. WebSocket data from GR to browser.
            snprintf(outputBuffer, sizeof(outputBuffer), "WS #%u: browser <-- %s", clientId, jsonBuffer);
            logPrint(outputBuffer);
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
                snprintf(outputBuffer, sizeof(outputBuffer), "RTCM3 desync - expected preamble, got %02x\n", (uint8_t)inputChar);
                logPrint(outputBuffer);    // (textToLogPrint, add to log file = true, add newline = true).
            }
            return;                         // Drop stray byte from parsing; relay above already happened.
        }
        RTCMinFlag     = true;
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
        snprintf(outputBuffer, sizeof(outputBuffer), "RTCM3 (%s) active(%d) #%zu Type:%u bytes:%u kbps:%.2f\n",
            prfRtcmInSource, RTCMinFlag, rtcmSentenceCount, msg_type, byteCount, rtcmKbps);
            logPrint(outputBuffer);    // (textToLogPrint, add to log file = true, add newline = true).
    }
    lastRTCMtime = esp_timer_get_time();
    // bytesLeftInFrame is already 0 - next byte in is treated as the next preamble.
}

/**
 * -------------------------------------------------------------------------
 *  Compute NMEA checksum (XOR of all bytes between '$' and '*').
 *    e.g. $GLGSV,1,1,01,77,06,333,10,3*4F\r\n
 * -------------------------------------------------------------------------
 *
 * @param  char *sentence             NMEA sentence.
 * @return uint8_t                    Checksum for NMEA sentence.
 * @since  3.4.0 [2026-09-12-07:15pm] Height/position lock/unlock.
 * @see    nmeaSetFields().
 */
uint8_t nmeaChecksum(const char *sentence) {

    // --- Local vars. ---
    const char    *p       = sentence + 1;          // Skip leading '$'.
          uint8_t checksum = 0;

    // XOR each byte between beginning of sentence ('$') and end('*'). 
    while (*p && *p != '*') {
        checksum ^= (uint8_t)*p;
        p++;
    }
    return checksum;
}

/**
 * -------------------------------------------------------------------------
 *  Replace one or more comma-delimited fields in a NMEA sentence, then
 *  recompute and append the checksum.
 * -------------------------------------------------------------------------
 * 
 *  Field numbering: 0 = message ID (e.g. "GNGGA"), 1 = first field after the ID,
 *    matching standard NMEA field numbering conventions.
 *
 * @param  sentence   In/out buffer. Must currently hold a valid, complete NMEA "$....*CS\r\n" sentence.
 * @param  maxLen     Size of sentence buffer.
 * @param  fieldNums  Array of field numbers to replace.
 * @param  newValues  Array of replacement strings (parallel to fieldNums).
 * @param  count      Number of entries in fieldNums/newValues.
 * @return bool       True on success, false if the rebuilt sentence would overflow maxLen (original buffer left untouched).
 * @since  3.4.0      [2026-09-12-09:15pm] Height/position lock/unlock.
 * @see    substituteLockedNmea().
 */
bool nmeaSetFields(char *sentence, size_t maxLen, const uint8_t *fieldNums, const char **newValues, uint8_t count) {
    const char    *p           = sentence;
          char    rebuilt[136] = {'\0'};           // Headroom above the typical 120-byte sentence.
          uint8_t currentField = 0;
          size_t  rebuiltLen   = 0;

    // --- Copy message ID (field 0). ---
    while (*p && *p != ',' && *p != '*' && rebuiltLen < sizeof(rebuilt) - 1) {
        rebuilt[rebuiltLen++] = *p++;
    }

    // --- Walk remaining fields, substituting where requested. ---
    while (*p && *p != '*') {
        if (*p != ',') { p++; continue; }              // Defensive; shouldn't happen.
        rebuilt[rebuiltLen++] = ',';
        p++;
        currentField++;

        bool replaced = false;
        for (uint8_t i = 0; i < count; i++) {
            if (fieldNums[i] == currentField) {
                size_t vlen = strlen(newValues[i]);
                if (rebuiltLen + vlen >= sizeof(rebuilt) - 1) return false;
                memcpy(&rebuilt[rebuiltLen], newValues[i], vlen);
                rebuiltLen += vlen;
                while (*p && *p != ',' && *p != '*') p++;   // Skip original field content.
                replaced = true;
                break;
            }
        }
        if (!replaced) {
            while (*p && *p != ',' && *p != '*' && rebuiltLen < sizeof(rebuilt) - 1) {
                rebuilt[rebuiltLen++] = *p++;
            }
        }
    }

    // --- Recompute checksum, append CRLF. ---
    uint8_t cs = nmeaChecksum(rebuilt);
    char csStr[6];
    snprintf(csStr, sizeof(csStr), "*%02X", cs);
    size_t csLen = strlen(csStr);
    if (rebuiltLen + csLen + 2 >= sizeof(rebuilt)) return false;
    memcpy(&rebuilt[rebuiltLen], csStr, csLen);
    rebuiltLen += csLen;
    rebuilt[rebuiltLen++] = '\r';
    rebuilt[rebuiltLen++] = '\n';
    rebuilt[rebuiltLen] = '\0';

    // --- Copy back only if it fits caller's buffer. ---
    if (rebuiltLen >= maxLen) return false;
    strlcpy(sentence, rebuilt, maxLen);
    return true;
}

/**
 * -------------------------------------------------------------------------
 *  Convert decimal degrees to NMEA ddmm.mmmmmmm / dddmm.mmmmmmm format
 *  (7 decimal places, matching UBLOX_CFG_NMEA_HIGHPREC).
 * -------------------------------------------------------------------------
 *
 * @param  double decDeg     Decimal degrees.
 * @param  bool   isLatitude Lattitude?
 * @param  char*  outVal     Converted to NMEA format.
 * @param  size_t outLen     outVal length.
 * @param  char*  hemisphere Which hemisphere.
 * @return void   No output is returned.
 * @since  3.4.0 [2026-09-12-09:15pm] Height/position lock/unlock.
 * @see    substituteLockedNmea().
 */
void decimalDegreesToNmea(double decDeg, bool isLatitude, char *outVal, size_t outLen, char *hemisphere) {
    *hemisphere = isLatitude ? (decDeg >= 0 ? 'N' : 'S') : (decDeg >= 0 ? 'E' : 'W');
    double absDeg   = fabs(decDeg);
    int    deg      = (int)absDeg;
    double minutes  = (absDeg - deg) * 60.0;
    if (isLatitude) {
        snprintf(outVal, outLen, "%02d%010.7f", deg, minutes);   // ddmm.mmmmmmm.
    } else {
        snprintf(outVal, outLen, "%03d%010.7f", deg, minutes);   // dddmm.mmmmmmm.
    }
}

/**
 * -------------------------------------------------------------------------
 *  Substitute locked position/height values into a completed NMEA
 *  sentence, if the relevant lock is active.
 * -------------------------------------------------------------------------
 *
 * @param  sentence  In/out buffer holding a complete "$....*CS\r\n" sentence.
 * @param  maxLen    Size of sentence buffer.
 * @return void   No output is returned.
 * @since  3.4.0 [2026-09-12-09:15pm] Height/position lock/unlock.
 * @since  3.4.0 [2026-09-13-03:30pm] Undersized field arrays. Dangling pointers to out-of-scope locals.
 * @see    DevUBLOXGNSS::processNMEA().
 */
void substituteLockedNmea(char *sentence, size_t maxLen) {
    char    latStr[14], lonStr[14], altStr[16], geoidStr[16];
    char    latHemiStr[2], lonHemiStr[2];
    char    latHemi, lonHemi;
    uint8_t fieldNums[6];              // Max: 4 position + 2 height fields (GGA combined case).
    const char *newValues[6];
    uint8_t count;

    // --- GGA: lat/lon (fields 2-5), altitude/geoid (fields 9, 11). ---
    if (strncmp(&sentence[3], "GGA", 3) == 0) {
        count = 0;
        if (positionLockFlag) {
            decimalDegreesToNmea(lockedLat, true,  latStr, sizeof(latStr), &latHemi);
            decimalDegreesToNmea(lockedLon, false, lonStr, sizeof(lonStr), &lonHemi);
            latHemiStr[0] = latHemi; latHemiStr[1] = '\0';
            lonHemiStr[0] = lonHemi; lonHemiStr[1] = '\0';
            fieldNums[count] = 2; newValues[count++] = latStr;
            fieldNums[count] = 3; newValues[count++] = latHemiStr;
            fieldNums[count] = 4; newValues[count++] = lonStr;
            fieldNums[count] = 5; newValues[count++] = lonHemiStr;
        }
        if (heightLockFlag) {
            snprintf(altStr,   sizeof(altStr),   "%.3f", lockedHeightOrtho);
            snprintf(geoidStr, sizeof(geoidStr), "%.3f", lockedGeoidSep);
            fieldNums[count] = 9;  newValues[count++] = altStr;
            fieldNums[count] = 11; newValues[count++] = geoidStr;
        }
        if (count > 0) {
            nmeaSetFields(sentence, maxLen, fieldNums, newValues, count);
        }
        return;
    }

    // --- RMC: lat/lon (fields 3-6). ---
    if (strncmp(&sentence[3], "RMC", 3) == 0) {
        if (!positionLockFlag) return;
        decimalDegreesToNmea(lockedLat, true,  latStr, sizeof(latStr), &latHemi);
        decimalDegreesToNmea(lockedLon, false, lonStr, sizeof(lonStr), &lonHemi);
        latHemiStr[0] = latHemi; latHemiStr[1] = '\0';
        lonHemiStr[0] = lonHemi; lonHemiStr[1] = '\0';
        uint8_t fn[4]     = {3, 4, 5, 6};
        const char *nv[4] = {latStr, latHemiStr, lonStr, lonHemiStr};
        nmeaSetFields(sentence, maxLen, fn, nv, 4);
        return;
    }

    // --- GLL: lat/lon (fields 1-4). Currently disabled in config, included for completeness. ---
    if (strncmp(&sentence[3], "GLL", 3) == 0) {
        if (!positionLockFlag) return;
        decimalDegreesToNmea(lockedLat, true,  latStr, sizeof(latStr), &latHemi);
        decimalDegreesToNmea(lockedLon, false, lonStr, sizeof(lonStr), &lonHemi);
        latHemiStr[0] = latHemi; latHemiStr[1] = '\0';
        lonHemiStr[0] = lonHemi; lonHemiStr[1] = '\0';
        uint8_t fn[4]     = {1, 2, 3, 4};
        const char *nv[4] = {latStr, latHemiStr, lonStr, lonHemiStr};
        nmeaSetFields(sentence, maxLen, fn, nv, 4);
        return;
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
 * @since  3.3.3  [2026-09-01-08:00am] Added START_DELAY.
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
    const uint16_t START_DELAY      = 1000;         // Blocking delay, allow RTK-SMA to get started
    const uint32_t SERIAL_USB_SPEED = 115200;       // Serial USB speed.
          bool     startedSd        = true;
          size_t   remaining        = 0;            // Number of bytes remaining that can be written to char array. 

    // --- Set initial state. ---
    ws2812LedColor = WHITE;                         // Indicates no error during setup(). 
    ws2812LedBlink = false;
    statusLedOn();                                  // FreeRTOS task taskLoopStatusLed() has not started yet.
    memset(startOutputsResults, '\0', sizeof(startOutputsResults));     // Display startOutputsResults[] at end of buildInfo().

    // --- Start Serial USB. ---
    Serial.begin(SERIAL_USB_SPEED);
    Serial.setTxTimeoutMs(0);                       // Set Serial to non-blocking if USB is not connected.
    serialState[0] = 'u';   // USB interface is up.
    snprintf(startOutputsResults, sizeof(startOutputsResults), "Serial USB started @ %d.\n", SERIAL_USB_SPEED);
    

    // --- Blocking delay, allow RTK-SMA to get started. ---
    ws2812LedColor = YELLOW;
    ws2812LedBlink = false;
    statusLedOn();                                  // FreeRTOS task taskLoopStatusLed() has not started yet.   
    snprintf(startOutputsResults, sizeof(startOutputsResults), "RTK-SMA boot delay (%dms).\n", START_DELAY);
    delay(START_DELAY);
    ws2812LedColor = WHITE;
    ws2812LedBlink = false;
    statusLedOn();                                  // FreeRTOS task taskLoopStatusLed() has not started yet.       

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

        // -- Test SD card & reader by writing new LOG_FILE. --
        if (SD_MMC.exists(LOG_FILE)) {                          // Create a fresh log file for every boot.
            SD_MMC.remove(LOG_FILE);
        }
        File file = SD_MMC.open(LOG_FILE, FILE_WRITE);          // FILE_WRITE to write to log file, FILE_APPEND to append to log file.
        if (file) {
            snprintf(outputBuffer, sizeof(outputBuffer), "Log file created.");
            remaining = sizeof(startOutputsResults) - strlen(startOutputsResults) - 1;
            strncat(startOutputsResults, outputBuffer, remaining);
            file.print("#");
            file.close();
        } else  {
            startedSd = false;
        }
    } else {
        startedSd = false;
    };

    // --- Error starting SD? ---
    if (!startedSd)  {
        ws2812LedColor = RED;
        ws2812LedBlink = false;
        statusLedOn();                                          // FreeRTOS task taskLoopStatusLed() has not started yet.
        if (Serial) {   // SD card contains UI files, do not continue.
            Serial.print("ERROR SD card. Freezing ...");
        }
        while (true) {};                                        // Freeze.
    }
}

/**
 * -------------------------------------------------------------------------
 *  Build & processor info.
 * -------------------------------------------------------------------------
 * 
 * @return void  No output is returned.
 * @since  3.0.10 [2025-12-30-02:00pm].
 * @since  3.0.10 [2026-01-07-09:45am] Local vars.
 * @since  3.1.1  [2026-06-25-01:00pm] Updated version, added startup delay, transition LED.
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
    snprintf(buildString, sizeof(buildString), "%u.%u.%u - %s @ %s", MAJOR_VERSION, MINOR_VERSION, PATCH_VERSION, __DATE__, __TIME__);
    snprintf(buildInfoResults, sizeof(buildInfoResults),
        "\n%s\n"
        "%s\n"
        "%s, Rev %d, %d core(s), ID (MAC) %012llX.",
        NAME,
        buildString,
        ESP.getChipModel(), chip_info.revision, chip_info.cores, ESP.getEfuseMac()
    );

    // --- Continue. ---
    logPrint(buildInfoResults);
    logPrint(startOutputsResults);
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
 * @since  3.3.4  [2026-09-16-09:30pm] Crossed pins 16 & 17.
 * @see    buildInfo(), setup().
 * @link   https://github.com/G6EJD/ESP32-Using-Hardware-Serial-Ports.
 * @link   https://randomnerdtutorials.com/esp32-uart-communication-serial-arduino/#esp32-custom-uart-pins.
 */
void startSerial() {

    // --- Local vars. ---
    const uint8_t  HC12_TX       =  5;                              // HC-12 TXD     {white wire}.
    const uint8_t  HC12_RX       =  6;                              // HC-12 RXD     {yellow wire}.
    const uint8_t  ZED_RX2       = 16;                              // ZED UART2 RX2 {white wire}.
    const uint8_t  ZED_TX2       = 17;                              // ZED UART2 TX2 {yellow wire}.
    const uint32_t SERIAL1_SPEED = 9600;                            // HC-12 default speed is 9600.
    // const uint32_t SERIAL2_SPEED = 57600;                        // ZED UART2 default speed is 38400.
    const uint32_t SERIAL2_SPEED = 38400;                           // ZED UART2 default speed is 38400.

    // --- Start serial interfaces. ---
    serialState[1] = '-';
    logPrint("Serial0 is not used.");

    // --- Serial1 connects to HC-12. If config preference for NTRIP is not radio, it is not started.
    if (strncmp(prfRtcmInSource, "radio", sizeof(prfRtcmInSource)) == 0) {
        Serial1.begin(SERIAL1_SPEED, SERIAL_8N1, HC12_TX, HC12_RX); // UART1 object. RX, TX.
        serialState[2] = 'u';
        snprintf(outputBuffer, sizeof(outputBuffer), "Serial1 started @ %i bps.", SERIAL1_SPEED);
        logPrint(outputBuffer);
    } else {        // prfRtcmInSource is other than radio.
        serialState[2] = '-';
        logPrint("Serial1 NOT started.");
    }
    if (strncmp(prfRtcmInSource, "off", sizeof(prfRtcmInSource)) == 0) {
        RTCMinFlag = false;
    }

    // -- RTCM out is always over Serial2 (to ZED UART2).
    Serial2.begin(SERIAL2_SPEED, SERIAL_8N1, ZED_RX2, ZED_TX2);     // UART2 object. RX, TX.  
    serialState[3] = 'u';
    snprintf(outputBuffer, sizeof(outputBuffer), "Serial2 started @ %i bps.", SERIAL2_SPEED);
    logPrint(outputBuffer);
    
    // --- Display RTCM source & output. ---
    snprintf(outputBuffer, sizeof(outputBuffer), "RTCM in is \"%s\".", prfRtcmInSource);
    logPrint(outputBuffer);
    logPrint("RTCM out is Serial2 -> ZED UART2.");

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
    logPrint("Init HC-12 & laser pins.");
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
 * Wire1 is available:
 *   const uint8_t  I2C1_SDA   = 14;    // Secondary I2C bus - data.
 *   const uint8_t  I2C1_SCL   = 10;    // Secondary I2C bus - clock.
 *   if ((Wire.begin()) && (Wire1.begin(I2C1_SDA,I2C1_SCL))) {
 *   Wire1.setClock(WIRE_SPEED);
 *
 * @return void  No output is returned.
 * @since  3.0.9  [2025-12-05-05:00pm] New.
 * @since  3.0.10 [2025-12-27-07:00pm] Combine wire & wire1.
 * @since  3.0.10 [2026-01-07-10:00am] Local vars.
 * @since  3.3.1  [2026-08-17-02:45pm] Changed from Serial.print() to logPrint().
 * @since  3.3.3  [2026-08-30-05:15pm] Removed Wire1.
 * @see    setup().
 * @link   https://github.com/espressif/arduino-esp32/blob/master/libraries/Wire/src/Wire.h.
 * @link   https://docs.arduino.cc/language-reference/en/functions/communication/wire/. 
 */
void startI2C() {

    // --- Local vars. ---
    const uint8_t  I2C0_SDA   =  6;         // Primary I2C bus - data.
    const uint8_t  I2C0_SCL   =  7;         // Primary I2C bus - clock.
    const uint16_t RETRY      = 500;        // Try restarting I2C interfaces.
    const uint32_t WIRE_SPEED = 400000;     // I2C Fast mode (4kHz).

    // --- Start interfaces. ---
    if (Wire.begin()) {
        Wire.setClock(WIRE_SPEED);
        ws2812LedColor = WHITE;
        ws2812LedBlink = false;
        statusLedOn();                      // FreeRTOS task taskLoopStatusLed() has not started yet.
        logPrint("Wire started @ 4kHz.");

    } else {
        ws2812LedColor = RED;
        ws2812LedBlink = true;
        statusLedOn();                      // FreeRTOS task taskLoopStatusLed() has not started yet.
        logPrint("Wire failed to start. Retrying.");
        delay(RETRY);
        startI2C();
    };
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
        logPrint("LiPo not started. MAX17048 not detected.");
    } else {
        lipo.quickStart();          // Restart for a more accurate initial SOC guess.
        logPrint("LiPo started.");
    }
}

/**
 * -------------------------------------------------------------------------
 *  Start WiFi server (access point).
 * -------------------------------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.7  [2025-11-20-12:30pm]. New.
 * @since  3.0.10 [2026-01-07-11:00am] Local vars.
 * @since  3.0.12 [2026-01-27-04:00pm] Refactor from AP mode to AP+Station mode.
 * @since  3.0.12 [2026-02-01-05:30pm] Use preferences.
 * @since  3.2.1  [2026-07-31-12:30pm] Add Internet for NTRIP access. Refactor.
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
        ws2812LedColor = RED;                                       // Indicates error during setup(). 
        ws2812LedBlink = false;
        statusLedOn();                                              // FreeRTOS task taskLoopStatusLed() has not started yet.
        logPrint("Soft AP - config failed. Freezing ...");
        while (true) {};                                            // Freeze.
    }
    if (!WiFi.softAP(AP_SSID)) {                                    // Open access point - set SSID, omit password.
        ws2812LedColor = RED;                                       // Indicates error during setup(). 
        ws2812LedBlink = false;
        statusLedOn();                                              // FreeRTOS task taskLoopStatusLed() has not started yet.
        logPrint("Soft AP - create failed. Freezing ...");
        while (true) {};                                            // Freeze.
    }
    WiFi.softAPsetHostname(AP_NAME);                                // Set hostname.
    WiFi.onEvent(onWiFiEvent);                                      // Add event handler WiFiEvent().
    IPAddress ip = WiFi.softAPIP();                                 // Start WiFi & check status (get IP).
    snprintf(localIp, sizeof(localIp), "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
    snprintf(outputBuffer, sizeof(outputBuffer), "WiFi server \"%s\" started @ %s.", AP_SSID, localIp);
    logPrint(outputBuffer);
}

/**
 * -------------------------------------------------------------------------
 *  Start TCP server.
 * -------------------------------------------------------------------------
 *
 * Listens on both WiFi "server" and WiFi "client" interfaces.
 * Remote app like "GNSS Master" connects to server: always send NMEA out, can receive RTCM in.
 *
 * @return void No output is returned.
 * @since  3.2.3 [2026-08-10-10:45am] New.
 * @since  3.3.1 [2026-08-17-02:45pm] Changed from Serial.print() to logPrint().
 * @see    setup().
 * @see    checkTCPServer().
 */
void startTcpServer() {
    tcpServer.begin();
    tcpServer.setNoDelay(true);                        // Disable Nagle - don't batch NMEA/RTCM bytes.
    snprintf(outputBuffer, sizeof(outputBuffer), "TCP server started on port %u.", TCP_SERVER_PORT);
    logPrint(outputBuffer);
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
        snprintf(outputBuffer, sizeof(outputBuffer), "httpServer - Page \"%s\" requested.", request->url().c_str());
        logPrint(outputBuffer);
        request->send(SD_MMC, "/index.html", "text/html");      // Set root.
    }) ;

    // --- Route: file upload. ---   
    httpServer.on(PAGE_UPLOAD, HTTP_POST, [](AsyncWebServerRequest *req) {
        req->send(200, "text/plain", "Upload complete");
        if (commandFlag[DEBUG_FILE_SYSTEM]) {
            logPrint("httpServer - File upload complete.");
        }
    }, onHttpFileUpload);                                   // Register endpoint handler.

    // --- Route: file download. ---
    httpServer.on(PAGE_DOWNLOAD, HTTP_GET, [](AsyncWebServerRequest *request) { 
        if (request->hasParam("file")) {                    // Process request.
            String filename = request->getParam("file")->value();
            String filepath = "/" + filename;
            if (SD_MMC.exists(filepath)) {
                request->send(SD_MMC, filepath, "application/octet-stream", true);
                if (commandFlag[DEBUG_FILE_SYSTEM]) {
                    snprintf(outputBuffer, sizeof(outputBuffer), "httpServer - Downloading file: %s", filename.c_str());
                    logPrint(outputBuffer);
                }
            } else {
                request->send(404, "text/plain", "File not found");
                if (commandFlag[DEBUG_FILE_SYSTEM]) {
                    snprintf(outputBuffer, sizeof(outputBuffer), "File not found: %s", filename.c_str());
                    logPrint(outputBuffer);
                }
            }
        } else {
            request->send(400, "text/plain", "File parameter required");
        }
    });

    // --- Start server. ---
    httpServer.serveStatic(PAGE_ROOT, SD_MMC, PAGE_ROOT);       // File system root ("/") is on SD card.
    httpServer.begin();
    logPrint("HTTP server started on port 80.");
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
    snprintf(outputBuffer, sizeof(outputBuffer), "WebSocket server \"%s\" listening on port 80.", WEBSOCKET_SERVER_NAME);
    logPrint(outputBuffer);
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
 * @since  3.3.1  [2026-08-29-05:45pm] Added UBLOX_CFG_MSGOUT_UBX_NAV_HPPOSLLH_I2C.
 * @since  3.3.3  [2026-09-01-09:00am] Add roverGNSS.setAutoPVT(true).
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
        ws2812LedColor = RED;
        ws2812LedBlink = false;
        statusLedOn();                                              // FreeRTOS task taskLoopStatusLed() has not started yet.
        logPrint("Start roverGNSS failed. Freezing ...");           // Something is wrong, freeze.
        while (true) {};                                            // Freeze.
    } else {

        // -- Software reset. --
        roverGNSS.softwareResetGNSSOnly();
        roverGNSS.setAutoPVT(true);                                 // Create perodic output. Subsequent PVT calls will not block.
        logPrint("RoverGNSS started.\nEnumerating satellite constellations.");
        delay(1000); // Short delay to allow the module to complete the reset process.

        // uint16_t    prfGnsMsrInt;  // ZED: MEASURE every Y (e.g. 100) ms.
        // uint8_t     prfGnsNavRat;  // ZED: OUTPUT every X (e.g. 5) MEASURE intervals every (e.g. 5*100=500) ms.
        // roverGNSS.setNavigationFrequency(2) will produce 1 solution every 500ms, but only uses 2 (not 5) measurements per second.
        roverGNSS.setNavigationRate(prfGnsNavRat, VAL_LAYER_RAM);
        roverGNSS.setMeasurementRate(prfGnsMsrInt, VAL_LAYER_RAM);
        snprintf(outputBuffer, sizeof(outputBuffer), "Solution output every (%u * %u) ms.", prfGnsNavRat, prfGnsMsrInt);
        logPrint(outputBuffer);
    }

    // --- New config template. ---
    roverGNSS.newCfgValset(VAL_LAYER_RAM);                          // Save only to RAM.

    // --- Enable high precision mode. ---
    roverGNSS.addCfgValset(UBLOX_CFG_NMEA_HIGHPREC,          1);    // NMEA - High precision (7 instead of 5 decimal places for lat/lon in NMEA sentences).

    // --- Push solutions onto I2. ---
    roverGNSS.addCfgValset(UBLOX_CFG_MSGOUT_UBX_NAV_PVT_I2C, 1);    // Output solutions periodically on I2C.
    roverGNSS.addCfgValset(UBLOX_CFG_MSGOUT_UBX_NAV_HPPOSLLH_I2C, 1);   // avoid blocking polls in buildOperData()

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
    roverGNSS.sendCfgValset() ? logPrint("roverGNSS configured using valset keys.") : logPrint("roverGNSS config failed!");

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
 * @see    setup().
 * @see    onWebSocketEvent().
 * @see    processMessagesIn().
 */
void startQueues() {
    wsRxQueue = xQueueCreate(WS_RX_QUEUE_LEN, sizeof(WsQueueItem));
    if (wsRxQueue == NULL) {
        ws2812LedColor = RED;
        ws2812LedBlink = false;
        statusLedOn();                                      // FreeRTOS task taskLoopStatusLed() has not started yet.
        logPrint("Failed to create wsRxQueue. Freezing ...");
        while (true) {};           // Freeze.
    }
    logPrint("GhostRover FreeRTOS queue \"wsRxQueue\" created.");
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
 * @since  3.3.2  [2026-08-30-03:30pm] Add taskBuildOperData().
 * @since  3.3.3  [2026-08-30-05:00pm] Add taskEvery1000Ms().
 * @see    Global vars: FreeRTOS handles.
 * @see    setup().
 * @link   https://www.freertos.org/Documentation/02-Kernel/04-API-references/01-Task-creation/01-xTaskCreate.
 */
void startTasks() {

    // --- Loop status LED. ---
    xTaskCreate(taskLoopStatusLed, "Loop status LED", 2048, NULL, 2, &taskLoopStatusLedHandle);
    logPrint("FreeRTOS task \"Loop status LED\" started.");

    // --- RTCM relay. ---
    // Arduino-ESP32 core 0 defaults: WiFi/BT.
    // Arduino-ESP32 core 1 defaults: Arduino loop(), WiFi/I2C.
    // Pin taskRtcmRelay() to core 0 for parallel execution instead of round-robin in loop() since I2C calls block and don't yield.
    if (strncmp(prfRtcmInSource, "off", sizeof(prfRtcmInSource)) != 0) {    // Relay RTCM, skip for "off".
        xTaskCreatePinnedToCore(taskRtcmRelay, "RTCM_Relay", 8192, NULL, 2, &taskRtcmRelayHandle, 0);
        logPrint("FreeRTOS task \"RTCM relay\" started.");
    }
    if (strncmp(prfRtcmInSource, "off", sizeof(prfRtcmInSource)) != 0) {
        RTCMinFlag = false;
    }

    // --- Build operate data. ---
    xTaskCreate(taskBuildOperData, "Call buildOperData", 4096, NULL, 2, &taskBuildOperDataHandle);
    logPrint("FreeRTOS task \"Build operate data\" started.");

    // --- One second interval. ---
    xTaskCreate(taskEvery1000Ms, "Do these every 1 second.", 4096, NULL, 2, &taskEvery1000MsHandle);
    logPrint("FreeRTOS task \"Do these every 1 second\" started.");

    // --- Ten second interval. ---
    xTaskCreate(taskEvery10000Ms, "Do these every 10 seconds.", 4096, NULL, 2, &taskEvery10000MsHandle);
    logPrint("FreeRTOS task \"Do these every 10 seconds\" started.");

}

/**
 * -------------------------------------------------------------------------
 *  Prepare for loop().
 * -------------------------------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.7 [2025-11-21-06:00pm] Added inLoopFlag.
 * @since  3.3.1 [2026-08-17-03:45pm] Changed from Serial.print() to logPrint().
 * @since  3.3.3 [2026-09-01-09:15am] Add uptime().
 * @see    setup().
 */
void preLoop() {
    ws2812LedColor = BLUE;
    ws2812LedBlink = false;
    statusLedOn();                                      // FreeRTOS task taskLoopStatusLed() has not started yet.
    operMode[0] = 'r';
    inLoopFlag  = true;
    uptime(timeSinceBoot, sizeof(timeSinceBoot));       // Generate timestamp.
    snprintf(outputBuffer, sizeof(outputBuffer), "Time since boot: %s.\nLoop started.", timeSinceBoot);
    logPrint(outputBuffer);
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
 * @see   taskBuildOperData()   - GhostRover FreeRTOS task - Build data for operate page, trigger DevUBLOXGNSS::processNMEA().
 * @see   taskEvery1000Ms()     - GhostRover FreeRTOS task - Do these every 1 second: debug(), TBD.
 * @see   taskEvery10000Ms()    - GhostRover FreeRTOS task - Do these every 10 seconds: TBD. 
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
 * @since  3.3.2  [2026-08-30-03:30pm] Rename DELAY to LED_BLINK_INTERVAL, move to global vars.
 * @see    startTasks().
 * @see    Global vars: FreeRTOS tasks.
 * @link   https://www.freertos.org/Documentation/02-Kernel/04-API-references/02-Task-control/06-vTaskSuspend.
 */
void taskLoopStatusLed(void * pvParameters) {
    while(true) {
        statusLedOn();
        vTaskDelay(LED_BLINK_INTERVAL);
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
 * @since  3.3.0  [2026-08-13-12:30pm] Replaced ntripPushGGA() with checkLoopTimers().
 * @since  3.3.5  [2026-09-07-01:15pm] Replaced checkLoopTimers() in loop() with FreeRTOS tasks.
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
    while (true) {
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

            // - Handle connect/disconnect requests from browser (via processMessagesIn()). -

            lastNtripRtcmTime = esp_timer_get_time();                       // Local timer.

            // - Relay RTCM & push GGA while connected. -
            if (ntripClient.connected()) {
                if (ntripClient.connected()) {

                    while (ntripClient.available() > 0) {
                        char inputChar = ntripClient.read();
                        relayRtcmByte(inputChar, rtcmSentence, byteCount, bytesLeftInFrame, msg_type);
                        lastNtripRtcmTime = esp_timer_get_time();
                    }
                    if ((esp_timer_get_time() - lastNtripRtcmTime) > NTRIP_RTCM_TIMEOUT) {

                        // RTCM hangup timeout.
                        ntripClient.stop();
                        RTCMinFlag = false;
                        strlcpy(ntripStatusMsg, "DISCONNECTED: RTCM timeout.", sizeof(ntripStatusMsg));
                        if (commandFlag[DEBUG_NTRIP]) {
                            logPrint(ntripStatusMsg);
                        }
                        ntripStatusPendingFlag = true;
                    }
                } else {

                    // Caster closed the socket.
                    RTCMinFlag = false;
                    strlcpy(ntripStatusMsg, "DISCONNECTED: dropped by caster.", sizeof(ntripStatusMsg));
                    if (commandFlag[DEBUG_NTRIP]) {
                        logPrint(ntripStatusMsg);
                    }
                    ntripStatusPendingFlag = true;
                }
            }
        }

        // -- prfRtcmInSource preference is set to "bridge". --
        // RTCM in over TCP from external NTRIP client.
        if (strncmp(prfRtcmInSource, "bridge", sizeof(prfRtcmInSource)) == 0) {
            if (tcpClientConnectedFlag) {
                while (tcpClient.available() > 0) {
                    char inputChar = tcpClient.read();
                    relayRtcmByte(inputChar, rtcmSentence, byteCount, bytesLeftInFrame, msg_type);
                }
            }
        }
    }
}

/**
 * -------------------------------------------------------------------------
 *  GhostRover FreeRTOS task - Build data for operate page, trigger DevUBLOXGNSS::processNMEA().
 * -------------------------------------------------------------------------
 *
 * @param  void * pvParameters Pointer to FreeRTOS task parameters.
 * @return void   No output is returned (infinite loop).
 * @since  3.3.2 [2026-08-30-03:30pm] New.
 * @see    startTasks().
 * @see    Global vars: FreeRTOS tasks.
 * @see    buildOperData().
 * @see    DevUBLOXGNSS::processNMEA().
 */
void taskBuildOperData(void * pvParameters) {
    while(true) {
        buildOperData();
        browserUpdatePendingFlag = true;
        vTaskDelay(buildOperDataInterval);
    }
}

/**
 * -------------------------------------------------------------------------
 *  GhostRover FreeRTOS task - Do these every 1 second.
 * -------------------------------------------------------------------------
 *
 * @param  void * pvParameters Pointer to FreeRTOS task parameters.
 * @return void   No output is returned (infinite loop).
 * @since  3.3.3 [2026-08-31-05:00pm] New.
 * @see    startTasks().
 * @see    Global vars: FreeRTOS tasks.
 * @see    debug().
 * @see    any place where a DEBUG_xxx flag check may reside.
 */
void taskEvery1000Ms(void * pvParameters) {
    while(true) {

        // --- Run debug if debug flag is set.
        if(debugFlag) {
            debug();
        }

        // --- Display RTCM sentence count if on NTRIP page.
        if (strcmp(whichPage, "ntrip") == 0) {
            ntripSendRtcmSentenceCountFlag = true;
            browserUpdatePendingFlag       = true;            
        }

        vTaskDelay(EVERY_1000MS_INTERVAL);
    }
}

/**
 * -------------------------------------------------------------------------
 *  GhostRover FreeRTOS task - Do these every 10 seconds.
 * -------------------------------------------------------------------------
 *
 * @param  void * pvParameters Pointer to FreeRTOS task parameters.
 * @return void   No output is returned (infinite loop).
 * @since  3.3.3 [2026-09-06-07:15pm] New.
 * @see    startTasks().
 * @see    Global vars: FreeRTOS tasks.
 * @see    processMessagesIn().
 * @see    taskRtcmRelay().
 */
void taskEvery10000Ms(void * pvParameters) {
    while(true) {

        // --- NTRIP connected: send last $GGA. ---
        if ((ntripClient.connected()) && (ntripCaster.sendGga == true) && (lastGGA[0] != '\0')) {
            ntripClient.print(lastGGA);         // Push last $GGA to caster.
            if ((commandFlag[DEBUG_RTCM]) || (commandFlag[DEBUG_NTRIP])) {
                snprintf(outputBuffer, sizeof(outputBuffer), "Pushed last $GGA to NTRIP caster: %s", lastGGA);
                logPrint(outputBuffer);
            }
        }
        vTaskDelay(EVERY_10000MS_INTERVAL);
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
 * @since  3.3.1 [2026-08-29-03:30pm] Added IP address () assigned. Changed from Serial.print() to logPrint().
 * @see    startWiFiServer().
 * @link   https://docs.espressif.com/projects/arduino-esp32/en/latest/api/wifi.html.
 */
void onWiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info) {
    if (commandFlag[DEBUG_WIFI_EVENTS]) {

        // --- Which event? ---
        snprintf(outputBuffer, sizeof(outputBuffer), "WiFi event %d - ", event); 
        logPrint(outputBuffer);
        switch (event) {
            case ARDUINO_EVENT_WIFI_READY:              logPrint("WiFi interface ready."); break;
            case ARDUINO_EVENT_WIFI_SCAN_DONE:          logPrint("completed scan for access points."); break;
            case ARDUINO_EVENT_WIFI_AP_START:           logPrint("WiFi access point started."); break;
            case ARDUINO_EVENT_WIFI_AP_STOP:            logPrint("WiFi access point stopped."); break;
            case ARDUINO_EVENT_WIFI_AP_STACONNECTED:
                logPrint("client connected to access point.");
                displayWifiClients();
                break;
            case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED:
                logPrint("client disconnected from access point.");   
                displayWifiClients();
                break;
            case ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED:   
                snprintf(outputBuffer, sizeof(outputBuffer), "DHCP assigned IP address %u.%u.%u.%u.", 
                    IPAddress(info.wifi_ap_staipassigned.ip.addr)[0], 
                    IPAddress(info.wifi_ap_staipassigned.ip.addr)[1],
                    IPAddress(info.wifi_ap_staipassigned.ip.addr)[2],
                    IPAddress(info.wifi_ap_staipassigned.ip.addr)[3]);
                logPrint(outputBuffer); 
                break;
            case ARDUINO_EVENT_WIFI_AP_PROBEREQRECVED:  logPrint("received probe request."); break;
            case ARDUINO_EVENT_WIFI_AP_GOT_IP6:         logPrint("AP IPv6 is preferred."); break;
            case ARDUINO_EVENT_WIFI_STA_CONNECTED:      logPrint("client station is connected."); break;
            case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:   logPrint("client station is disconnected."); break;
            default:                                    logPrint("unknown event code."); break;
        }
    }
}

/**
 * Display a list of WiFi clients for soft AP.
 *
 * @return void No output is returned.
 * @since  3.3.1 [2026-08-29-04:45pm] New.
 * @see    onWiFiEvent().
 * @link   https://www.dfrobot.com/blog-1024.html
 */
void displayWifiClients() {

    // --- Get list of stations. ---
    wifi_sta_list_t stationList;
    esp_wifi_ap_get_sta_list(&stationList);

    // -- Output list of stations. --
    logPrint("WiFi clients connected:\n");
    for(int i = 0; i < stationList.num; i++) {
        wifi_sta_info_t station = stationList.sta[i];
        char macAddress[20];
        char str[3];
        for(int j = 0; j< 6; j++){
            sprintf(str, "%02x", (int)station.mac[j]);
            strcat(macAddress, str);
            if (j<5) {
                strcat(macAddress, ":");
            }
        }
        logPrint(macAddress);
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

    // --- Begin. ---
    if (index == 0) {                                       // Start.
        if (commandFlag[DEBUG_FILE_SYSTEM]) {
            logPrint("\nhttpServer endpoint \"/upload\".\nonHttpFileUpload() running.");
        }
        SD_MMC.remove("/" + filename);                          // Delete file.
        if (commandFlag[DEBUG_FILE_SYSTEM]) {
            snprintf(outputBuffer, sizeof(outputBuffer), "%s deleted on SD.", filename.c_str());
            logPrint(outputBuffer);
        }
        uploadFile = SD_MMC.open("/" + filename, FILE_WRITE);   // Open file for writing.
        if (uploadFile) {
            if (commandFlag[DEBUG_FILE_SYSTEM]) {
                snprintf(outputBuffer, sizeof(outputBuffer), "%s opened on SD.", filename.c_str());
                logPrint(outputBuffer);
            }
        } else {
            request->send(500, "text/plain", "Cannot open file for writing on SD.");
            if (commandFlag[DEBUG_FILE_SYSTEM]) {
                snprintf(outputBuffer, sizeof(outputBuffer), "Cannot open %s on SD for writing.", filename.c_str());
                logPrint(outputBuffer);
            }
            return;  
        }
    }

    // --- Continue (write data to SD). ---
    if (len) {                                              // Data chunk.                                            
        uploadFile.write(data, len);                        // Write received data to file.
        if (commandFlag[DEBUG_FILE_SYSTEM]) {
            snprintf(outputBuffer, sizeof(outputBuffer), "%u total bytes written.", (unsigned int)(index + len));
            logPrint(outputBuffer);
        }
    }

    // --- Finish. ---
    if (final) {                                            // Complete.
        uploadFile.close();
        if (commandFlag[DEBUG_FILE_SYSTEM]) {
            snprintf(outputBuffer, sizeof(outputBuffer), "%s closed on SD.", filename.c_str());
            logPrint(outputBuffer);
        }
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

            if (commandFlag[DEBUG_WS_EVENTS]) {             // Debug.
                snprintf(outputBuffer, sizeof(outputBuffer), "%s connected to WebSocket server on WS #%u.", client->remoteIP().toString().c_str(), clientId);
                logPrint(outputBuffer);
            }
            ws2812LedColor = GREEN;                         // Loop status indicator LED.
            ws2812LedBlink = false;
            wsSendCount    = 0;                             // Reset counter.
            break;
        case WS_EVT_DISCONNECT:
            if (commandFlag[DEBUG_WS_EVENTS]) {             // Debug.
                snprintf(outputBuffer, sizeof(outputBuffer), "%s disconnected to WebSocket server on WS #%u.", client->remoteIP().toString().c_str(), clientId);
                logPrint(outputBuffer);
            }
            ws2812LedColor = BLUE;
            ws2812LedBlink = false;
            wsSendCount    = 0;                             // Reset counter.
            break;
        case WS_EVT_DATA: {
                AwsFrameInfo *info = (AwsFrameInfo*)arg;
                if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {               // Full message received.
                    WsQueueItem messageItem;                                                                        // Struct - holds data & length. Keep separate from global WsQueueItem item.
                    messageItem.len = (len < sizeof(messageItem.data) - 1) ? len : sizeof(messageItem.data) - 1;    // Bounds check.
                    memcpy(messageItem.data, data, messageItem.len);
                    messageItem.data[messageItem.len] = '\0';                                                       // For debug printing.
                    if (xQueueSend(wsRxQueue, &messageItem, 0) != pdTRUE) {                                         // Non-blocking; drop if full.
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
 *  Process NMEA bytes, sentences, & counters.
 * -------------------------------------------------------------------------
 * 
 * Triggered by FreeRTOS task taskBuildOperData() -> buildOperData() -> roverGNSS.xxx() (e.g. roverGNSS.getSIV() etc.).
 * Timing is set by TickType_t buildOperDataInterval in global vars.
 * NMEA sentences are sent if tcpClientConnectedFlag (e.g. incoming connection from app like "GNSS Master" on Android).
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
 * @since  3.3.1  [2026-08-28-06:30pm] Add logic to manage nmeaBuffer if buffer gets out of sync.
 * @since  3.3.2  [2026-08-30-05:15pm] Moved DEBUG_NMEA_COUNTS from here to debug().
 * @since  3.4.0  [2026-09-12-07:15pm] Height/position lock/unlock.
 * @see    https://github.com/sparkfun/SparkFun_u-blox_GNSS_v3.
 * @link   https://docs.espressif.com/projects/arduino-esp32/en/latest/api/wifi.html.
 * @link   https://github.com/sparkfun/SparkFun_u-blox_GNSS_v3/tree/main/examples/Basics/Example2_NMEAParsing.
 */
void DevUBLOXGNSS::processNMEA(char incoming) {

    // --- Local vars. ---
    const  size_t  NMEA_MAX_SENTENCE_COUNT = 80;
    static size_t  nmeaSentenceCounter     = 0;
    static size_t  nmeaBytesCounter        = 0;
    static char    nmeaBuffer[120]         = {'\0'};
           int64_t countBeginTime          = esp_timer_get_time();

    // --- Loop check. ---
    if (!inLoopFlag) {
        return;
    }
    
    // -- Resync: '$' always starts a new sentence, regardless of prior buffer state. --
    if (incoming == '$') {
        memset(nmeaBuffer, '\0', sizeof(nmeaBuffer));
    }

    // -- Bounds check before append - never let strncat() exceed nmeaBuffer's size. --
    if (strlen(nmeaBuffer) < sizeof(nmeaBuffer) - 1) {
        strncat(nmeaBuffer, &incoming, 1);                          // Add NMEA byte from RTK-SMA to outbound buffer.
        nmeaBytesCounter++;
    } else {

        // - Overflow guard: drop this byte & wait for next '$' to resync. -
        memset(nmeaBuffer, '\0', sizeof(nmeaBuffer));
        if (commandFlag[DEBUG_NMEA] || commandFlag[DEBUG_NMEA_HEX]) {
            logPrint("nmeaBuffer overflow - discarded & resyncing.");
        }
    }

    // -- Full NMEA sentence. --
    if ((incoming == '\n') && (nmeaBuffer[0] == '$')) {

        // -- Substitute locked position/height, if active. --
        if (heightLockFlag || positionLockFlag) {
            substituteLockedNmea(nmeaBuffer, sizeof(nmeaBuffer));
        }

        // - send NMEA full sentence if tcpClientConnectedFlag (e.g. incoming connection from app like "GNSS Master" on Android).
        if (tcpClientConnectedFlag) {
            size_t bytesWritten = tcpClient.write((const uint8_t*)nmeaBuffer, strlen(nmeaBuffer));
            if (commandFlag[DEBUG_NMEA_SENT]) {
                snprintf(outputBuffer, sizeof(outputBuffer), "%u ", bytesWritten);
                logPrint(outputBuffer, true, false);     // (textToLogPrint, add to log file = true, add newline = true).
            }
            NMEAoutFlag = (bytesWritten > 0);
        }

        // -- Track NMEA stats. ---
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
        }
        if (zeroStatusCountersFlag) {
            nmeaCountAll = 0; nmeaCountGGA = 0; nmeaCountRMC = 0; nmeaCountGSA = 0;
            nmeaCountGSV = 0; nmeaCountGST = 0; nmeaCountTXT = 0; nmeaCountOther = 0;
            zeroStatusCountersFlag = false;
        }

        // -- Calculate average NMEA out rate for a block of NMEA_MAX_SENTENCE_COUNT sentences. --
        if (nmeaSentenceCounter == NMEA_MAX_SENTENCE_COUNT) {
            nmeaRate = nmeaBytesCounter / (esp_timer_get_time() - countBeginTime) * 8. * 1000000.;  // bps = (bytes/us) * 8bits/byte * 1000000us/1sec.
            nmeaBytesCounter    = 0;
            nmeaSentenceCounter = 0;
            countBeginTime      = esp_timer_get_time();         // Local timer.
        }

        // -- Debug: display NMEA sentence. ---
        if ((commandFlag[DEBUG_NMEA]) || (commandFlag[DEBUG_NMEA_HEX])) {
            snprintf(outputBuffer, sizeof(outputBuffer), "%u %s", nmeaCountAll, nmeaBuffer);
            logPrint(outputBuffer, true, false);     // (textToLogPrint, add to log file = true, add newline = true).
            if (commandFlag[DEBUG_NMEA_HEX]) {
                for (int i = 0; i < strlen(nmeaBuffer); i++) {
                    snprintf(outputBuffer, sizeof(outputBuffer), "[\"%c\" 0x%02X] ", nmeaBuffer[i], nmeaBuffer[i]);
                    logPrint(outputBuffer, true, false);
                }
            }
        }

        // -- If on NMEA page, save sentence for processMessagesIn() to send to browser. --
        if (strcmp(whichPage, "nmea") == 0) {
            strlcpy(lastNmea, nmeaBuffer, sizeof(lastNmea));
            browserUpdatePendingFlag = true;
        }

        // -- Save last $GGA full sentence for taskEvery10000Ms() to send if connected to NTRIP caster.
        if (strncmp(&nmeaBuffer[3], "GGA", 3) == 0) {
            strlcpy(lastGGA, nmeaBuffer, sizeof(lastGGA));
        }

        // -- Clear the buffer. --
        memset(nmeaBuffer, '\0', sizeof(nmeaBuffer));
    }
}

/**
 * =========================================================================
 *  Loop functions.
 * =========================================================================
 * 
 * @since 3.0.11 [2026-01-12-06:00pm] Browser initiated updates.
 * @since 3.3.0  [2026-08-13-12:00pm] Replaced checkZedTriggerUpdate() with checkLoopTimers().
 * @since 3.3.5  [2026-09-07-01:15pm] Replaced checkLoopTimers() in loop() with FreeRTOS tasks.
 * @since 3.4.0  [2026-09-13-04:30pm] Removed checkGnssLockButton(), not needed.
 * @since 3.4.0  [2026-09-20-05:30pm] Add prfLckAvgInt.
 * @see checkSerialUSB()          - Check serial USB for input.
 * @see checkFlags()              - Check flags set in other functions or FreeRTOS tasks.
 * @see processMessagesIn()       - Process queued WS messages. All JSON activity lives here & sendDataToBrowser().
 * @see checkTCPServer()          - Check TCP server for new/dropped client.
 * @see ws.cleanupClients()       - HTTP WebSocket cleanup.
 */

/**
 * -------------------------------------------------------------------------
 *  Process queued WS messages. All JSON activity lives here & sendDataToBrowser(). 
 * -------------------------------------------------------------------------
 * 
 * Operation summary:
 *  1. Pull (xQueueReceive) JSON struct (data & length) from GhostRover FreeRTOS QueueHandle_t wsRxQueue.
 *     JSON struct was pushed (xQueueSend) into GhostRover FreeRTOS QueueHandle_t wsRxQueue by onWebSocketEvent().
 *  2. If data pulled from queue, deserialize into jsonDocFromBrowser.
 *  3. Clear jsonDocToBrowser & response.
 *  4. Save browser page name as global var.
 *  5. Set global vars from jsonDocFromBrowser message. Read/set preferences if on config page.
 *  6. Fill jsonDocToBrowser with response and/or data (depends on which browser page).
 *  7. If preferences changed, restart ESP32 (restartGrMcuFlag).
 *  8. sendDataToBrowser().
 *  8.1  WebSocket send.
 * 
 * jsonDocFromBrowser is ONLY touched by this function.
 * jsonDocToBrowser & response are ONLY touched by this function and sendDataToBrowser().
 * "which" browser page is a global var but ONLY set by this function.
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
 *     8  = GNSS fix                        (uint8_t fixType).
 *     9  = GNSS satellites in view         (uint8_t numSatInView).
 *     10 = GNSS ellipsoid height           (float    heightEllipsoid    -> operBuffer[24]).
 *     11 = GNSS orthometric height         (float    heightOrthometric  -> operBuffer[24]).
 *     12 = GNSS latitude                   (double   lat                -> operBuffer[24]).
 *     13 = GNSS longitude                  (double   lon                -> operBuffer[24]).
 *     14 = GNSS horizontal accuracy        (float    accuracyHorizontal -> operBuffer[24]).
 *     15 = GNSS vertical accuracy          (float    accuracyVertical   -> operBuffer[24]).
 *     16 = RTCM in status - up/down        (bool     RTCMinFlag).
 *     17 = NMEA out status - up/down       (bool     NMEAoutFlag).
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
 *     42 = NTRIP caster active [1/2/3]     (uint8_t  prfNtripCastAct).
 *     43 = NTRIP caster id                 (struct ntripCasterProfile ntripCaster.id      - uint8_t).
 *     44 = NTRIP caster name               (struct ntripCasterProfile ntripCaster.name    - char name[48]).
 *     45 = NTRIP caster url                (struct ntripCasterProfile ntripCaster.url     - char url[48]).
 *     46 = NTRIP caster mount point        (struct ntripCasterProfile ntripCaster.mount   - char mount[24]).
 *     47 = NTRIP caster port               (struct ntripCasterProfile ntripCaster.port    - uint16_t).
 *     48 = NTRIP caster version            (struct ntripCasterProfile ntripCaster.version - uint8_t).
 *     49 = NTRIP caster user               (struct ntripCasterProfile ntripCaster.user    - char user[48]).
 *     50 = NTRIP caster password           (struct ntripCasterProfile ntripCaster.pass    - char user[48]).
 *     51 = NTRIP caster sendGga            (struct ntripCasterProfile ntripCaster.sendGga - bool ).
 *     52 = GNSS lock averaging interval    (uint16_t prfLckAvgInt).
 *     53 = NTRIP caster crs                (struct ntripCasterProfile ntripCaster.crs     - char mount[24]).        // Newest pref.
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
 *       "52":10,
 *       "39":"{\"43\":1,\"44\":\"PointPerfect (SparkPNT)\",\"45\":\"ppntrip.services.u-blox.com\",\"46\":\"NEAR-RTCM\",\"47\":2101,\"48\":1,\"49\":\"abcdefghijkl\",\"50\":\"abcdefghij\,\"51\":1}",
 *       "40":"{\"43\":2,\"44\":\"\",\"45\":\"\",\"46\":\"\",\"47\":2101,\"48\":1,\"49\":\"\",\"50\":\"\",\"51\":1},
 *       "41":"{\"43\":3,\"44\":\"\",\"45\":\"\",\"46\":\"\",\"47\":2101,\"48\":1,\"49\":\"\",\"50\":\"\",\"51\":1}",
 *       "42":"1".
 *
 *  -- NTRIP CASTER PREFERENCE. --
 *       "setNtripCasterPref":"{\"43\":1,\"44\":\"PointPerfect (SparkPNT)\",\"45\":\"ppntrip.services.u-blox.com\",\"46\":\"NEAR-RTCM\",\"47\":2101,\"48\":1,\"49\":\"abcdefghijkl\",\"50\":\"abcdefghij\",\"51\":1,\"53\":\"abcdefghij\"}",
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
 *     - Hello: Internet already connected. -
 *       browser (sends)    --> {"page:"menu","sendPrefs":""}.
 *       browser (receives) <-- {"sendPrefsResp":"Preferences sent.",ALL PREFERENCES}.
 *       browser (receives) <-- {"connectInternetResp":"Still CONNECTED to Internet via \"prfHotSsi\" as \"hotspotIp\""}.
 * 
 *     - Connect/disconnect Internet. -
 *       browser (sends)    --> {"connectInternet":""}.
 *       browser (receives) <-- {"connectInternetResp":"Connect to Internet via \"prfHotSsi\""}.
 *       browser (receives) <-- {"connectInternetResp":"Attempt x of y"}. Repeat for each "x" of max "y" attempts.
 *       browser (receives) <-- {"connectInternetResp":"Connect ABORTED"}. Max attempts exceeded. Connect failed.
 *       browser (receives) <-- {"connectInternetResp":"CONNECTED to Internet via \"prfHotSsi\" as \"hotspotIp\""}. Connect success.
 *       browser (sends)    --> {"disconnectInternet":""}.
 *       browser (sends)    --> {"disconnectInternetResp":"Internet DISCONNECTED."}
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
 *       browser (sends)    --> {"laserOn:""}.
 *       browser (receives) <-- {"laserOnResp":"Laser on."}.
 *       browser (receives) <-- {"laserOnResp":"Laser on."}.                // Periodically sent.
 *       browser (sends)    --> {"laserOff:""}.
 *       browser (receives) <-- {"laserOffResp":"Laser off."}.
 *
 *     - Height lock/unlock button. --
 *       browser (sends)    --> {"heightLock:""}.
 *       browser (receives) <-- {"heightLockResp":"Height lock started - averaging."}.
 *       browser (receives) <-- {"heightLockResp":"Height locked.""}.       // Periodically sent.
 *       browser (sends)    --> {"heightUnlock:""}.
 *       browser (receives) <-- {"heightUnlockResp":"Height unlocked."}.
 *
 *     - Position lock/unlock button. --
 *       browser (sends)    --> {"positionLock:""}.
 *       browser (receives) <-- {"positionLockResp":"Position lock started - averaging."}.
 *       browser (receives) <-- {"positionLockResp":"Position locked.""}.   // Periodically sent.
 *       browser (sends)    --> {"positionUnlock:""}.
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
 *
 *     - Hello: NTRIP client already connected. -
 *       browser (sends)    --> {"page:"ntrip","sendPrefs":""}.
 *       browser (receives) <-- {"sendPrefsResp":"Preferences sent.",ALL PREFERENCES}.
 *       browser (receives) <-- {"connectNtripCasterResp":"NTRIP CONNECTED: url:port @ mount"}.
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
 *  -- Instructions page. --
 *     - Hello. -
 *
 *  -- Wiring page. --
 *     - Hello. -
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
 * @since 3.2.1  [2026-07-30-10:45am] Implement FreeRTOS queues: refactor onWebSocketMessage() into processMessagesIn().
 *                Fix cross-task race on shared JsonDocuments causing intermittent LoadProhibited/heap-corruption crashes.
 * @since 3.2.1  [2026-07-30-11:45am] Set page name global var.
 * @since 3.2.1  [2026-07-31-01:30pm] Moved "Wrap up" section from here to sendDataToBrowser().
 * @since 3.2.1  [2026-08-03-10:00am] Removed jsonObj["21'] & jsonObj["21'].
 * @since 3.2.2  [2026-08-09-01:00pm] Add NTRIP client: add NTRIP Connect/Disconnect logic, add logic for "// Step 3/3: Forward pending NTRIP status update to browser.""
 * @since 3.3.2  [2026-08-31-09:30am] Moved browserUpdatePendingFlag check to checkFlags() in loop().
 * @since 3.3.3  [2026-09-03-04:45pm] Moved ntripBeginClient() to here.
 * @since 3.3.5  [2026-09-11-02:30pm] Tweaked NTRIP WiFi & Caster connection management.
 * @since 3.4.0  [2026-09-12-07:15pm] Height/position lock/unlock.
 * @since 3.4.0  [2026-09-19-12:15pm] Add debug processing.
 * @since 3.4.0  [2026-09-20-05:30pm] Add prfLckAvgInt.
 * @see   Global vars: GNSS
 * @see   prefUtility().
 * @see   onWebSocketEvent().
 * @see   startWebSocketServer().
 * @see   taskRtcmRelay().
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
 void  processMessagesIn() {      // From browser, to browser, periodic.

    // --- Debug. ---
    // Cannot use print()/println()/printf() to debug JSON doc value. Use these lines:
    // serializeJson(jsonDocToBrowser, Serial); // Debug.
    // Serial.println();

    // --- Local vars. ---
    // jsonDocFromBrowser, jsonDocToBrowser, &  JsonDocNtrip are global vars.


    // -------------------------------------------------------------------------
    // --- If incoming WebSocket message is NOT queued, return. ---
    // -------------------------------------------------------------------------
    if (xQueueReceive(wsRxQueue, &item, 0) != pdTRUE) {     // If WebSocket message from browser, add to queue.
        return;
    }

    // --- Debug. Print data received. ---
    if (commandFlag[DEBUG_WS_TRAFFIC]) {        // Debug. WebSocket data from browser to GR.
        Serial.printf("WS #%u: browser --> %s\n", clientId, item.data);
    }

    // --- WebSocket message - deserialize the JSON data into a JSON document (jsonDocFromBrowser). ---
    jsonDocFromBrowser.clear();
    DeserializationError error = deserializeJson(jsonDocFromBrowser, item.data, item.len);
    if (error) {
        snprintf(outputBuffer, sizeof(outputBuffer), "JSON deserialize failed: %s", error.f_str());
        logPrint(outputBuffer);
        return;
    }
    memset(outputBuffer, '\0', sizeof(outputBuffer));
    jsonDocToBrowser.clear();

    // --- Set page name global var. ---
    if (jsonDocFromBrowser["page"].is<JsonVariant>()) {                     // Does key exist?
        strlcpy(whichPage, jsonDocFromBrowser["page"], sizeof(whichPage));  // Important global used in loop().
        snprintf(outputBuffer, sizeof(outputBuffer), "Page loaded: \"%s\" ", whichPage);
        logPrint(outputBuffer);
    }

    // -------------------------------------------------------------------------
    // --- All pages. Send all preferences to browser. ---
    // -------------------------------------------------------------------------

    if (jsonDocFromBrowser["sendPrefs"].is<JsonVariant>()) {

        // -- Set global vars from preferences. --
        prefUtility(PREF_READ);

        // -- Set JSON values from global vars. --
        jsonDocToBrowser["0"]  = buildString;
        jsonDocToBrowser["1"]  = prfUnt;
        jsonDocToBrowser["2"]  = prfRtcmInSource;
        jsonDocToBrowser["4"]  = prfGnsMsrInt;
        jsonDocToBrowser["5"]  = prfGnsNavRat;
        jsonDocToBrowser["6"]  = prfHotSsi;
        jsonDocToBrowser["7"]  = prfHotPas;
        jsonDocToBrowser["35"] = clientId;
        jsonDocToBrowser["36"] = prfInstrHgt;
        jsonDocToBrowser["52"] = prfLckAvgInt;      // Newest pref. 2.
        jsonDocToBrowser["39"] = prfNtripCastAttr[0];
        jsonDocToBrowser["40"] = prfNtripCastAttr[1];
        jsonDocToBrowser["41"] = prfNtripCastAttr[2];
        jsonDocToBrowser["42"] = prfNtripCastAct;

        // -- Set response. --
        strcpy(outputBuffer, "Preferences sent.");
        jsonDocToBrowser["sendPrefsResp"] = outputBuffer;
    }

    // -------------------------------------------------------------------------
    // --- Config page. Set all preferences. ---
    // -------------------------------------------------------------------------
    if (jsonDocFromBrowser["setPrefs"].is<JsonVariant>()) {

        // -- Set global vars from JSON values. --
        strlcpy(prfUnt,          jsonDocFromBrowser["1"],  sizeof(prfUnt));  // dst, src, sizeof(dest)
        strlcpy(prfRtcmInSource, jsonDocFromBrowser["2"],  sizeof(prfRtcmInSource));
        strlcpy(prfHotSsi,       jsonDocFromBrowser["6"],  sizeof(prfHotSsi));
        strlcpy(prfHotPas,       jsonDocFromBrowser["7"],  sizeof(prfHotPas));
        prfNtripCastAct = jsonDocFromBrowser["42"];   
        prfGnsNavRat    = jsonDocFromBrowser["5"];
        prfGnsMsrInt    = jsonDocFromBrowser["4"];
        prfInstrHgt     = jsonDocFromBrowser["36"];
        prfLckAvgInt    = jsonDocFromBrowser["52"];     // Newest pref. 2.

        // -- Set new preferences from global vars. --
        prefUtility(PREF_SET);

        // -- Set response. --
        strcpy(outputBuffer, "Preferences saved.");
        jsonDocToBrowser["setPrefsResp"] = outputBuffer;
    }

    // -------------------------------------------------------------------------
    // --- Config page. Reset all preferences to defaults. ---
    // -------------------------------------------------------------------------
    if (jsonDocFromBrowser["resetPrefs"].is<JsonVariant>()) {

        // -- Set global vars to defaults. --
        prefUtility(PREF_RESET);

        // -- Set response. --
        strcpy(outputBuffer, "Preferences reset.");
        jsonDocToBrowser["resetPrefsResp"] = outputBuffer;
    }

    // -------------------------------------------------------------------------
    // --- Config page. Set NTRIP preference. ---
    // -------------------------------------------------------------------------
    if (jsonDocFromBrowser["setNtripCasterPref"].is<JsonVariant>()) {

        // -- Set new NTRIP preference. --
        prefUtility(PREF_SET_NTRIP);

        // -- Set response. --
        strcpy(outputBuffer, "Preference updated.");
        jsonDocToBrowser["setNtripCasterPrefResp"] = outputBuffer;
    }

    // -------------------------------------------------------------------------
    // --- Files page. List files. ---
    // -------------------------------------------------------------------------
    if (jsonDocFromBrowser["listFiles"].is<JsonVariant>()) {

        // -- Set JSON value: list of files. --
        memset(outputBuffer, '\0', sizeof(outputBuffer));
        File root = SD_MMC.open("/");
        File file = root.openNextFile();
        while(file) {
            if (strlen(outputBuffer) + strlen(file.name()) + 2 < sizeof(outputBuffer)) {       
                if ((file.name()[0] != '.') && (file.name() != "") && (!file.isDirectory())) {
                    // TODO: Flat fs for now, add directories & recursive call.
                    strcat(outputBuffer, "/");
                    strcat(outputBuffer, file.name());
                    strcat(outputBuffer, ",");
                }
            }
            file = root.openNextFile();
        }
        jsonDocToBrowser["fileList"] = outputBuffer;

        // -- Set response. --
        strcpy(outputBuffer, "Files listed.");
        jsonDocToBrowser["listFilesResp"] = outputBuffer;
    }

    // -------------------------------------------------------------------------
    // --- Files page. Delete file. ---
    // -------------------------------------------------------------------------

    if (jsonDocFromBrowser["deleteFile"].is<JsonVariant>()) {

        // -- Delete file. --
        const char* fileName = jsonDocFromBrowser["deleteFile"];
        strcpy(outputBuffer, fileName);
        strcat(outputBuffer, SD_MMC.remove(fileName) ? " deleted." : " NOT deleted.");

        // -- Set response. --
        jsonDocToBrowser["deleteFileResp"] = outputBuffer;
    }

    // -------------------------------------------------------------------------
    // --- Menu page. Restart GR-MCU. ---
    // -------------------------------------------------------------------------
    if (jsonDocFromBrowser["restartGR-MCU"].is<JsonVariant>()) {

        // -- Set response. --
        strcpy(outputBuffer, "GR-MCU will restart.");
        jsonDocToBrowser["restartGR-MCUResp"] = outputBuffer;
        restartGrMcuFlag = true;
    }

    // -------------------------------------------------------------------------
    // --- NMEA page. NMEA sentences. ---
    // -------------------------------------------------------------------------
    // task taskBuildOperData() -> buildOperData() -> DevUBLOXGNSS::processNMEA() sets browserUpdatePendingFlag = true; -> sendDataToBrowser().

    // -------------------------------------------------------------------------
    // --- Operate page. GNSS data. ---
    // -------------------------------------------------------------------------
    // task taskBuildOperData() -> buildOperData() sets browserUpdatePendingFlag = true; -> sendDataToBrowser().

    // -------------------------------------------------------------------------
    // --- Operate page. Laser on/off button. ---
    // -------------------------------------------------------------------------
    //   @link https://www.build-electronic-circuits.com/arduino-laser-module-ky-008/.
    //   @link https://docs.sparkfun.com/SparkFun_Thing_Plus_ESP32-S3/arduino_example/#rgb-led.
    if (jsonDocFromBrowser["laserOn"].is<JsonVariant>()) {
        digitalWrite(LSR_TRIGGER, HIGH);        // Turn laser on.
        laserOnFlag = true;

        // -- Set response. --
        strcpy(outputBuffer, "Laser on");
        jsonDocToBrowser["laserOnResp"] = outputBuffer;
        snprintf(outputBuffer, sizeof(outputBuffer), "%s.", outputBuffer);
        if (commandFlag[DEBUG_BTNS]) {                      // Debug.
            logPrint(outputBuffer);
        }
    }
    if (jsonDocFromBrowser["laserOff"].is<JsonVariant>()) {
        digitalWrite(LSR_TRIGGER, LOW);         // Turn laser off.
        laserOnFlag = false;

        // -- Set response. --
        strcpy(outputBuffer, "Laser off");
        jsonDocToBrowser["laserOffResp"] = outputBuffer;
        snprintf(outputBuffer, sizeof(outputBuffer), "%s.", outputBuffer);
        if (commandFlag[DEBUG_BTNS]) {                      // Debug.
            logPrint(outputBuffer);
        }
    }

    // -------------------------------------------------------------------------
    // --- Operate page. Height lock/unlock button. ---
    // -------------------------------------------------------------------------
    if (jsonDocFromBrowser["heightLock"].is<JsonVariant>()) {
        heightLockSumOrtho       = 0;
        heightLockSumGeoidSep    = 0;
        heightLockSampleCount    = 0;
        heightLockAveragingStart = esp_timer_get_time();
        heightLockAveragingFlag  = true;
        heightLockFlag           = false;                   // Not locked yet - averaging first.

        // -- Set response. --
        strcpy(outputBuffer, "Height lock started - averaging.");
        jsonDocToBrowser["heightLockResp"] = outputBuffer;
        if (commandFlag[DEBUG_BTNS]) {                      // Debug.
            logPrint(outputBuffer);
        }
    }
    if (jsonDocFromBrowser["heightUnlock"].is<JsonVariant>()) {
        heightLockFlag          = false;
        heightLockAveragingFlag = false;

        // -- Set response. --
        strcpy(outputBuffer, "Height unlocked.");
        jsonDocToBrowser["heightUnlockResp"] = outputBuffer;
        if (commandFlag[DEBUG_BTNS]) {                      // Debug.
            logPrint(outputBuffer);
        }
    }

    // -------------------------------------------------------------------------
    // --- Operate page. Position lock/unlock button. ---
    // -------------------------------------------------------------------------
    if (jsonDocFromBrowser["positionLock"].is<JsonVariant>()) {
        positionLockSumLat         = 0;
        positionLockSumLon         = 0;
        positionLockSampleCount    = 0;
        positionLockAveragingStart = esp_timer_get_time();
        positionLockAveragingFlag  = true;
        positionLockFlag           = false;

        // -- Set response. --
        strcpy(outputBuffer, "Position lock started - averaging.");
        jsonDocToBrowser["positionLockResp"] = outputBuffer;
        if (commandFlag[DEBUG_BTNS]) {                      // Debug.
            logPrint(outputBuffer);
        }
    }
    if (jsonDocFromBrowser["positionUnlock"].is<JsonVariant>()) {
        positionLockFlag          = false;
        positionLockAveragingFlag = false;

        // -- Set response. --    
        strcpy(outputBuffer, "Position unlocked.");
        jsonDocToBrowser["positionUnlockResp"] = outputBuffer;
        if (commandFlag[DEBUG_BTNS]) {                      // Debug.
            logPrint(outputBuffer);
        }
    }

    // -------------------------------------------------------------------------
    // --- Menu page. Internet already connected. ---
    // -------------------------------------------------------------------------
    if (jsonDocFromBrowser["sendPrefs"].is<JsonVariant>() && (strcmp(whichPage, "menu") == 0) && (WiFi.status() == WL_CONNECTED)) {

        // -- Was on NTRIP page, returned. WiFi still connected. --
        sendDataToBrowser();                                                                            // Send prefs WS message.
        jsonDocToBrowser.clear();
        if (commandFlag[DEBUG_WIFI_EVENTS]) {
            logPrint("Sent prefs message.");
        }

        // -- Set response. --
        snprintf(outputBuffer, sizeof(outputBuffer), "Still CONNECTED to Internet via %s as %s.", prfHotSsi, hotspotIp);    // Second WS message. Triggers UI.
        jsonDocToBrowser["connectInternetResp"] = outputBuffer;
        if (commandFlag[DEBUG_WIFI_EVENTS]) {
            logPrint(outputBuffer);
        }
    }

    // -------------------------------------------------------------------------
    // --- Menu page. Connect Internet. ---
    // -------------------------------------------------------------------------
    if ((jsonDocFromBrowser["connectInternet"].is<JsonVariant>()) && (WiFi.status() != WL_CONNECTED)) {

        // -- Local vars. --
        size_t numWifiTrys = 1;                         // Count connect attempts to STA_SSID.
        size_t maxWifiTrys = 10;                        // Max # of trys to connect to STA_SSID, one per second.
        IPAddress STA_IP(172, 20, 10, 2);               // Request this IP address.

        // -- Set response. Send to browser. --
        snprintf(outputBuffer, sizeof(outputBuffer), "Connect to Internet via \"%s\".", prfHotSsi);
        jsonDocToBrowser["connectInternetResp"] = outputBuffer;
        sendDataToBrowser();                            // Send now.
        jsonDocToBrowser.clear();
        if (commandFlag[DEBUG_WIFI_EVENTS]) {
            logPrint(outputBuffer);
        }

        // -- Configure & start Internet for RTCMin via internal Internet NTRIP caster. --
        WiFi.begin(prfHotSsi, prfHotPas);
        for (numWifiTrys; numWifiTrys <= maxWifiTrys; numWifiTrys++) {

            // -- Set response. Send to browser. --
            snprintf(outputBuffer, sizeof(outputBuffer), "Attempt %d of %d.", numWifiTrys, maxWifiTrys);
            jsonDocToBrowser["connectInternetResp"] = outputBuffer;
            sendDataToBrowser();                        // Send now.
            jsonDocToBrowser.clear();
            if (commandFlag[DEBUG_WIFI_EVENTS]) {
                logPrint(outputBuffer);
            }

            // -- Connected? --
            if (WiFi.status() == WL_CONNECTED) {
                break;
            }
            delay(1000);                                // Try again.
        }

        if (WiFi.status() == WL_CONNECTED) {
            ws2812LedColor = WHITE;                     // Indicates no error during setup(). 
            ws2812LedBlink = false;

            // -- Set response. --
            strlcpy(hotspotIp, WiFi.localIP().toString().c_str(), sizeof(hotspotIp));
            snprintf(outputBuffer, sizeof(outputBuffer), "CONNECTED to Internet via \"%s\" as %s.", prfHotSsi, hotspotIp);
            jsonDocToBrowser["connectInternetResp"] = outputBuffer;
            if (commandFlag[DEBUG_WIFI_EVENTS]) {
                logPrint(outputBuffer);
            }
        } else {
            memset(hotspotIp, '\0', sizeof(hotspotIp));
            WiFi.disconnect();

            // -- Set response. --
            snprintf(outputBuffer, sizeof(outputBuffer), "Connect to Internet ABORTED.");
            jsonDocToBrowser["connectInternetResp"] = outputBuffer;
            if (commandFlag[DEBUG_WIFI_EVENTS]) {
                logPrint(outputBuffer);
            }
        }
    }

    // -------------------------------------------------------------------------
    // --- Menu page. Disconnect Internet. ---
    // -------------------------------------------------------------------------
    if (jsonDocFromBrowser["disconnectInternet"].is<JsonVariant>()) {
        WiFi.disconnect();

        // -- Set response. --
        snprintf(outputBuffer, sizeof(outputBuffer), "Internet DISCONNECTED.");
        jsonDocToBrowser["disconnectInternetResp"] = outputBuffer;
        if (commandFlag[DEBUG_WIFI_EVENTS]) {
            logPrint(outputBuffer);
        }
    }

    // -------------------------------------------------------------------------
    // --- NTRIP page. NTRIP caster already connected. ---
    // -------------------------------------------------------------------------
    if (jsonDocFromBrowser["sendPrefs"].is<JsonVariant>() && (strcmp(whichPage, "ntrip") == 0) && (ntripClient.connected())) {

        // -- Was on NTRIP page, returned. NTRIP still connected. --
        sendDataToBrowser();                                                                // Send prefs WS message.
        jsonDocToBrowser.clear();

        // -- Set response. --
        snprintf(outputBuffer, sizeof(outputBuffer), "NTRIP still CONNECTED to %s:%d @ %s.", ntripCaster.url, ntripCaster.port, ntripCaster.mount);
        if (commandFlag[DEBUG_NTRIP]) {
            logPrint(outputBuffer);
        }
        jsonDocToBrowser["connectNtripCasterResp"] = outputBuffer;
    }

    // -------------------------------------------------------------------------
    // --- NTRIP page. Connect to NTRIP caster. ---
    // -------------------------------------------------------------------------
    
    if (jsonDocFromBrowser["connectNtripCaster"].is<JsonVariant>() && (!ntripClient.connected())) {     

        // --- Local vars. ---
        const uint16_t REQUEST_BUF_LEN                  = 512;
        const uint16_t RESPONSE_BUF_LEN                 = 512;
        const int64_t  CASTER_TIMEOUT                   = 5000000;      // Time (us) to wait for caster response (5 sec).
              bool     connected                        = false;
              char     serverRequest[REQUEST_BUF_LEN]   = {'\0'};
              char     credentials[REQUEST_BUF_LEN]     = {'\0'};
              char     casterResponse[RESPONSE_BUF_LEN] = {'\0'};
              size_t   responseSpot                     = 0;
              int      connectionResult                 = 0;
              int64_t  startTime;

        // -- Open socket connection. --
        connected = ntripClient.connect(ntripCaster.url, ntripCaster.port);  // Globals set/read in prefUtility().

        // -- Set response. Send to browser. --
        if (connected) {
            snprintf(outputBuffer, sizeof(outputBuffer), "CONNECTED to NTRIP %s:%d %s (v%d)!",
                ntripCaster.url, ntripCaster.port, ntripCaster.mount, ntripCaster.version);
        } else {
            snprintf(outputBuffer, sizeof(outputBuffer), "Connect to %s:%d %s (v%d) FAILED.",
                ntripCaster.url, ntripCaster.port, ntripCaster.mount, ntripCaster.version);
            connected = false;
        }
        jsonDocToBrowser["connectNtripCasterResp"] = outputBuffer;
        sendDataToBrowser();                        // Send now.
        jsonDocToBrowser.clear();
        if (commandFlag[DEBUG_NTRIP]) {
            logPrint(outputBuffer);
        }

        // -- Continue? --
        if (!connected) {
            return;
        }

        // -- Build request with base64-encode credentials (Basic Auth), if provided. --
        snprintf(serverRequest, REQUEST_BUF_LEN,
            "GET /%s HTTP/1.0\r\nUser-Agent: NTRIP GhostRover Client v1.0\r\n", ntripCaster.mount);
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

        // -- Send request. --
        ntripClient.write(serverRequest, strlen(serverRequest));

        // -- Set response. Send to browser. --
        snprintf(outputBuffer, sizeof(outputBuffer), "Authenticating ...");
        jsonDocToBrowser["connectNtripCasterResp"] = outputBuffer;
        sendDataToBrowser();                        // Send now.
        jsonDocToBrowser.clear();
        if (commandFlag[DEBUG_NTRIP]) {
            logPrint(outputBuffer);
        }

        // -- Wait for response. --
        startTime = esp_timer_get_time();       // Local watch for timeout.
        while (ntripClient.available() == 0) {

            // - Timed out waiting for caster's HTTP response. -
            if ((esp_timer_get_time() - startTime) > CASTER_TIMEOUT) {
                ntripClient.stop();
                connected = false;
            }
        }

        // -- Continue? --
        if (!connected) {
            snprintf(outputBuffer, sizeof(outputBuffer), "Authentication FAILED. Response timed out.\n");
            jsonDocToBrowser["connectNtripCasterResp"] = outputBuffer;
            sendDataToBrowser();                        // Send now.
            jsonDocToBrowser.clear();
            if (commandFlag[DEBUG_NTRIP]) {
                logPrint(outputBuffer);
            }
            return;
        }

        // -- Read response. Look for OK (200). Unauthorized is (401). ---
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

        // -- Credentials accepted? Set response. --
        if (connectionResult == 200) {          // Success.
            rtcmSentenceCount = 0;

            // Send first $GGA now, then taskEvery10000Ms will send one automatically.
            snprintf(outputBuffer, sizeof(outputBuffer), "SUCCESS, sending $GGA to start.\n");
            if ((ntripClient.connected()) && (ntripCaster.sendGga == true) && (lastGGA[0] != '\0')) {
                ntripClient.print(lastGGA);         // Push last $GGA to caster.
            }
        } else {
            ntripClient.stop();                 // Something went wrong.
            snprintf(outputBuffer, sizeof(outputBuffer), "REJECTED, response code %s.\n", casterResponse);
        }
        jsonDocToBrowser["connectNtripCasterResp"] = outputBuffer;
        if (commandFlag[DEBUG_NTRIP]) {
            logPrint(outputBuffer);
        }
    }

    // -------------------------------------------------------------------------
    // --- NTRIP page. Disconnect NTRIP caster. ---
    // -------------------------------------------------------------------------
    if (jsonDocFromBrowser["disconnectNtripCaster"].is<JsonVariant>()) {
        ntripClient.stop();
        RTCMinFlag = false;

        // -- Set response. --
        strlcpy(outputBuffer, "NTRIP DISCONNECTED by browser.", sizeof(outputBuffer));
        jsonDocToBrowser["disconnectNtripCasterResp"] = outputBuffer;
        if (commandFlag[DEBUG_NTRIP]) {
            logPrint(outputBuffer);
        }
    }

    // -------------------------------------------------------------------------
    // --- Debug page. Turn on debug. ---
    // -------------------------------------------------------------------------
    if (jsonDocFromBrowser["startDebug"].is<JsonVariant>()) {

        // -- Turn on debug. --
        const char* whichDebug = jsonDocFromBrowser["startDebug"];
        uint8_t debugNum;
        for (debugNum = 0; debugNum < NUM_COMMANDS; debugNum++) {
            if (strcmp(COMMAND[debugNum], whichDebug) == 0) {  // Find the enum value.
                commandFlag[debugNum] = true;
                debugFlag             = true;
                break;
            }
        }

        // -- Set response. --
        snprintf(outputBuffer, sizeof(outputBuffer), "%s (#%u) debug started.", whichDebug, debugNum);
        jsonDocToBrowser["startDebugResp"] = outputBuffer;
    }

    // -------------------------------------------------------------------------
    // --- Debug page. Turn off debug. ---
    // -------------------------------------------------------------------------
    if (jsonDocFromBrowser["stopDebug"].is<JsonVariant>()) {

        // -- Turn off debug. --
        const char* whichDebug = jsonDocFromBrowser["stopDebug"];
        uint8_t debugNum;
        for (debugNum = 0; debugNum < NUM_COMMANDS; debugNum++) {
            if (strcmp(COMMAND[debugNum], whichDebug) == 0) {  // Find the enum value.
                commandFlag[debugNum] = false;
                debugFlag             = false;
                break;
            }
        }

        // -- Set response. --
        snprintf(outputBuffer, sizeof(outputBuffer), "%s (#%u) debug stopped.", whichDebug, debugNum);
        jsonDocToBrowser["stopDebugResp"] = outputBuffer;
    }

    // -------------------------------------------------------------------------
    // --- Test. Echo. ---
    // -------------------------------------------------------------------------
    if (jsonDocFromBrowser["echo"].is<JsonVariant>()) {

        // -- Set JSON value. --
        jsonDocToBrowser["echo"] = jsonDocFromBrowser["echo"];

        // -- Set response. --
        strcpy(outputBuffer, "Message echoed.");
        jsonDocToBrowser["echoResp"] = outputBuffer;
    }

    // -------------------------------------------------------------------------
    // --- Send data to browser. ---
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
            zeroStatusCountersFlag = true;
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
 * -------------------------------------------------------------------------
 *  Check TCP server for new/dropped client. 
 * -------------------------------------------------------------------------
 *
 * Remote app like "GNSS Master" connects to server: always send NMEA out, can receive RTCM in.
 * 
 * Single-client design: incoming connection always replaces whatever
 * client is currently held, so a reconnect (e.g. phone WiFi toggled)
 * doesn't get stuck behind a dead socket.
 *
 * @return void No output is returned.
 * @since  3.2.2 [2026-08-10-10:15am] New.
 * @since  3.3.1 [2026-08-29-09:00pm] Refactored.
 * @see    loop().
 * @see    startTcpServer().
 */
void checkTCPServer() {

    // --- Accept new client connect, replace pre-existing. ---
    if (tcpServer.hasClient()) {
        if (tcpClient) {
            tcpClient.stop();                          // Drop old client.
        }
        tcpClient = tcpServer.available();
        tcpClient.setNoDelay(true);
        tcpClientConnectedFlag = true;
        snprintf(outputBuffer, sizeof(outputBuffer), "TCP client connected: %s", tcpClient.remoteIP().toString().c_str());
        logPrint(outputBuffer);
    }

    // --- Detect client disconnect. ---
    if (tcpClientConnectedFlag && !tcpClient.connected()) {
        tcpClientConnectedFlag = false;
        tcpClient.stop();
        NMEAoutFlag = false;
        logPrint("TCP client disconnected.");
    }
}

/**
 * -------------------------------------------------------------------------
 *  Check flags set in other functions or FreeRTOS tasks.
 * -------------------------------------------------------------------------
 *
 * @return void No output is returned.
 * @since  3.3.0 [2026-08-15-12:00pm] New.
 * @since  3.3.2 [2026-08-31-09:30am] Moved browserUpdatePendingFlag from processMessagesIn().
 * @see    loop().
 */
void checkFlags() {

    // --- Restart GR MCU. ---
    if (restartGrMcuFlag) {
        delay(1000);
        esp_restart();
    }

    // --- If periodic status update is pending, send to browser page. ---
    if (browserUpdatePendingFlag || ntripStatusPendingFlag) {
        jsonDocToBrowser.clear();       // Globals are set & will be sent. Ensure clean JSON doc for al periodic updates.
        sendDataToBrowser();            // Send now.
        browserUpdatePendingFlag = false;
        return;
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
 * @since  3.3.0  [2026-08-13-01:45pm] Move timer logic to checkLoopTimers().
 * @since  3.3.2  [2026-08-30-04:45pm] Add DEBUG_HEAP.
 * @since  3.3.3  [2026-09-07-11:30pm] Add DEBUG_LOOP_STACK.
 * @since  3.3.5  [2026-09-07-01:15pm] Replaced checkLoopTimers() in loop() with FreeRTOS tasks.
 * @see    checkSerialUSB().
 * @see    taskEvery1000Ms().
 */
void debug() {

    if (!Serial) {                                          // Nothing to see, move on.
        return;
    }

    // --- Uptime. ---
    if (commandFlag[SHOW_UPTIME]) {
        uptime(timeSinceBoot, sizeof(timeSinceBoot));       // Generate timestamp.
        logPrint(timeSinceBoot);
    }

    // --- Heap. ---
    if (commandFlag[DEBUG_HEAP]) {
        snprintf(outputBuffer, sizeof(outputBuffer), "heap: %u  min: %u", ESP.getFreeHeap(), ESP.getMinFreeHeap());
        logPrint(outputBuffer);
    }

if (commandFlag[DEBUG_LOOP_STACK]) {
    UBaseType_t remaining = uxTaskGetStackHighWaterMark(loopTaskHandle);
    snprintf(outputBuffer, sizeof(outputBuffer), "Stack headroom - %u bytes remain of %u total bytes.",
        remaining * sizeof(StackType_t), getArduinoLoopTaskStackSize());
    logPrint(outputBuffer);
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
    // @see relayRtcmByte().

    // --- GNSS. ---
    if (commandFlag[DEBUG_GNSS]) {
        roverGNSS.enableDebugging();    // "Pipe all NMEA sentences to serial USB."
    } else {
        roverGNSS.disableDebugging();
    }

    // --- Display NMEA counts. ---
    if (commandFlag[DEBUG_NMEA_COUNTS]) {
        snprintf(outputBuffer, sizeof(outputBuffer), "All=%u, GGA=%u, RMC=%u, GSA=%u, GSV=%u, GST=%u, TXT=%u, other=%u.",
            nmeaCountAll, nmeaCountGGA, nmeaCountRMC, nmeaCountGSA, nmeaCountGSV, nmeaCountGST, nmeaCountTXT, nmeaCountOther);
        logPrint(outputBuffer);
    }

    // --- NMEA sentences. ---
    // @see "if (commandFlag[DEBUG_NMEA])" in DevUBLOXGNSS::processNMEA() event handler.
    // @see "if (commandFlag[DEBUG_NMEA_HEX])" in DevUBLOXGNSS::processNMEA() event handler.
    // @see "if (commandFlag[DEBUG_NMEA_SENT])" in DevUBLOXGNSS::processNMEA() event handler.

    // --- Buttons. ---
    // if (debugBtn)  {
    //     // - GNSS lock button state (0,1). -
    //     Serial.print("GNSS lock button position = ");
    //     (UIstate[0] == '0') ? Serial.println("up.") : Serial.println("down.");
    // }

    // --- Serial. ---
    if (commandFlag[DEBUG_SER]) {
        // - Serial state (d,u). -
        snprintf(outputBuffer, sizeof(outputBuffer), "Serial USB %c  Serial0 (not used) %c serial1 (RTCMin -> HC12) %c  serial2 (RTCMout -> ZED UART2) %c",
            serialState[0], serialState[1], serialState[2], serialState[3]);
        logPrint(outputBuffer);     // (textToLogPrint, add to log file = true, add newline = true).
    }

    // --- WiFi. ---
    // --- WS. ---

    // --- LiPo. ---
    if (commandFlag[DEBUG_LIPO]) {
        lipo.enableDebugging();
    } else {
        lipo.disableDebugging();
    }

    // --- Reset. ---
    if (commandFlag[RESTART]) {
        Serial.println("Restarting ...");
        esp_restart();
    }

    // --- Temporary items. ---
    // memset(debugTemp, '\0', sizeof(debugTemp));
    // strcpy(debugTemp,numberbuffer);
    if (commandFlag[DEBUG_TEMP]) {
            snprintf(outputBuffer, sizeof(outputBuffer), 
                "ellipsoid=%d, ellipsoidHp=%d, heightEllipsoid=%.3f", ellipsoid, ellipsoidHp, heightEllipsoid);
            logPrint(outputBuffer);
            snprintf(outputBuffer, sizeof(outputBuffer), "msl=%d, mslHp=%d, prfInstrHgt=%.2f, heightOrthometric=%.3f, altitudeMSL=%.3f", msl, mslHp, prfInstrHgt, heightOrthometric, altitudeMSL);
            logPrint(outputBuffer);
            logPrint(lastGGA);
    }

    // --- Preferences. ---
    if (commandFlag[DEBUG_PREFS]) {
        prefUtility(PREF_PRINT);
        logPrint(" ");
        commandFlag[DEBUG_PREFS] = false;
        // Serial.printf("%s %s\n", COMMAND[i], (commandFlag[i]  ? "enabled." : "disabled."));
    }

    // --- NTRIP. ---
    // @see "if (commandFlag[DEBUG_NTRIP])" in ntripBeginClient(), called from taskRtcmRelay() FreeRTOS task.
}

/**
 * =========================================================================
 *  Setup.
 * =========================================================================
 *
 * @since 3.0.3 [2025-10-13-01:00pm] New.
 * @since 3.2.3  [2026-08-10-09:45am] Add startTcpServer().
 * @since 3.3.0  [2026-08-17-09:00am] Changed position of startOutputs() in setup().
 * @since 3.3.3  [2026-09-07-11:30am] Added xTaskGetCurrentTaskHandle().
 * @see   Global vars.
 * @see   uxTaskGetStackHighWaterMark(loopTaskHandle).
 */
void setup() {
    startOutputs();                                // Start serial & microSD card reader.
    buildInfo();                                   // Build & processor info.
    prefUtility(PREF_INIT);                        // Get preferences, (PREF_RESET_NO_REBOOT) to reinitialize.
    startSerial();                                 // Start serial interfaces.
    initPins();                                    // Initialize pin modes & pin values.
    startI2C();                                    // Start I2C wire interfaces.
    startLiPo();                                   // Start LiPo I2C interface.
    startWiFiServer();                             // Start WiFi server.
    startTcpServer();                              // Start TCP server for GNSS Master (NMEA out / RTCM in bridge).
    startHttpServer();                             // Start HTTP server.
    startWebSocketServer();                        // Start WebSocket server.
    startAndConfigGNSS();                          // Start GNSS, config ZED settings.
    startQueues();                                 // Start GhostRover FreeRTOS queues.
    startTasks();                                  // Start GhostRover FreeRTOS tasks.
    preLoop();                                     // Prepare for loop().
}

/**
 * =========================================================================
 *  Loop.
 * =========================================================================
 * 
 * @since 3.0.10 [2025-12-27-08:00pm] New.
 * @since 3.3.0  [2026-08-13-12:00pm] Replaced checkZedTriggerUpdate() with checkLoopTimers().
 * @since 3.3.0  [2026-08-13-01:00pm] Replaced debug timer with checkLoopTimers().
 * @since 3.3.5  [2026-09-07-01:15pm] Replaced checkLoopTimers() with FreeRTOS tasks.
 * @since 3.4.0  [2026-09-13-04:30pm] Removed checkGnssLockButton(), not needed.
 * @see   startTasks().
 * @see   GhostRover FreeRTOS functions.
 * @see   Event handlers.
 * @link  https://github.com/sparkfun/SparkFun_u-blox_GNSS_Arduino_Library/tree/main/examples.
 */
void loop() {
    checkSerialUSB();               // Check serial USB for input.
    checkFlags();                   // Check flags set in other functions or FreeRTOS tasks.
    processMessagesIn();            // Process queued WS messages. All JSON activity lives here & sendDataToBrowser().
    checkTCPServer();               // Check TCP server for new/dropped client. 
    ws.cleanupClients();            // HTTP WebSocket cleanup.
    vTaskDelay(1);                  // Play nice with FreeRTOS.
}
