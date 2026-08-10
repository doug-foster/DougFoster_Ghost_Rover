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
 * @since  3.2.1  [2026-07-27-08:30am] Add webSocketNum.
 * @since  3.2.1  [2026-07-28-10:00am] Remove webSocketNum.
 * @since  3.2.1  [2026-07-28-04:45pm] Removed NMEA out switch & preference.
 * @since  3.2.1  [2026-08-02-09:30am] Move clearMessageField() here from operate.js.
 * @since  3.2.1  [2026-08-07-09:15am] Added RTCM bridge mode.
 * @since  3.2.2  [2026-08-09-04:45pm] Completed NTRIP logic.
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
const messageField            = document.querySelector('#message-field');
let wsNumBytesThisMessage     = 0;       // # of bytes in this WebSocket message. @see webSocketRcvMessage(). 

// --- Header. ---
const ROVER_NAME              = 'GhostRover';
const headerH1                = document.querySelector('header h1');
const versionRoverId          = document.querySelector('#version-rover');

// --- WebSocket. ---
// @see "JSON key index" in webSocketRcvMessage for exchange protocol.
let   websocket;
const caster = [
  { },
  { id:'', name:'', url:'', mount:'', port:'', version:1, user:'', pass:'', sendGga:0 },
  { id:'', name:'', url:'', mount:'', port:'', version:1, user:'', pass:'', sendGga:0 },
  { id:'', name:'', url:'', mount:'', port:'', version:1, user:'', pass:'', sendGga:0 },
];
let jsonObj;

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
 * @since  3.2.1 [2026-08-02-09:30am] Move clearMessageField() here from operate.js.
 * @see   webSocketInit()       - WebSocket: init.
 * @see   webSocketOpened()     - WebSocket: opened.
 * @see   webSocketClosed()     - WebSocket: closed.
 * @see   webSocketError()      - WebSocket: error.
 * @see   webSocketStop()       - WebSocket: stopped.
 * @see   webSocketRcvMessage() - WebSocket: message from server. Decode.
 * @see   toJson()              - WebSocket: Encode values into JSON.
 * @see   clearMessageField()   - Clear message field.
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
 *     0  = Build info                      (buildString).
 *     1  = Units                           (char     prfUnt[6]).
 *     2  = RTCM in source                  (char     prfRtcIn[6]).
 *     3  = Not used.
 *     4  = GNSS measure interval           (uint16_t prfGnsMsrInt).
 *     5  = GNSS navigation rate            (uint8_t  prfGnsNavRat).
 *     6  = WiFi hot spot SSID              (char     prfHotSsi[20]).
 *     7  = WiFi hot spot password          (char     prfHotPas[30]).
 *     8  = GNSS fix                        (u_int8_t fixType).
 *     9  = GNSS satellites in view         (u_int8_t numSatInView).
 *     10 = GNSS ellipsoid height           (float    heightEllipsoid    -> operBuffer[24]).
 *     11 = GNSS orthometric height         (float    heightOrthometric  -> operBuffer[24]).
 *     12 = GNSS latitude                   (double   lat                -> operBuffer[24]).
 *     13 = GNSS longitude                  (double   lon                -> operBuffer[24]).
 *     14 = GNSS horizontal accuracy        (float    accuracyHorizontal -> operBuffer[24]).
 *     15 = GNSS vertical accuracy          (float    accuracyVertical   -> operBuffer[24]).
 *     16 = RTCM in status - up/down        (bool     RTCMin).
 *     17 = NMEA out status - up/down       (bool     NMEAout).
 *     18 = Battery State Of Charge (SOC)   (float    batterySoc         -> operBuffer[24]).
 *     19 = Battery change rate             (float    batteryChangeRate  -> operBuffer[24]).
 *     20 = Up time                         (char     uptime[20]).
 *     21 = Not used.
 *     22 = Not used.
 *     23 = NMEA GGA out sentence count     (size_t   nmeaCountGGA).
 *     24 = NMEA RMC out sentence count     (size_t   nmeaCountRMC).
 *     25 = NMEA GSA out sentence count     (size_t   nmeaCountGSA).
 *     26 = NMEA GSV out sentence count     (size_t   nmeaCountGSV).
 *     27 = NMEA GST out sentence count     (size_t   nmeaCountGST).
 *     28 = NMEA TXT out sentence count     (size_t   nmeaCountTXT).
 *     29 = NMEA other out sentence count   (size_t   nmeaCountOther).
 *     30 = NMEA total out sentence count   (size_t   nmeaCountAll).
 *     31 = NMEA out rate                   (int64_t  nmeaRate).
 *     32 = Operational mode                (char     operMode[2]).
 *     33 = WiFi local network IP address   (char     localIp[16]).
 *     34 = WiFi hot spot address           (char     hotspotIp[16]).
 *     35 = WebSocket client/session id     (uint8_t  clientId).
 *     36 = Instrument height               (uint16_t prfInstrHgt).
 *     37 = RTCM sentence count             (size_t   rtcmSentenceCount).
 *     38 = RTCM rate                       (float    rtcmKbps).
 *     39 = NTRIP caster #1 attributes      (char     prfNtripCastAttr[0][512]).
 *     40 = NTRIP caster #2 attributes      (char     prfNtripCastAttr[1][512]).
 *     41 = NTRIP caster #3 attributes      (char     prfNtripCastAttr[2][512]).
 *     42 = NTRIP caster active [1/2/3]     (char     prfNtripCastAct[2]).
 *     43 = NTRIP caster id                 (struct ntripCasterProfile ntripCaster.id      - uint8_t).
 *     44 = NTRIP caster name               (struct ntripCasterProfile ntripCaster.name    - char name[48]).
 *     45 = NTRIP caster url                (struct ntripCasterProfile ntripCaster.url     - char url[48]).
 *     46 = NTRIP caster mount point        (struct ntripCasterProfile ntripCaster.mount   - char mount[24]).
 *     47 = NTRIP caster port               (struct ntripCasterProfile ntripCaster.port    - uint16_t).
 *     48 = NTRIP caster version            (struct ntripCasterProfile ntripCaster.version - uint8_t).
 *     49 = NTRIP caster user               (struct ntripCasterProfile ntripCaster.user    - char user[48]).
 *     50 = NTRIP caster password           (struct ntripCasterProfile ntripCaster.pass    - char user[48]).
 *     51 = NTRIP caster sendGga            (struct ntripCasterProfile ntripCaster.sendGga - bool ).
 *
 * @return void  No output is returned.
 * @since  3.0.7 [2025-11-15-02:00pm].
 * @since  3.0.10 [2026-01-07-02:30pm] Check for null event data.
 * @since  3.0.12 [2026-01-28-09:00pm] Add numWsMessages.
 * @since  3.0.12 [2026-01-30-05:00pm] Add prefsMessage().
 * @since  3.0.12 [2026-02-07-12:30pm] Add displayNmeaMessage().
 * @since  3.1.0  [2026-03-20-11:15am] Update var names.
 * @since  3.2.1  [2026-07-27-08:30am] Refactor, add webSocketNum.
 * @since  3.2.1  [2026-07-28-10:00am] Remove webSocketNum.
 * @since  3.2.1  [2026-07-28-04:45pm] Removed NMEA out switch & preference.
 * @since  3.2.1  [2026-08-03-11:30am] Remove jsonObj["21'] & jsonObj["21']. Replace displayNmeaMessage() with webSocketRcvMessage().
 * @since  3.2.2  [2026-08-09-04:45pm] Completed NTRIP logic.
 * @see    filesMessage() in files.js.
 */
function webSocketRcvMessage(event) {

    // --- Local vars. ---
    const ntripCasterAttributes = [];

    // --- Process message. ---
    jsonObj = JSON.parse(event.data);
    if (null == jsonObj) {
        return;
    }
    let response = 'browser <-- ' + event.data
    wsNumBytesThisMessage  = event.data.length;     // Bytes per message.
    if (sessionStorage.getItem("displayJsConsoleMessages") == 'on') {
        console.log(response);
    }

    // --- Page header. ---
    if (undefined !== jsonObj["0"]) {
        versionRoverId.innerHTML = jsonObj["0"];
    }

    // --- Set global vars from sendPrefs message. ---
    if (undefined !== jsonObj["sendPrefsResp"]) {
        build                       = jsonObj["0"];
        prfUnt                      = jsonObj["1"];
        prfRtcIn                    = jsonObj["2"];
        prfGnsMsrInt                = jsonObj["4"];
        prfGnsNavRat                = jsonObj["5"];
        prfHotSsi                   = jsonObj["6"];
        prfHotPas                   = jsonObj["7"];
        hotspotIp                   = jsonObj["34"];
        prfInstrHght                = jsonObj["36"];
        ntripCasterAttributes[1]    = jsonObj["39"];
        ntripCasterAttributes[2]    = jsonObj["40"];
        ntripCasterAttributes[3]    = jsonObj["41"];
        if (prfRtcIn == 'ntrip') {
            prfNtripCasterAct = jsonObj["42"];  // Preference.
        } else {                                // "off","radio", ....
            prfNtripCasterAct = 1;              // Default.
        }
        // -- Load caster array. ntripAttributes() uses caster array values to set UI fields. --
        for (let i = 1; i < ntripCasterAttributes.length; i++) {  // Array element 0 is not used. All alpha values.
            let jsonObj = JSON.parse(ntripCasterAttributes[i]);
            caster[i].id      = jsonObj["43"];
            caster[i].name    = jsonObj["44"];
            caster[i].url     = jsonObj["45"];
            caster[i].mount   = jsonObj["46"];
            caster[i].port    = jsonObj["47"];
            caster[i].version = jsonObj["48"];
            caster[i].user    = jsonObj["49"];
            caster[i].pass    = jsonObj["50"];
            caster[i].sendGga = Boolean(jsonObj["51"]);
        }
    }

    // --- Config page. ---
    if ((window.location.pathname.includes('config') && (Object.keys(jsonObj).length > 1))) {
        document.querySelector('input[name="switch-unit"][value="'    + jsonObj["1"] + '"]').checked = true;
        document.querySelector('input[name="switch-rtcm-in"][value="' + jsonObj["2"] + '"]').checked = true;
        if(prfRtcIn == 'ntrip') {
            chooseCaster.forEach(item => {
                item.classList.remove('hide');      // Hide/show "choose caster" row.
            });
        }
        gnssMeasureInterval.value = prfGnsMsrInt;
        gnssNavRate.value         =  prfGnsNavRat;
        outputInterval.textContent = gnssMeasureInterval.value * gnssNavRate.value ;
        hotspotSsid.value     = jsonObj["6"];
        hotspotPassword.value = jsonObj["7"];
        setHeights('init');
        document.querySelector('input[name="switch-ntrip-caster-active"][value="' + prfNtripCasterAct +'"]').checked = true;
        ntripCaster.value     = prfNtripCasterAct;
        ntripAttributes('load');                    // Load UI fields.
    }

    // --- Files page. ---
    if (window.location.pathname.includes('files')) {

        if (undefined !== jsonObj["fileList"]) {
            fileListBuild(jsonObj["fileList"]);
        }

        if (undefined !== jsonObj["fileDeleted"]) {
            document.querySelectorAll('#files .selected').forEach(file => {
                if ( value === file.textContent) {
                    file.remove();  // Update list.
                }
            });
        }

        if (undefined !== jsonObj["fileDeleted"]) {
            alert( 'NOT DELETED: ' + value);
            document.querySelectorAll('#files .file').forEach(file => {
                if ( value === file.textContent) {
                    file.classList.toggle('selected');  // Unselect all files.
                }
            });
        }
    }

    // --- NMEA page. ---
    if (window.location.pathname.includes('nmea') && (undefined == jsonObj["sendPrefsResp"])) {
        // displayNmeaMessage(jsonObj["NMEA"]);

        if (solutionCount < numSolutionsToDisplay) {
            if (jsonObj["NMEA"].includes('$GNGGA')) {

                // --- Make a timestamp. ---
                const date         = new Date();
                const hours        = String(date.getHours()).padStart(2, '0');
                const minutes      = String(date.getMinutes()).padStart(2, '0');
                const seconds      = String(date.getSeconds()).padStart(2, '0');
                const milliseconds = String(date.getMilliseconds()).padStart(3, '0');
                let timeStamp      = `@${hours}:${minutes}:${seconds}.${milliseconds}`;

                // --- Calculate interval since last $GNGGA sentence. ---
                deltaMs = Math.abs(date - lastDate); 
                lastDate = date;

                // --- Display timestamp & delta ms. ---
                solutionCount++;
                nmeaDisplayArea.innerHTML += '<br><br><b> #' + solutionCount + '/' + numSolutionsToDisplay + ' - ' + timeStamp + '  </b>(<b>' + deltaMs + 'ms</b> since last<b>)</b><br>';
            }

            // --- Build the output. ---
            if (solutionCount > 0) {
                nmeaDisplayArea.innerHTML += jsonObj["NMEA"];
            }
        }
    }

    // --- Operate page. ---
    if (window.location.pathname.includes('operate')) {

        if (jsonObj["sendPrefsResp"]) {

            // -- {"1":"meter"}. --
            if ('feet' === prfUnt) {
                heightUnits = 'in';
            };
            switch (jsonObj["1"]) {
                case 'meter':
                    statusUnitDisplayId.innerText = 'Meter';
                    break;
                case 'feet':
                    statusUnitDisplayId.innerText = 'Feet';
                    convert = 3.2808399;
                    break;
                default:
                    statusUnitDisplayId.innerText = jsonObj["1"];
                    break;
            }

            // -- {"2":"radio}. --
            switch (jsonObj["2"]) {
                case 'bridge':
                    statusRtcmInId.innerText = 'Bridge';
                    rtcmSource.innerText = 'Bridge';
                    break;
                case 'radio':
                    statusRtcmInId.innerText = 'Radio';
                    rtcmSource.innerText = 'Radio';
                    break;
                case 'ntrip':
                    statusRtcmInId.innerText = 'NTRIP';
                    rtcmSource.innerText = 'NTRIP';
                    break;
                case 'off':
                    statusRtcmInId.innerText = 'Off';
                    rtcmSource.innerText = 'Off';
                    break;
                default:
                    statusRtcmInId.innerText = jsonObj["2"];
                    break;
            }

            // -- {"4":100}. --
            // -- {"5":2}. --
            statusSolutionIntervalId.innerHTML = prfGnsNavRat + ' x ' + prfGnsMsrInt;

            // -- {"6":"ssid"}. --
            statusHotspotSsidId.innerHTML = jsonObj["6"];

            // -- {"7":"pass"}. --
            statusHotspotPassId.innerHTML = jsonObj["7"];

            // -- {"35":137}. --
            statusWebSocketNumId.textContent = jsonObj["35"].toLocaleString();

            // -- {"36":1201}. --
            statusInstrumentHeight.textContent = jsonObj["36"].toLocaleString();

        } else {

            // -- {"8":1}. --
            fix(jsonObj["8"]);

            // -- {"9":24}. --
            numSIV.innerHTML = jsonObj["9"];

            // -- {"10":"xx.xx"} 3 posn = 10 mm. --
            numHeightElip.innerHTML = (Math.round(jsonObj["10"] * 100) / 100 * convert).toFixed(3);

            // -- {"11":"127.05"}. 3 posn = 10 mm. --
            numHeightOrth.innerHTML = (Math.round(jsonObj["11"] * 100) / 100 * convert).toFixed(3);

            // -- {"12":"35.60599395,"} 8 posn = 1.11 mm. --
            numLatitude.innerHTML = (Math.round(jsonObj["12"] * 100000000) / 100000000).toFixed(8);

            // -- {"13":"-78.79439717"} 8 posn = 1.11 mm. --
            numLongitude.innerHTML = (Math.round(jsonObj["13"] * 100000000) / 100000000).toFixed(8);

            // -- {"14":"0.016"}. --
            numHAC.forEach(hac => {
                hac.innerHTML = (Math.round(jsonObj["14"] * 10000) / 10000 * convert).toFixed(3);
            });

            // -- {"15":"0.014"}. --
            numVAC.forEach(vac => {
                vac.innerHTML = (Math.round(jsonObj["15"] * 10000) / 10000 * convert).toFixed(3);
            });

            // -- {"16":"u"}. --
            if (jsonObj["16"]) {
                commRtcm.classList.add('up');
                flashRtcm();
            } else {
                commRtcm.classList.remove('up');
            }

            // -- {"17":"u"}. --
            if (jsonObj["17"]) {
                commBt.classList.add('up');
                flashBt();
            } else {
                commBt.classList.remove('up');
            }

            // -- {"18":"83.75"}. --
            battery('soc', jsonObj["18"]);

            // -- {"19":"-1.2"}. --
            battery('change', jsonObj["19"]);

            // -- {"20":"0h 3m 8s"}. --
            statusUptimeRoverId.textContent = jsonObj["20"];

            // -- {"23":15271}. --
            statusNmeaCountGgaId.textContent = jsonObj["23"].toLocaleString()

            // -- {"24":15271}. --
            statusNmeaCountRmcId.textContent = jsonObj["24"].toLocaleString();

            // -- {"25":25450}. --
            statusNmeaCounGsatId.textContent = jsonObj["25"].toLocaleString();

            // -- {"26":72946}. --
            statusNmeaCountGsvId.textContent = jsonObj["26"].toLocaleString();

            // -- {"27":5090}. --
            statusNmeaCountGstId.textContent = jsonObj["27"].toLocaleString();

            // -- {"28":0}. --
            statusNmeaCountTxtId.textContent = jsonObj["28"].toLocaleString();

            // -- {"29":3541857088}. --
            statusNmeaCountOthrId.textContent = jsonObj["29"].toLocaleString();

            // -- {"30":154010}. --
            statusNmeaSentenceCountAllId.textContent = jsonObj["30"].toLocaleString();

            // -- {"31":81920} --
            statusNmeaRateId.textContent = (jsonObj["31"] / 1000.0).toFixed();

            // -- {"33":"192.168.23.1"}. --
            statusLocalIpId.textContent = jsonObj["33"];
            if (jsonObj["33"].length > 0) {         // Status for WiFi server.
                statusWifiMode.textContent = 'Server';
            }

            // -- {"34":"172.20.10.3"}. --
            statusHotspotIpId.textContent = jsonObj["34"];
            if (jsonObj["34"].length > 0) {         // Status for WiFi client.
                statusHotspotIpId.textContent = jsonObj["34"];
                statusWifiMode.textContent += '/Client';
            } else {
                statusHotspotIpId.textContent = 'N/A';
            }

            // -- {"37":659}. --
            statusRtcmSentenceCountAllId.textContent = jsonObj["37"].toLocaleString();

            // -- {"38":0}. --
            statusRtcmSentenceRateId.textContent = jsonObj["38"].toFixed(2);

            // case 'laser':                   // {"laser":"locked"}.
            // case 'height':                  // {"height":"locked"}.
            // case 'position':                // {"position":"locked"}.
            //     button(key, value);
        }
    }

    // --- NTRIP page. ---
    if (window.location.pathname.includes('ntrip')) {

        // -- WiFi client. --
        if (undefined !== jsonObj["connectWifiClientResp"]) {

            // - Add response to message field. -
            messageField.innerText += '<-- ' + jsonObj["connectWifiClientResp"] + '\n';

            // - Failed. Update display. -
            if (messageField.innerText.includes('ABORTED'))  {
                btnWifiClientLabel.textContent = "Connect";
                statusWifiClient.classList.add('hide');
                statusWifiClient.classList.remove('blink');
                clearMessageField();
            }

            // - Success. Update display. -
            if (messageField.innerText.includes('WiFi CONNECTED')) {
                statusWifiClient.textContent = "- Connected -";
                statusWifiClient.classList.remove('hide');
                statusWifiClient.classList.remove('blink');
                btnWifiClientLabel.innerText = "Disconnect";
                clearMessageField();
            }
        } else if (undefined !== jsonObj["disconnectWifiClientResp"]) {

            // - Add response to message field. -
            messageField.innerText += '<-- ' + jsonObj["disconnectWifiClientResp"] + '\n';
            clearMessageField();
        }

        // -- NTRIP caster. --
        if (undefined !== jsonObj["connectNtripCasterResp"]) {

            // - Add response to message field. -
            messageField.innerText += '<-- ' + jsonObj["connectNtripCasterResp"] + '\n';

            // - Failed. Update display. -
            if ((messageField.innerText.includes('FAILED')) || 
                (messageField.innerText.includes('REJECTED')) ||
                (messageField.innerText.includes('DISCONNECTED')))  {
                btnNtripCasterLabel.textContent = "Connect";
                statusNtripCaster.classList.add('hide');
                statusNtripCaster.classList.remove('blink');
                clearMessageField();
            }

            // - Success. Update display. -
            if ((messageField.innerText.includes('SUCCESS')) ||
                (messageField.innerText.includes('NTRIP CONNECTED'))) {
                statusNtripCaster.textContent = '- Connected -';
                statusNtripCaster.classList.remove('hide');
                statusNtripCaster.classList.remove('blink');
                btnNtripCasterLabel.innerText = "Disconnect";
                clearMessageField();
            }
        } else if (undefined !== jsonObj["disconnectNtripCasterResp"]) {

            // - Add response to message field. -
            messageField.innerText += '<-- ' + jsonObj["disconnectNtripCasterResp"] + '\n';
            clearMessageField();
        }

        // - RTCM sentence count. -
        if (undefined !== jsonObj["37"]) {
            rtcmSentenceCount.innerText = jsonObj["37"];
        }
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
            jsonString = JSON.stringify( {
                "setPrefs" : "", 
                       "1" : switchUnits,                                         // prfUnt.
                       "2" : switchRtcmIn,                                        // prfRtcIn.
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
 * -------------------------------------------------------------------------
 *  Clear message field.
 * -------------------------------------------------------------------------
 * 
 * @return void  No output is returned.
 * @since  3.0.12 [2026-02-07-11:00am] New.
 * @since  3.2.1  [2026-08-02-09:30am] Move clearMessageField() here from operate.js.
 */
function clearMessageField() {
    setTimeout(function() { messageField.innerHTML = "&nbsp;"; }, 6000);
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
