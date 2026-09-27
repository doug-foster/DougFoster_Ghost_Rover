/**
 * *************************************************************************
 *  Ghost Rover 3 - UI JS.
 * *************************************************************************
 * 
 * template.js
 *
 * @author D. Foster <doug@dougfoster.me>.
 * @since  3.4.1 [2025-10-26-04:45pm] New.
 * @link   http://dougfoster.me.
 */

/**
 * =========================================================================
 *  Global vars.
 * =========================================================================
 *
 * @since 3.4.1 [2025-10-26-04:45pm] New.
 */

const rtcmSelect         = document.querySelector('#instructions #rtcm-mode');
const noInstructions     = document.querySelector('#instructions #off');
const bridgeInstructions = document.querySelector('#instructions #bridge');
const ntripInstructions  = document.querySelector('#instructions #ntrip');
const radioInstructions  = document.querySelector('#instructions #radio');


/**
 * =========================================================================
 *  Functions.
 * =========================================================================
 *
 * @since 3.4.1 [2025-10-26-04:45pm] New.
 */

/**
 * =========================================================================
 *  Event listeners.
 * =========================================================================
 *
 * @since 3.4.1 [2025-10-26-04:45pm] New.
 */

// --- Page. ---
 document.addEventListener('DOMContentLoaded', () => {
    webSocketInit();
});

rtcmSelect.addEventListener('change', (event) => {
    showRtcmInstruction();
});

/**
 * -------------------------------------------------------------------------
 *  Show specific instructions for an rtmSelect mode. 
 * -------------------------------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.4.1 [2025-10-27-11:00am] New.
 */
function showRtcmInstruction() {
    switch (rtcmSelect.value) {
        case 'off':
            noInstructions.classList.remove('hide');
            bridgeInstructions.classList.add('hide');
            ntripInstructions.classList.add('hide');
            radioInstructions.classList.add('hide');
            break;
        case 'bridge':
            noInstructions.classList.add('hide');
            bridgeInstructions.classList.remove('hide');
            ntripInstructions.classList.add('hide');
            radioInstructions.classList.add('hide');
            break;
        case 'ntrip':
            noInstructions.classList.add('hide');
            bridgeInstructions.classList.add('hide');
            ntripInstructions.classList.remove('hide');
            radioInstructions.classList.add('hide');
            break;
        case 'radio':
            noInstructions.classList.add('hide');
            bridgeInstructions.classList.add('hide');
            ntripInstructions.classList.add('hide');
            radioInstructions.classList.remove('hide');
            break;
    }
}

/**
 * =========================================================================
 *  Test.
 * =========================================================================
 *
 * @since 3.4.1 [2025-10-26-04:45pm] New.
 */

/**
 * =========================================================================
 *  Run on page load.
 * =========================================================================
 *
 * @since 3.4.1 [2025-10-26-04:45pm] New.
 */
