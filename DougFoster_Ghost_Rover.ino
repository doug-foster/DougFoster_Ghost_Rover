/**
 * Ghost Rover.
 * 
 * Now you see me, now you ...
 * 
 * GNSS fix type: 0=none, 1=dead reckoning, 2=2D, 3=3D, 4=GNSS, 5=Time.
 * GNSS carrier solution: 0=none, 1=RTK float, 2=RTK fixed.
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
 * @since    0.4.6 [2025-05-17-09:00pm].
 * @link     http://dougfoster.me.
 *
 * ===================================
 *      Comments.
 * ===================================
 *
 *  --- Major components. ---
 *    MCU     https://www.sparkfun.com/sparkfun-thing-plus-esp32-s3.html.
 *    GNSS    https://www.sparkfun.com/sparkfun-gps-rtk-sma-breakout-zed-f9p-qwiic.html.
 *    Display https://www.sparkfun.com/sparkfun-qwiic-oled-1-3in-128x64.html.
 * 
 *  --- Other components. ---
 *  -- GNSS antenna. --
 *    antenna (L1/L2/L5, TNC-F): https://www.sparkfun.com/gnss-multi-band-l1-l2-l5-surveying-antenna-tnc-spk6618h.html.
 *    adapter (TNC-M to SMA-M): https://www.amazon.com/dp/B0BGPJP3J3.
 *    cable (SMA-F to SMA-M, 12" RG316): https://www.amazon.com/dp/B07MJQWH8S.
 *  -- Radio. --
 *    radio (433.4-473.0 MHz, 100mW, U.FL): https://www.amazon.com/HiLetgo-Wireless-Replace-Bluetooth-Antenna/dp/B01MYTE1XR.
 *    antenna (400-960 MHz, BNC-M): https://www.amazon.com/dp/B07R4PGZK3.
 *    cable (BNC-F bulkhead to U.FL, 8" RG178): https://www.amazon.com/dp/B098HX6NFH.
 *  -- Misc. --
 *    push button (12mm latching): https://www.amazon.com/dp/B0CGTXMLKL.
 *    power switch (15mm on/off latching toggle): https://www.amazon.com/dp/B09XMDXKTR.
 *    LED covers (5mm LED bulb socket): https://www.amazon.com/dp/B07CQ6TH14.
 *    battery (+5V 2.4A max, 8000 mAh): https://www.walmart.com/ip/onn-8000mAh-Portable-Battery-Power-Bank-with-USB-A-to-C-Charging-Cable-LED-Indicator-Black/5266111773
 *    enclosure: https://www.sparkfun.com/hand-held-enclosure-abs-plastic.html.
 *    handle: https://www.amazon.com/dp/B01FUEXLGU.
 *    tripod base: https://www.amazon.com/dp/B07GST1C2Z.
 *    adjustable phone mount: https://www.amazon.com/dp/B07S8TTH34.
 *    other: 80/20, 1/4-20 nuts, bots, washers, USB-A power cable.
 *
 *  -- References. --
 *   RTK   https://learn.sparkfun.com/tutorials/how-to-build-a-diy-gnss-reference-station/all.
 *   EVK   https://docs.sparkfun.com/SparkFun_RTK_EVK/introduction/.
 *   HC-12 https://www.elecrow.com/download/HC-12.pdf.
 * 
 *  -- Code flow. --
 *   Include libraries.
 *   Global vars: define vars, set constants, prototypes.
 *   Functions: init, config, begin, start, check, display, callback, operation, tasks, test.
 *   Setup.
 *   Loop.
 * 
 * -- TODO: --
 *  1. checkRadioToRTCM() - Implement out to serialRTCM, move to ESP32 task.
 *  2. serialBLEtaskRelayNEMA() - Finish relay task.
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
const char BUILD_DATE[]   = "2025-05-17-21:00";         // 24hr format, need to fit max (16) characters.
const char MAJOR_VERSION  = '0';
const char MINOR_VERSION  = '4';
const char PATCH_VERSION  = '6';

// -- Pin (pth) definitions. --
const uint8_t BUTTON_LOCK = 4;          // ESP32-S3 Thing+ PTH 4 <-> Red toggle button (yellow wire).
const uint8_t LED_RADIO   = 6;          // ESP32-S3 Thing+ PTH 6 <-> Blue LED (blue wire).
const uint8_t LED_GNSS    = 5;          // ESP32-S3 Thing+ PTH 5 <-> Yellow LED (green wire).
const uint8_t LED_LOCK    = 7;          // ESP32-S3 Thing+ PTH 7 <-> Red LED (white wire).
const uint8_t I2C_SDA     = 8;          // ESP32-S3 Thing+ PTH SDA/8 - I2C data pin.
const uint8_t I2C_SCL     = 9;          // ESP32-S3 Thing+ PTH SCL/9 - I2C clock pin.
const uint8_t PTH_SET     = 42;         // ESP32-S3 Thing+ PTH 42 (SET) <-> HC-12 SET (blue wire).
const uint8_t PTH_TX      = 43;         // ESP32-S3 Thing+ PTH 43 (UART2) <-> HC-12 RX (yellow wire).
const uint8_t PTH_RX      = 44;         // ESP32-S3 Thing+ PTH 44 (UART2) <-> HC-12 TX (white wire).
const uint8_t PTH_UBX_TX  = 14;         // ESP32-S3 Thing+ PTH 14 (UART1) <-> RTK-SMA UART1 TX (green wire) - UBX.
const uint8_t PTH_UBX_RX  = 15;         // ESP32-S3 Thing+ PTH 15 (UART1) <-> RTK-SMA UART1 RX (yellow wire) - UBX.
const uint8_t PTH_RTCM_TX = 16;         // ESP32-S3 Thing+ PTH 16 (UART0) <-> RTK-SMA UART2 TX (blue wire) - RTCM.
const uint8_t PTH_RTCM_RX = 17;         // ESP32-S3 Thing+ PTH 17 (UART0) <-> RTK-SMA UART2 RX (white wire) - RTCM.

// -- Serial USB - monitor. --
const  uint8_t  NUM_COMMANDS   = 14;            // How many possible commands.
const  uint32_t MONITOR_SPEED  = 115200;        // Serial USB monitor speed.
const  int64_t  THROTTLE_DEBUG = 1000000;       // Time (us) between checkDebug() = (every 2 sec).
       bool     testLEDr;                       // Test serial (radio) LED.
       bool     testLEDg;                       // Test GNSS LED.
       bool     testLEDl;                       // Test GNSS lock LED.
       bool     testRad;                        // Test serial (radio).
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
const  char     EXIT_TEST = '!';                // Exit test mode.
const  char*    commands[NUM_COMMANDS] = {      // Valid commands. Point to array of C-strings.
                                         "testLEDr",
                                         "testLEDg",
                                         "testLEDl",
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

// -- Serial(0) - GNSS(RTCM) out. --
      HardwareSerial serialRTCM(0);             // UART0 object. Used for RTCM relay: from ESP32 UART0 in to RTK-SMA UART2.
const uint32_t ZED_SPEED    = 38400;            // ZED-F9P default speed.
const int64_t  GNSS_TIMEOUT = 5000000;          // Time (us) not to exceed for last GNSS update (5 sec).

// -- Serial(1) - GNSS(UBX). --
HardwareSerial serialUBX(1);                    // UART1 object. Used for UBX CFG-VAL-SET/VAL-GET.

// -- Serial(2) - Radio in. --
const char     eosRTCM           = '\n';        // End of sentence character.
const uint32_t HC12_SPEED    = 9600;            // HC-12 default speed.
const int64_t  RADIO_TIMEOUT = 3000000;         // Time (us) not to exceed for inputCharRTCM received (3 sec).
      char     inputCharRTCM;                   // RTCM input read character.
int64_t  lastRTCMtime;                          // Last time (us) when RTCM inputCharRTCM received.
HardwareSerial serialRadio(2);                  // UART2 object. Used for HC-12 radio.

// -- BLE (Bluetooth Low Energy) out. --
const char     BLE_NAME[] = "GhostRover";       // BLE name.
      char     stateBLE;                        // BLE state.
      uint16_t bleCount;                        // BLE counter.
BleSerial ble;                                  // BLE object.

// -- WiFi. --
char stateWifi;                                 // Default WiFi state.

// -- I2C. --
const int32_t I2C_SPEED = 400000;               // I2C clock speed.

// -- OLED display. --
const int8_t ROW[]          = {0,13,26,40,53};  // Vertical px position for each row using QW_FONT_8X16.
const int64_t THROTTLE_OLED =  200000;          // Time (us) between updateOLED() = (every .2 sec).
      char   oledRow[5][17];                    // 2D array for OLED display.
Qwiic1in3OLED roverOLED;                        // OLED display object. Uses SparkFun_Qwiic_OLED library.

// -- LED display. --
const int16_t LED_TIME_FLASH_ON  =  100;        // Time (ms).
const int16_t LED_TIME_FLASH_OFF =  300;        // Time (ms).

// -- GNSS. --
const int8_t  MIN_SATELLITE_THRESHHOLD = 6;     // Minimum SIV for reliable coordinate information.
      int8_t  SIV;                              // Satellites In View.
      int8_t  fixType;                          // Fix type.
      int8_t  solnType;                         // Carrier solution type. 
      int32_t horizontalAcc;                    // Horizontal accuracy.                                                
      int32_t verticalAcc;                      // Vertical accuracy.
      int64_t latitude;                         // Latitude.
      int64_t longitude;                        // Longitude.
      int64_t altitude;                         // Altitude in meters above Mean Sea Level (-1.0 until set).
SFE_UBLOX_GNSS_SERIAL roverGNSS;                // GNSS object (uses serial instead of I2C).

// -- Task handles. --
TaskHandle_t serialLEDtaskRTCMHandle;           // serial (radio) LED - active: task handle.
TaskHandle_t gnssLEDtaskRTKfloatHandle;         // GNSS  LED - RTK float: task handle.
TaskHandle_t gnssLEDtaskRTKfixHandle;           // GNSS  LED - RTK fix: task handle.
TaskHandle_t gnssLEDtaskGNSSfixHandle;          // GNSS  LED - GNSS fix: task handle.
TaskHandle_t serialBLEtaskRelayNEMAHandle;      // serial to BLE NEMA relay: task handle.

// -- Operation. --

// -- Set state. --
char pgmState[2];       // Program state.
/**
 *  - Program state (debugPgm to debug) -
 *  [-][-] = Initalize.
 *  [s][-] = Program section: in setup().
 *  [l][-] = Program section: in loop().
 *  [-][b] = Rover mode: RTK base.
 *  [-][p] = Rover mode: RTK PointPerfect.
 */

char serState[4];       // Serial state.
/**
 *   - Serial state (debugSer to debug). -
 *  [-][-][-][-] = Initalize.
 *  [d][-][-][-] = USB Monitor down.
 *  [u][-][-][-] = USB Monitor up.
 *  [-][d][-][-] = RTCM data (serial0) down.
 *  [-][u][-][-] = RTCM data (serial0) up.
 *  [-][-][d][-] = UBX data (serial1) down.
 *  [-][-][u][-] = UBX data (serial1) up.
 *  [-][-][-][d] = serial (radio) data (serial2) down.
 *  [-][-][-][u] = serial (radio) data (serial2) up.
 */

char UIstate[4];        // UI state.
/**
 *  - UI state (debugUI to debug). -
 *  [-][-][-][-] = Initalize.
 *  [0][-][-][-] = GNSS lock button is in upPosition.
 *  [1][-][-][-] = GNSS lock button is in downPosition.
 *  [-][0][-][-] = serial (radio) LED - off.
 *  [-][1][-][-] = serial (radio) LED - on.
 *  [-][4][-][-] = serial (radio) LED - active.
 *  [-][-][0][-] = GNSS LED - off.
 *  [-][-][1][-] = GNSS LED - on.
 *  [-][-][2][-] = GNSS LED - GNSS fix.
 *  [-][-][3][-] = GNSS LED - RTK float.
 *  [-][-][4][-] = GNSS LED - RTK fix.
 *  [-][-][-][0] = GNSS lock LED - off.
 *  [-][-][-][1] = GNSS lock LED - on.
 */

char netState[2];       // Network state.
/**
 * - Network state (debugNet). -
 * [-][-] = Initalize.
 * [d][-] = BLE down.
 * [u][-] = BLE up.
 * [-][d] = WiFi down.
 * [-][u] = WiFi up.
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
 */
void initVars() {

    // -- Serial USB - monitor. --
    memset(monitorCommand, '\0', sizeof(monitorCommand));
    memset(radioCommand, '\0', sizeof(radioCommand));

    // -- Serial(2) - Radio in. --
    inputCharRTCM = '\0';
    lastRTCMtime = 0;                               // The interface is down until first character.

    // -- BLE (Bluetooth Low Energy) --
    stateBLE = '-';
    bleCount = 0;

    // -- WiFi. --
    stateWifi = '-';

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
    testLEDr  = false;
    testLEDg  = false;
    testLEDl  = false;
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

    // -- Print state. --
    Serial.println("Global vars initialized.");
}

// --- Config. ---

/**
 * ------------------------------------------------
 *      Config pins.
 * ------------------------------------------------
 *
 * @return void No output is returned.
 * @since  0.1.0 [2025-04-24-12:00pm] New.
 */
void configPins() {

    // -- Initialize pin modes. --
    pinMode(BUTTON_LOCK, INPUT_PULLUP);
    pinMode(LED_RADIO, OUTPUT);
    pinMode(LED_GNSS,  OUTPUT);
    pinMode(LED_LOCK,  OUTPUT);

    // -- Initialize pin values. --
    digitalWrite(LED_RADIO, LOW);
    digitalWrite(LED_GNSS,  LOW);
    digitalWrite(LED_LOCK,  LOW);

    // -- Print state. --
    Serial.println("Config pins.");
}

// --- Begin. ---

/**
 * ------------------------------------------------
 *      Begin serial (USB for monitor).
 * ------------------------------------------------
 *
 * @return void No output is returned.
 * @since  0.1.0 [2025-04-24-12:00pm] New.
 * @since  0.3.6 [2025-05-07-12:00pm] Refactored.
 * @since  0.3.8 [2025-05-10-09:00am] Set state.
 */
void beginSerialMonitor() {

    // -- Begin USB interface. --
    Serial.begin(MONITOR_SPEED);

    // -- Set/print state. --
    serState[0] = 'u';                                  // USB Monitor up.
    Serial.println("\nBegin serial monitor(USB).");
    pgmState[0] = 's';                                  // Program section: in setup().
    Serial.println("Program section: in setup().");
}

/**
 * ------------------------------------------------
 *      Begin serial (UART0 for RTCM).
 * ------------------------------------------------
 *
 * @return void No output is returned.
 * @since  0.3.6 [2025-05-07-12:00pm] New.
 */
void beginSerialRTCM() {

    // -- Begin serial interface. --
    serialRTCM.begin(ZED_SPEED, SERIAL_8N1, PTH_RTCM_RX, PTH_RTCM_TX);      // UART0 object. TX<->TX, RX<->RX.

    // -- Print status. --
    Serial.println("Begin serial (UART0 for RTCM).");
}

/**
 * ------------------------------------------------
 *      Begin serial (UART1 for UBX).
 * ------------------------------------------------
 * 
 * @return void No output is returned.
 * @since  0.3.6 [2025-05-07-12:00pm] New.
 */
void beginSerialUBX() {

    // -- Begin serial interface. --
    serialUBX.begin(ZED_SPEED, SERIAL_8N1, PTH_UBX_TX, PTH_UBX_RX);     // UART1 object. TX<->TX, RX<->RX.

    // -- Print status. --
    Serial.println("Begin serial (UART1 for UBX).");
}

/**
 * ------------------------------------------------
 *      Begin serial (UART2 for HC-12 radio).
 * ------------------------------------------------
 *
 * @return void No output is returned.
 * @since  0.1.0 [2025-04-24-12:00pm] New.
 * @since  0.3.6 [2025-05-07-12:00pm] Refactored.
 */
void beginSerialRadio() {

    // -- Begin serial interface. --
    serialRadio.begin(HC12_SPEED, SERIAL_8N1, PTH_RX, PTH_TX);      // UART2 object. TX<->RX, RX<->TX.

    // -- Set pin. --
    pinMode(PTH_SET, OUTPUT);                                       // Prepare pin for HC-12 AT command set mode.
    digitalWrite(PTH_SET, HIGH);                                    // Set pin HIGH for transparent.

    // -- Print status. --
    Serial.println("Begin serial (UART2 for HC-12 radio).");
}

/**
 * ------------------------------------------------
 *      Begin I2C interface.
 * ------------------------------------------------
 *
 * @return void No output is returned.
 * @since  0.1.0 [2025-04-24-12:00pm] New.
 * @since  0.2.0 [2025-04-28-12:30pm] Add error message.
 */
void beginI2C() {

    // -- Begin I2C interface. --
    if (Wire.begin() == false) {
        Serial.println("Begin I2C failed. Freezing ...");
        while (true);               // Infinite loop.
    }

    // -- Configure I2C interface. --
    // Wire.setClock(I2C_SPEED);    // I2C clock speed.
    // Serial.printf("I2C set to %iHz.\n", I2C_SPEED);

    // -- Print status. --
    Serial.println("Begin I2C.");
}

// --- Start. ---

/**
 * ------------------------------------------------
 *      Start OLED display.
 * ------------------------------------------------
 * 
 * @return void No output is returned.
 * @since  0.1.0 [2025-04-24-12:00pm] New.
 * @link   https://github.com/sparkfun/SparkFun_Qwiic_OLED_Arduino_Library/blob/main/src/res/qw_fnt_8x16.h.
 */
void startDisplay() {

    // -- Start OLED display. --
    if (roverOLED.begin() == false) {
        Serial.println("Start display failed. Freezing ...");
        while (true);                   // Infinite loop.
    }

    // -- Print status. --
    Serial.println("Start display.");

    // -- Configure font. Only use 8x16 size. --
    roverOLED.setFont(QW_FONT_8X16);    // Set display font.
}

/**
 * ------------------------------------------------
 *      Start BLE interface.
 * ------------------------------------------------
 *
 * @return void No output is returned.
 * @since  0.1.0 [2025-04-24-12:00pm] New.
 * @since  0.3.3 [2025-05-02-02:30pm].
 * @since  0.3.8 [2025-05-09-10:30pm] Move BLE task.
 * @link   https://github.com/avinabmalla/ESP32_BleSerial.
 */
void startBLE() {

    // -- Begin BLE interface and start. --
    ble.begin(BLE_NAME);

    // -- Set state, print status. --
    netState[0] = 'u';          // BLE up.
    Serial.printf("Start Bluetooth LE \"%s\".\n", BLE_NAME);
}

/**
 * ------------------------------------------------
 *      Start WiFi interface.
 * ------------------------------------------------
 *
 * @return void No output is returned.
 * @since  0.1.0 [2025-05-13-03:00pm] New.
 */
void startWiFi() {

    // -- Start Wifi --
    // Not installed.

    // -- Set state, print status. --
    netState[1] = 'd';      // WiFi down.
    Serial.println("Start WiFi - not installed.");
}

/**
 * ------------------------------------------------
 *      Start UBX GNSS on serialUBX(serial1), 
 *      config.
 * ------------------------------------------------
 * 
 * Uses library SparkFun_u-blox_GNSS_v3 for UBX-CFG-VALGET & UBX-CFG-VALSET binary commands.
 * BTW, where is startRoverRTCM?
 *  No need. serialRadio(serial2) -> serialRTCM(serial0) is only an RTCM relay. It does not use the GNSS library.
 *  serialRTCM(serial0) on the ESP32 is wired (white & blue wires) to UART2 on the RTK-SMA.
 *  Also no need for the GNSS library to configure the RTK-SMA. The ZED-9FP default for UART2 is RTCM in.
 *
 * @return void No output is returned.
 * @since  0.1.0 [2025-04-24-12:00pm] New.
 * @since  0.3.6 [2025-05-07-02:00pm] Refactored.
 * @see    Global vars: Serial(0) - GNSS(RTCM) out.
 * @see    Global vars: Serial(1) - GNSS(UBX).
 * @see    Global vars:  in.
 * @see    Global vars: GNSS.
 * @see    beginSerialRTCM(serial0).
 * @see    beginSerialRadio(Serial2).
 * @see    beginSerialUBX(serial1).
 */
void startUBX() {

    // -- Start GNSS over serial. --
    if (roverGNSS.begin(serialUBX) == false) {      // Bind serial1 to SFE_UBLOX_GNSS_SERIAL object.
        Serial.println("Start roverGNSS (using UART1 for UBX CFG-VAL-SET/VAL-GET) - failed. Freezing ...");
        while (true);                               // Infinite loop.
    }

    // -- Print status. --
    Serial.println("Start roverGNSS (using UART1 for UBX CFG-VAL-SET/VAL-GET).");

    // -- Config GNSS over serial. --
    // roverGNSS.connectedToUART2();                // Not needed.
    roverGNSS.setAutoPVT(true);                     // Output solutions periodically (default 1HZ).
    roverGNSS.setUART1Output(COM_TYPE_UBX);         // Set UART1 for only UBX messages.
    // roverGNSS.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT);    // Save port settings to flash and BBR.
    roverGNSS.setNavigationFrequency(1);         // Two solutions (2 HZ) per second.

    // -- Print state. --           
    Serial.println("Config roverGNSS for only UBX.");       // COM_TYPE_UBX | COM_TYPE_NMEA | COM_TYPE_RTCM3.
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
 * @see    Global vars: Task handles.
 * @see    checkLockButton(), updateUI().
 * @link   https://www.freertos.org/Documentation/02-Kernel/04-API-references/01-Task-creation/01-xTaskCreate.
 */
void startTasks() {

    // -- Create Tasks. --
    xTaskCreate(serialLEDtaskRTCM,         "serial_led_task_RTCM",          2048, NULL, 2, &serialLEDtaskRTCMHandle);
    xTaskCreate(gnssLEDtaskGNSSfix,        "gnss_led_task_GNSS_fix",        2048, NULL, 2, &gnssLEDtaskGNSSfixHandle);
    xTaskCreate(gnssLEDtaskRTKfloat,       "gnss_led_task_RTK_float",       2048, NULL, 2, &gnssLEDtaskRTKfloatHandle);
    xTaskCreate(gnssLEDtaskRTKfix,         "gnss_led_task_RTK_fix",         2048, NULL, 2, &gnssLEDtaskRTKfixHandle);
    // xTaskCreate(serialSerialtaskRelayRTCM, "serial_serial_task_relay_RTCM", 2048, NULL, 2, &serialSerialtaskRelayRTCMHandle);
    xTaskCreate(serialBLEtaskRelayNEMA,    "serial_ble_task_relay_NEMA",    2048, NULL, 2, &serialBLEtaskRelayNEMAHandle);
    
     // -- Suspend tasks. --
    vTaskSuspend(serialLEDtaskRTCMHandle);
    vTaskSuspend(gnssLEDtaskGNSSfixHandle);
    vTaskSuspend(gnssLEDtaskRTKfloatHandle);
    vTaskSuspend(gnssLEDtaskRTKfixHandle);

    // -- Print status. --
    Serial.println("Task created: serial LED RTCM.");
    Serial.println("Task created: GNSS   LED GNSS fix.");
    Serial.println("Task created: GNSS   LED RTK float");
    Serial.println("Task created: GNSS   LED RTK fix.");
    // Serial.println("Create task: serial to serial relay RTCM.");
    Serial.println("Task created: serial to BLE serial relay NMEA.");
}

/**
 * ------------------------------------------------
 *      Start UI.
 * ------------------------------------------------
 *
 * Last step in setup before loop().
 *
 * @param  array $*list List of items.--
 * @throws Exception Ignore.--
 * @return void No output is returned.--
 * @since  0.4.2 [2025-05-15] New.--
 * @see    cnv_post_loaded() in convinsys.php.--
 * @link   https://docs.phpdoc.org/guide/getting-started/what-is-a-docblock.html.--
 */
void startUI() {

    // -- Display serial (USB) startup message. --
    Serial.printf("\nGhost Rover - Version %c.%c.%c @ %s\n\n", MAJOR_VERSION, MINOR_VERSION, PATCH_VERSION, BUILD_DATE);

    // -- Check lock button. --
    checkLockButton();              // Check GNSS lock button.

    // -- Set rover mode. --
    pgmState[1] = 'b';              // Assume rover will be in RTK base mode.
    if (UIstate[0] == '1') {        // GNSS lock button is in downPosition.
        pgmState[1] = 'p';          // Set state to RTK PointPerfect.
    }

    // -- Display OLED startup message. --
    updateOLED('0');                // Must be after checkLockButton().

    // -- Verify LEDs.
    updateLEDs('1','1','1');        // All LEDs on.
    delay(2000);                    // Pause & show startup UI for 3 seconds.
    updateLEDs('0','0','0');        // All LEDs off.
}

// --- Check. ---

/**
 * ------------------------------------------------
 *      Check serial monitor.
 * ------------------------------------------------
 *
 * @return void No output is returned.
 * @since  0.1.0 [2025-04-28-11:00am] New.
 * @since  0.2.0 [2025-04-29-05:30pm] Refactored.
 * @since  0.3.7 [2025-05-09-04:30pm] Add loop() throttle.
 * @since  0.3.8 [2025-05-10-09:30am] Set state.
 * @since  0.3.9 [2025-05-11-10:45am] Refactored "which command".
 * @since  0.4.3 [2025-05-15-05:00pm] Added testLEDr, testLEDg, testLEDl.
 */
void checkSerialMonitor(char print = ' ') {

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

    while (Serial.available() > 0) {    // Input from serial monitor.

        // -- Read serial input. --
        inputCharMon = Serial.read();  // Read byte in from USB serial.

        if (inputCharMon != '\n' && (posnMon < (sizeof(monitorCommand) - 1))) {    // Are we done?
            monitorCommand[posnMon] = inputCharMon;     // Not done yet, add char to command.
            posnMon++;                                  // Increment command buffer posn.
        } else {
            monitorCommand[posnMon] = '\0';             // We're done reading, treat command[] as C-string.
            posnMon = 0;                                // Reset command buffer position.

        // -- Which command? --
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

        // -- Valid command? --
        if(whichMonitorCommand < 99) {   
            // - Print the validated command. -
            // Serial.printf("%s\n", commands[whichMonitorCommand]);  
            // - Toggle command flag & print new state. -
            switch (whichMonitorCommand) {

                case (0):
                    testLEDr = (testLEDr == true) ? false : true;           // Flip the state.
                        Serial.printf("%s %s\n", commands[whichMonitorCommand], (testLEDr  ? "enabled." : "disabled."));
                        break;
                    case 1:
                        testLEDg = (testLEDg == true) ? false : true;       // Flip the state.
                        Serial.printf("%s %s\n", commands[whichMonitorCommand], (testLEDg  ? "enabled." : "disabled."));
                        break;
                    case 2:
                        testLEDl = (testLEDl == true) ? false : true;       // Flip the state.
                        Serial.printf("%s %s\n", commands[whichMonitorCommand], (testLEDl  ? "enabled." : "disabled."));
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
                checkSerialMonitor('p');
            }

            // - Reset MCU. -
            if (reset) {
                Serial.println("Restarting ...");
                esp_restart();
            }

            // - Test LED. -
            if ((testLEDr) || (testLEDg) || (testLEDl)) {

                // Test mode.
                Serial.print("All LEDs off. ");
                updateLEDs('0','0','0');                        // All LEDs off.
                Serial.println("OLED updated.");
                updateOLED('4');                                // Test mode display.
            }

            // - Test serial (radio) LED. -
            if (testLEDr) {

                // Display instructions.
                Serial.printf("Valid options: 0(off), 1(on), 2(active). %c to quit.\n", EXIT_TEST);

                // Loop.
                while (true) {                                  // Infinite loop.
                    if (Serial.available() > 0) {
                        inputCharMon = Serial.read();              // Read input from serial monitor.
                        Serial.read();                          // Discard newline.
                        switch (inputCharMon) {
                            case EXIT_TEST:                     // All done.
                                Serial.println("testLEDr disabled.");
                                testLEDr = false;               // Clear test flag.
                                return;                         // Exit test mode.
                            case '0':                           // serial (radio) LED - off.
                                Serial.printf("%c - serial (radio) LED - off.\n", inputCharMon);
                                updateLEDs('0','-','-');
                                break;
                            case '1':                           // serial (radio) LED - on.
                                Serial.printf("%c - serial (radio) LED - on.\n", inputCharMon);
                                updateLEDs('1','-','-');
                                break;
                            case '2':                           // serial (radio) LED - active.
                                Serial.printf("%c - serial (radio) LED - active (5 cycles).\n", inputCharMon);
                                for (size_t i = 0; i < 5; i++) {
                                    updateLEDs('2','-','-');
                                    Serial.println("Blink LED: option = 2(active)");
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
                                Serial.printf("%c - GNSS LED - off.\n", inputCharMon);
                                updateLEDs('-','0','-');
                                break;
                            case '1':                                       // GNSS LED on.
                                Serial.printf("%c - GNSS LED - on.\n", inputCharMon);
                                updateLEDs('-','1','-');
                                break;
                            case '2':                                       // GNSS LED GNSS fix.
                                Serial.printf("%c - GNSS LED - GNSS fix (5 cycles).\n", inputCharMon);
                                for (size_t i = 0; i < 5; i++) {
                                    updateLEDs('-','2','-');
                                    Serial.println("Blink LED: option = 2(GNSS fix)");
                                    delay(1000);
                                }
                                Serial.println();
                                break;   
                            case '3':                                       // GNSS LED RTK float.
                                Serial.printf("%c - GNSS LED - RTK float (5 cycles).\n", inputCharMon);
                                for (size_t i = 0; i < 5; i++) {
                                    updateLEDs('-','3','-');
                                    Serial.println("Blink LED: option = 3(RTK float)");
                                    delay(1000);
                                }
                                Serial.println();
                                break;
                            case '4':                                       // GNSS LED RTK fix.
                                Serial.printf("%c - GNSS LED - GNSS fix( 5 cycles).\n", inputCharMon);
                                for (size_t i = 0; i < 5; i++) {
                                    updateLEDs('-','4','-');
                                    Serial.println("Blink LED: option = 4(RTK fix)");
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

            // - Test GNSS lock LED. -
            if (testLEDl) {

                // Display instructions.
                Serial.printf("Valid options: 0(off), 1(on). %c to quit.\n", EXIT_TEST);

                // Loop.
                while (true) {                                              // Infinite loop.
                    if (Serial.available() > 0) {
                        inputCharMon = Serial.read();                       // Read input from serial monitor.
                        Serial.read();                                      // Discard newline.
                        switch (inputCharMon) {
                            case EXIT_TEST:                                 // All done.
                                Serial.println("testLEDl disabled.");
                                testLEDl = false;                           // Clear test flag.
                                return;                                     // Exit test mode.
                            case '0':                                       // Radio LED off.
                                Serial.printf("%c - GNSS lock LED - off.\n", inputCharMon);
                                updateLEDs('-','-','0');
                                break;
                            case '1':                                       // Radio LED on.
                                Serial.printf("%c - GNSS lock LED - on.\n", inputCharMon);
                                updateLEDs('-','-','1');
                                break;
                            default:
                                Serial.printf("Valid options: 0(off), 1(on). %c to quit.\n", EXIT_TEST);
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
                updateLEDs('0','-','-');                    // serial (radio) LED off - AT command mode.
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
            // @see checkDebug().
            // @see checkRadioToRTCM().
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
 */
void checkLockButton() {

    // -- Set state of GNSS lock button. --
    if (digitalRead(BUTTON_LOCK) == HIGH) {
        updateLEDs('-','-','0');                // Update lock button LED.
        UIstate[0] = '0';                       // GNSS lock button is in upPosition.
    } else {
        updateLEDs('-','-','1');                // Update lock button LED.
        UIstate[0] = '1';                       // GNSS lock button is in downPosition.
    }
}

/**
 * ------------------------------------------------
 *      Check for new GNSS data.
 * ------------------------------------------------
 * 
 * Uses SparkFun_u-blox_GNSS_v3 library.
 * Fix type: 0=none, 1=dead reckoning, 2=2D, 3=3D, 4=GNSS, 5=Time.
 * Carrier solution: 0=none, 1=RTK float, 2=RTK fixed.
 *
 * @return void No output is returned.
 * @since  0.1.0 [2025-04-24-12:00pm] New.
 * @since  0.1.0 [2025-04-29-10:45am] Antenna check.
 * @since  0.3.1 [2025-04-30-01:30pm] updateLEDs.
 * @since  0.3.3 [2025-05-02-09:45am] Refactor.
 * @since  0.3.7 [2025-05-09-04:30pm] Add loop() throttle.
 * @since  0.3.8 [2025-05-10-09:30am] Set state.
 * @since  0.3.8 [2025-05-10-09:45am] Moved debug to checkDebug().
 * @since  0.4.4 [2025-05-15-06:30pm] Changed conditional structure.
 * @link   https://www.sparkfun.com/sparkfun-gnss-combo-breakout-zed-f9p-neo-d9s-qwiic.html.
 */
void checkGNSS() {

    if (SIV < MIN_SATELLITE_THRESHHOLD) {                   // Enough satellites?

        // -- Set state. --
        serState[2] = 'd';                                  // UBX data (serial1) down.
        updateLEDs('-','1','-');
    } 
    
    if (roverGNSS.getPVT() == true) {                       // New GNSS info is available.

       // -- Set state. --
        serState[2] = 'u';                                  // UBX data (serial1) up.

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
 *      Check for RTCM data. Send from radio to RTK-SMA.
 * ------------------------------------------------
 *
 * @return void No output is returned.
 * @since  0.3.6 [2025-05-07-03:45] New.
 * @since  0.3.7 [2025-05-09-04:30pm] Add loop() throttle.
 * @since  0.4.5 [2025-05-15-05:00pm] Refactor.
 * @see    beginSerialRTCM(), beginSerialRadio().
 * @link   https://github.com/sparkfun/SparkFun_u-blox_GNSS_v3/blob/main/examples/ZED-F9P/Example3_StartRTCMBase/Example3_StartRTCMBase.ino.
 */
void checkRadioToRTCM() {
    // TODO: 1. checkRadioToRTCM() - Implement out to serialRTCM, move to ESP32 task.
    
    // -- Local vars. --
    static bool gotBits = false;                    // Flag - if input since boot.

    // -- Check for Radio down. Set state. --
    if (gotBits) {                                  // Input has been received since boot.
        if((esp_timer_get_time()-lastRTCMtime) > RADIO_TIMEOUT) {
            serState[3] = 'd';                      // serial (radio) data (serial2) - down.
            serState[1] = 'd';                      // RTCM data (serial0) down.
        }
    }

    // -- Read radio input. Write to RTK-SMA --
    while(serialRadio.available() > 0) {            // serial (radio) data to read?

        // - Read input. -
        inputCharRTCM = serialRadio.read();         // Read a character @ HC12_SPEED.

        // Need somekind of line break, RTCM confirmation from ZED-9FP.
        // Integrate to updateUI.
        if (inputCharRTCM == '}') {                 // eosRTCM = '\n'.
            if (debugRad) {
                Serial.println(inputCharRTCM);      // Last charecter.
            }
            serState[3] = 'u';                      // serial (radio) data (serial2) - up.
            serState[1] = 'u';                      // RTCM data (serial0) - up.
            lastRTCMtime = esp_timer_get_time();    // Used to check for timeout.
            gotBits = true;                         // Flag for iniitial timeout.
            updateLEDs('2','-','-');                // serial (radio) LED - active.
            // serialRTCM.write(inputChar);             // Write a character @ ZED_SPEED.

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
 * @see    checkSerialMonitor().
 */
void checkDebug() {

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

    // -- Debug output. Serial data will be displayed by checkRadioToRTCM() in real time. --
    if (debugRad) {
        nowTime = esp_timer_get_time();
        diffTime = nowTime - lastRTCMtime;
            if (diffTime > RADIO_TIMEOUT) {
                Serial.printf("\nRadio down. (diffTime= %lld)  >  (timeout= %lld)\n", diffTime, RADIO_TIMEOUT);
            }
    }

    // -- GNSS. --
    if (debugGNSS) {
        // roverGNSS.enableDebugging();
        Serial.printf(
        "Lat= %lld(-7)  Lon= %lld(-7)  Alt= %lld(-3)  Hac= %ld(-3)  Vac= %ld(-3)  SIV= %i  Fix= %i  Soln= %i\n",
        latitude, longitude, altitude, horizontalAcc, verticalAcc, SIV, fixType, solnType );
    }

    // -- GNSS lock button.- -
    if (debugBtn)  {
        if (UIstate[0] == '0') {
            Serial.println("GNSS lock button position = up.");
        }
        if (UIstate[0] == '1') {
            Serial.println("GNSS lock button position = down.");
        }
    }

    // -- BLE. --
    if (debugBLE)  {
        Serial.printf("BLE Hello %i\n", bleCount);
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
        // Program section (-,s,l).
        // Rover mode (-,b,p).
        Serial.printf("PgmSection= %c  RoverMode= %c\n", pgmState[0], pgmState[1]);     
    }

    // -- Serial state. --
    if (debugSer) {
        Serial.printf(
            "USB= %c  RTCM(serial0)= %c  UBX(serial1)= %c  Radio(serial2)= %c\n",
            serState[0],        // USB Monitor (-,d,u).
            serState[1],        // RTCM data (serial0) (-,d,u).
            serState[2],        // UBX data (serial1) (-,d,u).
            serState[3]         // Radio data (serial2) (-,d,u)
            );
    }

    // -- UI state. --
    if (debugUI) {
        Serial.printf(
            "Button= %c  Radio LED= %c  GNSS LED= %C  GNSS lock LED= %C\n",
            UIstate[0],         // GNSS lock button (0,1).
            UIstate[1],         // serial (radio) LED (0,1,2).
            UIstate[2],         // GNSS LED (0,1,2,3,4).
            UIstate[3]          // GNSS lock LED (0,1).
        );
    }

    // -- Network state. --
    if (debugNet) {
        // BLE data (d,u).
        // WiFi data (d,u).
        Serial.printf("BLE= %c  WiFi= %c\n", netState[0], netState[1]);
    }
}

// --- Display. ---

/**
 * ------------------------------------------------
 *      Toggle LEDs.
 * ------------------------------------------------
 *
 * @param  char ledR serial (radio) LED.
 * @param  char ledG GNSS LED.
 * @param  char ledL GNSS lock LED.
 * @return void No output is returned.
 * @since  0.2.0 [2025-04-30-11:15am] New.
 * @since  0.4.2 [2025-05-15-08:00am] Refactor.
 * @link   https://www.freertos.org/Documentation/02-Kernel/04-API-references/02-Task-control/06-vTaskSuspend.
 * @link   https://www.freertos.org/Documentation/02-Kernel/04-API-references/02-Task-control/07-vTaskResume.
 */
void updateLEDs(char ledR, char ledG, char ledL) {


    // -- Set LEDs. --

    // - serial (radio) LED. -
    switch (ledR) {
        case '0':
            UIstate[1] = '0';                       // serial (radio) LED - off.
            digitalWrite(LED_RADIO, LOW);           // LED off.
            break;
        case '1':
            UIstate[1] = '1';                       // serial (radio) LED - on.
            digitalWrite(LED_RADIO, HIGH);          // LED on.
            break;
        case '2':
            UIstate[1] = '2';                       // serial (radio) LED - active.
            vTaskResume(serialLEDtaskRTCMHandle);   // Resume serialLEDtaskRTCM() task.
            break;
    }

    // - GNSS LED. -
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
            UIstate[2] = '2';                           // GNSS LED - GNSS fix.
            vTaskResume(gnssLEDtaskGNSSfixHandle);      // Resume task.
            break;
        case '3':
            UIstate[2] = '3';                           // GNSS LED - RTK float.
            vTaskResume(gnssLEDtaskRTKfloatHandle);     // Resume task.
            break;
        case '4':
            UIstate[2] = '4';                           // GNSS LED - RTK fix.
            vTaskResume(gnssLEDtaskRTKfixHandle);       // Resume task.
            break;
    }

    // - GNSS lock LED. -
    if (ledL != '-') {
        switch (ledL) {
            case '0':
                UIstate[3] = '0';                   // GNSS lock LED - off.
                digitalWrite(LED_LOCK, LOW);        // LED off.
                break;
            case '1':
                UIstate[3] = '1';                   // GNSS lock LED - on.
                digitalWrite(LED_LOCK, HIGH);       // LED on.
                break;
        }
    }
}

/**
 * ------------------------------------------------
 *      Display OLED.
 * ------------------------------------------------
 * 
 * Uses SparkFun_Qwiic_OLED library.
 * 8x16 font size yields (5) rows with (16) characters each.
 * oledRow[5][17] is defined [17] to include null terminator.
 * Assumptions:
 *  1. OLED object (roverOLED) has been created using Qwiic1in3OLED.
 *  2. I2C (Wire) has been started.
 *  3. OLED object (roverOLED) has been bound to I2C (Wire).
 *  4. OLED object (roverOLED) has font set.
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

// --- Operation. ---

// --- Tasks. ---

/**
 * ------------------------------------------------
 *      Serial (radio) to serial (RTCM) relay - LED task.
 * ------------------------------------------------
 *
 * @param  void * pvParameters Pointer to task parameters.
 * @return void No output is returned.
 * @since  0.3.3 [2025-05-02-09:00am] New.
 * @since  0.4.5 [2025-05-17-11:15am] Refactor.
 * @see    startTasks().
 * @see    updateLEDs().
 * @link   https://www.freertos.org/Documentation/02-Kernel/04-API-references/02-Task-control/01-vTaskDelay.
 * @link   https://www.freertos.org/Documentation/02-Kernel/05-RTOS-implementation-tutorial/02-Building-blocks/11-Tick-Resolution.
 * @link   https://www.freertos.org/Documentation/02-Kernel/04-API-references/02-Task-control/06-vTaskSuspend.
 */
void serialLEDtaskRTCM(void * pvParameters) {
    while(true) {
        digitalWrite(LED_RADIO, HIGH);                      // LED on.
        vTaskDelay(pdMS_TO_TICKS(LED_TIME_FLASH_ON));       // LED remains on.
        digitalWrite(LED_RADIO, LOW);                       // LED off.
        vTaskDelay(pdMS_TO_TICKS(500));                     // LED remains on.
        vTaskSuspend(NULL);                                 // Suspend task.
    }
}

/**
 * ------------------------------------------------
 *      LED flash effect.
 * ------------------------------------------------
 *
 * @param  array $*list List of items.
 * @return void No output is returned.
 * @since  0.8.7 [2025-05-17-02:15pm].
 * @see    updateLEDs().
 * @see    startTasks().
 * @see    gnssLEDtaskGNSSfix().
 * @see    gnssLEDtaskRTKfloat().
 * @see    gnssLEDtaskRTKfix().
 * @link   https://www.freertos.org/Documentation/02-Kernel/04-API-references/02-Task-control/01-vTaskDelay.
 * @link   https://www.freertos.org/Documentation/02-Kernel/05-RTOS-implementation-tutorial/02-Building-blocks/11-Tick-Resolution.
 */
void cycleGNSSLed() {
    digitalWrite(LED_GNSS, HIGH);                       // LED on.
    vTaskDelay(pdMS_TO_TICKS(LED_TIME_FLASH_ON ));      // LED remains on.
    digitalWrite(LED_GNSS, LOW);                        // LED off.
    vTaskDelay(pdMS_TO_TICKS(LED_TIME_FLASH_ON));       // Delay.
}

/**
 * ------------------------------------------------
 *      GNSS - GNSS fix LED task.
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
void gnssLEDtaskGNSSfix(void * pvParameters) {
    while(true) {                                   // Infinite loop.
        for (size_t i = 0; i < 1; i++) {            // Repeat one time.
            cycleGNSSLed();
        }
        vTaskSuspend(NULL);                         // Suspend task.
    }
}

/**
 * ------------------------------------------------
 *      GNSS - GNSS RTK float LED task.
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
void gnssLEDtaskRTKfloat(void * pvParameters) {
    while(true) {                                   // Infinite loop.
        for (size_t i = 0; i < 2; i++) {            // Repeat two times.
            cycleGNSSLed();
        }
        vTaskSuspend(NULL);                         // Suspend task.
    }
}

/**
 * ------------------------------------------------
 *      GNSS - GNSS RTK fix LED task.
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
void gnssLEDtaskRTKfix(void * pvParameters) {
    while(true) {                                   // Infinite loop.
        for (size_t i = 0; i < 3; i++) {            // Repeat three times.
            cycleGNSSLed();
        }
        vTaskSuspend(NULL);                         // Suspend task.
    }
}

/**
 * ------------------------------------------------
 *      UBX data (serial1) to BLE serial - relay NEMA task.
 * ------------------------------------------------
 *
 * @return void No output is returned.
 * @since  0.3.4 [2025-05-06-02:30pm] New.
 * @since  0.3.7 [2025-05-09-01:30pm] Moved counter to local.
 * @since  0.4.5 [2025-05-17-09:30pm] Moved to task.
 * @see    startTasks().
 * @link   https://github.com/avinabmalla/ESP32_BleSerial.
 * @link   https://www.freertos.org/Documentation/02-Kernel/04-API-references/02-Task-control/01-vTaskDelay.
 * @link   https://www.freertos.org/Documentation/02-Kernel/05-RTOS-implementation-tutorial/02-Building-blocks/11-Tick-Resolution.
 */
void serialBLEtaskRelayNEMA(void * pvParameters) {
    // TODO: 2. Finish relay task.

    // -- Test loop. --
    while(true) {                               // Infinite loop.

        // -- Send message out BLE serial. --
        ble.printf("Hello %i", bleCount);

        // -- Count & pause. --
        bleCount++;
        vTaskDelay(pdMS_TO_TICKS(1000));        // Broadcast every 1,000 ms.
    }
}

// --- Test. ---

// ===================================
//               Setup.
// ===================================

void setup() {
    initVars();                 // Initialize vars.
    beginSerialMonitor();       // Begin serial (USB for monitor), print version.
    configPins();               // Initialize pin modes & pin values.
    beginSerialRTCM();          // Begin serial (UART0 for RTCM).
    beginSerialUBX();           // Begin serial (UART1 for UBX).
    beginSerialRadio();         // Begin serial (UART2 for HC-12 radio).
    beginI2C();                 // Begin I2C.
    startDisplay();             // Start OLED display.
    startBLE();                 // Begin & start BLE.
    startWiFi();                // Start WiFi.
    startUBX();                 // Start & config for UBX GNSS.
    startTasks();               // Start tasks (LED, BLE test).
    startUI();                  // Start rover UI.
}                                               

// =================================== 
//               Loop.
// ===================================

void loop() {
    checkSerialMonitor();       // Check for serial input.
    checkRadioToRTCM();         // Check for RTCM data. Send from radio to RTK-SMA.
    checkLockButton();          // Check GNSS lock button.
    checkGNSS();                // Check for new GNSS data.
    checkDebug();               // Check to display debug.
}
