function MissingFilesPage() {
}

MissingFilesPage.prototype.createNodes = function() {
    this.page = document.createElement("div");
    this.page.innerHTML = "<h1>Missing Files</h1>";

    this.content = document.createElement("div");
    this.page.appendChild(this.content);

    this.loadList();
    return this.page;
}

MissingFilesPage.prototype.removeNodes = function() {
    $(this.page).remove();
}

MissingFilesPage.prototype.loadList = function() {
    var self = this;
    $.getJSON("/api/library/filter/missingFiles", function(data) {
        // TODO group by directory
        var grouped = data.missingFiles.reduce(function(acc, file) {
            acc[file.showName] = acc[file.showName] || [];
            acc[file.showName].push(file);
            return acc;
        }, {});

        $(self.content).empty();
        for (var groupKey in grouped) {
            var groupEl = document.createElement("ol");
            groupEl.textContent = groupKey;
            var groupArray = grouped[groupKey];

            var nodes = groupArray.map(function(file) {
                var el = document.createElement("li");
                el.textContent = file.path;
                return el;
            });
            $(groupEl).append(nodes);
            self.content.appendChild(groupEl);
        }
    });
}
