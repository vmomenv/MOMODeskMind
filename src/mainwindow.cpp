#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "petai.h"
#include <QMessageBox>
#include <QPainter>
#include <QImage>
#include "settings.h"
#include <QSettings>
#include <QFile>
#include <QStyle>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setWindowFlags(Qt::FramelessWindowHint); // 移除边框
    this->setStyleSheet(
        "background-color: #F3F4F6;"
        "border-radius: 8px;"
        );

    // 创建天气API对象
    weatherAPI = new WeatherAPI(this);
    ui->weatherLabel->setStyleSheet(
        "background-color: #ffffff;"
        "border-radius: 8px;"
        );
    loadAvatar();
    // connect(ui->changeAvatarButton, &QPushButton::clicked, this, &MainWindow::onChangeAvatarButtonClicked);

    // 连接天气API的信号到updateWeatherDisplay槽函数
    connect(weatherAPI, &WeatherAPI::weatherDataUpdated, this, &MainWindow::updateWeatherDisplay);
    connect(ui->settingsButton, &QPushButton::clicked, this, &MainWindow::openSettingsDialog);

    // 创建宠物AI对象
    petAI = new PetAI(this);
    ui->dialogueWidget->setStyleSheet(
        "background-color: #ffffff;"
        "border-radius: 8px;"
        );
    ui->answerLabel->setText(petAI->getPetGreeting());

    // 初始化天气API并请求天气数据
    ui->weatherLabel->setText(weatherAPI->getCurrentWeather());


    // 初始化日程提醒
    ui->reminderWidget->setStyleSheet(
        "background-color: #ffffff;"
        "border-radius: 8px;"
        );
    ui->addReminderButton->setStyleSheet("QPushButton {"
                                         "background-color: #000000;"
                                         "color: #FFFFFF;"
                                         "border-radius: 8px;"
                                         "border: none;"
                                         "padding: 0px;}"
                                         );
    ui->addReminderButton->setFixedSize(68,28);
    ui->countLabel->setStyleSheet("QLabel {"
                                  "background-color: #000000;"
                                  "color: #FFFFFF;"
                                  "border-radius: 8px;"
                                  "border: none;"
                                  "padding: 0px;}"
                                  );
    ui->countLabel->setFixedSize(68,28);
    ui->countLabel->setAlignment(Qt::AlignCenter);

}

void MainWindow::onAskButtonClicked(){
    QString userQuestion = ui->questionInput->text();
    QString response = petAI->getResponse(userQuestion);
    ui->answerLabel->setText(response);
}

void MainWindow::updateWeatherDisplay(const QString &location, double tempC, const QString &condition)
{
    // 更新 QLabel 显示天气信息
    QString weatherText = QString("Location: %1\nTemperature: %2 °C\nCondition: %3")
                              .arg(location)
                              .arg(tempC)
                              .arg(condition);
    ui->weatherLabel->setText(weatherText);
}
void MainWindow::onAddReminderButtonClicked()
{

}

void MainWindow::onRemoveReminderButtonClicked()
{

}

void MainWindow::onReminderTriggered(const QString &content)
{
    QMessageBox::information(this, "提醒", content);
}
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        // 获取鼠标按下的位置
        offset = event->globalPos() - frameGeometry().topLeft();
    }
}

// 处理鼠标移动事件
void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        // 根据鼠标的移动更新窗口的位置
        move(event->globalPos() - offset);
    }
}
void MainWindow::loadAvatar()
{
    // 获取配置文件中的头像路径
    QSettings settings("momodesk-mind", "settings");
    QString avatarPath = settings.value("avatarPath").toString();
    qDebug() << "Avatar path from settings:" << avatarPath;

    QPixmap avatar;
    if (!avatarPath.isEmpty() && QFile::exists(avatarPath)) {
        // 如果头像路径有效，加载头像
        avatar.load(avatarPath);
    } else {
        // 如果头像路径无效，则加载默认头像
        qDebug() << "Failed to load avatar. Loading default avatar.";
        avatar.load(":/img/momen.jpg");
    }

    // 将头像裁剪为圆形
    QPixmap circularAvatar = avatar.scaled(128, 128, Qt::KeepAspectRatio); // 调整头像大小
    QBitmap mask(circularAvatar.size());
    mask.fill(Qt::color0);  // 填充为透明
    QPainter painter(&mask);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(Qt::color1);  // 用黑色填充圆形区域
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(0, 0, circularAvatar.width(), circularAvatar.height());
    painter.end();

    // 使用遮罩将头像裁剪成圆形
    circularAvatar.setMask(mask);
    ui->avatarLabel->setPixmap(circularAvatar); // 设置头像
    ui->avatarLabel->move(128, 16);  // 设置头像的位置
}

void MainWindow::openSettingsDialog() {
    // 创建 Settings 对话框并显示
    Settings *settingsDialog = new Settings(this);
    connect(settingsDialog,&Settings::avatarUpdated,this,&MainWindow::loadAvatar);
    settingsDialog->exec();  // 使用 exec() 打开模态对话框
}
MainWindow::~MainWindow()
{
    delete ui;
}
