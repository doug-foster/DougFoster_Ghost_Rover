/**
 * Ghost Rover.
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
 * @link     http://dougfoster.me.
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
 *        without the rover being visible in the photo. This is done by 1) positioning the rover above
 *        the subject and obtaining a fix, 2) pressing the GNSS lock button, 3) moving the rover out of view
 *        for the photograph, 4) taking the photo, 5) releasing the GNSS lock button, then 6) moving on to
 *        the next subject. While the GNSS lock button is down, the rover will continue to broadcast the
 *        position of where the button was pressed. When the button is released, it returns to normal roving
 *        operation.
 * 
 *        A 16 character x 5 row OLED display on the rover's front is used to display GNSS information
 *        (latitude, longitude, altitude above Mean Sea Level, horizontal & vertical accuracy, satellites in
 *        view, GNSS fix type, GNSS solution type, and network status for both bluetooth & WiFi communications).
 * 
 *        The front of the rover has a push-to-latch button (the GNSS lock button described above), a blue
 *        LED (flashes to indicate a Bluetooth Low energy - BLE broadcast), a yellow LED (flashes to indicates
 *        GNSS activity), and a red LED (flashes to indicate reception of data from an RF link). The rover's
 *        case also has an off/on switch to control +5v power in from a small USB portable battery pack. A
 *        removable UHF external antenna for the RF link is attached to the case, and the USB serial ports for
 *        both MCU & GNSS boards are accessible. The rover is powered from either 1) the battery pack or 2) the
 *        MCU or GNSS USB serial port.
 * 
 *        While the rover displays GNSS location status on the OLED for user convience, its main purpose is to
 *        broadcast NMEA sentences (GGA & RMC for position & time, GSA & GSV for skyplot, and GST for accuracy)
 *        out over BLE to a device (like a mobile device running SW Maps) which can take photos and geotag them
 *        using data from an external GNSS source. Using the SparkFun_u-blox_GNSS_v3 library, the GNSS processor
 *        (in this case a uBlox ZED-F9P) is set to broadcast a solution of these (5) sentences once every
 *        second.
 * 
 *        The GNSS processor operates in high precision mode. This means all position information (both on 
 *        the OLED display and in the NMEA sentences over BLE) are reported with 7 decimal position precision
 *        rather than the standard 5 decimal precision.
 * 
 *        The rover uses an RF link (internal HC-12 radio) to receive RTCM3 correction data from a base station.
 *        (A roadmap feature is to support stand-alone mode using PointPerfect correction data received from
 *        a WiFi connected hot spot. This is initiated by booting the rover with the GNSS lock button in the
 *        downPosition on start up as opposed to the normal upPosition). The rover's companion base station
 *        is a Sparkfun EVK running Sparkfun's RTK everywhere firmware. Internally, another ESP32-S3 and HC-12
 *        were added to the EVK to create a serial relay for transmitting RTCM bytes streamed out from the
 *        UART2 port of the base's GNSS processor an in to the rover's HC-12 radio.
 * 
 *        WHen the rover boots, it will first progress into 3D GNSS fix. This is indicated by the status line
 *        on the OLED display (F3 S0). Also, the yellow LED fashes once every second. When the rover is
 *        receiveing RTCM data, it will enter RTK float mode (displaying F3 S1) and present two quick flashes
 *        every second. When the rover finally enters RTK fix mode, it will display (F3 S2) and the yellow LED
 *        will quickly flash three times every second. When/if the rover's fix acuracy digresses from RTK, the
 *        display and LEDs will perform as described in the progesssion to RTK fix.
 * 
 *        If no BLE transmission occurs, or no GNSS update is received, or no RTCM data received over the RF
 *        link from the base, the LEDs will correspondingly be off. When data is received again, the LEDs will
 *        begin flashing their status. At startup, the OLED display may show no location data (all 0's) until
 *        enough satellites have been acquired to obtain a valid GNSS solution (aka fix). Ghost rover configures
 *        the GNSS settings both in RAM and battery backed-up RAM (BBR). The battery is trickle charged by the
 *        Sparkfun GNSS board and (if not needing to be replaced), should hold the GNSS settings as well as last
 *        fix information for about 2 weeks. This can significantly reduce time to first fix for the GNSS.
 * 
 *        Internally, the rover uses a serial connection between the ESP32-S3 MCU and the ZED-F9P (UART1) for
 *        1) configuring the GNSS via UBX commands and 2) receiving NMEA sentences from the ZED. The I2C bus
 *        is used only for communication between the ESP32 and the OLED display.
 * 
 *        A set of serial commands (entered over the serial USB interface of the ESP32-S3) are provided for
 *        testing and debugging ooperation. checkSerialUSB() in the loop() section watches for a command.
 *        To see a list of available commands, just type some garbarge - like "dfads" - into the Serial Monitor 
 *        command line of the Arduino IDE. The response will be a list of case sensitive, valid commands.
 * 
 *        "testRad" is a serial monitor test/debug command which can be used to place the rover into test
 *        mode with the HC-12 radio. As descibed by the sentence when testRad mode is initiated, the HC-12
 *        uses AT type command for configuration (e.g. type "AT" and you should receive an "OK" response back
 *        from the radio). To exit any of the test/debug modes, just type a '!' character. 
 *
 * --- Major components. ---
 *     -- rover MCU      https://www.sparkfun.com/sparkfun-thing-plus-esp32-s3.html.
 *     -- rover GNSS     https://www.sparkfun.com/sparkfun-gps-rtk-sma-breakout-zed-f9p-qwiic.html.
 *     -- rover display  https://www.sparkfun.com/sparkfun-qwiic-oled-1-3in-128x64.html.
 *     -- base station   https://www.sparkfun.com/sparkfun-rtk-evk.html.
 *
 * --- Other components. ---
 *     -- GNSS antenna. --
 *        - antenna (L1/L2/L5, TNC-F): https://www.sparkfun.com/gnss-multi-band-l1-l2-l5-surveying-antenna-tnc-spk6618h.html.
 *        - adapter (TNC-M to SMA-M): https://www.amazon.com/dp/B0BGPJP3J3.
 *        - cable (SMA-F to SMA-M, 12" RG316): https://www.amazon.com/dp/B07MJQWH8S.
 *     -- Radio. --
 *        - radio (433.4-473.0 MHz, 100mW, U.FL): https://www.amazon.com/HiLetgo-Wireless-Replace-Bluetooth-Antenna/dp/B01MYTE1XR.
 *        - antenna (UHF 400-960 MHz, BNC-M): https://www.amazon.com/dp/B07R4PGZK3.
 *        - cable (BNC-F bulkhead to U.FL, 8" RG178): https://www.amazon.com/dp/B098HX6NFH.
 *     -- Misc. --
 *        - push button (12mm latching): https://www.amazon.com/dp/B0CGTXMLKL.
 *        - power switch (15mm on/off latching toggle): https://www.amazon.com/dp/B09XMDXKTR.
 *        - LED covers (5mm LED bulb socket): https://www.amazon.com/dp/B07CQ6TH14.
 *        - battery (+5V 2.4A max, 8000 mAh): https://www.walmart.com/ip/onn-8000mAh-Portable-Battery-Power-Bank-with-USB-A-to-C-Charging-Cable-LED-Indicator-Black/5266111773
 *        - enclosure: https://www.sparkfun.com/hand-held-enclosure-abs-plastic.html.
 *        - handle: https://www.amazon.com/dp/B01FUEXLGU.
 *        - tripod base: https://www.amazon.com/dp/B07GST1C2Z.
 *        - adjustable phone mount: https://www.amazon.com/dp/B07S8TTH34.
 *        - other: 80/20, 1/4-20 nuts, bots, washers, USB-A power cable.
 *
 * --- References. ---
 *     -- RTK   https://learn.sparkfun.com/tutorials/how-to-build-a-diy-gnss-reference-station/all.
 *     -- EVK   https://docs.sparkfun.com/SparkFun_RTK_EVK/introduction/.
 *     -- HC-12 https://www.elecrow.com/download/HC-12.pdf.
 *
 * --- Code flow. ---
 *     -- Include libraries.
 *     -- Global vars: define vars, set constants, prototypes.
 *     -- Functions: init, config, begin, start, check, display, callback, operation, tasks, test.
 *     -- Setup.
 *     -- Loop.
 * 
 * --- TODO: ---
 *     1. checkRadioRTCMToZED() - Implement out to serialRTCM, move to ESP32 task.
 */

// ===================================
//      Include libraries.
// ===================================

// --- Libraries. ---
#include <Wire.h>                       // https://github.com/espressif/arduino-esp32/blob/master/libraries/Wire/src/Wire.h 
#include <HardwareSerial.h>             // https://github.com/espressif/arduino-esp32/blob/master/cores/esp32/HardwareSerial.cpp.
#include <esp_system.h>                 // https://github.com/pycom/esp-idf-2.0/blob/master/components/esp32/include/esp_system.h.
#include <SparkFun_u-blox_GNSS_v3.h>    // https://github.com/sparkfun/SparkFun_u-blox_GNSS_v3 (3.1.9).
#include <SparkFun_Qwiic_OLED.h>        // https://github.com/sparkfun/SparkFun_Qwiic_OLED_Arduino_Library (1.0.13).
#include <res/qw_fnt_8x16.h>            // https://github.com/sparkfun/SparkFun_Qwiic_OLED_Arduino_Library/blob/main/src/res/qw_fnt_8x16.h.
#include <BleSerial.h>                  // https://github.com/avinabmalla/ESP32_BleSerial (2.0.1).

// ===================================
//      Global vars.
// ===================================

// -- Version. --
const char BUILD_DATE[]   = "2025-05-28-17:45";     // 24hr format, need to fit max (16) characters.
const char MAJOR_VERSION  = '0';
const char MINOR_VERSION  = '5';
const char PATCH_VERSION  = '0';

// -- Pin (pth) definitions. --
const uint8_t BUTTON_LOCK = 4;          // ESP32-S3 Thing+ PTH 4 <-> Red toggle button (yellow wire).
const uint8_t LED_BLE     = 6;          // ESP32-S3 Thing+ PTH 6 <-> Blue LED (blue wire).
const uint8_t LED_GNSS    = 5;          // ESP32-S3 Thing+ PTH 5 <-> Yellow LED (green wire).
const uint8_t LED_RADIO   = 7;          // ESP32-S3 Thing+ PTH 7 <-> Red LED (white wire).
const uint8_t I2C_SDA     = 8;          // ESP32-S3 Thing+ PTH SDA/8 - I2C data pin.
const uint8_t I2C_SCL     = 9;          // ESP32-S3 Thing+ PTH SCL/9 - I2C clock pin.
const uint8_t PTH_SET     = 42;         // ESP32-S3 Thing+ PTH 42 (SET) <-> HC-12 SET (blue wire).
const uint8_t PTH_TX      = 43;         // ESP32-S3 Thing+ PTH 43 (UART2) <-> HC-12 RX (yellow wire).
const uint8_t PTH_RX      = 44;         // ESP32-S3 Thing+ PTH 44 (UART2) <-> HC-12 TX (white wire).
const uint8_t PTH_UBX_TX  = 14;         // ESP32-S3 Thing+ PTH 14 (UART1) <-> RTK-SMA UART1 TX (green wire) - UBX.
const uint8_t PTH_UBX_RX  = 15;         // ESP32-S3 Thing+ PTH 15 (UART1) <-> RTK-SMA UART1 RX (yellow wire) - UBX.
const uint8_t PTH_RTCM_TX = 16;         // ESP32-S3 Thing+ PTH 16 (UART0) <-> RTK-SMA UART2 TX (blue wire) - RTCM.
const uint8_t PTH_RTCM_RX = 17;         // ESP32-S3 Thing+ PTH 17 (UART0) <-> RTK-SMA UART2 RX (white wire) - RTCM.

// -- Serial USB (monitor). --
const  uint8_t  NUM_COMMANDS     = 14;          // How many possible commands.
const  uint32_t SERIAL_MON_SPEED = 115200;      // Serial USB monitor speed.
const  int64_t  THROTTLE_DEBUG   = 1000000;     // Time (us) between checkForDebug() = (every 2 sec).
       bool     testLEDb;                       // Test BLE LED.
       bool     testLEDg;                       // Test GNSS LED.
       bool     testLEDr;                       // Test radio LED.
       bool     testRad;                        // Test radio.
       bool     debugRad;                       // Debug radio.
       bool     debugGNSS;                      // Debug GNSS.
       bool     debugBtn;                       // Debug lock button.
       bool     debugBLE;                       // Debug BLE.
       bool     debugDis;                       // Debug display.
       bool     debugPgm;                       // Show program state.
       bool     debugSer;                       // Show serial state.
       bool     debugUI;                        // Show UI state.
       bool     debugNet;                       // Show network state.
       bool     reset;                          // Reset MCU.
const  char     EXIT_TEST              = '!';   // Exit test mode.
const  char*    commands[NUM_COMMANDS] = {      // Valid commands. Point to array of C-strings.
                                         "testLEDb",
                                         "testLEDg",
                                         "testLEDr",
                                         "testRad",
                                         "debugRad",
                                         "debugGNSS",
                                         "debugBtn",
                                         "debugBLE",
                                         "debugDis",
                                         "debugPgm",
                                         "debugSer",
                                         "debugUI",
                                         "debugNet",
                                         "reset"
};
       char     inputCharMon;                   // Monitor input read character.
       char     monitorCommand[11];             // Serial monitor command (C-string).
       char     radioCommand[11];               // serial (radio) test command (C-string).

// -- Serial0 (RTCM->ZED-F9P). --
      HardwareSerial serialRTCM(0);             // UART0 object. Used for RTCM relay: from ESP32 UART0 in to RTK-SMA UART2.
const uint32_t SERIAL0_SPEED = 38400;           // ZED-F9P default speed.
const int64_t  GNSS_TIMEOUT  = 5000000;         // Time (us) not to exceed for last GNSS update (5 sec).

// -- Serial1 (UBX & NMEA). --
HardwareSerial serialUBXandNMEA(1);             // UART1 object. Used for UBX CFG-VAL-SET/VAL-GET & NMEA (for BLE).
const uint32_t SERIAL1_SPEED_INIT = 38400;      // Speed for ESP32Serial1 <-> ZED-F9P.
const uint32_t SERIAL1_SPEED      = 115200;     // Speed for ESP32Serial1 <-> ZED-F9P.
      char     inputCharUBXandNMEA;             // UBX & NMEA input read character.
      char     NMEAsentence[120];               // NMEA sentence buffer (C-string).

// -- Serial2 (<-RTCMradio). --
const char     eosRTCM        = '\n';           // End of sentence character.
const uint32_t SERIAL2_SPEED  = 9600;           // HC-12 default speed.
const int64_t  RADIO_TIMEOUT  = 3000000;        // Time (us) not to exceed for inputCharRTCM received (3 sec).
      char     inputCharRTCM;                   // RTCM input read character.
      int64_t  lastRTCMtime;                    // Last time (us) when RTCM inputCharRTCM received.
HardwareSerial serialRadio(2);                  // UART2 object. Used for HC-12 radio.

// -- BLE (Bluetooth Low Energy) out. --
const char       BLE_NAME[]        = "GhostRover";              // BLE name.
const TickType_t BLE_TEST_CYCLE    = 1000/portTICK_PERIOD_MS;   // Time (ms).
const u_int8_t   LED_TRIGGER_COUNT = 5;                         // Flash BLE LED once for every LED_TRIGGER_COUNT sentences sent.
const int64_t    NMEA_TIMEOUT      = 5000000;                   // Time (us) not to exceed for DevUBLOXGNSS::processNMEA().
      char       stateBLE;                                      // BLE state. Used in OLED dsiplay.
BleSerial ble;                                                  // BLE object.

// -- WiFi. --
char stateWifi;                                 // WiFi state. Used in OLED dsiplay.

// -- I2C. --
const int32_t I2C_SPEED = 400000;               // I2C clock speed.

// -- Lock button position. --
int8_t lastButtonPos;

// -- LED display. --
const TickType_t LED_TIME_FLASH_ON  = 100/portTICK_PERIOD_MS;  // Time (ms).
const TickType_t LED_TIME_FLASH_OFF = 100/portTICK_PERIOD_MS;  // Time (ms).

// -- OLED display. --
const int8_t  ROW[]         = {0,13,26,40,53};  // Vertical px position for each row using QW_FONT_8X16.
const int64_t THROTTLE_OLED = 200000;           // Time (us) between updateOLED() = (every .2 sec).
      char    oledRow[5][17];                   // 2D array for OLED display.
Qwiic1in3OLED roverOLED;                        // OLED display object. Uses SparkFun_Qwiic_OLED library.

// -- GNSS. --
const int8_t   MIN_SATELLITE_THRESHHOLD = 6;     // Minimum SIV for reliable coordinate information.
      int8_t   SIV;                              // Satellites In View.
      int8_t   fixType;                          // Fix type.
      int8_t   solnType;                         // Carrier solution type. 
      int32_t  horizontalAcc;                    // Horizontal accuracy.                                                
      int32_t  verticalAcc;                      // Vertical accuracy.
      int64_t  latitude;                         // Latitude.
      int64_t  longitude;                        // Longitude.
      int64_t  altitude;                         // Altitude in meters above Mean Sea Level (-1.0 until set).
SFE_UBLOX_GNSS_SERIAL roverGNSS;                // GNSS object (uses serial instead of I2C).

// -- Task handles. --
TaskHandle_t bleNmeaLEDtaskHandle;              // BLE NMEA       LED task handle.
TaskHandle_t gnssFixLEDtaskHandle;              // GNSS fix       LED task handle.
TaskHandle_t gnssRTKfloatLEDtaskHandle;         // GNSS RTK float LED task handle.
TaskHandle_t gnssRTKfixLEDtaskHandle;           // GNSS RTK fix   LED task handle.
TaskHandle_t radioRtcmLEDtaskHandle;            // Radio RTCM     LED task handle.
TaskHandle_t bleNMEAoutTaskHandle;              // BLE NMEA out task handle.

// -- Operation. --
static bool ghostMode = false;                  // Flag, in Ghost mode.

// -- Set state. --
char pgmState[2];       // Program state.
/**
 *    - Program state (use debugPgm to debug) -
 *      [-][-] = Initalize.
 *      [s][-] = Program section: in setup().
 *      [l][-] = Program section: in loop().
 *      [-][b] = Rover mode: RTK base.
 *      [-][p] = Rover mode: RTK PointPerfect.
 */

char serState[4];       // Serial state.
/**
 *    - Serial state (use debugSer to debug). -
 *      [-][-][-][-] = Initalize.
 *      [d][-][-][-] = USB Monitor down.
 *      [u][-][-][-] = USB Monitor up.
 *      [-][d][-][-] = serial0 (RTCM->ZED-F9P) down.
 *      [-][u][-][-] = serial0 (RTCM->ZED-F9P) up.
 *      [-][-][d][-] = serial1 (UBX & NMEA) down.
 *      [-][-][u][-] = serial1 (UBX & NMEA) up.
 *      [-][-][-][d] = serial2 (<-RTCMradio) down.
 *      [-][-][-][u] = serial2 (<-RTCMradio) up.
 */

char UIstate[4];        // UI state.
/**
 *    - UI state (debugUI to debug). -
 *      [-][-][-][-] = Initalize.
 *      [0][-][-][-] = GNSS lock button is in upPosition.
 *      [1][-][-][-] = GNSS lock button is in downPosition.
 *      [-][0][-][-] = BLE   LED - off.
 *      [-][1][-][-] = BLE   LED - on.
 *      [-][2][-][-] = BLE   LED - active.
 *      [-][-][0][-] = GNSS  LED - off.
 *      [-][-][1][-] = GNSS  LED - on.
 *      [-][-][2][-] = GNSS  LED - active (GNSS fix).
 *      [-][-][3][-] = GNSS  LED - active (RTK float).
 *      [-][-][4][-] = GNSS  LED - active (RTK fix).
 *      [-][-][-][0] = Radio LED - off.
 *      [-][-][-][1] = Radio LED - on.
 *      [-][-][-][2] = Radio LED - active.
 */

char netState[2];       // Network state.
/**
 *    - Network state (debugNet). -
 *      [-][-] = Initalize.
 *      [d][-] = NMEA out BLE down.
 *      [u][-] = NMEA out BLE up.
 *      [-][d] = WiFi down.
 *      [-][u] = WiFi up.
 */

// -- Prototypes. --
void updateLEDs(char, char, char);
void checkLockButton(char);             // Eliminates compiler scope error due to definition order.
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
 * 
 */
void initVars() {

    // -- Serial USB (monitor). --
    Serial.print("Initialize global vars");
    memset(monitorCommand, '\0', sizeof(monitorCommand));
    memset(radioCommand, '\0', sizeof(radioCommand));

    // -- Serial2 (<-RTCMradio). --
    inputCharRTCM = '\0';
    lastRTCMtime = 0;                               // The interface is down until first character.

    // -- BLE (Bluetooth Low Energy) out. --
    stateBLE = '-';

    // -- WiFi. --
    stateWifi = '-';

    // -- Lock button position. --
    lastButtonPos = 0;

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
    altitude      = -1.0;

    // -- Operation. --
    testLEDb  = false;
    testLEDg  = false;
    testLEDr  = false;
    testRad   = false;
    debugRad  = false;
    debugGNSS = false;
    debugBtn  = false;
    debugBLE  = false;
    debugDis  = false;
    debugPgm  = false;
    debugSer  = false;
    debugUI   = false;
    debugNet  = false;
    reset     = false;

    // -- Set state. --
    memset(pgmState, '-', sizeof(pgmState));
    memset(serState, '-', sizeof(serState));
    memset(UIstate,  '-', sizeof(UIstate));
    memset(netState, '-', sizeof(netState));
    Serial.println(".");

    // -- Program section. --
    Serial.print("Program section: in setup()");
    pgmState[0] = 's';
    Serial.println(".");

    // -- Serial (USB) for monitor. --
    serState[0] = 'u';
}

// --- Config. ---

/**
 * ------------------------------------------------
 *      Initialize pin modes & pin values.
 * ------------------------------------------------
 *
 * @return void No output is returned.
 * @since  0.1.0 [2025-04-24-12:00pm] New.
 * @since  0.4.7 [2025-05-21-09:30am] Switch Radio & BLE LEDs.
 */
void configPins() {

    // -- Initialize pin modes. --
    Serial.print("Config pins");
    pinMode(BUTTON_LOCK, INPUT_PULLUP);
    pinMode(LED_BLE,     OUTPUT);
    pinMode(LED_GNSS,    OUTPUT);
    pinMode(LED_RADIO,   OUTPUT);

    // -- Initialize pin values. --
    digitalWrite(LED_BLE,   LOW);
    digitalWrite(LED_GNSS,  LOW);
    digitalWrite(LED_RADIO, LOW);
    Serial.println(".");

}

// --- Begin. ---

/**
 * ------------------------------------------------
 *      Begin serial (USB) for monitor.
 * ------------------------------------------------
 *
 * @return void No output is returned.
 * @since  0.1.0 [2025-04-24-12:00pm] New.
 * @since  0.3.6 [2025-05-07-12:00pm] Refactored.
 * @since  0.3.8 [2025-05-10-09:00am] Set state.
 * @since  0.4.7 [2025-05-21-09:45am] Clean up.
 * @since  0.4.9 [2025-05-27-12:00pm] Moved to initVars.
 */
void beginSerialUSB() {

    // -- Begin USB interface. --
    Serial.begin(SERIAL_MON_SPEED);
    Serial.println("\nBegin serial monitor(USB) @ 115,200 bps.");
}

/**
 * ------------------------------------------------
 *      Begin serial0 (UART0) for RTCM->ZED-F9P.
 * ------------------------------------------------
 *
 * @return void No output is returned.
 * @since  0.3.6 [2025-05-07-12:00pm] New.
 * @since  0.4.6 [2025-05-20-09:00pm] Speed const.
 * @since  0.4.7 [2025-05-21-09:45am] Clean up.
 */
void beginSerial0RTCMtoZED() {

    // -- Begin serial0 interface. --
    Serial.print("Begin serial0 (UART0) for RTCM->ZED-F9P @ 38,400 bps");
    serialRTCM.begin(SERIAL0_SPEED, SERIAL_8N1, PTH_RTCM_RX, PTH_RTCM_TX);      // UART0 object. TX<->TX, RX<->RX.
    Serial.println(".");
}

/**
 * ------------------------------------------------
 *      Begin serial1 (UART1) for UBX & NMEA.
 * ------------------------------------------------
 * 
 * @return void No output is returned.
 * @since  0.3.6 [2025-05-07-12:00pm] New.
 * @since  0.4.6 [2025-05-20-09:00pm] Change initial speed.
 * @since  0.4.7 [2025-05-21-09:45am] Clean up.
 */
void beginSerial1UBXandNMEA() {

    // -- Begin serial1 interface. --
    Serial.print("Begin serial1 (UART1) for UBX & NMEA @ 38,400 bps");
    serialUBXandNMEA.begin(SERIAL1_SPEED_INIT, SERIAL_8N1, PTH_UBX_TX, PTH_UBX_RX);     // UART1 object. TX<->TX, RX<->RX.
    Serial.println(".");
}

/**
 * ------------------------------------------------
 *      Begin serial2 (UART2) for <-RTCMradio.
 * ------------------------------------------------
 *
 * @return void No output is returned.
 * @since  0.1.0 [2025-04-24-12:00pm] New.
 * @since  0.3.6 [2025-05-07-12:00pm] Refactored.
 * @since  0.4.7 [2025-05-21-09:45am] Clean up.
 */
void beginSerial2RTCMinFromRadio() {

    // -- Begin serial2 interface. --
    Serial.print("Begin serial2 (UART2) for <-RTCMradio @ 9,600 bps");
    serialRadio.begin(SERIAL2_SPEED, SERIAL_8N1, PTH_RX, PTH_TX);   // UART2 object. TX<->RX, RX<->TX.
    Serial.println(".");

    // -- Set pin. --
    Serial.print("Radio config pin set high (transparent mode)");
    pinMode(PTH_SET, OUTPUT);                                       // Prepare pin for HC-12 AT command set mode.
    digitalWrite(PTH_SET, HIGH);                                    // Set pin HIGH for transparent.
    Serial.println(".");
}

/**
 * ------------------------------------------------
 *      Begin I2C interface.
 * ------------------------------------------------
 *
 * @return void No output is returned.
 * @since  0.1.0 [2025-04-24-12:00pm] New.
 * @since  0.2.0 [2025-04-28-12:30pm] Add error message.
 * @since  0.4.7 [2025-05-21-09:45am] Clean up.
 */
void beginI2C() {

    // -- Begin I2C interface. --
    Serial.print("Begin I2C");
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
    Serial.print("Start display");
    if (roverOLED.begin() == false) {
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
 *      Begin & start BLE interface.
 * ------------------------------------------------
 *
 * @return void No output is returned.
 * @since  0.1.0 [2025-04-24-12:00pm] New.
 * @since  0.3.3 [2025-05-02-02:30pm].
 * @since  0.3.8 [2025-05-09-10:30pm] Move BLE task.
 * @since  0.4.7 [2025-05-21-09:45am] Clean up.
 * @link   https://github.com/avinabmalla/ESP32_BleSerial.
 */
void startBLE() {

    // -- Begin BLE interface and start. --
    Serial.print("Start Bluetooth LE");
    ble.begin(BLE_NAME);

    // -- Set state, print status. --
    netState[0] = 'u';          // NMEA out BLE up.
    Serial.printf(" \"%s\".\n", BLE_NAME);
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
    Serial.print("Start WiFi");

    // -- Set state, print status. --
    netState[1] = 'd';      // Not installed. WiFi down.
    Serial.println(" - not installed.");
}

/**
 * ------------------------------------------------
 *      Start UBX & NMEA on serial1 (UBX & NMEA). Config.
 * ------------------------------------------------
 * 
 * -- Uses library SparkFun_u-blox_GNSS_v3 for UBX-CFG-VALGET & UBX-CFG-VALSET binary commands.
 * -- BTW, where is startRoverRTCM?
 *    - No need. serialRadio(serial2) -> serialRTCM(serial0) is only an RTCM relay. It does not use the GNSS library.
 *    - serialRTCM(serial0) on the ESP32 is wired (white & blue wires) to UART2 on the RTK-SMA.
 *    - Also no need for the GNSS library to configure the RTK-SMA. The ZED-F9P default for UART2 is RTCM in.
 *
 * @return void No output is returned.
 * @since  0.1.0 [2025-04-24-12:00pm] New.
 * @since  0.3.6 [2025-05-07-02:00pm] Refactored.
 * @since  0.4.6 [2025-05-20-06:30pm] Refactored for speed change.
 * @since  0.4.7 [2025-05-21-10:30am] Clean up.
 * @since  0.4.8 [2025-05-23-09:45am] Add config options.
 * @since  0.4.9 [2025-05-27-11:15am] Adjust config options.
 * @see    Global vars: Serial0 (RTCM->ZED-F9P).
 * @see    Global vars: Serial1 (UBX & NMEA).
 * @see    Global vars:  in.
 * @see    Global vars: GNSS.
 * @see    beginSerial0RTCMtoZED().
 * @see    beginSerial2RTCMinFromRadio().
 * @see    beginSerial1UBXandNMEA().
 * @link   https://github.com/sparkfun/SparkFun_u-blox_GNSS_v3/blob/main/src/u-blox_config_keys.h.
 */
void startUBXandNMEA() {

    // -- Start GNSS interface on serialUBXandNMEA: (serial1 on ESP32) <-> UART1 on ZED-F9P. --
    Serial.print("Start roverGNSS");                                            // Print status.
    if (roverGNSS.begin(serialUBXandNMEA) == false) {                           // Bind serialUBXandNMEA to SFE_UBLOX_GNSS_SERIAL object.

        // - Could not connect to ZED-F9P over serial1 @ 38,400 bps. -
        Serial.println(" failed @ 38,400 bps. Trying another speed.");          // Hmm ... UART1 on ZED-f9P may be at 115,200 bps. Switch speeds. 
        Serial.print("End serial1 (UART1 for UBX & NMEA) @ 38,400 bps");                    
        serialUBXandNMEA.end();
        delay(100);                                                             // Give it time to close out.
        Serial.println(".");
        Serial.print("Begin serial1 (UART1 for UBX & NMEA) @ 115,200 bps");
        serialUBXandNMEA.begin(SERIAL1_SPEED, SERIAL_8N1, PTH_UBX_TX, PTH_UBX_RX);     // Set serial1 to 115,200 bps.
        Serial.println(".");
        if (roverGNSS.begin(serialUBXandNMEA) == false) {                              // Try again.
            Serial.println("Start roverGNSS failed. Freezing ...");             // Ok, something else is wrong. Freeze.
            while (true);                                                       // Infinite loop.
        } else {
            // Connected to ZED-F9P @ 115,200 bps.
            Serial.println("Start roverGNSS.");                                 // Nothing else to do. Carry on.
        }
    } else {
        // - Connected to ZED-F9P @ 38,400 bps. -
        Serial.println(".");

        // - Now, increase ZED-F9P UART1 speed ...
        Serial.printf("Config roverGNSS: CfgValset(UBLOX_CFG_UART1_BAUDRATE, %i\n", SERIAL1_SPEED);
        roverGNSS.newCfgValset();                                           // Create a new Configuration Item VALSET message.
        roverGNSS.addCfgValset(UBLOX_CFG_UART1_BAUDRATE, SERIAL1_SPEED);    // Match ZED-F9P baudrate to ESP32.
        roverGNSS.sendCfgValset() ? Serial.println(".") : Serial.println(" - failed.");

        // ... and increase serial1 UART1 speed to match. -
        Serial.print("End serial1 (UART1 for UBX & NMEA): ESP32<->ZED-F9P @ 38,400 bps"); 
        serialUBXandNMEA.end();
        delay(100);                                                          // Give it time to close out.
        Serial.println(".");
        Serial.print("Begin serial (UART1 for UBX & NMEA): ESP32<->ZED-F9P @ 115,200 bps");
        serialUBXandNMEA.begin(SERIAL1_SPEED, SERIAL_8N1, PTH_UBX_TX, PTH_UBX_RX);  // Set serial1 to 115,200 bps.
        Serial.println(".");
    }

    // -- Configure interface. --
    Serial.print("Config roverGNSS: Valset 13 keys");
    roverGNSS.newCfgValset(VAL_LAYER_RAM);                                                  // New config template.
    roverGNSS.addCfgValset(UBLOX_CFG_UART1OUTPROT_UBX,  1);                                 // UBX protocol support for config.
    roverGNSS.addCfgValset(UBLOX_CFG_UART1OUTPROT_NMEA, 1);                                 // NMEA protocol support for PVT.
    roverGNSS.addCfgValset(UBLOX_CFG_UART1OUTPROT_RTCM3X, 0);                               // Suppress.
    roverGNSS.addCfgValset(UBLOX_CFG_NMEA_HIGHPREC, 1);                                     // High precision NMEA. 
    roverGNSS.addCfgValset(UBLOX_CFG_MSGOUT_UBX_NAV_PVT_UART1, 1);                          // Output solutions periodically.
    roverGNSS.addCfgValset(UBLOX_CFG_RATE_MEAS, 500);                                       // Two solutions every 1 second (2 HZ).
    roverGNSS.addCfgValset(UBLOX_CFG_MSGOUT_NMEA_ID_GGA_UART1, 1);                          // Used by SW Maps.
    roverGNSS.addCfgValset(UBLOX_CFG_MSGOUT_NMEA_ID_GSV_UART1, 1);                          // Used by SW Maps.
    roverGNSS.addCfgValset(UBLOX_CFG_MSGOUT_NMEA_ID_GSA_UART1, 1);                          // Used by SW Maps.
    roverGNSS.addCfgValset(UBLOX_CFG_MSGOUT_NMEA_ID_GLL_UART1, 0);                          // Not needed. Suppress.
    roverGNSS.addCfgValset(UBLOX_CFG_MSGOUT_NMEA_ID_VTG_UART1, 0);                          // Not needed. Suppress.
    roverGNSS.addCfgValset(UBLOX_CFG_MSGOUT_NMEA_ID_RMC_UART1, 1);                          // Used by SW Maps.
    roverGNSS.addCfgValset(UBLOX_CFG_MSGOUT_NMEA_ID_GST_UART1, 1);                          // Used by SW Maps.
    roverGNSS.sendCfgValset() ? Serial.println(".") : Serial.println(". Failed.");          // Send the config.
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
 * @since  0.3.5 [2025-05-06-02:30pm] Create BLE test task.
 * @since  0.3.8 [2025-05-09-10:30pm] Refactor.
 * @since  0.3.8 [2025-05-14-05:45pm] Refactor.
 * @since  0.4.5 [2025-05-17-11:00am] Refactor.
 * @since  0.4.7 [2025-05-21-07:30pm] Clean up. Add BLE LED task.
 * @see    Global vars: Task handles.
 * @see    checkLockButton(), updateUI().
 * @link   https://www.freertos.org/Documentation/02-Kernel/04-API-references/01-Task-creation/01-xTaskCreate.
 */
void startTasks() {

    // -- Create Tasks. --
    xTaskCreate(bleNmeaLEDtask,      "BLE_NMEA_LED_task",         2048, NULL, 2, &bleNmeaLEDtaskHandle);
    xTaskCreate(gnssFixLEDtask,      "GNSS_fix_LED_task",         2048, NULL, 2, &gnssFixLEDtaskHandle);
    xTaskCreate(gnssRTKfloatLEDtask, "GNSS_RTK_float_LED_task",   2048, NULL, 2, &gnssRTKfloatLEDtaskHandle);
    xTaskCreate(gnssRTKfixLEDtask,   "GNSS_RTK_fix_LED_task",     2048, NULL, 2, &gnssRTKfixLEDtaskHandle);
    xTaskCreate(radioRtcmLEDtask,    "radio_RTCM_LED_task",       2048, NULL, 2, &radioRtcmLEDtaskHandle);

    // -- Suspend tasks. --
    vTaskSuspend(bleNmeaLEDtaskHandle);
    vTaskSuspend(gnssFixLEDtaskHandle);
    vTaskSuspend(gnssRTKfloatLEDtaskHandle);
    vTaskSuspend(gnssRTKfixLEDtaskHandle);
    vTaskSuspend(radioRtcmLEDtaskHandle);

    // -- Print status. --
    Serial.println("Create/suspend LED task: BLE NMEA.");
    Serial.println("Create/suspend LED task: GNSS fix, RTK float, RTK fix.");
    Serial.println("Create/suspend LED task: Radio RTCM.");
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
    checkLockButton();              // Check GNSS lock button.

    // -- Set rover mode. --
    Serial.print("Rover mode set to ");
    if (UIstate[0] == '1') {        // GNSS lock button is in downPosition.
        pgmState[1] = 'p';          // Set state to RTK PointPerfect.
        Serial.println("PointPerfect.");
    } else {
        pgmState[1] = 'b';          // Set state to RTK base mode.
        Serial.println("base.");
    }
    
    // -- Display OLED startup message. --
    Serial.print("Verify OLED");
    updateOLED('0');                // Must be after checkLockButton().
    Serial.println(".");

    // -- Verify LEDs.
    Serial.print("Verify LEDs");
    updateLEDs('1','1','1');        // All LEDs on.
    delay(2000);                    // Pause & show startup UI for 3 seconds.
    updateLEDs('0','0','0');        // All LEDs off.
    Serial.println(".");

    // -- Loop. --
    Serial.println("Starting loop.");

    // -- Display serial (USB) startup message. --
    Serial.printf("\nGhost Rover - Version %c.%c.%c @ %s.\n\n", MAJOR_VERSION, MINOR_VERSION, PATCH_VERSION, BUILD_DATE);
}

// --- Check. ---

/**
 * ------------------------------------------------
 *      Check for serial (USB) input.
 * ------------------------------------------------
 *
 * @return void No output is returned.
 * @since  0.1.0 [2025-04-28-11:00am] New.
 * @since  0.2.0 [2025-04-29-05:30pm] Refactored.
 * @since  0.3.7 [2025-05-09-04:30pm] Add loop() throttle.
 * @since  0.3.8 [2025-05-10-09:30am] Set state.
 * @since  0.3.9 [2025-05-11-10:45am] Refactored "which command".
 * @since  0.4.3 [2025-05-15-05:00pm] Added testLEDb, testLEDg, testLEDr.
 * @since  0.4.7 [2025-05-21-11:30am] Switch Radio & BLE LEDs. Cleanup.
 */
void checkSerialUSB(char print = ' ') {

    // -- Local vars. --
    static uint8_t posnMon = 0;         // Persistant input position for USB serial monitor command.
    static uint8_t posnRad = 0;         // Persistant input position for radio test command.
    uint8_t whichMonitorCommand;        // Which command was entered from the USB serial monitor.

    // -- Set state. --
    pgmState[0] = 'l';                  // Program section: in loop().

    // -- Print valid commands. --
    if(print == 'p') {
        Serial.print("\nValid commands: ");
        for (size_t i = 0; i < NUM_COMMANDS-1; i++) {
            if ((i != 0) && (i % 7 == 0)) {
                Serial.println();
            }
            Serial.printf("%s, ", commands[i]);
        }
        Serial.printf("%s.\n! to quit.\n\n", commands[NUM_COMMANDS-1]);
        return;                         // Done for now.
    }

    // -- Read serial input. --
    while (Serial.available() > 0) {    // Input from serial monitor.
        inputCharMon = Serial.read();   // Read byte in from USB serial.
        if (inputCharMon != '\n' && (posnMon < (sizeof(monitorCommand) - 1))) {    // Are we done?
            monitorCommand[posnMon] = inputCharMon;     // Not done yet, add char to command.
            posnMon++;                                  // Increment command buffer posn.
        } else {
            monitorCommand[posnMon] = '\0';             // We're done reading, treat command[] as C-string.
            posnMon = 0;                                // Reset command buffer position.

        // - Which command? -
        if(*monitorCommand == EXIT_TEST) {              // Reset debug flags & return.
            debugRad   = false;
            debugGNSS  = false;
            debugBtn   = false;
            debugBLE   = false;
            debugDis   = false;
            debugPgm   = false;
            debugSer   = false;
            debugUI    = false;
            debugNet   = false;
            Serial.println();
            return;
        }
        whichMonitorCommand = 99;       // Which command was entered. Assume invalid until validated.
            for (size_t i = 0; i < NUM_COMMANDS; i++) {
            if(strcmp(monitorCommand, commands[i]) == 0) {      // Compare C-strings.
                whichMonitorCommand = i;
                break;
            }
        }

        // - Valid command? -
        if(whichMonitorCommand < 99) {   
            // Print the validated command.
            // Toggle command flag & print new state.
            switch (whichMonitorCommand) {

                case (0):
                    testLEDb = (testLEDb == true) ? false : true;           // Flip the state.
                        Serial.printf("%s %s\n", commands[whichMonitorCommand], (testLEDb  ? "enabled." : "disabled."));
                        break;
                    case 1:
                        testLEDg = (testLEDg == true) ? false : true;       // Flip the state.
                        Serial.printf("%s %s\n", commands[whichMonitorCommand], (testLEDg  ? "enabled." : "disabled."));
                        break;
                    case 2:
                        testLEDr = (testLEDr == true) ? false : true;       // Flip the state.
                        Serial.printf("%s %s\n", commands[whichMonitorCommand], (testLEDr  ? "enabled." : "disabled."));
                        break;
                    case 3:
                        testRad = (testRad == true) ? false : true;         // Flip the state.
                        Serial.printf("%s %s\n", commands[whichMonitorCommand], (testRad  ? "enabled." : "disabled."));
                        break;
                    case 4:
                        debugRad = (debugRad == true) ? false : true;       // Flip the state.
                        Serial.printf("%s %s\n", commands[whichMonitorCommand], (debugRad ? "enabled." : "disabled."));
                        break;
                    case 5:
                        debugGNSS = (debugGNSS == true) ? false : true;     // Flip the state.
                        Serial.printf("%s %s\n", commands[whichMonitorCommand], (debugGNSS  ? "enabled." : "disabled."));
                        break;
                    case 6:
                        debugBtn = (debugBtn == true) ? false : true;       // Flip the state.
                        Serial.printf("%s %s\n", commands[whichMonitorCommand], (debugBtn   ? "enabled." : "disabled."));
                        break;
                    case 7:
                        debugBLE = (debugBLE == true) ? false : true;       // Flip the state.
                        Serial.printf("%s %s\n", commands[whichMonitorCommand], (debugBLE   ? "enabled." : "disabled."));
                        break;
                    case 8:
                        debugDis = (debugDis == true) ? false : true;       // Flip the state.
                        Serial.printf("%s %s\n", commands[whichMonitorCommand], (debugDis   ? "enabled." : "disabled."));
                        break;
                    case 9:
                        debugPgm = (debugPgm == true) ? false : true;       // Flip the state.
                        Serial.printf("%s %s\n", commands[whichMonitorCommand], (debugPgm    ? "enabled." : "disabled."));
                        break;
                    case 10:
                        debugSer = (debugSer == true) ? false : true;         // Flip the state.
                        Serial.printf("%s %s\n", commands[whichMonitorCommand], (debugSer    ? "enabled." : "disabled."));
                        break;
                    case 11:
                        debugUI = (debugUI == true) ? false : true;         // Flip the state.
                        Serial.printf("%s %s\n", commands[whichMonitorCommand], (debugUI     ? "enabled." : "disabled."));
                        break;
                    case 12:
                        debugNet = (debugNet == true) ? false : true;       // Flip the state.
                        Serial.printf("%s %s\n", commands[whichMonitorCommand], (debugNet     ? "enabled." : "disabled."));
                        break;
                    case 13:
                        reset = (reset == true) ? false : true;             // Flip the state.
                        Serial.printf("%s %s\n", commands[whichMonitorCommand], (reset       ? "enabled." : "disabled."));
                        break;
                };
                monitorCommand[0] = '\0';                                   // Ready for the next time.
            } else {

                // Invalid command.
                Serial.printf("\n%s is not a valid command. \n", monitorCommand);

                // Display valid serial Monitor commands.
                checkSerialUSB('p');
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

            // - Test BLE LED. -
            if (testLEDb) {

                // Display instructions.
                Serial.printf("Valid options: 0(off), 1(on), 2(active). %c to quit.\n", EXIT_TEST);

                // Suspend BLE NMEA out task. 
                vTaskSuspend(bleNMEAoutTaskHandle);

                // Loop.
                while (true) {                                          // Infinite loop.
                    if (Serial.available() > 0) {
                        inputCharMon = Serial.read();                   // Read input from serial monitor.
                        Serial.read();                                  // Discard newline.
                        switch (inputCharMon) {
                            case EXIT_TEST:                             // All done.
                                Serial.println("testLEDb disabled.");
                                testLEDb = false;                       // Clear test flag.
                                vTaskResume(bleNMEAoutTaskHandle);      // Resume BLE NMEA out task.
                                return;                                 // Exit test mode.
                            case '0':                                   // BLE LED - off.
                                Serial.printf("%c - BLE LED off.\n", inputCharMon);
                                updateLEDs('0','-','-');
                                break;
                            case '1':                                   // BLE LED - on.
                                Serial.printf("%c - BLE LED on.\n", inputCharMon);
                                updateLEDs('1','-','-');
                                break;
                            case '2':                                   // BLE LED - active.
                                Serial.printf("%c - BLE LED active - 5 cycles.\n", inputCharMon);
                                for (size_t i = 0; i < 5; i++) {
                                    updateLEDs('2','-','-');
                                    Serial.printf("Blink %i\n", i+1);
                                    delay(1000);
                                }
                                Serial.println();
                                break;
                            default:
                                Serial.printf("%c to quit. Valid options: 0(off), 1(on), 2(active).\n", EXIT_TEST);
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
                        inputCharMon = Serial.read();                       // Read input from serial monitor.
                        Serial.read();                                      // Discard newline.
                        switch (inputCharMon) {
                            case EXIT_TEST:                                 // All done.
                                Serial.println("testLEDg disabled.");
                                testLEDg = false;                           // Clear test flag.
                                return;                                     // Exit test mode.
                            case '0':                                       // GNSS LED off.
                                Serial.printf("%c - GNSS LED off.\n", inputCharMon);
                                updateLEDs('-','0','-');
                                break;
                            case '1':                                       // GNSS LED on.
                                Serial.printf("%c - GNSS LED on.\n", inputCharMon);
                                updateLEDs('-','1','-');
                                break;
                            case '2':                                       // GNSS LED GNSS fix.
                                Serial.printf("%c - GNSS LED active (GNSS fix) - 5 cycles.\n", inputCharMon);
                                for (size_t i = 0; i < 5; i++) {
                                    updateLEDs('-','2','-');
                                    Serial.printf("Blink %i\n", i+1);
                                    delay(1000);
                                }
                                Serial.println();
                                break;   
                            case '3':                                       // GNSS LED RTK float.
                                Serial.printf("%c - GNSS LED active (RTK float) - 5 cycles.\n", inputCharMon);
                                for (size_t i = 0; i < 5; i++) {
                                    updateLEDs('-','3','-');
                                    Serial.printf("Blink %i\n", i+1);
                                    delay(1000);
                                }
                                Serial.println();
                                break;
                            case '4':                                       // GNSS LED RTK fix.
                                Serial.printf("%c - GNSS LED active (GNSS fix) - 5 cycles.\n", inputCharMon);
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
                        inputCharMon = Serial.read();           // Read input from serial monitor.
                        Serial.read();                          // Discard newline.
                        switch (inputCharMon) {
                            case EXIT_TEST:                     // All done.
                                Serial.println("testLEDr disabled.");
                                testLEDr = false;               // Clear test flag.
                                return;                         // Exit test mode.
                            case '0':                           // Radio LED - off.
                                Serial.printf("%c - Radio LED off.\n", inputCharMon);
                                updateLEDs('-','-','0');
                                break;
                            case '1':                           // BLE LED - on.
                                Serial.printf("%c - Radio LED on.\n", inputCharMon);
                                updateLEDs('-','-','1');
                                break;
                            case '2':                           // BLE LED - active.
                                Serial.printf("%c - Radio LED active - 5 cycles.\n", inputCharMon);
                                for (size_t i = 0; i < 5; i++) {
                                    updateLEDs('-','-','2');
                                    Serial.printf("Blink %i\n", i+1);
                                    delay(1000);
                                }
                                Serial.println();
                                break;
                            default:
                                Serial.printf("%c to quit. Valid options: 0(off), 1(on), 2(active).\n", EXIT_TEST);
                        }
                    }
                }
            }

            // - Test Radio. -
            if (testRad) {

                // HC-12 AT command mode.
                radioCommand[0] = '\0';                     // Reset read buffer.
                posnRad = 0;
                digitalWrite(PTH_SET, LOW);
                serialRadio.read();                         // Garbage first character.
                updateLEDs('-','-','0');                    // Radio LED off - AT command mode.
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
                        inputCharMon = Serial.read();                       // Read input from serial monitor.
                        if(inputCharMon == EXIT_TEST) {                     // All done?
                            Serial.println("HC-12 command mode disabled.\n");
                            digitalWrite(PTH_SET, HIGH);
                            Serial.read();                                  // Clear the newline.
                            testRad = false;                                // Clear test flag.
                            return;                                         // Exit test mode.
                        } else {
                            inputCharMon = toupper(inputCharMon);           // Convert char to upper case.
                        }
                        switch (inputCharMon) {
                            case '\n':                                      // Interact with HC-12.
                                serialRadio.write(radioCommand);            // Write command to HC-12.
                                Serial.println("");
                                delay(200);                                 // Allow HC-12 to process command & respond.
                                while (serialRadio.available() > 0) {       // Read response from HC-12.
                                    inputCharMon = '\0';
                                    inputCharMon = serialRadio.read();
                                    if ((255 != (int) inputCharMon) && (posnRad > 0)) {    // Ignore first garbage character.
                                        Serial.print(inputCharMon);         // Echo character to serial monitor.
                                    }
                                }
                                radioCommand[0] = '\0';                     // Reset read buffer.
                                posnRad=0;
                                break;
                            default:                                        // Echo & save input character.
                                if (255 != (int) inputCharMon) {
                                    Serial.print(inputCharMon);
                                    radioCommand[posnRad] = inputCharMon;   // Add character to command buffer.
                                    posnRad++;
                                }
                        }
                    }
                }
            }

            // - Other. -
            // @see checkForDebug().
            // @see checkRadioRTCMToZED().
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
 * @since  0.4.7 [2025-05-21-07:30pm] Switch Radio & BLE LEDs.
 */
void checkLockButton() {

    // -- Set state of GNSS lock button. --
    if (digitalRead(BUTTON_LOCK) == HIGH) {
        UIstate[0] = '0';                   // GNSS lock button is in upPosition.
        if (lastButtonPos == 1) {           // Only true if lock button was in downPosition and now is in upPosition.
            updateLEDs('2','-','-');        // Overide BLE LED.
            lastButtonPos = 0;              // Reset lock button position.
        }
    } else {
        updateLEDs('1','-','-');            // Overide BLE LED.
        UIstate[0] = '1';                   // GNSS lock button is in downPosition.
        lastButtonPos = 1;                  // Last lock button position.
        ghostMode = true;                   // Flag for DevUBLOXGNSS::processNMEA().
    }
}

/**
 * ------------------------------------------------
 *      Check for new GNSS data.
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
 * @link   https://www.sparkfun.com/sparkfun-gnss-combo-breakout-zed-f9p-neo-d9s-qwiic.html.
 */
void checkGNSS() {

    if (SIV < MIN_SATELLITE_THRESHHOLD) {                   // Enough satellites?

        // -- Set state. --
        serState[2] = 'd';                                  // serial1 (UBX & NMEA) down.
        updateLEDs('-','1','-');
    } 
    
    if (roverGNSS.getPVT() == true) {                       // New GNSS info is available.

       // -- Set state. --
        serState[2] = 'u';                                  // serial1 (UBX & NMEA) up.

        // -- Always update these values. --
        SIV      = roverGNSS.getSIV();
        fixType  = roverGNSS.getFixType();
        solnType = roverGNSS.getCarrierSolutionType();

        // -- Update these values when GNSS is not locked. --
        if (UIstate[0] == '0') {                                // GNSS lock button is in upPosition (GNSS unlocked).
                latitude      = roverGNSS.getLatitude();
                longitude     = roverGNSS.getLongitude();
                altitude      = roverGNSS.getAltitudeMSL();     // Altitude above Mean Sea Level.
                horizontalAcc = roverGNSS.getHorizontalAccEst();
                verticalAcc   = roverGNSS.getVerticalAccEst();
        }

        // -- Update LEDs & OLED displays. --
        if (solnType == 0) {                                // Carrier Solution: 0=GNSS fix.
            updateLEDs('-','2','-');                        // GNSS LED - GNSS fix.
        } else if (solnType == 2) {                         // Carrier Solution: 1=RTK float.
            updateLEDs('-','3','-');                        // GNSS LED - RTK float.
        } else {                                            // Carrier Solution: 2=RTK fixed.
            updateLEDs('-','4','-');                        // GNSS LED - RTK fix.
        }
        
    }
    updateOLED('1');                                        // Normal display.
}

/**
 * ------------------------------------------------
 *      Check serial2 (<-RTCMradio). Send to serial0 (RTCM->ZED-F9P).
 * ------------------------------------------------
 *
 * @return void No output is returned.
 * @since  0.3.6 [2025-05-07-03:45] New.
 * @since  0.3.7 [2025-05-09-04:30pm] Add loop() throttle.
 * @since  0.4.5 [2025-05-15-05:00pm] Refactor.
 * @see    beginSerial0RTCMtoZED(), beginSerial2RTCMinFromRadio().
 * @since  0.4.7 [2025-05-21-11:45am] Switch Radio & BLE LEDs. Cleanup.
 * @link   https://github.com/sparkfun/SparkFun_u-blox_GNSS_v3/blob/main/examples/ZED-F9P/Example3_StartRTCMBase/Example3_StartRTCMBase.ino.
 */
void checkRadioRTCMToZED() {
    // TODO: 1. checkRadioRTCMToZED() - Implement out to serialRTCM, move to ESP32 task.
    
    // -- Local vars. --
    static bool gotBits = false;                    // Flag - if input since boot.

    // -- Check for Radio down. Set state. --
    if (gotBits) {                                  // Input has been received since boot.
        if((esp_timer_get_time()-lastRTCMtime) > RADIO_TIMEOUT) {
            serState[3] = 'd';                      // serial2 (<-RTCMradio) down.
            serState[1] = 'd';                      // serial0 (RTCM->ZED-F9P) down.
        }
    }

    // -- Read serial2 (<-RTCMradio) input. Send to serial0 (RTCM->ZED-F9P). --
    while(serialRadio.available() > 0) {            // serial2 (<-RTCMradio) data to read?

        // - Read serial2 (<-RTCMradio). -
        inputCharRTCM = serialRadio.read();         // Read a character @ HC12_SPEED.

        // Need somekind of line break, RTCM confirmation from ZED-F9P.
        // Integrate to updateUI.
        if (inputCharRTCM == '}') {                 // eosRTCM = '\n'.  Test code.
            if (debugRad) {
                Serial.println(inputCharRTCM);      // Last charecter.
            }
            serState[3] = 'u';                      // serial2 (<-RTCMradio) up.
            serState[1] = 'u';                      // serial0 (RTCM->ZED-F9P) up.
            lastRTCMtime = esp_timer_get_time();    // Used to check for timeout.
            gotBits = true;                         // Flag for iniitial timeout.
            updateLEDs('-','-','2');                // Radio LED - active.
            // serialRTCM.write(inputChar);         // Write a character @ SERIAL_SPEED_38.

        } else if (debugRad) {
            Serial.print(inputCharRTCM);            // Debug serial (radio) - print character.
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
 * @see    checkSerialUSB().
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
    // - Timeout status. Transit bytes are displayed by checkRadioRTCMToZED(). -
    if (debugRad) {
        nowTime = esp_timer_get_time();
        diffTime = nowTime - lastRTCMtime;
            if (diffTime > RADIO_TIMEOUT) {
                Serial.printf("\nRadio down. (diffTime= %lld)  >  (timeout= %lld)\n", diffTime, RADIO_TIMEOUT);
            }
    }

    // -- GNSS. --
    if (debugGNSS) {
        // - Display regardless up or down. -
        // roverGNSS.enableDebugging();
        Serial.printf(
        "Lat= %lld(-7)  Lon= %lld(-7)  Alt= %lld(-3)  Hac= %ld(-3)  Vac= %ld(-3)  SIV= %i  Fix= %i  Soln= %i\n",
        latitude, longitude, altitude, horizontalAcc, verticalAcc, SIV, fixType, solnType );
    }

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
            "USB= %c  RTCM(serial0)= %c  UBX(serial1)= %c  Radio(serial2)= %c\n",
            serState[0],        // USB Monitor.
            serState[1],        // serial0 (RTCM->ZED-F9P).
            serState[2],        // serial1 (UBX & NMEA).
            serState[3]         // serial2 (<-RTCMradio).
            );
    }

    // -- UI state. --
    if (debugUI) {
        Serial.printf(
            "Button= %c  BLE LED= %c  GNSS LED= %C  Radio LED= %C\n",
            UIstate[0],         // GNSS lock button (0,1).
            UIstate[1],         // BLE LED (0,1,2).
            UIstate[2],         // GNSS LED (0,1,2,3,4).
            UIstate[3]          // Radio LED (0,1,2).
        );
    }

    // -- Network state. --
    if (debugNet) {
        // - BLE data (d,u). WiFi data (d,u). -
        Serial.printf("BLE= %c  WiFi= %c\n", netState[0], netState[1]);
    }
}

// --- Display. ---

/**
 * ------------------------------------------------
 *      Toggle LEDs.
 * ------------------------------------------------
 *
 * @param  char ledB BLE LED.
 * @param  char ledG GNSS LED.
 * @param  char ledR Radio LED.
 * @return void No output is returned.
 * @since  0.2.0 [2025-04-30-11:15am] New.
 * @since  0.4.2 [2025-05-15-08:00am] Refactor.
 * @since  0.4.7 [2025-05-21-12:45pm] Switch Radio & BLE LEDs.
 * @link   https://www.freertos.org/Documentation/02-Kernel/04-API-references/02-Task-control/06-vTaskSuspend.
 * @link   https://www.freertos.org/Documentation/02-Kernel/04-API-references/02-Task-control/07-vTaskResume.
 */
void updateLEDs(char ledB, char ledG, char ledR) {

    // --- BLE LED. ---
    switch (ledB) {
        case '0':
            UIstate[1] = '0';                           // BLE LED - off.
            digitalWrite(LED_BLE, LOW);                 // LED off.
            break;
        case '1':
            UIstate[1] = '1';                           // BLE LED - on.
            digitalWrite(LED_BLE, HIGH);                // LED on.
            break;
        case '2':
            UIstate[1] = '2';                           // BLE LED - active.
            vTaskResume(bleNmeaLEDtaskHandle);          // Resume task.
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

    // --- Radio LED. ---
    switch (ledR) {
        case '0':
            UIstate[3] = '0';                           // Radio LED - off.
            digitalWrite(LED_RADIO, LOW);               // LED off.
            break;
        case '1':
            UIstate[3] = '1';                           // Radio LED - on.
            digitalWrite(LED_RADIO, HIGH);              // LED on.
            break;
        case '2':
            UIstate[3] = '2';                           // BLE LED - active.
            vTaskResume(radioRtcmLEDtaskHandle);        // Resume task.
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
            strcpy(oledRow[4], " ");
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

            // 3rd row - altitude.
            sprintf(oledRow[2], "Alt   %.2f m", double( altitude ) / 1000.0);

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
            if (netState[0] == 'u') {                           // BLE data state.
                stateBLE = '+';                                 // Displayed in 5th row.
            } else {
                stateBLE = '-';                                 // Displayed in 5th row.
            } 
            if (netState[1] == 'u') {                           // WiFi data state.
                stateWifi = '+';                                // Displayed in 5th row.
            } else {
                stateWifi = '-';                                // Displayed in 5th row.
            }
            sprintf(oledRow[4], "S:%2i F%1i S%1i B%c W%c",
                SIV, fixType, solnType, stateBLE, stateWifi);
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

/**
 * ------------------------------------------------
 *      NMEA out BLE.
 * ------------------------------------------------
 * 
 * -- Process each NMEA character in from the SparkFun u-blox Arduino Library.
 *    - Triggered by roverGNSS.getPVT() in checkGNSS().
 * -- NEMA interface:
 *    - serial: serialUBXandNMEA = pins PTH_UBX_TX & PTH_UBX_RX bound to ESP32 serial1 (UART1) in beginSerial1UBXandNMEA() in setup().
 *      baudrate: initially set at 38,400 bps in beginSerial1UBXandNMEA() in setup().
 *      baudrate: increased to 115,4200 bps in startUBXandNMEA() in setup.
 *    - library: roverGNSS = GNSS library object bound to serialUBXandNMEA in startUBXandNMEA() in setup.
 * -- NEMA config:
 *    - startUBXandNMEA() in setup() sets the following for ZED-F9P UART1:
 *      Output GNSS solutions periodically (as opposed to being polled).
 *      Calculate one GNSS solution every second.
 *      NMEA protocol out.
 *      NMEA sentences suppressed (GLL, VTG).
 *      NMEA sentences out at 1 cycle/second:
 *        GNGGA = PVT, fix quality, SIV, HDOP, ...
 *        GPGSV = # Sats visible, sat info, ...
 *        GNGSA = PRN # for active sat, PDOP/HDOP/VDOP, ...
 *        GNRMC = PVT, ...
 *        GNGST = Position error statistics, ...
  * -- BLE interface:
 *    - created & initialized in startBLE() in setup().
 *
 * @param  char incoming GNSS sentence char.
 * @return void No output is returned.
 * @since  0.4.9 [2025-05-28-05:45pm] New.
 * @see    startBLE().
 * @see    startUBXandNMEA().
 * @see    checkGNSS().
 * @see    updateOLED().
 * @link   https://receiverhelp.trimble.com/alloy-gnss/en-us/NMEA-0183messages_GGA.html.
 * @link   https://receiverhelp.trimble.com/alloy-gnss/en-us/NMEA-0183messages_RMC.html.
 * @link   https://receiverhelp.trimble.com/alloy-gnss/en-us/NMEA-0183messages_GSA.html.
 * @link   https://receiverhelp.trimble.com/alloy-gnss/en-us/NMEA-0183messages_GSV.html.
 * @link   https://receiverhelp.trimble.com/alloy-gnss/en-us/NMEA-0183messages_GST.html.
 * @link   https://cdn.sparkfun.com/assets/a/3/2/f/a/NMEA_Reference_Manual-Rev2.1-Dec07.pdf.
 * @link   https://github.com/avinabmalla/ESP32_BleSerial.
 * @link   https://docs.sparkfun.com/SparkFun_RTK_Everywhere_Firmware/connecting_bluetooth/.
 * @link   https://www.tutorialspoint.com/c_standard_library/c_function_strtok.htm.
 */
void DevUBLOXGNSS::processNMEA(char incoming) {

    // -- Local vars. --
    static bool      gnssSentenceStarted    = false;        // Flag, forming a sentence.
    static char      gnssSentence[128]      = {'\0'};       // Working sentence.
    static char      lastGNGGAsentence[128] = {'\0'};       // Last GGGGA sentence.
    static char      lastGNRMCsentence[128] = {'\0'};       // Last GGRMC sentence.
    static u_int8_t  LEDsentenceCount       = 0;            // Flash BLE LED if count == LEDTRIGGER_LED_COUNT.
    static u_int8_t  gnssSentencePosn       = 0;            // Index position for working sentence.
    static u_int8_t  gnssSentencePosnEnd    = 0;            // Working sentence end.
    static u_int32_t sentenceCount          = 0;            // Total # of sentences sent out BLE since boot.
    const  char *    delimiter              = ",";          // Delimiter for GNGGA strtok().
           char *    token;                                 // Token for GNGGA strtok().
           u_int8_t  count;                                 // Counter for GNGGA strtok() field tokens.

    // -- Update state. --
    netState[0] = 'u';                                      // Character received. Set NMEA out BLE to up.

    // -- Look for start of sentence. --
    if (incoming == '$') {                                  // First char of a NMEA sentence is always '$'.
        gnssSentenceStarted = true;                         // Starting a new sentence.
        memset(gnssSentence, '\0', sizeof(gnssSentence));   // Clear the sentence buffer.
        gnssSentencePosn = 0;                               // First character.
        gnssSentencePosnEnd = sizeof(gnssSentence);         // Initial length. Shorten once '*' is read.
    }

    // -- If forming a sentence, add the incoming character. -- 
    if (gnssSentenceStarted == true) {
        gnssSentence[gnssSentencePosn++] = incoming;        // Started a sentence, add the incoming char.

        // - Is sentence too long, almost done, or done? -
        if (gnssSentencePosn == sizeof(gnssSentence)) {     // Sentence too long.
            gnssSentenceStarted = false;                    // Start over.
        } else if (incoming == '*') {                       // Almost done. CRC indicator. Length of sentence is now known.
            gnssSentencePosnEnd = gnssSentencePosn + 2;     // Length of sentence = current position + two bytes for CRC.
        } else if (gnssSentencePosn == gnssSentencePosnEnd) {   // Done!

            // Done. Full NMEA sentence.
            gnssSentenceStarted = false;                    // Start over.
            gnssSentence[gnssSentencePosn+1] = '\0';        // Terminate the sentence (C-string).
            sentenceCount++;                                // Counter for total # of sentences sent out BLE since boot.

            // Check GNSS lock button.
            if (UIstate[0] == '0') {                        // GNSS lock button is in upPosition.

                // Send current sentence (any Gxxxx) over BLE.
                ble.printf("%s\r\n", gnssSentence);

                // Update rover BLE LED & counts.
                if (LEDsentenceCount == LED_TRIGGER_COUNT) {    // Flash BLE LED once for every LED_TRIGGER_COUNT sentences sent.
                    updateLEDs('2','-','-');                    // BLE LED - active.
                    LEDsentenceCount = 0;
                } else {
                    LEDsentenceCount++;                         // Counter for LED flash.
                }
                
                // Check debug.
                if (debugBLE) {
                    Serial.printf("\nBLE NMEA sentence # %i.\n%s\n", sentenceCount, gnssSentence);
                }

                // If sentence is PVT type (GNGGA or GNRMC), save current sentence as last sentence.
                if (memcmp(gnssSentence,"$GNGGA",6) == 0) {
                    memset(lastGNGGAsentence, '\0', sizeof(lastGNGGAsentence));
                    strcpy(lastGNGGAsentence, gnssSentence);
                }
                if (memcmp(gnssSentence,"$GNRMC",6) == 0) {
                    memset(lastGNRMCsentence, '\0', sizeof(lastGNRMCsentence));
                    strcpy(lastGNRMCsentence, gnssSentence);
                }

            } else {    // GNSS lock button is in downPosition. Ghost mode.

                // If current sentence is PVT type (GNGGA or GNRMC), send last (not current) sentence out BLE interface.
                // Spoofs the BLE receiver with coordinates from where the button was pushed, not where it is now.
                if (memcmp(gnssSentence,"$GNGGA",6) == 0) {
                    ble.printf("%s\r\n",lastGNGGAsentence);     // Send sentence.
                    if (debugBLE) {                             // Check debug.
                        Serial.printf("\nSubstituting last GNGGA sentence.\n%s\n", lastGNGGAsentence);
                    }
                }
                if (memcmp(gnssSentence,"$GNRMC",6) == 0) {
                    ble.printf("%s\r\n",lastGNRMCsentence);     // Send sentence.
                    if (debugBLE) {                             // Check debug.
                        Serial.printf("\nSubstituting last GNRMC sentence.\n%s\n", lastGNRMCsentence);
                    }
                }

                // Rover BLE LED is already set on by checkLockButton().

                // Update OLED position data. Calculate values direct from lastGNGGA sentence.

                if (ghostMode) {                                        // Set by checkLockButton().

                    // Init vars.
                    ghostMode = false;                                  // Only do first time after ghostMode is set.
                    memset(gnssSentence, '\0', sizeof(gnssSentence));   // Null out the working sentence.
                    strcpy(gnssSentence,lastGNGGAsentence);             // Copy the GNGGA sentence into the working sentence.
                    token = strtok(gnssSentence, delimiter);            // Tokenize the working sentence.
                    count = 0;                                          // Field counter.

                    // Loop sentence fields.
                    while (token != NULL) {
                        if (count == 2) {                               // Field #2 is ddmm.mmmmmmm latitude.
                            latitude = convert_nmea_ddmm_to_int64(token);
                        }
                        if (count == 3) {                               // Field #3 is (N/S) latitude direction.
                            if (*token == 'S') {
                                latitude = latitude * -1;
                            }
                        }
                        if (count == 4) {                               // Field #4 is ddmm.mmmmmmm longitude.
                            longitude = convert_nmea_ddmm_to_int64(token);
                        }
                        if (count == 5) {                               // Field #5 is (E/W) longitude direction.
                            if (*token == 'W') {
                                longitude = longitude * -1;
                            }
                        }
                        if (count == 9) {                               // Field #9 is M altitude (orthometric height - MSL).
                            altitude = (int64_t)(((double)atof(token)) * 1000.0);
                        }
                        count++;                                        // Next field.
                        token = strtok(NULL, delimiter);                // Next token.
                    }

                    updateOLED('1');                                    // Normal display.
                }

                if (debugBLE) {                                         // Debug.
                    Serial.printf("\nlastGNGGAsentence = %s\n",lastGNGGAsentence);
                    Serial.printf("latitude=%i\n",latitude);
                    Serial.printf("longitude=%i\n" ,longitude);
                    Serial.printf("altitude=%i\n"  ,altitude);
                    Serial.printf("lastGNRMCsentence = %s\n",lastGNRMCsentence); 

                }
            }
        }
    }
}

// --- Operation. ---

/**
 * Utility to convert ddmm.mmmmmmm to int_64.
 *
 * @param  const char *nmea_token POsition token from GNGGA sentence. 
 * @return int64_t number.
 * @since  0.5.0 [2025-05-28-05:45pm] New.
 * @see    DevUBLOXGNSS::processNMEA().
 */
int64_t convert_nmea_ddmm_to_int64(const char *nmea_token) {

    // -- Convert token to float. --
    double raw = atof(nmea_token);

    // -- Separate degrees and minutes. --
    int degrees = (int)(raw / 100);  // Get dd.
    double minutes = raw - (degrees * 100);  // Get mm.mmmmmmm.

    // -- Convert to decimal degrees. --
    double decimal_degrees = degrees + (minutes / 60.0);

    // -- Scale to preserve 7 decimal places and convert to int64_t. --
    return (int64_t)round(decimal_degrees * 10000000.0);
}

// --- Tasks. ---

/**
 * ------------------------------------------------
 *      BLE LED - active task.
 * ------------------------------------------------
 *
 * @param  void * pvParameters Pointer to task parameters.
 * @return void No output is returned.
 * @since  0.4.7 [2025-05-21-01:15pm] New.
 * @since  0.4.7 [2025-05-21-06:00pm] Refactor. Duh.
 * @see    startTasks().
 * @link   https://www.freertos.org/Documentation/02-Kernel/04-API-references/02-Task-control/06-vTaskSuspend.
 */
void bleNmeaLEDtask(void * pvParameters) {
    while(true) {
        digitalWrite(LED_BLE, HIGH);                // LED on.
        vTaskDelay(LED_TIME_FLASH_ON);              // LED remains on.
        digitalWrite(LED_BLE, LOW);                 // LED off.
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
 * @see    bleNmeaLEDtask().
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
    beginSerialUSB();               // Begin serial (USB) for monitor.
    initVars();                     // Initialize global vars.
    configPins();                   // Initialize pin modes & pin values.
    beginSerial0RTCMtoZED();        // Begin serial0 (UART0) for RTCM->ZED-F9P.
    beginSerial1UBXandNMEA();       // Begin serial1 (UART1) for UBX & NMEA.
    beginSerial2RTCMinFromRadio();  // Begin serial2 (UART2) for <-RTCMradio.
    beginI2C();                     // Begin I2C.
    startDisplay();                 // Start OLED display.
    startBLE();                     // Begin & start BLE interface.
    startWiFi();                    // Begin & start WiFi interface.
    startUBXandNMEA();              // Start UBX & NMEA on serial1 (UBX & NMEA). Config.
    startTasks();                   // Start tasks.
    startUI();                      // Start UI.
}                                               

// =================================== 
//               Loop.
// ===================================

void loop() {
    checkSerialUSB();           // Check for serial (USB) input.
    checkLockButton();          // Check GNSS lock button.
    checkGNSS();                // Check for new GNSS data. DevUBLOXGNSS::processNMEA().
    checkRadioRTCMToZED();      // Check serial2 (<-RTCMradio). Send to serial0 (RTCM->ZED-F9P).
    checkForDebug();            // Check to display debug.
}
