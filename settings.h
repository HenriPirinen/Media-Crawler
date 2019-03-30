#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QListWidgetItem>

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
    void on_buttonAddToInclude_clicked();
    void on_buttonRemoveFromInclude_clicked();

    void on_includeList_itemDoubleClicked(QListWidgetItem *item);

    void on_excludeList_itemDoubleClicked(QListWidgetItem *item);

    void on_buttonAddToExclude_clicked();

    void on_buttonRemoveFromExclude_clicked();

private:
    Ui::Settings *ui;
    QString saveFileTo;
};

#endif // SETTINGS_H
