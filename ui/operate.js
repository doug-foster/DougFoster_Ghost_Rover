/**
 * *************************************
 *      Ghost Rover 3 - UI JS.
 * *************************************
 * 
 * operate.js
 *
 * @author   D. Foster <doug@dougfoster.me>.
 * @since    3.0.11 [2026-01-13-07:00pm].
 * @link     http://dougfoster.me.
*/

/**
 * ============================================================================
 *                                Global vars.
 * ============================================================================
 *
 * @since  3.0.7 [2025-11-14-09:30am].
 */

// --- General: Sub-header. ---
const operVersion       = document.querySelector('#sub-header #version');
const operMode          = document.querySelector('#sub-header #mode');
const operUnits         = document.querySelector('#sub-header #units');

// --- Section: Fix. ---
const fixNone           = document.querySelector('.fix #none .inside');
const fixSingle         = document.querySelector('.fix #single .inside');
const fixRtkFloat       = document.querySelector('.fix #rtkFloat .inside');
const fixRtkFix         = document.querySelector('.fix #rtkFix .inside');
const fixFix            = document.querySelector('.fix #fix');
const numSIV            = document.querySelector('.fix #siv');

// --- Section: Numbers. ---
const numAltitude       = document.querySelector('.numbers #altitude');
const numLatitude       = document.querySelector('.numbers #latitude');
const numLongitude      = document.querySelector('.numbers #longitude');
const numVAC            = document.querySelector('.numbers #vac');
const numHAClat         = document.querySelector('.numbers #hac-lat');
const numHAClon         = document.querySelector('.numbers #hac-lon');

// --- Section: Buttons. ---
const btnAltitude       = document.querySelector('.buttons #altitude');
const btnAltitudeIcon   = document.querySelector('.buttons #altitude .icon');
const btnAltitudeLabel  = document.querySelector('.buttons #altitude .label');
const btnLaser          = document.querySelector('.buttons #laser');
const btnLaserIcon      = document.querySelector('.buttons #laser .icon');
const btnLaserLabel     = document.querySelector('.buttons #laser .label');
const btnLocation       = document.querySelector('.buttons #location');
const btnLocationIcon   = document.querySelector('.buttons #location .icon');
const btnLocationLabel  = document.querySelector('.buttons #location .label');
const btnUnlock         = document.querySelector('.buttons #unlock');
const btnUnlockLabel    = document.querySelector('.buttons #unlock .label');
const btns              = document.querySelectorAll('.btn');

// --- Section: Comm. ---
const commRtcm          = document.querySelector('.info #rtcm');
const commBt            = document.querySelector('.info #bt');

// --- Section: Battery. ---
const batteryStatus     = document.querySelector('.info #battery-status');
const batteryBars       = document.querySelectorAll('.info #battery .bar');

// --- General. ---
const GNSS_INTERVAL     =  500;  // Time (ms) between ZED GNSS solution intervals.

/**
 * ============================================================================
 *                                Functions.
 * ============================================================================
 *
 * @since    3.0.3 [2025-10-16-01:45pm].
 */

/**
 * ------------------------------------------------
 *      Set version.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.3 [2025-10-19-01:30pm].
 */
function version(string) {
    operVersion.innerHTML = string;
}

/**
 * ------------------------------------------------
 *      Set mode.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.3 [2025-10-19-01:30pm].
 * @since  3.0.10 [2026-01-08-01:30pm] Shortened keywords.
 */
function mode(m) {
    switch (m) {
        case 'b':
            operMode.innerHTML = 'base';
            break;
        case 'r':
            operMode.innerHTML = 'base';
            break;
        default:
            operMode.innerHTML = 'unknown';
    }
}

/**
 * ------------------------------------------------
 *      Set units.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.3 [2025-10-19-01:30pm].
 * @since  3.0.10 [2026-01-08-01:30pm] Shortened keywords.
 */
function unit(string) {
    switch (string) {
        case 'm':
            operUnits.innerHTML = 'meters';
            break;
        case 'f':
            operUnits.innerHTML = 'feet';
            break;
        default:
            operUnits.innerHTML = 'unknown';
    }
}

/**
 * ------------------------------------------------
 *      Fix - set state.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.3 [2025-10-19-02:00pm].
 * @since  3.0.10 [2026-01-08-01:30pm] Shortened keywords.
 */
function fix(state) {
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
        which.classList.add('active');          // LED indicator bar.
    }
    fixFix.innerHTML = status;                  // Status below LED indicator bar.
}

/**
 * ------------------------------------------------
 *      Numbers - set values.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.7 [2025-11-14-09:30am].
 * @since  3.0.10 [2026-01-08-01:45pm] Shortened keywords.
 */
function number(which, num) {
    let numItem = null;
    switch (which) {
        case 'siv':
            numSIV.innerHTML = num;
            break;
        case 'alt':
            numAltitude.innerHTML = (Math.round(num * 100) / 100).toFixed(3);
            break;
        case 'lat':
            numLatitude.innerHTML = (Math.round(num * 100000000) / 100000000).toFixed(8);
            break;
        case 'lon':
            numLongitude.innerHTML = (Math.round(num * 100000000) / 100000000).toFixed(8);
            break;
        case 'vac':
            numVAC.innerHTML = (Math.round(num * 10000) / 10000).toFixed(3);
            break;
        case 'hac-lat':
            numHAClat.innerHTML = (Math.round(num * 10000) / 10000).toFixed(3);
            break;
        case 'hac-lon':
            numHAClon.innerHTML = (Math.round(num * 10000) / 10000).toFixed(3);
    }
}

/**
 * ------------------------------------------------
 *      Buttons - set button states.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.3 [2025-10-13-02:15pm].
 */
function button(which, action) {
    let icon = null, label = null;
    let message;
    switch (which) {
        case 'altitude':
            icon  = btnAltitudeIcon;
            label = btnAltitudeLabel;
            break;
        case 'laser':
            icon  = btnLaserIcon;
            label = btnLaserLabel;
            break;
        case 'location':
            icon  = btnLocationIcon;
            label = btnLocationLabel;
            break;
        case 'unlock':
            btnUnlockLabel.classList.add('shadow');
            setTimeout(function() { btnUnlockLabel.classList.remove('shadow'); }, 100);
            // --- Unlock each button. ---
            document.querySelectorAll('.show-column').forEach(lock => {
                button(lock.parentElement.id, which);
            });
}
    switch (action) {
        case 'lock':  // Command to server.
            icon.classList.add('show-column');
            label.classList.add('remove-left-border-radius');
            message = '{"' + which + '":"' + action + '"}';  // {"altitude":"lock"}.
            websocket.send(message);
            console.log('browser --> server ' + message);
            break;
        case 'locked':  // Command confirmation from server.
            label.classList.add('shadow');
            break;
        case 'unlock':
            icon.classList.remove('show-column');
            label.classList.remove('remove-left-border-radius');
            message = '{"' + which + '":"' + action + '"}';  // {"altitude":"unlock"}.
            websocket.send(message);
            console.log('browser --> server ' + message);
            break;
        case 'unlocked':
            label.classList.remove('shadow');

    }
}

/**
 * ------------------------------------------------
 *      Buttons - set icon states.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.3 [2025-10-13-02:15pm].
 */
function toggleButtons(which) {
    let buttonIcon = null;
    let buttonState;
    switch (which) {
        case 'altitude':
            buttonIcon = btnAltitudeIcon;
            break;
        case 'laser':
            buttonIcon = btnLaserIcon;
            break;
        case 'location':
            buttonIcon = btnLocationIcon;
            break;
    }
    if (buttonIcon !== null) {
        if (buttonIcon.classList.contains('show-column')) {
            buttonState = 'unlock';     // Is locked, unlock.
        } else {
            buttonState = 'lock';       // Is unlocked, lock.
        }
        button(which, buttonState);
    }
}

/**
 * ------------------------------------------------
 *      Comm - set items.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.3 [2025-10-19-02:15pm].
 */
function comm(which, state) {
    const types = [commRtcm, commRtcm];
    switch (which) {
        case 'rtcm':
            item = commRtcm;
            break;
        case 'bt':
            item = commBt;
            break;
        default:
    }
    if(null !== item) {
        switch (state) {
            case 'up':
                item.classList.add('up');
                break;
            case 'down':
                item.classList.remove('up');
        }
    }
}

/**
 * ------------------------------------------------
 *      Battery - set items.
 * ------------------------------------------------
 *
 * example JSON: {"bat":93.07031,"batc":-1.2}
 *
 * @return void   No output is returned.
 * @since  3.0.7  [2025-11-10-11:30am].
 * @since  3.0.10 [2026-01-08-12:30pm] Shortened keywords.
 */
function battery(which, info) {
    let item = null;
    switch (which) {
        case 'soc':
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

            // <=20% add alert.
            if (batterySoc <= 20) {
                batteryBars.forEach(bar => {
                    bar.classList.add('alert');
                });  
            }

            // >=30% remove alert.
            if (batterySoc >= 30) {
                batteryBars.forEach(bar => {
                    bar.classList.remove('alert');
                });
            }

            break;
        case 'change':
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
 * ------------------------------------------------
 *      Execute WebSocket message.
 * ------------------------------------------------
 *
 * @return void   No output is returned.
 * @since  3.0.7  [2025-11-14-09:30am].
 * @since  3.0.10 [2026-01-08-12:30pm] Shortened keywords.
 * @see    messageRcvWebSocket() in global.js.
 */
function operateMessage(key, value) {
    switch (key) {
        case 'status':                  // {"status":"ready"}.
            if(value === 'ready') {
                headerH1.classList.remove('red');
            }
            break;
        case 'ver':                     // {"ver":"3.0.3"}.
            version(value);
            break;
        case 'mode':                    // {"mode":"base"}.
            mode(value);
            break;
        case 'unit':                    // {"unit":"meters"}.
            unit(value);
            break;
        case 'fix':                     // {"fix":"float"}.
            fix(value);
            break;
        case 'siv':                     // {"siv":"24"}.
        case 'alt':                     // {"alt":"100.05"}. 3 posn = 10 mm.
        case 'lat':                     // {"lat":"35.60599395,"} 8 posn = 1.11 mm.
        case 'lon':                    // {"lon":"-78.79439717"} 8 posn = 1.11 mm.
        case 'vac':                     // {"vac":"0.014"}.
            number(key, value);         
            break;
        case 'hac':                     // {"hac":"0.016"}.
            number(key + '-lat', value);
            number(key + '-lon', value);          
            break;
        case 'altitudeLock':            // {"altitudeLock":"on"}.
        case 'laserLock':               // {"laserLock":"on"}.
        case 'locationLock':            // {"locationLock":"on"}.
        case 'unlock':                  // {"unlock":""}.
            break;
        case 'rtcm':                    // {"rtcm":"up"}.
        case 'bt':                      // {"bt":"up"}.
            break;
        case 'bat':                     // {"bat":"83.75"}.
            battery('soc', value);
            break;
        case 'batc':                    // {"batc":"-1.2"}.
            battery('change', value);
            break;
    }
}

/**
 * ------------------------------------------------
 *      Clear Operate UI.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.7  [2025-11-15-05:00pm].
 * @since  3.0.10 [2026-01-08-09:30am] Shortened keywords (e.g. latitude to lat).
 */
function clearOperateUi() {
version(         '-.-.-');
mode(             '-');
unit(            '-');
fix(           'down');
number('siv',    '--');
number('alt',  0);
number('lat',  0);
number('long', 0);
number('hac-lat',   0);
number('hac-lon',   0);
number('vac',       0);
comm('rtcm',   'down');
comm('bt',     'down');
battery('soc',      0);
}

/**
 * ============================================================================
 *                             Event listeners.
 * ============================================================================
 *
 * @since  3.0.3 [2025-10-20-01:30pm].
 */

/**
 * ------------------------------------------------
 *      General.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.3  [2025-10-22-01:30pm].
 * @since  3.0.10 [2026-01-07-02:00pm] Add update.
 */

// --- Page. ---
 document.addEventListener('DOMContentLoaded', () => {
    initWebSocket();
    update();
});

// --- Buttons. ---
btnAltitude.addEventListener('click', () => {
    toggleButtons('altitude');
});
btnLaser.addEventListener('click', () => {
    toggleButtons('laser');
});
btnLocation.addEventListener('click', () => {
    toggleButtons('location');
});
btnUnlock.addEventListener('click', () => {
    button('unlock', 'all');
});

/**
 * ============================================================================
 *                                    Test.
 * ============================================================================
 *
 * @since  3.0.3 [2025-11-14-09:45am].
 */

// version('3.0.3');
// mode('b');
// unit('m');
// fix('2'); // 0=down, 1-Single, 2=RTK-float, 3-RTK-fix.
// number('siv', 22);
// number('altitude', 100.05);
// number('latitude', 35.60599395);
// number('longitude', -78.79439717);
// number('hac', 0.014);
// number('vac', 0.016);
comm('rtcm', 'up');
comm('bt', 'up');
// battery('soc', 83.45);

function flash() {
    commRtcm.style.opacity = .2;
    setTimeout(function() {
        commRtcm.style.opacity = 1;
    }, 1000);
    flash();
}

/**
 * ------------------------------------------------
 *      Send update UI request to server.
 * ------------------------------------------------
 *
 * Send an update UI request to the server every UPDATE_UI_INTERVAL. 
 * 
 * @return void  No output is returned.
 * @since  3.0.10 [2026-01-07-04:30pm] New.
 * @since  3.0.11 [2026-01-13-11:45am] GNSS_INTERVAL.
 */
function update() {

    // --- Loop. ---
    setTimeout(function() { 
        if (1 == websocket.readyState) {
            message = '{"operate":"update"}';
            websocket.send(message);
            console.log('browser --> ' + message);
        }
        update();
    }, 450);
}

/**
 * ============================================================================
 *                             Run on page load.
 * ============================================================================
 *
 * @since  3.0.3 [2025-10-16-10:00am].
 */
