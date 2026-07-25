/**
 * *************************************************************************
 *  Ghost Rover 3 - UI JS.
 * *************************************************************************
 * 
 * config.js
 *
 * @author D. Foster <doug@dougfoster.me>.
 * @since  3.0.12 [2026-01-31-03:30pm] New.
 * @since  3.0.12 [2026-02-01-06:45pm] Shortened names.
 * @since  3.0.12 [2026-02-06-10:45pm] Add reset.
 * @since  3.0.12 [2026-02-07-07:30am] Add SEND_PREFS.
 * @since  3.0.12 [2026-02-08-06:30pm] Removed prfRqsPvtInt.
 * @since  3.0.12 [2026-02-21-03:30pm] Remove GET_PREFS.
 * @since  3.0.12 [2026-02-22-04:30pm] Change JSON keys from string to alpha integer.
 * @since  3.0.12 [2026-02-25-05:45pm] Websocket send - preserve KV pair order by changing JSON data to array.
 * @since  3.0.12 [2026-02-26-10:30am] Check for missing preference.
 * @since  3.1.0  [2026-03-02-05:00pm] Stable 3.0 version.
 * @since  3.1.0  [2026-03-20-11:45am] Update var names, add pole height.
 * @since  3.1.1  [2026-06-25-02:00pm] Regroup: upload to SD card.
 * @since  3.0.12 [2026-02-28-09:00pm] WS_PREF_GNSS_MESASURE_INTERVAL -> WS_PREF_GNSS_MEASURE_INTERVAL.
 * @since  3.1.1  [2026-06-29-03:45pm] Refactor.
 * @since  3.1.1  [2026-07-01-06:30pm] window.location.reload() after resetting default preferences.
 * @since  3.1.2  [2026-07-05-08:30pm] General cleanup.
 * @since  3.1.2  [2026-07-07-01:00pm] Instrument height always in mm.
 * @since  3.1.2  [2026-07-14-09:45am] Add NTRIP.
 * @since  3.2.1  [2026-07-25-04:00pm] Moved JSON to global.js.
 * @link   http://dougfoster.me.
*/

/**
 * =========================================================================
 *  Global vars.
 * =========================================================================
 *
 * @since 3.0.12  [2026-01-31-01:30pm] New.
 * @since 3.0.12  [2026-02-01-02:00pm] Shortened names.
 * @since 3.0.12  [2026-02-06-10:45pm] Add reset.
 * @since 3.0.12  [2026-02-07-11:00am] Add clearMessageField().
 * @since 3.0.12  [2026-02-21-03:30pm] Remove GET_PREFS.
 * @since  3.0.12 [2026-02-25-05:45pm] Websocket send - preserve KV pair order by changing JSON data to array.
 * @since  3.1.0  [2026-03-20-12:00pm] Pole heights.
 * @since  3.1.2  [2026-07-21-02:00pm] Add NTRIP.
 */

const gnssMeasureInterval     = document.querySelector('#config #gnss-measure-interval');
const gnssNavRate             = document.querySelector('#config #gnss-nav-rate');
const outputInterval          = document.querySelector('#config #output-interval');
const hotspotSsid             = document.querySelector('#config #hotspot-ssid');
const hotspotPassword         = document.querySelector('#config #hotspot-password');
const updateConfigBtn         = document.querySelector('#config #update-config-btn');
const resetConfigBtn          = document.querySelector('#config #reset-config-btn');
const updateCasterBtn         = document.querySelector('#config #ntrip-update-attributes');
const messageForm             = document.querySelector('#config #message-form');
const formConfig              = document.querySelector('form#config');
const jsConsoleMessages       = document.querySelector('#js-console-messages input');
const ghostRoverHeight        = document.querySelector('#config #ghostrover-height');
const poleSelect              = document.querySelector('#config #pole-select');
const poleSelectOptions       = document.querySelectorAll('#config #pole-select option');
const poleSelectGrip          = document.querySelector('#config #pole-select #grip');
const poleSelectXYZ0          = document.querySelector('#config #pole-select #xyzPole-0');
const poleSelectXYZ1          = document.querySelector('#config #pole-select #xyzPole-1');
const poleSelectXYZ2          = document.querySelector('#config #pole-select #xyzPole-2');
const poleSelectXYZ3          = document.querySelector('#config #pole-select #xyzPole-3');
const poleHeightPreset        = document.querySelector('#config #pole-height-preset');
const poleHeight              = document.querySelector('#config #pole-height');
const instrumentHeight        = document.querySelector('#config #instrument-height');
const instrumentHeightMm      = document.querySelector('#config #instrument-height-mm');
const instrumentHeightIn      = document.querySelector('#config #instrument-height-in');
const heightUnitsVal          = document.querySelector('#config #height-units');
const ntripCaster             = document.querySelector('#config #ntrip-caster');
const ntripName               = document.querySelector('#config #ntrip-name');
const ntripUrl                = document.querySelector('#config #ntrip-url');
const ntripMount              = document.querySelector('#config #ntrip-mount');
const ntripPort               = document.querySelector('#config #ntrip-port');
const ntripVersion            = document.querySelector('#config #ntrip-version');
const ntripUser               = document.querySelector('#config #ntrip-user');
const ntripPassword           = document.querySelector('#config #ntrip-password');
const ntripSendGGA            = document.querySelector('#config #ntrip-send-gga input');
const ntripCasterdup          = document.querySelector('#config #ntrip-caster-dup');
const switchRtcmInButtons     = document.querySelector('#rtcm-in-buttons');
const ntripCasterActive       = document.querySelector('#ntrip-caster-active');
const SEND_PREFS              = '{"page":"config","sendPrefs":""}';
const PREF_RESET              = '{"page":"config","resetPrefs":""}';
let   ntripCasterAttributes   = [];
let   prfNtripCasterAct       = 0;
        
/**
 * =========================================================================
 *  Functions.
 * =========================================================================
 *
 * @since 3.0.12 [2026-01-31-11:00am].
 * @since 3.0.12 [2026-02-07-07:30am] Add SEND_PREFS.
 * @since 3.2.1  [2026-07-25-11:45am] Remove uiToPrefs().
 * @since 3.2.1  [2026-07-25-04:00pm] Remove prefsMessage().
 * @see   update()            - Update server.
 * @see   clearMessageField() - Clear message field.
 * @see   setHeights()        - Set & compute instrument height values.
 * @see   ntripAttributes()   - Save/display NTRIP caster attibutes.
 */

/**
 * -------------------------------------------------------------------------
 *  Update server.
 * -------------------------------------------------------------------------
 * 
 * @return void  No output is returned.
 * @since  3.0.12 [2026-01-31-03:30pm] New.
 * @since  3.0.12 [2026-02-21-03:30pm] Remove GET_PREFS.
 * @see    webSocketOpened() in global.js. Update() is different for every page.js.
 */
function update() {
    websocket.send(SEND_PREFS);  // Send SEND_PREFS message.
    console.log('browser --> ' + SEND_PREFS);
}

/**
 * -------------------------------------------------------------------------
 *  Clear message field.
 * -------------------------------------------------------------------------
 * 
 * @return void  No output is returned.
 * @since  3.0.12 [2026-02-07-11:00am] New.
 */
function clearMessageField() {
    setTimeout(function() { messageForm.innerHTML = "&nbsp;"; }, 6000);
}

/**
 * -------------------------------------------------------------------------
 *  Set & compute instrument height values.
 * -------------------------------------------------------------------------
 *
 * const heightUnitsVal      = document.querySelector('#config #height-units');
 * const ghostRoverHeight    = document.querySelector('#config #ghostrover-height');
 * const poleSelect          = document.querySelector('#config #pole-select');
 * const poleSelectGrip      = document.querySelector('#config #pole-select #grip');
 * const poleSelectXYZ0      = document.querySelector('#config #pole-select #xyzPole-0');
 * const poleSelectXYZ1      = document.querySelector('#config #pole-select #xyzPole-1');
 * const poleSelectXYZ2      = document.querySelector('#config #pole-select #xyzPole-2');
 * const poleSelectXYZ3      = document.querySelector('#config #pole-select #xyzPole-3');
 * const poleHeightPreset    = document.querySelector('#config #pole-height-preset');
 * const poleHeight          = document.querySelector('#config #pole-height');
 * const instrumentHeight    = document.querySelector('#config #instrument-height');
 * const instrumentHeightMm  = document.querySelector('#config #instrument-height-mm');
 * const instrumentHeightIn  = document.querySelector('#config #instrument-height-in');
 * const jsConsoleMessages   = document.querySelector('#js-console-messages input');
 *
 * @param  string action Action to take.
 * @return void   No output is returned.
 * @since  3.1.0 [2026-03-08] New.
 * @since  3.1.0 [2026-03-19-11:00am] Update var names.
 * @since  3.1.0 [2026-03-20-12:45pm] Pole heights.
 * @since  3.1.0 [2026-03-21-10:45pm] Refactor.
 * @since  3.1.1 [2026-06-29-03:45pm] Refactor.
 * @see    global.js.
 */
function setHeights(action) {

    let usedStdPoleHeight = false;

    switch (action) {
        case 'init':
            heightUnitsVal.textContent     = heightUnits;
            ghostRoverHeight.textContent   = HEIGHT_ROVER;
            poleSelectGrip.value           = HEIGHT_GRIP_TRIPOD;
            poleSelectXYZ0.value           = HEIGHT_XYZPOLE_0;
            poleSelectXYZ1.value           = HEIGHT_XYZPOLE_1;
            poleSelectXYZ2.value           = HEIGHT_XYZPOLE_2;
            poleSelectXYZ3.value           = HEIGHT_XYZPOLE_3;
            instrumentHeightMm.textContent = prfInstrHght.toLocaleString();                         // NVS preference from WebSocket.
            instrumentHeightIn.textContent = (parseFloat(instrumentHeightMm.textContent.replace(',', ''))/25.4).toFixed(1);
            poleSelectOptions.forEach((option) => {                                                 // Loop all select options.
                if ((parseInt(option.value) + HEIGHT_ROVER) == prfInstrHght) {
                    if (0 !== parseInt(option.value)) {
                        poleSelect.value = parseInt(option.value);                                  // Set select value to option used.
                        poleHeightPreset.textContent = parseInt(option.value).toLocaleString();     // Display the selected option value.
                        usedStdPoleHeight = true;                                                   // Set the flag for select option was/wasn't used.
                    }
                }
            });
            if(!usedStdPoleHeight) {                                                                // A select option was NOT used (non-standard - aka "custom" pole height).
                poleHeightPreset.classList.add('hide');                                             // Value for the selected pole option - do not display.
                poleHeight.value = (prfInstrHght - HEIGHT_ROVER).toLocaleString();                  // Calculate and display the "custom" pole height.
                // ".toLocaleString()" displays numbers > 999 with a comma: e.g. 1,483 instead of 1483.
            } else {                                                                                // A select option WAS used (standard pole height).
                poleHeightPreset.classList.remove('hide');
                poleHeight.classList.add('hide');
            }
            break;
        case 'change':
            if ('0' === poleSelect.value) {                                                         // "Custom" pole height option.
                poleHeightPreset.classList.add('hide');
                poleHeight.classList.remove('hide');
            } else {
                poleHeightPreset.classList.remove('hide');
                poleHeight.classList.add('hide');                                                   // "Standard" pole height option.
            }
            heightPole = parseFloat(poleSelect.value);
            poleHeightPreset.textContent   = heightPole.toLocaleString();  
            poleHeight.value               = heightPole;
            instrumentHeightMm.textContent = (parseFloat(poleHeight.value.replace(',', '')) + parseFloat(HEIGHT_ROVER)).toLocaleString();
            instrumentHeightIn.textContent = (parseFloat(instrumentHeightMm.textContent.replace(',', ''))/25.4).toFixed(1);
            break;
        case 'compute':
            instrumentHeightMm.textContent = (parseFloat(poleHeight.value.replace(',', '')) + parseFloat(HEIGHT_ROVER)).toLocaleString();  // Calculate a new instrument height.
            instrumentHeightIn.textContent = (parseFloat(instrumentHeightMm.textContent.replace(',', ''))/25.4).toFixed(1);
            break;
    }
    messageForm.textContent = 'Instrument height calculated.';
    clearMessageField();
}

/**
 * -------------------------------------------------------------------------
 *  Save/display NTRIP caster attibutes.
 * -------------------------------------------------------------------------
 * 
 * Values for all key/value pairs are sent as alpha characters (not int).
 *
 * @param  string Action to be performed (save, load).
 * @return void   No output is returned.
 * @since  3.1.2 [2026-07-17-09:30pm] New.
 * @since  3.1.2 [2026-07-17-09:30pm] ntripCasterAttributes stored as string, not array.
 * @since  3.2.1 [2026-07-25-04:00pm] Moved JSON to webSocketRcvMessage() & toJson() in global.js.
 * @see    uiPrefs().
 */
function ntripAttributes(action) {

    // --- Caster # on "Save atttributes" button must match # caster for displayed attributes. 
    ntripCasterdup.textContent = ntripCaster.value;

    // --- Update NVS caster attribute preferences. ---
    if ('save' == action) {
        ntripCasterAttributes[parseInt(ntripCaster.value)] = toJson('ntripAttributes');
        websocket.send(ntripCasterAttributes[parseInt(ntripCaster.value)]);
        console.log('browser --> ' + ntripCasterAttributes[parseInt(ntripCaster.value)]);
        messageForm.textContent = 'Caster ' + ntripCaster.value + ' preference saved.';
        clearMessageField();
    } else if ('load' == action) {

        // --- Load UI form fields for selected NTRIP caster. ---
        let which;
        if (ntripCaster.value == prfNtripCasterAct) {
            which                   = prfNtripCasterAct;
            messageForm.textContent = 'Preferences loaded.';
        } else {
            which = ntripCaster.value;
        }
        ntripName.value      = caster[parseInt(which)].name;
        ntripUrl.value       = caster[parseInt(which)].url;
        ntripMount.value     = caster[parseInt(which)].mount;
        ntripPort.value      = caster[parseInt(which)].port;
        ntripVersion.value   = caster[parseInt(which)].version;
        ntripUser.value      = caster[parseInt(which)].user;
        ntripPassword.value  = caster[parseInt(which)].pass;
        ntripSendGGA.checked = Boolean(caster[parseInt(which)].sendGga);
        clearMessageField();
    }
}

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
 * @since  3.0.12 [2026-01-31-01:30pm] New.
 * @since  3.0.12 [2026-02-07-11:00am] Add clearMessageField().
 * @since  3.1.0  [2026-03-08-12:30pm] Add setHeights().
 * @since  3.1.0  [2026-03-20-11:15am] Update var names.
 * @see    global.js: Global vars & webSocketInit().
 */
 document.addEventListener('DOMContentLoaded', (event) => {
    webSocketInit();
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
 * -------------------------------------------------------------------------
 *  Switches.
 * -------------------------------------------------------------------------
 *
 * @return void  No output is returned.
 * @since 3.0.12 [2026-01-31-01:30pm] New.
 * @since 3.0.12 [2026-02-01-06:00pm] Shortened names.
 * @since 3.1.2  [2026-02-21-03:45pm] Add NTRIP.
 */
ntripCasterActive.addEventListener('change', (event) => {
    prfNtripCasterAct = event.target.value;
    ntripCaster.value = prfNtripCasterAct;
    ntripAttributes('load');
});

/**
 * -------------------------------------------------------------------------
 *  Buttons.
 * -------------------------------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.12 [2026-02-06-10:45pm] New.
 * @since  3.0.12 [2026-02-07-11:00am] Add clearMessageField().
 * @since  3.1.2  [2026-07-14-09:30am] Add updateCasterBtn listener.
 */
updateConfigBtn.addEventListener('click', () => {
    updateConfigBtn.classList.add('shadow');    // Visual feedback.
    setTimeout(function() { updateConfigBtn.classList.remove('shadow'); }, 100);
    if ((websocket) && (1 == websocket.readyState)) {
        websocket.send( toJson('uiToPrefs') );          // Send "setPrefs" message to rover.
    }
    console.log('browser --> ' + toJson('uiToPrefs'));
    messageForm.textContent = 'Preferences saved.';
    setTimeout(function() { window.location.reload(); }, 2000);
    clearMessageField();
});

resetConfigBtn.addEventListener('click', () => {
    updateConfigBtn.classList.add('shadow');    // Visual feedback.
    setTimeout(function() { updateConfigBtn.classList.remove('shadow'); }, 100);
    if ((websocket) && (1 == websocket.readyState)) {
        let confirmed = confirm("Are you sure?");
        if (confirmed) {
            websocket.send(PREF_RESET);  // Send "resetPrefs" message to rover.
            console.log('browser --> ' + PREF_RESET);
            messageForm.textContent = 'Preferences reset.';
            setTimeout(function() { window.location.reload(); }, 2000);
        } else {
            messageForm.textContent = 'Reset cancelled.';
            clearMessageField();
        }
    }
});

updateCasterBtn.addEventListener('click', () => {
    updateCasterBtn.classList.add('shadow');    // Visual feedback.
    setTimeout(function() { updateCasterBtn.classList.remove('shadow'); }, 100);
    ntripAttributes('save');
});

/**
 * -------------------------------------------------------------------------
 *  Other.
 * -------------------------------------------------------------------------
 *
 * @return void   No output is returned.
 * @since  3.0.12 [2026-02-08-08:45pm] New.
 * @since  3.1.2  [2026-07-21-02:30pm] Add switchNtripCasterActive &  ntripCaster listeners.
 */
jsConsoleMessages.addEventListener('change', () => {
    if (jsConsoleMessages.checked) {
        sessionStorage.setItem("displayJsConsoleMessages", 'on');
    } else {
        sessionStorage.setItem("displayJsConsoleMessages", 'off');
    }
    console.log('Show console messages is "' + sessionStorage.getItem("displayJsConsoleMessages") + '".');
});

poleSelect.addEventListener('change', () => {
    setHeights('change');
});

poleHeight.addEventListener('blur', () => {
    setHeights('compute');
});

gnssMeasureInterval.addEventListener('change', () => {
    outputInterval.textContent = gnssMeasureInterval.value * gnssNavRate.value ;
});

gnssNavRate.addEventListener('change', () => {
    outputInterval.textContent = gnssMeasureInterval.value * gnssNavRate.value ;
});

ntripCaster.addEventListener('change', (event) => {
    ntripAttributes('load');
});

/**
 * =========================================================================
 *  Test.
 * =========================================================================
 *
 * @since  3.0.12 [2026-01-31-01:30pm].
 */

/**
 * =========================================================================
 *  Run on page load.
 * =========================================================================
 *
 * @since  3.0.12 [2026-01-31-01:30pm].
 */

// Display a simple summary value for how often output happens. 
outputInterval.textContent = gnssMeasureInterval.value * gnssNavRate.value ;
