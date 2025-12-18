/**
 * *************************************
 *      Ghost Rover 3 - UI Javascript.
 * *************************************
 *
 * @author   D. Foster <doug@dougfoster.me>.
 * @since    3.0.3 [2025-10-13-02:15pm].
 * @link     http://dougfoster.me.
*/

/**
 * ============================================================================
 *                             Init on page load.
 * ============================================================================
 *
 * @return void  No output is returned.
 * @since  3.0.3 [2025-10-13-02:15pm].
 */
const initApp = () => {

    /**
     * ============================================================================
     *                              Global vars.
     * ============================================================================
     *
     * @since  3.0.3 [2025-10-13-05:15pm].
     */

    // --- Test. ---
    // const uploadUrl = 'https://httpbin.org/post';
    // const sdFiles   = ['file1.txt', 'file2.txt', 'file3.txt', 'file4.txt'];  // Test data.

    // --- General: HTTP. ---
    let wSEndpoint  = 'ws://' + window.location.hostname + '/ws';
    let websocket;
    const listUrl   = '/list';
    const uploadUrl = '/upload';

    // --- General: header. ---
    const header            = document.querySelector('#header');
    const headerH1          = document.querySelector('#header h1');
    const headerVersion     = document.querySelector('#header #version');
    const headerMode        = document.querySelector('#header #mode');
    const headerUnits       = document.querySelector('#header #units');
    const version           = '3.0.3';

    // --- Operation section. ---
    const oper              = document.querySelector('#operation');

    // --- Operation section: gnss. ---
    const gnssNone          = document.querySelector('.gnss #none .inside');
    const gnssSingle        = document.querySelector('.gnss #single .inside');
    const gnssFloat         = document.querySelector('.gnss #float .inside');
    const gnssFix           = document.querySelector('.gnss #fix .inside');
    const gnssState         = document.querySelector('.gnss #state');

    // --- Operation section: Number. ---
    const numElevation      = document.querySelector('.number #elevation');
    const numLatitude       = document.querySelector('.number #latitude');
    const numLongitude      = document.querySelector('.number #longitude');
    const numVAC            = document.querySelector('.number #vac');
    const numHAC            = document.querySelectorAll('.number .hac');

    // --- Operation section: Button. ---
    const btnElevation      = document.querySelector('.button #elevation');
    const btnElevationIcon  = document.querySelector('.button #elevation .icon');
    const btnElevationLabel = document.querySelector('.button #elevation .label');
    const btnLaser          = document.querySelector('.button #laser');
    const btnLaserIcon      = document.querySelector('.button #laser .icon');
    const btnLaserLabel     = document.querySelector('.button #laser .label');
    const btnLocation       = document.querySelector('.button #location');
    const btnLocationIcon   = document.querySelector('.button #location .icon');
    const btnLocationLabel  = document.querySelector('.button #location .label');
    const btnUnlock         = document.querySelector('.button #unlock');
    const btnUnlockLabel    = document.querySelector('.button #unlock .label');
    const btns              = document.querySelectorAll('.btn');

    // --- Operation section: Info. ---
    const infoRtcm          = document.querySelector('.info #rtcm');
    const infoBt            = document.querySelector('.info #bt');
    const infoBatterySoc    = document.querySelector('.info #soc');
    const infoBatteryBars   = document.querySelectorAll('.info .battery .bar');

    // --- Config section. ---
    const config            = document.querySelector('#config');

    // --- Config section: Info. ---
    const filesProcess      = document.querySelector('#do-files');
    const filesArea         = document.querySelector('#files-area');
    const filesAreaFiles    = document.querySelectorAll('#files-area .file');
    const filesOpSelect     = document.querySelector('#files-op-select');
    const dropArea          = document.querySelector('#drop-area');
    const uploadFiles       = document.querySelectorAll('#drop-area .upload-file');
    const startUpload       = document.querySelector('#start-upload');
    const uploadStatus      = document.querySelector('#uploadStatus');
    let uploadResponse      = null;
    let droppedFiles        = null;


    /**
     * ============================================================================
     *                                 Functions.
     * ============================================================================
     *
     * @since  3.0.3 [2025-10-13-02:15pm].
     */

    /**
     * ------------------------------------------------
     *      Set header items.
     * ------------------------------------------------
     *
     * @return void  No output is returned.
     * @since  3.0.3 [2025-10-13-02:15pm].
     */
    function hdr(which, string = 'meters') {
        let item = null;
        switch (which) {
            case 'version':
                item = headerVersion;
                break;
            case 'mode':
                item = headerMode;
                break;
            case 'units':
                item = headerUnits;
        }
        if (item !== null) {
            item.innerHTML = string;
        }
    }

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
        websocket           = new WebSocket(wSEndpoint);
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
     * @since  3.0.3 [2025-10-13-02:15pm].
     */
    function openedWebSocket(event) {
        console.log('WebSocket opened to ' + wSEndpoint + '.');
    }

    /**
     * ------------------------------------------------
     *      WebSocket - closed.
     * ------------------------------------------------
     *
     * @return void  No output is returned.
     * @since  3.0.3 [2025-10-13-02:15pm].
     */
    function closedWebSocket(event) {
        console.log('WebSocket closed.');
        headerH1.classList.add('red');
        setTimeout(initWebSocket, 2000);
    }

    /**
     * ------------------------------------------------
     *      WebSocket - error.
     * ------------------------------------------------
     *
     * @return void  No output is returned.
     * @since  3.0.3 [2025-10-13-02:15pm].
     */
    function errorWebsocket() {
        if (!headerH1.classList.contains('red')) {
            headerH1.classList.add('red');
        }
    }

    /**
     * ------------------------------------------------
     *      WebSocket - message from server.
     * ------------------------------------------------
     *
     * @return void  No output is returned.
     * @since  3.0.3 [2025-10-13-02:15pm].
     */
    function messageRcvWebSocket(event) {
        console.log(event.data);
        var myObj = JSON.parse(event.data);
        Object.entries(myObj).forEach(([key, value]) => {
            switch (key) {
                case 'status':
                    if(value === 'ready') {
                        headerH1.classList.remove('red');
                    }
                    break;
                case 'version':
                case 'units':
                    hdr(key, value);
                    break;
                case 'gnssSection':
                    gnss(value);
                    break;
                case 'elevation':
                case 'latitude':
                case 'longitude':
                case 'vac':
                case 'hac':
                    number(key, value);
                    break;
                case 'elevation':
                case 'laser':
                case 'location':
                    button(key, value);  // Confirmation (action + 'ed').
                    break;
                case 'rtcm':
                case 'bt':
                case 'batSoc':
                case 'batAlert':               
                    info(key, value);
            }
        });
    }

    /**
     * ------------------------------------------------
     *      Operation: GNSS - set fix state.
     * ------------------------------------------------
     *
     * @return void  No output is returned.
     * @since  3.0.3 [2025-10-13-02:15pm].
     */
    function gnss(state) {
        const states = [gnssNone, gnssSingle, gnssFloat, gnssFix];
        states.forEach(type => {
            type.classList.remove('active');
        });
        let which = null, status = null;
        switch (state) {
            case 'down':  // Command to server.
                which = gnssNone;
                status = 'gnss DOWN';
                break;
            case 'single':  // Command to server.\
                which = gnssSingle;
                status = 'SINGLE FIX';
                break;
            case 'float':  // Command to server.
                which = gnssFloat;
                status = 'RTK-FLOAT';
                break;
            case 'fix':  // Command to server.
                which = gnssFix;
                status = 'RTK-FIX';
        }
        if (which!=null) {
            which.classList.add('active');
            gnssState.innerHTML = status;
        }
    }

    /**
     * ------------------------------------------------
     *      Operation: numbers - set values.
     * ------------------------------------------------
     *
     * @return void  No output is returned.
     * @since  3.0.3 [2025-10-13-02:15pm].
     */
    function number(which, num) {
        let numItem = null;
        switch (which) {
            case 'elevation':  // Command to server.
                numItem = numElevation;
                decimal = 100;
                break;
            case 'latitude':  // Command confirmation from server.
                numItem = numLatitude;
                decimal = 10000000;
                break;
            case 'longitude':
                numItem = numLongitude;
                decimal = 10000000;
                break;
            case 'vac':
                numItem = numVAC;
                decimal = 1000;
            case 'hac':
                numItem = numHAC;
        }
        if (which !== num) {
            numItem.innerHTML = Math.round(num * decimal) / decimal;
        }
    }

    /**
     * ------------------------------------------------
     *      Operation: buttons - set button states.
     * ------------------------------------------------
     *
     * @return void  No output is returned.
     * @since  3.0.3 [2025-10-13-02:15pm].
     */
    function button(which, action) {
        let icon = null, label = null;
        switch (which) {
            case 'elevation':
                icon  = btnElevationIcon;
                label = btnElevationLabel;
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
                button('elevation', 'unlock');
                button('laser', 'unlock');
                button('location', 'unlock');
                setTimeout(function() { btnUnlockLabel.classList.remove('shadow'); }, 100);
                // websocket.send('{"laser":"unlock", "elevation":"unlock", "location":"unlock"}');
                button('elevation', 'unlocked');    // Test.
                button('laser', 'unlocked');        // Test.
                button('location', 'unlocked');     // Test.
        }
        switch (action) {
            case 'lock':  // Command to server.
                icon.classList.add('show-column');
                label.classList.add('remove-left-border-radius');
                // websocket.send('{' + which + ' : ' + action + '}');
                button(which, action + 'ed');     // Test.
                break;
            case 'locked':  // Command confirmation from server.
                label.classList.add('shadow');
                break;
            case 'unlock':
                icon.classList.remove('show-column');
                label.classList.remove('remove-left-border-radius');
                // websocket.send('{' + which + ' : ' + action + '}');
                button(which, action + 'ed');     // Test.
                break;
            case 'unlocked':
                label.classList.remove('shadow');
                break;
            case 'all':
        }
    }

    /**
     * ------------------------------------------------
     *      Operation: buttons - set icon states.
     * ------------------------------------------------
     *
     * @return void  No output is returned.
     * @since  3.0.3 [2025-10-13-02:15pm].
     */
    function toggleButtons(which) {
        let buttonIcon = null;
        switch (which) {
            case 'elevation':
                buttonIcon = btnElevationIcon;
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
                button(which, 'unlock');  // Is locked, unlock.
            } else {
                button(which, 'lock');  // Is unlocked, lock.
            }
        }
    }

    /**
     * ------------------------------------------------
     *      Operation: info - set items.
     * ------------------------------------------------
     *
     * @return void  No output is returned.
     * @since  3.0.3 [2025-10-13-02:15pm].
     */
    function info(which, info) {
        let item = null;
        switch (which) {
            case 'rtcm':
                item = rtcm;
                break;
            case 'bt':
                item = bt;
                break;
            case 'batSoc':
                let charge = parseFloat(info);
                infoBatterySoc.innerHTML = Math.round(charge) + '%';
                let index = 0;
                for ( let infoBatteryBar of infoBatteryBars ) {
                    var power = Math.round(charge / 10);
                    if (index != power) {
                        infoBatteryBar.classList.add('active');
                        index++;
                    } else {
                        infoBatteryBar.classList.remove('active');
                    }
                }
                break;
            case 'batAlert':
                // TBD.
        }
        switch (info) {
            case 'up':
                item.classList.add('up');
                break;
            case 'down':
                item.classList.remove('up');
        }
    }

    /**
     * ------------------------------------------------
     *      Config: SD files - list files.
     * ------------------------------------------------
     *
     * @return void  No output is returned.
     * @since  3.0.3 [2025-10-13-02:15pm].
     */
    async function sdListFiles() {
    let sdFiles = [];
        document.querySelectorAll('#config #files-area .file').forEach(file => {
            file.remove();  // Remove previous files.
        });
        try {
            listResponse = await fetch(listUrl);
            if(!listResponse.ok) {
                throw new Error('Server response: ' + listResponse.status);
            }
            let data    = await listResponse.text();
            let lines   = data.split('\n');
            for (let i = 0; i < lines.length; i++) {
                if ((lines[i][0] !== '.') && (lines[i] !== '')) {
                    var line = lines[i].trim();
                    sdFiles.push(line);
                }
            }
        } catch(error) {
            showUploadStatus('Failed to load files: ' + error.message, 'error');
        }
        for (const sdFile of sdFiles) {
            const newElement = document.createElement('div');
            newElement.textContent = sdFile;
            newElement.classList.add('file');  // Add file.
            filesArea.appendChild(newElement);
            newElement.addEventListener('click', (event) => {
                newElement.classList.toggle('selected');  // Select file.
            });
        };
    }

    /**
     * ------------------------------------------------
     *      Config: SD files - download files.
     * ------------------------------------------------
     *
     * @return void  No output is returned.
     * @since  3.0.3 [2025-10-14-05:15pm].
     */
    async function sdDownloadFiles() {
        const filesSelected = document.querySelectorAll('#files-area .selected')
        for (const fileSelected of filesSelected) {
            alert(fileSelected.textContent);
        }
    }

    /**
     * ------------------------------------------------
     *      Config: SD drop area - prevent defaults.
     * ------------------------------------------------
     *
     * @return void  No output is returned.
     * @since  3.0.3 [2025-10-13-02:15pm].
     */
    function preventDefaults(e) {
        e.preventDefault();
        e.stopPropagation();
    }

    /**
     * ------------------------------------------------
     *      Config: SD drop area - add/remove highlight.
     * ------------------------------------------------
     *
     * @return void  No output is returned.
     * @since  3.0.3 [2025-10-13-02:15pm].
     */
    function active()   {dropArea.classList.add("highlight-drop");}
    function inactive() {dropArea.classList.remove("highlight-drop");}

    /**
     * ------------------------------------------------
     *      Config: SD drop area - list files.
     * ------------------------------------------------
     *
     * @return void  No output is returned.
     * @since  3.0.3 [2025-10-13-02:15pm].
     */
    function listDroppedFiles(e) {
        document.querySelectorAll('#drop-area .upload-file').forEach(file => {
            file.remove();                      // Remove previous files.
        });
        droppedFiles = e.dataTransfer.files;    // Dropped files object.
        const filesToUpload = [...droppedFiles].forEach(file => {
            const newElement = document.createElement('div');
            newElement.textContent = file['name'];
            newElement.classList.add('upload-file');
            dropArea.appendChild(newElement);   // Add file.
            newElement.addEventListener('click', (event) => {
                event.target.remove();          // Remove file.
                // [2025-10-13] need code to remove file from droppedFiles
            });
        })
    }

    /**
     * ------------------------------------------------
     *      Config: SD drop area - upload files.
     * ------------------------------------------------
     *
     * @global droppedFiles.
     * @return void  No output is returned.
     * @since  3.0.3 [2025-10-13-05:15pm].
     * @see    listDroppedFiles().
     */
  
    async function uploadDroppedFiles(e) {
        let numFiles = droppedFiles.length;
        let dummy = 1;
        if ( numFiles > 0) {
            try {
                for(var i = 0; i < droppedFiles.length; i++) {
                    await uploadFile(droppedFiles[i]);
                }
                showUploadStatus('All files uploaded successfully!', 'success');
            } catch(error) {
                showUploadStatus('Upload failed: ' + error, 'error');
            } finally {
                // uploadBtn.disabled = false;
            }
            // filesToUpload.forEach(async file => {
            //     await uploadFile(file);  // Upload file.
            //     if(uploadResponse.status == 200) {
            //         showUploadStatus(file.innerText + ' uploaded');  // Status update.
            //         file.remove();  // Remove file.
            //         numFiles--;
            //         if (0 == numFiles) { // Done.
            //             showUploadStatus('Done. ' + filesToUpload.length + ' files uploaded.', 'success');
            //         }
            //     } else {
            //         throw new Error( showUploadStatus('Upload Failed.', 'error') );
            //         return;
            //     }
            // });
        }
    }

    async function uploadFile(file) {
        var formData = new FormData();
        formData.append('file', file);

        var response = await fetch('/upload', {
            method: 'POST',
            body: formData
        });

        if(!response.ok) {
            throw new Error(file.name + ' upload failed');
        }

        console.log('Uploaded: ' + file.name);
    }

    /**
     * ------------------------------------------------
     *      Config: SD drop area - upload one file.
     * ------------------------------------------------
     *
     * @return void  No output is returned.
     * @since  3.0.3 [2025-10-13-02:15pm].
     */
    // async function uploadFile(file) {
    //     let form = new FormData();
    //     form.append('file', file);
    //     uploadResponse   = await fetch(uploadUrl, { method: 'POST', body:form });
    //     const uploadData = await uploadResponse.json();
    //     console.log(uploadResponse);
    //     console.log(uploadData);
    // }

    /**
     * ------------------------------------------------
     *      Config: SD drop area - display upload status.
     * ------------------------------------------------
     *
     * @return void  No output is returned.
     * @since  3.0.3 [2025-10-13-02:15pm].
     */
    function showUploadStatus(message, type = '') {
        uploadStatus.textContent = message;
        uploadStatus.className = 'show ' + type;
        if(type === 'success') {
            setTimeout(function() { uploadStatus.className = 'hide'; }, 1000);
        }
    }

    /**
     * ============================================================================
     *                             Event listeners.
     * ============================================================================
     *
     * @since  3.0.3 [2025-10-13-02:15pm].
     */

    /**
     * ------------------------------------------------
     *      General.
     * ------------------------------------------------
     *
     * @return void  No output is returned.
     * @since  3.0.3 [2025-10-13-02:15pm].
     */
    // window.addEventListener('load', initWebSocket);

    /**
     * ------------------------------------------------
     *      Header.
     * ------------------------------------------------
     *
     * @return void  No output is returned.
     * @since  3.0.3 [2025-10-13-02:15pm].
     */
    header.addEventListener('click', (event) => {
        if (headerH1.innerHTML == 'Ghost Rover') {
            headerH1.innerHTML = 'Config';
        } else {
            headerH1.innerHTML = 'Ghost Rover';
        }
        oper.classList.toggle('collapsed');
        config.classList.toggle('collapsed');
    });

     /**
     * ------------------------------------------------
     *      Operation: buttons.
     * ------------------------------------------------
     *
     * @return void  No output is returned.
     * @since  3.0.3 [2025-10-13-02:15pm].
     */
    btnElevation.addEventListener('click', (event) => {
        toggleButtons('elevation');
    });
    btnLaser.addEventListener('click', (event) => {
        toggleButtons('laser');
    });
    btnLocation.addEventListener('click', (event) => {
        toggleButtons('location');
    });
    btnUnlock.addEventListener('click', (event) => {
        button('unlock', 'all');  // Unlock all.
        // websocket.send('{"status":"ready"}');  // JSON.
    });

     /**
     * ------------------------------------------------
     *      Operation section: SD.
     * ------------------------------------------------
     *
     * @return void  No output is returned.
     * @since  3.0.3 [2025-10-13-02:15pm].
     */
    // --- List/download/delete SD files. ---
    filesProcess.addEventListener('click', (event) => {
        switch (event.target.nextElementSibling.value) {
            case 'list':
                sdListFiles();
                break;
            case 'download':
                sdDownloadFiles();
                alert('download SD files').
                break
            case 'delete':
                alert('delete SD files')
                break
        }
    });

    // --- Prevent default actions. ---
    ['dragenter', 'dragover', 'dragleave', 'drop'].forEach(eventName => {
        dropArea.addEventListener(eventName, preventDefaults, false);
    });

    // --- Add/remove drop area highlight. ---
    ['dragenter', 'dragover'].forEach(eventName => {
        dropArea.addEventListener(eventName, active, false);
    });
    ['dragleave', 'drop'].forEach(eventName => {
        dropArea.addEventListener(eventName, inactive, false);
    });

    // --- List dropped files. ---
    dropArea.addEventListener('drop', listDroppedFiles);

    // --- Upload dropped files. ---
    startUpload.addEventListener('click', uploadDroppedFiles);

    /**
     * ============================================================================
     *                                  Test.
     * ============================================================================
     *
     * @since  3.0.3 [2025-10-13-02:15pm].
     */

    hdr('version', version);
    hdr('units');
    hdr('mode', 'base');
    // gnss('down');
    // gnss('single');
    // gnss('float');
    gnss('float');
    number('elevation', 100.05);
    number('latitude', 35.5536111);
    number('longitude', -78.7713888);
    number('hac', 0.014);
    number('vac', 0.016);
    info('rtcm', 'up');
    info('bt', 'up');
    info('batSoc', 83.45);
    info('batAlert', false);
}

/**
 * ============================================================================
 *                             Run on page load.
 * ============================================================================
 *
 * @since  3.0.3 [2025-10-13-02:15pm].
 */

// Use readystatechange = "complete" instead if verifying all _external_ resources also loaded.
document.addEventListener("DOMContentLoaded", initApp); // Be sure DOM is loaded and parsed.
