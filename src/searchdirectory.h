#ifndef SEARCHDIRECTORY_H
#define SEARCHDIRECTORY_H

#include <QDir>

class SearchDirectory
{
public:
    SearchDirectory(QDir dir, bool enabled = true);

public:
    QDir dir;
    bool enabled;
};

#endif // SEARCHDIRECTORY_H
