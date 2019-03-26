#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settings.h"
#include "item.h"
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include <QSettings>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Media Crawler");

    currentCollectionIdx = 0;
    crawler = new Crawler();
    connect(crawler,&Crawler::parseCompleted,this,[=](){
        Item curretItem = crawler->getCurrentItem();
        qDebug() << curretItem.language;
    });
    connect(crawler,&Crawler::collectionReceived,this,[=](){
        currentCollectionIdx++;
        ui->progressBar->setValue(currentCollectionIdx * 100 / (endIdx - startIdx));
    });
    connect(crawler,&Crawler::finished,this,[=](){
        ui->progressBar->setValue(100);
    });
}

MainWindow::~MainWindow()
{
    delete crawler;
    delete ui;
}

void MainWindow::on_actionStart_triggered()
{
    QSettings settings("Regni","Settings");
    settings.beginGroup("CrawlerConfig");
    startIdx = settings.value("sequenceStart",QVariant(0)).toInt();
    endIdx = settings.value("sequenceEnd",QVariant(0)).toInt();
    currentCollectionIdx = 0;
    ui->progressBar->setValue(0);
    crawler->start();
}

void MainWindow::on_actionPause_triggered()
{

}

void MainWindow::on_actionFiles_triggered()
{
    fileOutputLocation = QFileDialog::getSaveFileName(this,"Output files to",QString(),"Images (*.png)");
}

void MainWindow::on_actionCrawling_settings_triggered()
{
    Settings *settingsDialog = new Settings(this);
    settingsDialog->exec();
    delete settingsDialog;
}

void MainWindow::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(this,"About Qt");
}

void MainWindow::on_actionQuit_triggered()
{
    QApplication::quit();
}
