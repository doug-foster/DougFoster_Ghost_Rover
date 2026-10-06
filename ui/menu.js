/**
 * *************************************************************************
 *  Ghost Rover 3 - UI JS.
 * *************************************************************************
 * 
 * menu.js
 *
 * @author D. Foster <doug@dougfoster.me>.
 * @since  3.0.7 [2025-11-10-12:30pm].
 * @since  3.0.12 [2026-02-25-05:45pm] Websocket send - preserve KV pair order by changing JSON data to array.
 * @since  3.1.0  [2026-03-02-05:00pm] Stable 3.0 version.
 * @since  3.1.0  [2026-03-20-11:15am] Update var names.
 * @since  3.1.1  [2026-06-25-02:00pm] Regroup: upload to SD card.
 * @since  3.1.2  [2026-07-05-09:30pm] General cleanup.
 * @since  3.2.1  [2026-07-25-06:00pm] Update JSON messages.
 * @since  3.3.4  [2026-09-17-09:45pm] Move restart, Internet, back to menu page.
 * @since  3.4.0  [2026-09-19-11:30am] Remove update().
 * @since  3.4.0  [2026-09-19-11:30am] Remove update() and const SEND_PREFS.
 * @since  3.4.1  [2025-10-26-04:45pm] Cleanup formatting.
 * @since  3.4.2  [2026-10-06-04:45pm] Add btnNtripCasterFiller.
 * @link   http://dougfoster.me.
 */

/**
 * =========================================================================
 *  Global vars.
 * =========================================================================
 *
 * @since  3.0.7  [2025-11-10-12:30pm] New.
 * @since  3.0.12 [2026-02-25-05:45pm] Websocket send - preserve KV pair order by changing JSON data to array.
 * @since  3.3.4  [2026-09-17-07:15pm] Migrated ntrip.js to menu.js.
 * @since  3.3.4  [2026-09-18-03:00pm] Move restart, Internet, & NTRIP back to menu page. 
 * @since  3.4.0  [2026-09-19-11:30am] Remove update() and const SEND_PREFS.
 * @since  3.4.2  [2026-09-19-11:30am] Add btnNtripCasterFiller.
 */
const RESTART                 = '{"page":"menu","restartGR-MCU":""}';
const CONNECT_INTERNET        = '{"connectInternet":""}';
const DISCONNECT_INTERNET     = '{"disconnectInternet":""}';
const CONNECT_NTRIP_CASTER    = '{"connectNtripCaster":""}';
const DISCONNECT_NTRIP_CASTER = '{"disconnectNtripCaster":""}';
const btnRestart              = document.querySelector('#menu-items #restart');
const btnInternet             = document.querySelector('#menu-items #internet');
const btnNtripCaster          = document.querySelector('#menu-items #ntrip-caster');
const btnNtripCasterFiller    = document.querySelector('#menu-items #ntrip-caster-filler');
const stateInternet           = document.querySelector('#menu-items #internet .state');
const stateNtripCaster        = document.querySelector('#menu-items #ntrip-caster .state');
const rtcmStatusMessage       = document.querySelector('#menu-items #rtcm-status.note');
const rtcmSentenceCount       = document.querySelector('#menu-items #rtcm-sentence-count');

/**
 * =========================================================================
 *  Functions.
 * =========================================================================
 *
 * @since 3.0.7 [2025-11-10-12:30pm] New.
 * @since 3.4.0 [2026-09-19-11:30am] Remove update() and const SEND_PREFS.
 * @see   mcuRestart() - WebSocket - MCU restart.
 */

/**
 * -------------------------------------------------------------------------
 *  WebSocket - MCU restart.
 * -------------------------------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.7 [2025-11-10-12:30pm] New.
 * @since  3.3.4 [2026-09-18-03:00pm] Move restart, Internet, & NTRIP back to menu page. 
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
 * @since 3.0.7 [2025-11-10-12:30pm] New.
 */

// --- Page. ---
 document.addEventListener('DOMContentLoaded', () => {
    webSocketInit();
});

// --- Buttons. ---
btnRestart.addEventListener('click', () => {
    mcuRestart();
});

btnInternet.addEventListener('click', () => {

    // -- Test for dependencies. --
    if  (('' == prfHotSsi) || ('' == prfHotPas)) {              // SSID & password preference must be configured.
        alert('SSID & password NOT SET on config page.');
        return;
    }

    // -- Connect/disconnect. --
    if (stateInternet.classList.contains('connecting') || stateInternet.classList.contains('connected')) {
        disconnectInternet();
    } else {
        connectInternet();
    }
});

btnNtripCaster.addEventListener('click', () => {

    // -- Test for dependencies. --
    if  ('ntrip' !== prfRtcIn) {                                // NTRIP must be set on config page.
        alert('NTRIP NOT selected on config page.');
        return;
    }
    if (!stateInternet.classList.contains('connected')) {       // Internet must be connected.
        alert('Internet NOT connected.');
        return;
    }

    // -- Connect/disconnect. --
    if (stateNtripCaster.classList.contains('connecting') || stateNtripCaster.classList.contains('connected')) {
        disconnectNtripCaster();
    } else {
        connectNtripCaster();
    }
});

/**
 * -------------------------------------------------------------------------
 *  General.
 * -------------------------------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.7 [2025-11-10-12:30pm] New.
 * @since  3.1.0 [2026-03-20-11:15am] Update var names.
 * @since  3.3.4 [2026-09-18-03:00pm] Move restart, Internet, & NTRIP back to menu page. 
 */

/**
 * -------------------------------------------------------------------------
 *  Update UI.
 * -------------------------------------------------------------------------
 * 
 * @return void  No output is returned.
 * @since  3.0.12 [2026-02-07-11:00am] New.
 * @since  3.2.1  [2026-08-02-09:30am] Move updateUi() here from operate.js.
 * @since  3.3.4  [2026-09-18-03:00pm] Move restart, Internet, & NTRIP back to menu page. 
 */
function updateUi(connectionState) {
    messageField.classList.remove('hide');
    clearUi = false;
    switch (connectionState) {
        case 'internetIsNotConnected':
            stateInternet.textContent = '- Wait -';
            clearUi = true;
            break;
        case 'ntripCasterIsNotConnected':
            stateNtripCaster.textContent = '- Wait -';
            clearUi = true;
            break;
        case 'internetIsConnecting':
            messageField.innerText = ('--> connectInternet\n');
            stateInternet.classList.add('connecting');
            stateInternet.classList.add('blink');
            stateInternet.textContent = '- Connecting -';
            break;
        case 'ntripCasterIsConnecting':
            messageField.innerText = ('--> connectNtripCaster\n');
            stateNtripCaster.classList.add('connecting');
            stateNtripCaster.classList.add('blink');
            stateNtripCaster.textContent = '- Connecting -';
            rtcmStatusMessage.classList.remove('hide');
            break;
        case 'internetIsConnected':
            stateInternet.classList.remove('connecting');
            stateInternet.classList.add('connected');
            stateInternet.classList.remove('blink');
            stateInternet.textContent = "- Connected -";
            clearUi = true;
            break;
        case 'ntripCasterIsConnected':
            stateNtripCaster.classList.remove('connecting');
            stateNtripCaster.classList.add('connected');
            stateNtripCaster.classList.remove('blink');
            stateNtripCaster.textContent = "- Connected -";
            rtcmStatusMessage.classList.remove('hide');
            clearUi = true;
    }
    if (clearUi) {
        setTimeout(function() {
                switch (connectionState) {
                    case 'internetIsNotConnected':
                        stateInternet.classList.remove('connected');
                        stateInternet.classList.remove('connecting');
                        stateInternet.classList.remove('blink');
                        stateInternet.textContent = "";
                        break;
                    case 'ntripCasterIsNotConnected':
                        stateNtripCaster.classList.remove('connected');
                        stateNtripCaster.classList.remove('connecting');
                        stateNtripCaster.classList.remove('blink');
                        stateNtripCaster.textContent = "";
                        rtcmStatusMessage.classList.add('hide');
                        break;
                    case 'internetIsConnecting':
                    case 'ntripCasterIsConnecting':
                    case 'internetIsConnected':
                    case 'ntripCasterIsConnected':
                }
                messageField.innerHTML = "&nbsp;";
                messageField.classList.add('hide');
            }, 8000);
    }
}

/**
 * -------------------------------------------------------------------------
 *  Connect Internet.
 * -------------------------------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.3.5 [2026-09-18-04:30pm] New.
 */
function connectInternet() {
    websocket.send(CONNECT_INTERNET);       // Send connect message. Response is handled in webSocketRcvMessage() in global.js.
    console.log('browser --> ' + CONNECT_INTERNET);
    updateUi('internetIsConnecting');
}

/**
 * -------------------------------------------------------------------------
 *  Connect NTRIP caster.
 * -------------------------------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.3.5 [2026-09-18-04:30pm] New.
 */
function connectNtripCaster() {
    websocket.send(CONNECT_NTRIP_CASTER);       // Send connect message. Response is handled in webSocketRcvMessage() in global.js.
    console.log('browser --> ' + CONNECT_NTRIP_CASTER);
    updateUi('ntripCasterIsConnecting');
}

/**
 * -------------------------------------------------------------------------
 *  Disconnect Internet.
 * -------------------------------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.3.5 [2026-09-09-03:15pm] New.
 * @since  3.3.4 [2026-09-18-03:00pm] Move restart, Internet, & NTRIP back to menu page. 
 */
function disconnectInternet() {
    websocket.send(DISCONNECT_INTERNET);       // Send connect message. Response is handled in webSocketRcvMessage() in global.js.
    console.log('browser --> ' + DISCONNECT_INTERNET);
    updateUi('internetIsNotConnected');
}

/**
 * -------------------------------------------------------------------------
 *  Disconnect NTRIP caster.
 * -------------------------------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.3.5 [2026-09-09-03:15pm] New.
 * @since  3.3.4 [2026-09-18-03:00pm] Move restart, Internet, & NTRIP back to menu page. 
 */
function disconnectNtripCaster() {
    websocket.send(DISCONNECT_NTRIP_CASTER);       // Send connect message. Response is handled in webSocketRcvMessage() in global.js.
    console.log('browser --> ' + DISCONNECT_NTRIP_CASTER);
    updateUi('ntripCasterIsNotConnected');
}

/**
 * =========================================================================
 *  Test.
 * =========================================================================
 *
 * @since  3.0.3 [2025-10-16-10:00am] New.
 */

/**
 * =========================================================================
 *  Run on page load.
 * =========================================================================
 *
 * @since  3.0.3 [2025-10-16-10:00am] New.
 */
