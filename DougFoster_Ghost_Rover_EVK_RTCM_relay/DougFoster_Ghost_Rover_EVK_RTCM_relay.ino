/**
 * *************************************************************************
 *  Ghost Rover 3 - RTCM relay from EVK ZED-F9P to HC-12 serial RF radio.
 * *************************************************************************
 * 
 * @author D. Foster <doug@dougfoster.me>.
 * @since  3.0.10 [2025-12-30-06:00pm] New.
 * @since  3.2.1  [2026-07-27-11:45pm] Regroup. Cleanup.
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
 * @since 3.1.1 [2026-06-26-11:00am] New.
 * 
 * --- Comments. ---
 * --- Code structure. ---
 * --- Code operation. ---
 * --- Board LED status. ---
 * --- ESP32 (Arduino framework) data types. ---
 * --- Other doc. ---
 */

/**
 * -------------------------------------------------------------------------
 *  Comments.
 * -------------------------------------------------------------------------
 * 
 * @since 3.1.1  [2026-06-26-10:00am] New.
 * @link https://github.com/doug-foster/DougFoster_Ghost_Rover.
 * 
 * --- Description & operation. ---
 *     -- A SparkFun EVK is configured to operate as an assisted base GNSS station. The base has a companion GNSS
 *        rover - Ghost Rover. This sketch transmits RTCM3 correction data from the base to the rover.
 * 
 *        Inside the EVK enclosure, a SparkFun Thing Plus ESP32-C6 and an HC-12 RF radio (powered by a Qwiic I2C bus
 *        connection inside the EVK - power only, no data) have been added.
 *
 *        When the EVK is in base mode and a fix has been obtained, the EVK's ZED-F9P GNSS processor (UART2) will send
 *        out a serial stream of RTCM3 correction data via a terminal block mounted on the back panel of the EVK.
 *        The ESP32-C6 is connected to the TX2 lug on this terminal block.
 *
 *        In the loop(), data is read byte-by-byte from the ZED-F9P UART2 by checkRTCMtoRadio() and transfered to
 *        the HC-12. The HC-12 transmits the serial RTCM3 stream over RF to the rover's receiving HC-12.
 *
 *        An LED mounted on the EVK back panel blinks once for every RTCM3 sentence transmitted.
 *
 * --- Major components. ---
 *     -- EVK   https://www.sparkfun.com/sparkfun-rtk-evk.html.
 *     -- MCU   https://www.sparkfun.com/sparkfun-thing-plus-esp32-c6.html.
 *     -- Radio (433.4-473.0 MHz, 100mW, U.FL) https://www.amazon.com/HiLetgo-Wireless-Replace-Bluetooth-Antenna/dp/B01MYTE1XR.
 *     -- Rover https://github.com/doug-foster/DougFoster_Ghost_Rover/.
 *
 * --- Other components. ---
 *     -- Radio antenna. --
 *        - UHF 400-960 MHz, BNC-M: https://www.amazon.com/dp/B07R4PGZK3.
 *        - cable (BNC-F bulkhead to U.FL, 8" RG178): https://www.amazon.com/dp/B098HX6NFH.
 *     -- Misc. --
 *        - LED cover (5mm LED bulb socket): https://www.amazon.com/dp/B07CQ6TH14.
 *
 * --- Misc. references. ---
 *     -- RTCM        https://www.use-snip.com/kb/knowledge-base/an-rtcm-message-cheat-sheet/.
 *     -- HC-12       https://www.elecrow.com/download/HC-12.pdf.
 *     -- EVK         https://docs.sparkfun.com/SparkFun_RTK_Everywhere_Firmware/menu_base/#rtcm-message-rates.
 *     -- SparkFun    https://learn.sparkfun.com/tutorials/tags/gnss.
 * 
 * --- Dev environment. ---
 *     -- IDE         VS Code & Arduino Maker Workshop 1.1.8 extension (uses Arduino CLI 1.2.0).
 *     -- Platform    https://github.com/espressif/arduino-esp32/releases/latest (Arduino Release v3.3.10 based on ESP-IDF v5.5.4).
 * 
 * --- Caveats. ---
 *     -- No known.
 *
 * --- TODO: ---
 *     -- No todo items.
 */

 /**
 * -------------------------------------------------------------------------
 *  Code structure.
 * -------------------------------------------------------------------------
 * 
 * @since 3.1.1 [2026-06-25-01:00pm] New.
 *
 *  --- Docs. ---
 *  --- Include libraries. ---
 *      -- Core.
 *      -- Additional.
 *  --- Global vars.---
 *      -- Pin assignments.
 *      -- LED.
 *      -- BLE (Bluetooth Low Energy).
 *      -- Task handles.
 *      -- Operation.
 *      -- Declaration.
 *      -- Test.
 *  --- General functions. ---
 *      -- statusLedOn()          - Turn on status LED.
 *  --- Setup functions. ---
 *      -- showBuild()            - Display build & processor info.
 *      -- startSerial()          - Start serial interfaces.
 *      -- initVars()             - Initialize global vars.
 *      -- initPins()             - Initialize pins & pin values.
 *      -- startI2C()             - Start I2C wire interfaces.
 *      -- startTasks()           - Start tasks.
 *      -- startLoop()            - Start loop().
 *  --- Task functions. ---
 *      -- radioRtcmLEDtask()     - Check serial 1 (EVK-RTCM). Send to serial 2 (HC-12 radio).
 *  --- Event handlers. ---
 *  --- Loop functions. ---
 *      -- checkSerialUSB()       - Check serial USB for input.
 *      -- checkRTCMtoRadio()     - Check if last NMEA sent to MCU #2 was within NMEA_TIMEOUT.
 *      -- rtcm3GetMessageType()  - Return RTCM3 message type.
 *      -- updateLED              - Toggle LEDs.
 *      -- debug()                - Display debug.
 *  --- Setup. ---
 *  --- Loop. ---
 */

 /**
 * -------------------------------------------------------------------------
 *  Code Operation.
 * -------------------------------------------------------------------------
 *
 * @since 3.1.1 [2026-06-26-11:00am] New.
 *
 * --- Boot. ---
 *     Include libraries.
 *     Define global vars.
 *     Define general functions.
 *     Define setup() functions.
 *     Define task functions.
 *     Define event handlers.
 *     Define loop() functions.
 * --- Run setup(). ---
 *     showBuild()                  // Display build & processor info.
 *     startSerial()                // Start serial interfaces.
 *     initVars()                   // Initialize global vars.
 *     initPins()                   // Initialize pins & pin values.
 *     startTasks()                 // Start tasks.
 *     startLoop()                  // On to loop().
 * --- Run loop(). ---    
 *     checkSerialUSB()             // Check serial USB for input.
 *     checkNMEAin()                // Check if last NMEA received from MCU #2 was within NMEA_TIMEOUT.
 *     debug()                      // Display debug.
 * --- Tasks. ---
 *     checkRTCMtoRadio()           // Check if last NMEA sent to MCU #2 was within NMEA_TIMEOUT.
 * --- Event handlers. ---
 */

 /**
 * -------------------------------------------------------------------------
 *  Board LED status.
 * -------------------------------------------------------------------------
 * N/A.
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
 *     float                        %f        32 bits = 4 bytes,   6-7 sig. digits (hardware),  -3.40e+38 to 3.40e+38).
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
 * =========================================================================
 *  Include libraries.
 * =========================================================================
 *
 * @since                   3.0.9 [2025-12-17-06:00pm] New.
 * @link  Arduino           https://docs.arduino.cc/libraries/.
 * @link  ESP32             https://docs.espressif.com/projects/arduino-esp32/en/latest/libraries.html.
 */

// --- Core. ---
#include <Arduino.h>            // https://github.com/espressif/arduino-esp32.
#include <esp_system.h>         // https://github.com/pycom/esp-idf-2.0/blob/master/components/esp32/include/esp_system.h.
#include <esp_chip_info.h>      // https://github.com/pycom/pycom-esp-idf.

// --- Additional. ---

/**
 * ============================================================================
 *  Global vars.
 * ============================================================================
 *
 * @since  3.0.9 [2025-12-15-06:30pm] New.
 */

// --- Pin asignments. ---

// -- Serial0 (UART0). --
const uint8_t RTCM_IN  = 5;                 // ZED TX2 (RTCM) {green wire}.
const uint8_t RTCM_OUT = 4;                 // ZED RX2 (not used) <- RTCM {yellow wire}.

// -- Serial1 (UART1). --   
const uint8_t HC12_TX  = 16;                // HC-12 TXD {yellow wire}.
const uint8_t HC12_RX  = 17;                // HC-12 RXD {white wire}.
const uint8_t HC12_SET =  2;                // HC-12 SET {blue wire}.

// -- LED. --
const uint8_t LED_RADIO = 3;                // Red LED {blue wire}.

// --- Serial. ---
const uint32_t SERIAL_USB_SPEED = 115200;   // Serial USB speed.
const uint32_t SERIAL0_SPEED    = 57600;    // ZED default speed.
const uint32_t SERIAL1_SPEED    = 9600;     // HC-12 default speed.
char  monitorChar;                          // Monitor i/o character.  // ToDo.
char  serialChar;                           // Serial i/o character.
char  rtcmSentence[300];                    // RTCM3 sentence buffer.

// --- I2C. ---
// Power only.

// --- Timing. ---
const TickType_t LED_TIME_FLASH_ON = 100/portTICK_PERIOD_MS;  // Time (ms).

// --- Task handles. ---
TaskHandle_t radioRtcmLEDtaskHandle;            // Radio RTCM LED task handle.

// --- Operation. ---
enum CommandIndex {                                       //  Readable index for command array.
    TEST_RAD = 0,                                         //  0.
    TEST_LED,                                             //  1.
    DEBUG_RAD,                                            //  2.
    SHOW_UPTIME,                                          //  3.
    RESTART,                                              //  4.
    NUM_COMMANDS                                          //  5 = automatic array length.
};     
const char* COMMAND[NUM_COMMANDS] = {                     // Command strings; match CommandIndex.
    "testRad",                                            // TEST_RAD.
    "testLED",                                            // TEST_LED.
    "debugRad",                                           // DEBUG_RAD.
    "showUptime",                                         // SHOW_UPTIME.
    "restart"                                             // RESTART.
}; 
const char    EXIT_TEST              = '!';     // Exit test mode.
const char*   COMMANDS[NUM_COMMANDS] = {        // Valid commands. Point to array of C-strings.
    "testLEDr",
    "testRad",
    "debugRad",
    "reset"
};
bool     commandFlag[NUM_COMMANDS] = {false};      // Command flags.
bool     testLEDr;                                 // Test radio LED.
bool     testRad;                                  // Test radio.
bool     debugRad;                                 // Debug radio.
bool     reset;                                    // Reset MCU.
bool     inLoop;                                   // In loop indicator.
char     monitorCommand[11];                       // Serial monitor command (C-string). // ToDo.
char     radioCommand[11];                         // serial (radio) test command (C-string). // ToDo.
char     buildString[40]           = {'\0'};       // Build string (build version on date at time). e.g. 3.0.12 - Feb 19 2026 @ 12:23:13
char     uptime[20]                = {'\0'};       // 01h 03m 12s.
int64_t  startTime;                                // Boot time.
esp_chip_info_t chip_info;                      // Chip info.

// --- Version. ---
const char BUILD_DATE[]  = "[2025-12-30-06:30pm]";
const uint8_t MAJOR_VERSION             = 3;              // Current major build version (@see showBuild()).
const uint8_t MINOR_VERSION             = 2;              // Current minor build version (@see showBuild()).
const uint8_t PATCH_VERSION             = 1;              // Current patch build version (@see showBuild()).
const char NAME[]        = "Ghost Rover 3 - RTCM Relay";

// --- Declaration. ---
void updateLED(char);

// --- Test. ---

/**
 * =========================================================================
 *  Setup functions.
 * =========================================================================
 */

/**
 * ------------------------------------------------
 *      Display build & processor info.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.10 [2025-12-30-02:00pm]. New
 * @since  3.2.1  [2026-07-27-06:00pm]. Refactored.
 * @see    setup().
 */
void showBuild() {

    // --- Local vars. ---
    const char      NAME[]           = "Ghost Rover 3";
    const uint32_t  SERIAL_USB_SPEED = 115200;   // Serial USB speed.
    const uint64_t  START_DELAY      = 4000000;  // 4 second startup delay.
    esp_chip_info_t chip_info;

    // --- Run. ---
    startTime = esp_timer_get_time();
    Serial.begin(SERIAL_USB_SPEED);
    esp_chip_info(&chip_info);
    sprintf(buildString, "%u.%u.%u - %s @ %s", MAJOR_VERSION, MINOR_VERSION, PATCH_VERSION, __DATE__, __TIME__);
    while ((esp_timer_get_time() - startTime) < START_DELAY) {
        vTaskDelay(1);  // busy-wait; yield to RTOS if needed
    }
    Serial.print("\033[2J");   // Clear screen.
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
 * @return void No output is returned.
 * @since  3.0.9  [2025-12-14-02:00pm] New.
 * @since  3.0.10 [2025-12-30-02:00pm] Add Serial USB.
 * @see    setup().
 * @link   https://randomnerdtutorials.com/esp32-uart-communication-serial-arduino/#esp32-custom-uart-pins.
 */
void startSerial() {

    // --- Serial interface. ---
    Serial.println("Setup() started.");
    Serial.printf("Serial (USB) started @ %i bps.\n", SERIAL_USB_SPEED);

    // --- Serial0 interface. ---
    Serial.printf("Serial0 (ZED) started @ %i bps", SERIAL0_SPEED);
    Serial0.begin(SERIAL0_SPEED, SERIAL_8N1, RTCM_IN, RTCM_OUT);     // UART0 object. RX, TX.
    Serial.println(".");

    // --- Serial1 interface. ---
    Serial.printf("Serial1 (HC-12) started @ %i bps", SERIAL1_SPEED);
    Serial1.begin(SERIAL1_SPEED, SERIAL_8N1, HC12_RX, HC12_TX);     // UART1 object. RX, TX.
    Serial.println(".");
}

/**
 * -------------------------------------------------------------------------
 *  Initialize global vars.
 * -------------------------------------------------------------------------
 *
 * @return void No output is returned.
 * @since  3.0.9 [2025-12-15-06:30pm] New.
 * @see    setup().
 */
void initVars() {
    Serial.print("Init global vars");

    // --- I/O. ---
    serialChar = '\0';
    memset(monitorCommand, '\0', sizeof(monitorCommand));
    memset(radioCommand,   '\0', sizeof(radioCommand));
    memset(rtcmSentence,   '\0', sizeof(rtcmSentence));

    // --- Operation. ---
    inLoop  = false;

    // --- Commands. ---
    testLEDr = false;
    testRad  = false;
    debugRad = false;
    reset    = false;

    Serial.println(".");
}

/**
 * -------------------------------------------------------------------------
 *  Initialize pins & pin values.
 * -------------------------------------------------------------------------
 *
 * @return void No output is returned.
 * @since  3.0.9 [2025-12-14-02:00pm] New.
 * @see    setup().
 */
void initPins() {
    Serial.print("Init pins");
    pinMode(HC12_SET,  OUTPUT);                 // HC-12 - set pin for AT command mode.
    digitalWrite(HC12_SET,  HIGH);              // HC-12 - initially set pin for transparent mode.
    pinMode(LED_RADIO, OUTPUT);
    digitalWrite(LED_RADIO, LOW);
    Serial.println(".");
}

/**
 * -------------------------------------------------------------------------
 *  Start I2C Wire interfaces.
 * -------------------------------------------------------------------------
 *
 * Power only
 * 
 * @return void No output is returned.
 * @since  3.0.9 [2025-12-14-02:00pm] New.
 * @see    setup().
 */
void startI2c() {

    // --- Start interfaces. ---
    // --- Register event functions. ---
}

/**
 * -------------------------------------------------------------------------
 *  Start tasks.
 * -------------------------------------------------------------------------
 *
 * @return void No output is returned.
 * @since  3.0.9 [2025-12-14-02:00pm] New.
 * @see    Global vars: Task handles.
 * @see    setup().
 * @link   https://www.freertos.org/Documentation/02-Kernel/04-API-references/01-Task-creation/01-xTaskCreate.
 */
void startTasks() {

    // -- RTCM SEND status LED. --
    xTaskCreate(radioRtcmLEDtask,    "radio_RTCM_LED_task",       2048, NULL, 2, &radioRtcmLEDtaskHandle);
    vTaskSuspend(radioRtcmLEDtaskHandle);
    Serial.println("Task started: \"RTCM SEND status LED\".");
}

/**
 * -------------------------------------------------------------------------
 *  Start loop().
 * -------------------------------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.9 [2025-12-14-02:00pm] New.
 * @see    setup().
 */
void startLoop() {
    updateLED('0');     // RTCM LED off.
    inLoop = true;
    Serial.println("Loop() started.");
}   

/**
 * =========================================================================
 *  Task functions.
 * =========================================================================
 * @see startTasks()              - Start tasks.
 * @see radioRtcmLEDtask()      - Radio active LED.
 */

/**
 * -------------------------------------------------------------------------
 *  Task - Radio active LED.
 * -------------------------------------------------------------------------
 *
 * @param  void * pvParameters Pointer to task parameters.
 * @return void No output is returned.
 * @since  3.0.9 [2025-12-14-02:00pm] New.
 * @see    startTasks().
 * @link   https://docs.espressif.com/projects/esp-idf/en/v4.3/esp32/api-reference/system/freertos.html.
 * @link   https://www.freertos.org/Documentation/02-Kernel/04-API-references/02-Task-control/06-vTaskSuspend.
 */
void radioRtcmLEDtask(void * pvParameters) {
    while(true) {
        digitalWrite(LED_RADIO, HIGH);      // LED on.
        vTaskDelay(LED_TIME_FLASH_ON);      // LED remains on (ms).
        digitalWrite(LED_RADIO, LOW);       // LED off.
        vTaskSuspend(NULL);                 // Suspend task.
    }
}

/**
 * =========================================================================
 *  Loop functions.
 * =========================================================================
 */

/**
 * -------------------------------------------------------------------------
 *  Check serial USB for input.
 * -------------------------------------------------------------------------
 *
 * @return void No output is returned.
 * @since  3.0.9  [2025-12-17-06:00pm] New.
 * @since  3.0.10 [2025-12-30-01:15pm] Refactor.
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
            Serial.print("\nGR-MCU2:\n\"?\" Print commands.\n\"!\" Disable all debug.\nCommands:");
            for (size_t i = 0; i <= NUM_COMMANDS-1; i++) {
                Serial.printf(" %s", COMMAND[i]);
            }
            Serial.println('.');
        } else if ((command[0]) == '!') {                       // Disable all debugs.
            for (size_t i = 0; i <= NUM_COMMANDS; i++) {
                commandFlag[i] = false;
            }
            Serial.println("All debug disabled.");
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
 * -------------------------------------------------------------------------
 *  Check serial 1 (EVK-RTCM). Send to serial 2 (HC-12 radio).
 * -------------------------------------------------------------------------
 * 
 * RTCM preamble = '11010011 000000xx' = 0xd3 0x00.
 *
 * @return void No output is returned.
 * @since  0.1.0  [2025-05-29-10:30pm] New.
 * @since  3.0.9  [2025-12-14-06:00pm] Version 3.
 * @since  3.0.10 [2025-12-14-06:00pm] Match Ghost_Rover.ino.
 * @see    Global vars: Serial.
 * @see    startSerialInterfaces().
 * @see    loop().
 * @link   https://github.com/sparkfun/SparkFun_u-blox_GNSS_v3/blob/main/examples/ZED-F9P/Example3_StartRTCMBase/Example3_StartRTCMBase.ino.
 * @link   https://www.use-snip.com/kb/knowledge-base/an-rtcm-message-cheat-sheet/.
 * @link   https://www.use-snip.com/kb/knowledge-base/rtcm-3-message-list/.
 * @link   https://www.singularxyz.com/blog_detail/11.
 */
void checkRTCMtoRadio() {

    // -- Local vars. --
    static uint8_t  preamble  = 0;
    static uint16_t byteCount = 0;
           uint16_t msg_type  = 0;

    // -- Read Serial0 (EVK RTCM3) input. Send to Serial1 (HC-12 radio). --
    if (Serial0.available() > 0) {                                  // EVK RTCM3 data to read?
        serialChar = Serial0.read();                                // Read a character from Serial0 (EVK RTCM3) @ SERIAL0_SPEED.
        Serial1.write(serialChar);                                  // Write a character to Serial1 (HC-12 radio) @ SERIAL1_SPEED.
        if (serialChar == 0xd3) {                                   // Look for preamble (beginning of RTCM3 sentence).
            preamble = (preamble == 0) ? 1 : 2;                     // First (1) or new (2) preamble?
        }
        if (preamble == 1) {                                        // First preamble.
            if (byteCount < sizeof(rtcmSentence) - 1) {             // Bounds check - prevent buffer overflow.
                rtcmSentence[byteCount] = serialChar;               // Add byte to sentence buffer.
            }
            byteCount++;                                            // Increment byte counter.
        } else if (preamble == 2) {                                 // New Preamble.
            if (commandFlag[DEBUG_RAD]) {                                         // Debug.
                msg_type = rtcm3GetMessageType(rtcmSentence);       // Parse message type.
                Serial.printf("\nRTCM3 %ld: %i bytes.\n",  msg_type, byteCount);
                for (size_t i = 0; i < byteCount; i++) {
                    Serial.printf("%02x ", rtcmSentence[i]);
                }
                Serial.println();
            }
            updateLED('2');                                         // Blink LED.
            byteCount = 0;
            preamble = 0;
            memset(rtcmSentence, '\0', sizeof(rtcmSentence));       // Clear the RTCM3 sentence buffer.
        }
    }
}

/**
 * -------------------------------------------------------------------------
 *  Return RTCM3 message type.
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
uint16_t rtcm3GetMessageType(const char *buffer) {
    // Serial.printf("[%02x] [%02x] [%02x] [%02x] [%02x]\n", buffer[0],  buffer[1], buffer[2], buffer[3], buffer[3]);
    if (buffer[0] != 0xD3) {    // Check if preamble is correct
        return 0;               // Invalid preamble.
    }
    uint16_t message_type = ((uint16_t)buffer[3] << 4) | (buffer[4] >> 4);
    return message_type;
}

/**
 * -------------------------------------------------------------------------
 *  Toggle LEDs.
 * -------------------------------------------------------------------------
 *
 * @param  char ledR Radio LED.
 * @return void No output is returned.
 * @since  0.3.3 [2025-05-29-10:00pm] New.
 * @since  3.0.9 [2025-12-14-02:00pm] Version 3.
 * @link   https://www.freertos.org/Documentation/02-Kernel/04-API-references/02-Task-control/06-vTaskSuspend.
 * @link   https://www.freertos.org/Documentation/02-Kernel/04-API-references/02-Task-control/07-vTaskResume.
 */
void updateLED(char ledR) {

    // --- Radio LED. ---
    switch (ledR) {
        case '0':
            digitalWrite(LED_RADIO, LOW);               // LED off.
            break;
        case '1':
            digitalWrite(LED_RADIO, HIGH);              // LED on.
            break;
        case '2':
            vTaskResume(radioRtcmLEDtaskHandle);        // Resume task.
            break;
    }
}

// --- Test. ---
/**
 * -------------------------------------------------------------------------
 *  Display debug.
 * -------------------------------------------------------------------------
 *
 * @return void No output is returned.
 * @since  3.2.1  [2026-07-27-06:00pm] New.
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

    // --- TEST_LED. ---
    if (commandFlag[TEST_LED]) {
        Serial.printf("Valid options: 0(off), 1(on), 2(active). %c to quit.\n", EXIT_TEST);
        while (true) {                                  // Infinite loop.
            if (Serial.available() > 0) {
                serialChar = Serial.read();             // Read input from serial USB.
                switch (serialChar) {
                    case EXIT_TEST:                     // All done.
                        Serial.println("\ntestLED disabled.\n");
                        commandFlag[TEST_LED] = !commandFlag[TEST_LED];
                        return;                         // Exit test mode.
                    case '0':                           // Radio LED - off.
                        Serial.printf("\nradio LED off.\n");
                        updateLED('0');
                        break;
                    case '1':                           // BLE LED - on.
                        Serial.printf("\nradio LED on.\n");
                        updateLED('1');
                        break;
                    case '2':                           // BLE LED - active.
                        Serial.printf(" - radio LED active - 5 cycles.\n");
                        for (size_t i = 0; i < 5; i++) {
                            updateLED('2');
                            Serial.printf("Blink %i\n", i+1);
                            delay(1000);
                        }
                        Serial.println();
                        break;
                    default:
                        Serial.printf("\n%c to quit. Valid options: 0(off), 1(on), 2(active).\n", EXIT_TEST);
                }
            }
        }
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

    // --- Temporary items. ---
    // memset(debugTemp, '\0', sizeof(debugTemp));
    // strcpy(debugTemp,numberbuffer);
    // if (commandFlag[DEBUG_TEMP]) {
    //     Serial.printf("[%s]\n", debugTemp);
    // }
}

/**
 * =========================================================================
 *  Setup.
 * =========================================================================
 *
 * @return void  No output is returned.
 * @since  3.0.9 [2025-12-14-02:00pm] New.
 */
void setup() {
    showBuild();                        // Show build & processor info.
    startSerial();                      // Start serial interfaces.
    initVars();                         // Initialize global vars.
    initPins();                         // Initialize pins & pin values.
    startTasks();                       // Start tasks.
    startLoop();                        // On to loop().
}

/**
 * =========================================================================
 *  Task functions.
 * =========================================================================
 * @see startTasks()       - Start tasks.
 * @see radioRtcmLEDtask() - Radio active LED.
 */

/**
 * =========================================================================
 *  Loop.
 * =========================================================================
 *
 * @return void No output is returned.
 * @since  3.0.9 [2025-12-14-02:00pm] New.
 */
void loop() {
    checkSerialUSB();                   // Check serial USB for input.
    checkRTCMtoRadio();                 // Check Serial0 for input (EVK RTCM), relay to Serial1 (HC-12 radio).
    debug();                            // Display debug.
}
