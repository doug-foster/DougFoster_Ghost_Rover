#line 1 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
/**
 * **********************************************************************
 * Ghost Rover 3 - GNSS "invisible" rover (with RTK base/PP) for SW Maps.
 * **********************************************************************
 * 
 * ------------------------------------------------
 *      Code structure.
 * ------------------------------------------------
 *  --- Docs. ---
 *          Comments.
 *          ESP32 (Arduino framework) data types.
 *          WebSocket exchange protocol and data.
 *  --- Include libraries. ---
 *          Core.
 *          Additional.
 *  --- Global vars.---
 *          Pin assignments.
 *          LED.
 *          Battery.
 *          WiFi.
 *          HTTP.
 *          WebSocket.
 *          GNSS.
 *          Task handles.
 *          Operation.
 *          Preferences.
 *          Oper status.
 *          Declaration.
 *          Test.
 *  --- General functions. ---
 *          wsKey()             - Replace [wsKey(WS_VERSION)] with ["0"], ["1"], etc.
 *          statusLedOn()       - Turn on status LED.
 *          prefUtility()       - Preference utility.
 *          operDataToJsonDoc() - Load "operate" page data into JSON doc.
 *  --- Setup functions. ---
 *          showBuild()            - Display build & processor info.
 *          prefUtility(PREF_INIT) - Preference utility.
 *          startSerial()          - Start serial interfaces.
 *          initPins()             - Initialize pins & pin values.
 *          startI2C()             - Start I2C wire interfaces.
 *          startLiPo()            - Start LiPo I2C interface.
 *          startWiFi()            - Start WiFi.
 *          startSD()              - Start & test microSD card reader.
 *          startHttpServer()      - Start HTTP server.
 *          startWebSocketServer() - Start WebSocket server.
 *          startAndConfigGNSS()   - Start GNSS, config ZED settings.
 *          startTasks()           - Start tasks.
 *          preLoop()              - Prepare for loop().
 *  --- Task functions. ---
 *          startTasks()        - Start tasks in setup().
 *          taskLoopStatusLed() - Status LED for loop().
 *  --- Event handlers. ---
 *          onWiFiEvent()               - WiFi event handler.
 *          onHttpFileUpload()          - HTTP server endpoint handler.
 *          onWebSocketEvent()          - WebSocket event handler.
 *          onWebSocketMessage()        - WebSocket message event handler.
 *          DevUBLOXGNSS::processNMEA() - SparkFun_u-blox_GNSS_v3 library: process NMEA bytes.
 *  --- Loop functions. ---
 *          checkZED()                - NMEA - Check ZED to trigger DevUBLOXGNSS::processNMEA().
 *          relaySerial1toSerial2()   - RTCM - Relay from Serial1 (HC-12 radio) to Serial2 (ZED UART2).
 *          rtcm3GetMessageType()     - RTCM - Return RTCM3 message type.
 *          checkSerialUSB()          - Check serial USB for input.
 *          debug()                   - Display debug.
 *          checkGnssLockButton()     - Check GNSS lock button. // ToDo: implement.
 *          ws.cleanupClients()       - HTTP WebSocket cleanup.
 *  --- Setup. ---
 *          See "Setup functions" above.
 *  --- Loop. ---
 *          See "Loop functions" above.
 *
 * @author D. Foster <doug@dougfoster.me>.
 * @since  3.0.12 [2026-02-23-12:15pm] Shorten RTCM & NMEA status.
 * @since  3.0.12 [2026-02-28-02:15pm] Add WS_SOCKET_NUM.
 * @see    https://github.com/doug-foster/DougFoster_Ghost_Rover.
 * @see    https://github.com/doug-foster/DougFoster_Ghost_Rover_BT_relay.
 * @see    https://github.com/doug-foster/DougFoster_Ghost_Rover_EVK_RTCM_relay.
 * @link   http://dougfoster.me.
 */

/**
 * ============================================================================
 *  Docs.
 * ============================================================================
 *
 * @since 3.0.12 [2026-02-20-12:00pm] New.
 */

/**
 * ------------------------------------------------
 *      Comments.
 * ------------------------------------------------
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
 *     -- EVK         https://docs.sparkfun.com/SparkFun_RTK_EVK/introduction/.ard
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
 *     1. Add button lock functionality.
 *     2. Add stand-alone mode using NTRIP.
 *     3. Add "vector to coordinates" (navigate to a position) function.
 * @link https://www.build-electronic-circuits.com/arduino-laser-module-ky-008/.
 */

/**
 * ------------------------------------------------
 *      ESP32 (Arduino framework) data types.
 * ------------------------------------------------
 *
 * @since 3.0.12 [2026-02-20-09:00am] New.
 * 
 * --- Unsigned integer. ---
 * uint8_t                      %u        8 bits = 1 byte,  0 to 255.
 * uint16_t/unsigned short      %u       16 bits = 2 bytes, 0 to 65,535.
 * uint32_t/unsigned long       %u,%lu   32 bits = 4 bytes, 0 to 4,294,967,295.
 * size_t (size,length,count)   %zu      32 bits = 4 bytes, 0 to 4,294,967,295.
 * uint64_t/unsigned long long  %llu     64 bits = 8 bytes, 0 to 18,446,744,073,709,551,615.
 *
 * --- Signed integer. ---
 * int8_t                       %d        8 bits = 1 byte,            -128 to 127.
 * int16_t/short                %d       16 bits = 2 bytes,        -32,768 to 32,767.
 * int32_t/int/long             %d,%ld   32 bits = 4 bytes, -2,147,483,648 to 2,147,483,647.
 * int64_t/long long            %lld     64 bits = 8 bytes,      -9.22e+18 to 9.22e+18.
 *
 * --- Signed decimal/floating point. ---
 * float                        %f       32 bits = 4 bytes,   6-7 sig. digits (hardware),  -3.40e+38 to 3.40e+38).
 * double/long double           %f,%lf   64 bits = 8 bytes, 15-17 sig. digits (software), -1.79e+308 to 1.79e+308).
 *
 * --- Character/text. ---
 * char (signed)                %c         8 bit = 1 byte,  -128 to 127.
 * unsigned char                %c         8 bit = 1 byte,     0 to 255.
 *
 * --- Other. ---
 *  bool                        %d (0/1)   8 bit = 1 byte,  true or false.
 *  bool                        %s (text)  8 bit = 1 byte,  true or false.
 *  void                        n/a.
 *  array                       n/a.
 *  string                      %s
 */

/**
 * ------------------------------------------------
 *      WebSocket exchange protocol and data.
 * ------------------------------------------------
 *
 * @since 3.0.12 [2026-02-20-09:00am] New.
 *
 *  --- Exchange protocol for all pages. ---
 *  -- Hello. --
 *  browser --> [{"page":"menu/nmea/files/config/operate"}].
 *  browser <-- {"0":"3.0.12 - Feb 28 2026 @ 09:40:15","1":"meter","2":"radio","3":"on","4":100,"5":2,"6":"ssid","7":"pass","35":10}
 *  -- Echo. --
 *  browser --> [{"echo":"hello/etc"}].
 *  browser <-- {"echo":"hello/etc"}.
 *
 *  --- Exchange protocol for menu.html page. ---
 *  -- Hello. --
 *  browser --> [{"page":"menu"}].
 *  browser <-- {"0":"3.0.12 - Feb 28 2026 @ 09:40:15","1":"meter","2":"radio","3":"on","4":100,"5":2,"6":"ssid","7":"pass","35":10}
 *  -- Restart. --
 *  browser --> [{"mcu":"restart"}].
 *  browser <-- No reply.
 *
 *  --- Exchange protocol for nema.html page. ---
 *  -- Hello. --
 *  browser --> [{"page":"nmea"}].
 *  browser <-- {"0":"3.0.12 - Feb 28 2026 @ 09:40:15","1":"meter","2":"radio","3":"on","4":100,"5":2,"6":"ssid","7":"pass","35":10}
 *  -- NMEA sentences. --
 *  browser <-- {"nmea":"$GLGSV,1,1,01,77,06,333,10,3*4F\r\n"}.
 *
 * --- Exchange protocol for files.html page. ---
 *  -- Hello. --
 *  browser --> [{"page":"files"}].
 *  browser <-- {"page":"files", "0":"3.0.12 - Feb 19 2026 @ 12:46:28","1":"meter","2":"radio","3":"on","4":"50","5":"2","6":"xxxx","7":"xxxx"}
 *  -- List files. --
 *  browser --> [{"listFiles":""}].
 *  browser <-- {"listFiles":"/index.html,/config.css,/config.html,/config.js,/upload-image-icon.png,/files.css,/files.html,
 *                /files.js,/global.css,/global.js,/menu.css,/menu.html,/menu.js,/operate.css,/operate.js,/junk.txt,/operate.html,"}.
 *  -- Delete files. --
 *  browser --> [{"deleteFile":"filename"}].
 *  browser <-- {"deleteFile":"fileDeleted/fileNOTdeleted"}.
 *
 * --- Exchange protocol for config.html page. ---
 *  -- Hello. --
 *  browser --> [{"page":"config"}].
 *  browser <-- {"0":"3.0.12 - Feb 28 2026 @ 09:40:15","1":"meter","2":"radio","3":"on","4":100,"5":2,"6":"ssid","7":"pass","35":10}
 *  -- Set preferences. --
 *  browser --> [{"config":"set","1":"meter","2":"radio","3":"on","4":"50","5":"2","6":"xxxx","7":"xxxx"}]
 *      prefUtility(prefAction) which uses:
 *          0 - jsonDocToClient["ver"]           --> jsonDocToClient[wsKey(WS_VERSION)].
 *          1 - jsonDocToClient["prfUnt"]        --> jsonDocToClient[wsKey(WS_PREF_UNIT)].
 *          2 - jsonDocToClient["prfRtcIn"]      --> jsonDocToClient[wsKey(WS_PREF_RTCM_IN)].
 *          3 - jsonDocToClient["prfNmeOut"]     --> jsonDocToClient[wsKey(WS_PREF_NMEA_OUT)].
 *          4 - jsonDocToClient["prfGnsMsrInt"]  --> jsonDocToClient[wsKey(WS_PREF_GNSS_MESASURE_INTERVAL)].
 *          5 - jsonDocToClient["prfGnsNavRat"]  --> jsonDocToClient[wsKey(wWS_PREF_GNSS_NAV_RATE)].
 *          6 - jsonDocToClient["prfHotSsi"]     --> jsonDocToClient[wsKey(WS_PREF_HOT_SPOT_SSID)].
 *          7 - jsonDocToClient["prfHotPas"]     --> jsonDocToClient[wsKey(WS_PREF_HOT_SPOT_PASS)].
 *  <-- {"config":"Preference values updated."}
 *  -- Reset preferences. --
 *  browser --> [{"config":"reset"}].
 *  browser <-- browser {"config":"message"}     message="Preferences reset to defaults."
 *
 * --- Exchange protocol for operate.html page. ---
 *  -- Hello. --
 *  browser --> [{"page":"operate"}].
 *  browser <-- {"0":"3.0.12 - Feb 28 2026 @ 09:40:15","1":"meter","2":"radio","3":"on","4":100,"5":2,"6":"ssid","7":"pass","35":10}
 *  -- GNSS & status values. --
 *  browser <-- {"8":1,"9":10,"10":"-40.68","11":"-4.62","12":"35.44418163","13":"-76.92332881","14":"8.464","15":"10.229","16":"d","17":"u","18":"101.30",
 *               "19":"2.5","20":"4h 29m 52s","30":526389,"31":160768,"23":77545,"24":77545,"25":129240,"26":216211,"27":25848,"28":0,"29":0,"32":"r",
 *               "33":"192.168.23.1","34":"172.20.10.2"}
 *      checkZed() calls operDataToJsonDoc() which uses:
 *           1 - jsonDocToClient["prfUnt"]        --> jsonDocToClient[wsKey(WS_PREF_UNIT)].
 *           2 - jsonDocToClient["prfRtcIn"]      --> jsonDocToClient[wsKey(WS_PREF_RTCM_IN)].
 *           3 - jsonDocToClient["prfNmeOut"]     --> jsonDocToClient[wsKey(WS_PREF_NMEA_OUT)].
 *           4 - jsonDocToClient["prfGnsMsrInt"]  --> jsonDocToClient[wsKey(WS_PREF_GNSS_MESASURE_INTERVAL)].
 *           5 - jsonDocToClient["prfGnsNavRat"]  --> jsonDocToClient[wsKey(WS_PREF_GNSS_NAV_RATE)].
 *           6 - jsonDocToClient["prfHotSsi"]     --> jsonDocToClient[wsKey(WS_PREF_HOT_SPOT_SSID)].
 *           7 - jsonDocToClient["prfHotPas"]     --> jsonDocToClient[wsKey(WS_PREF_HOT_SPOT_PASS)].
 *           8 - jsonDocToClient["fix"]           --> jsonDocToClient[wsKey(WS_GNSS_FIX)].
 *           9 - jsonDocToClient["siv"]           --> jsonDocToClient[wsKey(WS_GNSS_SAT_IN_VIEW)].
 *          10 - jsonDocToClient["hgt-elip"]      --> jsonDocToClient[wsKey(WS_GNSS_HEIGHT_ELLIPSOID)].
 *          11 - jsonDocToClient["hgt-orth"]      --> jsonDocToClient[wsKey(WS_GNSS_HEIGHT_ORTHOMETRIC)].
 *          12 - jsonDocToClient["lat"]           --> jsonDocToClient[wsKey(WS_GNSS_LATITUDE].
 *          13 - jsonDocToClient["lon"]           --> jsonDocToClient[wsKey(WS_GNSS_LONGITUDE)].
 *          14 - jsonDocToClient["hac"]           --> jsonDocToClient[wsKey(WS_GNSS_HORIZONTAL_ACCURACY)].
 *          15 - jsonDocToClient["vac"]           --> jsonDocToClient[wsKey(WS_GNSS_VERTICAL_ACCURACY)].
 *          16 - jsonDocToClient["rtcm"]          --> jsonDocToClient[wsKey(WS_ROVER_RTCM_UP_DOWN)].
 *          17 - jsonDocToClient["bt"]            --> jsonDocToClient[wsKey(WS_ROVER_BT_NMEA_UP_DOWN)].
 *          18 - jsonDocToClient["bat"]           --> jsonDocToClient[wsKey(WS_ROVER_BATTERY_SOC)].
 *          19 - jsonDocToClient["batc"]          --> jsonDocToClient[wsKey(WS_ROVER_BATTERY_CHANGE_RATE)].
 *          20 - jsonDocToClient["up-tm"]         --> jsonDocToClient[wsKey(WS_ROVER_UP_TIME)].
 *          21 - jsonDocToClient["rtcm-cnt-all"]  --> jsonDocToClient[wsKey(WS_RTCM_IN_COUNT_ALL)].
 *          22 - jsonDocToClient["rtcm-rate"]     --> jsonDocToClient[wsKey(WS_RTCM_IN_RATE)].
 *          23 - jsonDocToClient["nmea-cnt-gga"]  --> jsonDocToClient[wsKey(WS_NMEA_OUT_COUNT_GGA)].
 *          24 - jsonDocToClient["nmea-cnt-rmc"]  --> jsonDocToClient[wsKey(WS_NMEA_OUT_COUNT_RMC)].
 *          25 - jsonDocToClient["nmea-cnt-gsa"]  --> jsonDocToClient[wsKey(WS_NMEA_OUT_COUNT_GSA].
 *          26 - jsonDocToClient["nmea-cnt-gsv"]  --> jsonDocToClient[wsKey(WS_NMEA_OUT_COUNT_GSV)].
 *          27 - jsonDocToClient["nmea-cnt-gst"]  --> jsonDocToClient[wsKey(WS_NMEA_OUT_COUNT_GST)].
 *          28 - jsonDocToClient["nmea-cnt-txt"]  --> jsonDocToClient[wsKey(WS_NMEA_OUT_COUNT_TXT)].
 *          29 - jsonDocToClient["nmea-cnt-othr"] --> jsonDocToClient[wsKey(WS_NMEA_OUT_COUNT_OTHR)].
 *          30 - jsonDocToClient["nmea-cnt-all"]  --> jsonDocToClient[wsKey(WS_NMEA_OUT_COUNT_ALL)].
 *          31 - jsonDocToClient["nmea-rate"]     --> jsonDocToClient[wsKey(WS_NMEA_OUT_RATE)].
 *          32 - jsonDocToClient["mode"]          --> jsonDocToClient[wsKey(WS_OPERATIONAL_MODE)].
 *          33 - jsonDocToClient["l-ip"]          --> jsonDocToClient[wsKey(WWS_WIFI_LOCAL_NETWORK_IP)].
 *          34 - jsonDocToClient["h-ip"]          --> jsonDocToClient[wsKey(WS_WIFI_HOT_SPOT_IP)].
 *          35 - jsonDocToClient["socketNum"]     --> jsonDocToClient[wsKey(WS_SOCKET_NUM)].
 *  -- Lock/unlock buttons. --
 * browser --> [{"laser"/height/position:"lock/unlock"}].
 * browser <-- {"laser"/height/position:"locked/unlocked"}. jsonDocToClient[key] = echo value + 'ed'
 */

/**
 * ============================================================================
 *  Include libraries.
 * ============================================================================
 *
 * @since 3.0.9   [2025-12-01-05:15pm].
 * @since 3.0.11  [2026-01-08-10:30am] Browser initiated updates.
 * @since 3.0.11  [2026-01-26-04:15pm] Add preferences library.
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
#include <AsyncTCP.h>                                      // https://github.com/ESP32Async/AsyncTCP (3.4.9).
#include <ESPAsyncWebServer.h>                             // https://github.com/ESP32Async/ESPAsyncWebServer (3.8.1).
#include <ArduinoJson.h>                                   // https://github.com/bblanchon/ArduinoJson (7.4.2).
#include <SparkFun_MAX1704x_Fuel_Gauge_Arduino_Library.h>  // https://github.com/sparkfun/SparkFun_MAX1704x_Fuel_Gauge_Arduino_Library (1.0.4).
#include <SparkFun_u-blox_GNSS_v3.h>                       // https://github.com/sparkfun/SparkFun_u-blox_GNSS_v3 (3.1.13).

/**
 * ============================================================================
 *  Global vars.
 * ============================================================================
 *
 * @since 3.0.10 [2026-01-06-10:00pm].
 * @since 3.0.11 [2026-01-08-10:30am] Browser initiated updates.
 * @since 3.0.12 [2026-02-01-09:30am] Add preferences.
 * @since 3.0.12 [2026-02-14-06:15pm] Remove prfRqsPvtInt.
 * @since  3.0.12 [2026-02-28-02:15pm] Add WS_SOCKET_NUM.
 */

// -- Pin assignments. --
const uint8_t HC12_SET    = 7;          // HC-12 SET {blue wire}.
const uint8_t LSR_TRIGGER = 15;         // KY-008 trigger pin {yellow wire}.

// --- LED. ---
bool  ws2812LedBlink     = false;
const uint8_t LED_BRIGHT = 50;          // 0-255. taskLoopStatusLed()
enum  ws2812_LED_COLOR {                // WS2812 RGB STAT LED.
    OFF,                                // 0.
    RED,                                // 1. Solid red indicates an error.
    YELLOW,                             // 2. Not used.
    GREEN,                              // 3. Solid color displayed when in loop and web client connected. Blinks if RTCM is received, solid red if RTCMin fails.
    BLUE,                               // 4. Solid color displayed when loop() is entered.
    WHITE                               // 5. Solid color displayed during setup() if no errors.
} ws2812LedColor;

// --- Battery. ---
SFE_MAX1704X lipo(MAX1704X_MAX17048);   // LiPo battery.

// --- WiFi. ---
char localIp[16];
char hotspotIp[16];

// --- HTTP. ---
const char     WEBSOCKET_SERVER_NAME[] = "/ghostRover";
uint8_t        clientId                = 0; // HTTP WebSocket client ID # (+1 for each new connection).
AsyncWebServer httpServer(80);              // HTTP AsyncWebServer object on port 80.
AsyncWebSocket ws(WEBSOCKET_SERVER_NAME);   // HTTP WebSocket object.

// --- WebSocket. ---
char         JSONbuffer[768];               // @see onWebSocketMessage() & DevUBLOXGNSS::processNMEA().
size_t       jsonPairNum;                   // Track number of JSON KV pairs.
JsonDocument jsonDocToClient;               // JSON document sent to client.
JsonDocument jsonDocFromClient;             // JSON document received from client.
enum wsKeyID {                              // Readable index for WwebSocket keys. @see Global vars. - WebSockets in global.js.
                                            //             Was,                                  is now.
    WS_VERSION,                             //  0 - jsonDocToClient["ver"]           --> jsonDocToClient[wsKey(WS_VERSION)].
    WS_PREF_UNIT,                           //  1 - jsonDocToClient["prfUnt"]        --> jsonDocToClient[wsKey(WS_PREF_UNIT)].
    WS_PREF_RTCM_IN,                        //  2 - jsonDocToClient["prfRtcIn"]      --> jsonDocToClient[wsKey(WS_PREF_RTCM_IN)].
    WS_PREF_NMEA_OUT,                       //  3 - jsonDocToClient["prfNmeOut"]     --> jsonDocToClient[wsKey(WS_PREF_NMEA_OUT)].
    WS_PREF_GNSS_MESASURE_INTERVAL,         //  4 - jsonDocToClient["prfGnsMsrInt"]  --> jsonDocToClient[wsKey(WS_PREF_GNSS_MESASURE_INTERVAL)].
    WS_PREF_GNSS_NAV_RATE,                  //  5 - jsonDocToClient["prfGnsNavRat"]  --> jsonDocToClient[wsKey(WS_PREF_GNSS_NAV_RATE)].
    WS_PREF_HOT_SPOT_SSID,                  //  6 - jsonDocToClient["prfHotSsi"]     --> jsonDocToClient[wsKey(WS_PREF_HOT_SPOT_SSID)].
    WS_PREF_HOT_SPOT_PASS,                  //  7 - jsonDocToClient["prfHotPas"]     --> jsonDocToClient[wsKey(WS_PREF_HOT_SPOT_PASS)].
    WS_GNSS_FIX,                            //  8 - jsonDocToClient["fix"]           --> jsonDocToClient[wsKey(WS_GNSS_FIX)].
    WS_GNSS_SAT_IN_VIEW,                    //  9 - jsonDocToClient["siv"]           --> jsonDocToClient[wsKey(WS_GNSS_SAT_IN_VIEW)].
    WS_GNSS_HEIGHT_ELLIPSOID,               // 10 - jsonDocToClient["hgt-elip"]      --> jsonDocToClient[wsKey(WS_GNSS_HEIGHT_ELLIPSOID)].
    WS_GNSS_HEIGHT_ORTHOMETRIC,             // 11 - jsonDocToClient["hgt-orth"]      --> jsonDocToClient[wsKey(WS_GNSS_HEIGHT_ORTHOMETRIC)].
    WS_GNSS_LATITUDE,                       // 12 - jsonDocToClient["lat"]           --> jsonDocToClient[wsKey(WS_GNSS_LATITUDE)].
    WS_GNSS_LONGITUDE,                      // 13 - jsonDocToClient["lon"]           --> jsonDocToClient[wsKey(WS_GNSS_LONGITUDE)].
    WS_GNSS_HORIZONTAL_ACCURACY,            // 14 - jsonDocToClient["hac"]           --> jsonDocToClient[wsKey(WS_GNSS_HORIZONTAL_ACCURACY)].
    WS_GNSS_VERTICAL_ACCURACY,              // 15 - jsonDocToClient["vac"]           --> jsonDocToClient[wsKey(WS_GNSS_VERTICAL_ACCURACY)].
    WS_ROVER_RTCM_UP_DOWN,                  // 16 - jsonDocToClient["rtcm"]          --> jsonDocToClient[wsKey(WS_ROVER_RTCM_UP_DOWN)].
    WS_ROVER_BT_NMEA_UP_DOWN,               // 17 - jsonDocToClient["bt"]            --> jsonDocToClient[wsKey(WS_ROVER_BT_NMEA_UP_DOWN)].
    WS_ROVER_BATTERY_SOC,                   // 18 - jsonDocToClient["bat"]           --> jsonDocToClient[wsKey(WS_ROVER_BATTERY_SOC)].
    WS_ROVER_BATTERY_CHANGE_RATE,           // 19 - jsonDocToClient["batc"]          --> jsonDocToClient[wsKey(WS_ROVER_BATTERY_CHANGE_RATE)].
    WS_ROVER_UP_TIME,                       // 20 - jsonDocToClient["up-tm"]         --> jsonDocToClient[wsKey(WS_ROVER_UP_TIME)].
    WS_RTCM_IN_COUNT_ALL,                   // 21 - jsonDocToClient["rtcm-cnt-all"]  --> jsonDocToClient[wsKey(WS_RTCM_IN_COUNT_ALL)].
    WS_RTCM_IN_RATE,                        // 22 - jsonDocToClient["rtcm-rate"]     --> jsonDocToClient[wsKey(WS_RTCM_IN_RATE)].
    WS_NMEA_OUT_COUNT_GGA,                  // 23 - jsonDocToClient["nmea-cnt-gga"]  --> jsonDocToClient[wsKey(WS_NMEA_OUT_COUNT_GGA)].
    WS_NMEA_OUT_COUNT_RMC,                  // 24 - jsonDocToClient["nmea-cnt-rmc"]  --> jsonDocToClient[wsKey(WS_NMEA_OUT_COUNT_RMC)].
    WS_NMEA_OUT_COUNT_GSA,                  // 25 - jsonDocToClient["nmea-cnt-gsa"]  --> jsonDocToClient[wsKey(WS_NMEA_OUT_COUNT_GSA)].
    WS_NMEA_OUT_COUNT_GSV,                  // 26 - jsonDocToClient["nmea-cnt-gsv"]  --> jsonDocToClient[wsKey(WS_NMEA_OUT_COUNT_GSV)].
    WS_NMEA_OUT_COUNT_GST,                  // 27 - jsonDocToClient["nmea-cnt-gst"]  --> jsonDocToClient[wsKey(WS_NMEA_OUT_COUNT_GST)].
    WS_NMEA_OUT_COUNT_TXT,                  // 28 - jsonDocToClient["nmea-cnt-txt"]  --> jsonDocToClient[wsKey(WS_NMEA_OUT_COUNT_TXT)].
    WS_NMEA_OUT_COUNT_OTHR,                 // 29 - jsonDocToClient["nmea-cnt-othr"] --> jsonDocToClient[wsKey(WS_NMEA_OUT_COUNT_OTHR)].
    WS_NMEA_OUT_COUNT_ALL,                  // 30 - jsonDocToClient["nmea-cnt-all"]  --> jsonDocToClient[wsKey(WS_NMEA_OUT_COUNT_ALL)].
    WS_NMEA_OUT_RATE,                       // 31 - jsonDocToClient["nmea-rate"]     --> jsonDocToClient[wsKey(WS_NMEA_OUT_RATE)].
    WS_OPERATIONAL_MODE,                    // 32 - jsonDocToClient["mode"]          --> jsonDocToClient[wsKey(WS_OPERATIONAL_MODE)].
    WS_WIFI_LOCAL_NETWORK_IP,               // 33 - jsonDocToClient["l-ip"]          --> jsonDocToClient[wsKey(WS_WIFI_LOCAL_NETWORK_IP)].
    WS_WIFI_HOT_SPOT_IP,                    // 34 - jsonDocToClient["h-ip"]          --> jsonDocToClient[wsKey(WS_WIFI_HOT_SPOT_IP)].
    WS_SOCKET_NUM                           // 35 - jsonDocToClient["socketNum"]     --> jsonDocToClient[wsKey(WS_SOCKET_NUM)].
};
const char* const WS_KEY_NUMS[] = {
    "0",                                    // WS_VERSION, jsonDocToClient[wsKey(WS_VERSION)] converts to jsonDocToClient["0"].
    "1",                                    // WS_PREF_UNIT.
    "2",                                    // WS_PREF_RTCM_IN.
    "3",                                    // WS_PREF_NMEA_OUT.
    "4",                                    // WS_PREF_GNSS_MESASURE_INTERVAL.
    "5",                                    // WS_PREF_GNSS_NAV_RATE.
    "6",                                    // WS_PREF_HOT_SPOT_SSID.
    "7",                                    // WS_PREF_HOT_SPOT_PASS.
    "8",                                    // WS_GNSS_FIX.
    "9",                                    // WS_GNSS_SAT_IN_VIEW.
    "10",                                   // WS_GNSS_HEIGHT_ELLIPSOID.
    "11",                                   // WS_GNSS_HEIGHT_ORTHOMETRIC.
    "12",                                   // WS_GNSS_LATITUDE.
    "13",                                   // WS_GNSS_LONGITUDE.
    "14",                                   // WS_GNSS_HORIZONTAL_ACCURACY.
    "15",                                   // WS_GNSS_VERTICAL_ACCURACY.
    "16",                                   // WS_ROVER_RTCM_UP_DOWN.
    "17",                                   // WS_ROVER_BT_NMEA_UP_DOWN.
    "18",                                   // WS_ROVER_BATTERY_SOC.
    "19",                                   // WS_ROVER_BATTERY_CHANGE_RATE.
    "20",                                   // WS_ROVER_UP_TIME.
    "21",                                   // WS_RTCM_IN_COUNT_ALL.
    "22",                                   // WS_RTCM_IN_RATE.
    "23",                                   // WS_NMEA_OUT_COUNT_GGA.
    "24",                                   // WS_NMEA_OUT_COUNT_RMC.
    "25",                                   // WS_NMEA_OUT_COUNT_GSA.
    "26",                                   // WS_NMEA_OUT_COUNT_GSV.
    "27",                                   // WS_NMEA_OUT_COUNT_GST.
    "28",                                   // WS_NMEA_OUT_COUNT_TXT.
    "29",                                   // WS_NMEA_OUT_COUNT_OTHR.
    "30",                                   // WS_NMEA_OUT_COUNT_ALL.
    "31",                                   // WS_NMEA_OUT_RATE.
    "32",                                   // WS_OPERATIONAL_MODE.
    "33",                                   // WS_WIFI_LOCAL_NETWORK_IP.
    "34",                                   // WS_WIFI_HOT_SPOT_IP.
    "35"                                    // WS_SOCKET_NUM.
};

// --- GNSS. ---
SFE_UBLOX_GNSS roverGNSS;                   // GNSS object (uses I2C-1).

// --- Task handles. ---
TaskHandle_t taskLoopStatusLedHandle;       // Task: Loop status LED.

// --- Operation. ---
enum CommandIndex {                         //  Readable index for command array.
    TEST_RAD = 0,                           //  0.
    DEBUG_RTCM,                             //  1.
    DEBUG_GNSS,                             //  2.
    DEBUG_NMEA,                             //  3.
    DEBUG_BTN,                              //  4.
    DEBUG_SER,                              //  5.
    DEBUG_WIFI,                             //  6.
    DEBUG_WS,                               //  7.
    DEBUG_LIPO,                             //  8.
    SHOW_UPTIME,                            //  9.
    RESET,                                  // 10.
    CHECK_WIRE1,                            // 11.
    DEBUG_TEMP,                             // 12.
    DEBUG_NMEA_HEX,                         // 13.
    DEBUG_NMEA_COUNTS,                      // 14.
    DEBUG_PREFS,                            // 15.
    NUM_COMMANDS                            // 16 = automatic array length.
};      
const char* COMMAND[NUM_COMMANDS] = {       // Command strings; match CommandIndex.
    "testRad",                              // TEST_RAD.
    "debugRTCM",                            // DEBUG_RTCM.
    "debugGNSS",                            // DEBUG_GNSS.
    "debugNMEA",                            // DEBUG_NMEA.
    "debugBtn",                             // DEBUG_BTN.
    "debugSer",                             // DEBUG_SER.
    "debugWiFi",                            // DEBUG_WIFI.
    "debugWs",                              // DEBUG_WS.
    "debugLiPo",                            // DEBUG_LIPO.
    "showUpTime",                           // SHOW_UPTIME.
    "reset",                                // RESET.
    "checkWire1",                           // CHECK_WIRE1.
    "debugTemp",                            // DEBUG_TEMP.
    "debugNMEAhex",                         // DEBUG_NMEA_HEX.
    "debugNMEAcounts",                      // DEBUG_NMEA_COUNTS.
    "debugPrefs"                            // DEBUG_PREFS.
};      
const bool RW_MODE = false;                     // Open preference name space as read/write.
const bool RO_MODE = true;                      // Open preference name space as read only.
bool    ghostMode          = false;             // Flag, in Ghost mode (i.e. locked coordinates).
bool    i2cUp              = false;             // Status: true if both Wire & Wire1 up, else false.
bool    inLoop             = false;             // In loop() indicator.
bool    RTCMin             = false;             // RTCM received from NTRIP or radio within RTCM_TIMEOUT.
bool    NMEAout            = false;             // NMEA sent OUT to MCU #2?
bool    zeroStatusCounters = false;             // Flag to zero status counters.
bool    buttonGnssLock;                         // UI - // ToDo: implement.
bool    buttonAltitudeLock;                     // UI - // ToDo: implement.
bool    buttonPositionLock;                     // UI - // ToDo: implement.
bool    buttonLaser;                            // UI button to turn laser pointer on/off.
bool    buttonUnlockAll;                        // UI - // ToDo: implement.
bool    commandFlag[NUM_COMMANDS] = {false};    // Command flags.
char    operMode[2]               = {'\0'};     // Operation mode (r=rover, b=base).
char    debugTemp[250]            = {'\0'};     // Various debug scenarios.
char    whichPage[10]             = {'\0'};     // Current browser page served by startHttpServer().
char    buildString[40]           = {'\0'};     // Build string (build version on date at time). e.g. 3.0.12 - Feb 19 2026 @ 12:23:13
char    serialState[4];                         // Serial state: [USB] [S0] [S1] [S2]; value = u, d, or -.
size_t  wsSendCount = 0;                        // # of WebSocket messages sent.
int64_t startTime;                              // Boot time.

// --- Preferences. ---
char        prfUnt[6];          // Distance units: meter/feet (used only in browser).
char        prfRtcIn[6];        // Control RTCM in: off/radio/ntrip.
char        prfNmeOut[4];       // Control NMEA out: off/on.
char        prfHotSsi[20];      // WiFi hotspot client: network SSID.
char        prfHotPas[30];      // WiFi hotspot client: password.
uint16_t    prfGnsMsrInt;       // ZED: MEASURE every Y (e.g. 100) ms.
uint8_t     prfGnsNavRat;       // ZED: OUTPUT every X (e.g. 5) MEASURE intervals every (e.g. 5*100=500) ms.
Preferences roverPrefs;         // Rover's NVS preferences namespace.
enum        prefAction {        // Readable index for preference actions.
    PREF_INIT,                  // 0.
    PREF_READ,                  // 1.
    PREF_SAVE,                  // 2.
    PREF_RESET,                 // 3.
    PREF_PRINT,                 // 4.
    PREF_TO_JSON                // 5.
};

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
int64_t nmeaSentenceLength = 0;

// --- Declaration. ---
// --- Test. ---

/**
 * ============================================================================
 *  General functions.
 * ============================================================================
 *
 * @since 3.0.12 [2026-02-06-04:00pm] New.
 * @see   wsKey()             - Replace [wsKey(WS_VERSION)] with ["0"], ["1"], etc.
 * @see   statusLedOn()       - Turn on status LED.
 * @see   prefUtility()       - Preference utility.
 * @see   operDataToJsonDoc() - Load "operate" page data into JSON doc.
 */

 /**
 * ------------------------------------------------
 * Replace [wsKey(WS_VERSION)] with ["0"], ["1"], etc.
 * ------------------------------------------------
 *
 * Example, given:  jsonDocToClient[wsKey(WS_VERSION)] = someValue;.
 *          Yields: jsonDocToClient["0"].
 *
 * @param  int id Key ID #.
 * @return char* const Key ID #.
 * @since  03.0.12 [2026-02-19] New.
 */
#line 597 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
const char * wsKey(int id);
#line 610 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void statusLedOn();
#line 838 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void operDataToJsonDoc();
#line 997 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void showBuild();
#line 1042 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void startSerial();
#line 1073 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void initPins();
#line 1099 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void startI2C();
#line 1136 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void startLiPo();
#line 1160 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void startWiFi();
#line 1260 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void startSD();
#line 1322 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void startHttpServer();
#line 1377 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void startWebSocketServer();
#line 1412 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void startAndConfigGNSS();
#line 1492 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void startTasks();
#line 1508 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void preLoop();
#line 1545 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void taskLoopStatusLed(void * pvParameters);
#line 1585 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void onWiFiEvent(arduino_event_id_t event);
#line 1617 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void onHttpFileUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final);
#line 1663 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void onWebSocketEvent(AsyncWebSocket *httpServer, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);
#line 1718 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void onWebSocketMessage(void *arg, uint8_t *data, size_t len);
#line 2062 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void checkZED();
#line 2127 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void relaySerial1toSerial2();
#line 2198 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
uint16_t rtcm3GetMessageType(const char* rtcmSentence);
#line 2221 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void checkSerialUSB();
#line 2293 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void debug();
#line 2467 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void checkGnssLockButton();
#line 2496 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void setup();
#line 2523 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
void loop();
#line 597 "/Users/dougfoster/Library/CloudStorage/Dropbox/Data/doug/Topics/_dev-arduino/DougFoster_Ghost_Rover/DougFoster_Ghost_Rover.ino"
const char* wsKey(int id) {
    return WS_KEY_NUMS[id];
}

 /**
 * ------------------------------------------------
 *      Turn on status LED.
 * ------------------------------------------------
 * 
 * @return void  No output is returned.
 * @since  3.0.12 [2026-02-10-10:45pm] New.
 * @see showBuild(), startWiFi(), startSD(), startAndConfigGNSS(), taskLoopStatusLed().
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
 * ------------------------------------------------
 *      Preference utility.
 * ------------------------------------------------
 * 
 * Keys for preference values are sent in WebSocket as "1", "2", etc. but stored in NVS as "prfUnt", "prfRtcIn", ... .
 * Refer to enum wsKeyID and char array WS_KEY_NUMS[] in WebSocket sertion of Global Vars to decode.
 * 
 * @param  object prefAction PREF_INIT, PREF_READ, PREF_SAVE, PREF_RESET, PREF_PRINT, PREF_TO_JSON.
 * @param  array key WebSocket JSON key.
 * @param  array value WebSocket JSON value.
 * @return void  No output is returned.
 * @since  3.0.12 [2026-02-07-10:30am] New.
 * @since  3.0.12 [2026-02-14-06:15pm] Remove prfRqsPvtInt.
 * @since  3.0.12 [2026-02-18-06:00pm] Add buildString.
 * @since  3.0.12 [2026-02-23-01:00pm] Shorten RTCM & NMEA status.
 * @since  3.0.12 [2026-02-28-02:45pm] Fix bugs: prfRtcIn, jsonDocToClient.clear().
 * @see    Global vars: Preference defaults, setup().
 * @link   https://docs.espressif.com/projects/arduino-esp32/en/latest/tutorials/preferences.html.
 * @link   https://github.com/espressif/arduino-esp32/tree/master/libraries/Preferences/.
 */
void prefUtility(prefAction action, const char* key = NULL, const char* value = NULL) {

    // --- Preference defaults. ---
    const char      NAMESPACE[]         = "config"; // The preference namespace. 
    const char      DEF_PRF_UNT[]       = "meter";  // Distance units: meter/feet (used only in browser).   Matching global var: char     prfUnt[6].
    const char      DEF_PRF_RTC_IN[]    = "radio";  // Control RTCM in: off/radio/ntrip.                    Matching global var: char     prfRtcIn[6].
    const char      DEF_PRF_NME_OUT[]   = "on";     // Control NMEA out: off/on.                            Matching global var: char     prfNmeOut[4].
    const char      DEF_PRF_HOT_SSI[]   = "ssid";   // WiFi hotspot client: network SSID.                   Matching global var: char     prfHotSsi[20].
    const char      DEF_PRF_HOT_PASS[]  = "pass";   // WiFi hotspot client: password.                       Matching global var: char     prfHotPas[30].
    const uint8_t   DEF_PRF_GNS_NAV_RAT = 2;        // ZED rate (times/interval): OUTPUT a new solution.    Matching global var: uint8_t  prfGnsNavRat.
    const uint16_t  DEF_PRF_GNS_MSR_INT = 100;      // ZED interval (ms): CREATE a new solution.            Matching global var: uint16_t prfGnsMsrInt.
    const uint16_t  NUM_PREFS           = 8;        // Number of preferences being used.

    // --- Other. ---
    bool hasKey = false;
    JsonObject JSONdata = jsonDocFromClient[1].as<JsonObject>();     // Second array element is JSON data

    // --- Which action? ---
    switch (action) {
        case PREF_INIT:                                                     // Only called by setup().

            // -- Check namespace. --
            roverPrefs.begin(NAMESPACE, RW_MODE);                           // Open NAMESPACE object for read/write. If it doesn't exist, create it.
            hasKey = roverPrefs.isKey("prfUnt");                            // Check for preferences (stored as "prfUnt", "prfRtcIn", etc.).
            roverPrefs.end();                                               // Close NAMESPACE object.

            // -- Read or reset? --
            if(hasKey) {
                prefUtility(PREF_READ);                                     // Recursive. Test preference exists, so they all should. Read values from NVS & set global vars.
            } else {
                prefUtility(PREF_RESET);                                    // Recursive. If the test preference doesn't exist, none of them do.
            }

            // -- Wrap up. --
            Serial.printf("NVS namespace %s using %u entries with %u available.\n", NAMESPACE, NUM_PREFS, roverPrefs.freeEntries());
            break;

        case PREF_READ:

            // -- Read values. --
            roverPrefs.begin(NAMESPACE, RO_MODE);                               // Open NAMESPACE object for read.
            roverPrefs.getString("prfUnt",       prfUnt,    sizeof(prfUnt));    // Preference stored as "prfUnt".
            roverPrefs.getString("prfRtcIn",     prfRtcIn,  sizeof(prfRtcIn));
            roverPrefs.getString("prfNmeOut",    prfNmeOut, sizeof(prfNmeOut));
            roverPrefs.getString("prfHotSsi",    prfHotSsi, sizeof(prfHotSsi));
            roverPrefs.getString("prfHotPas",    prfHotPas, sizeof(prfHotPas));
            prfGnsNavRat = roverPrefs.getUChar("prfGnsNavRat");
            prfGnsMsrInt = roverPrefs.getUShort("prfGnsMsrInt");
            roverPrefs.end();                                                   // Close NAMESPACE object.

            // -- Wrap up. --
            Serial.println("Preferences read."); 
            break;

        case PREF_SAVE:
            
            // -- Open name space. ---
            roverPrefs.begin("config", RW_MODE);                                        // Open namespace object for read/write. Namespace remains open.

            // -- Process KV pairs. --
            strcpy(prfUnt, JSONdata[wsKey(WS_PREF_UNIT)]);                              // Update global var.
            roverPrefs.putString("prfUnt", prfUnt);                                     // Store preference "prfUnt" (sent/rcvd as "1").

            strcpy(prfRtcIn, JSONdata[wsKey(WS_PREF_RTCM_IN)]);
            roverPrefs.putString("prfRtcIn", prfRtcIn);                                 // Store preference as "prfRtcIn" (sent/rcvd as "2").

            strcpy(prfNmeOut, JSONdata[wsKey(WS_PREF_NMEA_OUT)]);
            roverPrefs.putString("prfNmeOut", prfNmeOut);                               // Store preference as "prfNmeOut" (sent/rcvd as "3").

            prfGnsMsrInt = (uint16_t) atoi(JSONdata[wsKey(WS_PREF_GNSS_MESASURE_INTERVAL)]);
            roverPrefs.putUShort("prfGnsMsrInt", prfGnsMsrInt);                             // Store preference as "prfGnsMsrInt" (sent/rcvd as "4").

            prfGnsNavRat = (uint8_t) atoi(JSONdata[wsKey(WS_PREF_GNSS_NAV_RATE)]);      // KV values are stored in NVS as int, but set to C-string in onWebSocketMessage() for code clarity.
            roverPrefs.putUChar("prfGnsNavRat", prfGnsNavRat);                          // Store preference as "prfGnsNavRat" (sent/rcvd as "45").

            strcpy(prfHotSsi, JSONdata[wsKey(WS_PREF_HOT_SPOT_SSID)]);
            roverPrefs.putString("prfHotSsi", prfHotSsi);                               // Store preference as "prfHotSsi" (sent/rcvd as "6").

            strcpy(prfHotPas, JSONdata[wsKey(WS_PREF_HOT_SPOT_PASS)]);
            roverPrefs.putString("prfHotPas", prfHotPas);                               // Store preference as "prfHotPas" (sent/rcvd as "7").

            // if (strcmp(key, wsKey(WS_PREF_UNIT)) == 0) {                            // Preference sent as "1".
                
            //     strcpy(prfUnt, JSONdata["0"]);                                      // Update global var.
            //     roverPrefs.putString("prfUnt", prfUnt);                             // Store preference "prfUnt".
            // } else if (strcmp(key, wsKey(WS_PREF_RTCM_IN)) == 0) {                  // Preference sent as "2".
            //     strcpy(prfRtcIn, value);

            //     roverPrefs.putString("prfRtcIn", prfRtcIn);                         // Store preference as "prfRtcIn".
            // } else if (strcmp(key, wsKey(WS_PREF_NMEA_OUT)) == 0) {                 // Preference sent as "3".
            //     strcpy(prfNmeOut, value);
            //     roverPrefs.putString("prfNmeOut", prfNmeOut);                       // Store preference "prfNmeOut".
            // } else if (strcmp(key, wsKey(WS_PREF_GNSS_MESASURE_INTERVAL)) == 0) {   // Preference sent as "4".
            //     prfGnsNavRat = (uint8_t) atoi(value);                               // KV values are stored in NVS as int, but set to C-string in onWebSocketMessage() for code clarity.
            //     roverPrefs.putUChar("prfGnsNavRat", prfGnsNavRat);                  // Store preference "prfGnsNavRat".
            // } else if (strcmp(key, wsKey(WS_PREF_GNSS_NAV_RATE)) == 0) {            // Preference sent as "5".
                
            //     roverPrefs.putUShort("prfGnsMsrInt", prfGnsMsrInt);                 // Store preference as "prfGnsMsrInt".
            // }  else if (strcmp(key, wsKey(WS_PREF_HOT_SPOT_SSID)) == 0) {           // Preference sent as "6".
            //     strcpy(prfHotSsi, value);
            //     roverPrefs.putString("prfHotSsi", prfHotSsi);                       // Store preference as "prfHotSsi".
            // } else if (strcmp(key, wsKey(WS_PREF_HOT_SPOT_PASS)) == 0) {            // Preference sent as "7".
            //     strcpy(prfHotPas, value);
            //     roverPrefs.putString("prfHotPas", prfHotPas);                       // Store preference as "prfHotPas".
            // }
            
            // -- Wrap up. --
            roverPrefs.end();                                           // Close NAMESPACE object.
            jsonDocToClient["config"] = "Preference values updated.";
            Serial.println("Preferences saved.");
            if (inLoop) {                                               // Rerun dependent functions.
                startAndConfigGNSS();                                   // Uses prfGnsNavRat, prfGnsMsrInt.
                if (strcmp(prfRtcIn, "ntrip") == 0) {
                    startWiFi();                                        // Uses prfHotSsi & prfHotPas.
                } 
            }
            break;

        case PREF_RESET:

            // -- Set each KV pair to default values. --
            strcpy(prfUnt,    DEF_PRF_UNT);                                     // Set global vars to defaults.
            strcpy(prfRtcIn,  DEF_PRF_RTC_IN);
            strcpy(prfNmeOut, DEF_PRF_NME_OUT);
            strcpy(prfHotSsi, DEF_PRF_HOT_SSI);
            strcpy(prfHotPas, DEF_PRF_HOT_PASS);
            prfGnsNavRat = DEF_PRF_GNS_NAV_RAT;
            prfGnsMsrInt = DEF_PRF_GNS_MSR_INT;
            roverPrefs.begin(NAMESPACE, RW_MODE);                           // Open NAMESPACE object for read/write. 
            roverPrefs.putString("prfUnt",       prfUnt);                   // If key doesn't exist, create it. Set value to global var (aka default).
            roverPrefs.putString("prfRtcIn",     prfRtcIn);
            roverPrefs.putString("prfNmeOut",    prfNmeOut);
            roverPrefs.putString("prfHotSsi",    prfHotSsi);
            roverPrefs.putString("prfHotPas",    prfHotPas);
            roverPrefs.putUChar("prfGnsNavRat",  prfGnsNavRat);
            roverPrefs.putUShort("prfGnsMsrInt", prfGnsMsrInt);            roverPrefs.end();                                               // Close NAMESPACE object.

            // -- Wrap up. --
            Serial.println("Preferences reset.");
            if(inLoop) {                                                    // Rerun dependent functions if in loop.
                startAndConfigGNSS();                                       // Uses prfGnsNavRat, prfGnsMsrInt.
            }
            break;

        case PREF_PRINT:
            roverPrefs.begin(NAMESPACE, RO_MODE);                           // Open NAMESPACE object for read.
            Serial.println("--- Default, Global var, NVS pref. ---");
            Serial.printf( "prfUnt       \"%s\", \"%s\", \"%s\"\n", DEF_PRF_UNT,         prfUnt,       roverPrefs.getString("prfUnt"));
            Serial.printf( "prfRtcIn     \"%s\", \"%s\", \"%s\"\n", DEF_PRF_RTC_IN,      prfRtcIn,     roverPrefs.getString("prfRtcIn"));
            Serial.printf( "prfNmeOut    \"%s\", \"%s\", \"%s\"\n", DEF_PRF_NME_OUT,     prfNmeOut,    roverPrefs.getString("prfNmeOut"));
            Serial.printf( "prfHotSsi    \"%s\", \"%s\", \"%s\"\n", DEF_PRF_HOT_SSI,     prfHotSsi,    roverPrefs.getString("prfHotSsi"));
            Serial.printf( "prfHotPas    \"%s\", \"%s\", \"%s\"\n", DEF_PRF_HOT_PASS,    prfHotPas,    roverPrefs.getString("prfHotPas"));
            Serial.printf( "prfGnsNavRat %u, %u, %u\n",             DEF_PRF_GNS_NAV_RAT, prfGnsNavRat, roverPrefs.getUChar("prfGnsNavRat"));
            Serial.printf( "prfGnsMsrInt %u, %u, %u\n",             DEF_PRF_GNS_MSR_INT, prfGnsMsrInt, roverPrefs.getUShort("prfGnsMsrInt"));
            roverPrefs.end();                                               // Close NAMESPACE object.
            break;

        case PREF_TO_JSON:
        // --------------------------------------------
        //  browser --> {"page":"menu/nmea/files/config/operate"}.
        //  browser <-- {"0":"3.0.12 - Feb 19 2026 @ 12:46:28","1":"meter","2":"radio","3":"on","4":"50","5":"2","6":"xxx","7":"xxxx"}.
        //  Value of each global var preference will always match value stored in NVS.
        // --------------------------------------------
            jsonDocToClient.clear();
            jsonDocToClient[wsKey(WS_VERSION)]                     = buildString;   // 0.
            jsonDocToClient[wsKey(WS_PREF_UNIT)]                   = prfUnt;        // 1.
            jsonDocToClient[wsKey(WS_PREF_RTCM_IN)]                = prfRtcIn;      // 2.
            jsonDocToClient[wsKey(WS_PREF_NMEA_OUT)]               = prfNmeOut;     // 3.
            jsonDocToClient[wsKey(WS_PREF_GNSS_MESASURE_INTERVAL)] = prfGnsMsrInt;  // 4.
            jsonDocToClient[wsKey(WS_PREF_GNSS_NAV_RATE)]          = prfGnsNavRat;  // 5.
            jsonDocToClient[wsKey(WS_PREF_HOT_SPOT_SSID)]          = prfHotSsi;     // 6.
            jsonDocToClient[wsKey(WS_PREF_HOT_SPOT_PASS)]          = prfHotPas;     // 7.
            jsonDocToClient[wsKey(WS_SOCKET_NUM)]                  = clientId;      // 35.
    }
}

 /**
 * ------------------------------------------------
 *      Load "operate" page data into JSON doc.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.10 [2026-01-08-01:30pm] New
 * @since  3.0.12 [2026-02-18-11:00pm] Shorten RTCM & NMEA status.
 * @see    Global vars: WebSockets, setup().
 */

 void operDataToJsonDoc() {

    // --- Local vars. ---
    const  int8_t MIN_SATELLITE_THRESHHOLD =  2;  // Minimum SIV for reliable coordinate information.
    char numberbuffer[64];

    // --- Fill jsonDocToClient. ---
    jsonDocToClient.clear();
    if (roverGNSS.getSIV() > MIN_SATELLITE_THRESHHOLD) {            // Enough satellites?

        // --- Fix type. ---
        if (roverGNSS.getFixType() == 3) {
            jsonDocToClient[wsKey(WS_GNSS_FIX)] = 1;                // Single.
        } else if (roverGNSS.getCarrierSolutionType() == 1 ) {
            jsonDocToClient[wsKey(WS_GNSS_FIX)] = 2;                // RTK-float.
        } else if (roverGNSS.getCarrierSolutionType() == 2 ) {
            jsonDocToClient[wsKey(WS_GNSS_FIX)] = 3;                // RTK-fix.
        }

        // --- Satellites in view. ---
        jsonDocToClient[wsKey(WS_GNSS_SAT_IN_VIEW)] = roverGNSS.getSIV();

        /**
         * --- Heights: ---
         * H = Orthometric height (elevation above sea level). 
         * N = Geoid height/undulation (separation between ellipsoid and geoid) based on a chosen geoid model.
         * Ellipsoid height (h) = H + N.
         * u-blox receivers use EGM96 (Earth Gravitational Model 1996).
         * EGM96 is an irregular, gravity-based surface geoid model, based on a 10° x 10° grid, and interpolated to the receiver's position.
         * WGS84 is a mathematical ellipsoid (smooth, idealized shape).
         */

        // --- Height - ellipsoid (h). ---
        int32_t ellipsoid     = roverGNSS.getElipsoid();                    // mm
        int8_t ellipsoidHp    = roverGNSS.getElipsoidHp();                  // mm * 10^-1.
        float heightEllipsoid = (ellipsoid * 10 + ellipsoidHp) / 10000.0;   // Convert to meters.
        memset(numberbuffer, '\0', sizeof(numberbuffer));
        sprintf(numberbuffer, "%.2f", heightEllipsoid);
        jsonDocToClient[wsKey(WS_GNSS_HEIGHT_ELLIPSOID)] = numberbuffer;

        // Height - orthometric (H).
        int32_t msl               = roverGNSS.getMeanSeaLevel();            // a.k.a getAltitudeMSL()?
        int8_t  mslHp             = roverGNSS.getMeanSeaLevelHp();
        float   heightOrthometric = (msl * 10 + mslHp) / 10000.0;
        memset(numberbuffer, '\0', sizeof(numberbuffer));
        sprintf(numberbuffer, "%.2f", heightOrthometric);
        jsonDocToClient[wsKey(WS_GNSS_HEIGHT_ORTHOMETRIC)] = numberbuffer;

        // --- Latitude. ---
        int32_t latitude   = roverGNSS.getHighResLatitude();                // Degrees * 10^-7.
        int8_t  latitudeHp = roverGNSS.getHighResLatitudeHp();              // High precision component: degrees * 10^-9.
        double  lat        = latitude / 10000000.0;                         // Convert to to 64 bit double - degrees (8 decimal places).
        lat += latitudeHp / 1000000000.0;                                   // Add high precision component.
        memset(numberbuffer, '\0', sizeof(numberbuffer));
        sprintf(numberbuffer, "%.8f", lat);
        jsonDocToClient[wsKey(WS_GNSS_LATITUDE)] = numberbuffer;

        // --- Longitude. ---
        int32_t longitude   = roverGNSS.getHighResLongitude();
        int8_t  longitudeHp = roverGNSS.getHighResLongitudeHp();
        double  lon         = longitude / 10000000.0;
        lon += longitudeHp / 1000000000.0;
        memset(numberbuffer, '\0', sizeof(numberbuffer));
        sprintf(numberbuffer, "%.8f", lon);
        jsonDocToClient[wsKey(WS_GNSS_LONGITUDE)] = numberbuffer;

        // --- Horizontal accuracy. ---
        memset(numberbuffer, '\0', sizeof(numberbuffer));
        sprintf(numberbuffer, "%.3f", (roverGNSS.getHorizontalAccuracy() / 10000.0));
        jsonDocToClient[wsKey(WS_GNSS_HORIZONTAL_ACCURACY)] = numberbuffer;

        // --- Vertical accuracy. ---
        memset(numberbuffer, '\0', sizeof(numberbuffer));
        sprintf(numberbuffer, "%.3f", (roverGNSS.getVerticalAccuracy() / 10000.0));
        jsonDocToClient[wsKey(WS_GNSS_VERTICAL_ACCURACY)] = numberbuffer;

        // --- RTCM & BT status. ---
        jsonDocToClient[wsKey(WS_ROVER_RTCM_UP_DOWN)]    = (RTCMin)  ? "u" : "d";                    // Up, down. @see relaySerial1toSerial2().
        jsonDocToClient[wsKey(WS_ROVER_BT_NMEA_UP_DOWN)] = (NMEAout) ? "u" : "d";                    // Up, down. @see DevUBLOXGNSS::processNMEA().

        // --- Battery. ---
        memset(numberbuffer, '\0', sizeof(numberbuffer));
        sprintf(numberbuffer, "%.2f", lipo.getSOC());
        jsonDocToClient[wsKey(WS_ROVER_BATTERY_SOC)] = numberbuffer;
        memset(numberbuffer, '\0', sizeof(numberbuffer));
        sprintf(numberbuffer, "%.1f", lipo.getChangeRate());
        jsonDocToClient[wsKey(WS_ROVER_BATTERY_CHANGE_RATE)] = numberbuffer;   

        // --- Status. ---
        int32_t seconds = (esp_timer_get_time() - startTime)/1000000;
        int32_t minutes = seconds / 60;
        int32_t hours = minutes / 60;
        char uptime[15] = {'\0'};                                               // 0h 3m 8s.
        sprintf(uptime, "%uh %um %us", hours % 24, minutes % 60, seconds % 60);
        jsonDocToClient[wsKey(WS_ROVER_UP_TIME)] = uptime;
        // WebSocket status items are calculated in operate.js. All prefs use global vars.
        jsonDocToClient[wsKey(WS_NMEA_OUT_COUNT_ALL)]      = 1;                 // Todo: finish.
        jsonDocToClient[wsKey(WS_NMEA_OUT_RATE)]           = 4;
        jsonDocToClient[wsKey(WS_NMEA_OUT_COUNT_GGA)]      = nmeaCountGGA;
        jsonDocToClient[wsKey(WS_NMEA_OUT_COUNT_RMC)]      = nmeaCountRMC;
        jsonDocToClient[wsKey(WS_NMEA_OUT_COUNT_GSA)]      = nmeaCountGSA;
        jsonDocToClient[wsKey(WS_NMEA_OUT_COUNT_GSV)]      = nmeaCountGSV;
        jsonDocToClient[wsKey(WS_NMEA_OUT_COUNT_GST)]      = nmeaCountGST;
        jsonDocToClient[wsKey(WS_NMEA_OUT_COUNT_TXT)]      = nmeaCountTXT;
        jsonDocToClient[wsKey(WS_NMEA_OUT_COUNT_OTHR)]     = nmeaCountOther;
        jsonDocToClient[wsKey(WS_NMEA_OUT_COUNT_ALL)]      = nmeaCountAll;
        jsonDocToClient[wsKey(WS_NMEA_OUT_RATE)]           = nmeaRate;
        jsonDocToClient[wsKey(WS_OPERATIONAL_MODE)]        = operMode;
        jsonDocToClient[wsKey(WS_WIFI_LOCAL_NETWORK_IP)]   = localIp;
       jsonDocToClient[wsKey(WS_WIFI_HOT_SPOT_IP)]         = hotspotIp;
    } else {
        jsonDocToClient[wsKey(WS_GNSS_FIX)]                = 0;                 // GNSS down. 
        jsonDocToClient[wsKey(WS_GNSS_SAT_IN_VIEW)]        = 0;
        jsonDocToClient[wsKey(WS_GNSS_LATITUDE)]           = 0;
        jsonDocToClient[wsKey(WS_GNSS_LONGITUDE)]          = 0;
        jsonDocToClient[wsKey(WS_GNSS_HEIGHT_ELLIPSOID)]   = 0;
        jsonDocToClient[wsKey(WS_GNSS_HEIGHT_ORTHOMETRIC)] = 0;
        jsonDocToClient[wsKey(WS_GNSS_VERTICAL_ACCURACY)]  = 0;
        jsonDocToClient[wsKey(WS_PREF_RTCM_IN)]            = 0;
        jsonDocToClient[wsKey(WS_PREF_NMEA_OUT)]           = 0;
    }
}

/**
 * ============================================================================
 *  Setup functions.
 * ============================================================================
 *
 * @since 3.0.11 [2026-01-08-10:30am] Browser initiated updates.
 * @see   showBuild()            - Display build & processor info.
 * @see   prefUtility(PREF_INIT) - Preference utility.
 * @see   startSerial()          - Start serial interfaces.
 * @see   initPins()             - Initialize pins & pin values.
 * @see   startI2C()             - Start I2C wire interfaces.
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
 * @see    Global vars: Version, setup().
 * @link   https://github.com/pycom/pycom-esp-idf.
 */
void showBuild() {

    // --- Local vars. ---
    const uint8_t MAJOR_VERSION = 3;
    const uint8_t MINOR_VERSION = 0;
    const uint8_t PATCH_VERSION = 12;
    const char    NAME[]        = "Ghost Rover 3";
    const uint32_t SERIAL_USB_SPEED = 115200;   // Serial USB speed.
          esp_chip_info_t chip_info;

    // --- Run. ---
    startTime = esp_timer_get_time();
    ws2812LedColor = WHITE;
    ws2812LedBlink = false;
    statusLedOn();
    Serial.begin(SERIAL_USB_SPEED);
    serialState[0] = 'u';   // Serial USB is up [u] [S0] [S1] [S2].
    esp_chip_info(&chip_info);
    sprintf(buildString, "%u.%u.%u - %s @ %s", MAJOR_VERSION, MINOR_VERSION, PATCH_VERSION, __DATE__, __TIME__);
    Serial.printf("\n%s\n%s\n", NAME, buildString);
    Serial.printf("Using %s, Rev %d, %d core(s), ID (MAC) %012llX.\n", ESP.getChipModel(), chip_info.revision, chip_info.cores, ESP.getEfuseMac());
    Serial.println("setup() started.");
    Serial.printf("Serial (USB) started @ %u bps.\n", SERIAL_USB_SPEED);
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
 * @see    showBuild(), setup().
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
 *      Start I2C wire interfaces.
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
 *      Start WiFi.
 * ------------------------------------------------
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

    WiFi.mode(WIFI_AP_STA);                     // Enable dual mode

    // --- Local ESP32 Access Point (AP) network. ---

    // -- Local Vars. --
    const char AP_SSID[] = "Ghost Rover";           // Local ESP32 Access Point (AP) network.
    const char AP_NAME[] = "ghost";                 // AP name.
    const IPAddress AP_LOCAL_IP(192, 168, 23, 1);   // AP host address.
    const IPAddress AP_GATEWAY(192, 168, 23, 1);    // AP gateway address.
    const IPAddress AP_SUBNET(255, 255, 255, 0);    // AP subnet mask.

    // -- Global Vars. --
    snprintf(localIp, sizeof(localIp), "%d.%d.%d.%d", AP_LOCAL_IP[0], AP_LOCAL_IP[1], AP_LOCAL_IP[2], AP_LOCAL_IP[3]);

    // -- Config & start AP. --
    if (!WiFi.softAPConfig(AP_LOCAL_IP, AP_GATEWAY, AP_SUBNET)) {   // Configure IP addresses.
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
    Serial.printf("WiFi AP \"%s\" started @ %d.%d.%d.%d.\n", AP_SSID, ip[0], ip[1], ip[2], ip[3]);

    // --- Cellular hotspot client. ---

    // -- Local Vars. --
    size_t maxTrys = 20;                        // Max # of trys to connect to STA_SSID.
    IPAddress STA_IP(172, 20, 10, 2);           // Request to use this IP address.

    // -- Global Vars. --
    snprintf(hotspotIp, sizeof(hotspotIp), "%d.%d.%d.%d", STA_IP[0], STA_IP[1], STA_IP[2], STA_IP[3]);

    // -- Config & start hotspot client. --
    // - char prfHotSsi[] = "ssid";  // WiFi hotspot client: network SSID. -
    // - char prfHotPas[] = "pass";  // WiFi hotspot client: password. -
    if ((strcmp(prfHotSsi, "ssid") != 0) && (strcmp(prfRtcIn, "ntrip") == 0)) {  // RTCMin by NTRIP requires Internet hotspot access, RTCMin by radio does not.
        WiFi.config (STA_IP);
        WiFi.begin(prfHotSsi, prfHotPas);
        Serial.printf("WiFi STA connecting to \"%s\" ", prfHotSsi);
        size_t numTrys;
        for (numTrys = 0; numTrys < maxTrys; numTrys++) {
            Serial.print('.');
            if (WiFi.status() == WL_CONNECTED) {
                Serial.printf(", connected with IP = %s.\n", WiFi.localIP().toString().c_str());
                ws2812LedColor = WHITE;
                ws2812LedBlink = false;
                statusLedOn();
                break;
            }
            delay(1000);                            // Try again.
        }
        if (numTrys == maxTrys) {
            Serial.printf(", max trys exceeded, not connected.\n", prfHotSsi);
        }
    } else {
        Serial.println("Internet WiFi STA not started (required only for RCTM in via NTRIP).");
    }
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
 * ------------------------------------------------
 *      Start HTTP server. 
 * ------------------------------------------------
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
 * ------------------------------------------------
 *      Start WebSocket server.
 * ------------------------------------------------
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

        // --- Software reset. ---
        roverGNSS.softwareResetGNSSOnly();
        Serial.println("Reset roverGNSS. Restarting and re-enumerating.");
        delay(1000); // Short delay to allow the module to complete the reset process.

        // uint16_t    prfGnsMsrInt;  // ZED: MEASURE every Y (e.g. 100) ms.
        // uint8_t     prfGnsNavRat;  // ZED: OUTPUT every X (e.g. 5) MEASURE intervals every (e.g. 5*100=500) ms.
        // roverGNSS.setNavigationFrequency(2) will produce 1 solution every 500ms, but only uses 2 (not 5) measurements per second.
        roverGNSS.setNavigationRate(prfGnsNavRat, VAL_LAYER_RAM);
        roverGNSS.setMeasurementRate(prfGnsMsrInt, VAL_LAYER_RAM);
        Serial.printf("roverGNSS started.\nroverGNSS solution output every (%u * %u) ms.\n", prfGnsNavRat, prfGnsMsrInt);
    }

    // --- New config template. ---
    roverGNSS.newCfgValset(VAL_LAYER_RAM);                          // Save only to RAM.

    // -- Enable high precision mode. --
    roverGNSS.addCfgValset(UBLOX_CFG_NMEA_HIGHPREC,          1);    // NMEA - High precision (7 instead of 5 decimal places for lat/lon in NMEA sentences).

    // -- Push solutions onto I2. --
    roverGNSS.addCfgValset(UBLOX_CFG_MSGOUT_UBX_NAV_PVT_I2C, 1);    // Output solutions periodically on I2C.  

    // -- Minimize ZED processing: UART1 & SPI are not used. UART2 only uses RTCM in. UBX & NMEA over I2C, UBX & NMEA over USB for pygpsclient debugging. --
    roverGNSS.addCfgValset(UBLOX_CFG_UART1_ENABLED,          0);    // UART1 - Disable (on by default).  SPI is off by default.
    roverGNSS.addCfgValset(UBLOX_CFG_UART2INPROT_SPARTN,     0);    // UART2 - Disable SPARTN in (on by default). Only RTCM3 in is needed.
    roverGNSS.addCfgValset(UBLOX_CFG_UART2INPROT_UBX,        0);    // UART2 - Disable UBX in (on by default). 
    roverGNSS.addCfgValset(UBLOX_CFG_UART2OUTPROT_UBX,       0);    // UART2 - Disable UBX in (on by default).
    roverGNSS.addCfgValset(UBLOX_CFG_UART2OUTPROT_RTCM3X,    0);    // UART2 - Disable RTCM3 out (on by default).
    roverGNSS.addCfgValset(UBLOX_CFG_I2CINPROT_RTCM3X,       0);    // I2C - Disable RTCM3 in (on by default).
    roverGNSS.addCfgValset(UBLOX_CFG_I2COUTPROT_RTCM3X,      0);    // I2C - Disable RTCM3 out (on by default).
    roverGNSS.addCfgValset(UBLOX_CFG_USBINPROT_RTCM3X,       0);    // USB - Disable RTCM3 in (on by default).
    roverGNSS.addCfgValset(UBLOX_CFG_USBOUTPROT_RTCM3X,      0);    // USB - Disable RTCM3 out (on by default).

    // -- Minimize I2C bandwidth. --
    roverGNSS.addCfgValset(UBLOX_CFG_MSGOUT_NMEA_ID_GLL_I2C, 0);    // I2C messages - Disable GLL (on by default).
    roverGNSS.addCfgValset(UBLOX_CFG_MSGOUT_NMEA_ID_VTG_I2C, 0);    // I2C messages - Disable GLL (on by default).
    roverGNSS.addCfgValset(UBLOX_CFG_MSGOUT_NMEA_ID_GSA_I2C, 3);    // I2C messages - Reduce GSA to 1 per 3 solutions (default is 1 per 1 solution).
    roverGNSS.addCfgValset(UBLOX_CFG_MSGOUT_NMEA_ID_GSV_I2C, 5);    // I2C messages - Reduce GSV to 1 per 5 solutions (default is 1 per 1 solution).
    roverGNSS.addCfgValset(UBLOX_CFG_MSGOUT_NMEA_ID_GST_I2C, 3);    // I2C messages - Enable GSA to 1 per 3 solutions  (default is 1 per 1 solution).
                                                                    // ZDA & GNS sentences are off by default.
    // -- Send the config. --
    roverGNSS.sendCfgValset() ? Serial.println("roverGNSS configured using valset keys.") : Serial.println("roverGNSS config failed.");

    // Not used.
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
 * ============================================================================
 *  Task functions.
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
 * ============================================================================
 *  Event handlers.
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
            ws2812LedColor = GREEN;                          // Loop status indicator LED.
            ws2812LedBlink = false;
            wsSendCount    = 0;                             // Reset counter.
            break;
        case WS_EVT_DISCONNECT:
            Serial.printf("WS #%u: disconnected.\n\n", clientId);
            ws2812LedColor = BLUE;
            ws2812LedBlink = false;
            wsSendCount    = 0;                             // Reset counter.
            break;
        case WS_EVT_DATA:
            onWebSocketMessage(arg, data, len);             // Register WebSocket message handler.
            break;
        case WS_EVT_PONG:
        case WS_EVT_ERROR:
            // ws2812LedColor = RED;
            // ws2812LedBlink = false;
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
 * @since  3.0.10 [2026-01-07-02:30pm] Change {"opr":"ready"} to {"opr":"?"}.
 * @since  3.0.10 [2026-01-08-09:30am] Shortened keywords (e.g. latitude to lat).
 * @since  3.0.11 [2026-01-08-10:30am] Browser initiated updates.
 * @since  3.0.11 [2026-01-22-02:45pm] Add laser logic.
 * @since  3.0.12 [2026-02-06-06:15pm] Add preferences.
 * @since  3.0.12 [2026-02-07-07:30am] Check for {"page":"opr/cfg/menu/nmea"}.
 * @since  3.0.12 [2026-02-19-04:00pm] Removed leaving message.
 * @see    Global vars: GNSS, prefUtility(), onWebSocketEvent(), startWebSocketServer().
 * @link   https://randomnerdtutorials.com/esp32-websocket-server-arduino/.
 * @link   https://randomnerdtutorials.com/esp32-websocket-server-sensor/.
 * @link   https://shawnhymel.com/1882/how-to-create-a-web-server-with-websockets-using-an-esp32-in-arduino/.
 * @link   https://arduinojson.org/v6/api/json/deserializejson/.
 * @link   https://arduinojson.org/v6/doc/deserialization/.
 * @link   https://arduinojson.org/v7/api/jsonvariant/.
 * @link   https://github.com/espressif/arduino-esp32/blob/master/libraries/SD/examples/SD_Test/SD_Test.ino.
 * @link   https://docs.espressif.com/projects/arduino-esp32/en/latest/tutorials/preferences.html.
 * @link   https://github.com/espressif/arduino-esp32/tree/master/libraries/Preferences/.
 */

void onWebSocketMessage(void *arg, uint8_t *data, size_t len) {

    // --- Local vars. ---
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    // jsonDocFromClient is a global var.
    // jsonDocToClient is a global var.

    // --- WebSocket message. ---
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {   // Full message has been received.

        // -- Debug. Print data received. --
        if (commandFlag[DEBUG_WS]) {
            Serial.printf("WS #%u: browser --> %s\n", clientId, data);
        }

        // -- WebSocket message - deserialize the JSON data into a JSON object: jsonDocFromClient. --
        jsonDocFromClient.clear();
        DeserializationError error = deserializeJson(jsonDocFromClient, data);
        if (error) {
            Serial.printf("JSON deserialize failed: %s\n", error.f_str());
            return;
        }

        // -- WebSocket message - validate data is an array with either a one or two elements. --
        if (!jsonDocFromClient.is<JsonArray>() || jsonDocFromClient.as<JsonArray>().size() > 2) {
            Serial.println("Invalid Websocket message format");
            return;
        }

        // -- WebSocket message - process the JSON array based on (term, subTerm) in the first element and (data) in the second element. --
        jsonDocToClient.clear();
        JsonObject instruction = jsonDocFromClient[0].as<JsonObject>();     // Convert each array element into JSON object.
        const char* term       = nullptr;
        const char* subTerm    = nullptr;
        for (JsonPair kv : instruction) {                                   // Iterate kv pairs in the first array element since "term" will vary.
            term = kv.key().c_str();
            subTerm = kv.value().as<const char*>();
        }

        // --------------------------------------------
        // browser --> {"mcu":"restart"}.
        // browser <-- No reply.
        // --------------------------------------------
        if ((strcmp(term, "mcu") == 0) && (strcmp(subTerm, "restart") == 0 )) {
            Serial.println("Restarting ...\n");
            esp_restart();  // Restart.
        }

        // --------------------------------------------
        // browser --> {"echo":"hello/etc"}.
        // browser <-- {"echo":"hello/etc"}.
        // --------------------------------------------
        if (strcmp(term, "echo") == 0) {
            jsonDocToClient[term] = subTerm;
        }

        // --------------------------------------------
        //  browser --> {"page":"menu/nmea/files/config/operate"}.
        //  browser <-- {"0":"3.0.12 - Feb 19 2026 @ 12:46:28","1":"meter","2":"radio","3":"on","4":"50","5":"2","6":"xxx","7":"xxxx"}.
        // --------------------------------------------
        if ((strcmp(term, "page") == 0)) {
            memset(whichPage, '\0', sizeof(whichPage));
            strcpy(whichPage, subTerm);     // Set the page name from the kv value.
            prefUtility(PREF_TO_JSON, term, subTerm);
        }

        // --------------------------------------------
        // browser --> {"config":"set"}.
        // browser <-- browser {"config":"message"}.
        // --------------------------------------------
        if ((strcmp(term, "config") == 0) && (strcmp(subTerm, "set") == 0)) {
            prefUtility(PREF_SAVE, term, subTerm);
        } 

        // --------------------------------------------
        // browser --> {"config":"reset"}.
        // browser <-- browser {"config":"message"}.
        // --------------------------------------------
        if ((strcmp(subTerm, "reset") == 0)) {
            prefUtility(PREF_RESET);     // Reset preferences to defaults.
            prefUtility(PREF_TO_JSON);   // Send new preference values back to browser.
            jsonDocToClient["config"] = "Preferences reset to defaults.";
        }

        // --------------------------------------------
        // browser --> {"listFiles":""}.
        // browser <-- {"listFiles":"file1.ext, file2.ext, ..."}.
        // --------------------------------------------
        if (strcmp(term, "listFiles") == 0) {
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
            jsonDocToClient[term] = output;
        }

        // --------------------------------------------
        // browser --> {"deleteFile":"filename"}.
        // browser <-- {"deleteFile":"fileDeleted/fileNOTdeleted"}.
        // --------------------------------------------
        if (strcmp(term, "deleteFile") == 0) {
            if (SD.remove(subTerm)) {  // Delete file.
                jsonDocToClient["fileDeleted"] = subTerm;
            } else {
                jsonDocToClient["fileNOTdeleted"] =                     jsonDocToClient["fileDeleted"] = subTerm;
            }
        }

        // --------------------------------------------
        //  browser --> {"laser"/height/position:"lock/unlock"}.
        //  browser <-- {"laser"/height/position:"locked/unlocked"}.
        // --------------------------------------------
        if ((strcmp(term, "laser") == 0) || (strcmp(term, "height") == 0) || (strcmp(term, "position") == 0)) {
            if ((strcmp(subTerm, "lock") == 0) || (strcmp(subTerm, "unlock") == 0)) {
                    
                // - Laser pointer button. -
                //   @link https://www.build-electronic-circuits.com/arduino-laser-module-ky-008/.
                //   @link https://docs.sparkfun.com/SparkFun_Thing_Plus_ESP32-S3/arduino_example/#rgb-led.
                if (strcmp(term, "laser") == 0) {
                    if (strcmp(subTerm, "lock") == 0) {                          
                        digitalWrite(LSR_TRIGGER, HIGH);        // Turn laser on.
                    }
                    if (strcmp(subTerm, "unlock") == 0) { 
                        digitalWrite(LSR_TRIGGER, LOW);         // Turn laser off.
                    }
                }

                // - Height button. -
                if (strcmp(term, "height") == 0) {
                    if (strcmp(subTerm, "lock") == 0) {  
                        // ToDo: the work.
                    }
                    if (strcmp(subTerm, "unlock") == 0) {  
                        // ToDo: the work.
                    }
                }

                // - Position button. -
                if (strcmp(term, "height") == 0) {
                    if (strcmp(subTerm, "lock") == 0) {  
                        // ToDo: the work.
                    }
                    if (strcmp(subTerm, "unlock") == 0) {  
                        // ToDo: the work.
                    }
                }
                    
                // - Reply with value sent + "ed". -
                if ((strcmp(term, "laser") == 0) || (strcmp(term, "height") == 0) || (strcmp(term, "position") == 0)) {
                    memset(JSONbuffer, '\0', sizeof(JSONbuffer));
                    strcpy(JSONbuffer, subTerm);
                    strcat(JSONbuffer, "ed");
                    jsonDocToClient[term] = JSONbuffer;
                }
            }
        }
    }

    memset(JSONbuffer, '\0', sizeof(JSONbuffer));
    serializeJson(jsonDocToClient, JSONbuffer, sizeof(JSONbuffer));
    ws.textAll(JSONbuffer);                         // Send WebSocket message.
    wsSendCount++;
    if (commandFlag[DEBUG_WS]) {                    // Debug.
        Serial.printf("WS #%u: browser <-- %s\n", clientId, JSONbuffer);
    }
}

/**
 * ------------------------------------------------
 *      DevUBLOXGNSS task - SparkFun_u-blox_GNSS_v3 library: process NMEA bytes.
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
 * @since  3.0.11 [2026-01-23-10:15am] Added startI2C(), DEBUG_NMEA_HEX.
 * @since  3.0.12 [2026-02-07-11:00am] Check prfNmeOut preference.
 * @since  3.0.12 [2026-02-18-11:00pm] Shorten RTCM & NMEA status.
 * @link   https://docs.espressif.com/projects/arduino-esp32/en/latest/api/wifi.html.
 * @link   https://github.com/sparkfun/SparkFun_u-blox_GNSS_v3/tree/main/examples/Basics/Example2_NMEAParsing.
 */
void DevUBLOXGNSS::processNMEA(char incoming) {

    // --- Check preference. ---
    if (strcmp(prfNmeOut, "off") == 0) {
        return;
    }

    // --- Local vars. ---
    static char    nmeaBuffer[120]                   = {'\0'};              // Buffer for NMEA sentence.
           uint8_t writeStatus;                                             // Return value from Wire.endTransmission().
           static uint64_t nmeaSolutionLength        = 1;
           static bool     nmeaSolutionBlockComplete = false;

    // --- Loop. ---
    if (inLoop) {
        strncat(nmeaBuffer, &incoming, 1);                                  // Add NMEA byte from RTK-SMA to outbound buffer.
        if ((incoming == '\n') && (nmeaBuffer[0] == '$')) {                 // We have a full sentence.
            if (i2cUp) {                                                    // Slave is up.
                Wire1.beginTransmission(8);                                 // Prepare to send on I2C1.
                for (int i = 0; i < strlen(nmeaBuffer); i++) {              // Add bytes to output queue.
                    Wire1.write(nmeaBuffer[i]);
                }
                if (strcmp(prfNmeOut, "off") != 0) {                        // Set in preferences.
                    writeStatus = Wire1.endTransmission(8);                 // Send sentence on I2C1.
                }
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
                    if (strcmp(whichPage, "nmea") == 0) {                   // If on NMEA page, send NMEA sentence out via websocket.
                        jsonDocToClient.clear();
                        jsonDocToClient["nmea"] = nmeaBuffer;
                        memset(JSONbuffer, '\0', sizeof(JSONbuffer));
                        serializeJson(jsonDocToClient, JSONbuffer, sizeof(JSONbuffer));
                        ws.textAll(JSONbuffer);                             // Send WebSocket message.
                        if (commandFlag[DEBUG_WS]) {                        // Debug.
                            Serial.printf("WS #%u: browser <-- %s\n", clientId, JSONbuffer);
                        }
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
 * ============================================================================
 *  Loop functions.
 * ============================================================================
 * 
 * Check task functions and event handlers. These are independent of loop().
 * 
 * @since 3.0.11 [2026-01-12-06:00pm] Browser initiated updates.
 * @see checkZED()                - NMEA - Check ZED to trigger DevUBLOXGNSS::processNMEA().
 * @see relaySerial1toSerial2()   - RTCM - Relay from Serial1 (HC-12 radio) to Serial2 (ZED UART2).
 * @see rtcm3GetMessageType()     - RTCM - Return RTCM3 message type.
 * @see checkSerialUSB()          - Check serial USB for input.
 * @see debug()                   - Display debug.
 * @see checkGnssLockButton()     - Check GNSS lock button. // ToDo: implement.
 * @see ws.cleanupClients()       - HTTP WebSocket cleanup.
 */

 /**
 * ------------------------------------------------
 *      NMEA - Check ZED to trigger DevUBLOXGNSS::processNMEA().
 * ------------------------------------------------
 * 
 * (prfGnsNavRat * prfGnsMsrInt) = interval (ms) to query ZED for PVT data.
 * 
 *
 * @return void No output is returned.
 * @since  3.0.12 [2026-02-08-05:00pm] New.
 * @since  3.0.12 [2026-02-14-06:15pm] Replace prfRqsPvtInt with (prfGnsNavRat * prfGnsMsrInt).
 * @see    DevUBLOXGNSS::processNMEA().
 */
void checkZED() {

    // --- "nmea" page. ---
    if (strcmp(whichPage, "nmea") == 0) {

        // --- Local vars. ---
        const int64_t  THROTTLE_CHECK_ZED = (prfGnsNavRat * prfGnsMsrInt) * 1000;   // Convert from (us) to (ms)Time (us), time between checkZED().
        static int64_t lastZedCheck = esp_timer_get_time();                         // Throttle. Initialize only once, then persist.
            int64_t lastTime;

        // --- Throttle loop() calls. ---
        if ((esp_timer_get_time() - lastZedCheck) < THROTTLE_CHECK_ZED) {           // Not time to run.
            return; 
        }
        lastZedCheck = esp_timer_get_time();                                        // Time to run. Reset timer.

        // --- Check ZED. ---
        roverGNSS.checkUblox();
        lastTime = esp_timer_get_time();
    }

    // --- "operate" page ---
    if (strcmp(whichPage, "operate") == 0) {

        // --- Load data. ---
        operDataToJsonDoc();

        // --- Send update.
        memset(JSONbuffer, '\0', sizeof(JSONbuffer));
        serializeJson(jsonDocToClient, JSONbuffer, sizeof(JSONbuffer));
        ws.textAll(JSONbuffer);                                                     // Send WebSocket message.
        if (commandFlag[DEBUG_WS]) {                                                // Debug.
            Serial.printf("WS #%u: browser <-- %s\n", clientId, JSONbuffer);
        }
    }
}

 /**
 * ------------------------------------------------
 *      RTCM - Relay from Serial1 (HC-12 radio) to Serial2 (ZED UART2).
 * ------------------------------------------------
 * 
 * RTCM preamble = '11010011 000000xx' = 0xd3 0x00.
 *
 *  ESP32-S3 Serial1 (HC12) is set to 9,600 bps (default speed) in Global Vars.
 *  ESP32-S3 Serial2 (ZED UART2) is set to 57,600 bps in Global Vars.
 *  RTK-SMA (ZED UART2) is set to 57,600 bps by default (could change in startAndConfigGNSS() ).
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
 * @since  3.0.12 [2026-02-01-12:35pm] Check prfRtcIn preference.
 * @see    Global vars: Serial, startSerialInterfaces(), loop().
 * @link   https://github.com/sparkfun/SparkFun_u-blox_GNSS_v3/blob/main/examples/ZED-F9P/Example3_StartRTCMBase/Example3_StartRTCMBase.ino.
 * @link   https://www.use-snip.com/kb/knowledge-base/an-rtcm-message-cheat-sheet/.
 * @link   https://www.use-snip.com/kb/knowledge-base/rtcm-3-message-list/.
 * @link   https://www.singularxyz.com/blog_detail/11.
 */
void relaySerial1toSerial2() {

    // --- Check preference. ---
    if (strcmp(prfRtcIn, "off") == 0) {
        return;
    }
    
    // --- Local vars. ---
    const  uint16_t RTCM_TIMEOUT      = 3000000;            // Time (us) not to exceed for RTCM input received (3 sec).
    static uint8_t  preamble          =       0;
    static uint16_t byteCount         =       0;
    static int64_t  lastRTCMtime      =       0;            // Last time (us) when RTCM input received.
    static char     rtcmSentence[300] =  {'\0'};            // RTCM3 sentence buffer.
           uint16_t msg_type          =       0;

    // --- Check for Radio down. Set RTCMin state. ---
    if ((esp_timer_get_time()-lastRTCMtime) > RTCM_TIMEOUT) {       // RTCM received within RTCM_TIMEOUT?
        RTCMin = false;
        ws2812LedColor = RED;
        ws2812LedBlink = false;
    }

    // --- Read from Serial1 (HC-12), write to Serial2 (ZED UART2). --
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
             ws2812LedColor = GREEN;
             ws2812LedBlink = true;
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
 *      RTCM - Return RTCM3 message type.
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
uint16_t rtcm3GetMessageType(const char* rtcmSentence) {
    // Serial.printf("[%02x] [%02x] [%02x] [%02x] [%02x]\n", rtcmSentence[0],  rtcmSentence[1], rtcmSentence[2], rtcmSentence[3], rtcmSentence[3]);
    if (rtcmSentence[0] != 0xD3) {    // Check if preamble is correct
        return 0;               // Invalid preamble.
    }
    uint16_t message_type = ((uint16_t)rtcmSentence[3] << 4) | (rtcmSentence[4] >> 4);
    return message_type;
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
 * ------------------------------------------------
 *  Display debug.
 * ------------------------------------------------
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

    // --- RTCM in. ---
    // @see relaySerial1toSerial2().

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
            "USB %c  Serial0 (unused) %c serial1 (&rarr; HC12) %c  serial2 (ZED UART2 &rarr;) %c\n",
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
        uint8_t status = Wire1.endTransmission(8);              // Test Wire1. Is device up?
        Serial.print("Wire 1 is ");
        if (status != 0) {
            Serial.printf("down. Error = %i. \n", status);
            ws2812LedColor = RED;
            ws2812LedBlink = false;
            i2cUp = false;                                      // Slave is down.
            startI2C();                                         // Restart Wire1.
        } else {                                                // 0 = success (slave ACKed).
            ws2812LedColor = WHITE;
            ws2812LedBlink = false;
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
 * ============================================================================
 *  Setup.
 * ============================================================================
 *
 * @since  3.0.3 [2025-10-13-01:00pm] New.
 * @since  3.0.8 [2025-11-21-12:15pm] Removed startWiFiClient(), added startWiFiSoftAP().
 * @see    Global vars.
 */

void setup() {
    showBuild();                // Display build & processor info.
    prefUtility(PREF_INIT);     // Check preferences.
    startSerial();              // Start serial interfaces.
    initPins();                 // Initialize pin modes & pin values.
    startI2C();                 // Start I2C wire interfaces.
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
 *  Loop.
 * ============================================================================
 * 
 * @since 3.0.10 [2025-12-27-08:00pm] New.
 * @see   startTasks().
 * @see   Task functions.
 * @see   Event handlers.
 */

void loop() {
    checkZED();                         // NMEA - Check ZED to trigger DevUBLOXGNSS::processNMEA().
    relaySerial1toSerial2();            // RTCM - Relay from Serial1 (HC-12 radio) to Serial2 (ZED UART2).
    checkSerialUSB();                   // Check serial USB for input.
    // checkGnssLockButton();           // Check GNSS lock button. // ToDo: implement.
    ws.cleanupClients();                // HTTP WebSocket cleanup.
    debug();                            // Display debug.
}

