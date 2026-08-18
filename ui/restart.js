/**
 * *************************************************************************
 *  Ghost Rover 3 - UI JS.
 * *************************************************************************
 * 
 * restart.js
 *
 * @author D. Foster <doug@dougfoster.me>.
 * @since  3.2.1 [2026-07-31-02:15pm]. New.
 * @link   http://dougfoster.me.
*/

/**
 * =========================================================================
 *  Global vars.
 * =========================================================================
 *
 * @since  3.2.1 [2026-07-31-02:15pm]. New.
 */

const btnRestart = document.querySelector('#restart-items #restart');
const SEND_PREFS = '{"page":"restart","sendPrefs":""}';
const RESTART    = '{"page":"restart","restartGR-MCU":""}';

/**
 * =========================================================================
 *  Functions.
 * =========================================================================
 *
 * @since  3.2.1 [2026-07-31-02:15pm]. New.
 * @see   update()     - Update server.
 * @see   mcuRestart() - WebSocket - MCU restart.
 */

/**
 * -------------------------------------------------------------------------
 *  Update server.
 * -------------------------------------------------------------------------
 * 
 * @return void  No output is returned.
 * @since  3.2.1 [2026-07-31-02:15pm]. New.
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
 * @since  3.2.1 [2026-07-31-02:15pm]. New.
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
 * @since  3.2.1 [2026-07-31-02:15pm]. New.
 */

/**
 * -------------------------------------------------------------------------
 *  General.
 * -------------------------------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.2.1 [2026-07-31-02:15pm]. New.
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
 * @since  3.2.1 [2026-07-31-02:15pm]. New.
 */

/**
 * =========================================================================
 *  Test.
 * =========================================================================
 *
 * @since  3.2.1 [2026-07-31-02:15pm]. New.
 */

/**
 * =========================================================================
 *  Run on page load.
 * =========================================================================
 *
 * @since  3.2.1 [2026-07-31-02:15pm]. New.
 */
