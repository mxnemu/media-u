function TvShowPage() {
}

TvShowPage.prototype.createNodes = function() {
    var p = $(".page");
    // debug stuff
    var playButton = $("<input class='playButton'/>");
    // TODO debug atm, give num and show as json
    // TODO block page function and display loading while json not provided
    $.getJSON("api/getUnwatchedEpisodes?10", function(data) {
        playButton.attr("data-episode", data[0]);
    });
    
    playButton.click(function() {
        $.getJSON("api/playFile?" + playButton.attr("data-fileName"), function() {
            
        });
    })
    p.append(playButton);
}
