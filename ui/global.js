/**
 * *************************************
 *      Ghost Rover 3 - UI JS.
 * *************************************
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
 * @link   http://dougfoster.me.
 * 
 * Websocket messages: See DougFoster_GhostRover.ino for exchange protocol.
*/

/**
 * ============================================================================
 *                                Global vars.
 * ============================================================================
 *
 * @since  3.0.8 [2025-11-21-09:00am].
 * @since  3.0.12 [2026-02-09-01:45pm].
 * @since  3.0.12 [2026-02-28-02:15pm] Add WS_SOCKET_NUM.
 * @since  3.1.0  [2026-03-20-11:15am] Update var names.
 * @since  3.1.1  [2026-06-26-09:30pm] HEIGHT_QUICK_RELEASE, changed height values.
 * @since  3.1.1  [2026-06-26-09:30pm] change WS_PREF_GNSS_MESASURE_INTERVAL to WS_PREF_GNSS_MEASURE_INTERVAL.
 * @see    operateMessage() in operate.js.
 * @see    setHeights() in config.js.
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
// @see Global vars. - WebSockets in DougFoster_Ghost_Rover.ino.
let   websocket;
const wsKey = Object.freeze({       // wsKey.WS_VERSION
    WS_VERSION:                     '0',
    WS_PREF_UNIT:                   '1',
    WS_PREF_RTCM_IN:                '2',
    WS_PREF_NMEA_OUT:               '3',
    WS_PREF_GNSS_MEASURE_INTERVAL:  '4',
    WS_PREF_GNSS_NAV_RATE:          '5',
    WS_PREF_HOT_SPOT_SSID:          '6',
    WS_PREF_HOT_SPOT_PASS:          '7',
    WS_GNSS_FIX:                    '8',
    WS_GNSS_SAT_IN_VIEW:            '9',
    WS_GNSS_HEIGHT_ELLIPSOID:       '10',
    WS_GNSS_HEIGHT_ORTHOMETRIC:     '11',
    WS_GNSS_LATITUDE:               '12',
    WS_GNSS_LONGITUDE:              '13',
    WS_GNSS_HORIZONTAL_ACCURACY:    '14',
    WS_GNSS_VERTICAL_ACCURACY:      '15',
    WS_ROVER_RTCM_UP_DOWN:          '16',
    WS_ROVER_BT_NMEA_UP_DOWN:       '17',
    WS_ROVER_BATTERY_SOC:           '18',
    WS_ROVER_BATTERY_CHANGE_RATE:   '19',
    WS_ROVER_UP_TIME:               '20',
    WS_RTCM_IN_COUNT_ALL:           '21',
    WS_RTCM_IN_RATE:                '22',
    WS_NMEA_OUT_COUNT_GGA:          '23',
    WS_NMEA_OUT_COUNT_RMC:          '24',
    WS_NMEA_OUT_COUNT_GSA:          '25',
    WS_NMEA_OUT_COUNT_GSV:          '26',
    WS_NMEA_OUT_COUNT_GST:          '27',
    WS_NMEA_OUT_COUNT_TXT:          '28',
    WS_NMEA_OUT_COUNT_OTHR:         '29',
    WS_NMEA_OUT_COUNT_ALL:          '30',
    WS_NMEA_OUT_RATE:               '31',
    WS_OPERATIONAL_MODE:            '32',
    WS_WIFI_LOCAL_NETWORK_IP:       '33',
    WS_WIFI_HOT_SPOT_IP:            '34',
    WS_SOCKET_NUM:                  '35',
    WS_INSTRUMENT_HEIGHT:           '36'
});

// --- Preferences. ---
let prfUnt       = 0;
let prfRtcIn     = 0;
let prfNmeOut    = 0;
let prfGnsMsrInt = 0;
let prfGnsNavRat = 0;
let prfHotSsi    = 0;
let prfHotPas    = 0;
let prfInstrHght = 0;

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
 * ============================================================================
 *                                Functions.
 * ============================================================================
 *
 * @since  3.0.3 [2025-10-16-01:45pm].
 * @since  3.1.0 [2026-03-20-11:15am] Update var names.
 * @see   webSocketInit()       - WebSocket: init.
 * @see   webSocketOpened()     - WebSocket: opened.
 * @see   webSocketClosed()     - WebSocket: closed.
 * @see   webSocketError()      - WebSocket: error.
 * @see   webSocketStop()       - WebSocket: stopped.
 * @see   webSocketRcvMessage() - WebSocket: message from server, what to do?
 * 
 */

/**
 * ------------------------------------------------
 *      WebSocket: init.
 * ------------------------------------------------
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
 * ------------------------------------------------
 *      WebSocket: opened.
 * ------------------------------------------------
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
 * ------------------------------------------------
 *      WebSocket: closed.
 * ------------------------------------------------
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
 * ------------------------------------------------
 *      WebSocket: error.
 * ------------------------------------------------
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
 * ------------------------------------------------
 *      WebSocket: stopped.
 * ------------------------------------------------
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
 * ------------------------------------------------
 *      WebSocket: message from server, what to do?
 * ------------------------------------------------
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
    var myObj = JSON.parse(event.data);
    let response = 'browser <-- ' + event.data
    wsNumBytesThisMessage  = event.data.length;     // Bytes per message.
    if (sessionStorage.getItem("displayJsConsoleMessages") == 'on') {
        console.log(response);
    }

    // --- Loop objects. ---
    if ('null' !== event.data) {
        Object.entries(myObj).forEach(([key, value]) => {

            switch (key) {

                // --- Header. ---
                case wsKey.WS_VERSION:
                    versionRoverId.innerHTML = value;
                    break;
                case wsKey.WS_PREF_UNIT:
                    switch (value) {
                        case 'meter':
                            statusUnitDisplayId.innerHTML = 'Meter';
                            break;
                        case 'feet':
                            statusUnitDisplayId.innerHTML = 'Feet';
                            convert = 3.2808399;
                            break;
                        default:
                            statusUnitDisplayId.innerHTML = value;
                            break;
                    }

                // --- Status section. ---
                case wsKey.WS_PREF_RTCM_IN:
                    prfRtcIn = value;
                    break;
                // Todo: [2026-06-28] Add case for RTCM count.
                // Todo: [2026-06-28] Add case for RTCM rate (kbps).
                case wsKey.WS_PREF_NMEA_OUT:
                    prfNmeOut = value;
                    break;
                case wsKey.WS_PREF_GNSS_MEASURE_INTERVAL:
                    prfGnsMsrInt = value;
                    break;
                case wsKey.WS_PREF_GNSS_NAV_RATE:
                    prfGnsNavRat = value;
                    break;
                case wsKey.WS_PREF_HOT_SPOT_SSID:
                    prfHotSsi = value;
                    break;
                case wsKey.WS_PREF_HOT_SPOT_PASS:
                    prfHotPas = value;
                    break;
                case wsKey.WS_SOCKET_NUM:
                    prfHotPas = value;
                    break;
                case wsKey.WS_INSTRUMENT_HEIGHT:
                    prfInstrHght = value;
                    break;
            }

            // -- Route each message to its page. --
            if (window.location.pathname.includes('operate')) {
                operateMessage(key, value);         // operate.js.
            } else if (window.location.pathname.includes('files')) {
                filesMessage(key, value);           // files.js
            } else if (window.location.pathname.includes('config')) {
                prefsMessage(key, value);           // config.js
            } else if (window.location.pathname.includes('nmea')) {
                nmeaMessage(key, value);            // nmea.js
            }
        });
    }
}

/**
 * ============================================================================
 *                             Event listeners.
 * ============================================================================
 *
 * @since  3.0.3 [2025-10-16-01:45pm].
 */

/**
 * ------------------------------------------------
 *      General.
 * ------------------------------------------------
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
 * ============================================================================
 *                                    Test.
 * ============================================================================
 *
 * @since  3.0.3 [2025-10-16-10:00am].
 */

/**
 * ============================================================================
 *                             Run on page load.
 * ============================================================================
 *
 * @since  3.0.3 [2025-10-16-10:00am].
 */

