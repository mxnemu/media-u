PlayButton = {
    create: function(episodeList) {
        var button = $("<input class='playButton' type='button'/>");
        button.attr("value", "play");
        
        if (episodeList) {
            this.initOnClick(button, episodeList);
        }
        
        return button;
    },
    
    initOnClick: function(button, episodeList) {
        if (episodeList instanceof EpisodeList) {
            button.click(PlayButton.episodeListClickCallback(episodeList));
        } else if (typeof episodeList == "string") {
            button.click(PlayButton.ajaxClickCallback(episodeList));
        } else {
            var err = "invalid initOnClick with arg" + episodeList;
            throw err;
        }
    },
    
    initOnClickAjax: function() {
        
    },
    
    episodeListClickCallback: function(episodeList) {
        return function() {
            PlayButton.play(episodeList.unwatchedArray());
        }
    },
    
    ajaxClickCallback: function(tvShow) {
        return function() {
            // TODO make page independent version
            $.getJSON("/api/library/tvShowDetails?" + tvShow, function(data) {
                var episodeList = new EpisodeList(data.episodes);
                PlayButton.episodeListClickCallback(episodeList)();
            });
        }
    },
    
    play: function(files) {
        var json = {
            //tvShow: this.tvShow.name,
            episodes: files
        }
        
        $.getJSON("api/player/setPlaylist?" + JSON.stringify(json), function(data) {
            if (!data.error) {
                window.location.hash = "#!/PlayerPage";
            }
        });
    }
};
