function PlayerUi() {
    this.tvShow = null;
    this.gifData = null;
}

PlayerUi.prototype.createNodes = function() {
    var self = this;
    this.node = $(document.createElement("div"));
    this.node.hide();

    this.togglePauseButton = $(document.createElement("span"));
    this.togglePauseButton.addClass("button");
    this.setPauseDisplay("paused")

    this.togglePauseButton.click(function() {
        $.getJSON("api/player/togglePause");
    });

    var stopButton = $(document.createElement("span"));
    stopButton.text("■");
    stopButton.addClass("stopButton button");

    stopButton.click(function() {
        $.getJSON("api/player/stop");
    });

    var backwardsButton = $(document.createElement("span"));
    backwardsButton.text("<<");
    backwardsButton.addClass("button");
    backwardsButton.click(function() {
        $.getJSON("api/player/backwards");
    });

    var forwardsButton = $(document.createElement("span"));
    forwardsButton.text(">>");
    forwardsButton.addClass("button");
    forwardsButton.click(function() {
        $.getJSON("api/player/forwards");
    });

    $.getJSON("api/player/pauseStatus", function(data) {
        self.setPauseDisplay(data.status);
    });

    this.gifButton = $(document.createElement("span"));
    this.gifButton.text("gif");
    this.gifButton.addClass("button gifbutton");
    this.gifButton.click(function() {
        ClipCreator.getTime(function(seconds) {
            self.setGifTime(seconds);
        });
    });

    var playerControls = $(document.createElement("div"));
    playerControls.addClass("playerControls");

    this.seekbar = new Seekbar(playerControls);
    this.node.append(this.seekbar.tooltip);

    this.seekbar.progress.onReady(function() {
        if (self.seekbar.progress.isPlaying()) {
            self.node.stop(true, true);
            self.node.show("slow");
        }
    });

    this.seekbar.progress.onReset(function() {
        self.node.stop(true, true);
        self.node.hide("slow");
    });


    playerControls.append(this.seekbar.bar);
    playerControls.append(backwardsButton);
    playerControls.append(this.togglePauseButton);
    playerControls.append(forwardsButton);
    playerControls.append(stopButton);
    playerControls.append(this.gifButton);
    this.node.append(playerControls);

    this.bindEvents();
    this.addBodyPadding();
    return this.node;
}

PlayerUi.prototype.setGifTime = function(seconds) {
    var self = this;
    var genCallback = function() {
        self.gifData.progress = "generating";
        self.gifButton.text("gif_gen");
    }
    var closeCallback = function() {
        self.gifData = null;
        self.gifButton.text("gif");
    }

    if (!this.gifData) {
        this.gifData = {
            start: seconds,
            end: null
        }
        this.gifButton.text("gif_end");
        ClipCreator.open(seconds, true, genCallback, closeCallback);
    } else if (this.gifData.progress != "generating") {
        this.gifData.end = seconds;
        ClipCreator.create(this.gifData, closeCallback);
        genCallback();
    }
}

PlayerUi.prototype.removeNodes = function() {
    this.seekbar.destroy();
    this.unbindEvents();
}

PlayerUi.prototype.addBodyPadding = function() {
    var self = this;
    window.setTimeout(function() {
        $(".page").css("padding-bottom", $(".playerControls").outerHeight() + "px");
    }, 100)
}

PlayerUi.prototype.bindEvents = function() {
    var self = this;
    this.onPlaybackStartedListener = function(event) {
        self.setPauseDisplay("unpaused");
    }

    this.onPausedListener = function(event) {
        self.setPauseDisplay("paused");
    }

    this.onUnpausedListener = function(event) {
        self.setPauseDisplay("unpaused");
    }

    G.app.serverEvents.addEventListener("playbackStarted", this.onPlaybackStartedListener);
    G.app.serverEvents.addEventListener("paused", this.onPausedListener);
    G.app.serverEvents.addEventListener("unpaused", this.onUnpausedListener);
}

PlayerUi.prototype.unbindEvents = function() {
    G.app.serverEvents.removeEventListener("playbackStarted", this.onPlaybackStartedListener);
    G.app.serverEvents.removeEventListener("paused", this.onPausedListener);
    G.app.serverEvents.removeEventListener("unpaused", this.onUnpausedListener);
}

PlayerUi.prototype.setPauseDisplay = function(status) {
    this.togglePauseButton.attr("data-status", status);
    if (status != "unpaused") {
        this.togglePauseButton.text("▶");
    } else {
        this.togglePauseButton.text("II");
    }
}
