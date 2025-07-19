/**
 * ***********************************
 *      Ghost Rover.
 * ***********************************
 * 
 * Now you see me, now you don't. But you know exactly where I was.
 * 
 * @author   D. Foster <doug@dougfoster.me>.
 * @since    0.1.0 [2025-04-24-12:00pm] New.
 * @since    0.3.0 [2025-04-29-05:30pm].
 * @since    0.3.1 [2025-04-30-12:30pm].
 * @since    0.3.2 [2025-05-01-09:30pm].
 * @since    0.3.4 [2025-05-02-02:00pm].
 * @since    0.3.4 [2025-05-03-11:15am].
 * @since    0.3.7 [2025-05-08-12:00am].
 * @since    0.3.9 [2025-05-10-12:15pm].
 * @since    0.4.6 [2025-05-20-06:45pm].
 * @since    0.4.8 [2025-05-21-08:15pm].
 * @since    0.4.8 [2025-05-22-12:00pm].
 * @since    0.5.0 [2025-05-28-05:45pm].
 * @since    0.5.1 [2025-05-29-11:15am].
 * @since    0.5.2 [2025-06-26-02:45pm]. Changed LED_RADIO to 6, LED_BLE to 7.
 * @since    0.6.1 [2025-07-12-10:00am]. Refactor - move Bluetooth to ESP32-C6.
 * @since    0.6.4 [2025-07-19-01:00pm].
 * @link     http://dougfoster.me.
 * @link     https://www.build-electronic-circuits.com/arduino-laser-module-ky-008/.
 *
 * ===================================
 *      Comments.
 * ===================================
 * 
 * --- Description & operation. ---
 *     -- Primary use is to provide SW Maps (or another GNSS app) precise location data for geotagging
 *        a photograph. The photos will most often locate grave headstones in a cemetery or artifacts
 *        found while investigating historical sites.
 * 
 *        What makes Ghost Rover unique? It provides a precise location for the photograph's subject
 *        without the rover being visible in the photo. This is done by:
 *          1) positioning the rover above the subject and obtaining a fix,
 *          2) pressing the GNSS lock button,
 *          3) moving the rover out of view for the photograph,
 *          4) taking the photo,
 *          5) releasing the GNSS lock button,
 *          6) moving on to the next subject.
 * 
 *        While the GNSS lock button is down, the rover will continue to send the position of where the button
 *        was pressed, regardless of its current location (this allows moving the rover out of view for the
 *        photograph). When the button is released, the rover returns to sending current position coordinates.
 *        These location coordinates are transmitted from the rover to the GNSS app over a Bluetooth Low Energy
 *        serial (aka BLEserial) wireless link.
 * 
 *        The primary rover processor is a SparkFun ESP32-S3 Thing+ board. This processor receives NMEA sentences
 *        from the RTK-SMA GNSS board, then sends them to a SEEED XIAO ESP32C6 board dedicated to running the 
 *        BleSerial library, which then transmits the GNSS sentences out over Bluetooth BLE.
 * 
 *        The rover UI uses a:
 *          1) red LED (flashes to indicate reception of RTK correction data from an RF radio link),
 *          2) yellow LED (flashes to indicates GNSS activity),
 *          3) blue LED (flashes to indicate BLEserial transmissions or remains solid indicating locked GNSS
 *             corrdinates,
 *          4) side-mounted off/on switch to control +5v power in from a USB portable battery pack,
 *          5) push-to-latch button (the GNSS lock button described above),
 *          6) temporary contact push button (turns the vertical laser pointer on or off).
 *          7) 16 character x 5 row OLED display for GNSS information (latitude, longitude, elevation (above
 *             mean sea level), horizontal & vertical accuracy, satellites in view, GNSS fix type, GNSS solution
 *             type, and network status for both BLEserial & WiFi communications).
 *
 *        The rover case:
 *          1) encloses all UI components in a clear acrylic clam-shell case,
 *          2) has a removable UHF external antenna for the RF link,
 *          3) has an attached survey grade GNSSS 3 channel antenna.
 * 
 *        Inside the clam-shell case, a removable USB battery pack provides power to major components over
 *        a dedicated USB-A power cable. Optionally the rover can receive power through any of the MCU or GNSS USB
 *        serial ports.
 *
 *        https://github.com/doug-foster/DougFoster_Ghost_Rover_BT_relay:
 *          While the rover does display GNSS location on the OLED for quick reference, its main purpose is to
 *          unicast NMEA sentences (GGA & RMC for position & time, GSA & GSV for skyplot, and GST for accuracy)
 *          over BLEserial to a receiver (like a mobile device running SW Maps) which can take photos and geotag
 *          them using that transmitted data. The SparkFun_u-blox_GNSS_v3 library is used to configure the
 *          GNSS processor (in this case a uBlox ZED-F9P) to calculate a fix twice per second and transmit these
 *          (5) sentences twice every second. A dedicated SEED XIAO ESP32C6 MCU runs the BleSerial library which
 *          receives GNSS sentences from the main ESP32-S3 processor over a serial connection, then sends the
 *          sentences out byte-by-byte out over a Bluetooth Low Energy (BLE) connection.
 * 
 *        The SparkFun_u-blox_GNSS_v3 library was also used to configure the GNSS processor to operate in high
 *        precision mode. This means position information (both on the OLED display and in the BLEserial NMEA
 *        sentences) is calculated and reported with seven decimal position precision rather than standard NMEA
 *        five decimal precision.
 * 
 *        https://github.com/doug-foster/DougFoster_Ghost_Rover_EVK_RTCM_relay:
 *          The rover uses an RF link (internal HC-12 radio) to receive RTCM3 correction data from a companion base
 *          station - a Sparkfun EVK running Sparkfun's RTK everywhere firmware. Inside the EVK enclosure, an
 *          identical ESP32-S3 MCU (SparkFun Thing Plus ESP32-S3) and HC-12 RF radio were added. Both devices are
 *          powered by a connection to the EVK's internal I2C bus. 
 * 
 *        When the EVK is in base mode and a fix has been obtained, it will send an RTCM3 correction stream out
 *        over two lugs on a terminal block exposed at the back of the EVK. The add-on MCU runs a simple serial
 *        relay sketch which reads data in byte-by-byte from one of the MCU's serial ports (which is wired to
 *        the EVK's terminal lugs (UART2 of the EVK's internal ZED-F9P). The sketch then relays the data
 *        byte-by-byte, out another serial port which is wired to the HC-12. The HC-12 transmits the serial
 *        data stream over RF to the rover.
 * 
 *        [A future code update will support stand-alone mode (no base needed) by acquiring correction data
 *        from Ublox's PointPerfect (PP) service over a WiFi connected hot spot. For now, the current code will
 *        default to base mode on startup. However, if the rover is booted with the GNSS lock button in the
 *        downPosition on start up - as opposed to the normal upPosition - the rover mode wil be set to PP, which
 *        will be used to trigger the  future enhancement.]
 * 
 *        [Another future addition will be a wifi/web server used to:
 *         1) remotely operate the rover (i.e. when attached to the top of a survey pole),
 *         2) display location data,
 *         3) allow for advanced rover configuration.]
 * 
 *        While using a temporary base station with the rover is physically not as convenient as a stand-alone
 *        rover using PP, it does minimize the amount of time needed connected to the PP service. By using a
 *        a temporary base, which only connects to PP during the base's initial survey-in mode, one can be
 *        quite stingy with the PP service time needed.
 * 
 *        WHen the rover boots, it will first progress into 3D GNSS fix. This is indicated by the status line
 *        on the OLED display (e.g. for F3 S0: Fix=3, Solution=0). Also, the yellow LED fashes once every second. When
 *        the rover is receiveing RTCM data, it will enter RTK float mode (displaying F3 S1: Solution = 1 and
 *        present two quick flashes every second. When the rover finally enters RTK fix mode, it will display
 *        F3 S2: Solution = 2 and the yellow LED will quickly flash three times every second. When/if the rover's
 *        fix acuracy digresses from RTK, the display and LEDs will display an appropriate status.
 * 
 *        If no BLE transmission occurs, or no GNSS update is received, or no RTCM data received over the RF
 *        link from the base, the LEDs will correspondingly be off. When data is received again, the LEDs will
 *        begin flashing their status. At startup, the OLED display may show no location data (all 0's) until
 *        enough satellites have been acquired to obtain a valid GNSS solution (aka fix). Ghost rover configures
 *        the GNSS settings both in RAM and battery backed-up RAM (BBR). The battery is trickle charged by the
 *        Sparkfun GNSS board and (if not needing to be replaced), should hold the GNSS settings as well as last
 *        fix information for about 2 weeks. This can significantly reduce time to first fix for the GNSS.
 * 
 *        The rover uses an ESP32-S3 MCU I2C connection @ 400,000 kbps for:
 *          1) communication with the OLED display,
 *          2) configuring the TK-SMA GNSS receiver via UBX commands,
 *          2) receiving NMEA sentences from the RTK-SMA GNSS receiver,
 *          3) communicating with the SparkFun L53L1X Time of Flight (TOF) distance sensor.
 * 
 *        A set of serial commands (entered over the serial monitor USB interface of all MCUs) are provided for
 *        testing and debugging operation. checkSerialMonitor() in the loop() section watches for a command.
 *        To see a list of available commands, just type some garbarge - like "dfads" - into the serial monitor 
 *        command line for any MCU. The response will be a list of case sensitive, valid commands.
 * 
 *        "testRad" is a serial monitor test/debug command which can be used to place the rover into test
 *        mode with the HC-12 radio. As descibed by the sentence when testRad mode is initiated, the HC-12
 *        uses AT type command for configuration (e.g. type "AT" and you should receive an "OK" response back
 *        from the radio). To exit any of the test/debug modes, just type a '!' character.
 *
 * --- Rover, major components. ---
 *     -- primary MCU      https://www.sparkfun.com/sparkfun-thing-plus-esp32-s3.html.
 *     -- secondary MCU    https://www.amazon.com/dp/B0D2NKVB34.
 *     -- GNSS             https://www.sparkfun.com/sparkfun-gps-rtk-sma-breakout-zed-f9p-qwiic.html.
 *     -- HC-12 RF radio   https://www.amazon.com/dp/B01MYTE1XR.
 *     -- display          https://www.sparkfun.com/sparkfun-qwiic-oled-1-3in-128x64.html.
 *     -- distance sensor  https://www.sparkfun.com/sparkfun-distance-sensor-breakout-4-meter-vl53l1x-qwiic.html.
 *     -- laser pointer    https://www.amazon.com/dp/B07R6QFD4T.
 * 
 * --- Base station, major components. ---
 *     -- base station     https://www.sparkfun.com/sparkfun-rtk-evk.html.
 *     -- RTCM relay MCU   https://www.sparkfun.com/sparkfun-thing-plus-esp32-s3.html.
 *     -- HC-12 RF radio   https://www.amazon.com/dp/B01MYTE1XR.
 *
 * --- Other components. ---
 *     -- Rover, GNSS antenna. --
 *        - GNSS antenna (L1/L2/L5, TNC-F): https://www.sparkfun.com/gnss-multi-band-l1-l2-l5-surveying-antenna-tnc-spk6618h.html.
 *        - adapter (TNC-M to SMA-M):  https://www.amazon.com/dp/B0BGPJP3J3.
 *        - adapter (SMA-M to SMA-F):  https://www.amazon.com/dp/B00VHAZ0KW.
 *        - cable (SMA-F bulkhead to SMA-M, 6" RG316): https://www.amazon.com/dp/B081BHHPHQ.
 *     -- Rover, RF radio. --
 *        - radio (433.4-473.0 MHz, 100mW, U.FL):     https://www.amazon.com/dp/B01MYTE1XR.
 *        - antenna (UHF 400-960 MHz, BNC-M):         https://www.amazon.com/dp/B07R4PGZK3.
 *        - cable (BNC-F bulkhead to U.FL, 8" RG178): https://www.amazon.com/dp/B098HX6NFH.
 *     -- Rover, Misc. --
 *        - I2C Qwiic cable kit:  https://www.amazon.com/dp/B08HQ1VSVL.
 *        - 26 AWG stranded wire: https://www.amazon.com/dp/B089CQJHQC?th=1.
 *        - push button switch (12mm latching):  https://www.amazon.com/dp/B0CGTXMLKL.
 *        - push button switch (7mm momentary):  https://www.amazon.com/dp/B07RV1D98T.
 *        - power switch (15mm on/off latching toggle): https://www.amazon.com/dp/B09XMDXKTR.
 *        - LEDs (5mm):  https://www.amazon.com/dp/B0739RYXVC.
 *        - LED covers (5mm LED bulb socket):    https://www.amazon.com/dp/B07CQ6TH14.
 *        - battery (+5V 2.4A max, 8000 mAh):    https://www.walmart.com/ip/onn-8000mAh-Portable-Battery-Power-Bank-with-USB-A-to-C-Charging-Cable-LED-Indicator-Black/5266111773
 *        - enclosure (Pelican Micro Case 1040): https://www.rei.com/product/778220/pelican-micro-case-1040-with-carabiner.
 *        - pistol grip handle: https://www.amazon.com/dp/B01FUEXLGU.
 *        - tripod legs:        https://www.amazon.com/dp/B07GST1C2Z.
 *        - magnetic phone/tripod mount:  https://www.amazon.com/dp/B0D21RP69C.
 *        - GNSS antenna thread adapter:  https://www.sparkfun.com/antenna-thread-adapter-1-4in-to-5-8in.html
 *        - other: nuts, bolts, 1/4-10 bolt, washers, USB-A power cable, heat shrink tubing.
 *     -- Base, Misc. --
 *        - mini tripod: https://www.amazon.com/dp/B0CQ6WTRW6.
 *        - laser pointer: https://www.petsmart.com/cat/toys/interactive-and-electronic/whisker-city-thrills-and-chills-laser-cat-toy-84577.html.
 *        - battery/smartphone holder: https://www.amazon.com/dp/B07S8TTH34
 *        - 3.5mm/ 0.14 in. pitch 10 pin pluggable PCB screw terminal block connector (female): https://www.amazon.com/dp/B0BPHLZ8XN.
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
 *     -- IDE: Arduino 2.3.6.
 *     -- Board: "Sparkfun ESP32-S3 Thing Plus" (~/Library/Arduino15/packages/esp32/hardware/esp32/3.2.0/boards.txt).
 *     -- Editor: VS Code 1.100.2 (Extensions: Better Comments, Bookmarks, C/C++, C/C++ Extension Pack, C/C++ Themes,
 *        CMake Tools, Dash, Diff Folders, Git Graph, GitHub Theme, GitLens, Markdown All in One, Serial Monitor,
 *        SFTP).
 *     -- GitHub repo: https://github.com/doug-foster/DougFoster_Ghost_Rover/.
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
 * --- Code flow. ---
 *     -- Include libraries.
 *     -- Global vars: define vars, set constants, declarations.
 *     -- Functions: init, config, begin, start, check, display, callback, operation, tasks, test.
 *     -- Setup.
 *     -- Loop.
 */

// ===================================
//      Include libraries.
// ===================================

// --- Libraries. ---       // what's wrong?
#include <Wire.h>                       // https://github.com/espressif/arduino-esp32/blob/master/libraries/Wire/src/Wire.h 
// #include <WiFi.h>                       // https://github.com/espressif/arduino-esp32/tree/master/libraries/WiFi.
#include <HardwareSerial.h>             // https://github.com/espressif/arduino-esp32/blob/master/cores/esp32/HardwareSerial.cpp.
#include <esp_system.h>                 // https://github.com/pycom/esp-idf-2.0/blob/master/components/esp32/include/esp_system.h.
// #include <AsyncTCP.h>                   // https://github.com/ESP32Async/ESPAsyncWebServer.
// #include <ESPAsyncWebServer.h>          // https://github.com/ESP32Async/AsyncTCP.
#include <SparkFun_u-blox_GNSS_v3.h>    // https://github.com/sparkfun/SparkFun_u-blox_GNSS_v3 (3.1.9).
#include <SparkFun_Qwiic_OLED.h>        // https://github.com/sparkfun/SparkFun_Qwiic_OLED_Arduino_Library (1.0.13).
#include <res/qw_fnt_8x16.h>            // https://github.com/sparkfun/SparkFun_Qwiic_OLED_Arduino_Library/blob/main/src/res/qw_fnt_8x16.h.
// #include <SparkFun_VL53L1X.h>           // https://github.com/sparkfun/SparkFun_VL53L1X_Arduino_Library.

// ===================================
//      Global vars.
// ===================================

// -- Version. --
const char BUILD_DATE[]   = "2025-07-19-13:00";     // 24hr format, need to fit max (16) characters.
const char MAJOR_VERSION  = '0';
const char MINOR_VERSION  = '6';
const char PATCH_VERSION  = '4';

// -- Communication port usage. --
// Serial monitor (USB).
//
// Serial 0 (UART0) RTCM_TX: ESP32-S3 Thing+ -> RTK-SMA RX2 {white wire} - RTCM out.
// Serial 0 (UART0) NMEA_RX: ESP32-S3 Thing+ <- RTK-SMA TX2 {blue wire} - NMEA in.
//
// Serial 1 (UART1) NMEA_TX:     ESP32-S3 Thing+ -> ESP32-C6 RX {green wire}  - NMEA out.
// Serial 1 (UART1) NMEA_ACK_RX: ESP32-S3 Thing+ <- ESP32-C6 TX {yellow wire} - NMEA BT ACK in.
//
// Serial 2 (UART2) TX: ESP32-S3 Thing+ -> HC-12 RX radio {yellow wire} - AT out.
// Serial 2 (UART2) RX: ESP32-S3 Thing+ <- HC-12 TX radio {white wire} - AT/RTCM in.
//
// I2C-1 bus {Qwicc}: ESP32-S3 <-> OLED (display) <-> VL53L1X (sensor) <-> RTK-SMA GNSS (UBX).

// -- Pin (pth) definitions. --
const uint8_t NMEA_RX          = 16;            // See above.
const uint8_t RTCM_TX          = 17;            // See above.

const uint8_t NMEA_TX          = 14;            // See above.
const uint8_t NMEA_ACK_RX      = 15;            // See above.

const uint8_t HC12_RX          = 43;            // See above.
const uint8_t HC12_TX          = 44;            // See above.

const uint8_t LED_RADIO        =  6;            // ESP32-S3 Thing+ <-> Red LED               {blue wire}.
const uint8_t LED_GNSS         =  5;            // ESP32-S3 Thing+ <-> Yellow LED            {green wire}.
const uint8_t LED_BT           =  7;            // ESP32-S3 Thing+ <-> Blue LED              {white wire}.

const uint8_t BUTTON_GNSS_LOCK =  4;            // ESP32-S3 Thing+ <-> Black latching button {yellow wire}
const uint8_t BUTTON_LSR_PNTR  =  2;            // ESP32-S3 Thing+ <-> Red push button       {white wire}

const uint8_t TRIGGER_LSR_PNTR =  1;            // ESP32-S3 Thing+ <-> KY-008 trigger pin    {white wire}
const uint8_t HC12_SET         = 42;            // ESP32-S3 Thing+ <-> HC-12 SET             {blue wire}.

const uint8_t I2C_SDA          =  8;            // ESP32-S3 Thing+ <-> I2C-1 data pin        {Qwiic} - SDA.
const uint8_t I2C_SCL          =  9;            // ESP32-S3 Thing+ <-> I2C-1 clock pin       {Qwiic} - SCL.

// -- Serial monitor. --
const uint32_t SERIAL_MON_SPEED = 115200;       // Serial USB monitor speed.

// -- Serial 0. --
const uint32_t        SERIAL0_SPEED = 57600;    // ZED-F9P default speed is 38400, bump it up.
      HardwareSerial serial0(0);                // UART0 object.


// -- Serial 1. --
const uint32_t       SERIAL1_SPEED = 115200;    // ESP32-S3 <-> ESP32-C6 speed.
      HardwareSerial serial1(1);                // UART1 object.

// -- Serial 2. --
const uint32_t       SERIAL2_SPEED = 9600;      // HC-12 default speed.
      HardwareSerial serial2(2);                // UART2 object.

// -- I2C-1. --
const int32_t I2C_SPEED = 400000;               // I2C-1 clock speed (default is 100,000).

// -- OLED display. --
const int8_t  ROW[] = {0,13,26,40,53};          // Vertical px position for each row using QW_FONT_8X16.
      char    oledRow[5][17];                   // 2D array for OLED display.
Qwiic1in3OLED roverOLED;                        // OLED display object. Uses SparkFun_Qwiic_OLED library.

// -- Timing. --
const int64_t RADIO_TIMEOUT         = 3000000;  // Time (us) not to exceed for inputCharRTCM received (3 sec).
const int64_t NMEA_BT_ACK_TIMEOUT   = 5000000;  // Time (us) not to exceed for last ACK-NMEA-BT (5 sec).
const int64_t NMEA_IN_TIMEOUT       = 5000000;  // Time (us) not to exceed for last NMEA byte in (5 sec).
const int64_t THROTTLE_DEBUG        = 1000000;  // Time (us) between checkForDebug() = (every 2 sec).
const int64_t THROTTLE_OLED         = 200000;   // Time (us) between updateOLED() = (every .2 sec).
const TickType_t LED_TIME_FLASH_ON  = 100/portTICK_PERIOD_MS;  // Time (ms).
const TickType_t LED_TIME_FLASH_OFF = 100/portTICK_PERIOD_MS;  // Time (ms).
      int64_t lastRTCMtime;                     // Last time (us) when RTCM inputCharRTCM received.
      int64_t lastNMEAbtACKtime;                // Last time (us) when ACK-NMEA-BT received.
      int64_t lastNMEAinTime;                   // Last time (us) when NMEA byte received.

// -- Task handles. --
TaskHandle_t btNmeaLEDtaskHandle;               // BT NMEA        LED task handle.
TaskHandle_t gnssFixLEDtaskHandle;              // GNSS fix       LED task handle.
TaskHandle_t gnssRTKfloatLEDtaskHandle;         // GNSS RTK float LED task handle.
TaskHandle_t gnssRTKfixLEDtaskHandle;           // GNSS RTK fix   LED task handle.
TaskHandle_t radioRtcmLEDtaskHandle;            // Radio RTCM     LED task handle.
TaskHandle_t btNMEAoutTaskHandle;               // BT NMEA out task handle.

// -- GNSS. --
const int8_t   MIN_SATELLITE_THRESHHOLD = 2;    // Minimum SIV for reliable coordinate information.
      int8_t   SIV;                             // Satellites In View.
      int8_t   fixType;                         // Fix type.
      int8_t   solnType;                        // Carrier solution type. 
      int32_t  horizontalAcc;                   // Horizontal accuracy.                                                
      int32_t  verticalAcc;                     // Vertical accuracy.
      int64_t  latitude;                        // Latitude.
      int64_t  longitude;                       // Longitude.
      int64_t  elevation;                       // Elevation in meters above Mean Sea Level (-1.0 until set).
SFE_UBLOX_GNSS roverGNSS;                       // GNSS object (uses I2C-1).

// -- I/O. --
// const char eosRTCM = '\n';                      // End of sentence character.
      char monitorChar;                         // Monitor i/o character.
      char serial0Char;                         // Serial 0 i/o character.
      char serial1Char;                         // Serial 1 i/o character.
      char serial2Char;                         // Serial 2 i/o character.
      char sentenceOutNMEA[120];                // Sentence buffer (C-string) out - NMEA.
      char serial1Buffer[11];                   // Serial 1 input buffer (C-string).

// -- Operation. --
static bool     ghostMode = false;              // Flag, in Ghost mode.
       int8_t   lastButtonPos;                  // Lock button position.

// -- Commands. --
const  uint8_t NUM_COMMANDS = 15;               // How many possible commands.
const  char    EXIT_TEST   = '!';               // Exit test mode.
const  char*   COMMANDS[NUM_COMMANDS] = {       // Valid commands. Point to array of C-strings.
               "testLEDr",
               "testLEDg",
               "testLEDb",
               "testRad",
               "debugRad",
               "debugGNSS",
               "debugNMEA",              
               "debugBtn",
               "debugBT",
               "debugDis",
               "debugPgm",
               "debugSer",
               "debugUI",
               "debugNet",
               "reset"
};
       char     monitorCommand[11];             // Serial monitor command (C-string).
       char     radioCommand[11];               // Serial (radio) test command (C-string).
       bool     testLEDr;                       // Test radio LED.
       bool     testLEDg;                       // Test GNSS LED.
       bool     testLEDb;                       // Test BT LED.
       bool     testRad;                        // Test radio.
       bool     debugRad;                       // Debug radio.
       bool     debugGNSS;                      // Debug GNSS.
       bool     debugNMEA;                      // Debug NMEA.
       bool     debugBtn;                       // Debug lock button.
       bool     debugBT;                        // Debug BT.
       bool     debugDis;                       // Debug display.
       bool     debugPgm;                       // Show program state.
       bool     debugSer;                       // Show serial state.
       bool     debugUI;                        // Show UI state.
       bool     debugNet;                       // Show network state.
       bool     reset;                          // Reset MCU.

// -- State. --
    char wifiState;                             // WiFi state. Used in OLED dsiplay.
    char btState;                               // Bluetooth state. Used in OLED dsiplay.
    char pgmState[2];                           // Program state.
/**
 *    - Program state (debugPgm to debug) -
 *      [-][-] = Initalize.
 *      [s][-] = Program section: in setup().
 *      [l][-] = Program section: in loop().
 *      [-][b] = Rover mode: RTK base.
 *      [-][p] = Rover mode: RTK PointPerfect.
 */
    char serState[4];                           // Serial state.
/**
 *    - Serial state (debugSer to debug). -
 *      [-][-][-][-] = Initalize.
 *      [d][-][-][-] = USB monitor down.
 *      [u][-][-][-] = USB monitor up.
 *      [-][d][-][-] = serial 0 down.
 *      [-][u][-][-] = serial 0 up.
 *      [-][-][d][-] = serial 1 down.
 *      [-][-][u][-] = serial 1 up.
 *      [-][-][-][d] = serial 2 down.
 *      [-][-][-][u] = serial 2 up.
 */
    char UIstate[4];                            // UI state.
/**
 *    - UI state (debugUI to debug). -
 *      [-][-][-][-] = Initalize.
 *      [0][-][-][-] = GNSS lock button is in upPosition.
 *      [1][-][-][-] = GNSS lock button is in downPosition.
 *      [-][0][-][-] = Radio LED - off.
 *      [-][1][-][-] = Radio LED - on.
 *      [-][2][-][-] = Radio LED - active.
 *      [-][-][0][-] = GNSS  LED - off.
 *      [-][-][1][-] = GNSS  LED - on.
 *      [-][-][2][-] = GNSS  LED - active (GNSS fix).
 *      [-][-][3][-] = GNSS  LED - active (RTK float).
 *      [-][-][4][-] = GNSS  LED - active (RTK fix).
 *      [-][-][-][0] = BT    LED - off.
 *      [-][-][-][1] = BT    LED - on.
 *      [-][-][-][2] = BT    LED - active.

 */
    char netState[2];                           // Network state.
/**
 *    - Network state (debugNet to debug). -
 *      [-][-] = Initalize.
 *      [d][-] = NMEA out BT down.
 *      [u][-] = NMEA out BT up.
 *      [-][d] = WiFi down.
 *      [-][u] = WiFi up.
 */

// -- Declarations. --                          // Eliminate compiler scope error due to definition order.
void updateLEDs(char, char, char);
void checkGnssLockButton(char);
void updateOLED(char);

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
 * @return void No output is returned.
 * @since  0.8.7 [2025-05-13-08:45am] New.
 * @since  0.4.7 [2025-05-21-09:45am] Clean up.
 * @since  0.6.0 [2025-07-07-01:45pm] Refactor.
 */
void initVars() {

    Serial.println("setup() - Running.");
    Serial.print("initVars() - Initialize global vars");

    // -- OLED display. --
    memset(oledRow, '\0', sizeof(oledRow));

    // -- GNSS. --
    SIV           = MIN_SATELLITE_THRESHHOLD;
    fixType       = 0;
    solnType      = 0;
    horizontalAcc = 0;                                        
    verticalAcc   = 0;
    latitude      = 0;
    longitude     = 0;
    elevation     = -1.0;

    // -- Timing. --
    lastRTCMtime      = 0;                      // Interface is down until first character.
    lastNMEAbtACKtime = 0;
    lastNMEAinTime    = 0;

    // -- I/O. --
    monitorChar = '\0';
    serial0Char = '\0';
    serial1Char = '\0';
    serial2Char = '\0';

    // -- Operation. --
    lastButtonPos = 0;                          // Lock button position.

    // -- Commands. --
    memset(monitorCommand, '\0', sizeof(monitorCommand));
    memset(radioCommand,   '\0', sizeof(radioCommand));
    testLEDr  = false;
    testLEDg  = false;
    testLEDb  = false;
    testRad   = false;
    debugRad  = false;
    debugGNSS = false;
    debugNMEA = false;
    debugBtn  = false;
    debugBT  = false;
    debugDis  = false;
    debugPgm  = false;
    debugSer  = false;
    debugUI   = false;
    debugNet  = false;
    reset     = false;

    // -- State. --
    memset(pgmState, '-', sizeof(pgmState));
    memset(serState, '-', sizeof(serState));
    memset(UIstate,  '-', sizeof(UIstate));
    memset(netState, '-', sizeof(netState));
    pgmState[0] = 's';
    serState[0] = 'u';                          // USB Monitor.
    wifiState = '-';
    btState = '-';
    
    Serial.println(".");

}

// --- Config. ---

/**
 * ------------------------------------------------
 *      Initialize pin modes & pin values.
 * ------------------------------------------------
 *
 * @return void No output is returned.
 * @since  0.1.0 [2025-04-24-12:00pm] New.
 * @since  0.4.7 [2025-05-21-09:30am] Switch Radio & BT LEDs.
 * @since  0.6.0 [2025-07-19-08:00am] Refactor.
 */
void configPins() {

    // -- Initialize pin modes. --
    Serial.print("configPins() - Config pins");
    pinMode(LED_BT,           OUTPUT);
    pinMode(LED_GNSS,         OUTPUT);
    pinMode(LED_RADIO,        OUTPUT);
    pinMode(HC12_SET,         OUTPUT);          // HC-12 - set pin for AT command mode.
    pinMode(TRIGGER_LSR_PNTR, OUTPUT);          // KY-008 laser - S trigger pin.
    pinMode(BUTTON_GNSS_LOCK, INPUT_PULLUP);
    pinMode(BUTTON_LSR_PNTR,  INPUT_PULLUP);

    // -- Initialize pin values. --
    digitalWrite(LED_BT,    LOW);
    digitalWrite(LED_GNSS,  LOW);
    digitalWrite(LED_RADIO, LOW);
    digitalWrite(HC12_SET,  HIGH);              // HC-12 - initially set pin for transparent mode.
    Serial.println(".");
}

// --- Begin. ---

/**
 * ------------------------------------------------
 *      Begin serial monitor (USB).
 * ------------------------------------------------
 *
 * @return void No output is returned.
 * @since  0.1.0 [2025-04-24-12:00pm] New.
 * @since  0.3.6 [2025-05-07-12:00pm] Refactored.
 * @since  0.3.8 [2025-05-10-09:00am] Set state.
 * @since  0.4.7 [2025-05-21-09:45am] Clean up.
 * @since  0.4.9 [2025-05-27-12:00pm] Moved to initVars.
 * @since  0.5.2 [2025-06-28-08:00am] Serial.printf() used.
 * @since  0.6.0 [2025-07-07-01:45pm] Refactor.
 */
void beginSerialMonitor() {

    Serial.begin(SERIAL_MON_SPEED);
    Serial.printf("\nbeginSerialMonitor() - Begin serial monitor (USB) @ %i bps.\n", SERIAL_MON_SPEED);
}

/**
 * ------------------------------------------------
 *      Begin serial interfaces.
 * ------------------------------------------------
 *
 * @return void No output is returned.
 * @since  0.3.6 [2025-05-07-12:00pm] New.
 * @since  0.6.0 [2025-07-07-02:45pm] Refactor.
 * @link   https://github.com/G6EJD/ESP32-Using-Hardware-Serial-Ports.
 * @link   https://randomnerdtutorials.com/esp32-uart-communication-serial-arduino/#esp32-custom-uart-pins.
 */
void beginSerialInterfaces() {

    // -- Serial 0 interface. --
    Serial.printf("beginSerialInterfaces() - Begin serial 0 (UART0) @  %i bps", SERIAL0_SPEED);
    serial0.begin(SERIAL0_SPEED, SERIAL_8N1, NMEA_RX, RTCM_TX);         // UART0 object. RX, TX.
    Serial.println(".");

    // -- Serial 1 interface. --
    Serial.printf("beginSerialInterfaces() - Begin serial 1 (UART1) @  %i bps", SERIAL1_SPEED);
    serial1.begin(SERIAL1_SPEED, SERIAL_8N1, NMEA_ACK_RX, NMEA_TX);     // UART1 object. RX, TX.
    Serial.println(".");

    // -- Serial 2 interface. --
    Serial.printf("beginSerialInterfaces() - Begin serial 2 (UART2) @  %i bps", SERIAL2_SPEED);
    serial2.begin(SERIAL2_SPEED, SERIAL_8N1, HC12_TX, HC12_RX);         // UART1 object. Cross RX & TX.
    Serial.println(".");

}

/**
 * ------------------------------------------------
 *      Begin I2C interfaces.
 * ------------------------------------------------
 *
 * @return void No output is returned.
 * @since  0.1.0 [2025-04-24-12:00pm] New.
 * @since  0.2.0 [2025-04-28-12:30pm] Add error message.
 * @since  0.4.7 [2025-05-21-09:45am] Clean up.
 */
void beginI2C() {

    // -- I2C-1 interface. --
    Serial.print("beginI2C() - Begin I2C-1");
    if (Wire.begin() == false) {
        Serial.println(" failed. Freezing ...");
        while (true);               // Infinite loop.
    } else {
        Serial.println(".");
    }
}

// --- Start. ---

/**
 * ------------------------------------------------
 *      Start OLED display.
 * ------------------------------------------------
 * 
 * @return void No output is returned.
 * @since  0.1.0 [2025-04-24-12:00pm] New.
 * @since  0.4.7 [2025-05-21-09:45am] Clean up.
 * @link   https://github.com/sparkfun/SparkFun_Qwiic_OLED_Arduino_Library/blob/main/src/res/qw_fnt_8x16.h.
 */
void startDisplay() {

    // -- Start OLED display. --
    Serial.print("startDisplay() - Start display");
    if (roverOLED.begin() == false) {   // Something is wrong, freeze.
        Serial.println(" failed. Freezing ...");
        while (true);                   // Infinite loop.
    } else {
        Serial.println(".");
    }

    // -- Configure font. Only use 8x16 size. --
    roverOLED.setFont(QW_FONT_8X16);    // Set display font.
}

/**
 * ------------------------------------------------
 *      Begin & start WiFi interface.
 * ------------------------------------------------
 *
 * @return void No output is returned.
 * @since  0.1.0 [2025-05-13-03:00pm] New.
 * @since  0.4.7 [2025-05-21-09:45am] Clean up.
 */
void startWiFi() {

    // -- Start Wifi --
    Serial.print("startWiFi() - WiFi");

    // -- Set state, print status. --
    netState[1] = 'd';      // Not installed. WiFi down.
    Serial.println(" not installed.");
}

/**
 * ------------------------------------------------
 *      Start GNSS object, config ZED settings.
 * ------------------------------------------------
 * 
 * -- Uses library SparkFun_u-blox_GNSS_v3 for UBX-CFG-VALGET & UBX-CFG-VALSET binary commands.
 * -- BTW, where is startRoverRTCM?
 *    - No need. serial 2 -> serial 0 is only an RTCM relay. It does not use the GNSS library.
 *    - serial 0 on the ESP32-S3 is wired (white & blue wires) to UART2 on the RTK-SMA.
 *    - Also no need for the GNSS library to configure the RTK-SMA. The ZED-F9P default for UART2 is RTCM in.
 *
 * @return void No output is returned.
 * @since  0.1.0 [2025-04-24-12:00pm] New.
 * @since  0.3.6 [2025-05-07-02:00pm] Refactored.
 * @since  0.4.6 [2025-05-20-06:30pm] Refactored for speed change.
 * @since  0.4.7 [2025-05-21-10:30am] Clean up.
 * @since  0.4.8 [2025-05-23-09:45am] Add config options.
 * @since  0.4.9 [2025-05-27-11:15am] Adjust config options.
 * @since  0.6.0 [2025-07-07-04:00pm] Refactor.
 * @since  0.6.1 [2025-07-13-09:15am] Rework addCfgValset() commands.
 * @see    Global vars: GNSS.
 * @see    beginSerialInterfaces().
 * @see    beginI2C().
 * @link   https://github.com/sparkfun/SparkFun_u-blox_GNSS_v3/blob/main/examples/Example1_PositionVelocityTime/Example1_PositionVelocityTime.ino.
 * @link   https://github.com/sparkfun/SparkFun_u-blox_GNSS_v3/blob/main/src/u-blox_config_keys.h.
 */
void startAndConfigGNSS() {

    // -- Start GNSS interface on I2C-1. --
    Serial.print("startAndConfigGNSS() - Start roverGNSS");                // Print status.
    if (roverGNSS.begin() == false) {
        Serial.println(" failed. Freezing ...");    // Something is wrong, freeze.
        while (true);                               // Infinite loop.
    } else {
        Serial.println(".");
    }

    // -- Configure interfaces. --                                                           // what's wrong?
    Serial.print("startAndConfigGNSS() - roverGNSS: valset keys");
    roverGNSS.newCfgValset(VAL_LAYER_RAM);                                              // New config template.

    roverGNSS.addCfgValset(UBLOX_CFG_NMEA_HIGHPREC, 1);                                 // High precision NMEA.  Adds 2 more decimal places.
    // roverGNSS.addCfgValset(UBLOX_CFG_RATE_MEAS, 500);                                   // Two solutions every 1 second (2 HZ).

    // roverGNSS.addCfgValset(UBLOX_CFG_MSGOUT_UBX_NAV_PVT_I2C, 1);                        // I2C - Output solutions periodically.
    // roverGNSS.addCfgValset(UBLOX_CFG_MSGOUT_UBX_NAV_PVT_UART2, 1);                      // UART2 - Output solutions periodically.

    // roverGNSS.addCfgValset(UBLOX_CFG_I2CINPROT_UBX,       1);                           // I2C - Turn on UBX protocol in.
    // roverGNSS.addCfgValset(UBLOX_CFG_I2COUTPROT_UBX,      1);                           // I2C - Turn on UBX protocol out.
    // roverGNSS.addCfgValset(UBLOX_CFG_I2COUTPROT_NMEA,     0);                           // I2C - Turn off NMEA protocol out.

    roverGNSS.addCfgValset(UBLOX_CFG_I2CINPROT_NMEA,     0);  
    roverGNSS.addCfgValset(UBLOX_CFG_I2COUTPROT_NMEA,    1);
    roverGNSS.addCfgValset(UBLOX_CFG_I2CINPROT_RTCM3X,   0);
    roverGNSS.addCfgValset(UBLOX_CFG_I2COUTPROT_RTCM3X,  0);                           // I2C - Turn off RTCM3 protocol out.

    // roverGNSS.addCfgValset(UBLOX_CFG_UART1_ENABLED,       0);                           // UART1 - Disable.
    roverGNSS.addCfgValset(UBLOX_CFG_UART1INPROT_NMEA,    0);  
    roverGNSS.addCfgValset(UBLOX_CFG_UART1OUTPROT_NMEA,   0);
    roverGNSS.addCfgValset(UBLOX_CFG_UART1INPROT_RTCM3X,  0);  
    roverGNSS.addCfgValset(UBLOX_CFG_UART1OUTPROT_RTCM3X, 0);  

    roverGNSS.addCfgValset(UBLOX_CFG_UART2_BAUDRATE,      57600);                       // UART2 - Increase speed from default 38,400 to 57,600 bps.
    roverGNSS.addCfgValset(UBLOX_CFG_UART2INPROT_NMEA,    0);                           // UART2 - Turn off NMEA protocol out.
    roverGNSS.addCfgValset(UBLOX_CFG_UART2OUTPROT_NMEA,   1);                           // UART2 - Turn on NMEA protocol out.
    roverGNSS.addCfgValset(UBLOX_CFG_UART2INPROT_RTCM3X,  1);                           // UART2 - Turn on RTCM3 protocol out.
    roverGNSS.addCfgValset(UBLOX_CFG_UART2OUTPROT_RTCM3X, 0);                           // UART2 - Turn off RTCM3 protocol out.
    roverGNSS.addCfgValset(UBLOX_CFG_UART2INPROT_UBX,     1);                           // UART2 - Turn on UBX protocol in.
    roverGNSS.addCfgValset(UBLOX_CFG_UART2OUTPROT_UBX,    0);                           // UART2 - Turn off UBX protocol out.
    
    // - GNGGA = PVT, fix quality, SIV, HDOP, ...           https://receiverhelp.trimble.com/alloy-gnss/en-us/NMEA-0183messages_GGA.html.
    // - GPGSV = # Sats visible, sat info, ...              https://receiverhelp.trimble.com/alloy-gnss/en-us/NMEA-0183messages_RMC.html.
    // - GNGSA = PRN # for active sat, PDOP/HDOP/VDOP, ...  https://receiverhelp.trimble.com/alloy-gnss/en-us/NMEA-0183messages_GSA.html.
    // - GNRMC = PVT, ...                                   https://receiverhelp.trimble.com/alloy-gnss/en-us/NMEA-0183messages_GSV.html.
    // - GNGST = Position error statistics, ...             https://receiverhelp.trimble.com/alloy-gnss/en-us/NMEA-0183messages_GST.html.
    // - GNGLL = Position fix & status                      https://receiverhelp.trimble.com/alloy-gnss/en-us/NMEA-0183messages_GLL.html
    // - GNVTG = Tracking.                                  https://receiverhelp.trimble.com/alloy-gnss/en-us/NMEA-0183messages_VTG.html
    roverGNSS.addCfgValset(UBLOX_CFG_MSGOUT_NMEA_ID_GGA_UART2, 1);                      // UART2 - Turn on GGA sentences for SW Maps.
    roverGNSS.addCfgValset(UBLOX_CFG_MSGOUT_NMEA_ID_GSV_UART2, 1);                      // UART2 - Turn on GSV sentences for SW Maps.
    roverGNSS.addCfgValset(UBLOX_CFG_MSGOUT_NMEA_ID_GSA_UART2, 1);                      // UART2 - Turn on GSA sentences for SW Maps.
    roverGNSS.addCfgValset(UBLOX_CFG_MSGOUT_NMEA_ID_RMC_UART2, 1);                      // UART2 - Turn on RMC sentences for SW Maps.
    roverGNSS.addCfgValset(UBLOX_CFG_MSGOUT_NMEA_ID_GST_UART2, 1);                      // UART2 - Turn on GST sentences for SW Maps.
    roverGNSS.addCfgValset(UBLOX_CFG_MSGOUT_NMEA_ID_GLL_UART2, 0);                      // UART2 - Turn off GLL sentences.
    roverGNSS.addCfgValset(UBLOX_CFG_MSGOUT_NMEA_ID_VTG_UART2, 0);                      // UART2 - Turn off VTG sentences.

    roverGNSS.sendCfgValset() ? Serial.println(".") : Serial.println(". Failed.");      // Send the config.
    // - ZDA & GNS sentences are off by default. -

    // -- Not used. --
    // roverGNSS.saveConfiguration();                           // Save current settings to BBR/Flash.
    // roverGNSS.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT);    // Save port settings to flash and BBR.
    // roverGNSS.setNMEAOutputPort(ble);                        // Debug - pipe all NMEA sentences to serial USB.
}

/**
 * ------------------------------------------------
 *      Start tasks.
 * ------------------------------------------------
 *
 * @return void No output is returned.
 * @since  0.1.0 [2025-04-24-12:00pm] New.
 * @since  0.2.0 [2025-04-30-09:30am] Refactored.
 * @since  0.3.5 [2025-05-06-02:30pm] Create BT test task.
 * @since  0.3.8 [2025-05-09-10:30pm] Refactor.
 * @since  0.3.8 [2025-05-14-05:45pm] Refactor.
 * @since  0.4.5 [2025-05-17-11:00am] Refactor.
 * @since  0.4.7 [2025-05-21-07:30pm] Clean up. Add BT LED task.
 * @since  0.6.0 [2025-07-07-04:00pm] Refactor.
 * @see    Global vars: Task handles.
 * @see    checkGnssLockButton(), updateUI().
 * @link   https://www.freertos.org/Documentation/02-Kernel/04-API-references/01-Task-creation/01-xTaskCreate.
 */
void startTasks() {

    // -- Create Tasks. --
    xTaskCreate(radioRtcmLEDtask,    "radio_RTCM_LED_task",      2048, NULL, 2, &radioRtcmLEDtaskHandle);
    xTaskCreate(gnssFixLEDtask,      "GNSS_fix_LED_task",        2048, NULL, 2, &gnssFixLEDtaskHandle);
    xTaskCreate(gnssRTKfloatLEDtask, "GNSS_RTK_float_LED_task",  2048, NULL, 2, &gnssRTKfloatLEDtaskHandle);
    xTaskCreate(gnssRTKfixLEDtask,   "GNSS_RTK_fix_LED_task",    2048, NULL, 2, &gnssRTKfixLEDtaskHandle);
    xTaskCreate(btNmeaLEDtask,       "BT_NMEA_LED_task",         2048, NULL, 2, &btNmeaLEDtaskHandle);

    // -- Suspend tasks. --
    vTaskSuspend(radioRtcmLEDtaskHandle);
    vTaskSuspend(gnssRTKfloatLEDtaskHandle);
    vTaskSuspend(gnssRTKfixLEDtaskHandle);
    vTaskSuspend(gnssFixLEDtaskHandle);
    vTaskSuspend(btNmeaLEDtaskHandle);

    // -- Print status. --
    Serial.println("startTasks() - LED task: Radio RTCM.");
    Serial.println("startTasks() - LED task: GNSS fix, RTK float, RTK fix.");
    Serial.println("startTasks() - LED task: BT NMEA.");
}

/**
 * ------------------------------------------------
 *      Start UI.
 * ------------------------------------------------
 *
 * Last step in setup before loop().
 *
 * @return void No output is returned.
 * @since  0.4.2 [2025-05-15] New.
 * @since  0.4.9 [2025-05-27] Reorganized.
 */
void startUI() {

    // -- Check lock button. --
    checkGnssLockButton();              // Check GNSS lock button.

    // -- Set rover mode. --
    Serial.print("startUI() - Rover mode is ");
    if (UIstate[0] == '1') {        // GNSS lock button is in downPosition.
        pgmState[1] = 'p';          // Set state to RTK PointPerfect.
        Serial.println("PointPerfect.");
    } else {
        pgmState[1] = 'b';          // Set state to RTK base mode.
        Serial.println("base.");
    }
    
    // -- Display OLED startup message. --
    Serial.print("startUI() - Verify OLED");
    updateOLED('0');                // Must be after checkGnssLockButton().
    Serial.println(".");

    // -- Verify LEDs.
    Serial.print("startUI() - Verify LEDs");
    updateLEDs('1','1','1');        // All LEDs on.
    delay(2000);                    // Pause & show startup UI for 2 seconds.
    updateLEDs('0','0','0');        // All LEDs off.
    Serial.println(".");

    // -- Loop. --
    Serial.println("loop() - Running.");

    // -- Display serial (USB) startup message. --
    Serial.printf("\nGhost Rover - Version %c.%c.%c @ %s.\n\n", MAJOR_VERSION, MINOR_VERSION, PATCH_VERSION, BUILD_DATE);
}

// --- Check. ---

/**
 * ------------------------------------------------
 *      Check serial monitor (USB) for input.
 * ------------------------------------------------
 *
 * @return void No output is returned.
 * @since  0.1.0 [2025-04-28-11:00am] New.
 * @since  0.2.0 [2025-04-29-05:30pm] Refactored.
 * @since  0.3.7 [2025-05-09-04:30pm] Add loop() throttle.
 * @since  0.3.8 [2025-05-10-09:30am] Set state.
 * @since  0.3.9 [2025-05-11-10:45am] Refactored "which command".
 * @since  0.4.3 [2025-05-15-05:00pm] Added testLEDb, testLEDg, testLEDr.
 * @since  0.4.7 [2025-05-21-11:30am] Switch Radio & BT LEDs. Cleanup.
 * @since  0.5.2 [2025-06-26-03:45pm] testLEDb - changed bleNMEAoutTaskHandle to btNmeaLEDtaskHandle.
 * @since  0.6.0 [2025-07-07-04:30pm] Refactor.
 * @since  0.6.0 [2025-07-10-09:30am] Refactored monitorCommand.
 */
void checkSerialMonitor(char print = ' ') {

    // -- Local vars. --
    static uint8_t posnMon = 0;         // Persistant input position for USB serial monitor command.
    static uint8_t posnRad = 0;         // Persistant input position for radio test command.
    uint8_t whichMonitorCommand;        // Which command was entered from the USB serial monitor.

    // -- Set state. --
    pgmState[0] = 'l';                  // Program section: in loop().

    // -- Print valid commands. --
    if (print == 'p') {
        Serial.print("\nValid commands: ");
        for (size_t i = 0; i < NUM_COMMANDS-1; i++) {
            if ((i != 0) && (i % 7 == 0)) {
                Serial.println();
            }
            Serial.printf("%s, ", COMMANDS[i]);
        }
        Serial.printf("%s.\n! to quit.\n\n", COMMANDS[NUM_COMMANDS-1]);
        return;                         // Done for now.
    }

    // -- Read serial input. --
    while (Serial.available() > 0) {    // Input from serial monitor.
        monitorChar = Serial.read();    // Read byte in from USB serial.
        if (monitorChar != '\n' && (posnMon < (sizeof(monitorCommand) - 1))) {    // Are we done?
            monitorCommand[posnMon] = monitorChar;      // Not done yet, add char to command.
            posnMon++;                                  // Increment command buffer posn.
        } else {
            monitorCommand[posnMon] = '\0';             // We're done reading, treat command[] as C-string.
            posnMon = 0;                                // Reset command buffer position.

            // - Which command? -
            if (*monitorCommand == EXIT_TEST) {         // Reset debug flags & return.
                debugRad  = false; 
                debugGNSS = false;
                debugNMEA = false;
                debugBtn  = false;
                debugBT   = false;
                debugDis  = false;
                debugPgm  = false;
                debugSer  = false;
                debugUI   = false;
                debugNet  = false;
                Serial.println("All debugging disabled.");
                return;
            }
            whichMonitorCommand = 99;       // Which command was entered. Assume invalid until validated.
                for (size_t i = 0; i < NUM_COMMANDS; i++) {
                if (strcmp(monitorCommand, COMMANDS[i]) == 0) {      // Compare C-strings.
                    whichMonitorCommand = i;
                    break;
                }
            }

            // - Valid command? -
            if(whichMonitorCommand < 99) {   

                // Toggle command flag & print new state.
                switch (whichMonitorCommand) {

                    case (0):
                        testLEDr = (testLEDr == true) ? false : true;       // Flip the state.
                            Serial.printf("%s %s\n", COMMANDS[whichMonitorCommand], (testLEDr  ? "enabled." : "disabled."));
                            break;
                    case 1:
                        testLEDg = (testLEDg == true) ? false : true;       // Flip the state.
                        Serial.printf("%s %s\n", COMMANDS[whichMonitorCommand], (testLEDg  ? "enabled." : "disabled."));
                        break;
                    case 2:
                        testLEDb = (testLEDb == true) ? false : true;       // Flip the state.
                        Serial.printf("%s %s\n", COMMANDS[whichMonitorCommand], (testLEDb  ? "enabled." : "disabled."));
                        break;
                    case 3:
                        testRad = (testRad == true) ? false : true;         // Flip the state.
                        Serial.printf("%s %s\n", COMMANDS[whichMonitorCommand], (testRad  ? "enabled." : "disabled."));
                        break;
                    case 4:
                        debugRad = (debugRad == true) ? false : true;       // Flip the state.
                        Serial.printf("\n%s %s\n", COMMANDS[whichMonitorCommand], (debugRad ? "enabled." : "disabled."));
                        break;
                    case 5:
                        debugGNSS = (debugGNSS == true) ? false : true;     // Flip the state.
                        Serial.printf("%s %s\n", COMMANDS[whichMonitorCommand], (debugGNSS  ? "enabled." : "disabled."));
                        break;
                    case 6:
                        debugNMEA = (debugNMEA == true) ? false : true;     // Flip the state.
                        Serial.printf("%s %s\n", COMMANDS[whichMonitorCommand], (debugNMEA  ? "enabled." : "disabled."));
                        break;
                    case 7:
                        debugBtn = (debugBtn == true) ? false : true;       // Flip the state.
                        Serial.printf("%s %s\n", COMMANDS[whichMonitorCommand], (debugBtn   ? "enabled." : "disabled."));
                        break;
                    case 8:
                        debugBT = (debugBT == true) ? false : true;       // Flip the state.
                        Serial.printf("%s %s\n", COMMANDS[whichMonitorCommand], (debugBT   ? "enabled." : "disabled."));
                        break;
                    case 9:
                        debugDis = (debugDis == true) ? false : true;       // Flip the state.
                        Serial.printf("%s %s\n", COMMANDS[whichMonitorCommand], (debugDis   ? "enabled." : "disabled."));
                        break;
                    case 10:
                        debugPgm = (debugPgm == true) ? false : true;       // Flip the state.
                        Serial.printf("%s %s\n", COMMANDS[whichMonitorCommand], (debugPgm    ? "enabled." : "disabled."));
                        break;
                    case 11:
                        debugSer = (debugSer == true) ? false : true;       // Flip the state.
                        Serial.printf("%s %s\n", COMMANDS[whichMonitorCommand], (debugSer    ? "enabled." : "disabled."));
                        break;
                    case 12:
                        debugUI = (debugUI == true) ? false : true;         // Flip the state.
                        Serial.printf("%s %s\n", COMMANDS[whichMonitorCommand], (debugUI     ? "enabled." : "disabled."));
                        break;
                    case 13:
                        debugNet = (debugNet == true) ? false : true;       // Flip the state.
                        Serial.printf("%s %s\n", COMMANDS[whichMonitorCommand], (debugNet     ? "enabled." : "disabled."));
                        break;
                    case 14:
                        reset = (reset == true) ? false : true;             // Flip the state.
                        Serial.printf("%s %s\n", COMMANDS[whichMonitorCommand], (reset       ? "enabled." : "disabled."));
                        break;
                };
                monitorCommand[0] = '\0';                                   // Ready for the next time.
            } else {

                // Invalid command.
                Serial.printf("\n%s is not a valid command. \n", monitorCommand);

                // Display valid serial Monitor commands.
                checkSerialMonitor('p');
            }

            // - Reset MCU. -
            if (reset) {
                Serial.println("Restarting ...");
                esp_restart();
            }

            // - Test LED. -
            if ((testLEDb) || (testLEDg) || (testLEDr)) {

                // Test mode.
                Serial.print("All LEDs off. ");
                updateLEDs('0','0','0');                        // All LEDs off.
                Serial.println("OLED updated.");
                updateOLED('4');                                // Test mode display.
            }

            // - Test BT LED. -
            if (testLEDb) {

                // Display instructions.
                Serial.printf("Valid options: 0(off), 1(on), 2(active). %c to quit.\n", EXIT_TEST);

                // Suspend BT NMEA out task. 
                vTaskSuspend(btNmeaLEDtaskHandle);

                // Loop.
                while (true) {                                          // Infinite loop.
                    if (Serial.available() > 0) {
                        monitorChar = Serial.read();                    // Read input from serial monitor.
                        Serial.read();                                  // Discard newline.
                        switch (monitorChar) {
                            case EXIT_TEST:                             // All done.
                                Serial.println("testLEDb disabled.");
                                testLEDb = false;                       // Clear test flag.
                                vTaskResume(btNmeaLEDtaskHandle);       // Resume BT NMEA out task.
                                return;                                 // Exit test mode.
                            case '0':                                   // BT LED - off.
                                Serial.printf("%c - BT LED off.\n", monitorChar);
                                updateLEDs('-','-','0');
                                break;
                            case '1':                                   // BT LED - on.
                                Serial.printf("%c - BT LED on.\n", monitorChar);
                                updateLEDs('-','-','1');
                                break;
                            case '2':                                   // BT LED - active.
                                Serial.printf("%c - BT LED active - 5 cycles.\n", monitorChar);
                                for (size_t i = 0; i < 5; i++) {
                                    updateLEDs('-','-','2');
                                    Serial.printf("Blink %i\n", i+1);
                                    delay(1000);
                                }
                                Serial.println();
                                break;
                            default:
                                Serial.printf("Valid options: 0(off), 1(on), 2(active). %c to quit.\n", EXIT_TEST);
                        }
                    }
                }
            }

            // - Test GNSS LED. -
            if (testLEDg) {

                // Display instructions.
                Serial.printf("Valid options: 0(off), 1(on), 2(GNSS fix), 3(RTK float), 4(RTK fix). %c to quit.\n", EXIT_TEST);

                // Loop.
                while (true) {                                              // Infinite loop.
                    if (Serial.available() > 0) {
                        monitorChar = Serial.read();                        // Read input from serial monitor.
                        Serial.read();                                      // Discard newline.
                        switch (monitorChar) {
                            case EXIT_TEST:                                 // All done.
                                Serial.println("testLEDg disabled.");
                                testLEDg = false;                           // Clear test flag.
                                return;                                     // Exit test mode.
                            case '0':                                       // GNSS LED off.
                                Serial.printf("%c - GNSS LED off.\n", monitorChar);
                                updateLEDs('-','0','-');
                                break;
                            case '1':                                       // GNSS LED on.
                                Serial.printf("%c - GNSS LED on.\n", monitorChar);
                                updateLEDs('-','1','-');
                                break;
                            case '2':                                       // GNSS LED GNSS fix.
                                Serial.printf("%c - GNSS LED active (GNSS fix) - 5 cycles.\n", monitorChar);
                                for (size_t i = 0; i < 5; i++) {
                                    updateLEDs('-','2','-');
                                    Serial.printf("Blink %i\n", i+1);
                                    delay(1000);
                                }
                                Serial.println();
                                break;   
                            case '3':                                       // GNSS LED RTK float.
                                Serial.printf("%c - GNSS LED active (RTK float) - 5 cycles.\n", monitorChar);
                                for (size_t i = 0; i < 5; i++) {
                                    updateLEDs('-','3','-');
                                    Serial.printf("Blink %i\n", i+1);
                                    delay(1000);
                                }
                                Serial.println();
                                break;
                            case '4':                                       // GNSS LED RTK fix.
                                Serial.printf("%c - GNSS LED active (GNSS fix) - 5 cycles.\n", monitorChar);
                                for (size_t i = 0; i < 5; i++) {
                                    updateLEDs('-','4','-');
                                    Serial.printf("Blink %i\n", i+1);
                                    delay(1000);
                                }
                                Serial.println();
                                break;
                            default:
                                Serial.printf("Valid options: 0(off), 1(on), 2(GNSS fix), 3(RTK float), 4(RTK fix). %c to quit.\n", EXIT_TEST);
                        }
                    }
                }
            }

            // - Test Radio LED. -
            if (testLEDr) {

                // Display instructions.
                Serial.printf("Valid options: 0(off), 1(on), 2(active). %c to quit.\n", EXIT_TEST);

                // Loop.
                while (true) {                                  // Infinite loop.
                    if (Serial.available() > 0) {
                        monitorChar = Serial.read();            // Read input from serial monitor.
                        Serial.read();                          // Discard newline.
                        switch (monitorChar) {
                            case EXIT_TEST:                     // All done.
                                Serial.println("testLEDr disabled.");
                                testLEDr = false;               // Clear test flag.
                                return;                         // Exit test mode.
                            case '0':                           // Radio LED - off.
                                Serial.printf("%c - Radio LED off.\n", monitorChar);
                                updateLEDs('0','-','-');
                                break;
                            case '1':                           // Radio LED - on.
                                Serial.printf("%c - Radio LED on.\n", monitorChar);
                                updateLEDs('1','-','-');
                                break;
                            case '2':                           // Radio LED - active.
                                Serial.printf("%c - Radio LED active - 5 cycles.\n", monitorChar);
                                for (size_t i = 0; i < 5; i++) {
                                    updateLEDs('2','-','-');
                                    Serial.printf("Blink %i\n", i+1);
                                    delay(1000);
                                }
                                Serial.println();
                                break;
                            default:
                                Serial.printf("Valid options: 0(off), 1(on), 2(active). %c to quit.\n", EXIT_TEST);
                        }
                    }
                }
            }

            // - Test Radio. -
            if (testRad) {

                // HC-12 AT command mode.
                radioCommand[0] = '\0';                     // Reset read buffer.
                posnRad = 0;
                digitalWrite(HC12_SET, LOW);
                updateLEDs('0','-','-');                    // Radio LED off - AT command mode.
                updateOLED('5');                            // Test mode display.

                // Display instructions.
                Serial.println("\nHC-12 command mode enabled (! to exit)");
                Serial.println("  AT, AT+Bxxxx, AT+Cxxx, AT+FUx, AT+Px,");
                Serial.println("  AT+Ry (AT+RB, AT+RC, AT+RF, AT+RP, AT+RX),");
                Serial.println("  (y = B=baudrate, C=channel, F=mode, P=power),");
                Serial.println("  AT+Udps, AT+V, AT+SLEEP, AT+DEFAULT, AT+UPDATE.");
                Serial.println("  https://www.datsi.fi.upm.es/docencia/DMC/HC-12_v2.3A.pdf\n");

                // Loop.
                while (true) {                                              // Infinite loop.
                    if (Serial.available() > 0) {
                        monitorChar = Serial.read();                        // Read input from serial monitor.
                        if(monitorChar == EXIT_TEST) {                      // All done?
                            Serial.println("HC-12 command mode disabled.\n");
                            digitalWrite(HC12_SET, HIGH);
                            Serial.read();                                  // Clear the newline.
                            testRad = false;                                // Clear test flag.
                            return;                                         // Exit test mode.
                        } else {
                            monitorChar = toupper(monitorChar);             // Convert char to upper case.
                        }
                        switch (monitorChar) {
                            case '\n':                                      // Interact with HC-12.
                                serial2.write(radioCommand);            // Write command to HC-12.
                                Serial.println("");
                                delay(200);                                 // Allow HC-12 to process command & respond.
                                while (serial2.available() > 0) {       // Read response from HC-12.
                                    monitorChar = '\0';
                                    monitorChar = serial2.read();
                                    if ((255 != (int) monitorChar) && (posnRad > 0)) {    // Ignore first garbage character.
                                        Serial.print(monitorChar);          // Echo character to serial monitor.
                                    }
                                }
                                radioCommand[0] = '\0';                     // Reset read buffer.
                                posnRad=0;
                                break;
                            default:                                        // Echo & save input character.
                                if (255 != (int) monitorChar) {
                                    Serial.print(monitorChar);
                                    radioCommand[posnRad] = monitorChar;   // Add character to command buffer.
                                    posnRad++;
                                }
                        }
                    }
                }
            }

            // - Other. -
            // @see checkForDebug().
            // @see checkRTCMin().
        }
    }
}

/**
 * ------------------------------------------------
 *      Check GNSS lock button (upPosition or downPosition).
 * ------------------------------------------------
 *
 * @return void No output is returned.
 * @since  0.1.0 [2025-04-24-12:00pm] New.
 * @since  0.3.3 [2025-05-02-08:00am] Refactored.
 * @since  0.3.8 [2025-05-10-09:30am] Set state.
 * @since  0.4.2 [2025-05-15-07:00am] Refactored.
 * @since  0.4.7 [2025-05-21-07:30pm] Switch Radio & BT LEDs.
 * @link   https://roboticsbackend.com/arduino-turn-led-on-and-off-with-button/.
 */
void checkGnssLockButton() {

    // -- Set state of GNSS lock button. --
    if (digitalRead(BUTTON_GNSS_LOCK) == HIGH) {
        UIstate[0] = '0';                   // GNSS lock button is in upPosition.
        if (lastButtonPos == 1) {           // Only true if lock button was in downPosition and now is in upPosition.
            updateLEDs('-','-','2');        // Overide BT LED.
            lastButtonPos = 0;              // Reset lock button position.
        }
    } else {
        updateLEDs('-','-','1');            // Overide BT LED.
        UIstate[0] = '1';                   // GNSS lock button is in downPosition.
        lastButtonPos = 1;                  // Last lock button position.
        ghostMode = true;                   // Flag for checkNMEAin().
    }
}

/**
 * ------------------------------------------------
 *      Check Laser pointer button (up or down).
 * ------------------------------------------------
 *
 * @return void No output is returned.
 * @since  0.6.3 [2025-07-19-08:30am] New.
 * @link https://www.build-electronic-circuits.com/arduino-laser-module-ky-008/.
 */
void checkLaserPointerButton() {

    // -- Set state of GNSS lock button. --
    if (digitalRead(BUTTON_GNSS_LOCK) == HIGH) {
        UIstate[0] = '0';                   // GNSS lock button is in upPosition.
        if (lastButtonPos == 1) {           // Only true if lock button was in downPosition and now is in upPosition.
            updateLEDs('-','-','2');        // Overide BT LED.
            lastButtonPos = 0;              // Reset lock button position.
        }
    } else {
        updateLEDs('-','-','1');            // Overide BT LED.
        UIstate[0] = '1';                   // GNSS lock button is in downPosition.
        lastButtonPos = 1;                  // Last lock button position.
        ghostMode = true;                   // Flag for checkNMEAin().
    }
}

/**
 * ------------------------------------------------
 *      Check for new GNSS data (includes SIV).
 * ------------------------------------------------
 * 
 * -- Uses SparkFun_u-blox_GNSS_v3 library.
 *    - Fix type: 0=none, 1=dead reckoning, 2=2D, 3=3D, 4=GNSS, 5=Time.
 *    - Carrier solution: 0=none, 1=RTK float, 2=RTK fixed.
 *
 * @return void No output is returned.
 * @since  0.1.0 [2025-04-24-12:00pm] New.
 * @since  0.1.0 [2025-04-29-10:45am] Antenna check.
 * @since  0.3.1 [2025-04-30-01:30pm] updateLEDs.
 * @since  0.3.3 [2025-05-02-09:45am] Refactor.
 * @since  0.3.7 [2025-05-09-04:30pm] Add loop() throttle.
 * @since  0.3.8 [2025-05-10-09:30am] Set state.
 * @since  0.3.8 [2025-05-10-09:45am] Moved debug to checkForDebug().
 * @since  0.4.4 [2025-05-15-06:30pm] Changed conditional structure.
 * @since  0.4.7 [2025-05-21-09:30am] Cleanup.
 * @since  0.6.0 [2025-07-09-10:15am] Refactor for low SIV.
 * @link   https://github.com/sparkfun/SparkFun_u-blox_GNSS_v3/blob/main/examples/ZED-F9P/Example10_GetHighPrecisionPositionAndAccuracy/Example10_GetHighPrecisionPositionAndAccuracy.ino
 */
void checkGNSS() {

    if (roverGNSS.getPVT() == true) {                   // New GNSS info is available.

        // -- How many sats? --
        SIV = roverGNSS.getSIV();

        if (SIV < MIN_SATELLITE_THRESHHOLD) {           // Enough satellites?

            // -- Set state. --
            updateLEDs('-','0','-');
            updateOLED('0');                            // Waiting display.
        } else {

            // -- Update these values when GNSS is not locked. --
            if (UIstate[0] == '0') {                                // GNSS lock button is in upPosition (GNSS unlocked).
                    fixType       = roverGNSS.getFixType();
                    solnType      = roverGNSS.getCarrierSolutionType();
                    latitude      = roverGNSS.getLatitude();
                    longitude     = roverGNSS.getLongitude();
                    elevation     = roverGNSS.getAltitudeMSL();     // elevation above Mean Sea Level.
                    horizontalAcc = roverGNSS.getHorizontalAccEst();
                    verticalAcc   = roverGNSS.getVerticalAccEst();
            }

            // -- Update LEDs & OLED displays. --
            if (solnType == 0) {                            // Carrier Solution: 0=GNSS fix.
                updateLEDs('-','2','-');                    // GNSS LED - GNSS fix.
            } else if (solnType == 2) {                     // Carrier Solution: 1=RTK float.
                updateLEDs('-','3','-');                    // GNSS LED - RTK float.
            } else {                                        // Carrier Solution: 2=RTK fixed.
                updateLEDs('-','4','-');                    // GNSS LED - RTK fix.
            }
            updateOLED('1');                                // Normal display.
        }
    }
}

/**
 * ------------------------------------------------
 *      Check RTCM in, relay bytes from serial 2 (HC-12) to serial 0 (RTK-SMA).
 * 
 *        -- ESP32-S3 serial 2 (HC12_TX & HC12_RX) are set to 9,600 bps (HC-12 default speed) in Global Vars.
 *        -- ESP32-S3 serial 0 (RTCM_TX & NMEA_RX) are set to 57,600 bps in Global Vars.
 *           - RTK-SMA (ZED UART2): TX2 (NMEA out) & RX2 (RTCM in) are set to 57,600 bps in startAndConfigGNSS().
 *        -- Check for RTCM preamble (new RTCM sentence). If found, flash radio LED. 
 * ------------------------------------------------
 *
 * @return void No output is returned.
 * @since  0.3.6 [2025-05-07-03:45] New.
 * @since  0.3.7 [2025-05-09-04:30pm] Add loop() throttle.
 * @since  0.4.5 [2025-05-15-05:00pm] Refactor.
 * @since  0.4.7 [2025-05-21-11:45am] Switch Radio & BT LEDs. Cleanup.
 * @since  0.5.1 [2025-06-07-06:45pm] Removed gotbits. Tweked debug.
 * @since  0.6.3 [2025-07-19-09:45am] Tweaked.
 * @see    beginSerial0RTCMtoZED(), beginSerial2RTCMinFromRadio().
 * @link   https://github.com/sparkfun/SparkFun_u-blox_GNSS_v3/blob/main/examples/ZED-F9P/Example3_StartRTCMBase/Example3_StartRTCMBase.ino.
 */
void checkRTCMin() {
    
    // -- Local vars. --
    static uint8_t  preambleCount = 0;
    static uint16_t byteCount     = 0;
    static uint32_t sentCount     = 0;

    // -- Check for Radio down. Set state. --
    if ((esp_timer_get_time()-lastRTCMtime) > RADIO_TIMEOUT) {
        serState[3] = 'd';                      // serial 2 (<- RTCMradio) down.
    }

    // -- Read from serial 2 HC12_TX. --
    while(serial2.available() > 0) {            // serial 2 - data to read?
        serial2Char = serial2.read();           // Read a character @ HC12_SPEED (9,600 bps).
        byteCount++;                            // Increment byte counter (57,600 bps).

        // - Write to serial 0 RTCM_TX (UART0). -
        serial0.write(serial2Char);          // Write a character @ SERIAL0_SPEED.

        // - Update UI. -
        // Look for RTCM preamble = '11010011 000000xx'
        if ((preambleCount == 0) && (serial2Char == 0xd3)) {
            preambleCount++;
        } else if ((preambleCount == 1) && (((int)serial2Char) < 3)) {
            preambleCount++;
        } else {
            if ((debugRad) && (serState[3] == 'u')) {                       // Debug.
                Serial.printf("%02x", serial2Char);                         // Print the byte.
            }
            preambleCount = 0;                                              // Reset preamble counter.
        }
        if (preambleCount == 2) {
            sentCount++;                                                    // Increment the RTCM sentence counter.
            if ((debugRad) && (serState[3] == 'u')) {                       // Debug.
                Serial.printf("\n#%i - %i bytes.\n\nd3%02x", sentCount, byteCount, serial2Char);
            }
            updateLEDs('2','-','-');                                        // Radio LED - active.
            serState[3] = 'u';                                              // serial 2 (<- RTCMradio) up.
            lastRTCMtime = esp_timer_get_time();                            // Used to check for timeout.
            byteCount = 0;                                                  // Reset the byte count.
            preambleCount = 0;                                              // Reset preamble counter.
        }
    }
}

/**
 * ------------------------------------------------
 *      Check NMEA in, relay sentences from serial 0 (RTK-SMA) to serial 1 (ESP32C6) for Bluetooth out.
 *
 *        -- ESP32-S3 serial 0 (RTCM_TX & NMEA_RX) are set to 57,600 bps in Global Vars.
 *           - RTK-SMA (ZED UART2): TX2 (NMEA out) & RX2 (RTCM in) are set to 57,600 bps in startAndConfigGNSS().
 *        -- ESP32-S3 serial 1 (NMEA_TX & NMEA_ACK_RX) are set to 115,200 bps in Global Vars.
 *           - ESP32C6 (NMEA_ACK_TX & NMEA_RX) UART0 are set to 115,200 bps in Global Vars in DougFoster_Ghost_Rover_BT_relay.ino.
 * ------------------------------------------------
 * 
 * Description:
 * -- Read NMEA bytes on serial 0 in from RTK-SMA UART2.
 * -- When enough bytes are received to form a new NMEA sentence:
 *   - If GNSS button is up (normal position):
 *       1) set last $GNGGA sentence = empty,
 *       2) set last $GNRMC sentence = empty,
 *       3) send the new NMEA sentence out serial 1 to ESP32C6.
 *   - If GNSS button is down (GNSS lock is on, aka "Ghost mode"):
 *       1) If the new NMEA sentence type is $GNGGA:
 *            If the last $GNGGA sentence is empty:
 *               set last $GNGGA sentence = new NMEA sentence             
 *            else
 *               set new NMEA sentence = last $GNGGA sentence.
 *       2) If the new NMEA sentence type is $GNRMC:
 *            If the last $GNRMC sentence is empty:
 *               set last $GNRMC sentence = new NMEA sentence             
 *            else
 *               set new NMEA sentence = last $GNRMC sentence..
 *       3) Send the new NMEA sentence out serial 1 to ESP32C6.
 * 
 * -- Gathering a whole sentence is done so that $GNGGA & $GNRMC sentences may be replaced (frozen) when in "Ghost mode".
 *    This will spoof a BT receiver with coordinates from where the button was pushed, not where the receiver is now.
 * 
 * -- Keep in mind the "NMEA in" path (serial 0) relies on a series of "roverGNSS.addCfgValset(UBLOX_CFG_UART2..." settings
 *    executed in startAndConfigGNSS() in setup() over I2C-1 using UBX to configure the RTK-SMA ZED's UART2 interface.
 *    - This includes:
 *      1) setting the baud rate
 *      2) turning off/on protocols
 *      3) turning off/on specific NMEA sentences.
 *
 * -- Keep in mind roverGNSS.getPVT() in checkGNSS() in loop() uses UBX over I2C to retrieve PVT data for the OLED display.
 * 
 * -- Hint: if you use strtok() in a function, then call a subfunction which also uses strtok(), weird things happen. 
 *
 * -- NMEA in:
 *    - serial 0, pin NMEA_RX.
 *    - from RTK-SMA UART2 TX2.
 * -- NMEA out:
 *    - serial 1, pin NMEA_TX.
 *    - to ESP32-C6 running BleSerial.
 *
 * @return void No output is returned.
 * @since  0.6.2 [2025-07-13-02:30pm] New.
 * @since  0.6.3 [2025-07-18-10:00pm] Refactor: added nmeaGetField() & nmea_token_to_decimal().
 * @since  0.6.3 [2025-07-19-09:45am] Tweaked.
 * @see    startAndConfigGNSS() - binds roverGNSS object to I2C-1 fo checkGNSS().
 * @see    checkGNSS() - gets PVT via UBX over I2C-1 using roverGNSS object.
 * @see    nmeaGetField() - parses "lat, "lon", & "elev" from NMEA $GNGGA sentence.
 * @see      - nmea_token_to_decimal() - converts "lat" & "lon" fields from char ddmm.mmmmmmm to double (dd.ddddddddd).
 * @link   https://github.com/doug-foster/DougFoster_Ghost_Rover_BT_relay/.
 * @link   https://receiverhelp.trimble.com/alloy-gnss/en-us/NMEA-0183messages_GGA.html.
 * @link   https://receiverhelp.trimble.com/alloy-gnss/en-us/NMEA-0183messages_RMC.html.
 * @link   https://stackoverflow.com/questions/36054052/is-using-strtok-in-two-functions-safe.
 */

void checkNMEAin() {

    // -- Local vars. --
    static bool      gnssSentenceStarted    = false;                // Flag, forming a sentence.
    static char      gnssSentence[128]      = {'\0'};               // New GNSS sentence.
    static char      lastGNGGAsentence[128] = {'\0'};               // Last $GNGGA sentence.
    static char      lastGNRMCsentence[128] = {'\0'};               // Last $GGRMC sentence.
    static u_int8_t  gnssSentencePosn       = 0;                    // Index position for working sentence.
    static u_int8_t  gnssSentencePosnEnd    = 0;                    // Working sentence end.
    static u_int32_t sentenceCount          = 0;                    // Total # of sentences sent out BT since boot.
           char      lockMessage[15]        = {'\0'};               // Lock message for debug.

    // -- Check for NMEA in from RTK-SMA down. Set state. --
    if ((esp_timer_get_time()-lastNMEAinTime) > NMEA_IN_TIMEOUT) {
        serState[1] = 'd';                      // serial 0 (NMEA <- ZED-F9P) down.
    }

    // -- Read NMEA sentence. --
    while(serial0.available() > 0) {            // serial 0 - data to read?

        serial0Char = serial0.read();           // Read a character @ ZED-F9P default speed.
        lastNMEAinTime = esp_timer_get_time();  // Used to check for timeout.
        serState[1] = 'u';                      // serial 0 (NMEA <- ZED-F9P) up.

        // - Look for start of sentence. -
        if (serial0Char == '$') {                               // First char of a NMEA sentence is always '$'.
            memset(gnssSentence, '\0', sizeof(gnssSentence));   // Clear the sentence buffer.
            gnssSentenceStarted = true;                         // Starting a new sentence.
            gnssSentencePosn    = 0;                            // First character.
            gnssSentencePosnEnd = sizeof(gnssSentence);         // Initial length. Shorten once '*' is read.
        }

        // - If forming a sentence, add the received serial0Char character. - 
        if (gnssSentenceStarted == true) {
            gnssSentence[gnssSentencePosn++] = serial0Char;     // Started a sentence, add the serial0Char char.

            // Is sentence too long, almost done, or done?
            if (gnssSentencePosn == sizeof(gnssSentence)) {     // Sentence too long.
                gnssSentenceStarted = false;                    // Start over.
            } else if (serial0Char == '*') {                    // Almost done. CRC indicator. Length of sentence is now known.
                gnssSentencePosnEnd = gnssSentencePosn + 2;     // Length of sentence = current position + two bytes for CRC.
            } else if (gnssSentencePosn == gnssSentencePosnEnd) {   // Done!

                // Done. Full NMEA sentence.
                gnssSentenceStarted = false;                    // Prepare for a new sentence.
                gnssSentence[gnssSentencePosn+1] = '\0';        // Terminate the sentence (C-string).
                sentenceCount++;                                // Counter for total # of sentences sent out BT since boot.

                // -- GNSS lock button in upPosition? --
                if (UIstate[0] == '0') {

                    // - Clear PVT sentences for next GNSS lock.
                    memset(lastGNGGAsentence, '\0', sizeof(lastGNGGAsentence));
                    memset(lastGNRMCsentence, '\0', sizeof(lastGNRMCsentence));

                    // - Clear lock message. -
                    memset(lockMessage, '\0', sizeof(lockMessage));
                }
                
                // -- GNSS lock button in downPosition (GNSS lock, aka "Ghost mode")? 
                if (UIstate[0] == '1') {

                    // - Set lock message. -
                    memset(lockMessage, '\0', sizeof(lockMessage));
                    strcpy(lockMessage, "GNSS lock: ");

                    // - Was last GNSS sentence PVT type $GNGGA? -
                    if (memcmp(gnssSentence,"$GNGGA",6) == 0) {
                        if (lastGNGGAsentence[0] == 0) {
                            strcpy(lastGNGGAsentence, gnssSentence);    // Save this sentence (locked mode).
                        } else {
                            strcpy(gnssSentence, lastGNGGAsentence);    // Substitute the saved sentence (locked mode).
                        }
                    }
                    // - Was last GNSS sentence PVT type $GNRMC? -
                    if (memcmp(gnssSentence,"$GNRMC",6) == 0) {
                        if (lastGNRMCsentence[0] == 0) {
                            strcpy(lastGNRMCsentence, gnssSentence);    // Save this sentence (locked mode). 
                        } else {
                            strcpy(gnssSentence, lastGNRMCsentence);    // Substitute the saved sentence (locked mode).
                        }
                    }
                }

                // - The NMEA sentence is now ready. Send it to ESP32C6 for Bluetooth out.
                serial1.printf("%s\r\n", gnssSentence);

                // - Debug. -
                if (debugNMEA) {

                    Serial.printf("%sNMEA sentence #%i: %s\n", lockMessage, sentenceCount, gnssSentence);

                    // - Is sentence type $GNGGA? --
                    // Match format ".7f" in checkNMEAin(), updateOLED(), & nmea_token_to_decimal().
                    if (memcmp(gnssSentence,"$GNGGA",6) == 0) {

                        Serial.printf("%s  --> Lat  (NMEA)  : ", lockMessage);
                        nmeaGetField(gnssSentence, "lat");
                        Serial.printf("%s  --> Lat  (get..) : %  13.7f\n", lockMessage, (double)latitude/10000000.0);

                        Serial.printf("%s  --> Lon  (NMEA)  : ");
                        nmeaGetField(gnssSentence, "lon");
                        Serial.printf("%s  --> Lon  (get..) : %  13.7f\n", lockMessage, (double)longitude/10000000.0);
                        
                        Serial.printf("%s  --> Elev (NMEA)  : % +9.3f        (ellipsoidal HAE)\n", lockMessage, nmeaGetField(gnssSentence, "elev"));
                        Serial.printf("%s  --> Elev (get..) : % +9.3f        (orthometric: meters MSL)\n", lockMessage, (double)elevation/1000.0);

                    }
                }
            }
        }
    }
}

/**
 * ------------------------------------------------
 *      Check if Bluetooth on ESP32C6 is up.
 * ------------------------------------------------
 *
 * @return void No output is returned.
 * @since  0.6.1 [2025-07-12-14:45pm] New.
 * @since  0.6.1 [2025-07-13-08:15pm] Refactor.
 * @see    checkSerialMonitor().
 * @link   https://github.com/doug-foster/DougFoster_Ghost_Rover_BT_relay/.
 */
void checkBluetooth() {

    // -- Local vars. --
    static uint8_t posnSer1 = 0;                        // Persistant input position for serial 1 input buffer.
    const  char    MESSAGE[20] = {"ACK-NMEA-BT"};       // ACK sent from ESP32C6.
    
    // -- If NMEA Blutooth on ESP32C6 is down, update states and UI. --
    if ((esp_timer_get_time()-lastNMEAbtACKtime) > NMEA_BT_ACK_TIMEOUT) {
        netState[0] = 'd';                              // NMEA out over BT on ESP32C6 is down.
        serState[2] = 'u';                              // Set state - serial 1 is up.
        updateLEDs('-','-','0');                        // BT LED - inactive.
    }

    // -- Process serial 1. --
    while(serial1.available() > 0) {                    // Serial 1 - data 
        serial1Char = serial1.read();                   // Read a character in from ESP32C6 @ SERIAL1_SPEED
        if (serial1Char != '\n') {                      // Are we done?
            serial1Buffer[posnSer1] = serial1Char;      // Not done yet, add char to buffer. Expecting "ACK-NMEA-BT".
            posnSer1++;                                 // Increment command buffer posn.
        } else {
            serial1Buffer[posnSer1] = '\0';             // We're done reading, treat serial1Buffer[] as C-string.

            if (strncmp(serial1Buffer, MESSAGE, 11) == 0) {     // Compare C-strings to validate ACK.

                // - NMEA Blutooth on ESP32C6 is up, update states and UI.
                netState[0] = 'u';                      // Set state - NMEA out over BT on ESP32C6 is up.
                serState[2] = 'u';                      // Set state - serial 1 is up.
                if (UIstate[0] == '0') {                // GNSS lock button is in upPosition not locked).
                    updateLEDs('-','-','2');            // Flash BT LED - active.
                }
                if (debugBT) {                          // Debug.
                    Serial.printf("%s in from ESP32C\n", serial1Buffer);
                }
                lastNMEAbtACKtime = esp_timer_get_time();       // Reset timeout timer.
            }
            posnSer1 = 0;                               // Reset buffer position.
        }
    }
}

/**
 * ------------------------------------------------
 *      Check to display debug.
 * ------------------------------------------------
 *
 * @return void No output is returned.
 * @since  0.3.3 [2025-05-02-12:00pm] New.
 * @since  0.3.7 [2025-05-09-04:30pm] Add loop() throttle.
 * @since  0.5.1 [2025-06-07-03:45pm] Removed gotbits.
 * @since  0.6.1 [2025-07-13-08:00am] Added debugNMEA.
 * @see    checkSerialMonitor().
 */
void checkForDebug() {

    // -- Local vars. --
    static int64_t   lastThrottleTime = esp_timer_get_time();       // Throttle. Initialize only once, then persist.
           u_int64_t nowTime;
           u_int64_t diffTime;

    // -- Throttle loop() calls. --
    if ((esp_timer_get_time() - lastThrottleTime) > THROTTLE_DEBUG) {
        lastThrottleTime = esp_timer_get_time();                    // Time to run. Reset timer.
    } else {
        return;                                                     // Not time to run.
    }

    // -- Radio. --
    // - Timeout status. Transit bytes are displayed by checkRTCMin(). -
    if (debugRad) {
        nowTime = esp_timer_get_time();
        diffTime = nowTime - lastRTCMtime;
            if (diffTime > RADIO_TIMEOUT) {
                Serial.printf("Radio down. (diffTime= %lld)  >  (timeout= %lld)\n", diffTime, RADIO_TIMEOUT);
            }
    }

     // -- GNSS. --
    if (debugGNSS) {
        // - Display regardless up or down. -
        // roverGNSS.enableDebugging();
        Serial.printf(
        "Lat= %lld(-7)  Lon= %lld(-7)  Alt= %lld(-3)  Hac= %ld(-3)  Vac= %ld(-3)  SIV= %i  Fix= %i  Soln= %i\n",
        latitude, longitude, elevation, horizontalAcc, verticalAcc, SIV, fixType, solnType );
    }

    // -- NMEA. --
    // see "if (debugNMEA)" in checkNMEAin() in loop().

    // -- GNSS lock button. --
    if (debugBtn)  {
        // - GNSS lock button state (0,1). -
        Serial.print("GNSS lock button position = ");
        (UIstate[0] == '0') ? Serial.println("up.") : Serial.println("down.");
    }

    // -- OLED display. --
    if (debugDis) {
        for (size_t i = 0; i < 5; i++) {
            Serial.printf("%s\n",oledRow[i]);
        }
        Serial.println();
    }

    // -- Program state. --
    if (debugPgm) {
        // - Program section (s,l). Rover mode (b,p). -
        Serial.printf("PgmSection= %c  RoverMode= %c\n", pgmState[0], pgmState[1]);     
    }

    // -- Serial state. --
    if (debugSer) {
        // - Serial state (d,u). -
        Serial.printf(
            "USB= %c  serial0= %c  serial1= %c  serial2= %c\n",
            serState[0],        // USB Monitor.
            serState[1],        // serial 0.
            serState[2],        // serial 1.
            serState[3]         // serial 2.
            );
    }

    // -- UI state. --
    if (debugUI) {
        Serial.printf(
            "Button= %c  Radio LED= %c  GNSS LED= %C  BT LED= %C\n",
            UIstate[0],         // GNSS lock button (0,1).
            UIstate[1],         // Radio LED (0,1,2).
            UIstate[2],         // GNSS LED (0,1,2,3,4).
            UIstate[3]          // BT LED (0,1,2). 
        );
    }

    // -- Network state. --
    if (debugNet) {
        // - BT data (d,u). WiFi data (d,u). -
        Serial.printf("BT= %c  WiFi= %c\n", netState[0], netState[1]);
    }
}

// --- Display. ---

/**
 * ------------------------------------------------
 *      Toggle LEDs.
 * ------------------------------------------------
 *
 * @param  char ledR Radio LED.
 * @param  char ledG GNSS LED.
 * @param  char ledB BT LED.
 * @return void No output is returned.
 * @since  0.2.0 [2025-04-30-11:15am] New.
 * @since  0.4.2 [2025-05-15-08:00am] Refactor.
 * @since  0.4.7 [2025-05-21-12:45pm] Switch Radio & BT LEDs.
 * @since  0.6.0 [2025-07-07-04:45pm] Tweaked.
 * @link   https://www.freertos.org/Documentation/02-Kernel/04-API-references/02-Task-control/06-vTaskSuspend.
 * @link   https://www.freertos.org/Documentation/02-Kernel/04-API-references/02-Task-control/07-vTaskResume.
 */
void updateLEDs(char ledR, char ledG, char ledB) {

    // --- Radio LED. ---
    switch (ledR) {
        case '0':
            UIstate[1] = '0';                           // Radio LED - off.
            digitalWrite(LED_RADIO, LOW);               // LED off.
            break;
        case '1':
            UIstate[1] = '1';                           // Radio LED - on.
            digitalWrite(LED_RADIO, HIGH);              // LED on.
            break;
        case '2':
            UIstate[1] = '2';                           // Radio LED - active.
            vTaskResume(radioRtcmLEDtaskHandle);        // Resume task.
            break;
    }


    // --- GNSS LED. ---
    switch (ledG) {
        case '0':
            UIstate[2] = '0';                           // GNSS LED - off.
            digitalWrite(LED_GNSS, LOW);                // LED off.
            break;
        case '1':
            UIstate[2] = '1';                           // GNSS LED - on.
            digitalWrite(LED_GNSS, HIGH);               // LED on.
            break;
        case '2':
            UIstate[2] = '2';                           // GNSS LED - active (GNSS fix).
            vTaskResume(gnssFixLEDtaskHandle);          // Resume task.
            break;
        case '3':
            UIstate[2] = '3';                           // GNSS LED - active (RTK float).
            vTaskResume(gnssRTKfloatLEDtaskHandle);     // Resume task.
            break;
        case '4':
            UIstate[2] = '4';                           // GNSS LED - active (RTK fix).
            vTaskResume(gnssRTKfixLEDtaskHandle);       // Resume task.
            break;
    }

    // --- BT LED. ---
    switch (ledB) {
        case '0':
            UIstate[3] = '0';                           // BT LED - off.
            digitalWrite(LED_BT, LOW);                  // LED off.
            break;
        case '1':
            UIstate[3] = '1';                           // BT LED - on.
            digitalWrite(LED_BT, HIGH);                 // LED on.
            break;
        case '2':
            UIstate[3] = '2';                           // BT LED - active.
            vTaskResume(btNmeaLEDtaskHandle);           // Resume task.
            break;
    }
}

/**
 * ------------------------------------------------
 *      Display OLED.
 * ------------------------------------------------
 * 
 * -- Uses SparkFun_Qwiic_OLED library.
 *    - 8x16 font size yields (5) rows with (16) characters each.
 * -- oledRow[5][17] is defined [17] to include null terminator.
 *    - Assumptions:
 *      1. OLED object (roverOLED) has been created using Qwiic1in3OLED.
 *      2. I2C (Wire) has been started.
 *      3. OLED object (roverOLED) has been bound to I2C (Wire).
 *      4. OLED object (roverOLED) has font set.
 *
 * @param  u_int8_t display Which display format to use.
 * @return void No output is returned.
 * @since  0.1.0 [2025-04-24-12:00pm] New.
 * @since  0.2.0 [2025-04-29-11:00am] Refactor.
 * @since  0.2.0 [2025-04-30-11:43am] Add roverMode.
 * @since  0.3.7 [2025-05-09-08:45pm] Add loop() throttle.
 * @since  0.4.2 [2025-05-15-09:00am] Refactor.
 * @since  0.6.0 [2025-07-07-04:15pm] Change: wifiState, btState.
 * @since  0.6.1 [2025-07-13-08:30am] Change display from "Alt" to "MSL".
 * @since  0.6.1 [2025-07-13-03:00pm] Check for MIN_SATELLITE_THRESHHOLD.
 * @see    Include libraries: qw_fnt_8x16.
 * @see    Global vars: OLED display.
 * @see    beginI2C(), startDisplay().
 * @link   https://docs.sparkfun.com/SparkFun_Qwiic_OLED_1.3in/introduction//
 */

void updateOLED(char display) {

    // -- Local vars. --
    bool displayNow = false;

    // -- Setup() display. --
    switch (display) {
        case '0':       // - Startup. -
            sprintf(oledRow[0], "Version  %c.%c.%c", MAJOR_VERSION, MINOR_VERSION, PATCH_VERSION);
            strcpy(oledRow[1], BUILD_DATE);
            strcpy(oledRow[2], "D. Foster");
            if (pgmState[1] == 'b') {
                strcpy(oledRow[3], "RTK: Base");
            } else {
                strcpy(oledRow[3], "RTK: PP");
            };
            if (SIV < MIN_SATELLITE_THRESHHOLD) {           // Enough satellites?
                sprintf(oledRow[4], "Sats: %i of %i ...", SIV, MIN_SATELLITE_THRESHHOLD);
            } else {
                strcpy(oledRow[4], "Waiting ...");
            }
            displayNow = true;
            break;
        case '2':
        case '3':       // - Diagnostic. -
            strcpy(oledRow[0], "0123456789abcdef");
            strcpy(oledRow[1], "0123456789abcdef");
            strcpy(oledRow[2], "0123456789abcdef");
            strcpy(oledRow[3], "0123456789abcdef");
            strcpy(oledRow[4], "0123456789abcdef");
            displayNow = true;
            break;
        case '4':       // - LED test mode. -
            strcpy(oledRow[0], "LED test mode.  ");
            strcpy(oledRow[1], "                ");
            strcpy(oledRow[2], "                ");
            strcpy(oledRow[3], "                ");
            strcpy(oledRow[4], "                ");
            displayNow = true;
            break;
        case '5':       // - Radio test mode. -
            strcpy(oledRow[0], "Radio test mode.");
            strcpy(oledRow[1], "                ");
            strcpy(oledRow[2], "                ");
            strcpy(oledRow[3], "                ");
            strcpy(oledRow[4], "                ");
            displayNow = true;
            break;
    }

    // -- Display now? ---
    if (displayNow) { 
        roverOLED.erase();                              // Erase display.
        for (size_t i = 0; i < 5; i++) {                // Set text.
            roverOLED.text(0, ROW[i], oledRow[i]);      // Each row starts at char 0.
        }
        roverOLED.display();
        return;                                         // Back to work.
    }

    // -- Throttle loop() calls. --
    static int64_t   lastThrottleTime = esp_timer_get_time();       // Throttle. Initialize only once, then persist.
    if ((esp_timer_get_time() - lastThrottleTime) > THROTTLE_OLED) {
        lastThrottleTime = esp_timer_get_time();                    // Time to run. Reset timer.
    } else {
        return;                                                     // Not time to run.
    };

    // -- Loop() display. --
    // Match format ".7f" in checkNMEAin(), updateOLED(), & nmea_token_to_decimal().
    switch (display) {
        case '1':       // - Normal. -

            // 1st row - latitude.
            if ( latitude > 0 )  {
                sprintf(oledRow[0], "Lat   %.7f", double( latitude ) / 10000000.0);  // Extra space.
            } else {
                sprintf(oledRow[0], "Lat  %.7f", double( latitude ) / 10000000.0);
            }
            
            // 2nd row - longitude.
            if ((longitude > 99 ) || (longitude < -99)) {
                sprintf(oledRow[1], "Lon  %.7f", double( longitude ) / 10000000.0);  // Extra space.
            } else {
                sprintf(oledRow[1], "Lon %.7f", double( longitude ) / 10000000.0);
            }

            // 3rd row - elevation.
            sprintf(oledRow[2], "MSL   %.2f m", double( elevation ) / 1000.0);

            // 4th row - accuracy.
            float hAc, vAc;
            if (float(horizontalAcc)/1000.0 > 10.0) {
                hAc = 9.99;
            } else {
                hAc = float(horizontalAcc)/1000.0;
            }
            if (float( verticalAcc)/1000.0 > 10.0) {
                vAc = 9.99;
            } else {
                vAc = float( verticalAcc)/1000.0;
            }
            sprintf(oledRow[3], "HVa  %.2f/%.2f m", hAc, vAc);

            // 5th row - misc state.
            if (netState[0] == 'u') {                           // BT data state.
                btState = '+';                                  // Displayed in 5th row.
            } else {
                btState = '-';                                  // Displayed in 5th row.
            } 
            if (netState[1] == 'u') {                           // WiFi data state.
                wifiState = '+';                                // Displayed in 5th row.
            } else {
                wifiState = '-';                                // Displayed in 5th row.
            }
            sprintf(oledRow[4], "S:%2i F%1i S%1i B%c W%c",
                SIV, fixType, solnType, btState, wifiState);
            break;
    }

    // -- Display. ---
    roverOLED.erase();                                          // Erase display.
    for (size_t i = 0; i < 5; i++) {                            // Set text.
        roverOLED.text(0, ROW[i], oledRow[i]);                  // Each row starts at char 0.
    }
    roverOLED.display();
}

// --- Callback. ---

// --- Operation. ---

/**
 * ------------------------------------------------
 *      Convert field value from ddmm.mmmmmmm char array to dd.mmmmmmmmm double.
 *
 *      -- Example: "3536.3638559" -> 35.363638559.
 * ------------------------------------------------
 *
 * @param  char* field Field value.
 * @param  char* dir Direction value.
 * @return double value Field value.
 * @since  0.6.3 [2025-07-19-11:15am] New.
 * @see    nmeaGetField() - retrieves field (tkn) from NMEA $GNGGA.
 */
double nmea_token_to_decimal(char* field, char* dir) {

    // e.g. tkn = 3536.3638559 .

    // Local vars.
    char   wholeDegrees[4]  = {'\0'};
    char   minutes[15]      = {'\0'};
    double degrees          = 0;
    double decDegrees       = 0;
    double value            = 0;
  
    // Tokens.
    char *token1 = strtok(field,".");                   // First token, e.g. 3536.
    char *token2 = strtok(NULL,".");                    // Next token, e.g. 3638559.

    // Degrees.
    memcpy(wholeDegrees, token1, strlen(token1)-2);     // Whole degrees: e.g. 35 .
    
    // Minutes.
    memcpy(&minutes[0], &token1[strlen(token1)-2], 2);
    strcat(minutes, ".");
    strcat(minutes, token2);

    // Convert.
    degrees = atof(wholeDegrees);                       // Convert to double.
    decDegrees = atof(minutes)/60.0;                    // Convert minutes to decimal degrees.
    value = degrees + decDegrees;

    if ((dir[0] == 'S') || (dir[0] == 'W')) {           // Negative value?
        value *= -1.0;
    }

    // Debug.
    // Match format ".7f" in checkNMEAin(), updateOLED(), & nmea_token_to_decimal().
    Serial.printf("%  13.7f = %.0f dd + %s mm (%.9f dd) \n", value, degrees, minutes, decDegrees);

    return (value);
}

/**
 * ------------------------------------------------
 *      Return latitude, longitude, or elevation field from NMEA $GNGGA sentence as int_64.
 * ------------------------------------------------
 *
 * @param  char* sentence GNSS sentence.
 * @param  const char* field Which field to get: "lat, "lon", or "ele."
 * @return double Value of field.
 * @since  0.6.3 [2025-07-19-11:15am] New.
 * @link.  https://receiverhelp.trimble.com/alloy-gnss/en-us/NMEA-0183messages_GGA.html.
 * @link   https://www.tutorialspoint.com/c_standard_library/c_function_strtok.htm.
 */
double nmeaGetField(char* sentence, const char* field) {

     // Local vars.
           char    type[6]          = {'\0'};
           char    nmeaLat[16]      = {'\0'};
           char    nmeaLatDir[2]    = {'\0'};
           char    nmeaLon[16]      = {'\0'};
           char    nmeaLonDir[2]    = {'\0'};   
           char    nmeaElev[16]     = {'\0'};   
          uint8_t  length           = 0;                   // Size of sentence.
          uint8_t  fieldNum         = 0;
          uint8_t  posn             = 0;  
          double   value            = 0;                   // Field value. 

    // Parse the sentence.
    if (memcmp(sentence,"$GNGGA",6) == 0) {     // Check sentence type.
        
        // Parse fields.
        length = strlen(sentence);
        for (size_t i = 0; i < length; i++) {
            if (sentence[i] == ',') {
                posn = 0;
                fieldNum++;
            } else {
                if (fieldNum == 0) {
                    type[posn] = sentence[i];
                }
                if (fieldNum == 2) {
                    nmeaLat[posn] = sentence[i];
                }
                if (fieldNum == 3) {
                    nmeaLatDir[posn] = sentence[i];
                }
                if (fieldNum == 4) {
                    nmeaLon[posn] = sentence[i];
                }
                if (fieldNum == 5) {
                    nmeaLonDir[posn] = sentence[i];
                }
                if (fieldNum == 9) {
                    nmeaElev[posn] = sentence[i];
                }
                posn++;
            }
        }

        // Calculate lat, long, & elev values.
        if (strcmp(field, "lat") == 0) {
            value = nmea_token_to_decimal(nmeaLat, nmeaLatDir);     // Set the value.
        }
        if (strcmp(field, "lon") == 0) {
            value = nmea_token_to_decimal(nmeaLon, nmeaLonDir);     // Set the value.
        }
        if (strcmp(field, "elev") == 0) {
            value = atof(nmeaElev);                     // Set the value.
        }
    }

    return value;
}

// --- Tasks. ---

/**
 * ------------------------------------------------
 *      BT LED - active task.
 * ------------------------------------------------
 *
 * @param  void * pvParameters Pointer to task parameters.
 * @return void No output is returned.
 * @since  0.4.7 [2025-05-21-01:15pm] New.
 * @since  0.4.7 [2025-05-21-06:00pm] Refactor. Duh.
 * @see    startTasks().
 * @link   https://www.freertos.org/Documentation/02-Kernel/04-API-references/02-Task-control/06-vTaskSuspend.
 */
void btNmeaLEDtask(void * pvParameters) {
    while(true) {
        digitalWrite(LED_BT, HIGH);                 // LED on.
        vTaskDelay(LED_TIME_FLASH_ON);              // LED remains on.
        digitalWrite(LED_BT, LOW);                  // LED off.
        vTaskSuspend(NULL);                         // Suspend task.
    }
}

/**
 * ------------------------------------------------
 *      GNSS LED - flash effect.
 * ------------------------------------------------
 *
 * @param  array $*list List of items.
 * @return void No output is returned.
 * @since  0.4.2 [2025-05-17-02:15pm].
 * @since  0.4.7 [2025-05-21-06:00pm].
 * @see    startTasks().
 * @see    btNmeaLEDtask().
 * @see    gnssFixLEDtask().
 * @see    gnssRTKfloatLEDtask().
 * @see    gnssRTKfixLEDtask().
 * @link   https://www.freertos.org/Documentation/02-Kernel/04-API-references/02-Task-control/01-vTaskDelay.
 * @link   https://docs.espressif.com/projects/esp-idf/en/v4.3/esp32/api-reference/system/freertos.html.
 * @link   https://www.freertos.org/Documentation/02-Kernel/05-RTOS-implementation-tutorial/02-Building-blocks/11-Tick-Resolution.
 */
void cycleGNSSLed() {
    digitalWrite(LED_GNSS, HIGH);                   // LED on.
    vTaskDelay(LED_TIME_FLASH_ON);                  // LED remains on (ms).
    digitalWrite(LED_GNSS, LOW);                    // LED off.
    vTaskDelay(LED_TIME_FLASH_OFF);                 // LED remains off (ms).
}

/**
 * ------------------------------------------------
 *      GNSS LED - active (GNSS fix) task.
 * ------------------------------------------------
 *
 * @param  void * pvParameters Pointer to task parameters.
 * @return void No output is returned.
 * @since  0.3.3 [2025-05-14-06:00pm] New.
 * @since  0.4.5 [2025-05-17-11:30am] Refactor.
 * @see    cycleGNSSLed().
 * @see    startTasks().
 * @link   https://www.freertos.org/Documentation/02-Kernel/04-API-references/02-Task-control/06-vTaskSuspend.
 */
void gnssFixLEDtask(void * pvParameters) {
    while(true) {                                   // Infinite loop.
        for (size_t i = 0; i < 1; i++) {            // Repeat one time.
            cycleGNSSLed();
        }
        vTaskSuspend(NULL);                         // Suspend task.
    }
}

/**
 * ------------------------------------------------
 *      GNSS LED - active (RTK float) task.
 * ------------------------------------------------
 *
 * @param  void * pvParameters Pointer to task parameters.
 * @return void No output is returned.
 * @since  0.3.3 [2025-05-14-06:30pm] New.
 * @since  0.4.5 [2025-05-17-11:30am] Refactor.
 * @see    startTasks().
 * @see    cycleGNSSLed().
 * @link   https://www.freertos.org/Documentation/02-Kernel/04-API-references/02-Task-control/06-vTaskSuspend.
 */
void gnssRTKfloatLEDtask(void * pvParameters) {
    while(true) {                                   // Infinite loop.
        for (size_t i = 0; i < 2; i++) {            // Repeat two times.
            cycleGNSSLed();
        }
        vTaskSuspend(NULL);                         // Suspend task.
    }
}

/**
 * ------------------------------------------------
 *      GNSS LED - active (RTK fix) task.
 * ------------------------------------------------
 *
 * @param void * pvParameters Pointer to task parameters.
 * @return void No output is returned.
 * @since  0.3.3 [2025-05-14-06:30pm] New.
 * @since  0.4.5 [2025-05-17-11:30am] Refactor.
 * @see    startTasks().
 * @see    cycleGNSSLed().
 * @link   https://www.freertos.org/Documentation/02-Kernel/04-API-references/02-Task-control/06-vTaskSuspend.
 */
void gnssRTKfixLEDtask(void * pvParameters) {
    while(true) {                                   // Infinite loop.
        for (size_t i = 0; i < 3; i++) {            // Repeat three times.
            cycleGNSSLed();
        }
        vTaskSuspend(NULL);                         // Suspend task.
    }
}

/**
 * ------------------------------------------------
 *      Radio LED - active task.
 * ------------------------------------------------
 *
 * @param  void * pvParameters Pointer to task parameters.
 * @return void No output is returned.
 * @since  0.3.3 [2025-05-02-09:00am] New.
 * @since  0.4.5 [2025-05-17-11:15am] Refactor.
 * @since  0.4.7 [2025-05-21-03:00pm] Refactor. Duh.
 * @see    startTasks().
 * @link   https://docs.espressif.com/projects/esp-idf/en/v4.3/esp32/api-reference/system/freertos.html.
 * @link   https://www.freertos.org/Documentation/02-Kernel/04-API-references/02-Task-control/06-vTaskSuspend.
 */
void radioRtcmLEDtask(void * pvParameters) {
    while(true) {
        digitalWrite(LED_RADIO, HIGH);                      // LED on.
        vTaskDelay(LED_TIME_FLASH_ON);                      // LED remains on (ms).
        digitalWrite(LED_RADIO, LOW);                       // LED off.
        vTaskSuspend(NULL);                                 // Suspend task.
    }
}

// --- Test. ---

// ===================================
//               Setup.
// ===================================

void setup() {
    beginSerialMonitor();           // Begin serial monitor (USB).
    initVars();                     // Initialize global vars.
    configPins();                   // Initialize pin modes & pin values.
    beginSerialInterfaces();        // Begin serial interfaces. 
    beginI2C();                     // Begin I2C interfaces.
    startDisplay();                 // Start OLED display.
    startWiFi();                    // Begin & start WiFi interface.
    startAndConfigGNSS();           // Start GNSS object, config ZED settings.
    startTasks();                   // Start tasks.
    startUI();                      // Start UI.
}                                               

// =================================== 
//               Loop.
// ===================================

void loop() {
    checkSerialMonitor();                   // Check serial monitor (USB) for input.
    checkGnssLockButton();                  // Check GNSS lock button.
    checkLaserPointerButton();              // Check Laser pointer button.
    checkGNSS();                            // Check for new GNSS data (includes SIV).
    if (SIV > MIN_SATELLITE_THRESHHOLD) {   // Enough satellites?
        checkRTCMin();                      // Check RTCM in, relay bytes from serial 2 (HC-12) to serial 0 (RTK-SMA).
        checkNMEAin();                      // Check NMEA in, relay sentences from serial 0 (RTK-SMA) to serial 1 (ESP32C6) for Bluetooth out.
        checkBluetooth();                   // Check for timely "ACK-NMEA-BT" returned from ESP32C6.
    }
    checkForDebug();                        // Check to display debug.
}
