/**
 * *************************************************************************
 *  Ghost Rover 3 - UI JS.
 * *************************************************************************
 * 
 * debug.js
 *
 * @author D. Foster <doug@dougfoster.me>.
 * @since  3.4.0 [2026-09-19-11:00am] New.
 * @since  3.4.1 [2025-10-26-04:45pm] Cleanup formatting.
 * @link   http://dougfoster.me.
 */

/**
 * =========================================================================
 *  Global vars.
 * =========================================================================
 *
 * @since  3.2.1 [2026-07-31-02:15pm] New.
 */

// --- General. ---
const SEND_PREFS   = '{"page":"debug","sendPrefs":""}';
const fileList     = document.querySelector('#file-list');
const debugChoices = document.querySelectorAll('#debug-choices .choice');
const btnUnDebug   = document.querySelector('#buttons #unDebug');

/**
 * =========================================================================
 *  Functions.
 * =========================================================================
 *
 * @since 3.2.1 [2026-07-31-02:15pm] New.
 * @see   update() - Update server.
 */

/**
 * -------------------------------------------------------------------------
 *  Select/unselect debug choice. Send start/stop debug to server.
 * -------------------------------------------------------------------------
 * 
 * @return void  No output is returned.
 * @since  3.4.0 [2026-09-19-01:00pm] New.
 */
function debugThis(choice, option) {
    let command;
    let notify = false;
    if (choice.classList.contains('selected')) {
        command = 'stopDebug';
        choice.classList.remove('selected');
        notify = true;
    } else {
        if ('on' == option) {
            command = 'startDebug';
            choice.classList.add('selected');
            notify = true;
        }
    }
    if (notify) {
        let message = '{"' + command + '":"' + choice.id + '"}';
        websocket.send(message);  // Send message.
        console.log('browser --> ' + message);
    }
}

/**
 * =========================================================================
 *  Event listeners.
 * =========================================================================
 *
 * @since 3.2.1 [2026-07-31-02:15pm] New.
 */

// --- Page. ---
 document.addEventListener('DOMContentLoaded', () => {
    webSocketInit();
});

// --- Debug choice: select/unselect. ---
debugChoices.forEach(debugChoice => {
    debugChoice.addEventListener('click', () => {
        debugThis(debugChoice, 'on');
    });
});

// --- Button: unselect all choices. ---
btnUnDebug.addEventListener('click', () => {
    debugChoices.forEach(debugChoice => {
        debugThis(debugChoice, 'off');
    });
});

/**
 * =========================================================================
 *  Test.
 * =========================================================================
 *
 * @since  3.2.1 [2026-07-31-02:15pm] New.
 */

/**
 * =========================================================================
 *  Run on page load.
 * =========================================================================
 *
 * @since  3.2.1 [2026-07-31-02:15pm] New.
 */
