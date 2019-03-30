#include "settings.h"
#include "ui_settings.h"
#include <QList>
#include <QSettings>
#include <QDebug>
#include <QFileDialog>

Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);
    this->setWindowTitle("Crawler settings");

    qRegisterMetaTypeStreamOperators<QList<QString> >("QList<QString>");

    QSettings settings("Regni","Settings");
    settings.beginGroup("CrawlerConfig");
    ui->inputURL->setText(settings.value("url",QVariant(0)).toString());
    ui->mediaServer->setText(settings.value("mediaServer",QVariant(0)).toString());
    ui->inputRegex->setText(settings.value("regex",QVariant(0)).toString());
    ui->inputTrimStart->setValue(settings.value("trimStart",QVariant(0)).toInt());
    ui->inputTrimEnd->setValue(settings.value("trimEnd",QVariant(0)).toInt());
    ui->inputStartIndex->setValue(settings.value("sequenceStart",QVariant(0)).toInt());
    ui->inputEndIndex->setValue(settings.value("sequenceEnd",QVariant(0)).toInt());
    ui->inputSaveTo->setText(settings.value("saveTo",QVariant(0)).toString());
    ui->includeList->addItems(settings.value("include").value<QList<QString>>());
    ui->excludeList->addItems(settings.value("exclude").value<QList<QString>>());
    settings.endGroup();
}

Settings::~Settings()
{
    delete ui;
}

void Settings::on_applySettings_clicked()
{
    QList<QString> includeTags;
    QList<QString> excludeTags;

    for (int i = 0; i < ui->includeList->count(); i++) {
        includeTags.push_back(ui->includeList->item(i)->text());
    }
    for (int i = 0; i < ui->excludeList->count(); i++) {
        excludeTags.push_back(ui->excludeList->item(i)->text());
    }

    QSettings settings("Regni","Settings");
    settings.beginGroup("CrawlerConfig");
    settings.setValue("url",ui->inputURL->text());
    settings.setValue("mediaServer",ui->mediaServer->text());
    settings.setValue("regex",ui->inputRegex->text());
    settings.setValue("trimStart",ui->inputTrimStart->value());
    settings.setValue("trimEnd",ui->inputTrimEnd->value());
    settings.setValue("sequenceStart",ui->inputStartIndex->value());
    settings.setValue("sequenceEnd",ui->inputEndIndex->value());
    settings.setValue("saveTo",ui->inputSaveTo->text());
    settings.setValue("include",QVariant::fromValue(includeTags));
    settings.setValue("exclude",QVariant::fromValue(excludeTags));
    settings.endGroup();
}

void Settings::on_okSettings_clicked()
{
    accept();
}

void Settings::on_cancelSettings_clicked()
{
    reject();
}

void Settings::on_toolButton_clicked()
{
    ui->inputSaveTo->setText(QFileDialog::getExistingDirectory(this,"Save media to",QString(),QFileDialog::ShowDirsOnly));
}

void Settings::on_buttonAddToInclude_clicked()
{
    ui->includeList->addItem("Edit tag");
}

void Settings::on_buttonRemoveFromInclude_clicked()
{
    qDeleteAll(ui->includeList->selectedItems());
}

void Settings::on_includeList_itemDoubleClicked(QListWidgetItem *item)
{
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    ui->includeList->editItem(item);
}

void Settings::on_excludeList_itemDoubleClicked(QListWidgetItem *item)
{
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    ui->excludeList->editItem(item);
}

void Settings::on_buttonAddToExclude_clicked()
{
    ui->excludeList->addItem("Edit tag");
}

void Settings::on_buttonRemoveFromExclude_clicked()
{
    qDeleteAll(ui->excludeList->selectedItems());
}
