#ifndef ITEM_H
#define ITEM_H
#include <QString>

struct Item{
    int id;
    int mediaId;
    int numberOfPages;
    int uploadDate;
    QString title;
    QString language;
    Item():id(0),mediaId(0),numberOfPages(0),uploadDate(0),title(""),language(""){}
    Item(int id, int mediaId, int numberOfPages, int uploadDate, QString title, QString language)
        :id(id),mediaId(mediaId),numberOfPages(numberOfPages),uploadDate(uploadDate),title(title),language(language){}
};

#endif // ITEM_H
