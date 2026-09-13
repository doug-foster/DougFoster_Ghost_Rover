/**
 * *************************************************************************
 *  Ghost Rover 3 - UI JS.
 * *************************************************************************
 * 
 * ntrip.js
 *
 * @author D. Foster <doug@dougfoster.me>.
 * @since  3.2.1 [2026-08-01-04:45pm] New.
 * @since  3.3.3 [2026-09-01-08:15pm] Fix btnNtripCaster.
 * @since  3.3.5 [2026-09-08-09:30pm] Event listeners: add ntripCasterLabel, refactor btnNtripCaster, 
 *  add ntripCasterIsNotConnected(), ntripCasterIsConnecting(), ntripCasterIsConnected(), disconnectWifiClient(), disconnectNtripCaster().
 * @link   http://dougfoster.me.
*/

/**
 * =========================================================================
 *  Global vars.
 * =========================================================================
 *
 * @since  3.2.1 [2026-07-31-02:15pm]. New.
 */

const btnWifiClient           = document.querySelector('#ntrip #wifi-client');
const btnWifiClientLabel      = document.querySelector('#ntrip #wifi-client .label');
const btnNtripCaster          = document.querySelector('#ntrip #ntrip-caster');
const btnNtripCasterLabel     = document.querySelector('#ntrip #ntrip-caster .label');
const wifiClientLabel         = document.querySelector('#ntrip #wifi-client-label');
const ntripCasterLabel        = document.querySelector('#ntrip #ntrip-caster-label');
const statusWifiClient        = document.querySelector('#ntrip #wifi-client-status');
const statusNtripCaster       = document.querySelector('#ntrip #ntrip-caster-status');
const statusNote              = document.querySelector('#ntrip #status.note');
const rtcmSentenceCount       = document.querySelector('#ntrip #rtcm-sentence-count');
const SEND_PREFS              = '{"page":"ntrip","sendPrefs":""}';
const CONNECT_WIFI_CLIENT     = '{"connectWifiClient":""}';
const DISCONNECT_WIFI_CLIENT  = '{"disconnectWifiClient":""}';
const CONNECT_NTRIP_CASTER    = '{"connectNtripCaster":""}';
const DISCONNECT_NTRIP_CASTER = '{"disconnectNtripCaster":""}';

/**
 * =========================================================================
 *  Functions.
 * =========================================================================
 *
 * @since 3.2.1 [2026-07-31-02:15pm]. New.
 * @see   update()     - Update server.
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
 * =========================================================================
 *  Event listeners.
 * =========================================================================
 *
 * @since 3.2.1 [2026-07-31-02:15pm]. New.
 * @since 3.3.3 [2026-09-01-08:15pm]. Fix btnNtripCaster.
 */

/**
 * -------------------------------------------------------------------------
 *  General.
 * -------------------------------------------------------------------------
 *
 * @return void  No output is returned.
z
 */

 // --- Page. ---
 document.addEventListener('DOMContentLoaded', () => {
    webSocketInit();
});

// --- Labels. ---
wifiClientLabel.addEventListener('click', () => {
    disconnectWifiClient();
    wifiClientIsNotConnected();
});

ntripCasterLabel.addEventListener('click', () => {
    disconnectNtripCaster();
    ntripCasterIsNotConnected();
});

// --- Buttons. ---
btnWifiClient.addEventListener('click', () => {

    // -- Test for dependencies. --
    if  (('' == prfHotSsi) || ('' == prfHotPas)) {      // SSID & password preference must be configured.
        alert('SSID & password NOT SET on config page.');
        return;
    }

    if (btnWifiClient.innerText.includes("Connect")) {
        wifiClientIsConnecting();
        websocket.send(CONNECT_WIFI_CLIENT);            // Send connect message.
        console.log('browser --> ' + CONNECT_WIFI_CLIENT);
        messageField.innerText = ('--> connectWifiClient\n');
        // Response is handled in 'ntrip page' section of webSocketRcvMessage() in global.js.
    } else {
        disconnectWifiClient();
        wifiClientIsNotConnected();
    }
});

btnNtripCaster.addEventListener('click', () => {

    // -- Test for dependencies. --
    if  ('ntrip' !== prfRtcIn) {                        // NTRIP must be set on config page.
        alert('NTRIP NOT SET on config page.');
        return;
    }
    if (btnWifiClient.innerText.includes("Connect")) {      // WiFi client must be connected.
        alert('WiFi client NOT connected.');
        return;
    }

    if (btnNtripCaster.innerText.includes("Connect")) {

        // -- Connect NTRIP caster. --
        ntripCasterIsConnecting();
        websocket.send(CONNECT_NTRIP_CASTER);           // Send connect message.
        console.log('browser --> ' + CONNECT_NTRIP_CASTER);
        messageField.innerText = ('--> connectNtripCaster\n');
        // Response is handled in 'ntrip page' section of webSocketRcvMessage() in global.js.
    } else {
        disconnectNtripCaster();
        ntripCasterIsNotConnected();
    }
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
 * -------------------------------------------------------------------------
 *  WiFiClient states.
 * -------------------------------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.3.5 [2026-09-08-08:45pm] New.
 */
function wifiClientIsNotConnected() {
    statusWifiClient.textContent = "";
    statusWifiClient.classList.add('hide');
    statusWifiClient.classList.remove('blink');
    statusWifiClient.classList.remove('connected');
    btnWifiClientLabel.innerText = "Connect";
    btnWifiClientLabel.classList.remove('connected');
    btnWifiClientLabel.classList.remove('wait');
}
function wifiClientIsConnecting() {
    statusWifiClient.textContent = "- Connecting -";
    statusWifiClient.classList.remove('hide');
    statusWifiClient.classList.add('blink');
    statusWifiClient.classList.remove('connected');
    btnWifiClientLabel.innerText = "Wait";
    btnWifiClientLabel.classList.remove('connected');
    btnWifiClientLabel.classList.add('wait');
}
function wifiClientIsConnected() {
    statusWifiClient.textContent = "- Connected -";
    statusWifiClient.classList.remove('hide');
    statusWifiClient.classList.remove('blink');
    statusWifiClient.classList.add('connected');
    btnWifiClientLabel.innerText = "Disconnect";
    btnWifiClientLabel.classList.add('connected');
    btnWifiClientLabel.classList.remove('wait');
}

/**
 * -------------------------------------------------------------------------
 *  NTRIP caster states.
 * -------------------------------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.3.5 [2026-09-08-10:00pm] New.
 */
function ntripCasterIsNotConnected() {
    statusNtripCaster.textContent = "";
    statusNtripCaster.classList.add('hide');
    statusNtripCaster.classList.remove('blink');
    statusNtripCaster.classList.remove('connected');
    btnNtripCaster.innerText = "Connect";
    btnNtripCaster.classList.remove('connected');
    btnNtripCaster.classList.remove('wait');
}
function ntripCasterIsConnecting() {
    statusNtripCaster.textContent = "- Connecting -";
    statusNtripCaster.classList.remove('hide');
    statusNtripCaster.classList.add('blink');
    statusNtripCaster.classList.remove('connected');
    btnNtripCaster.innerText = "Wait";
    btnNtripCaster.classList.remove('connected');
    btnNtripCaster.classList.add('wait');
}
function ntripCasterIsConnected() {
    statusNtripCaster.textContent = "- Connected -";
    statusNtripCaster.classList.remove('hide');
    statusNtripCaster.classList.remove('blink');
    statusNtripCaster.classList.add('connected');
    btnNtripCaster.innerText = "Disconnect";
    btnNtripCaster.classList.add('connected');
    btnNtripCaster.classList.remove('wait');
}

/**
 * -------------------------------------------------------------------------
 *  Disconnect WiFi client.
 * -------------------------------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.3.5 [2026-09-09-03:15pm] New.
 */
function disconnectWifiClient() {
    websocket.send(DISCONNECT_WIFI_CLIENT);         // Send disconnect message.
    console.log('browser --> ' + DISCONNECT_WIFI_CLIENT);
    messageField.innerText += ('--> disconnectWifiClient\n');
    // Response is handled in 'ntrip page' section of webSocketRcvMessage() in global.js.
}

/**
 * -------------------------------------------------------------------------
 *  Disconnect NTRIP caster.
 * -------------------------------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.3.5 [2026-09-09-03:15pm] New.
 */
function disconnectNtripCaster() {
    websocket.send(DISCONNECT_NTRIP_CASTER);        // Send disconnect message.
    console.log('browser --> ' + DISCONNECT_NTRIP_CASTER);
    messageField.innerText = ('--> disconnectNtripCaster\n');
    // Response is handled in 'ntrip page' section of webSocketRcvMessage() in global.js.
}


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
