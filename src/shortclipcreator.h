#ifndef SHORTCLIPCREATOR_H
#define SHORTCLIPCREATOR_H

#include <QThread>

class ShortClipCreator : public QThread
{
    Q_OBJECT
public:
    ShortClipCreator(QObject* parent = NULL);
};

#endif // SHORTCLIPCREATOR_H
