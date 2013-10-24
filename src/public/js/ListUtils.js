var ListUtils = {}

ListUtils.handleKeyDown = function(event) {
    if (event.keyCode == 13) { // enter
        if (document.activeElement && document.activeElement.tagName == "LI") {
            $(document.activeElement).click();
        }
    } else {
        return false;
    }
    event.preventDefault();
    return true;
}
