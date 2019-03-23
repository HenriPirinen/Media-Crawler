#ifndef INDEXER_H
#define INDEXER_H

#include <QObject>

class Indexer : public QObject
{
    Q_OBJECT
public:
    explicit Indexer(QObject *parent = nullptr);

signals:

public slots:
};

#endif // INDEXER_H