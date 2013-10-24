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
    this.fetchInfos(function() {
        var p = $(".page");
        p.append("<h1>StartPage</h1>");
        self.createLists(p);
    });
    
    this.bindEvents();
}

StartPage.prototype.removeNodes = function() {
    this.unbindEvents();
}

StartPage.prototype.bindEvents = function() {
    this.keyDownListener = function(event) {
        
        if (event.keyCode == 40) { // arrow down
            var focused = $("li.focused");
            if (focused.length) {
                var next = focused.next("li");
                if (!next.length) {
                    next = focused.parent().next().find("li:first")
                }
                next.mousemove();
            } else {
                $("li:first").mousemove();
            }
            event.preventDefault();
        } else if (event.keyCode == 38) {
            var focused = $("li.focused");
            if (focused.length) {
                var next = focused.prev("li");
                if (!next.length) {
                    next = focused.parent().prev().find("li:last")
                }
                next.mousemove();
            } else {
                $("li:first").mousemove();
            }
            event.preventDefault();
        } else if (event.keyCode == 13) {
            var focused = $("li.focused");
            if (focused.length) {
                focused.click();
                event.preventDefault();
            }
        }
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

// don't send a preview request when the mouse moves really fast
StartPage.prototype.delayedShowPreviewRequest = function(element) {
    var delayId = window.setTimeout(function() {
        var el = $(element);
        if (el.hasClass("focused")) {
            $.getJSON("api/setPage/TvShowPage?" + el.text());
        }
    }, 50);
}

StartPage.prototype.liForShow = function(show) {
    var self = this;
    var item = $("<li></li>");
    item.text(show.name);
    item.mousemove(function() {
        if (self.updateFocus && !$(this).hasClass("focused")) {
            $("li").removeClass("focused");
            $(this).addClass("focused");
            self.delayedShowPreviewRequest(this);
        }
    });
    item.click(function() {
        self.updateFocus = false;
        // todo don't send when the active page is already correct,
        // because this causes a new background img to be shown.
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
    }
}
