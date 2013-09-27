function StartPage() {
    this.updateFocus = true;
    var self = this;
    G.app.serverEvents.addEventListener("showAdded", function(event) {
        var show = JSON.parse(event.data);
        self.addShow(show);
        console.log("added show", show);
    });
}

StartPage.prototype.fetchInfos = function(callback) {
    var self = this;
    var unfetched = 2;
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
    $.getJSON("api/library/randomWallpaper", function(data) {
        console.log("got a bg pic:" + data.image);
        self.background = data.image;
        onFetched();
    });
}

StartPage.prototype.createNodes = function() {
    var self = this;
    this.fetchInfos(function() {
        var p = $(".page");
        G.app.setBackground(this.background);
        p.append("<h1>StartPage</h1>");
        self.createLists(p);
    });
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
        
        listHead.text(listName);
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

StartPage.prototype.liForShow = function(show) {
    var self = this;
    var item = $("<li></li>");
    item.text(show.name);
    item.mousemove(function() {
        if (self.updateFocus && !$(this).hasClass("focused")) {
            $("li").removeClass("focused");
            $(this).addClass("focused");
            $.getJSON("api/setPage/TvShowPage?" + $(this).text(), function() {
                // remote set done
            });
        }
    });
    item.click(function() {
        self.updateFocus = false;
        $.getJSON("api/setPage/TvShowPage?" + $(this).text(), function() {
            window.location.hash = "#!/TvShowPage"
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
        console.log("it happend!", show, item);
    }
}
