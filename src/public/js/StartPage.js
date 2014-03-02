function StartPage() {
    this.updateFocus = true;
    var self = this;
    G.app.serverEvents.addEventListener("showAdded", function(event) {
        var show = JSON.parse(event.data);
        self.addShow(show);
    });
}

StartPage.prototype.fetchInfos = function(callback) {
    var self = this;
    var unfetched = 1;
    var onFetched = function() {
        --unfetched;
        if (unfetched <= 0) {
            callback();
        }
    };
    $.getJSON("api/library/filter/lists", function(data) {
        self.lists = data.lists;
        onFetched();
    }).error(function(err, reason, dat, m) {
        console.error(err, reason, dat, m);
    });
}

StartPage.prototype.createNodes = function() {
    var self = this;
    this.page = $(document.createElement("div"));
    this.fetchInfos(function() {
        self.page.append("<h1>StartPage</h1>");
        self.createLists(self.page);
    });
    
    this.bindEvents();
    return this.page;
}

StartPage.prototype.removeNodes = function() {
    this.unbindEvents();
}

StartPage.prototype.bindEvents = function() {
    this.keyDownListener = function(event) {
        ListUtils.handleKeyDown(event);
    }
    window.addEventListener("keydown", this.keyDownListener);
}

StartPage.prototype.unbindEvents = function() {
    window.removeEventListener("keydown", this.keyDownListener);
}

StartPage.prototype.createLists = function(page) {
    var self = this;
    for (var listName in this.lists) {
        var list = this.lists[listName];
        if (list.length <= 0) {
            continue;
        }
        var listNode = $("<div class='showList list'></div>");
        var listHead = $("<div class='headline'></div>");
        list.node = listNode;
        
        var name = document.createElement("span");
        name.textContent = listName;
        
        (function(listNode) {
            var playButton = PlayButton.create();
            playButton.click(function() {
                var shows = listNode.find("li .name");
                var randomShow = shows.get(Math.floor(Math.random() * shows.length));
                if (randomShow) {
                    var randomShowName = randomShow.getAttribute("data-name");
                    PlayButton.ajaxClickCallback(randomShowName)();
                }
            });
            
            listHead.append(name);
            listHead.append(playButton);
        })(listNode);
        
        listNode.append(listHead);
        
        list.sort(function(a, b) {
            return a.name < b.name ? -1 : 1;
        });
        
        // show
        $.each(list, function() {
            var show = this;
            var item = self.liForShow(show);
            listNode.append(item);
        });
        page.append(listNode);
    };
}

// don't send a preview request when the mouse moves really fast
StartPage.prototype.delayedShowPreviewRequest = function(element, showName) {
    var delayId = window.setTimeout(function() {
        var el = $(element);
        if (el.hasClass("focused")) {
            $.getJSON("api/setPage/TvShowPage?" + showName);
        }
    }, 50);
}

StartPage.prototype.liForShow = function(show) {
    var self = this;
    var item = $(document.createElement("li"));
    item.attr("tabindex", "1");
    item.on("focus", function() {
        $(this).mousemove();
    });
    
    var name = document.createElement("span");
    name.className = "name";
    name.textContent = show.name;
    name.setAttribute("data-name", show.name);
    
    var watchCount = document.createElement("span");
    watchCount.className = "watchCount";
    watchCount.textContent = show.watchedEpisodes + "/" +
                             show.downloadedEpisodes + "/" + 
                             show.totalEpisodes;
    
    item.append(name);
    item.append(watchCount);
    item.mousemove(function() {
        if (self.updateFocus && !$(this).hasClass("focused")) {
            $("li.focused").removeClass("focused");
            $(this).addClass("focused");
            self.delayedShowPreviewRequest(this, show.name);
        }
    });
    item.click(function() {
        self.updateFocus = false;
        // todo don't send when the active page is already correct,
        // because this causes a new background img to be shown.
        $.getJSON("api/setPage/TvShowPage?" + show.name, function() {
            window.location.hash = "#!/TvShowPage/" + show.name;
        });
        //G.app.setPage(new TvShowPage());
    });
    return item;
}

StartPage.prototype.listsForShow = function(show) {
    var lists = [];
    for (var listName in this.lists) {
        if (listName === "airing" && show.airingStatus === "airing") {
            lists.push(this.lists[listName]);
        } else if (listName === "all") {
            lists.push(this.lists[listName]);
        } else if (listName === show.status) {
            lists.push(this.lists[listName]);
        }
    }
    if (lists.length === 0) {
        console.error("no lists for show", show, this.lists);
    }
    return lists;
}

// TODO sort the list again
StartPage.prototype.addShow = function(show) {
    var lists = this.listsForShow(show);
    for (var i=0; i < lists.length; ++i) {
        var item = this.liForShow(show);
        lists[i].node.append(item);
    }
}
