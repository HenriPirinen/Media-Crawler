#ifndef CRAWLER_H
#define CRAWLER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QRegularExpression>
#include <QNetworkReply>
#include <QStack>
#include <QList>
#include "item.h"

class Crawler : public QObject
{
    Q_OBJECT
public:
    explicit Crawler(QObject *parent = nullptr);
    ~Crawler();
    void start();
    void stop();
    void timeout();
    Item getCurrentItem() const;
private:
    QNetworkAccessManager *netManager;
    QNetworkReply *netReply;
    QByteArray *dataBuffer;
    QRegularExpression regex;
    QUrl url;
    QString mediaServer;
    QString savePath;
    QString itemLanguage;
    QStack<QString> pageTypes;
    QList<QString> include;
    QList<QString> exclude;
    Item itemInfo;
    int regexSplitStart;
    int regexSplitEnd;
    int nPages;
    int seqCurrent;
    int seqEnd;
private:
    void getSource(int id);
    void parseResult();
    void fetchMedia(QString type, int pNumber, int mediaId);
signals:
    void parseCompleted();
    void itemReceived();
    void collectionReceived();
    void finished();

public slots:
};

#endif // CRAWLER_H
