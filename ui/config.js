/**
 * *************************************
 *      Ghost Rover 3 - UI JS.
 * *************************************
 * 
 * config.js
 *
 * @author D. Foster <doug@dougfoster.me>.
 * @since  3.0.12 [2026-01-31-03:30pm] New.
 * @since  3.0.12 [2026-02-01-06:45pm] Shortened names.
 * @since  3.0.12 [2026-02-06-10:45pm] Add reset.
 * @since  3.0.12 [2026-02-07-07:30am] Add THIS_PAGE.
 * @since  3.0.12 [2026-02-08-06:30pm] Removed prfRqsPvtInt.
 * @since  3.0.12 [2026-02-21-03:30pm] Remove GET_PREFS.
 * @since  3.0.12 [2026-02-22-04:30pm] Change JSON keys from string to alpha integer.
 * @since  3.0.12 [2026-02-25-05:45pm] Websocket send - preserve KV pair order by changing JSON data to array.
 * @since  3.0.12 [2026-02-26-10:30am] Check for missing preference.
 * @since  3.1.0  [2026-03-02-05:00pm] Stable 3.0 version.
 * @link   http://dougfoster.me.
*/

/**
 * ============================================================================
 *                                Global vars.
 * ============================================================================
 *
 * @since 3.0.12 [2026-01-31-01:30pm] New.
 * @since 3.0.12 [2026-02-01-02:00pm] Shortened names.
 * @since 3.0.12 [2026-02-06-10:45pm] Add reset.
 * @since 3.0.12 [2026-02-07-11:00am] Add clearMessageField().
 * @since 3.0.12 [2026-02-21-03:30pm] Remove GET_PREFS.
 * @since  3.0.12 [2026-02-25-05:45pm] Websocket send - preserve KV pair order by changing JSON data to array.
 */

const gnssMeasureInterval = document.querySelector('#config #gnss-measure-interval');
const gnssNavRate         = document.querySelector('#config #gnss-nav-rate');
const outputInterval      = document.querySelector('#config #output-interval');
const hotspotSsid         = document.querySelector('#config #hotspot-ssid');
const hotspotPassword     = document.querySelector('#config #hotspot-password');
const updateConfigBtn     = document.querySelector('#config #update-config-btn');
const resetConfigBtn      = document.querySelector('#config #reset-config-btn');
const messageForm         = document.querySelector('#config #message-form');
const formConfig          = document.querySelector('form#config');
const jsConsoleMessages   = document.querySelector('#js-console-messages input');
const THIS_PAGE           = '[{"page":"config"}]';
const RESET_PREFS         = '[{"config":"reset"}]';
        
/**
 * ============================================================================
 *                                Functions.
 * ============================================================================
 *
 * @since 3.0.12 [2026-01-31-11:00am].
 * @since 3.0.12 [2026-02-07-07:30am] Add THIS_PAGE.
 * @see   update()            - Update server.
 * @see   clearMessageField() - Clear message field.
 * @see   prefsMessage()      - Clear message field.
 * @see   uiToPrefs()         - Get UI values, return as JSON formatted string.
 * @see   prefsMessage()      - Parse WebSocket message, update UI elements.
 */

/**
 * ------------------------------------------------
 *      Update server.
 * ------------------------------------------------
 * 
 * @return void  No output is returned.
 * @since  3.0.12 [2026-01-31-03:30pm] New.
 * @since  3.0.12 [2026-02-21-03:30pm] Remove GET_PREFS.
 * @see    openedWebSocket() in global.js. Update() is different for every page.js.
 */
function update() {
    websocket.send(THIS_PAGE);  // Send THIS_PAGE message.
    console.log('browser --> ' + THIS_PAGE);
}

/**
 * ------------------------------------------------
 *      Clear message field.
 * ------------------------------------------------
 * 
 * @return void  No output is returned.
 * @since  3.0.12 [2026-02-07-11:00am] New.
 */
function clearMessageField() {
    setTimeout(function() { messageForm.innerHTML = "&nbsp;"; }, 6000);
}

/**
 * ------------------------------------------------
 *      Get UI values, return as JSON formatted string.
 * ------------------------------------------------
 * 
 * @return JSON   formatted string.
 * @since  3.0.12 [2026-01-31-01:30pm] New.
 * @since  3.0.12 [2026-02-01-02:00pm] Shortened names.
 * @since  3.0.12 [2026-02-22-04:30pm] Change JSON keys from string to alpha integer.
 * @since  3.0.12 [2026-02-25-05:45pm] Websocket send - preserve KV pair order by changing JSON data to array.
 * @see    Global vars in global.js.
 */
function uiToPrefs() {
    return JSON.stringify( [
        {'config' : 'set'},
        {'1'      : document.querySelector('input[name="switch-unit"]:checked')?.value,     // 1. prfUnt.
        '2'       : document.querySelector('input[name="switch-rtcm-in"]:checked')?.value,   // 2. prfRtcIn.
        '3'       : document.querySelector('input[name="switch-nmea-out"]:checked')?.value,  // 3. prfNmeOut.
        '4'       : gnssMeasureInterval.value,                                               // 4. prfGnsMsrInt
        '5'       : gnssNavRate.value,                                                       // 5. prfGnsNavRat.
        '6'       : hotspotSsid.value,                                                       // 6. prfHotSsi
        '7'       : hotspotPassword.value                                                    // 7. prfHotPas
    }] );
}

/**
 * ------------------------------------------------
 *      Parse WebSocket message, update UI elements.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.12 [2026-01-31-01:30pm] New.
 * @since  3.0.12 [2026-02-01-02:00pm] Shortened names.
 * @since  3.0.12 [2026-02-22-04:30pm] Change JSON keys from string to alpha integer.
 * @since  3.0.12 [2026-02-26-10:30am] Check for missing preference.
 * @see    messageRcvWebSocket() in global.js.
 */
function prefsMessage(key, value) {
    if ("" === value) {
        alert('Preference ' + key + ' has no value. Resetting to defaults.');
        resetConfigBtn.click();
    }
    switch (key) {     
        case wsKey.WS_PREF_UNIT:                    // 1. prfUnt.
            prfUnt = value;
            document.querySelector('input[name="switch-unit"][value="' + value + '"]').checked = true;
            break;
        case wsKey.WS_PREF_RTCM_IN:                 // 2. prfRtcIn.
            prfRtcIn = value;
            document.querySelector('input[name="switch-rtcm-in"][value="' + value + '"]').checked = true;
            break;
        case wsKey.WS_PREF_NMEA_OUT:                // 3. prfNmeOut.
            prfNmeOut = value;
            document.querySelector('input[name="switch-nmea-out"][value="' + value + '"]').checked = true;
            break;
        case wsKey.WS_PREF_GNSS_MESASURE_INTERVAL:  // 4. prfGnsMsrInt.
            prfGnsMsrInt = value;
            gnssMeasureInterval.value = value;
            outputInterval.textContent = gnssMeasureInterval.value * gnssNavRate.value ;
            break;
        case wsKey.WS_PREF_GNSS_NAV_RATE:           // 5. prfGnsNavRat.
            prfGnsNavRat = value;
            gnssNavRate.value = value;
            outputInterval.textContent = gnssMeasureInterval.value * gnssNavRate.value ;
            break;
        case wsKey.WS_PREF_HOT_SPOT_SSID:           // 6. prfHotSsi.
            prfHotSsi = value;
            hotspotSsid.value = value;
            break;
        case wsKey.WS_PREF_HOT_SPOT_PASS:           // 7. prfHotPas.
            prfHotPas = value;
            hotspotPassword.value = value;
            break;
        case 'config':                              // browser <-- {"config":"some message"}
            messageForm.textContent = value;        // 'Config saved. Rover updated/reset.'
    }
}

/**
 * ============================================================================
 *                             Event listeners.
 * ============================================================================
 *
 * @since 3.0.12 [2026-01-31-01:30pm] New.
 */

/**
 * ------------------------------------------------
 *      Page.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.12 [2026-01-31-01:30pm] New.
 * @since  3.0.12 [2026-02-07-11:00am] Add clearMessageField().
 * @see    initWebSocket() in global.js.
 */
 document.addEventListener('DOMContentLoaded', () => {
    initWebSocket();
    clearMessageField();

    // --- Console debug. ---
    console.log('Show console messages is "' + sessionStorage.getItem("displayJsConsoleMessages") + '".');
    if (sessionStorage.getItem("displayJsConsoleMessages") == 'on') {
        jsConsoleMessages.checked = true;
    } else {
        jsConsoleMessages.checked = false;
    }
});

/**
 * ------------------------------------------------
 *      Switches.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since 3.0.12 [2026-01-31-01:30pm] New.
 * @since 3.0.12 [2026-02-01-06:00pm] Shortened names.
 */
gnssMeasureInterval.addEventListener('change', () => {
    outputInterval.textContent = gnssMeasureInterval.value * gnssNavRate.value ;
});
gnssNavRate.addEventListener('change', () => {
    outputInterval.textContent = gnssMeasureInterval.value * gnssNavRate.value ;
});

/**
 * ------------------------------------------------
 *      Buttons.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since 3.0.12 [2026-02-06-10:45pm] New.
 * @since  3.0.12 [2026-02-07-11:00am] Add clearMessageField().
 */
updateConfigBtn.addEventListener('click', () => {
    updateConfigBtn.classList.add('shadow');    // Visual feedback.
    setTimeout(function() { updateConfigBtn.classList.remove('shadow'); }, 100);
    if ((websocket) && (1 == websocket.readyState)) {
        websocket.send( uiToPrefs() );          // Send "set" preferences message to rover.
    }
    console.log('browser --> ' + uiToPrefs());
    clearMessageField();
});
resetConfigBtn.addEventListener('click', () => {
    updateConfigBtn.classList.add('shadow');    // Visual feedback.
    setTimeout(function() { updateConfigBtn.classList.remove('shadow'); }, 100);
    if ((websocket) && (1 == websocket.readyState)) {
        let confirmed = confirm("Are you sure?");
        if (confirmed) {
            websocket.send(RESET_PREFS);  // Send "reset" preferences message to rover.
            console.log('browser --> ' + RESET_PREFS);
            clearMessageField();
        } else {
            messageForm.textContent = 'Reset cancelled.';
            clearMessageField();
        }
    }
});

/**
 * ------------------------------------------------
 *      Other.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since 3.0.12 [2026-02-08-08:45pm] New.
 */
jsConsoleMessages.addEventListener('change', () => {
    if (jsConsoleMessages.checked) {
        sessionStorage.setItem("displayJsConsoleMessages", 'on');
    } else {
        sessionStorage.setItem("displayJsConsoleMessages", 'off');
    }
    console.log('Show console messages is "' + sessionStorage.getItem("displayJsConsoleMessages") + '".');
});

/**
 * ============================================================================
 *                                    Test.
 * ============================================================================
 *
 * @since  3.0.12 [2026-01-31-01:30pm].
 */

/**
 * ============================================================================
 *                             Run on page load.
 * ============================================================================
 *
 * @since  3.0.12 [2026-01-31-01:30pm].
 */
outputInterval.textContent = gnssMeasureInterval.value * gnssNavRate.value ;
