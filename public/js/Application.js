function Application() {
    this.screen = null;
}

Application.prototype.setScreenFromLocationHash = function()
{
    if (!this.screen) {
        $("#main").empty();
    }
    
    var fields = Utils.parseHashUrl(window.location.hash);
    
    if (fields.length === 1) {
        this.setScreen(new BooksScreen(fields[0]));
    } else if (fields.length === 2) {
        this.setScreen(new TrackScreen(fields[0], fields[1]));
    } else {
        this.setScreen(new AuthorsScreen());
    }
}

Application.prototype.setScreen = function(screen) {
    if (this.screen) {
        this.screen.removeNodes();
    }
    this.screen = screen;
    if (screen) {
        screen.createNodes();
    }
}
