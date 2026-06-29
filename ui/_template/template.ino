/**
 * *************************************************************************
 *  Ghost Rover 3 - "Invisible" GNSS rover
 * *************************************************************************
 *
 * @author D. Foster <doug@dougfoster.me>.
 * @since  3.1.1 [2026-06-26-11:00am] New.
 * @see    https://github.com/doug-foster/DougFoster_Ghost_Rover.
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
 * @since 3.1.1 [2026-06-26-11:00am] New.
 * 
 * --- Description & operation. ---
 *     -- Comment. --
 *        - Comment. -
 *          Comment.
 * --- Other. ---
 * --- TODO: ---
 *     1. Task.
 *     2. Task.
 */

/**
 * -------------------------------------------------------------------------
 *  Code structure.
 * -------------------------------------------------------------------------
 * 
 * @since 3.1.1 [2026-06-26-11:00am] New.
 *
 *  --- Docs. ---
 *  --- Include libraries. ---
 *  --- Global vars.---
 *  --- General functions. ---
 *  --- Setup functions. ---
 *  --- Task functions. ---
 *  --- Event handlers. ---
 *  --- Loop functions. ---
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
 *     prefUtility(PREF_INIT)       // Check preferences.
 *     startSerial()                // Start serial interfaces.
 *     initPins()                   // Initialize pin modes & pin values.
 *     startI2C()                   // Start I2C wire interfaces.
 *     startTasks()                 // Start tasks.
 *     preLoop()                    // Prepare for loop().
 * --- Run loop(). ---    
 *     other()                      // Description.
 *     debug()                      // Display debug.
 * --- Tasks. ---
 *     other()                      // Description.
 * --- Event handlers. ---
 *     -- Other()                   // Description.
 *        - Comments.
 */

/**
 * -------------------------------------------------------------------------
 *  Board LED status.
 * -------------------------------------------------------------------------
 *
 * @since 3.1.1 [2026-06-26-11:00am] New.
 * 
 *  --- Description. ---
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
 * -------------------------------------------------------------------------
 *  Other doc.
 * -------------------------------------------------------------------------
 *
 * @since 3.1.1 [2026-06-26-11:00am] New.
 *
 *  --- Description. ---
 */

/**
 * =========================================================================
 *  Include libraries.
 * =========================================================================
 *
 * @since 3.1.1 [2026-06-26-11:00am] New.
 */

 // --- Core. ---
 // --- Additional. --- 

/**
 * =========================================================================
 *  Global vars.
 * =========================================================================
 *
 * @since 3.1.1 [2026-06-26-11:00am] New.
 */

// --- Pin assignments. ---
// --- Other. ---
// --- Task handles. ---
// --- Operation. ---
// --- Preferences. ---
// --- Oper status. ---
// --- Declaration. ---
// --- Test. ---

/**
 * =========================================================================
 *  General functions.
 * =========================================================================
 *
 * @since 3.1.1 [2026-06-26-11:00am] New.
 * @see   function() - Decription.
 */

/**
 * -------------------------------------------------------------------------
 *  Function
 * -------------------------------------------------------------------------
 *
 * Replace with short description.
 *
 * Replace with long description.
 *
 * @param  array $*list List of items.
 * @throws Exception Ignore.
 * @return void No output is returned.
 * @since  0.8.7 [2026-06-26] New.
 * @see    cnv_post_loaded() in convinsys.php.
 * @link   https://docs.phpdoc.org/guide/getting-started/what-is-a-docblock.html.
*/
void function1() {

    // --- Local vars. ---

    // --- Topic. ---

        // -- Subtopic. --

            // - Subopic. -

                // Subtopic.

    // -------------------------------------------------------------------------
    //  Comments.
    //  Comments.
    // -------------------------------------------------------------------------
}

/**
 * =========================================================================
 *  Setup functions.
 * =========================================================================
 *
 * @since 3.1.1 [2026-06-26-11:00am] New.
 * 
 */

/**
 * -------------------------------------------------------------------------
 *  Start tasks.
 * -------------------------------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.1.1 [2026-06-26-11:00am] New.
 * @see    Global vars: Task handles.
 * @see    setup().
 * @link   https://www.freertos.org/Documentation/02-Kernel/04-API-references/01-Task-creation/01-xTaskCreate.
 */
void startTasks() {

    // --- Loop status LED. ---
    xTaskCreate();
    Serial.println("Task started.");
}

/**
 * -------------------------------------------------------------------------
 *  Prepare for loop().
 * -------------------------------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.1.1 [2026-06-26-11:00am] New.
 * @see    setup().
 */
void preLoop() {
}

/**
 * =========================================================================
 *  Task functions.
 * =========================================================================
 *
 * @since 3.1.1 [2026-06-26-11:00am] New.
 * @see   startTasks() - Start tasks in setup().
 * @see   function() - Description.
 */

/**
 * -------------------------------------------------------------------------
 *  Task.
 * -------------------------------------------------------------------------
 * 
 * Description.
 *
 * @param  void  * pvParameters Pointer to task parameters.
 * @return void  No output is returned.
 * @since  3.1.1 [2026-06-26-11:00am] New.
 * @see    startTasks().
 * @link   https://www.freertos.org/Documentation/02-Kernel/04-API-references/02-Task-control/06-vTaskSuspend.
 */
void someTask(void * pvParameters) {
}

/**
 * =========================================================================
 *  Event handlers.
 * =========================================================================
 *
 * @since 3.1.1 [2026-06-26-11:00am] New.
 * @see   function() - description.
 */

/**
 * -------------------------------------------------------------------------
 *  Event handler.
 * -------------------------------------------------------------------------
 *
 * @param  event_t event object.
 * @return void No output is returned.
 * @since  3.1.1 [2026-06-26-11:00am] New.
 * @see    function().
 * @link   https://docs.espressif.com/projects/arduino-esp32/en/latest/api/wifi.html.
 */
void eventHandler() {
}

/**
 * =========================================================================
 *  Loop functions.
 * =========================================================================
 * 
 * Check task functions and event handlers. These are independent of loop().
 * 
 * @since 3.1.1 [2026-06-26-11:00am] New.
 * @see   function() - description.
 */

 /**
 * -------------------------------------------------------------------------
 *  Loop function.
 * -------------------------------------------------------------------------
 * 
 * @since 3.1.1 [2026-06-26-11:00am] New.
 */
void loopFunction() {
}

/**
 * =========================================================================
 *  Setup.
 * =========================================================================
 *
 * @since 3.1.1 [2026-06-26-11:00am] New.
 * @see    Global vars.
 */
void setup() {
    startTasks();                // Description.
}

/**
 * =========================================================================
 *  Loop.
 * =========================================================================
 * 
 * @since 3.1.1 [2026-06-26-11:00am] New.
 * @see   startTasks().
 * @see   Task functions.
 * @see   Event handlers.
 */
void loop() {
    loopFunction();                 // Description.
}
