#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <crawler.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
public slots:

private slots:
    void on_actionStart_triggered();
    void on_actionPause_triggered();
    void on_actionFiles_triggered();
    void on_actionCrawling_settings_triggered();
    void on_actionAbout_Qt_triggered();

    void on_actionQuit_triggered();

signals:
    void nextItem(int index);

private:
    Ui::MainWindow *ui;
    QString fileOutputLocation;
    Crawler *crawler;
};

#endif // MAINWINDOW_H
