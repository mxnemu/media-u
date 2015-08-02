var ClipCreator = (function() {
    var template;
    function open(sec, implicit, genCallback, closeCallback) {
        var settings = getSettings();
        if (implicit && !settings["always-show"]) {
            return;
        }
        if (template) {
            var container = $(".clip-settings-container").append(template);
            container.show();
            settings.genCallback = genCallback;
            settings.closeCallback = closeCallback;
            initUi(sec, container, settings);
        } else {
            $.ajax("templates/clipsettings.html").done(function(data) {
                template = data;
                open(sec, implicit, genCallback, closeCallback);
            });
        }
    }

    function close(settings, data) {
        if (settings.closeCallback) {
            settings.closeCallback(data)
        }
        $(".clip-settings-container").empty();
        $(".clip-settings-container").hide();
    }

    function getSettings() {
        try {
            var settings = JSON.parse(localStorage.getItem("media-u-clip-settings"));
            if (!settings) { throw "invalid settings" }
            return settings;
        } catch(e) {
            return {
                "max-size": 3.0,
                "output-type": "type-webm",
                "always-show": true,
                "audio": false
            };
        }
    }

    function partial(fn, args) {
        return function() {
            var mergedArgs = args.concat(Array.prototype.slice.apply(arguments));
            return fn.apply(this, mergedArgs);
        }
    }

    function initUi(sec, container, settings) {
        var startInput = $(".start-time", container);
        var endInput = $(".end-time", container);
        startInput.val(sec);
        $(".start-button", container).click(partial(getTime, [null, startInput]));
        $(".end-button", container).click(partial(getTime, [null, endInput]));

        $(".max-size", container).val(settings["max-size"]);
        $(".audio", container).get(0).checked = settings["audio"];
        $("." + settings["output-type"], container).click();
        $(".always-show", container).get(0).checked = settings["always-show"];
        $(".store-settings", container).click(partial(store, [container]));
        $(".cancel-button", container).click(partial(close, [settings]));

        $(".create-button", container).click(function() {
            createClip(container, settings);
        });
    }

    function getTime(callback, input) {
        $.getJSON("api/player/exactProgress", function(data) {
            if (callback) {
                callback(data.seconds);
            }
            if (input) {
                input.val(data.seconds);
            }
        });
    }

    function createClip(container, settings) {
        var startTime = parseFloat($(".start-time", container).val());
        var endTime = parseFloat($(".end-time", container).val());
        if (isNaN(startTime)) {
            alert("start time can not be NaN! " + startTime);
            return;
        }
        if (isNaN(endTime)) {
            getTime(function() {
                createClip(container, settings);
            }, $(".end-time", container));
            return;
        }
        var outputTypes = {"type-webm": "webm", "type-gif": "gif"}
        var json = {
            start: startTime,
            end: endTime,
            "max-size": parseFloat($(".max-size", container).val()),
            "output-type": outputTypes[$("[name=\"output-type\"]:checked", container).get(0).className],
            "audio": $(".audio", container).get(0).checked
        }
        if (settings.genCallback) {
            settings.genCallback();
        }

        var createButton = $(".create-button", container);
        createButton.text("generating...");
        createButton.unbind("click");
        $(".create-button", container).text("generating...");
        createForData(json, partial(close, [settings]));
    }

    function createForData(json, callback) {
        var jsonStr = JSON.stringify(json);
        $.getJSON("api/player/createGif?" + jsonStr, callback);
    }

    function store(container) {
        var json = {
            "max-size": parseFloat($(".max-size", container).val()),
            "output-type": $("[name=\"output-type\"]:checked", container).get(0).className,
            "always-show": $(".always-show", container).get(0).checked,
            "audio": $(".audio", container).get(0).checked
        }
        localStorage.setItem("media-u-clip-settings", JSON.stringify(json));
        var button = $(".store-settings", container);
        var oldText = button.text();
        button.text("done!");
        window.setTimeout(function() {
            button.text(oldText);
        }, 2000);
    }

    return {
        open: open,
        getTime: getTime,
        create: createForData
    }
})()
