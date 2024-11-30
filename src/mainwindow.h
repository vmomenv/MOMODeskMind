#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "petai.h"
#include "weatherapi.h"
#include "reminder.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    PetAI *petAI;
    WeatherAPI *weatherAPI;
    Reminder *reminder;

private slots:
    void onAskButtonClicked();
};
#endif // MAINWINDOW_H
