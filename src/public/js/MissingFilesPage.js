function MissingFilesPage() {
}

MissingFilesPage.prototype.createNodes = function() {
    var self = this;
    this.page = document.createElement("div");
    this.page.innerHTML = "<h1>Missing Files</h1>";

    this.content = document.createElement("div");
    this.page.appendChild(this.content);

    this.loadList(function() {

    });
    return this.page;
}

MissingFilesPage.prototype.removeNodes = function() {
    $(this.page).remove();
}

MissingFilesPage.prototype.groupData = function(data) {
    // TODO group by directory
    return data.missingFiles.reduce(function(acc, file) {
        acc[file.showName] = acc[file.showName] || [];
        acc[file.showName].push(file);
        return acc;
    }, {});
}

MissingFilesPage.prototype.loadList = function(callback) {
    var self = this;
    $.getJSON("/api/library/filter/missingFiles", function(data) {
        var grouped = self.groupData(data);

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
            var deleteButton = self.createDeleteButton(groupArray);
            $(groupEl).append(deleteButton);
            $(groupEl).append(nodes);
            self.content.appendChild(groupEl);
        }
        callback(grouped);
    });
}

MissingFilesPage.prototype.createDeleteButton = function(groupArray) {
    var button = document.createElement("button");
    button.textContent = "remove";
    $(button).click(function() {
        var paths = groupArray.map(function(file) {
            return file.path;
        });
        $.ajax({
            url: "/api/library/removeFiles/",
            type: "DELETE",
            data: JSON.stringify({paths: paths})
        }).done(function() {

        });;
    });
    return button;
}
