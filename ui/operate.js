/**
 * *************************************************************************
 *  Ghost Rover 3 - UI JS.
 * *************************************************************************
 * 
 * operate.js
 *
 * @author D. Foster <doug@dougfoster.me>.
 * @since  3.0.11 [2026-01-18-03:45pm] Basic functionality.
 * @since  3.0.11 [2026-01-21-10:00am] Altitude to height, lock to lock-unlock.
 * @since  3.0.11 [2026-01-22-11:00am] Websocket tweaks.
 * @since  3.0.11 [2026-01-28-12:00pm] UI tweaks.
 * @since  3.0.12 [2026-02-08-02:00pm] Add SEND_PREFS, change heights.
 * @since  3.0.12 [2026-02-09-03:45pm] Add WS message transfer rate.
 * @since  3.0.12 [2026-02-17-10:00am] Change location to position.
 * @since  3.0.12 [2026-02-17-09:15pm] Add RTCM & NMEA status.
 * @since  3.0.12 [2026-02-18-11:00pm] Shorten RTCM & NMEA status.
 * @since  3.0.12 [2026-02-25-05:45pm] Websocket send - preserve KV pair order by changing JSON data to array.
 * @since  3.0.12 [2026-02-27-06:45pm] Add WebSocket #.
 * @since  3.0.12 [2026-02-28-02:15pm] Add WS_SOCKET_NUM.
 * @since  3.1.0  [2026-03-02-05:00pm] Stable 3.0 version.
 * @since  3.1.0  [2026-03-20-11:15am] Update var names.
 * @since  3.1.1  [2026-06-25-02:00pm] Regroup: upload to SD card.
 * @since  3.1.2  [2026-07-05-05:45pm] Adjust decimal places for status items.
 * @since  3.1.2  [2026-07-05-05:45pm] Remove clearOperateUi().
 * @since  3.1.2  [2026-07-05-08:45pm] General cleanup.
 * @since  3.2.1  [2026-07-28-10:00am] Move webSocket # & units to status items.
 * @since  3.2.1  [2026-07-31-02:00pm] Status display tweaks.
 * @since  3.3.5  [2026-09-08-07:45pm] Add MIN_SATELLITE_THRESHHOLD & logic for #start on "operate" page.
 * @since  3.3.5  [2026-09-12-11:15am] Add laser on/off logic to btnLaser.addEventListener().
 * @since  3.4.0  [2026-09-13-02:30pm] Height/position lock/unlock.
 * @since  3.4.0  [2026-09-17-04:30pm] Vertical slider for numbers.
 * @since  3.4.0  [2026-09-19-11:30am] Remove update() and const SEND_PREFS.
 * @since  3.4.0  [2026-09-22-09:30am] GNSS coordinate conversions.
 * @since  3.4.1  [2025-10-26-04:45pm] Cleanup formatting.
 * @link   http://dougfoster.me.
 */

/**
 * =========================================================================
 *  Global vars.
 * =========================================================================
 *
 * @since  3.0.7  [2025-11-14-09:30am] New.
 * @since  3.0.11 [2026-01-20-07:00pm] Change altitude to height.
 * @since  3.0.13 [2026-01-28-08:45pm] Add status section.
 * @since  3.0.12 [2026-02-08-02:00pm] Add SEND_PREFS, change heights.
 * @since  3.0.12 [2026-02-09-03:45pm] Add WS message transfer rate.
 * @since  3.0.12 [2026-02-17-09:15pm] Add RTCM & NMEA status.
 * @since  3.0.12 [2026-02-18-11:00pm] Shorten RTCM & NMEA status.
 * @since  3.0.12 [2026-02-25-06:30pm] Copy HAC logic to VAC.
 * @since  3.0.12 [2026-02-27-06:45pm] Add WebSocket #.
 * @since  3.3.5  [2026-09-08-07:30pm] Add fixNumSivDisplay, change numSiV to fixNumSIV.
 * @since  3.4.0  [2026-09-19-11:30am] Remove update() and const SEND_PREFS.
 * @since  3.4.0  [2026-09-22-09:30am] GNSS coordinate conversions.
 */

// --- Section: Fix. ---
const fixNone                      = document.querySelector('.fix #none .inside');
const fixSingle                    = document.querySelector('.fix #single .inside');
const fixRtkFloat                  = document.querySelector('.fix #rtkFloat .inside');
const fixRtkFix                    = document.querySelector('.fix #rtkFix .inside');
const fixFix                       = document.querySelector('.fix #fix');
const fixNumSIV                    = document.querySelector('.fix #siv');
const fixNumSivDisplay             = document.querySelector('.fix #sivDisplay');

// --- Section: Numbers. ---
const numHeightElip                = document.querySelector('.numbers #height-ellipsoid');
const numHeightOrth                = document.querySelector('.numbers #height-orthometric');
const numLatitude                  = document.querySelector('.numbers #latitude');
const numLongitude                 = document.querySelector('.numbers #longitude');
const numbers                      = document.querySelector('.numbers');
const numVAC                       = document.querySelectorAll('.numbers .vac');
const numHAC                       = document.querySelectorAll('.numbers .hac');
const numEcefX                     = document.querySelector('.numbers #pos-ecef-x');
const numEcefY                     = document.querySelector('.numbers #pos-ecef-y');
const numEcefZ                     = document.querySelector('.numbers #pos-ecef-z');
const numPosUtmZone                = document.querySelector('.numbers #pos-utm-zone');
const numPosUtmEast                = document.querySelector('.numbers #pos-utm-east');
const numPosUtmNorth               = document.querySelector('.numbers #pos-utm-north');

// --- Section: Buttons. ---
const btnLaser                     = document.querySelector('.buttons #laser');
const btnLaserIcon                 = document.querySelector('.buttons #laser .icon');
const btnLaserLabel                = document.querySelector('.buttons #laser .label');
const btnHeight                    = document.querySelector('.buttons #height');
const btnHeightIcon                = document.querySelector('.buttons #height .icon');
const btnHeightLabel               = document.querySelector('.buttons #height .label');
const btnPosition                  = document.querySelector('.buttons #position');
const btnPositionIcon              = document.querySelector('.buttons #position .icon');
const btnPositionLabel             = document.querySelector('.buttons #position .label');
const btnLockUnlock                = document.querySelector('.buttons #lock-unlock');
const btnLockUnlockLabel           = document.querySelector('.buttons #lock-unlock .label');
const btns                         = document.querySelectorAll('.btn');
const lockIcons                    = document.querySelectorAll('.icon .lock');

// --- Section: Comm. ---
const commRtcm                     = document.querySelector('.info #rtcm');
const commBt                       = document.querySelector('.info #bt');
const rtcmSource                   = document.querySelector('.info #rtcm-source');

// --- Section: Battery. ---
const batteryStatus                = document.querySelector('.info #battery-status');
const batteryBars                  = document.querySelectorAll('.info #battery .bar');

// --- Section: Status. ---
const statusUnitDisplayId          = document.querySelector('#unit-display');
const stuffStatus                  = document.querySelector('.status #status-stuff');
const btnStatus                    = document.querySelector('.status #status');
const tblStatus                    = document.querySelector('.status table');
const statusUptimeOperateId        = document.querySelector('.status #uptime-operate');
const statusUptimeRoverId          = document.querySelector('.status #uptime-rover');
const statusSolutionIntervalId     = document.querySelector('.status #solution-interval');
const statusWebSocketNumId         = document.querySelector('.status #ws-socket-num');
const statusWsMessageCountId       = document.querySelector('.status #ws-message-count');
const statusWsMessageIntervalId    = document.querySelector('.status #ws-message-interval');
const statusWsMessageRateId        = document.querySelector('.status #ws-message-rate');
const statusWsMessageLengthId      = document.querySelector('.status #ws-message-length');
const statusRtcmInId               = document.querySelector('.status #rtcm-in');
const statusRtcmSentenceCountAllId = document.querySelector('.status #rtcm-sentence-count-all');
const statusRtcmSentenceRateId     = document.querySelector('.status #rtcm-sentence-rate');
const statusNmeaSentenceCountAllId = document.querySelector('.status #nmea-sentence-count-all');
const statusNmeaCountGgaId         = document.querySelector('.status #nmea-sentence-count-gga');
const statusNmeaCountRmcId         = document.querySelector('.status #nmea-sentence-count-rmc');
const statusNmeaCounGsatId         = document.querySelector('.status #nmea-sentence-count-gsa');
const statusNmeaCountGsvId         = document.querySelector('.status #nmea-sentence-count-gsv');
const statusNmeaCountGstId         = document.querySelector('.status #nmea-sentence-count-gst');
const statusNmeaCountTxtId         = document.querySelector('.status #nmea-sentence-count-txt');
const statusNmeaCountOthrId        = document.querySelector('.status #nmea-sentence-count-other');
const statusNmeaRateId             = document.querySelector('.status #nmea-sentence-rate');
const statusLocalIpId              = document.querySelector('.status #local-ip');
const statusHotspotIpId            = document.querySelector('.status #hotspot-ip');
const statusHotspotSsidId          = document.querySelector('.status #hotspot-ssid');
const statusHotspotPassId          = document.querySelector('.status #hotspot-pass');
const statusWifiMode               = document.querySelector('.status #wifi-mode');
const statusInstrumentHeight       = document.querySelector('.status #instrument-height');

// --- GNSS. ---
let heightElip = 0;
let heightOrth = 0;
let latitude   = 0;
let longitude  = 0;
let GnssPos    = {
    lat:  0,
    lon:  0,
    hgtE: 0,
};

// --- GNSS coordinate conversions. ---
const WGS84_A  = 6378137.0;                 // Semi-major axis (m).
const WGS84_F  = 1 / 298.257223563;         // Flattening.
const WGS84_E2 = WGS84_F * (2 - WGS84_F);   // Eccentricity squared.

// --- General. ---
const LASER_ON                     = '{"laserOn":""}';
const LASER_OFF                    = '{"laserOff":""}';
// const wsMessageWindowMaxCount      = 10;         // Not used? WebSocket message status tracking window (# messages).
let prfGnsMsrInt                   = 0;
let prfGnsNavRat                   = 0;
let startTime;
let wsMessageCountTotal            = 0;             // Total # of WebSocket messages received. 
let wsWindowStartTime              = 0;
let wsWindowInterval               = 0;
let convert                        = 1;             // Conversion for default units preference (which is 'meter');

/**
 * =========================================================================
 *  Functions.
 * =========================================================================
 *
 * @since 3.0.3  [2025-10-16-01:45pm] New.
 * @since 3.0.12 [2026-01-28-06:00pm] Refactor for status section.
 * @since 3.0.12 [2026-02-07-07:30am] Add SEND_PREFS.
 * @since 3.1.2  [2026-07-05-05:45pm] Remove clearOperateUi().
 * @since 3.4.0  [2026-09-19-11:30am] Remove update() and const SEND_PREFS.
 * @since 3.4.0  [2026-09-22-09:30am] GNSS coordinate conversions.
 * @see   fix()            - Fix - set state.
 * @see   button()         - Buttons - set button states.
 * @see   toggleButtons()  - Buttons - set icon states.
 * @see   battery()        - Battery - set items.
 * @see   flash()          - Flash an LED.
 * @see   deg2rad()        - Convert degrees to radians.
 * @see   rad2deg()        - Convert radians to degrees.
 * @see   llhToECEF()      - Geodetic (lat, lon, ellipsoid height) -> ECEF (X, Y, Z), in meters.
 * @see   llToUTM()        - Geodetic (lat, lon) -> UTM (zone, hemisphere, easting, northing).
 */

/**
 * -------------------------------------------------------------------------
 *  Fix - set state.
 * -------------------------------------------------------------------------
 * 
 * Since there is (1) fix state per WebSocket message, also calculate the WebSocket stats.
 *
 * @return void   No output is returned.
 * @since  3.0.3  [2025-10-19-02:00pm] New.
 * @since  3.0.10 [2026-01-08-01:30pm] Shortened keywords.
 */
function fix(state) {

    // --- Fix state. ----
    const states = [fixNone, fixSingle, fixRtkFloat, fixRtkFix];
    states.forEach(type => {
        type.classList.remove('active');
    });
    let which = null, status = null;
    switch (state) {
        case 0:
            which = fixNone;
            status = 'GNSS DOWN';
            break;
        case 1:
            which = fixSingle;
            status = 'SINGLE FIX';
            break;
        case 2:
            which = fixRtkFloat;
            status = 'RTK-FLOAT';
            break;
        case 3:
            which = fixRtkFix;
            status = 'RTK-FIX';
            break;
        default:
    }
    if (which!=null) {
        which.classList.add('active');                  // LED indicator bar.
    }
    fixFix.innerHTML = status;                          // Status below LED indicator bar.

    // --- Track WebSocket stats. ---
    wsWindowInterval = Date.now() - wsWindowStartTime;  // Window interval (ms).

    // --- Display values. ---
    statusWsMessageCountId.innerHTML    = (wsMessageCountTotal++).toLocaleString();
    statusWsMessageIntervalId.innerHTML = (wsWindowInterval).toFixed(0);
    statusWsMessageLengthId.innerHTML   = wsNumBytesThisMessage;  // @see webSocketRcvMessage() in global.js.
    statusWsMessageRateId.innerHTML     = ((wsNumBytesThisMessage * 8) / (wsWindowInterval/1000) / 1024).toFixed(2);  // kbps.

    // --- Reset counters. ---
    wsWindowStartTime          = Date.now();
}

/**
 * -------------------------------------------------------------------------
 *  Buttons - set button states.
 * -------------------------------------------------------------------------
 *
 * @return void   No output is returned.
 * @since  3.0.3  [2025-10-13-02:15pm] New.
 * @since  3.0.11 [2026-01-20-07:00pm] Change altitude to height.
 * @since  3.0.11 [2026-01-21-09:00am] Check websocket.readyState.
 * @since  3.0.12 [2026-02-25-05:45pm] Websocket send - preserve KV pair order by changing JSON data to array.
 * @since  3.4.0  [2026-09-12-07:15pm] Height/position lock/unlock.
 */
function button(which, action) {
    let icon = null, label = null;
    let message;
    switch (which) {
        case 'laser':
            icon  = btnLaserIcon;
            label = btnLaserLabel;
            break;
        case 'height':
            icon  = btnHeightIcon;
            label = btnHeightLabel;
            break;
        case 'position':
            icon  = btnPositionIcon;
            label = btnPositionLabel;
            break;
    }
    switch (action) {
        case 'Lock':                                                // Command to server.
            icon.classList.add('show-column');
            label.classList.add('remove-left-border-radius');
            message = '{"' + which + action + '":""}';               // [{"heightLock":"""}].
            if (1 == websocket.readyState) {
                websocket.send(message);
            }
            console.log('browser --> ' + message);
            btnLockUnlockLabel.innerText = 'UNLOCK';                // Udpate lock/unlock button.  
            btnLockUnlock.classList.add('locked');
            break;
        case 'Locked':                                              // Command confirmation from server.
            label.classList.add('shadow');
            break;
        case 'Unlock':
            icon.classList.remove('show-column');
            label.classList.remove('remove-left-border-radius');
            message = '{"' + which + action + '":""}';                // [{"heightUnlock":"""}].
            if (1 == websocket.readyState) {
                websocket.send(message);
            }
            console.log('browser --> ' + message);
            btnLockUnlockLabel.innerText = 'LOCK';                  // Udpate lock/unlock button.
            btnLockUnlock.classList.remove('locked');  
            break;
        case 'Unlocked':
            label.classList.remove('shadow');
    }
}

/**
 * -------------------------------------------------------------------------
 *  Buttons - set icon states.
 * -------------------------------------------------------------------------
 *
 * @return void   No output is returned.
 * @since  3.0.3  [2025-10-13-02:15pm] New.
 * @since  3.0.11 [2026-01-20-07:00pm] Change altitude to height.
 * @since  3.0.11 [2026-01-22-10:30am] Refactor.
 * @since  3.4.0  [2026-09-12-10:15pm] Height/position lock/unlock.
 */
function toggleButtons(which) {
    let buttonIcon = null;
    let buttonState;
    switch (which) {
        case 'laser':
            buttonIcon = btnLaserIcon;
            break;
        case 'height':
            buttonIcon = btnHeightIcon;
            break;
        case 'position':
            buttonIcon = btnPositionIcon;
            break;
    }
    if (buttonIcon !== null) {
        buttonState = (buttonIcon.classList.contains('show-column')) ? 'Unlock' : 'Lock';
        button(which, buttonState);
    }
}

/**
 * -------------------------------------------------------------------------
 *  Battery - set items.
 * -------------------------------------------------------------------------
 *
 * example JSON: {"bat":93.07031,"batc":-1.2}
 *
 * @return void   No output is returned.
 * @since  3.0.7  [2025-11-10-11:30am] New.
 * @since  3.0.10 [2026-01-08-12:30pm] Shortened keywords.
 * @since  3.0.12 [2026-02-25-05:45pm] Websocket send - preserve KV pair order by changing JSON data to array.
 */
function battery(which, info) {
    let item = null;
    switch (which) {
        case 'soc':                                 // 18.  {"bat":"83.75"}.
            batterySoc = parseFloat(info);
            let index = 0;
            for ( let batteryBar of batteryBars ) {
                var power = Math.ceil(batterySoc/10);
                if(power > 10) {
                    power = 10;
                }
                if (index != power) {
                    batteryBar.classList.add('active');
                    index++;
                } else {
                    batteryBar.classList.remove('active');
                }
            }
            if (batterySoc <= 20) {                 // <=20% add alert.
                batteryBars.forEach(bar => {
                    bar.classList.add('alert');
                });  
            }
            if (batterySoc >= 30) {                 // >=30% remove alert.
                batteryBars.forEach(bar => {
                    bar.classList.remove('alert');
                });
            }
            break;
        case 'change':                              // 19.  {"batc":"-1.2"}.
            let symbol = '';
            if (info > 0) {
                symbol = '+';
            }
            if (0 !== batterySoc) {
                batteryStatus.innerHTML = batterySoc.toFixed(2) + '% (' + symbol + info + '%/hr)';
            }
            break;
    }
}

/**
 * -------------------------------------------------------------------------
 *  Flash an LED.
 * -------------------------------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.12 [2026-02-08-06:45pm] New.
 */
function flashBt() {
    commBt.style.opacity = .2;
    setTimeout(function() {
        commBt.style.opacity = 1;
    }, 50);
}
function flashRtcm() {
    rtcm.style.opacity = .2;
    setTimeout(function() {
        rtcm.style.opacity = 1;
    }, 50);
}

/**
 * -------------------------------------------------------------------------
 *  Convert degrees to radians.
 * -------------------------------------------------------------------------
 *
 * @param  Number d Degrees.
 * @return Number Radians.
 * @since  3.3.4  [2026-09-21-03:00pm] New.
 * @see    llhToECEF() in operate.js.
 * @see    llToUTM() in operate.js.
 */
function deg2rad(d) {
    return d * Math.PI / 180; 
}

/**
 * -------------------------------------------------------------------------
 * Convert radians to degrees.
 * -------------------------------------------------------------------------
 *
 * @param  Number r Radians.
 * @return Number Degrees.
 * @since  3.3.4  [2026-09-21-03:00pm] New.
 */
function rad2deg(r) {
    return r * 180 / Math.PI;
}

/**
 * -------------------------------------------------------------------------
 * Geodetic (lat, lon, height) -> ECEF (X, Y, Z), in meters.
 *  height = ellipsoidal height (HAE), not orthometric/MSL.
 * -------------------------------------------------------------------------
 *
 * @param  Number latDeg Lattitude in degrees.
 * @param  Number lonDeg Longitude in degrees.
 * @param  Number heightM Ellipsoidal height in meters.
 * @return Number x, y, z ECEF X,Y, & Z positions. (GnssPos.x, GnssPos.y, Gnss.z)
 * @since  3.3.4  [2026-09-21-03:00pm] New.
 * @see    webSocketRcvMessage() in global.js.
 */
function llhToECEF(latDeg, lonDeg, heightM) {
    latDeg       = Number(latDeg);
    lonDeg       = Number(lonDeg);
    heightM      = Number(heightM);
    const lat    = deg2rad(latDeg);
    const lon    = deg2rad(lonDeg);
    const sinLat = Math.sin(lat);
    const cosLat = Math.cos(lat);

  // --- Radius of curvature in the prime vertical. ---
  const N = WGS84_A / Math.sqrt(1 - WGS84_E2 * sinLat * sinLat);

  const x = (N + heightM) * cosLat * Math.cos(lon);
  const y = (N + heightM) * cosLat * Math.sin(lon);
  const z = (N * (1 - WGS84_E2) + heightM) * sinLat;

  return { x, y, z };
}

/**
 * -------------------------------------------------------------------------
 * Geodetic (lat, lon) -> UTM (zone, hemisphere, easting, northing).
 * Standard Transverse Mercator projection per NGA/USGS formulas.
 * -------------------------------------------------------------------------
 *
 * @param  Number latDeg Lattitude in degrees.
 * @param  Number lonDeg Longitude in degrees.
 * @return Number zone, hemisphere, easting, northing Zone, hemisphere, easting, northing positions.
 * @since  3.3.4  [2026-09-21-03:00pm] New.
 * @see    webSocketRcvMessage() in global.js.
 */
function llToUTM(latDeg, lonDeg) {
    latDeg   = Number(latDeg);
    lonDeg   = Number(lonDeg);
    const a  = WGS84_A;
    const e2 = WGS84_E2;
    const k0 = 0.9996;

    const zone = Math.floor((lonDeg + 180) / 6) + 1;
    const lonOriginDeg = (zone - 1) * 6 - 180 + 3;
    const lonOrigin = deg2rad(lonOriginDeg);

    const lat = deg2rad(latDeg);
    const lon = deg2rad(lonDeg);

    const ePrime2 = e2 / (1 - e2);
    const N = a / Math.sqrt(1 - e2 * Math.sin(lat) * Math.sin(lat));
    const T = Math.tan(lat) * Math.tan(lat);
    const C = ePrime2 * Math.cos(lat) * Math.cos(lat);
    const A = Math.cos(lat) * (lon - lonOrigin);

    const M = a * (
      (1 - e2 / 4 - 3 * e2 * e2 / 64 - 5 * e2 * e2 * e2 / 256) * lat
      - (3 * e2 / 8 + 3 * e2 * e2 / 32 + 45 * e2 * e2 * e2 / 1024) * Math.sin(2 * lat)
      + (15 * e2 * e2 / 256 + 45 * e2 * e2 * e2 / 1024) * Math.sin(4 * lat)
      - (35 * e2 * e2 * e2 / 3072) * Math.sin(6 * lat)
    );

    let easting = k0 * N * (
      A + (1 - T + C) * Math.pow(A, 3) / 6
      + (5 - 18 * T + T * T + 72 * C - 58 * ePrime2) * Math.pow(A, 5) / 120
    ) + 500000.0;

    let northing = k0 * (
      M + N * Math.tan(lat) * (
        A * A / 2
        + (5 - T + 9 * C + 4 * C * C) * Math.pow(A, 4) / 24
        + (61 - 58 * T + T * T + 600 * C - 330 * ePrime2) * Math.pow(A, 6) / 720
      )
    );

    const hemisphere = latDeg >= 0 ? 'N' : 'S';
    if (latDeg < 0) northing += 10000000.0; // false northing for southern hemisphere

    return { zone, hemisphere, easting, northing };
}

/**
 * =========================================================================
 *  Event listeners.
 * =========================================================================
 *
 * @since  3.0.3  [2025-10-22-01:30pm] New.
 * @since  3.0.10 [2026-01-07-02:00pm] Add update.
 * @since  3.0.11 [2026-01-20-07:00pm] Change altitude to height.
 * @since  3.0.12 [2026-02-08-05:00pm] Add uptime timer.
 * @since  3.1.0  [2026-03-20-11:15am] Update var names.
 * @since  3.3.5  [2026-09-12-11:15am] Add laser on/off logic to btnLaser.addEventListener().
 * @since  3.4.0  [2026-09-13-02:30pm] Height/position lock/unlock.
 * @since  3.4.0  [2026-09-17-03:00pm] Vertical slider for numbers.
 * @see global.js.
 */

// --- Page. ---
 document.addEventListener('DOMContentLoaded', () => {

    // -- Start WebSocket. --
    webSocketInit();

    // -- Uptime timer. --
    startTime = Date.now();
    const intervalId = setInterval(function() {
        let seconds = Math.floor((Date.now() - startTime) / 1000);
        let minutes = Math.floor(seconds / 60);
        let hours = Math.floor(minutes / 60);
        statusUptimeOperateId.innerHTML = (hours % 24) + 'h ' + (minutes % 60) + 'm ' + (seconds % 60) + 's';

    }, 1000); // Every 1000ms.

    // -- Console debug. --
    console.log('Show console messages is "' + sessionStorage.getItem("displayJsConsoleMessages") + '".');
});

// --- Numbers. ---
numbers.addEventListener('click', async () => {
    numbers.classList.toggle('fixed-height');
    numbers.querySelectorAll('.clear').forEach((element, index) => {
        element.classList.toggle('remove');
    });
});

// --- Buttons. ---
btnLaser.addEventListener('click', async () => {
    btnLaserLabel.classList.toggle('shadow');               // Visual feedback.
    setTimeout(function() { btnLaserLabel.classList.remove('shadow'); }, 100);
    toggleButtons('laser');
    if ((websocket) && (1 == websocket.readyState) && btnLaser.classList.contains('locked')) {
        btnLaser.classList.remove('locked');
        websocket.send(LASER_OFF);          // Send message to rover.
        console.log('browser --> ' + LASER_OFF);
    } else {
        btnLaser.classList.add('locked');
        websocket.send(LASER_ON);          // Send message to rover.
        console.log('browser --> ' + LASER_ON);
    }
});
btnHeight.addEventListener('click', () => {
    btnHeightLabel.classList.add('shadow');                 // Visual feedback.
    setTimeout(function() { btnHeightLabel.classList.remove('shadow'); }, 100);
    toggleButtons('height');
    lockIcons[1].classList.add('blink-fast');
    setTimeout(function() { lockIcons[1].classList.remove('blink-fast')}, prfLckAvgInt * 1000);
});
btnPosition.addEventListener('click', () => {
    btnPositionLabel.classList.add('shadow');               // Visual feedback.
    setTimeout(function() { btnPositionLabel.classList.remove('shadow'); }, 100);
    toggleButtons('position');
    lockIcons[2].classList.add('blink-fast');
    setTimeout(function() { lockIcons[2].classList.remove('blink-fast')}, prfLckAvgInt * 1000);
});
btnLockUnlock.addEventListener('click', () => {
    btnLockUnlockLabel.classList.add('shadow');             // Visual feedback.
    setTimeout(function() { btnLockUnlockLabel.classList.remove('shadow'); }, 100);
    if(btnLockUnlock.classList.contains('locked')) {
        button('laser', 'Unlock');
        button('height', 'Unlock');
        button('position', 'Unlock');
        btnLockUnlockLabel.innerText = 'LOCK';              // Udpate lock/unlock button.
        btnLockUnlock.classList.remove('locked');
    } else {
        button('height', 'Lock');
        button('position', 'Lock');
        btnLockUnlockLabel.innerText = 'UNLOCK';            // Udpate lock/unlock button.
        btnLockUnlock.classList.add('locked');
        lockIcons[1].classList.add('blink-fast');
        lockIcons[2].classList.add('blink-fast');
        setTimeout(function() { lockIcons[1].classList.remove('blink-fast');
            lockIcons[2].classList.remove('blink-fast');
         }, prfLckAvgInt * 1000);
    }
});
btnStatus.addEventListener('click', () => {
    btnStatus.classList.add('shadow');                      // Visual feedback.
    setTimeout(function() { btnStatus.classList.remove('shadow'); }, 100);
    stuffStatus.classList.toggle('hide');
    stuffStatus.scrollIntoView({ behavior: "smooth", block: "start" });
});

/**
 * =========================================================================
 *  Test.
 * =========================================================================
 *
 * @since  3.0.12 [2026-02-08-08:00pm] New.
 */

/**
 * =========================================================================
 *  Run on page load.
 * =========================================================================
 *
 * @since  3.0.3 [2025-10-16-10:00am] New.
 * @since  3.1.2 [2026-07-05-05:45pm] Remove clearOperateUi().
 */
fix(           0);
battery('soc', 0);
