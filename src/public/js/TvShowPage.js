function TvShowPage(tvShowName) {
    this.tvShowName = tvShowName;
}

TvShowPage.prototype.bindEvents = function() {
    this.keyDownListener = function(event) {
        ListUtils.handleKeyDown(event);
    }
    window.addEventListener("keydown", this.keyDownListener);
}

TvShowPage.prototype.apiPrefix = function() {
    return "api/library/tvshow/" + encodeURIComponent(this.tvShow.name);
}

TvShowPage.prototype.unbindEvents = function() {
    window.removeEventListener("keydown", this.keyDownListener);
}

TvShowPage.prototype.createNodes = function() {
    var self = this;
    this.page = $(document.createElement("div"));
    
    var backButton = $("<input class='backButton' type='button'/>");
    backButton.attr("value", "back");
    backButton.click(function() {
        window.location.hash = "#!/";
    });
    
    this.playButton = PlayButton.create();
    this.playButton.attr("disabled", "disabled");
    
    
    function addSelectItem(list, value, label) {
        var item = document.createElement("option");
        item.value = value;
        item.textContent = label || value;
        list.appendChild(item);
    }
    this.statusList = document.createElement("select");
    addSelectItem(this.statusList, "automatic");
    addSelectItem(this.statusList, "completed");
    addSelectItem(this.statusList, "watching");
    addSelectItem(this.statusList, "waitingForNewEpisodes");
    addSelectItem(this.statusList, "onHold");
    addSelectItem(this.statusList, "dropped");
    addSelectItem(this.statusList, "planToWatch");

    this.statusList.setAttribute("disabled", "disabled");
    this.statusList.className = "statusSelect";
    
    this.statusList.onchange = function() {
        // TODO refresh ep list if complete or planToWatch
        $.getJSON(self.apiPrefix() + "/setStatus?" + this.value);
    }
    
    var subtitleTrackField = $("<input type=\"number\"/>");
    var audioTrackField = $("<input type=\"number\"/>");
    
    
    this.episodesEl = $(document.createElement("div"));
    this.episodesEl.addClass("seasons");
    
    // TODO make sure this is the current page without changing when it is
    var requestUrl = this.tvShowName
        ? "api/library/tvshow/"+ encodeURIComponent(this.tvShowName) +"/details"
        : "api/page/details";
        
    if (this.tvShowName) {
        var url = "api/assurePage/TvShowPage?" + 
            encodeURIComponent(this.tvShowName);
        $.getJSON(url);
    }

    $.getJSON(requestUrl, function(data) {
        self.tvShow = data;
        
        //TODO cleanup
        $.getJSON(self.apiPrefix() + "/playerSettings", function(data) {
            subtitleTrackField.get(0).value = data.subtitleTrack;
            audioTrackField.get(0).value = data.audioTrack;
            
            function setFn(key) {
                return function() {
                    var number = this.value;
                    data[key] = number;
                    var json = JSON.stringify(data);
                    $.ajax({
                        url:self.apiPrefix() + "/playerSettings",
                        type: "PUT",
                        data: json
                    }).complete(function() {
                        console.log("set subtitle to ", number);
                    });
                }
            }
            
            subtitleTrackField.on("input", setFn("subtitleTrack"));
            audioTrackField.on("input", setFn("audioTrack"));
        });
        
        self.playButton.removeAttr("disabled");
        self.statusList.removeAttribute("disabled");
        self.statusList.value = data.customStatus;
        self.createReleaseGroupPreference(data.releaseGroupPreference);
        self.createEpisodeList(data.episodes, self.episodesEl);
        self.createRewatchMarker(data.rewatchMarker);        
    });

    this.page.append(backButton);
    this.page.append(this.playButton);
    this.page.append("<span> sub: </span>");
    this.page.append(subtitleTrackField);
    this.page.append("<span> audio: </span>");
    this.page.append(audioTrackField);
    this.page.append(this.episodesEl);
    this.page.append(this.statusList);
    this.bindEvents();
    return this.page;
}

TvShowPage.prototype.removeNodes = function() {
    this.unbindEvents();
}

TvShowPage.prototype.createEpisodeList = function(episodes, episodesEl) {
    var self = this;
    var seasonEl = $(document.createElement("ul"));
    seasonEl.addClass("season");
    
    this.episodeList = new EpisodeList(episodes);
    PlayButton.initOnClick(this.playButton, this.episodeList);
     
    var isRewatchMarker = false;
    $.each(this.episodeList.episodes, function() {
        var ep = this;
        
        if (!ep.exists) {
            return;
        }
        
        var episodeEl = $(document.createElement("li"));
        var text = $(document.createElement("span"));
        
        if (isRewatchMarker) {
            episodeEl.addClass("rewatchMarker");
        }
        if (self.tvShow.rewatchMarker >= 0 && 
            ep.numericEpisodeNumber == self.tvShow.rewatchMarker) {
            isRewatchMarker = true;
        }

        var title = 
            ep.episodeNumber + " " +
            ep.showName + " " +
            (ep.episodeName ? ("- " + ep.episodeName + " ") : "") +
            ep.releaseGroup;
        text.text(title);
        
        
        function watchButtonSetDisplay() {
            if (ep.watched) {
                toggleWatchedButton.text("+ ");
                toggleWatchedButton.addClass("watched");
            } else {
                toggleWatchedButton.text("- ");
                toggleWatchedButton.removeClass("watched");
            }
        }
        
        var toggleWatchedButton = $(document.createElement("span"));
        toggleWatchedButton.get(0).className = "textButton toggleWatchedButton";
        watchButtonSetDisplay();
        toggleWatchedButton.click(function(event) {
            ep.watched = !ep.watched;
            watchButtonSetDisplay();
            $.getJSON("api/library/toggleWatched?" + ep.path);
       });
       
        var chapterList = document.createElement("div");
        chapterList.className = "chapterList";
        chapterList.textContent = "-> chapters"
        
        $(chapterList).hover(function() {
            if (ep.metaData) {
                return;
            }
            ep.metaData = {};
            $.getJSON("api/library/movieFileMetaData?" + ep.path, function(data) {
                chapterList.innerHTML = "";
                ep.metaData = data;
                
                $.each(data.chapters, function() {
                    var chapter = this;
                    
                    var chapterLink = document.createElement("span");
                    chapterLink.className = "chapterLink";
                    chapterLink.textContent = chapter.title;
                    chapterList.appendChild(chapterLink);
                    
                    chapterLink.onclick = function(event) {
                        var json = {
                            filename: ep.path
                        };
                        var start = Math.floor(chapter.start);
                        $.getJSON("api/player/play?" + JSON.stringify(json), function(data) {
                            $.getJSON("api/player/jumpTo?" + start, function(data) {
                                //window.location.hash = "#!/PlayerPage";
                            });
                        });
                    }
                });
                console.log(ep.metaData);
            });
        });
        
        episodeEl.append(toggleWatchedButton);
        episodeEl.append(text);
        episodeEl.append(chapterList);
        episodeEl.attr("data-fileName", this.path);
        
        episodeEl.click(function(event) {
            if (!event.target.classList.contains("textButton")) {
                self.play($(this).nextAll("li").andSelf().map(function() {
                    return this.getAttribute("data-fileName");
                }).toArray());
            }
        });
        
        episodeEl.attr("tabindex", "1");
        episodeEl.on("focus", function() {
            $(this).mousemove();
        });
        episodeEl.mousemove(function() {
            $("li.focused").removeClass("focused");
            $(this).addClass("focused");
        });
        
        seasonEl.append(episodeEl);
    });
    episodesEl.append(seasonEl);
}

TvShowPage.prototype.createReleaseGroupPreference = function(array) {
    var rgp = document.createElement("div");
    rgp.className = "releaseGroupPreference";
    for (var i=0; i < array.length; ++i) {
        var group = array[i];
        var el = document.createElement("span");
        (function() {
            var fromIndex = i;
            el.onclick = function() {
                var toIndex = fromIndex -1;
                if (toIndex >= 0) {
                    var element = array.splice(fromIndex, 1)[0];
                    array.splice(toIndex, 0, element);
                    set(array);
                }
            }
        })();
        var caption = group;
        if (i+1 < array.length) {
            caption += " > ";
        }
        el.textContent = caption;
        rgp.appendChild(el);
    }
    
    var self = this;
    function set(newArray) {
        var json = JSON.stringify(newArray);
        // TODO move page independent
        $.ajax({
            url: self.apiPrefix() + "/releaseGroupPreference",
            type: "PUT",
            data: json
        }).complete(function() {
            console.log("set releaseGroupPreference to ", newArray);
            
            // TODO unify refetch; write show name into #url
            var url = "api/library/tvShowDetails?" + encodeURIComponent(this.tvShow.name);
            $.getJSON("api/page/showDetails", function(data) {
                self.tvShow = data;
                self.episodesEl.empty();
                self.createEpisodeList(data.episodes, self.episodesEl);
                self.playButton.unbind("click");
                PlayButton.initOnClick(self.playButton, self.episodeList);
                $(self.releaseGroupPreference).remove();
                self.createReleaseGroupPreference(data.releaseGroupPreference);
            });
        });
    }
    
    this.page.append(rgp);
    this.releaseGroupPreference = rgp;
}

TvShowPage.prototype.createRewatchMarker = function(marker) {
    if (marker >= 0) {
        var self = this;
        var button = document.createElement("button");
        button.innerHTML = "clear rewatch"
        button.onclick = function() {
            var uri = self.apiPrefix() + "/setRewatchMarker?-1";
            $.getJSON(uri, function(data) {
                console.log("reset rewatch", data);
                $(".rewatchMarker").removeClass("rewatchMarker");
                button.parentElement.removeChild(button);
            });
        }
        this.page.append(button);
    }
}

TvShowPage.prototype.play = function(episodes) {
    if (!(episodes instanceof Array)) {
        episodes = [episode];
    }
    var json = {
        tvShow: this.tvShow.name,
        episodes: episodes
    }
    
    $.ajax({
        url: "api/player/setPlaylist",
        type: "POST",
        data: JSON.stringify(json)
    }).done(function(data) {
        if (data.error) {
            alert(data.error);
        }
    });
}
