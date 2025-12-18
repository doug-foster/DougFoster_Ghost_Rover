/**
 * *************************************
 *      Ghost Rover 3 - UI JS.
 * *************************************
 * 
 * files.js
 *
 * @author   D. Foster <doug@dougfoster.me>.
 * @since    3.0.7 [2025-11-11-05:30pm].
 * @link     http://dougfoster.me.
*/

/**
 * ============================================================================
 *                                Global vars.
 * ============================================================================
 *
 * @since 3.0.7 [2025-11-11-03:30pm].
 */

// --- General. ---
const dropArea          = document.querySelector('#drop-area');
const dropAreaBanner    = document.querySelector('#drop-area-banner');
// const uploadStatus      = document.querySelector('#upload-status');
// let uploadResponse      = null;
let droppedFiles        = null;
const listUrl           = '/list';
const uploadUrl         = '/upload';
const fileList          = document.querySelector('#file-list');


/**
 * ============================================================================
 *                                Functions.
 * ============================================================================
 *
 * @since 3.0.7 [2025-11-11-03:30pm].
 */

/**
 * ------------------------------------------------
 *      Execute WebSocket message.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.7 [2025-11-11-05:30pm].
 * @see    messageRcvWebSocket() in global.js.
 */
function filesMessage(key, value) {
    switch (key) {     
        case 'listFiles':  // {"listFiles":"/index.html\n/upload-image-icon.png\n"}
            fileListBuild(value);
            break;
        case 'fileDeleted':  // {"fileDeleted":"/menu.html"}
            document.querySelectorAll('#files .selected').forEach(file => {
                if ( value === file.textContent) {
                    file.remove();  // Update list.
                }
            });
            break;
        case 'fileNOTdeleted':  // {"fileNOTdeleted":"/menu.html"}
            alert( 'NOT DELETED: ' + value);
            document.querySelectorAll('#files .file').forEach(file => {
                if ( value === file.textContent) {
                    file.classList.toggle('selected');  // Unselect all files.
                }
            });
            break;
    }
}

/**
 * ------------------------------------------------
 *      File list - request.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.7 [2025-11-10-12:45pm].
 */
function fileListRequest(e) {
    message = '{"listFiles":""}';
    websocket.send(message);
    console.log('browser --> server ' + message);
}

/**
 * ------------------------------------------------
 *      File list - build.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.7 [2025-11-10-09:15pm].
 * @see messageRcvWebSocket() in global.js.
 */
function fileListBuild(listOfFiles) {

    // --- Remove current list of files. ---
    if (fileList.childNodes.length > 0) {
        fileList.childNodes.forEach(child => {
            child.remove();
        });
    }

    // --- Add new list of files. ---
    let files = listOfFiles.replace(/,+$/, '').split(',');  // Remove trailing ',', create array of files.
    files.forEach(file => {
        let newChild = fileList.appendChild(document.createElement('div'));
        newChild.textContent = file;
        newChild.classList.add('file');
    });

    // --- File select/unselect listeners. ---
    document.querySelectorAll('#drop-area .file').forEach(file => {
        file.addEventListener('click', (event) => {
            file.classList.toggle('selected');
        });
    });
    dropAreaBanner.addEventListener('click', (event) => {
        document.querySelectorAll('#files .selected').forEach(file => {
            file.classList.toggle('selected');
        });
    });
}

/**
 * ------------------------------------------------
 *      Drop area - upload files.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.7 [2025-11-11-03:30pm].
 */
function uploadDroppedFiles(e) {

    // --- Check for correct server. ---
    if (window.location.pathname.includes('ui')) {
        alert('This is not the ESP32 server. Stopping.');
        return false;
    }

    // --- Prepare. ---
    droppedFiles = e.dataTransfer.files;  // Dropped files object.
    let message = "Upload these files? ";
    for (let i = 0; i < droppedFiles.length; i++) {
        message += '"' + [...droppedFiles][i]['name'] + '"';
        (i < (droppedFiles.length -1)) ? message += ', ' : message += '.';
    }
    let upload = confirm(message);  // Proceed?
    if ((upload == null || upload == '')) {
        return;
    } else {

        // --- Upload each file. ---
        filesToUpload = [...droppedFiles].forEach(async file => {
            let formData = new FormData();
            formData.append('file', file);
            try {
                const response = await fetch('/upload', {
                    method: 'POST',
                    body: formData
                });

                if(response.ok) {
                    const result = await response.text();
                    console.log(result);
                    alert(file.name + ' uploaded.');

                    // -- Regenerate list (and event listners). --
                    setTimeout(function() { window.location.reload(); }, 500);
                } else {
                    alert(file.name + ' upload failed.');
                }
            } catch (error) {
                console.error('Error:', error);
                alert('Error occurred during upload.');
            }
        });
    }
}

/**
 * ------------------------------------------------
 *      Pprevent defaults.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.3 [2025-10-22-11:45am].
 */
function preventDefaults(e) {
    e.preventDefault();
    e.stopPropagation();
}

/**
 * ============================================================================
 *                             Event listeners.
 * ============================================================================
 *
 * @since  3.0.7 [2025-11-10-05:30pm].
 */

/**
 * ------------------------------------------------
 *      General.
 * ------------------------------------------------
 *
 * @return void  No output is returned.
 * @since  3.0.7 [2025-11-10-05:30pm].
 */

// --- Page. ---
 document.addEventListener('DOMContentLoaded', () => {
    initWebSocket();
    setTimeout(function() { fileListRequest(); }, 500);
});

// --- Prevent default actions. ---
['dragenter', 'dragover', 'dragleave', 'drop'].forEach(eventName => {
    dropArea.addEventListener(eventName, preventDefaults, false);
});

// --- Add/remove drop area highlight. ---
['dragenter', 'dragover'].forEach(eventName => {
    dropArea.addEventListener(eventName, (event) => {
        dropArea.classList.add("highlight-drop");
    });
});
['dragleave', 'drop'].forEach(eventName => {
    dropArea.addEventListener(eventName, (event) => {
        dropArea.classList.remove("highlight-drop");
    });
});

// --- Upload dropped files. ---
dropArea.addEventListener('drop', uploadDroppedFiles);

// --- File select/unselect. ---
// Listeners are in fileListBuild() since list of files is dynamically built.

// --- Button actions. ---
// -- View a file. --
document.querySelector('#view').addEventListener('click', (event) => {
    const viewFiles = document.querySelectorAll('#files .selected');
    if (viewFiles.length == 0) {
        return;
    }
    if (viewFiles.length > 1) {
        alert ('View only one file at a time.');
        viewFiles.forEach(file => {
            file.classList.toggle('selected');
        });
    } else {
        viewFiles[0].classList.toggle('selected');
        let prefix = '';
        if (window.location.pathname.includes('ui')) {  // If page hosted from VS Code "Live Server" instead of ESP32.
            prefix = '/ui';
        }
        window.open(prefix + viewFiles[0].textContent, '_blank', 'menubar=yes');
    }
});

// -- Download a file. --
document.querySelector('#download').addEventListener('click', (event) => {
    document.querySelectorAll('#files .selected').forEach(file => {
        file.classList.toggle('selected');
        window.location.href = `/download?file=${encodeURIComponent(file.innerText)}`;
    });
});

// -- Delete a file. --
document.querySelector('#delete').addEventListener('click', (event) => {
    document.querySelectorAll('#files .selected').forEach(file => {
        if (confirm('delete "' + file.textContent + '"')) {
            let message = '{"deleteFile":"' + file.innerText + '"}';
            websocket.send(message);
            console.log('browser --> server ' + message);
        }
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
