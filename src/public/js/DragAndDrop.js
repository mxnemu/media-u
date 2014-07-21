var DragAndDrop = (function() {
    var bind = function(el, callback) {
        if (!el || !callback) {
            throw "DragAndDrop.bind needs target element and callback";
        }
    
        var dragOver = function(e) {e.preventDefault();return false;};
        var drop = function(e) {
            e.preventDefault();
            e.originalEvent.dataTransfer.items[0].getAsString(function(url) {
                callback(url)
            });
        };
        var listeners = [dragOver, drop];
    
        $(el).on('dragover', dragOver);
        $(el).on('drop', drop);
        
        return listeners;
    }
    
    var unbind = function(el, listeners) {
        if (listeners instanceof Array) {
            return listeners.map(function(t) {
                return [
                    $(el).unbind("dragover", t),
                    $(el).unbind("drop", t)
                ];
            });
        } else if (listeners === "all") {
            return [
                listeners.unbind("dragover"),
                listeners.unbind("drop")
            ]
        } else {
            throw "give [functions] to unbind or the string \"all\" to unbind all";
        }
    }
    
    return {
        bind: bind,
        unbind: unbind
    }
})();
