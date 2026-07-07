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
 * @since  3.0.12 [2026-02-08-02:00pm] Add THIS_PAGE, change heights.
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
 * @link   http://dougfoster.me.
*/

/**
 * =========================================================================
 *  Global vars.
 * =========================================================================
 *
 * @since  3.0.7  [2025-11-14-09:30am].
 * @since  3.0.11 [2026-01-20-07:00pm] Change altitude to height.
 * @since  3.0.13 [2026-01-28-08:45pm] Add status section.
 * @since  3.0.12 [2026-02-08-02:00pm] Add THIS_PAGE, change heights.
 * @since  3.0.12 [2026-02-09-03:45pm] Add WS message transfer rate.
 * @since  3.0.12 [2026-02-17-09:15pm] Add RTCM & NMEA status.
 * @since  3.0.12 [2026-02-18-11:00pm] Shorten RTCM & NMEA status.
 * @since  3.0.12 [2026-02-25-06:30pm] Copy HAC logic to VAC.
 * @since  3.0.12 [2026-02-27-06:45pm] Add WebSocket #.
 */

// --- Section: Fix. ---
const fixNone                      = document.querySelector('.fix #none .inside');
const fixSingle                    = document.querySelector('.fix #single .inside');
const fixRtkFloat                  = document.querySelector('.fix #rtkFloat .inside');
const fixRtkFix                    = document.querySelector('.fix #rtkFix .inside');
const fixFix                       = document.querySelector('.fix #fix');
const numSIV                       = document.querySelector('.fix #siv');

// --- Section: Numbers. ---
const numHeightElip                = document.querySelector('.numbers #height-ellipsoid');
const numHeightOrth                = document.querySelector('.numbers #height-orthometric');
const numLatitude                  = document.querySelector('.numbers #latitude');
const numLongitude                 = document.querySelector('.numbers #longitude');
const numVAC                       = document.querySelectorAll('.numbers .vac');
const numHAC                       = document.querySelectorAll('.numbers .hac');

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

// --- Section: Comm. ---
const commRtcm                     = document.querySelector('.info #rtcm');
const commBt                       = document.querySelector('.info #bt');

// --- Section: Battery. ---
const batteryStatus                = document.querySelector('.info #battery-status');
const batteryBars                  = document.querySelectorAll('.info #battery .bar');

// --- Section: Status. ---
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
const statusInstrumentHeight       = document.querySelector('.status #instrument-height');


// --- General. ---
const THIS_PAGE                    = '[{"page":"operate"}]';
const wsMessageWindowMaxCount      = 10;    // WebSocket message status tracking window (# messages).
let prfGnsMsrInt                   = 0;
let prfGnsNavRat                   = 0;
let startTime;
let wsMessageCountTotal            = 0;     // Total # of WebSocket messages received. 
let wsWindowStartTime              = 0;
let wsWindowInterval               = 0;
let convert                        = 1;     // Conversion for default units preference (which is 'meter');

/**
 * =========================================================================
 *  Functions.
 * =========================================================================
 *
 * @since 3.0.3  [2025-10-16-01:45pm] New.
 * @since 3.0.12 [2026-01-28-06:00pm] Refactor for status section.
 * @since 3.0.12 [2026-02-07-07:30am] Add THIS_PAGE.
 * @since 3.1.2  [2026-07-05-05:45pm] Remove clearOperateUi().
 * @see   update()         - Update server.
 * @see   fix()            - Fix - set state.
 * @see   button()         - Buttons - set button states.
 * @see   toggleButtons()  - Buttons - set icon states.
 * @see   battery()        - Battery - set items.
 * @see   operateMessage() - Execute WebSocket message.
 * @see   flash()          - Flash an LED.
 */

/**
 * -------------------------------------------------------------------------
 *  Update server.
 * -------------------------------------------------------------------------
 * 
 * @return void  No output is returned.
 * @since  3.0.10 [2026-01-07-04:30pm] New.
 * @since  3.0.11 [2026-01-13-11:45am] UPDATE_INTERVAL.
 * @since  3.0.11 [2026-01-21-09:00am] Check websocket.readyState.
 * @since  3.0.11 [2026-01-21-10:00am] Changed "update" to "?".
 * @since  3.0.12 [2026-01-31-03:30pm] Used UPDATE.
 * @since  3.0.12 [2026-02-07-07:30am] Add THIS_PAGE.
 * @see    webSocketOpened() in global.js.
 */
function update() {

    // --- Send THIS_PAGE message. ---
    websocket.send(THIS_PAGE);
    console.log('browser --> ' + THIS_PAGE);
}

/**
 * -------------------------------------------------------------------------
 *  Fix - set state.
 * -------------------------------------------------------------------------
 * 
 * Since there is (1) fix state per WebSocket message, also calculate the WebSocket stats.
 *
 * @return void  No output is returned.
 * @since  3.0.3 [2025-10-19-02:00pm].
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
 * @return void  No output is returned.
 * @since  3.0.3 [2025-10-13-02:15pm].
 * @since  3.0.11 [2026-01-20-07:00pm] Change altitude to height.
 * @since  3.0.11 [2026-01-21-09:00am] Check websocket.readyState.
 * @since  3.0.12 [2026-02-25-05:45pm] Websocket send - preserve KV pair order by changing JSON data to array.
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
        case 'lock':  // Command to server.
            icon.classList.add('show-column');
            label.classList.add('remove-left-border-radius');
            message = '[{"' + which + '":"' + action + '"}]';       // [{"hgt":"lock"}].
            if (1 == websocket.readyState) {
                websocket.send(message);
            }
            console.log('browser --> ' + message);
            btnLockUnlockLabel.innerText = 'UNLOCK';                // Udpate lock/unlock button.  
            btnLockUnlock.classList.add('locked');
            break;
        case 'locked':  // Command confirmation from server.
            label.classList.add('shadow');
            break;
        case 'unlock':
            icon.classList.remove('show-column');
            label.classList.remove('remove-left-border-radius');
            message = '[{"' + which + '":"' + action + '"}]';       // [{"hgt":"unlock"}].
            if (1 == websocket.readyState) {
                websocket.send(message);
            }
            console.log('browser --> ' + message);
            btnLockUnlockLabel.innerText = 'LOCK';                  // Udpate lock/unlock button.
            btnLockUnlock.classList.remove('locked');  
            break;
        case 'unlocked':
            label.classList.remove('shadow');
    }
}

/**
 * -------------------------------------------------------------------------
 *  Buttons - set icon states.
 * -------------------------------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.3 [2025-10-13-02:15pm].
 * @since  3.0.11 [2026-01-20-07:00pm] Change altitude to height.
 * @since  3.0.11 [2026-01-22-10:30am] Refactor.
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
        buttonState = (buttonIcon.classList.contains('show-column')) ? 'unlock' : 'lock';
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
 * @since  3.0.7  [2025-11-10-11:30am].
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
 *  Execute WebSocket message.
 * -------------------------------------------------------------------------
 *
 * @return void   No output is returned.
 * @since  3.0.7  [2025-11-14-09:30am].
 * @since  3.0.10 [2026-01-08-12:30pm] Shortened keywords.
 * @since  3.0.11 [2026-01-20-07:00pm] Change altitude to height.
 * @since  3.0.11 [2026-01-21-03:30pm] Add comm(key, value).
 * @since  3.0.12 [2026-01-28-04:30pm] Add status section. Change "mode" to "RTCM".
 * @since  3.0.12 [2026-02-07-05:00pm] Add preferences.
 * @since  3.0.12 [2026-02-08-01:45pm] Add THIS_PAGE, change heights.
 * @since  3.0.12 [2026-02-09-03:45pm] Add WS message transfer rate.
 * @since  3.0.12 [2026-02-15-03:00pm] Add RTCM & NMEA status.
 * @since  3.0.12 [2026-02-15-03:45pm] Check wsLastMessageDate for undefined.
 * @since  3.0.12 [2026-02-18-11:00pm] Shorten RTCM & NMEA status.
 * @since  3.0.12 [2026-02-25-09:45pm] Refactor, move into statusItem().
 * @since  3.0.12 [2026-02-28-02:15pm] Add WS_SOCKET_NUM.
 * @since  3.1.2  [2026-07-05-05:45pm] Remove clearOperateUi().
 * @see    webSocketRcvMessage() in global.js.
 */
function operateMessage(key, value) {

    // --- Set element content directly or via fix(), button(), or battery() functions. ---
    switch (key) {
        case 'status':                                      // {"status":"ready"}.
            if(value === 'ready') {
                HEADER_H1.classList.remove('red');
            }
            break;
        case wsKey.WS_PREF_UNIT:                            // {"1":"meter"}. Set in global.js.
            break;
        case wsKey.WS_PREF_RTCM_IN:                         // {"2":"radio}.
            switch (value) {
                case 'radio':
                    statusRtcmInId.innerHTML = 'Radio';
                    break;
                case 'ntrip':
                    statusRtcmInId.innerHTML = 'NTRIP';
                    break;
                case 'off':
                    statusRtcmInId.innerHTML = 'Off';
                    break;
                default:
                    statusRtcmInId.innerHTML = value;
                    break;
            }
            break;
        case wsKey.WS_PREF_NMEA_OUT:                        // {"3":"on"}.
            break;
        case wsKey.WS_PREF_GNSS_MEASURE_INTERVAL:           // {"4":100}
            prfGnsMsrInt = value;
            statusSolutionIntervalId.innerHTML = prfGnsNavRat + ' x ' + prfGnsMsrInt;
            break;
        case wsKey.WS_PREF_GNSS_NAV_RATE:                   // {"5":2}
            prfGnsNavRat = value;
            statusSolutionIntervalId.innerHTML = prfGnsNavRat + ' x ' + prfGnsMsrInt;
            break;
        case wsKey.WS_PREF_HOT_SPOT_SSID:                   // {"6":"ssid"}.
            statusHotspotSsidId.innerHTML   = value;
            break;
        case wsKey.WS_PREF_HOT_SPOT_PASS:                   // {"7":"pass"}.
            statusHotspotPassId.innerHTML   = value;
            break;
        case wsKey.WS_GNSS_FIX:                             // {"8":1}.
            fix(value);
            break;
        case wsKey.WS_GNSS_SAT_IN_VIEW:                     // {"9":24}.
            numSIV.innerHTML = value;
            break;
        case wsKey.WS_GNSS_HEIGHT_ELLIPSOID:                // {"10":"xx.xx"}. 3 posn = 10 mm.
            numHeightElip.innerHTML = (Math.round(value * 100) / 100 * convert).toFixed(3);
            break;
        case wsKey.WS_GNSS_HEIGHT_ORTHOMETRIC:              // {"11":"127.05"}. 3 posn = 10 mm.
            numHeightOrth.innerHTML = (Math.round(value * 100) / 100 * convert).toFixed(3);
            break;
        case wsKey.WS_GNSS_LATITUDE:                        // {"12":"35.60599395,"} 8 posn = 1.11 mm.
            numLatitude.innerHTML = (Math.round(value * 100000000) / 100000000).toFixed(8);
            break;
        case wsKey.WS_GNSS_LONGITUDE:                       // {"13":"-78.79439717"} 8 posn = 1.11 mm.
            numLongitude.innerHTML = (Math.round(value * 100000000) / 100000000).toFixed(8);
            break;
        case wsKey.WS_GNSS_HORIZONTAL_ACCURACY:             // {"14":"0.016"}.
            numHAC.forEach(hac => {
                hac.innerHTML = (Math.round(value * 10000) / 10000 * convert).toFixed(3);
            });
            break;
        case wsKey.WS_GNSS_VERTICAL_ACCURACY:                // {"15":"0.014"}.
            numVAC.forEach(vac => {
                vac.innerHTML = (Math.round(value * 10000) / 10000 * convert).toFixed(3);
            });
        case wsKey.WS_ROVER_RTCM_UP_DOWN:                   // {"16":"u"}.
            if (value == 'u') {
                commRtcm.classList.add('up');
                flashRtcm();
            } else {
                commRtcm.classList.remove('up');
            }
            break;
        case wsKey.WS_ROVER_BT_NMEA_UP_DOWN:                // {"17":"u"}.
            if (value == 'u') {
                commBt.classList.add('up');
                flashBt();
            } else {
                commBt.classList.remove('up');
            }
            break;
        case wsKey.WS_ROVER_BATTERY_SOC:                    // {"18":"83.75"}.
            battery('soc', value);
            break;
        case wsKey.WS_ROVER_BATTERY_CHANGE_RATE:            // {"19":"-1.2"}.
            battery('change', value);
            break;
        case wsKey.WS_ROVER_UP_TIME:                        // {"20":"0h 3m 8s"}.
            statusUptimeRoverId.textContent          = value;
            break;
        case wsKey.WS_RTCM_IN_COUNT_ALL:                    // {"21":"1234"}.
            statusRtcmSentenceCountAllId.textContent = value.toLocaleString()
            break;
        case wsKey.WS_RTCM_IN_RATE:                         // {"22":"1234"}.
            statusRtcmSentenceRateId.textContent     = value.toLocaleString();
            break;
        case wsKey.WS_NMEA_OUT_COUNT_GGA:                   // {"23":15271}.
            statusNmeaCountGgaId.textContent         = value.toLocaleString()
            break;
        case wsKey.WS_NMEA_OUT_COUNT_RMC:                   // {"24":15271}.
            statusNmeaCountRmcId.textContent         = value.toLocaleString();
            break;
        case wsKey.WS_NMEA_OUT_COUNT_GSA:                   // {"25":25450}.
            statusNmeaCounGsatId.textContent         = value.toLocaleString();
            break;
        case wsKey.WS_NMEA_OUT_COUNT_GSV:                   // {"26":72946}.
            statusNmeaCountGsvId.textContent         = value.toLocaleString();
            break;
        case wsKey.WS_NMEA_OUT_COUNT_GST:                   // {"27":5090}.
            statusNmeaCountGstId.textContent         = value.toLocaleString();
            break;
        case wsKey.WS_NMEA_OUT_COUNT_TXT:                   // {"28":0}.
            statusNmeaCountTxtId.textContent         = value.toLocaleString();
            break; 
        case wsKey.WS_NMEA_OUT_COUNT_OTHR:                  // {"29":3541857088}.
            statusNmeaCountOthrId.textContent        = value.toLocaleString();
            break;
        case wsKey.WS_NMEA_OUT_COUNT_ALL:                   // {"30":154010}.
            statusNmeaSentenceCountAllId.textContent = value.toLocaleString();
            break;
        case wsKey.WS_NMEA_OUT_RATE:                        // {"31":81920}
            statusNmeaRateId.textContent             = (value / 1000.0).toFixed();
            break;
        case wsKey.WS_WIFI_LOCAL_NETWORK_IP:                // {"33":"192.168.23.1"}.
            statusLocalIpId.textContent              = value;
            break;
        case wsKey.WS_WIFI_HOT_SPOT_IP:                     // {"34":"172.20.10.3"}.
            statusHotspotIpId.textContent            = value;
            break;
        case wsKey.WS_SOCKET_NUM:                           // {"35":137}.
            statusWebSocketNumId.textContent         = value.toLocaleString();
            break;
        case wsKey.WS_INSTRUMENT_HEIGHT:                    // {"36":1201}.
            statusInstrumentHeight.textContent       = value.toLocaleString();
            break;
        case wsKey.WS_RTCM_SENTENCE_COUNT:                  // {"37":659}.
            statusRtcmSentenceCountAllId.textContent = value.toLocaleString();
            break;
        case wsKey.WS_RTCM_KBPS:                            // {"38":0}.
            statusRtcmSentenceRateId.textContent     = value.toFixed(2);
            break;
        case 'laser':                                       // {"laser":"locked"}.
        case 'height':                                      // {"height":"locked"}.
        case 'position':                                    // {"position":"locked"}.
            button(key, value);
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
 * =========================================================================
 *  Event listeners.
 * =========================================================================
 *
 * @since  3.0.3  [2025-10-22-01:30pm].
 * @since  3.0.10 [2026-01-07-02:00pm] Add update.
 * @since  3.0.11 [2026-01-20-07:00pm] Change altitude to height.
 * @since  3.0.12 [2026-02-08-05:00pm] Add uptime timer.
 * @since  3.1.0  [2026-03-20-11:15am] Update var names.
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

// --- Buttons. ---
btnLaser.addEventListener('click', async () => {
    btnLaserLabel.classList.toggle('shadow');               // Visual feedback.
    setTimeout(function() { btnLaserLabel.classList.remove('shadow'); }, 100);
    toggleButtons('laser');
    // Work with this for lock buttons.
    // for (let i = 0; i < 10; i++) {
    //     await new Promise(resolve => setTimeout(resolve, 2000));
    //     console.log(`iteration ${i}`);  // replace with your command
    // }
});
btnHeight.addEventListener('click', () => {
    btnHeightLabel.classList.add('shadow');                 // Visual feedback.
    setTimeout(function() { btnHeightLabel.classList.remove('shadow'); }, 100);
    toggleButtons('height');
});
btnPosition.addEventListener('click', () => {
    btnPositionLabel.classList.add('shadow');               // Visual feedback.
    setTimeout(function() { btnPositionLabel.classList.remove('shadow'); }, 100);
    toggleButtons('position');
});
btnLockUnlock.addEventListener('click', () => {
    btnLockUnlockLabel.classList.add('shadow');             // Visual feedback.
    setTimeout(function() { btnLockUnlockLabel.classList.remove('shadow'); }, 100);
    if(btnLockUnlock.classList.contains('locked')) {
        button('laser', 'unlock');
        button('height', 'unlock');
        button('position', 'unlock');
        btnLockUnlockLabel.innerText = 'LOCK';              // Udpate lock/unlock button.
        btnLockUnlock.classList.remove('locked');
    } else {
        button('height', 'lock');
        button('position', 'lock');
        btnLockUnlockLabel.innerText = 'UNLOCK';            // Udpate lock/unlock button.
        btnLockUnlock.classList.add('locked');
    }
});
btnStatus.addEventListener('click', () => {
    btnStatus.classList.add('shadow');                      // Visual feedback.
    setTimeout(function() { btnStatus.classList.remove('shadow'); }, 100);
    stuffStatus.classList.toggle('hide');
});

/**
 * =========================================================================
 *  Test.
 * =========================================================================
 *
 * @since  3.0.12 [2026-02-08-08:00pm].
 */

/**
 * =========================================================================
 *  Run on page load.
 * =========================================================================
 *
 * @since  3.0.3 [2025-10-16-10:00am].
 * @since  3.1.2  [2026-07-05-05:45pm] Remove clearOperateUi().
 */
fix(           0);
battery('soc', 0);
