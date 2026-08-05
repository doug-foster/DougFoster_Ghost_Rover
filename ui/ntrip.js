/**
 * *************************************************************************
 *  Ghost Rover 3 - UI JS.
 * *************************************************************************
 * 
 * ntrip.js
 *
 * @author D. Foster <doug@dougfoster.me>.
 * @since  3.2.1 [2026-08-01-04:45pm]. New.
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
const statusWifiClient        = document.querySelector('#ntrip #wifi-client-status');
const statusNtripCaster       = document.querySelector('#ntrip #ntrip-caster-status');
const statusNote              = document.querySelector('#ntrip #status.note');
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

// --- Buttons. ---
btnWifiClient.addEventListener('click', () => {

    // -- Test for dependencies. --
    if  (('' == prfHotSsi) || ('' == prfHotPas)) {      // SSIP & password preference must be configured.
        alert('SSID & password NOT SET on config page.');
        return;
    }
    if (statusWifiClient.classList.contains('hide')) {

        // -- Connecting. --
        websocket.send(CONNECT_WIFI_CLIENT);    // Send connect message.
        console.log('browser --> ' + CONNECT_WIFI_CLIENT);
        messageField.innerText = ('--> connectWifiClient\n');
        statusWifiClient.textContent = "- Connecting -";
        statusWifiClient.classList.add('blink');
        statusWifiClient.classList.remove('hide');
        // 'ntrip page' section of webSocketRcvMessage() in global.js will receive & process multiple sequential responses.
    } else {

        // -- Disconnect. --
        websocket.send(DISCONNECT_WIFI_CLIENT);
        console.log('browser --> ' + DISCONNECT_WIFI_CLIENT);
        messageField.innerText = ('--> disconnectWifiClient\n');
        statusWifiClient.classList.add('hide');
        statusWifiClient.classList.remove('blink');
        btnWifiClientLabel.innerText = "Connect";
    }
});
btnNtripCaster.addEventListener('click', () => {
    // btnNtripCasterLabel.classList.toggle('blink');

    // -- Test for dependencies. --
    if  ('ntrip' !== prfRtcIn) {                                    // NTRIP must be set on config page.
        alert('NTRIP NOT SET on config page.');
        return;
    }
    if (!statusWifiClient.textContent.includes('Connected')) {      // WiFi client must be connected.
        alert('WiFi client NOT connected.');
        return;
    }

    if (statusNtripCaster.classList.contains('hide')) {

        // -- Connect to NTRIP caster. --
        websocket.send(CONNECT_NTRIP_CASTER);    // Send connect message.
        console.log('browser --> ' + CONNECT_NTRIP_CASTER);
        messageField.innerText = ('--> connectNtripCaster\n');
        statusNtripCaster.textContent = "- Connecting -";
        statusWifiClient.classList.add('blink');
        statusNtripCaster.classList.remove('hide');
        // messageField.innerText = ('Connecting to NTRIP caster ...\n--> connectNtripCaster\n');

    } else {

        // -- Disconnect from NTRIP caster. --
        websocket.send(DISCONNECT_NTRIP_CASTER);
        console.log('browser --> ' + DISCONNECT_NTRIP_CASTER);
        messageField.innerText = ('--> disconnectNtripCaster\n');
        statusNtripCaster.classList.add('hide');
        statusWifiClient.classList.remove('blink');
        btnNtripCasterLabel.innerText = "Connect";
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
