/**
 * *************************************************************************
 *  Ghost Rover 3 - UI JS.
 * *************************************************************************
 * 
 * menu.js
 *
 * @author D. Foster <doug@dougfoster.me>.
 * @since  3.0.7 [2025-11-10-12:30pm].
 * @since  3.0.12 [2026-02-07-07:30am] Add SEND_PREFS.
 * @since  3.0.12 [2026-02-25-05:45pm] Websocket send - preserve KV pair order by changing JSON data to array.
 * @since  3.1.0  [2026-03-02-05:00pm] Stable 3.0 version.
 * @since  3.1.0  [2026-03-20-11:15am] Update var names.
 * @since  3.1.1  [2026-06-25-02:00pm] Regroup: upload to SD card.
 * @since  3.1.2  [2026-07-05-09:30pm] General cleanup.
 * @since  3.2.1  [2026-07-25-06:00pm] Update JSON messages.
 * @link   http://dougfoster.me.
*/

/**
 * =========================================================================
 *  Global vars.
 * =========================================================================
 *
 * @since  3.0.7 [2025-11-10-12:30pm].
 * @since  3.0.12 [2026-02-07-07:30am] Add SEND_PREFS.
 * @since  3.0.12 [2026-02-25-05:45pm] Websocket send - preserve KV pair order by changing JSON data to array.
 */

const btnRestart = document.querySelector('#menu-items #restart');
const SEND_PREFS = '{"page":"menu","sendPrefs":""}';
const RESTART    = '{"page":"menu","restartGR-MCU1":""}';

/**
 * =========================================================================
 *  Functions.
 * =========================================================================
 *
 * @since 3.0.7 [2025-11-10-12:30pm].
 * @since 3.0.12 [2026-02-07-07:30am] Add SEND_PREFS.
 * @see   update()     - Update server.
 * @see   mcuRestart() - WebSocket - MCU restart.
 */

/**
 * -------------------------------------------------------------------------
 *  Update server.
 * -------------------------------------------------------------------------
 * 
 * @return void  No output is returned.
 * @since  3.0.12 [2026-01-31-03:30pm] New.
 * @since  3.0.12 [2026-02-07-07:30am] Add SEND_PREFS.
 * @see    webSocketOpened() in global.js.
 */
function update() {
    websocket.send(SEND_PREFS);  // Send SEND_PREFS message.
    console.log('browser --> ' + SEND_PREFS);
}

/**
 * -------------------------------------------------------------------------
 *  WebSocket - MCU restart.
 * -------------------------------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.7 [2025-11-10-12:30pm].
 */
function mcuRestart() {
    let message = RESTART;      // Send restart message to server.
    websocket.send(message);
    setTimeout(function() { console.log('browser --> ' + message) }, 1000);
}

/**
 * =========================================================================
 *  Event listeners.
 * =========================================================================
 *
 * @since 3.0.7 [2025-11-10-12:30pm].
 */

/**
 * -------------------------------------------------------------------------
 *  General.
 * -------------------------------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.7 [2025-11-10-12:30pm].
 * @since  3.1.0 [2026-03-20-11:15am] Update var names.
 */

 // --- Page. ---
 document.addEventListener('DOMContentLoaded', () => {
    webSocketInit();
});

// --- Buttons. ---
btnRestart.addEventListener('click', () => {
    mcuRestart();
});

/**
 * -------------------------------------------------------------------------
 *  General.
 * -------------------------------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.3 [2025-10-13-02:15pm].
 */

/**
 * =========================================================================
 *  Test.
 * =========================================================================
 *
 * @since  3.0.3 [2025-10-16-10:00am].
 */

/**
 * =========================================================================
 *  Run on page load.
 * =========================================================================
 *
 * @since  3.0.3 [2025-10-16-10:00am].
 */
