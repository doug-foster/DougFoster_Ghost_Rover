/**
 * *************************************************************************
 *  Ghost Rover 3 - UI JS.
 * *************************************************************************
 * 
 * global.js
 *
 * @author D. Foster <doug@dougfoster.me>.
 * @since  3.0.11 [2026-01-18-03:45pm] Basic functionality.
 * @since  3.0.11 [2026-01-20-07:00pm] Altitude to height, lock to lock-unlock.
 * @since  3.0.11 [2026-01-22-11:00am] Websocket tweaks.
 * @since  3.0.12 [2026-01-27-06:15pm] Changed wsEndpoint from static to dynamic.
 * @since  3.0.12 [2026-01-28-02:30pm] Cleanup.
 * @since  3.0.12 [2026-02-15-03:30pm] Moved reconnect from webSocketClosed().
 * @since  3.0.12 [2026-02-17-10:00am] Change location to position.
 * @since  3.0.12 [2026-02-19-04:00pm] Removed leaving message.
 * @since  3.0.12 [2026-02-28-02:15pm] Add WS_SOCKET_NUM.
 * @since  3.1.0  [2026-03-02-05:00pm] Stable 3.0 version.
 * @since  3.1.0  [2026-03-20-11:15am] Update var names.
 * @since  3.1.1  [2026-06-25-02:00pm] Regroup: upload to SD card.
 * @since  3.1.1  [2026-06-26-09:30pm] HEIGHT_QUICK_RELEASE, changed height values.
 * @since  3.1.1  [2026-06-29-03:45pm] CHanged NVS pref from pole height to instument height.
 * @since  3.1.2  [2026-07-05-09:30pm] General cleanup.
 * @since  3.2.1  [2026-07-25-04:15pm] Move JSON to webSocketRcvMessage() & toJson().
 * @since  3.2.1  [2026-07-25-05:00pm] Convert NTRIP keys from alpha to numeric.
 * @link   http://dougfoster.me.
*/

/**
 * =========================================================================
 *  Global vars.
 * =========================================================================
 *
 * @since  3.0.8 [2025-11-21-09:00am].
 * @since  3.0.12 [2026-02-09-01:45pm].
 * @since  3.0.12 [2026-02-28-02:15pm] Add WS_SOCKET_NUM.
 * @since  3.1.0  [2026-03-20-11:15am] Update var names.
 * @since  3.1.1  [2026-06-26-09:30pm] HEIGHT_QUICK_RELEASE, changed height values.
 * @since  3.1.1  [2026-06-26-09:30pm] change WS_PREF_GNSS_MESASURE_INTERVAL to WS_PREF_GNSS_MEASURE_INTERVAL.
 * @since  3.1.2  [2026-07-16-10:00am] Add NTRIP.
 * @since  3.2.1  [2026-07-25-04:30pm] Add caster[{}].
 * @see    operateMessage() in operate.js.
 * @see    setHeights() in config.js.
 * @see    Global vars () WebSockets) in DougFoster_Ghost_Rover.ino.
 */

// --- Test. ---
// const uploadUrl = 'https://httpbin.org/post';
// const sdFiles   = ['file1.txt', 'file2.txt', 'file3.txt', 'file4.txt'];  // Test data.

// --- HTTP. ---
let ws_target = '';
if (window.location.hostname == '127.0.0.1') {     // VS Code Live Server.
    ws_target = '192.168.23.1';
} else {
    ws_target = window.location.hostname;
}
const wsEndpoint = 'ws://' + ws_target + '/ghostRover';

// --- Battery. ---
let batterySoc;

// --- General. ---
const newPage                 = document.querySelectorAll('a.new-page');
const RECONNECT_INTERVAL      = 2000;    // Server reconnect interval.
let wsNumBytesThisMessage     = 0;       // # of bytes in this WebSocket message. @see webSocketRcvMessage(). 

// --- Header. ---
const ROVER_NAME              = 'GhostRover';
const headerH1                = document.querySelector('header h1');
const versionRoverId          = document.querySelector('#version-rover');
const statusUnitDisplayId     = document.querySelector('#unit-display');

// --- WebSocket. ---
// @see "JSON key index" in webSocketRcvMessage for exchange protocol.
let   websocket;
const caster = [
  { },
  { id:'', name:'', url:'', mount:'', port:'', version:1, user:'', pass:'', sendGga:0 },
  { id:'', name:'', url:'', mount:'', port:'', version:1, user:'', pass:'', sendGga:0 },
  { id:'', name:'', url:'', mount:'', port:'', version:1, user:'', pass:'', sendGga:0 },
];

// --- Preferences. ---
// let prfGnsMsrInt = 0;
// let prfGnsNavRat = 0;

// --- SFESPK6618H antenna phase center offsets. ---
// https://community.sparkfun.com/t/spk6618h-antenna-north-marker/68211/5
// Frequency  North Offset (mm)  East Offset (mm) Up Offset (mm)
// L1 (GPS)	    +0.47	      -1.26	     48.02
// L2 (GPS)	    +2.73	      -1.87	     35.91
// L5 (GPS)	    +3.16	      -2.02	     36.91
// Since North & East offsets are so small, ignore them.
const HEIGHT_APC_TO_ARP      =   48;    // Antenna phase center to Antenna Reference Position height (mm).
const HEIGHT_ARP_TO_QR_PLATE =   66;    // Antenna Reference Position to bottom of FALCAM F38 Quick Release plate height (mm).
const HEIGHT_QUICK_RELEASE   =   14;    // FALCAM F38 Quick Release total height (mm).
                                        // Quick Release plate height                  ( 0.25 inch =    6.4 mm).
                                        // Quick Release receiver height               ( 0.31 inch =    7.9 mm).
                                        // Quick Release total height                  ( 0.56 inch =    14.3mm).
const HEIGHT_GRIP_TRIPOD     =  166;    // Gun grip + washer + Zeadio tripod           ( 6.53 inch =  165.8 mm).
const HEIGHT_XYZPOLE_0       =  691;    // SingularXYZ pole - no extensions out        (27.19 inch =  690.6 mm).
const HEIGHT_XYZPOLE_1       = 1073;    // SingularXYZ pole - top 1 extension out      (42.25 inch = 1073.2 mm).
const HEIGHT_XYZPOLE_2       = 1468;    // SingularXYZ pole - top 1 & 2 extensions out (57.81 inch = 1468.4 mm).
const HEIGHT_XYZPOLE_3       = 1819;    // SingularXYZ pole - all 3 extensions out     (71.60 inch = 1818.6 mm).
const HEIGHT_ROVER           = HEIGHT_APC_TO_ARP + HEIGHT_ARP_TO_QR_PLATE + HEIGHT_QUICK_RELEASE;  // 48 + 66 + 14 = 128.
let heightUnits              = 'mm';
let heightPole               =    0;    // mm.

/**
 * =========================================================================
 *  Functions.
 * =========================================================================
 *
 * @since  3.0.3 [2025-10-16-01:45pm].
 * @since  3.1.0 [2026-03-20-11:15am] Update var names.
 * @since  3.2.1 [2026-07-25-04:30pm] add toJson().
 * @see   webSocketInit()       - WebSocket: init.
 * @see   webSocketOpened()     - WebSocket: opened.
 * @see   webSocketClosed()     - WebSocket: closed.
 * @see   webSocketError()      - WebSocket: error.
 * @see   webSocketStop()       - WebSocket: stopped.
 * @see   webSocketRcvMessage() - WebSocket: message from server. Decode.
 * @see   toJson()              - WebSocket: Encode values into JSON.
 */

/**
 * -------------------------------------------------------------------------
 *  WebSocket: init.
 * -------------------------------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.3 [2025-10-13-02:15pm].
 * @since  3.1.0 [2026-03-20-11:15am] Update var names.
 */
function webSocketInit() {
    console.log('Opening new WebSocket ...');
    websocket           = new WebSocket(wsEndpoint);
    websocket.onopen    = webSocketOpened;
    websocket.onclose   = webSocketClosed;
    websocket.onerror   = webSocketError;
    websocket.onmessage = webSocketRcvMessage;
}

/**
 * -------------------------------------------------------------------------
 *  WebSocket: opened.
 * -------------------------------------------------------------------------
 *
 * @return void  No output is returned.
 * @since. 3.0.7  [2025-11-15-12:30pm].
 * @since. 3.0.10 [2026-01-07-05:30pm] Removed ready handshake.
 * @since  3.1.0  [2026-03-20-11:15am] Update var names.
 */
function webSocketOpened(event) {

    // --- UI indicator that a WebSocket is now open. ---
    console.log('WebSocket opened to ' + wsEndpoint + '.');
    headerH1.textContent = ROVER_NAME;
    headerH1.classList.remove('red');
    update();   // Different for every page.js.
}

/**
 * -------------------------------------------------------------------------
 *  WebSocket: closed.
 * -------------------------------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.3  [2025-10-15-01:15pm] New.
 * @since  3.0.11 [2026-01-22-10:15am] Add reconnect.
 * @since  3.0.12 [2026-02-15-03:30pm] Moved reconnect to DOMContentLoaded event listener.
 * @since  3.1.0  [2026-03-20-11:15am] Update var names.
 */
function webSocketClosed(event) {
    console.log('WebSocket closed.');
    headerH1.textContent = 'No server';
    headerH1.classList.add('red');
}

/**
 * -------------------------------------------------------------------------
 *  WebSocket: error.
 * -------------------------------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.3 [2025-10-15-01:15pm].
 * @since  3.1.0  [2026-03-20-11:15am] Update var names.
 */
function webSocketError() {
    if (!headerH1.classList.contains('red')) {
        headerH1.classList.add('red');
    }
}
/**
 * -------------------------------------------------------------------------
 *  WebSocket: stopped.
 * -------------------------------------------------------------------------
 * 
 * A new websocket is opened each time a page is loaded: and closed each time a page is left.
 *
 * @return void    No output is returned.
 * @since  3.0.3  [2025-10-22-01:30pm].
 * @since  3.0.11 [2026-01-21-09:00am] Check websocket.readyState.
 * @since  3.0.12 [2026-01-31-03:15pm] Refactored.
 * @since  3.0.12 [2026-02-19-04:00pm] Removed leaving message.
 * @since  3.1.0  [2026-03-20-11:15am] Update var names.
 */
async function webSocketStop(event) {
    let waitToClose = new Promise(function(resolve) {           // Link to new page was prevented.
        if ((websocket) && (1 == websocket.readyState)) {
            websocket.close();                                  // Close socket.
        }
        setTimeout(function() {
            window.location = event.target.closest('a').href;   // Continue to link target.
            }, 100);
    });
    await waitToClose;
}

/**
 * -------------------------------------------------------------------------
 *  WebSocket: message from server. Decode.
 * -------------------------------------------------------------------------
 * 
 * --- JSON key index. ---
 *     0  = Build info.
 *     1  = Units                           (prfUnt).
 *     2  = RTCM in source                  (prfRtcIn).
 *     3  = NMEA out - on/off               (prfNmeOut).
 *     4  = GNSS measure interval           (prfGnsMsrInt).
 *     5  = GNSS navigation rate            (prfGnsNavRat).
 *     6  = WiFi hot spot SSID              (prfHotSsi).
 *     7  = WiFi hot spot password          (prfHotPas).
 *     8  = GNSS fix.
 *     9  = GNSS satellites in view.
 *     10 = GNSS ellipsoid height.
 *     11 = GNSS orthometric height .
 *     12 = GNSS latitude.
 *     13 = GNSS longitude.
 *     14 = GNSS horizontal accuracy.
 *     15 = GNSS vertical accuracy.
 *     16 = RTCM in status - up/down.
 *     17 = NMEA out status - up/down.
 *     18 = Battery State Of Charge (SOC).
 *     19 = Battery change rate.
 *     20 = Up time.
 *     21 = RTCM in count all               Not used?
 *     22 = RTCM in rate                    Not used?
 *     23 = NMEA GGA out sentence count.
 *     24 = NMEA RMC out sentence count.
 *     25 = NMEA GSA out sentence count.
 *     26 = NMEA GSV out sentence count.
 *     27 = NMEA GST out sentence count.
 *     28 = NMEA TXT out sentence count.
 *     29 = NMEA other out sentence count.
 *     30 = NMEA total out sentence count.
 *     31 = NMEA out rate.
 *     32 = Operational mode.
 *     33 = WiFi local network IP address.
 *     34 = WiFi hot spot address           Not used?
 *     35 = WebSocket client/session id.
 *     36 = Instrument height.
 *     37 = RTCM sentence count.
 *     38 = RTCM rate.
 *     39 = NTRIP caster #1 attributes      (prfNtripCastAttr[0]).
 *     40 = NTRIP caster #2 attributes      (prfNtripCastAttr[1]).
 *     41 = NTRIP caster #3 attributes      (prfNtripCastAttr[2]).
 *     42 = NTRIP caster active [1/2/3]     (prfNtripCastAct).
 *     43 = NTRIP caster id                 (caster[1/2/3].id).
 *     44 = NTRIP caster name               (caster[1/2/3].name).
 *     45 = NTRIP caster url                (caster[1/2/3].url).
 *     46 = NTRIP caster mount point        (caster[1/2/3].mount).
 *     47 = NTRIP caster port               (caster[1/2/3].port).
 *     48 = NTRIP caster version            (caster[1/2/3].version).
 *     49 = NTRIP caster user               (caster[1/2/3].user).
 *     50 = NTRIP caster password           (caster[1/2/3].pass).
 *     51 = NTRIP caster sendGga            (caster[1/2/3].sendGga).
 *
 * @return void  No output is returned.
 * @since  3.0.7 [2025-11-15-02:00pm].
 * @since  3.0.10 [2026-01-07-02:30pm] Check for null event data.
 * @since  3.0.12 [2026-01-28-09:00pm] Add numWsMessages.
 * @since  3.0.12 [2026-01-30-05:00pm] Add prefsMessage().
 * @since  3.0.12 [2026-02-07-12:30pm] Add nmeaMessage().
 * @since  3.1.0  [2026-03-20-11:15am] Update var names.
 * @see    operateMessage() in operate.js.
 * @see    filesMessage() in files.js.
 */
function webSocketRcvMessage(event) {

    // --- Process message. ---
    let jsonObj = JSON.parse(event.data);
    let response = 'browser <-- ' + event.data
    wsNumBytesThisMessage  = event.data.length;     // Bytes per message.
    if (sessionStorage.getItem("displayJsConsoleMessages") == 'on') {
        console.log(response);
    }

    // -- Page header. --
    if (undefined !== jsonObj["0"]) {
        versionRoverId.innerHTML = jsonObj["0"];
    }
    if (undefined !== jsonObj["1"]) {
        switch (jsonObj["1"]) {
            case 'meter':
                statusUnitDisplayId.innerHTML = 'Meter';
                break;
            case 'feet':
                statusUnitDisplayId.innerHTML = 'Feet';
                convert = 3.2808399;
                break;
            default:
                statusUnitDisplayId.innerHTML = jsonObj["1"];
                break;
        }
    }

    // -- Config page. --
    if ((window.location.pathname.includes('config') && (Object.keys(jsonObj).length > 1))) {

        // - "1". -
        prfUnt = jsonObj["1"];
        document.querySelector('input[name="switch-unit"][value="' + jsonObj["1"] + '"]').checked = true;
        if ('feet' === prfUnt) {
            heightUnits = 'in';
        };

        // - "2". -
        prfRtcIn = jsonObj["2"];
        document.querySelector('input[name="switch-rtcm-in"][value="' + jsonObj["2"] + '"]').checked = true;

        // - "3". -
        prfNmeOut = jsonObj["3"];
        document.querySelector('input[name="switch-nmea-out"][value="' + jsonObj["3"] + '"]').checked = true;

        // - "4". -
        prfGnsMsrInt              = jsonObj["4"];
        gnssMeasureInterval.value = jsonObj["4"];
        outputInterval.textContent = gnssMeasureInterval.value * gnssNavRate.value ;

        // - "5". -
        prfGnsNavRat      = jsonObj["5"];
        gnssNavRate.value = jsonObj["5"];
        outputInterval.textContent = gnssMeasureInterval.value * gnssNavRate.value ;

        // - "6". -
        prfHotSsi         = jsonObj["6"];
        hotspotSsid.value = jsonObj["6"];

        // - "7. -
        prfHotPas             = jsonObj["7"];
        hotspotPassword.value = jsonObj["7"];

        // - "36". -
        prfInstrHght = jsonObj["36"];
        setHeights('init');

        // - "39. -
        ntripCasterAttributes[1] = jsonObj["39"];

        // - "40". -
        ntripCasterAttributes[2] = jsonObj["40"];

        // - "41". -
        ntripCasterAttributes[3] = jsonObj["41"];

        // - "42". -
        document.querySelector('input[name="switch-ntrip-caster-active"][value="' + jsonObj["42"] + '"]').checked = true;
        prfNtripCasterAct = jsonObj["42"];
        ntripCaster.value = jsonObj["42"]; // Display the caster that matches the saved preference.

        // - Load caster array. ntripAttributes() uses caster array values to set UI fields.
        for (let i = 1; i < ntripCasterAttributes.length; i++) {  // Array element 0 is not used. All alpha values.
            let jsonObj = JSON.parse(ntripCasterAttributes[i]);
            caster[i].id      = (undefined == jsonObj["43"]) ? '' : jsonObj["43"];
            caster[i].name    = (undefined == jsonObj["44"]) ? '' : jsonObj["44"];
            caster[i].url     = (undefined == jsonObj["45"]) ? '' : jsonObj["45"];
            caster[i].mount   = (undefined == jsonObj["46"]) ? '' : jsonObj["46"];
            caster[i].port    = (undefined == jsonObj["47"]) ? '' : jsonObj["47"];
            caster[i].version = (undefined == jsonObj["48"]) ? '' : jsonObj["48"];
            caster[i].user    = (undefined == jsonObj["49"]) ? '' : jsonObj["49"];
            caster[i].pass    = (undefined == jsonObj["50"]) ? '' : jsonObj["50"];
            caster[i].sendGga = ntripSendGGA.checked = Boolean(jsonObj["51"]);
        }

        // - Load UI fields. =
        ntripAttributes('load');
    }

    // --- Deprecated. // ToDo: Replace & dump. ---
    if ('null' !== event.data) {
        Object.entries(jsonObj).forEach(([key, value]) => {

            // -- Route each message to its page. --
            if (window.location.pathname.includes('operate')) {
                operateMessage(key, value);         // operate.js.
            } else if (window.location.pathname.includes('files')) {
                filesMessage(key, value);           // files.js
            } else if (window.location.pathname.includes('nmea')) {
                nmeaMessage(key, value);            // nmea.js
            }
        });
    }
}

/**
 * -------------------------------------------------------------------------
 *  WebSocket: Encode values into JSON.
 * -------------------------------------------------------------------------
 *
 * @param  which Group of prefs to apply.
 * @return void  No output is returned.
 * @since  3.1.2  [2026-07-25-04:15pm] New.
 * @see    webSocketRcvMessage() in global.js.
 * @see    ntripAttributes() in config.js.
 * @see    updateConfigBtn.addEventListener() in config.js.
 * @see    global vars in global.js.
 */
function toJson(which) {
    let jsonString;
    switch (which) {
        case 'uiToPrefs':
            const switchUnits             = document.querySelector('input[name="switch-unit"]:checked')?.value;
            const switchRtcmIn            = document.querySelector('input[name="switch-rtcm-in"]:checked')?.value;
            const switchNtripCasterActive = document.querySelector('input[name="switch-ntrip-caster-active"]:checked')?.value;
            const switchNmeaOut           = document.querySelector('input[name="switch-nmea-out"]:checked')?.value;
            jsonString = JSON.stringify( {
                "setPrefs" : "", 
                       "1" : switchUnits,                                         // prfUnt.
                       "2" : switchRtcmIn,                                        // prfRtcIn.
                       "3" : switchNmeaOut,                                       // prfNmeOut.
                       "4" : gnssMeasureInterval.value,                           // prfGnsMsrInt
                       "5" : gnssNavRate.value,                                   // prfGnsNavRat.
                       "6" : hotspotSsid.value,                                   // prfHotSsi.
                       "7" : hotspotPassword.value,                               // prfHotPas.
                      "36" : instrumentHeightMm.textContent.replace(',', ''),     // prfInstrHght.
                      "39" : ntripCasterAttributes[1],                            // ntripCasterAttributes[1].
                      "40" : ntripCasterAttributes[2],                            // ntripCasterAttributes[2].
                      "41" : ntripCasterAttributes[3],                            // ntripCasterAttributes[3].
                      "42" : switchNtripCasterActive                              // prfNtripCasterAct.
            } )
            break;
        case 'ntripAttributes':
            jsonString = JSON.stringify( {
                            "config" : "setNtripCasterPref",
                "setNtripCasterPref" : JSON.stringify( {
                                "43" : ntripCaster.value,
                                "44" : ntripName.value,
                                "45" : ntripUrl.value,
                                "46" : ntripMount.value,                                             
                                "47" : ntripPort.value,                                                      
                                "48" : ntripVersion.value,                                                     
                                "49" : ntripUser.value,                                                
                                "50" : ntripPassword.value,                       
                                "51" : Number(ntripSendGGA.checked).toString()  // Send "0"/"1", not false/true.                    
                })
            });
            break;
    }
    return jsonString;
}

/**
 * =========================================================================
 *  Event listeners.
 * =========================================================================
 *
 * @since  3.0.3 [2025-10-16-01:45pm].
 */

/**
 * -------------------------------------------------------------------------
 *  General.
 * -------------------------------------------------------------------------
 *
 * @return void   No output is returned.
 * @since  3.0.3  [2025-10-22-01:30pm].
 * @since  3.0.12 [2026-02-15-03:30pm] Moved reconnect from webSocketClosed().
 */

 document.addEventListener('DOMContentLoaded', () => {

     // --- Navigation links. ---
    newPage.forEach(page => {
        page.addEventListener('click', (event) => {
            event.preventDefault();
            event.stopPropagation();
            webSocketStop(event);
        });
    });

     // --- Attempt to reconnect every RECONNECT_INTERVAL if no WebSocket connection. ---
    setInterval(() => {
        if (headerH1.classList.contains('red')) {
            // window.location.reload();       // Restart connection.  // ToDo: temp for test
        }
    }, RECONNECT_INTERVAL);

});

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
