#ifndef CONTOLLER_H
#define CONTOLLER_H

#include <QObject>

class Contoller : public QObject
{
    Q_OBJECT
public:
    explicit Contoller(QObject *parent = nullptr);

signals:

};

#endif // CONTOLLER_H
