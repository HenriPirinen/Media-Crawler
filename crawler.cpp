#include "crawler.h"
#include <QNetworkReply>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QVariantMap>
#include <QFile>
#include <QFileDialog>
#include <QBuffer>
#include <QSaveFile>
#include <QSettings>
#include <QDir>
#include <QVector>

Crawler::Crawler(QObject *parent) : QObject(parent){
    netManager = new QNetworkAccessManager(this);
    netReply = nullptr;
    dataBuffer = new QByteArray();
}

Crawler::~Crawler()
{
    delete netReply;
    delete[] dataBuffer;
    delete netManager;
}

void Crawler::start()
{
    QSettings settings("Regni","Settings");
    settings.beginGroup("CrawlerConfig");
    url = settings.value("url",QVariant(0)).toString();
    mediaServer = settings.value("mediaServer",QVariant(0)).toString();
    regex.setPattern(settings.value("regex",QVariant(0)).toString());
    regexSplitStart = settings.value("trimStart",QVariant(0)).toInt();
    regexSplitEnd = settings.value("trimEnd",QVariant(0)).toInt();
    seqCurrent = settings.value("sequenceStart",QVariant(0)).toInt();
    seqEnd = settings.value("sequenceEnd",QVariant(0)).toInt();
    savePath = settings.value("saveTo",QVariant(0)).toString();
    settings.endGroup();

    getSource(seqCurrent);
}

void Crawler::getSource(int id = NULL)
{
    qDebug() << id;
    if(id > seqEnd){
        emit finished();
        return;
    }
    QNetworkRequest request;
    if(id == NULL) request.setUrl(url);
    if(id != NULL) request.setUrl(QUrl(url.toString() + QString::number(id) + "/"));
    netReply = netManager->get(request);
    connect(netReply,&QIODevice::readyRead,this,[=](){
        dataBuffer->append(netReply->readAll());
    });
    connect(netReply,&QNetworkReply::finished,this,&Crawler::parseResult);
}

void Crawler::parseResult()
{
    if(netReply->error()){
        qDebug() << "Err: " << netReply->errorString();
    } else {
        QRegularExpressionMatch match = regex.match(QString(*dataBuffer));
        dataBuffer->clear();
        if(match.hasMatch()){
            QString rawJson = match.captured(0);
            rawJson = rawJson.mid(regexSplitStart,rawJson.count() - regexSplitEnd);

            QJsonDocument doc = QJsonDocument::fromJson(rawJson.toUtf8());
            QJsonObject obj = doc.toVariant().toJsonObject();
            QVariantMap map = obj.toVariantMap();
            int media_id = map["media_id"].toInt();
            nPages = map["num_pages"].toInt();

            QJsonObject images = map["images"].toJsonObject();
            QVariantMap imagesMap = images.toVariantMap();
            QJsonArray pages = imagesMap["pages"].toJsonArray();

            for (int i = pages.size() - 1; i >= 0; i--) {
                QJsonObject objPage = pages.at(i).toObject();
                QVariantMap pageMap = objPage.toVariantMap();
                pageTypes.push_front(pageMap["t"].toString());
            }

            Crawler::fetchMedia(pageTypes[0],1,media_id);
        }
    }
}

void Crawler::fetchMedia(QString type, int pNumber, int mediaId)
{
    QString fileType = type == "j" ? ".jpg" : ".png";
    QString query = mediaServer + QString::number(mediaId) + "/" + QString::number(pNumber) + fileType;
    QString fileLocation = savePath + "/" + QString::number(seqCurrent) + "/" + QString::number(pNumber) + fileType;
    QNetworkRequest req;
    req.setUrl(query);
    netReply = netManager->get(req);
    connect(netReply,&QNetworkReply::finished,this,[=](){
        if(netReply->error()){
           qDebug() << "fetchMedia err: " << netReply->errorString();
        } else {
            QByteArray res = netReply->readAll();

            QDir dir(QString(savePath + "/" + QString::number(seqCurrent)));
            if(!dir.exists()) QDir().mkdir(QString(savePath + "/" + QString::number(seqCurrent)));

            QFile file(fileLocation);
            file.open(QIODevice::WriteOnly);
            file.write((res));
            file.close();
            pageTypes.pop_front();
            if(pNumber < nPages){
                Crawler::fetchMedia(pageTypes[0],pNumber + 1,mediaId); //Call itself until every media file is retrivied
            } else {
                emit collectionReceived();
                Crawler::getSource(++seqCurrent);
            }
        }
    });
}
