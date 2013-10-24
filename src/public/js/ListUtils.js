var ListUtils = {}

ListUtils.handleKeyDown = function(event) {
    if (event.keyCode == 40) { // arrow down
        var focused = $("li.focused");
        if (focused.length) {
            var next = focused.next("li");
            if (!next.length) {
                next = focused.parent().next().find("li:first")
            }
            next.mousemove();
        } else {
            $("li:first").mousemove();
        }
        window.scroll(0, $(".focused").position().top - window.innerHeight/2);
        event.preventDefault();
    } else if (event.keyCode == 38) {
        var focused = $("li.focused");
        if (focused.length) {
            var next = focused.prev("li");
            if (!next.length) {
                next = focused.parent().prev().find("li:last")
            }
            next.mousemove();
        } else {
            $("li:first").mousemove();
        }
        window.scroll(0, $(".focused").position().top - window.innerHeight/2);
        event.preventDefault();
    } else if (event.keyCode == 13) {
        var focused = $("li.focused");
        if (focused.length) {
            focused.click();
            event.preventDefault();
        }
    } else {
        return false;
    }
    return true;
}
