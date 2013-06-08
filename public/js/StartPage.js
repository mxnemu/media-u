function StartPage() {
    this.updateFocus = true;
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
    $.getJSON("api/page/lists", function(data) {
        self.lists = data.lists;
        onFetched();
    }).error(function(err, reason, dat, m) {
        console.error(err, reason, dat, m);
    });
    $.getJSON("api/page/background", function(data) {
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
        var items = $("<ul></ul>");
        listHead.text(listName);
        listNode.append(listHead);
        // show
        $.each(list, function() {
            var show = this;
            var item = $("<li></li>");
            item.text(this.name);
            item.mousemove(function() {
                if (self.updateFocus && !$(this).hasClass("focused")) {
                    $("li").removeClass("focused");
                    $(this).addClass("focused");
                    $.getJSON("api/setPage/TvShowPage?" + $(this).text(), function() {
                        console.log("setPage");
                    });
                }
            });
            item.click(function() {
                self.updateFocus = false;
                console.log("should change to showPage TODO impl");
            });
            listNode.append(item);
        });
        page.append(listNode);
    };
}
