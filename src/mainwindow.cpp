#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include"petai.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    petAI = new PetAI(this);
    ui->petDisplay->setText(petAI->getPetGreeting());

    weatherAPI = new WeatherAPI(this);
    ui->weatherLabel->setText(weatherAPI->getCurrentWeather());


}

MainWindow::~MainWindow()
{
    delete ui;
}

