#include "settings.h"
#include "ui_settings.h"
#include <QSettings>
#include <QDebug>
#include <QFileDialog>

Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);
    this->setWindowTitle("Crawler settings");

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
    settings.endGroup();
}

Settings::~Settings()
{
    delete ui;
}

void Settings::on_applySettings_clicked()
{
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
