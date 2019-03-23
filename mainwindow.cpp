#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settings.h"
#include <QFileDialog>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    crawler = new Crawler();
    connect(crawler,&Crawler::collectionReceived,this,[](){ qDebug() << "Collection received"; });
    connect(crawler,&Crawler::finished,this,[](){ qDebug() << "Crawling finished";});
}

MainWindow::~MainWindow()
{
    delete crawler;
    delete ui;
}

void MainWindow::on_actionStart_triggered()
{
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
}
