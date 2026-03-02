/**
 * *************************************
 *      Ghost Rover 3 - UI JS.
 * *************************************
 * 
 * nmea.js
 *
 * @author D. Foster <doug@dougfoster.me>.
 * @since  3.0.12 [2026-01-31-03:30pm].
 * @since  3.0.12 [2026-02-07-04:00pm] Add THIS_PAGE.
 * @since  3.0.12 [2026-02-08-06:30pm] Removed prfRqsPvtInt.
 * @since  3.0.12 [2026-02-15-01:30pm] Removed summary statistics.
 * @since  3.0.12 [2026-02-25-05:45pm] Websocket send - preserve KV pair order by changing JSON data to array.
 * @link   http://dougfoster.me.
*/

/**
 * ============================================================================
 *                                Global vars.
 * ============================================================================
 *
 * @since 3.0.12 [2026-01-31-01:30pm].
 * @since 3.0.12 [2026-02-07-07:30am] Add THIS_PAGE.
 * @since 3.0.12 [2026-02-08-06:30pm] Removed prfRqsPvtInt.
 * @since 3.0.12 [2026-02-15-01:30pm] Removed summary statistics.
 * @since 3.0.12 [2026-02-25-05:45pm] Websocket send - preserve KV pair order by changing JSON data to array.
 */
const THIS_PAGE             = '[{"page":"nmea"}]';
const nmeaDisplayArea       = document.querySelector('#nmeaOutput #nmeaDisplay');
const nmeaMessageLine       = document.querySelector('#nmeaOutput #nmeaMessage')
const numSolutionsToDisplay = 20;
let   solutionCount         = 0;
let   deltaMs               = 0;
let   lastDate;

/**
 * ============================================================================
 *                                Functions.
 * ============================================================================
 *
 * @since 3.0.12 [2026-01-31-01:30pm].
 * @see   update()       - Update server.
 * @see   nmeaMessage() - Parse WebSocket message, update UI elements.
 * @since 3.0.12 [2026-02-15-01:30pm] Removed summary statistics. 
 */

/**
 * ------------------------------------------------
 *      Update server.
 * ------------------------------------------------
 * 
 * @return void  No output is returned.
 * @since  3.0.12 [2026-01-31-04:15pm] New.
 * @since  3.0.12 [2026-02-07-04:00pm] Add THIS_PAGE.
 * @see    openedWebSocket() in global.js.
 */
function update() {
    websocket.send(THIS_PAGE);  // Send THIS_PAGE message.
    console.log('browser --> ' + THIS_PAGE);
}

/**
 * ------------------------------------------------
 *      Parse WebSocket message, update UI elements.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.12 [2026-01-31-01:30pm] New.
 * @since  3.0.12 [2026-02-08-06:30pm] Removed prfRqsPvtInt.
 * @since  3.0.12 [2026-02-09-03:45pm] Correct kbps.
 * @since  3.0.12 [2026-02-15-01:30pm] Removed summary statistics.
 * @since  3.1.0  [2026-03-02-05:00pm] Stable 3.0 version.
 * @see    messageRcvWebSocket() in global.js.
 */
function nmeaMessage(key, value) {
    switch (key) {     
        case 'nmea':                          // browser <-- {"nmea":"$GPGSV,1, ..."}
            if (solutionCount < numSolutionsToDisplay) {
                if (value.includes('$GNGGA')) {

                    // --- Make a timestamp. ---
                    const date         = new Date();
                    const hours        = String(date.getHours()).padStart(2, '0');
                    const minutes      = String(date.getMinutes()).padStart(2, '0');
                    const seconds      = String(date.getSeconds()).padStart(2, '0');
                    const milliseconds = String(date.getMilliseconds()).padStart(3, '0');
                    let timeStamp      = `${hours}h ${minutes}m ${seconds}s ${milliseconds}ms`;

                    // --- Calculate interval since last $GNGGA sentence. ---
                    deltaMs = Math.abs(date - lastDate); 
                    lastDate = date;

                    // --- Display timestamp & delta ms. ---
                    solutionCount++;
                    nmeaDisplayArea.innerHTML += '<br><br><b> #' + solutionCount + '/' + numSolutionsToDisplay + ' - ' + timeStamp + '  (' + deltaMs + 'ms since last)</b><br>';
                }

                // --- Build the output. ---
                if (solutionCount > 0) {
                    nmeaDisplayArea.innerHTML += value;
                }
            }
            break;
    }
}

/**
 * ============================================================================
 *                             Event listeners.
 * ============================================================================
 *
 * @since 3.0.12 [2026-01-31-01:30pm].
 */

/**
 * ------------------------------------------------
 *      Page.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since 3.0.12 [2026-01-31-01:30pm] New.
 * @see global.js.
 */
 document.addEventListener('DOMContentLoaded', () => {
    initWebSocket();

    // --- Console debug. ---
    console.log('Show console messages is "' + sessionStorage.getItem("displayJsConsoleMessages") + '".');
});

/**
 * ============================================================================
 *                                    Test.
 * ============================================================================
 *
 * @since 3.0.12 [2026-01-31-01:30pm].
 */

/**
 * ============================================================================
 *                             Run on page load.
 * ============================================================================
 *
 * @since 3.0.12 [2026-01-31-01:30pm].
 */
