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
 * @since  3.4.0  [2026-09-19-11:30am] Remove update() and const SEND_PREFS.
 * @since  3.4.1  [2025-10-26-04:45pm] Cleanup formatting.
 * @link   http://dougfoster.me.
 */

/**
 * =========================================================================
 *  Global vars.
 * =========================================================================
 *
 * @since 3.0.12 [2026-01-31-01:30pm] New.
 * @since 3.0.12 [2026-02-07-07:30am] Add SEND_PREFS.
 * @since 3.0.12 [2026-02-08-06:30pm] Removed prfRqsPvtInt.
 * @since 3.0.12 [2026-02-15-01:30pm] Removed summary statistics.
 * @since 3.0.12 [2026-02-25-05:45pm] Websocket send - preserve KV pair order by changing JSON data to array.
 * @since 3.4.0  [2026-09-19-11:30am] Remove update() and const SEND_PREFS.
 */

const nmeaDisplayArea        = document.querySelector('#nmeaOutput #nmeaDisplay');
const nmeaMessageLine        = document.querySelector('#nmeaOutput #nmeaMessage')
const nmeaSentencesToDisplay = 80;
let   nmeaSentenceCount      = 0;
let   deltaMs                = 0;
let   lastDate;
let   numBytes               = 0;

/**
 * =========================================================================
 *  Functions.
 * =========================================================================
 *
 * @since 3.0.12 [2026-01-31-01:30pm] New.
 * @since 3.0.12 [2026-02-15-01:30pm] Removed summary statistics.
 * @since 3.4.0  [2026-09-19-11:30am] Remove update() and const SEND_PREFS. Remove displayNmeaMessage().
 */

/**
 * =========================================================================
 *  Event listeners.
 * =========================================================================
 *
 * @since 3.0.12 [2026-01-31-01:30pm] New.
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
 * @since 3.0.12 [2026-01-31-01:30pm] New.
 */

/**
 * =========================================================================
 *  Run on page load.
 * =========================================================================
 *
 * @since 3.0.12 [2026-01-31-01:30pm] New.
 */
