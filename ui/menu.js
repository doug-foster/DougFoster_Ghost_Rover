/**
 * *************************************
 *      Ghost Rover 3 - UI JS.
 * *************************************
 * 
 * menu.js
 *
 * @author D. Foster <doug@dougfoster.me>.
 * @since  3.0.7 [2025-11-10-12:30pm].
 * @since  3.0.12 [2026-02-07-07:30am] Add THIS_PAGE.
 * @since  3.0.12 [2026-02-25-05:45pm] Websocket send - preserve KV pair order by changing JSON data to array.
 * @since  3.1.0  [2026-03-02-05:00pm] Stable 3.0 version.
 * @link   http://dougfoster.me.
*/

/**
 * ============================================================================
 *                                Global vars.
 * ============================================================================
 *
 * @since  3.0.7 [2025-11-10-12:30pm].
 * @since  3.0.12 [2026-02-07-07:30am] Add THIS_PAGE.
 * @since  3.0.12 [2026-02-25-05:45pm] Websocket send - preserve KV pair order by changing JSON data to array.
 */

const btnRestart = document.querySelector('#menu-items #restart');
const THIS_PAGE = '[{"page":"menu"}]';
const RESTART   = '[{"mcu":"restart"}]';

/**
 * ============================================================================
 *                                Functions.
 * ============================================================================
 *
 * @since 3.0.7 [2025-11-10-12:30pm].
 * @since 3.0.12 [2026-02-07-07:30am] Add THIS_PAGE.
 * @see   update()     - Update server.
 * @see   mcuRestart() - WebSocket - MCU restart.
 */

/**
 * ------------------------------------------------
 *      Update server.
 * ------------------------------------------------
 * 
 * @return void  No output is returned.
 * @since  3.0.12 [2026-01-31-03:30pm] New.
 * @since  3.0.12 [2026-02-07-07:30am] Add THIS_PAGE.
 * @see    openedWebSocket() in global.js.
 */
function update() {
    websocket.send(THIS_PAGE);  // Send THIS_PAGE message.
    console.log('browser --> ' + THIS_PAGE);
}

/**
 * ------------------------------------------------
 *      WebSocket - MCU restart.
 * ------------------------------------------------
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
 * ============================================================================
 *                             Event listeners.
 * ============================================================================
 *
 * @since 3.0.7 [2025-11-10-12:30pm].
 */

/**
 * ------------------------------------------------
 *      General.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.7 [2025-11-10-12:30pm].
 */

 // --- Page. ---
 document.addEventListener('DOMContentLoaded', () => {
    initWebSocket();
});

// --- Buttons. ---
btnRestart.addEventListener('click', () => {
    mcuRestart();
});

/**
 * ------------------------------------------------
 *      General.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.3 [2025-10-13-02:15pm].
 */

/**
 * ============================================================================
 *                                    Test.
 * ============================================================================
 *
 * @since  3.0.3 [2025-10-16-10:00am].
 */

/**
 * ============================================================================
 *                             Run on page load.
 * ============================================================================
 *
 * @since  3.0.3 [2025-10-16-10:00am].
 */
