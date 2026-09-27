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
 * @since  3.2.1  [2026-08-02-09:30am] Move updateUi() here from operate.js.
 * @since  3.2.1  [2026-08-07-09:15am] Added RTCM bridge mode.
 * @since  3.2.2  [2026-08-09-04:45pm] Completed NTRIP logic.
 * @since  3.3.5  [2026-09-08-07:45pm] Add MIN_SATELLITE_THRESHHOLD & logic for #start in webSocketRcvMessage().
 * @since  3.3.5  [2026-09-08-08:00pm] Changed timeout from 6s to 8s in updateUi().
 * @since  3.3.5  [2026-09-11-03:30pm] In webSocketRcvMessage(): increase decimal places: numLatitude & numLongitude from 8 to 9 decimal, numHeightElip & numHeightOrth from 3 to 4.
 * @since  3.4.0  [2026-09-13-05:30pm] Update RTCM in & NMEA out. Remove rtcmSource in webSocketRcvMessage().
 * @since  3.3.4  [2026-09-17-09:45pm] Move restart, Internet, back to menu page. 
 * @since  3.4.0  [2026-09-19-11:30am] Remove update() and const SEND_PREFS.
 * @since  3.4.0  [2026-09-20-04:15pm] Add prfLckAvgInt.
 * @since  3.4.0  [2026-09-22-09:30am] GNSS coordinate conversions.
 * @since  3.4.1  [2025-10-26-04:45pm] Cleanup formatting.
 * @since  3.4.1  [2025-10-27-11:15am] Add instructions page.
 * @link   http://dougfoster.me.
 */

/**
 * =========================================================================
 *  Global vars.
 * =========================================================================
 *
 * @since  3.0.8  [2025-11-21-09:00am] New.
 * @since  3.0.12 [2026-02-09-01:45pm] Refactor.
 * @since  3.0.12 [2026-02-28-02:15pm] Add WS_SOCKET_NUM.
 * @since  3.1.0  [2026-03-20-11:15am] Update var names.
 * @since  3.1.1  [2026-06-26-09:30pm] HEIGHT_QUICK_RELEASE, changed height values.
 * @since  3.1.1  [2026-06-26-09:30pm] change WS_PREF_GNSS_MESASURE_INTERVAL to WS_PREF_GNSS_MEASURE_INTERVAL.
 * @since  3.1.2  [2026-07-16-10:00am] Add NTRIP.
 * @since  3.2.1  [2026-07-25-04:30pm] Add caster[{}].
 * @since  3.3.5  [2026-09-08-07:45pm] Add MIN_SATELLITE_THRESHHOLD.
 * @since  3.3.5  [2026-09-19-11:15am] Add PAGE constant, change page name tests to use PAGE.
 * @since  3.4.0  [2026-09-22-09:30am] GNSS coordinate conversions.
 * @see    setHeights() in config.js.
 * @see    Global vars () WebSockets) in DougFoster_Ghost_Rover.ino.
 */

// --- Test. ---
// const uploadUrl = 'https://httpbin.org/post';
// const sdFiles   = ['file1.txt', 'file2.txt', 'file3.txt', 'file4.txt'];  // Test data.

// --- Page. ---
const PAGE = window.location.pathname.split("/").pop().split(".")[0];

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
const RECONNECT_INTERVAL       = 2000;      // Server reconnect interval.
const MIN_SATELLITE_THRESHHOLD = 2          // Mirror const in Ghost_Rover.ino.
const newPage                  = document.querySelectorAll('a.new-page');
const messageField             = document.querySelector('#message-field');
let wsNumBytesThisMessage      = 0;         // # of bytes in this WebSocket message. @see webSocketRcvMessage().

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
 * @since  3.0.3 [2025-10-16-01:45pm] New.
 * @since  3.1.0 [2026-03-20-11:15am] Update var names.
 * @since  3.2.1 [2026-07-25-04:30pm] add toJson().
 * @since  3.2.1 [2026-08-02-09:30am] Move updateUi() here from operate.js.
 * @since  3.3.5 [2026-09-08-07:45pm] Add MIN_SATELLITE_THRESHHOLD & logic for #start in webSocketRcvMessage().
 * @see    webSocketInit()       - WebSocket: init.
 * @see    webSocketOpened()     - WebSocket: opened.
 * @see    webSocketClosed()     - WebSocket: closed.
 * @see    webSocketError()      - WebSocket: error.
 * @see    webSocketStop()       - WebSocket: stopped.
 * @see    webSocketRcvMessage() - WebSocket: message from server. Decode.
 * @see    toJson()              - WebSocket: Encode values into JSON.
 * @see    updateUi()            - in menu.js, update user interface.
 */

/**
 * -------------------------------------------------------------------------
 *  WebSocket: init.
 * -------------------------------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.3 [2025-10-13-02:15pm] New.
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
 * @since. 3.0.7  [2025-11-15-12:30pm] New.
 * @since. 3.0.10 [2026-01-07-05:30pm] Removed ready handshake.
 * @since  3.1.0  [2026-03-20-11:15am] Update var names.
 * @since  3.4.0  [2026-09-19-11:30am] Remove update() and const SEND_PREFS.
 */
function webSocketOpened(event) {

    // --- UI indicator that a WebSocket is now open. ---
    console.log('WebSocket opened to ' + wsEndpoint + '.');
    headerH1.textContent = ROVER_NAME;
    headerH1.classList.remove('red');
    let firstMessage = '{"page":"' + PAGE +'","sendPrefs":""}';
    websocket.send(firstMessage);  // Send first message.
    console.log('browser --> ' + firstMessage);
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
 *     52 = GNSS lock averaging interval    (uint16_t prfLckAvgInt).
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
 * @since  3.3.0  [2026-08-14-11:15am] Added skip for incoming NMEA values of 0.
 * @since  3.3.0  [2026-08-14-01:30pm] Refactored webSocketRcvMessage() files page.
 * @since  3.3.1  [2026-08-17-11:00pm] Corrected NMEA summary line.
 * @since  3.3.5  [2026-09-08-07:45pm] Add MIN_SATELLITE_THRESHHOLD & logic for #start on "operate" page.
 * @since  3.3.5  [2026-09-11-03:30pm] Increase decimal places: numLatitude & numLongitude from 8 to 9 decimal, numHeightElip & numHeightOrth from 3 to 4.
 * @since  3.4.0  [2026-09-13-05:30pm] Update RTCM in & NMEA out. Remove rtcmSource for "operate" page.
 * @since  3.4.0  [2026-09-20-05:30pm] Add prfLckAvgInt.
 * @since  3.4.1  [2025-10-27-11:15am] Add instructions page.
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
        prfLckAvgInt                = jsonObj["52"];        // Newest pref.
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
    if (('config' == PAGE) && (Object.keys(jsonObj).length > 1)) {
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
        gnsLckAvgIntrvl.value = jsonObj["52"];      // Newest pref.
    }

    // --- Files page. ---
    if ('files' == PAGE) {

        if (undefined !== jsonObj["fileList"]) {
            fileListBuild(jsonObj["fileList"]);
        }

        if (undefined !== jsonObj["deleteFileResp"]) {
            let filename = jsonObj["deleteFileResp"].split(' ')[0];
            document.querySelectorAll('#files .selected').forEach(file => {
                if ( filename === file.textContent) {
                    if (jsonObj["deleteFileResp"].includes('NOT')) {
                        file.classList.toggle('selected');  // Unselect the file in the list.
                    } else {
                        file.remove();  // Remove the file from the list.
                    }
                }
            });
            alert(jsonObj["deleteFileResp"]);
        }
    }

    // --- NMEA page. ---
    if (('nmea' == PAGE) && ('' !== jsonObj["NMEA"])) {
        if (nmeaSentenceCount <= nmeaSentencesToDisplay) {
            if ((0 == (nmeaSentenceCount % 10)) || (0 == nmeaSentenceCount)) {
                // --- Make a timestamp. ---
                const date         = new Date();
                const hours        = String(date.getHours()).padStart(2, '0');
                const minutes      = String(date.getMinutes()).padStart(2, '0');
                const seconds      = String(date.getSeconds()).padStart(2, '0');
                const milliseconds = String(date.getMilliseconds()).padStart(3, '0');
                let   timeStamp    = `@${hours}:${minutes}:${seconds}.${milliseconds}`;
                let   kbps         = 0;

                // --- Calculate interval since last $GNGGA sentence. ---
                if (nmeaSentenceCount > 0) {
                    deltaMs = Math.abs(date - lastDate); 
                    lastDate = date;
                    kbps = (numBytes / deltaMs) * 8. * 1000. / 1024.;  // (bytes/ms) * 8bits/byte * 1000ms/1sec * 1kilobits/1024bits.
                }
                if (0 == nmeaSentenceCount) {
                    lastDate = new Date();
                    nmeaDisplayArea.innerHTML = '<b>#0/' + nmeaSentencesToDisplay + '</b> ' +
                        timeStamp + ' (starting at <b>0</b>ms)<br>';
                } else {
                    nmeaDisplayArea.innerHTML += '<b>#' + nmeaSentenceCount + '/' + nmeaSentencesToDisplay + '</b> ' +
                        timeStamp + '  </b>(<b>' + numBytes + '</b> bytes in <b>' + deltaMs + '</b> ms = <b>' + kbps.toFixed(2) + '</b> kbps since #' +
                        (nmeaSentenceCount-10) + '/' + nmeaSentencesToDisplay + ')<br><br>';
                    numBytes = 0;
                }
            }
            if (nmeaSentenceCount < nmeaSentencesToDisplay) {
                nmeaDisplayArea.innerHTML += jsonObj["NMEA"] + '<br>';
            }
            nmeaSentenceCount++;
            numBytes += jsonObj["NMEA"].length;
        }
    }

    // --- Operate page. ---
    if ('operate' == PAGE) {

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
                    break;
                case 'radio':
                    statusRtcmInId.innerText = 'Radio';
                    break;
                case 'ntrip':
                    statusRtcmInId.innerText = 'NTRIP';
                    break;
                case 'off':
                    statusRtcmInId.innerText = 'Off';
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

            // -- {"53":10}. --
            prfLckAvgInt = jsonObj["52"];      // Newest pref.

        } else {

            // -- {"8":1}. --
            fix(jsonObj["8"]);

            // -- {"9":24}. --
            fixNumSIV.innerHTML = jsonObj["9"];
            if (parseInt(fixNumSIV.innerHTML) < MIN_SATELLITE_THRESHHOLD) {
                fixFix.innerHTML = "GNSS startup";
                fixNumSivDisplay.classList.add('blink');
                fixFix.classList.add('blink');
                return;
            } else {
                fixNumSivDisplay.classList.remove('blink');
                fixFix.classList.remove('blink');
            }

            // -- {"10":"xx.xx"} 3 posn = 10 mm. --
            heightElip = (Math.round(jsonObj["10"] * 100) / 100 * convert).toFixed(3);
            numHeightElip.innerHTML = heightElip;

            // -- {"11":"127.05"}. 3 posn = 10 mm. --
            heightOrth = (Math.round(jsonObj["11"] * 100) / 100 * convert).toFixed(3);
            numHeightOrth.innerHTML = heightOrth;

            // -- {"12":"35.60599395,"} 8 posn = 1.11 mm. --
            latitude = (Math.round(jsonObj["12"] * 100000000) / 100000000).toFixed(9);
            numLatitude.innerHTML = latitude;

            // -- {"13":"-78.79439717"} 8 posn = 1.11 mm. --
            longitude = (Math.round(jsonObj["13"] * 100000000) / 100000000).toFixed(9);
            numLongitude.innerHTML = longitude;

            // -- Calculate ECEF values. --
            GnssPosECEF = llhToECEF(latitude, longitude, heightElip);
            numEcefX.innerHTML = GnssPosECEF.x.toFixed(4);
            numEcefY.innerHTML = GnssPosECEF.y.toFixed(4);
            numEcefZ.innerHTML = GnssPosECEF.z.toFixed(4);

            // -- Calculate UTM values. --
            GnssPosUTM = llToUTM(latitude, longitude);
            numPosUtmZone.innerHTML  = GnssPosUTM.zone + GnssPosUTM.hemisphere;
            numPosUtmEast.innerHTML  = GnssPosUTM.easting.toFixed(4);
            numPosUtmNorth.innerHTML = GnssPosUTM.northing.toFixed(4);

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

            // -- Values must be defined and non-zero. --
            if ((0 !== jsonObj["23"]) && (undefined!== jsonObj["23"])) {

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

                // -- {"31":81920}. --
                statusNmeaRateId.textContent = (jsonObj["31"] / 1024. / 1000000.).toFixed(2);
            }

            // -- {"33":"192.168.23.1"}. --
            statusLocalIpId.textContent = jsonObj["33"];
            if (jsonObj["33"].length > 0) {         // Status for WiFi server.
                statusWifiMode.textContent = 'Server';
            }

            // -- {"34":"172.20.10.3"}. --
            statusHotspotIpId.textContent = jsonObj["34"];
            if (jsonObj["34"].length > 0) {         // Status for Internet.
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

    // --- Menu page. ---
    if ('menu' == PAGE) {

        // -- Internet. --
        if (undefined !== jsonObj["connectInternetResp"]) {

            // - Add response to message field. -
            messageField.innerText += '<-- ' + jsonObj["connectInternetResp"] + '\n';

            // - Failed. Update display. -
            if (messageField.innerText.includes('ABORTED'))  {
                updateUi('internetIsNotConnected');
            }
            // - Success. Update display. -
            if (messageField.innerText.includes('CONNECTED to Internet')) {
                updateUi('internetIsConnected');
            }
        } else if (undefined !== jsonObj["disconnectInternetResp"]) {

            // - Add response to message field. -
            messageField.innerText += '<-- ' + jsonObj["disconnectInternetResp"] + '\n';
            updateUi('internetIsNotConnected');
        }

        // -- NTRIP caster. --
        if (undefined !== jsonObj["connectNtripCasterResp"]) {

            // - Add response to message field. -
            messageField.innerText += '<-- ' + jsonObj["connectNtripCasterResp"] + '\n';

            // - Failed. Update display. -
            if ((messageField.innerText.includes('FAILED')) || 
                (messageField.innerText.includes('REJECTED')) ||
                (messageField.innerText.includes('DISCONNECTED')))  {
                updateUi('ntripCasterIsNotConnected');
            }

            // - Success. Update display. -
            if ((messageField.innerText.includes('SUCCESS')) ||
                (messageField.innerText.includes('NTRIP CONNECTED')) ||
                (messageField.innerText.includes('NTRIP still CONNECTED'))) {
                updateUi('ntripCasterIsConnected');
            }
        } else if (undefined !== jsonObj["disconnectNtripCasterResp"]) {

            // - Add response to message field. -
            messageField.innerText += '<-- ' + jsonObj["disconnectNtripCasterResp"] + '\n';
            updateUi('ntripCasterIsNotConnected');
        }

        // - RTCM sentence count. -
        if (undefined !== jsonObj["37"]) {
            rtcmSentenceCount.innerText = jsonObj["37"];
        }
    }

    // --- Instructions page. ---
    if ('instructions' == PAGE) {
        if(prfRtcIn) {
            rtcmSelect.value = prfRtcIn;    // Set to preference value.
        } else {
            rtcmSelect.value = 'off';       // Set to off.
        }
        showRtcmInstruction();
    }
}

/**
 * -------------------------------------------------------------------------
 *  WebSocket: Encode values into JSON.
 * -------------------------------------------------------------------------
 *
 * @param  which Group of prefs to apply.
 * @return void  No output is returned.
 * @since  3.1.2 [2026-07-25-04:15pm] New.
 * @since  3.3.1 [2026-08-27-04:45pm] ["JsonDocNtrip["43,47,48, 51"] from str to int.
 * @since  3.4.0  [2026-09-20-04:15pm] Add prfLckAvgInt.
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
                      "42" : switchNtripCasterActive,                             // prfNtripCasterAct.
                      "52" : gnsLckAvgIntrvl.value                                // prfLckAvgInt.          // Newest pref.
            } )
            break;
        case 'ntripAttributes':
            jsonString = JSON.stringify( {
                            "config" : "setNtripCasterPref",
                "setNtripCasterPref" : JSON.stringify( {
                                "43" : parseInt(ntripCaster.value),
                                "44" : ntripName.value,
                                "45" : ntripUrl.value,
                                "46" : ntripMount.value,                                             
                                "47" : parseInt(ntripPort.value),                                                      
                                "48" : parseInt(ntripVersion.value),                                                     
                                "49" : ntripUser.value,                                                
                                "50" : ntripPassword.value,                       
                                "51" : Number(ntripSendGGA.checked)  // Send 0/1, not false/true.                    
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
 * @since  3.0.3 [2025-10-16-01:45pm] New.
 */

/**
 * -------------------------------------------------------------------------
 *  General.
 * -------------------------------------------------------------------------
 *
 * @return void   No output is returned.
 * @since  3.0.3  [2025-10-22-01:30pm] New.
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
 * @since  3.0.3 [2025-10-16-10:00am] New.
 */

/**
 * =========================================================================
 *  Run on page load.
 * =========================================================================
 *
 * @since  3.0.3 [2025-10-16-10:00am] New.
 */
