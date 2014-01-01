function PlayerUi() {
    this.tvShow = null;
}

PlayerUi.prototype.createNodes = function() {
    var self = this;
    var page = $(".playerFooter");
    
    this.togglePauseButton = $(document.createElement("span"));
    this.setPauseDisplay("paused")
    
    this.togglePauseButton.click(function() {
        $.getJSON("api/player/togglePause");
    });
    
    var stopButton = $(document.createElement("span"));
    stopButton.text("■");
    stopButton.addClass("stopButton");
    
    stopButton.click(function() {
        $.getJSON("api/player/stop");
    });
    
    var backwardsButton = $(document.createElement("span"));
    backwardsButton.text("<<");
    backwardsButton.click(function() {
        $.getJSON("api/player/backwards");
    });
    
    var forwardsButton = $(document.createElement("span"));
    forwardsButton.text(">>");
    forwardsButton.click(function() {
        $.getJSON("api/player/forwards");
    });
    
    $.getJSON("api/player/pauseStatus", function(data) {
        self.setPauseDisplay(data.status);
    });
    
    /* TODO impl in server
    $.getJSON("api/player/tvShow", function(data) {
        
    });
    */
    
    this.seekbar = new Seekbar();
    
    page.append(this.seekbar.tooltip);
    
    var playerControls = $(document.createElement("div"));
    playerControls.addClass("playerControls");

    playerControls.append(this.seekbar.bar);
    playerControls.append(backwardsButton);
    playerControls.append(this.togglePauseButton);
    playerControls.append(forwardsButton);
    playerControls.append(stopButton);
    page.append(playerControls);
    
    this.bindEvents();
    this.addBodyPadding();    
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
                 
