/**
 * Show a given element. It basically consists of removing the "hidden" class.
 @param id the id of the DOM element to show
 */
function showDOMEl(id) {
    document.getElementById(id).className =
       document.getElementById(id).className.replace
             ( /(?:^|\s)hidden(?!\S)/g , '' );
}

/**
 * Hide a given element. It basically consists of adding the "hidden" class.
 @param id the id of the DOM element to show
 */
function hideDOMEl(id) {
    document.getElementById(id).className += "hidden";
}

/**
 * Toggle the status of the element.
 * Ex.: if the element is shown, hide it. And Viceversa.
 * @param id the id of the DOM element to toggle
 */
function toggleDOMEl(id) {
    if (document.getElementById(id).className.match(/(?:^|\s)hidden(?!\S)/)) {
        showDOMEl(id);
    } else {
        hideDOMEl(id);
    }
}

/**
 * Change the form action and register the sensor to be monitored.
 */
function registerForAlert(id) {
	// change form action
	f = document.getElementById("measureAdd" + id);
    f.action = "addAlertMonitor";
    // remove required attribute from value, it's not needed for this action
    var v = document.getElementById("value" + id);
    v.required = false;
    f.submit();
}
