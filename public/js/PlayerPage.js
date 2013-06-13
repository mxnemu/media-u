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
    
    var togglePauseButton = $(document.createElement("span"));
    togglePauseButton.text("II");
    
    togglePauseButton.click(function() {
        if (togglePauseButton.attr("data-status") == "unPaused") {
            $.getJSON("api/player/pause");
            togglePauseButton.text("▶");
        } else {
            $.getJSON("api/player/unPause");
            togglePauseButton.text("II");
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
        if (data.status != "unPaused") {
            togglePauseButton.text("▶");
        }
        togglePauseButton.attr("data-status", data.status);
    });
    
    /* TODO impl in server
    $.getJSON("api/player/tvShow", function(data) {
        
    });
    */

    var playerControls = $(document.createElement("div"));
    playerControls.addClass("playerControls");

    playerControls.append(backwardsButton);
    playerControls.append(togglePauseButton);
    playerControls.append(forwardsButton);
    playerControls.append(stopButton);
    page.append(backButton);
    page.append(playerControls);
}
