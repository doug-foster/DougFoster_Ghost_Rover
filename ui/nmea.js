/**
 * *************************************************************************
 *  Ghost Rover 3 - UI JS.
 * *************************************************************************
 * 
 * nmea.js
 *
 * @author D. Foster <doug@dougfoster.me>.
 * @since  3.0.12 [2026-01-31-03:30pm].
 * @since  3.0.12 [2026-02-07-04:00pm] Add SEND_PREFS.
 * @since  3.0.12 [2026-02-08-06:30pm] Removed prfRqsPvtInt.
 * @since  3.0.12 [2026-02-15-01:30pm] Removed summary statistics.
 * @since  3.0.12 [2026-02-25-05:45pm] Websocket send - preserve KV pair order by changing JSON data to array.
 * @since  3.1.0  [2026-03-20-11:15am] Update var names.
 * @since  3.1.1  [2026-06-25-02:00pm] Regroup: upload to SD card.
 * @since  3.1.2  [2026-07-05-08:30pm] General cleanup.
 * @since  3.2.1  [2026-07-25-08:45pm] Update JSON messages.
 * @since  3.2.1  [2026-07-26-02:30pm] Update timestamp format.
 * @link   http://dougfoster.me.
*/

/**
 * =========================================================================
 *  Global vars.
 * =========================================================================
 *
 * @since 3.0.12 [2026-01-31-01:30pm].
 * @since 3.0.12 [2026-02-07-07:30am] Add SEND_PREFS.
 * @since 3.0.12 [2026-02-08-06:30pm] Removed prfRqsPvtInt.
 * @since 3.0.12 [2026-02-15-01:30pm] Removed summary statistics.
 * @since 3.0.12 [2026-02-25-05:45pm] Websocket send - preserve KV pair order by changing JSON data to array.
 */
const SEND_PREFS            = '{"page":"nmea","sendPrefs":""}';
const nmeaDisplayArea       = document.querySelector('#nmeaOutput #nmeaDisplay');
const nmeaMessageLine       = document.querySelector('#nmeaOutput #nmeaMessage')
const numSolutionsToDisplay = 20;
let   solutionCount         = 0;
let   deltaMs               = 0;
let   lastDate;

/**
 * =========================================================================
 *  Functions.
 * =========================================================================
 *
 * @since 3.0.12 [2026-01-31-01:30pm].
 * @see   update()             - Update server.
 * @see   displayNmeaMessage() - Display NMEA message.
 * @since 3.0.12 [2026-02-15-01:30pm] Removed summary statistics. 
 */

/**
 * -------------------------------------------------------------------------
 *  Update server.
 * -------------------------------------------------------------------------
 * 
 * @return void  No output is returned.
 * @since  3.0.12 [2026-01-31-04:15pm] New.
 * @since  3.0.12 [2026-02-07-04:00pm] Add SEND_PREFS.
 * @see    webSocketOpened() in global.js.
 */
function update() {
    websocket.send(SEND_PREFS);  // Send SEND_PREFS message.
    console.log('browser --> ' + SEND_PREFS);
}

/**
 * -------------------------------------------------------------------------
 *  Display NMEA message.
 * -------------------------------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.12 [2026-01-31-01:30pm] New.
 * @since  3.0.12 [2026-02-08-06:30pm] Removed prfRqsPvtInt.
 * @since  3.0.12 [2026-02-09-03:45pm] Correct kbps.
 * @since  3.0.12 [2026-02-15-01:30pm] Removed summary statistics.
 * @since  3.1.0  [2026-03-02-05:00pm] Stable 3.0 version.
 * @since  3.2.1  [2026-07-26-02:30pm] Update timestamp format.
 * @see    webSocketRcvMessage() in global.js.
 */
function displayNmeaMessage(message) {

    if (solutionCount < numSolutionsToDisplay) {
        if (message.includes('$GNGGA')) {

            // --- Make a timestamp. ---
            const date         = new Date();
            const hours        = String(date.getHours()).padStart(2, '0');
            const minutes      = String(date.getMinutes()).padStart(2, '0');
            const seconds      = String(date.getSeconds()).padStart(2, '0');
            const milliseconds = String(date.getMilliseconds()).padStart(3, '0');
            let timeStamp      = `@${hours}:${minutes}:${seconds}.${milliseconds}`;

            // --- Calculate interval since last $GNGGA sentence. ---
            deltaMs = Math.abs(date - lastDate); 
            lastDate = date;

            // --- Display timestamp & delta ms. ---
            solutionCount++;
            nmeaDisplayArea.innerHTML += '<br><br><b> #' + solutionCount + '/' + numSolutionsToDisplay + ' - ' + timeStamp + '  </b>(<b>' + deltaMs + 'ms</b> since last<b>)</b><br>';
        }

        // --- Build the output. ---
        if (solutionCount > 0) {
            nmeaDisplayArea.innerHTML += message;
        }
    }
}

/**
 * =========================================================================
 *  Event listeners.
 * =========================================================================
 *
 * @since 3.0.12 [2026-01-31-01:30pm].
 */

/**
 * -------------------------------------------------------------------------
 *  Page.
 * -------------------------------------------------------------------------
 *
 * @return void  No output is returned.
 * @since 3.0.12 [2026-01-31-01:30pm] New.
 * @since  3.1.0  * @since  3.1.0  [2026-03-20-11:15am] Update var names.[2026-03-20-11:15am] Update var names.
 * @see global.js.
 */
 document.addEventListener('DOMContentLoaded', () => {
    webSocketInit();

    // --- Console debug. ---
    console.log('Show console messages is "' + sessionStorage.getItem("displayJsConsoleMessages") + '".');
});

/**
 * =========================================================================
 *  Test.
 * =========================================================================
 *
 * @since 3.0.12 [2026-01-31-01:30pm].
 */

/**
 * =========================================================================
 *  Run on page load.
 * =========================================================================
 *
 * @since 3.0.12 [2026-01-31-01:30pm].
 */
