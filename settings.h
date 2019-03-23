#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>

namespace Ui {
class Settings;
}

class Settings : public QDialog
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = nullptr);
    ~Settings();

private slots:
    void on_applySettings_clicked();

    void on_okSettings_clicked();

    void on_cancelSettings_clicked();

    void on_toolButton_clicked();
private:
    Ui::Settings *ui;
    QString saveFileTo;
};

#endif // SETTINGS_H
