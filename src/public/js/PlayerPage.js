function PlayerPage() {
    this.tvShow = null;
}

PlayerPage.prototype.createNodes = function() {
    var self = this;
    var page = $(".page");
    
    var backButton = $("<input class='backButton' type='button'/>");
    backButton.attr("value", "back");
    backButton.click(function() {
        window.location.hash = "#!/TvShowPage"; // TODO get active page via api
    });
    
    this.togglePauseButton = $(document.createElement("span"));
    this.setPauseDisplay("paused")
    
    this.togglePauseButton.click(function() {
        if (self.togglePauseButton.attr("data-status") == "unPaused") {
            $.getJSON("api/player/togglePause", function(data) {
                self.setPauseDisplay(data.status);
            });
            self.togglePauseButton.text("▶");
        } else {
            $.getJSON("api/player/togglePause", function(data) {
                self.setPauseDisplay(data.status);
            });
            self.togglePauseButton.text("II");
        }
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
    
    this.seekbar = new Seekbar(this);
    
    page.append(this.seekbar.tooltip);
    
    var playerControls = $(document.createElement("div"));
    playerControls.addClass("playerControls");

    playerControls.append(this.seekbar.bar);
    playerControls.append(backwardsButton);
    playerControls.append(this.togglePauseButton);
    playerControls.append(forwardsButton);
    playerControls.append(stopButton);
    page.append(backButton);
    page.append(playerControls);
}

PlayerPage.prototype.removeNodes = function() {
    this.seekbar.destroy();
}

PlayerPage.prototype.setPauseDisplay = function(status) {
    this.togglePauseButton.attr("data-status", status);
    if (status != "unPaused") {
        this.togglePauseButton.text("▶");
    } else {
        this.togglePauseButton.text("II");
    }
}
                 
