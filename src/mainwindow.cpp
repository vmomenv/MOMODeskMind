#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include"petai.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 创建宠物AI对象
    petAI = new PetAI(this);
    ui->petDisplay->setText(petAI->getPetGreeting());

    // 初始化天气API
    weatherAPI = new WeatherAPI(this);
    ui->weatherLabel->setText(weatherAPI->getCurrentWeather());

    // 初始化日程提醒
    reminder = new Reminder(this);
    ui->reminderList->setModel(reminder->getReminders());

    connect(ui->askButton, &QPushButton::clicked, this, &MainWindow::onAskButtonClicked);
}

void MainWindow::onAskButtonClicked(){
    QString userQuestion = ui->questionInput->text();
    QString response = petAI->getResponse(userQuestion);
    ui->answerLabel->setText(response);
}
MainWindow::~MainWindow()
{
    delete ui;
}

