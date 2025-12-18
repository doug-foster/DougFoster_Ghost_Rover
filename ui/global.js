/**
 * *************************************
 *      Ghost Rover 3 - UI JS.
 * *************************************
 * 
 * global.js
 *
 * @author   D. Foster <doug@dougfoster.me>.
 * @since    3.0.7 [2025-11-15-04:45pm].
 * @link     http://dougfoster.me.
 * 
 * Websocket Messages:
 *   client --> server {"status":"ready"} = first part of two-way handshake.
 *   client <-- server {"status":"ready"} = second part of two-way handshake.
 *   client --> server {"echo":"hello"} = echo send.
 *   client <-- server {"echo":"hello"} = echo receive.
 *   client <-- server {"version":"3.0.3"}.
 *   client <-- server {"mode":"base"}.
 *   client <-- server {"units":"meters"}.
 *   client <-- server {"fix":"float"}.
 *   client <-- server {"altitude":"100.05"}.
 *   client <-- server {"latitude":"35.5536111"}.
 *   client <-- server {"longitude":"-78.7713888"}.
 *   client <-- server {"vac":"0.014"}.
 *   client <-- server {"hac":"0.016"}.
 *   client --> server {"altitude":"lock"} = lock altitude.
 *   client <-- server {"altitude":"locked"} = altitude locked.
 *   client --> server {"altitude":"unlock"} = unlock altitude.
 *   client <-- server {"altitude":"unlocked"} = altitude unlocked.
 *   client --> server {"laser":"lock"} = lock laser.
 *   client <-- server {"laser":"locked"} = laser locked.
 *   client --> server {"laser":"unlock"} = unlock laser.
 *   client <-- server {"laser":"unlocked"} = altitude laser.
 *   client --> server {"location":"lock"} = lock location.
 *   client <-- server {"location":"locked"} = location locked.
 *   client --> server {"location":"unlock"} = unlock location.
 *   client <-- server {"location":"unlocked"} = altitude location.

 *   client <-- server {"rtcm":"up"} = rtcm status.
 *   client <-- server {"rtcm":"down"} = rtcm status.
 *   client <-- server {"bt":"up"} = bt status.
 *   client <-- server {"bt":"down"} = bt status.
 *   client <-- server {"batterySoc":"83.75"} = battery state of charge.
 *   client <-- server {"batteryAlert":"true"} = battery alert (below threshold).
 *   client --> server {"listFiles":""} = list SD files.
 *   client <-- server {"listFiles":"list of files"} = list of SD files.
 *   client --> server {"viewFile":"filename"} = view SD file.
 *   client --> server {"downloadFile":"filename"} = download SD file.
 *   client --> server {"deleteFile":"filename"} = delete SD file.
*/

/**
 * ============================================================================
 *                                Global vars.
 * ============================================================================
 *
 * @since    3.0.8 [2025-11-21-09:00am].
 */

// --- Test. ---
// const uploadUrl = 'https://httpbin.org/post';
// const sdFiles   = ['file1.txt', 'file2.txt', 'file3.txt', 'file4.txt'];  // Test data.

// --- HTTP. ---
let wsEndpoint  = 'ws://192.168.23.1/ghostRover'
let websocket;

// --- Battery. ---
let batterySoc;

// --- General. ---
const headerH1 = document.querySelector('header h1');
const newPage  = document.querySelectorAll('a.new-page');
const roverName = 'GhostRover';
const RECONNECT_INTERVAL = 2000;        // Server reconnect interval.
const HEARTBEAT_INTERVAL = 3000;        // Client/server heartbeat interval.
let lastHeartbeatTime    = Date.now();  // Last heartbeat time.

/**
 * ============================================================================
 *                                Functions.
 * ============================================================================
 *
 * @since  3.0.3 [2025-10-16-01:45pm].
 */

/**
 * ------------------------------------------------
 *      Init WebSocket.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.3 [2025-10-13-02:15pm].
 */
function initWebSocket() {
    console.log('Opening new WebSocket ...');
    websocket           = new WebSocket(wsEndpoint);
    websocket.onopen    = openedWebSocket;
    websocket.onclose   = closedWebSocket;
    websocket.onerror   = errorWebsocket;
    websocket.onmessage = messageRcvWebSocket;
}

/**
 * ------------------------------------------------
 *      WebSocket - opened.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since. 3.0.7 [2025-11-15-12:30pm].
 */
function openedWebSocket(event) {
    // --- UI indicator that a WebSocket is now open. ---
    console.log('WebSocket opened to ' + wsEndpoint + '.');
    headerH1.textContent = roverName;
    headerH1.classList.remove('red');
    // --- Ready handshake with server. ---
    let message = '{"' + document.title + '":"ready"}';  // {"operate":"ready"}.
    websocket.send(message);
    console.log('browser --> server ' + message);
}

/**
 * ------------------------------------------------
 *      WebSocket - closed.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.3 [2025-10-15-01:15pm].
 */
function closedWebSocket(event) {
    console.log('WebSocket closed.');
    headerH1.textContent = 'No server';
    headerH1.classList.add('red');
}

/**
 * ------------------------------------------------
 *      WebSocket - error.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.3 [2025-10-15-01:15pm].
 */
function errorWebsocket() {
    if (!headerH1.classList.contains('red')) {
        headerH1.classList.add('red');
    }
}
/**
 * ------------------------------------------------
 *      WebSocket - stopped.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.3 [2025-10-22-01:30pm].
 */
async function stopWebSocket(event) {
    let waitToClose = new Promise(function(resolve) {           // Link to new page was prevented.
        let message = '{"' + document.title + '":"leaving"}';   // {"operate":"leaving"}.
        websocket.send(message);                                // Send "leaving" messaage to server.
        console.log('browser --> server ' + message);
        websocket.close();                                      // Close socket.
        console.log('WebSocket stopped.');
        setTimeout(function() {
            console.log("Leaving.");
            window.location = event.target.closest('a').href;   // Continue to link target.
            }, 100);
    });
    await waitToClose;
}

/**
 * ------------------------------------------------
 *      WebSocket - message from server, what to do?
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.7 [2025-11-15-02:00pm].
 * @see    operateMessage() in operate.js.
 * @see    filesMessage() in files.js.
 */
function messageRcvWebSocket(event) {
    var myObj = JSON.parse(event.data);
    console.log('browser <-- server ' + event.data);
    Object.entries(myObj).forEach(([key, value]) => {

        // --- Check for heartbeat. ---
        if (('echo' == key) && ('alive' == value)) {    // Received a heartbeat.
            lastHeartbeatTime = Date.now();
        }

        // --- Route message to its page. ---
        if (window.location.pathname.includes('operate')) {
            operateMessage(key, value);         // operate.js.
        } else if (window.location.pathname.includes('files')) {
            filesMessage(key, value);           // files.js
        }
    });
}
/**
 * ------------------------------------------------
 *      WebSocket - heartbeat.
 * ------------------------------------------------
 *
 * Send an echo message to the server every HEARTBEAT_INTERVAL.
 * 
 * @return void  No output is returned.
 * @since  3.0.7 [2025-11-15-04:30pm].
 */
function webSocketHeartbeat() {
    setTimeout(function() { 
        if (1 == websocket.readyState) {
            message = '{"echo":"alive"}';
            websocket.send(message);
            console.log('browser --> server ' + message);
        }
        webSocketHeartbeat();
    }, HEARTBEAT_INTERVAL);
}

/**
 * ------------------------------------------------
 *      WebSocket - check heartbeat.
 * ------------------------------------------------
 *
 * If no heartbeat message is received from the server within the timeout window,
 *   stop the websocket and start a new one.
 * 
 * @return void  No output is returned.
 * @since  3.0.7 [2025-11-15-04:45pm].
 */
function webSocketCheckHeartbeat() {
    setTimeout(function() { 
        const timeSinceLastHeartbeat = Date.now() - lastHeartbeatTime;
        if ( timeSinceLastHeartbeat > (HEARTBEAT_INTERVAL * 3)) {
            if (1 != websocket.readyState) {
                websocket.close();
                initWebSocket();
                lastHeartbeatTime = Date.now();
                if (window.location.pathname.includes('operate')) {
                    clearOperateUi();
                }
            }
        }
        webSocketCheckHeartbeat();
    }, HEARTBEAT_INTERVAL);
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
 * @return void  No output is returned.
 * @since  3.0.3 [2025-10-22-01:30pm].
 */

 // --- Navigation links. ---
 document.addEventListener('DOMContentLoaded', () => {
    webSocketHeartbeat();       // Start heartbeat.
    webSocketCheckHeartbeat();  // Check heartbeat.
    newPage.forEach(page => {
        page.addEventListener('click', (event) => {
            event.preventDefault();
            event.stopPropagation();
            stopWebSocket(event);
        });
    });
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

