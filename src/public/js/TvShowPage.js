function TvShowPage() {
    this.tvShow = null;
}

TvShowPage.prototype.createNodes = function() {
    var self = this;
    var page = $(".page");
    
    var backButton = $("<input class='backButton' type='button'/>");
    backButton.attr("value", "back");
    backButton.click(function() {
        window.location.hash = "#!/";
    });
    
    var playButton = $("<input class='playButton' type='button'/>");
    playButton.attr("value", "play");
    playButton.attr("disabled", "disabled");
    
    playButton.click(function() {
        var file = $(".season:first li:first").attr("data-fileName");
        self.play(file);
        console.log("TODO check for first unplayed file and play it");
    });
    
    var subtitleTrackField = $("<input type=\"number\"/>");
    var audioTrackField = $("<input type=\"number\"/>");
    $.getJSON("api/page/playerSettings", function(data) {
        subtitleTrackField.get(0).value = data.subtitleTrack;
        audioTrackField.get(0).value = data.audioTrack;
        
        function setFn(key) {
            return function() {
                var number = this.value;
                data[key] = number;
                var json = JSON.stringify(data);
                $.getJSON("api/page/setPlayerSettings?" + json, function() {
                    console.log("set subtitle to ", number);
                });
            }
        }
        
        subtitleTrackField.on("input", setFn("subtitleTrack"));
        audioTrackField.on("input", setFn("audioTrack"));
    });
    
    var seasonsEl = $(document.createElement("div"));
    seasonsEl.addClass("seasons");
    
    $.getJSON("api/page/showDetails", function(data) {
        console.log(data);
        self.tvShow = data;
        playButton.removeAttr("disabled");
        $.each(data.seasons, function() {
            self.createSeasonList(this, seasonsEl);
        })
    });

    page.append(backButton);
    page.append(playButton);
    page.append("<span> sub: </span>");
    page.append(subtitleTrackField);
    page.append("<span> audio: </span>");
    page.append(audioTrackField);
    page.append(seasonsEl);
}

TvShowPage.prototype.createSeasonList = function(season, seasonsEl) {
    var self = this;
    var seasonEl = $(document.createElement("ul"));
    seasonEl.addClass("season");
    
    season.episodes = season.episodes.sort(function(a,b) {
        return a.episodeNumber < b.episodeNumber ? -1 : 1;
    })
    
    $.each(season.episodes, function() {
        var ep = this;
        var episodeEl = $(document.createElement("li"));
        var text = $("<span></span>");
        
        var title = 
            ep.episodeNumber + " " +
            ep.showName + " " +
            (ep.episodeName ? ("- " + ep.episodeName + " ") : "") +
            ep.releaseGroup;
        text.text(title);
        
        
        var toggleWatchButton = $("<span class=\"textButton\"></span>");
        toggleWatchButton.text(ep.watched ? "+ " : "- ");
        toggleWatchButton.click(function(event) {
            ep.watched = !ep.watched;
            toggleWatchButton.text(ep.watched ? "+ " : "- ");
            $.getJSON("api/library/toggleWatched?" + ep.path);
        });
        
        episodeEl.append(toggleWatchButton);
        episodeEl.append(text);
        
        episodeEl.attr("data-fileName", this.path);
        
        episodeEl.click(function() {
            if (!event.target.classList.contains("textButton")) {
                self.play($(this).nextAll("li").andSelf().map(function() {
                    return this.getAttribute("data-fileName");
                }).toArray());
            }
        });
        
        seasonEl.append(episodeEl);
    });
    seasonsEl.append(seasonEl);
}

TvShowPage.prototype.play = function(episode) {
    if (G.playerType == "stream") {
        G.video = "/video/" + episode; // TODO DON'T SAVE IT HERE
        window.location.hash = "#!/StreamPlayerPage/";
    } else {
        if (episode instanceof Array) {
            var json = {
                tvShow: this.tvShow.name,
                episodes: episode
            }
            
            $.getJSON("api/player/setPlaylist?" + JSON.stringify(json), function(data) {
                if (!data.error) {
                    window.location.hash = "#!/PlayerPage";
                }
            });
        
            /*
            $.ajax({
                url: "api/player/setPlaylist",
                type:"POST",
                data:JSON.stringify(json),
                processData: false
            }).done(function(data) {
                console.log("set playlist got response:", data)
                if (!data.error) {
                    window.location.hash = "#!/PlayerPage";
                }
            });
            */
            
        } else {
            var json = {
                tvShow: this.tvShow.name,
                filename: episode
            }
        
            $.getJSON("api/player/play?" + JSON.stringify(json), function(data) {
                if (!data.error) {
                    window.location.hash = "#!/PlayerPage";
                }
            });
        }
    }
}
