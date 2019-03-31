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
#include <QTimer>

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
    qRegisterMetaTypeStreamOperators<QList<QString> >("QList<QString>");

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
    itemLanguage = settings.value("language",QVariant(0)).toString();
    include = settings.value("include").value<QList<QString>>();
    exclude = settings.value("exclude").value<QList<QString>>();
    settings.endGroup();

    getSource(seqCurrent);
}

void Crawler::getSource(int id = NULL)
{
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
        qDebug() << "Err: " << netReply->errorString(); //Todo: Handle error
    } else {
        QRegularExpressionMatch match = regex.match(QString(*dataBuffer));
        dataBuffer->clear();
        if(match.hasMatch()){
            QString rawJson = match.captured(0);
            rawJson = rawJson.mid(regexSplitStart,rawJson.count() - regexSplitEnd);

            QJsonDocument doc = QJsonDocument::fromJson(rawJson.toUtf8());
            QJsonObject obj = doc.toVariant().toJsonObject();
            QVariantMap map = obj.toVariantMap();

            int mediaId = map["media_id"].toInt();
            int uploadDate = map["upload_date"].toInt();
            nPages = map["num_pages"].toInt();

            QJsonObject titles = map["title"].toJsonObject();
            QVariantMap titlesMap = titles.toVariantMap();
            QString titlePretty = titlesMap["pretty"].toString();

            QJsonObject images = map["images"].toJsonObject();
            QVariantMap imagesMap = images.toVariantMap();
            QJsonArray pages = imagesMap["pages"].toJsonArray();
            for (int i = pages.size() - 1; i >= 0; i--) {
                QJsonObject objPage = pages.at(i).toObject();
                QVariantMap pageMap = objPage.toVariantMap();
                pageTypes.push(pageMap["t"].toString());
            }

            int tagMatch = 0;
            QJsonArray tags = map["tags"].toJsonArray();
            for (int i = 0; i < tags.size(); i++) {
                QJsonObject objTag = tags.at(i).toObject();
                QVariantMap tagMap = objTag.toVariantMap();

                for (int eTag = 0; eTag < exclude.size(); eTag++) { //Exclude tags have higher priority than include tags.
                    if(tagMap["name"].toString() == include.at(eTag)) goto ignoreItem; //Found exclude tag! Ignore this item
                }
                for (int iTag = 0; iTag < include.size(); iTag++) {
                    if(tagMap["name"].toString() == include.at(iTag)) tagMatch++;
                }

                if(tagMap["type"].toString() == "language" && tagMap["name"].toString() != "translated"){
                    if(itemLanguage != "" && tagMap["name"].toString() != itemLanguage) goto ignoreItem; //Language does not match! Ignore this item
                    itemInfo.language = tagMap["name"].toString();
                    if(include.size() == 0 && exclude.size() == 0) break;
                }
            }

            if(tagMatch == include.size()){
                itemInfo.id = seqCurrent;
                itemInfo.mediaId = mediaId;
                itemInfo.numberOfPages = nPages;
                itemInfo.uploadDate = uploadDate;
                itemInfo.title = titlePretty;

                emit parseCompleted();
                Crawler::fetchMedia(pageTypes.top(),1,mediaId);
            } else {
                ignoreItem:
                emit collectionReceived();
                QTimer::singleShot(5000,this,[=](){
                    Crawler::getSource(++seqCurrent);
                });
            }
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
           qDebug() << "fetchMedia err: " << netReply->errorString(); //Todo: handle error
        } else {
            QByteArray res = netReply->readAll();

            QDir dir(QString(savePath + "/" + QString::number(seqCurrent)));
            if(!dir.exists()) QDir().mkdir(QString(savePath + "/" + QString::number(seqCurrent)));

            QFile file(fileLocation);
            file.open(QIODevice::WriteOnly);
            file.write((res));
            file.close();
            pageTypes.pop();
            if(pNumber < nPages){
                Crawler::fetchMedia(pageTypes.top(),pNumber + 1,mediaId); //Call itself until every media file is retrivied
            } else {
                emit collectionReceived();
                Crawler::getSource(++seqCurrent);
            }
        }
    });
}

Item Crawler::getCurrentItem() const
{
    return itemInfo;
}
