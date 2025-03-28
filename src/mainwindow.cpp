#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "petai.h"
#include <QMessageBox>
#include <QPainter>
#include <QImage>
#include <QSettings>
#include <QFile>
#include <QStyle>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFormLayout>
#include <QDateTimeEdit>
#include <QPropertyAnimation>
#include <QStandardPaths>
#include <QMenu>
#include <QLocalSocket>
#include <QLocalServer>
#include <QDialogButtonBox>
#include <QBitmap>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    singleAppCheck();
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setWindowFlags(Qt::FramelessWindowHint); // 移除边框
    this->setStyleSheet(
        "background-color: #F3F4F6;"
        "border-radius: 8px;"
        );
    QApplication::setQuitOnLastWindowClosed(false);

    showTrayIcon();
    // 将窗口移动到右下角
    moveToBottomRight();
    //设置窗口贴边吸附
    setMouseTracking(true);
    //初始化设置
    settings = new Settings(this);
    settings->checkAndCopySettings();
    installEventFilter(this);
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
    ui->answerTextEdit->setText(petAI->getPetGreeting());

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



    // 设置日程提醒滚动区域
    ui->reminderScrollArea->setFixedSize(328, 144);
    ui->reminderScrollArea->setWidgetResizable(false); // 必须关闭
    ui->reminderScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->reminderScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // 创建内容容器
    reminderWidget = new QWidget();
    reminderWidget->setMinimumWidth(328); // 关键：固定内容宽度
    reminderWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    // 设置垂直布局
    reminderWidgetLayout = new QVBoxLayout(reminderWidget);
    reminderWidgetLayout->setAlignment(Qt::AlignTop);
    reminderWidgetLayout->setContentsMargins(0, 0, 10, 0);
    reminderWidgetLayout->setSpacing(4);

    ui->reminderScrollArea->setStyleSheet(R"(
    QScrollArea { border: none; }
    QScrollBar:vertical {
        width: 6px;
        background: #F3F4F6;
    }
    QScrollBar::handle:vertical {
        background: #D1D5DB;
        border-radius: 3px;
    }
)");
    ui->answerTextEdit->setStyleSheet(R"(
    QScrollArea { border: none; }
    QScrollBar:vertical {
        width: 6px;
        background: #F3F4F6;
    }
    QScrollBar::handle:vertical {
        background: #D1D5DB;
        border-radius: 3px;
    }
)");
    // 必须显式设置widget
    ui->reminderScrollArea->setWidget(reminderWidget);


    reminderLoadJsonData("reminderdata.json");


    //初始化ai模块

    ui->modelComboBox->setStyleSheet(
        "QComboBox {"
        "   border-radius: 8px;"              // 设置圆角
        "   background-color: #F3F4F6;"       // 设置底色
        "   padding: 5px;"                    // 设置内边距
        "   min-width: 100px;"                 // 设置最小宽度
        "}"
        "QComboBox::down-arrow {"
        "   image: url(:/img/down_arrow.svg);" // 使用自定义箭头图标
        "   width: 16px;"                       // 设置箭头宽度
        "   height: 16px;"                      // 设置箭头高度
        "   border: none;"                     // 去掉按钮边框
        "   background: none;"                 // 去掉按钮背景
        "}"
        "QComboBox::drop-down {"
        "   border: none;"                     // 去掉下拉框的边框
        "   background: none;"                 // 去掉下拉框的背景
        "   padding: 0px;"                     // 去掉内边距
        "}"
        );

    aiClient = new AIClient(this); // 修改初始化
    qDebug()<<aiClient->initializeConnection();//初始化连接
    QMap<QString, QString> languageModelSettings = settings->getLanguageModelSettings();
    QString ollamaAddress = languageModelSettings["OLLAMA_ADDRESS"];
    qDebug() << "OLLAMA_ADDRESS:" << ollamaAddress;
    aiClient->setServerUrl(ollamaAddress);
    // 连接信号
    connect(aiClient, &AIClient::modelsReceived, [](const QStringList &models){
        qDebug() << "Available models:" << models;
    });

    connect(aiClient, &AIClient::responseReceived, [](const QString &response){
        qDebug() << "Received chunk:" << response;
    });

    // 获取模型列表
    // MainWindow 构造函数中
    connect(aiClient, &AIClient::modelsReceived, this, &MainWindow::handleModelsReceived);
    connect(aiClient, &AIClient::responseReceived, this, &MainWindow::handleResponseReceived);
    connect(aiClient, &AIClient::errorOccurred, this, &MainWindow::handleErrorOccurred);
    connect(aiClient, &AIClient::responseComplete, this, &MainWindow::handleResponseComplete);


    // 初始化UI状态
    ui->sendButton->setEnabled(false); // 等待模型加载完成
    ui->answerTextEdit->setReadOnly(true); // 设置输出框只读
    ui->inputEdit->setPlaceholderText("和我聊聊天吧...");
    // 连接发送按钮和回车键
    connect(ui->inputEdit, &QLineEdit::returnPressed,
            this, [=]{
        setExpandDialogueWidget();
        sendRequest();  // 原有发送逻辑
    });

    aiClient->listModels();
    ui->pinnedButton->setIcon(QIcon(":/img/pin.svg"));
    ui->pinnedButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #000000;"
        "}"
        "QPushButton:hover {"
        "   background-color: #CDD1D9;"
        "}"
        );
    ui->settingsButton->setIcon(QIcon(":/img/settings.svg"));
    ui->settingsButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #000000;"
        "}"
        "QPushButton:hover {"
        "   background-color: #CDD1D9;"
        "}"
        );
    //初始化对话按钮图片
    ui->clearButton->setIcon(QIcon(":/img/Trash_black.svg"));
    ui->clearButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #F3F4F6;"
        "   border: 1px solid #FFFFFF;"
        "   border-radius: 4px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #CDD1D9;"
        "}"
        );
    ui->explainCodeButton->setIcon(QIcon(":/img/Code.svg"));
    ui->explainCodeButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #F3F4F6;"
        "   border: 1px solid #FFFFFF;"
        "   border-radius: 4px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #CDD1D9;"
        "}"
        );
    ui->networkSearchButton->setIcon(QIcon(":/img/NetWork.svg"));
    ui->networkSearchButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #F3F4F6;"
        "   border: 1px solid #FFFFFF;"
        "   border-radius: 4px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #CDD1D9;"
        "}"
        );
    ui->pasteButton->setIcon(QIcon(":/img/ClipBoard.svg"));
    ui->pasteButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #F3F4F6;"
        "   border: 1px solid #FFFFFF;"
        "   border-radius: 4px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #CDD1D9;"
        "}"
        );
    ui->translateButton->setIcon(QIcon(":/img/Translate.svg"));
    ui->translateButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #F3F4F6;"
        "   border: 1px solid #FFFFFF;"
        "   border-radius: 4px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #CDD1D9;"
        "}"
        );
    ui->sendButton->setIcon(QIcon(":/img/Send.svg"));
    ui->sendButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #000000;"
        "   border: 1px solid #FFFFFF;"
        "   border-radius: 4px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #CDD1D9;"
        "}"
        );
    ui->cancelButton->setIcon(QIcon(":/img/cancel.svg"));
    ui->cancelButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #F3F4F6;"
        "   border: 1px solid #FFFFFF;"
        "   border-radius: 4px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #FEE2E2;"
        "}"
        );
    m_originalSize = ui->dialogueWidget->size(); // 保存初始尺寸
    ui->dialogueWidget->installEventFilter(this);

    // 初始化剪切板对象
    clipboard = QApplication::clipboard();
    // 初始不激活监控
    isPasteMonitoring = false;

    // 连接中断按钮点击信号
    connect(ui->cancelButton, &QPushButton::clicked, aiClient, &AIClient::cancelRequest);
    // 处理中断信号
    connect(aiClient, &AIClient::responseInterrupted, this, [=](){
        qDebug()<<"中断";
    });

}


void MainWindow::onSettingsUpdated() {
    qDebug() << "更新设置";
    ui->weatherLabel->setText(weatherAPI->getCurrentWeather());
}


void MainWindow::reminderLoadJsonData(const QString &filePath){
    QFile file(filePath);
    if(!file.open(QIODevice::ReadOnly)){
        qWarning() << "Failed to open file!";
        return;
    }
    // 读取文件内容并解析JSON
    QByteArray jsonData = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    if (doc.isNull()) {
        qWarning() << "Invalid JSON data!";
        return;
    }

    // 获取JSON数组并展示每一条消息
    QJsonArray jsonArray = doc.array();
    for (const QJsonValue &value : jsonArray) {
        if (value.isObject()) {
            QJsonObject jsonObj = value.toObject();
            QString message = jsonObj["message"].toString();  // 假设每条信息的键为 "message"
            QString time = jsonObj["time"].toString();  // 假设每条信息的键为 "message"
            QString priority = jsonObj["priority"].toString();  // 紧急程度（urgent、high、non-urgent）
            displayMessage(message,time, priority);
            qDebug()<<message<<time<<priority;
        }
    }
    updateReminderCount();
}
void MainWindow::showTrayIcon()
{
    // 创建托盘图标
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/img/momen.jpg")); // 设置图标路径

    // 创建托盘菜单
    trayMenu = new QMenu(this);

    // 添加“恢复”菜单项
    QAction *restoreAction = new QAction("恢复", this);
    connect(restoreAction, &QAction::triggered, this, &MainWindow::onRestoreAction);

    // 添加“退出”菜单项
    QAction *quitAction = new QAction("退出", this);
    connect(quitAction, &QAction::triggered, this, &MainWindow::onQuitAction);

    // 将菜单项添加到托盘菜单
    trayMenu->addAction(restoreAction);
    trayMenu->addAction(quitAction);

    // 设置托盘图标的上下文菜单
    trayIcon->setContextMenu(trayMenu);
    connect(trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::onTrayIconActivated);
    // 显示托盘图标
    trayIcon->show();
}
void MainWindow::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger) {
        // 左键单击时执行恢复操作
        onRestoreAction();
    }
}
void MainWindow::onRestoreAction()
{
    show(); // 显示主窗口
}

// 退出应用的槽函数
void MainWindow::onQuitAction()
{
    qApp->quit(); // 退出应用程序
}

void MainWindow::singleAppCheck()
{
    const QString SERVER_NAME = "momodeskmind";
    QLocalSocket socket;
    qDebug()<<"jiance";
    socket.connectToServer(SERVER_NAME);
    if (socket.waitForConnected(500)) {
        qDebug()<<"出现重复进程";
        QCoreApplication::quit();
    }
    QLocalServer *server = new QLocalServer();
    if (!server->listen(SERVER_NAME)) {
#ifdef Q_OS_UNIX
        // Linux 可能会遗留 socket 文件，需手动删除
        QFile::remove("/tmp/" + SERVER_NAME);
        server->listen(SERVER_NAME);
#endif
    }
}
void MainWindow::displayMessage(const QString &message, const QString &time,const QString &priority)
{
    MessageWidget *widget = new MessageWidget(message, time,priority);
    widget->setFixedHeight(30); // 确保每个条目固定高度
    reminderWidgetLayout->addWidget(widget);

    // 动态调整内容区域高度
    int contentHeight = reminderWidgetLayout->count() * 34; // 30+4=34
    reminderWidget->setMinimumHeight(contentHeight);
    connect(widget, &MessageWidget::deleteClicked, this, [=]{
        // 从布局中移除控件
        reminderWidgetLayout->removeWidget(widget);
        // 安全删除对象
        widget->deleteLater();
        // 重新计算内容高度
        int contentHeight = reminderWidgetLayout->count() * 34;
        reminderWidget->setMinimumHeight(contentHeight);
        // 强制刷新布局
        reminderWidget->adjustSize();
        ui->reminderScrollArea->updateGeometry();
        removeReminderFromJson(widget->getMessage(), widget->getTime(), widget->getPriority());
        updateReminderCount();
        adjustScrollContent();
    });
}
void MainWindow::removeReminderFromJson(const QString &message, const QString &time, const QString &priority)
{
    QFile file("reminderdata.json");
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        qWarning() << "Failed to open file for deletion!";
        return;
    }

    // 读取现有数据
    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonArray jsonArray = doc.isArray() ? doc.array() : QJsonArray();

    // 查找并删除匹配项
    for(int i = 0; i < jsonArray.size();) {
        QJsonObject obj = jsonArray[i].toObject();
        if(obj["message"] == message &&
            obj["time"] == time &&
            obj["priority"] == priority) {
            jsonArray.removeAt(i);
        } else {
            ++i;
        }
    }

    // 写回文件
    file.resize(0);
    file.write(QJsonDocument(jsonArray).toJson());
    file.close();
}
void MainWindow::updateReminderCount()
{
    int count = reminderWidgetLayout->count();
    ui->countLabel->setText(QString("%1项待办").arg(count));
}
void MainWindow::adjustScrollContent()
{
    int contentHeight = reminderWidgetLayout->count() * 34;
    reminderWidget->setMinimumHeight(contentHeight);
    reminderWidget->adjustSize();
}
void MainWindow::updateWeatherDisplay(const QString &location, double tempC, const QString &condition)
{
    // 使用HTML标签设置不同部分的文本样式
    QString weatherText = QString("<div style='font-size:14pt;'>%1</div>" // 城市，14号字体
                                  "<div><span style='font-size:15pt; font-weight:bold;'>%2℃</span>&nbsp;&nbsp;" // 温度，20号加粗，后跟两个空格
                                  "<span style='font-size:14pt;'>%3</span></div>") // 气象，14号字体
                              .arg(location)
                              .arg(tempC)
                              .arg(condition);

    // 设置QLabel的文本并启用HTML格式
    ui->weatherLabel->setText(weatherText);
    ui->weatherLabel->setTextFormat(Qt::RichText);
}
void MainWindow::on_addReminderButton_clicked()
{
    // 创建对话框
    QDialog dialog(this);
    dialog.setWindowTitle("添加新提醒");
    dialog.setFixedSize(300, 200);

    // 表单控件
    QComboBox priorityCombo;
    QDateTimeEdit timeEdit;
    QLineEdit messageEdit;

    // 初始化控件
    priorityCombo.addItems({"紧急 (urgent)", "高 (high)", "普通 (normal)"});
    timeEdit.setDateTime(QDateTime::currentDateTime());
    timeEdit.setDisplayFormat("yyyy-MM-dd HH:mm");
    messageEdit.setPlaceholderText("输入提醒内容...");

    // 表单布局
    QFormLayout *formLayout = new QFormLayout();
    formLayout->addRow("优先级:", &priorityCombo);
    formLayout->addRow("时间:", &timeEdit);
    formLayout->addRow("内容:", &messageEdit);

    // 按钮组
    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    // 主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(buttons);

    if (dialog.exec() == QDialog::Accepted) {
        // 解析输入数据
        QString priority;
        switch (priorityCombo.currentIndex()) {
        case 0: priority = "urgent"; break;
        case 1: priority = "high"; break;
        default: priority = "normal";
        }

        QString time = timeEdit.dateTime().toString("hh:mm");
        QString message = messageEdit.text().trimmed();

        // 验证输入
        if (message.isEmpty()) {
            QMessageBox::warning(this, "错误", "提醒内容不能为空");
            return;
        }

        // 添加提醒到界面
        displayMessage(message, time, priority);

        // 保存到JSON文件
        saveReminderToJson(message, time, priority);

        // 更新待办计数
        ui->countLabel->setText(QString("%1项待办").arg(reminderWidgetLayout->count()));
    }
}

void MainWindow::saveReminderToJson(const QString &message, const QString &time, const QString &priority)
{
    QFile file("reminderdata.json");
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        qWarning() << "Failed to open file for saving!";
        return;
    }

    // 读取现有数据
    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonArray jsonArray = doc.isArray() ? doc.array() : QJsonArray();

    // 创建新条目
    QJsonObject newEntry;
    newEntry["message"] = message;
    newEntry["time"] = time;
    newEntry["priority"] = priority;
    jsonArray.append(newEntry);

    // 写回文件
    file.resize(0);
    file.write(QJsonDocument(jsonArray).toJson());
    file.close();
}


void MainWindow::onReminderTriggered(const QString &content)
{
    QMessageBox::information(this, "提醒", content);
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

void MainWindow::handleModelsReceived(const QStringList &models)
{
    // 更新模型下拉框
    ui->modelComboBox->clear();
    ui->modelComboBox->addItems(models);

    // 启用发送按钮（如果有可用模型）
    ui->sendButton->setEnabled(!models.isEmpty());

    if(models.isEmpty()) {
        QMessageBox::warning(this, "模型加载失败", "未找到可用模型，请检查Ollama服务");
    }
}

void MainWindow::handleResponseReceived(const QString &response)
{
    // 追加响应内容并自动滚动
    QTextCursor cursor = ui->answerTextEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    cursor.insertText(response);

    // 自动滚动到底部
    QScrollBar *answerScrollbar = ui->answerTextEdit->verticalScrollBar();
    answerScrollbar->setValue(answerScrollbar->maximum());
}

void MainWindow::handleErrorOccurred(const QString &error)
{
    // 显示错误信息
    QMessageBox::critical(this, "请求错误", error);

    // 在输出框追加错误信息
    ui->answerTextEdit->append("\n[系统错误] " + error);
}

void MainWindow::handleResponseComplete()
{
    // 完成响应后添加换行
    ui->answerTextEdit->append("\n");

    // 恢复UI状态
    ui->sendButton->setEnabled(true);
    ui->answerTextEdit->setFocus();
}

void MainWindow::sendRequest()
{
    // 获取输入内容
    const QString input = ui->inputEdit->text().trimmed();
    if(input.isEmpty()) {
        QMessageBox::warning(this, "输入为空", "请输入对话内容");
        return;
    }

    // 获取当前模型
    const QString model = ui->modelComboBox->currentText();
    if(model.isEmpty()) {
        QMessageBox::warning(this, "模型未选择", "请先选择对话模型");
        return;
    }

    // 更新UI状态
    // ui->sendButton->setEnabled(false);
    ui->inputEdit->clear();

    // 在输出框显示用户输入
    ui->answerTextEdit->append("You: " + input + "\nAI: ");

    // 发送请求
    aiClient->generateResponse(model, input);
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if(watched ==ui->dialogueWidget && event->type() ==QEvent::MouseButtonPress){
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);

        // 检测是否点击在空白区域
        if (!ui->dialogueWidget->childAt(mouseEvent->pos()))
        {
            setExpandDialogueWidget(); // 触发展开函数
            return true; // 拦截事件
        }
    }
    if (Q_NULLPTR == watched)
    {
        return false;
    }
    if (QEvent::ActivationChange == event->type())
    {
        qDebug()<<"活动窗口变化";
        if (QApplication::activeWindow() != this)
        {
            if(isTopping==false){
                this->hide();
            }
        }
    }
    return QMainWindow::eventFilter(watched, event);
}



void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        // 获取鼠标按下的位置
        offset = event->globalPos() - frameGeometry().topLeft();
    }
    setCollapseDialogueWidget();
}
// 处理鼠标移动事件
void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        // checkSnapToEdge();
        // 根据鼠标的移动更新窗口的位置
        move(event->globalPos() - offset);
    }
}


void MainWindow::on_sendButton_clicked()
{
    setExpandDialogueWidget();
    sendRequest();  // 原有发送逻辑

}
void MainWindow::setExpandDialogueWidget(){
    ui->dialogueWidget->setFixedSize(352, 748);
    m_isExpanded = !m_isExpanded;  // 切换状态
    ui->dialogueWidget->move(16,16);
    ui->answerTextEdit->setFixedSize(331,549);
    ui->sendWidget->move(0,632);


    // 添加动画效果（可选）
    QPropertyAnimation *animation = new QPropertyAnimation(ui->dialogueWidget, "size");
    animation->setDuration(300);
    animation->setStartValue(ui->dialogueWidget->size());
    animation->setEndValue(m_isExpanded ? QSize(384, 750) : m_originalSize);
    animation->start(QAbstractAnimation::DeleteWhenStopped);

}

void MainWindow::setCollapseDialogueWidget()
{
    ui->dialogueWidget->setFixedSize(352, 256);
    m_isExpanded = !m_isExpanded;  // 切换状态
    ui->dialogueWidget->move(16,508);
    ui->answerTextEdit->setFixedSize(328,56);
    ui->sendWidget->move(0,140);
}

void MainWindow::moveToBottomRight()
{
    QScreen* screen=QGuiApplication::primaryScreen();
    QRect availableGeometry = screen->availableGeometry();
    // 获取窗口的大小
    QSize windowSize = size();

    // 计算窗口右下角的坐标
    int x = availableGeometry.right() - windowSize.width();
    int y = availableGeometry.bottom() - windowSize.height();

    // 移动窗口到右下角
    move(x, y);
}

void MainWindow::checkSnapToEdge()
{
    // 获取当前屏幕的可用几何区域（除去任务栏）
    QScreen* screen = QGuiApplication::primaryScreen();
    QRect availableGeometry = screen->availableGeometry();

    // 获取窗口的几何区域
    QRect windowGeometry = geometry();

    // 吸附距离阈值（像素）
    const int snapDistance = 20;

    // 检查是否靠近左边缘
    if (qAbs(windowGeometry.left() - availableGeometry.left()) < snapDistance) {
        move(availableGeometry.left(), windowGeometry.top());
    }
    // 检查是否靠近右边缘
    else if (qAbs(windowGeometry.right() - availableGeometry.right()) < snapDistance) {
        move(availableGeometry.right() - windowGeometry.width(), windowGeometry.top());
    }
    // 检查是否靠近上边缘
    if (qAbs(windowGeometry.top() - availableGeometry.top()) < snapDistance) {
        move(windowGeometry.left(), availableGeometry.top());
    }
    // 检查是否靠近下边缘
    else if (qAbs(windowGeometry.bottom() - availableGeometry.bottom()) < snapDistance) {
        move(windowGeometry.left(), availableGeometry.bottom() - windowGeometry.height());
    }
}


void MainWindow::on_clearButton_clicked()
{
    ui->answerTextEdit->clear();
}


void MainWindow::on_networkSearchButton_clicked()
{

}


void MainWindow::on_pasteButton_clicked()
{
    if(!isPasteMonitoring){
        connect(clipboard,&QClipboard::dataChanged,this,&MainWindow::handleClipboardChange,Qt::UniqueConnection);//Qt::UniqueConnection防止重复连接的场景
        // 立即获取当前剪切板内容
        isPasteMonitoring = true;
        ui->pasteButton->setStyleSheet(
            "QPushButton {"
            "   background-color: #CDD1D9;"
            "   border: 1px solid #FFFFFF;"
            "   border-radius: 4px;"
            "}"
            "QPushButton:hover {"
            "   background-color: #F3F4F6;"
            "}"
            );
        qDebug() << "Clipboard monitoring started";
    }else{
        // 断开监控（可选）
        disconnect(clipboard, &QClipboard::dataChanged,
                   this, &MainWindow::handleClipboardChange);

        isPasteMonitoring = false;
        ui->pasteButton->setStyleSheet(
            "QPushButton {"
            "   background-color: #F3F4F6;"
            "   border: 1px solid #FFFFFF;"
            "   border-radius: 4px;"
            "}"
            "QPushButton:hover {"
            "   background-color: #CDD1D9;"
            "}"
            );
        qDebug() << "Clipboard monitoring stopped";
    }

}


void MainWindow::on_explainCodeButton_clicked()
{
    ui->inputEdit->setText("请阅读下面的代码并用中文进行解释。首先，识别这段代码使用的编程语言，然后提供代码的功能描述。解释代码的结构、重要函数、变量，以及任何可能需要注意的部分。如果有必要，提供简单的示例或改进建议。");

    if(!isexplainCodeMonitoring){
        if(isTranslateMonitoring){ //如果翻译按钮开启则关闭
            on_translateButton_clicked();
        }

        isexplainCodeMonitoring = true;
        ui->explainCodeButton->setStyleSheet(
            "QPushButton {"
            "   background-color: #CDD1D9;"
            "   border: 1px solid #FFFFFF;"
            "   border-radius: 4px;"
            "}"
            "QPushButton:hover {"
            "   background-color: #F3F4F6;"
            "}"
            );

    }else{


        isexplainCodeMonitoring = false;
        ui->explainCodeButton->setStyleSheet(
            "QPushButton {"
            "   background-color: #F3F4F6;"
            "   border: 1px solid #FFFFFF;"
            "   border-radius: 4px;"
            "}"
            "QPushButton:hover {"
            "   background-color: #CDD1D9;"
            "}"
            );

    }
}


void MainWindow::on_translateButton_clicked()
{
    ui->inputEdit->setText("I want you to act as an Chinese translator, spelling corrector and improver. I will speak to you in any language and you will detect the language, translate it and answer in the corrected and improved version of my text, in Chinese. I want you to replace my simplified A0-level words and buences with more beautiful and elegant, upper level Chinese words and sentences. Keep the meaning same, but make them more literary. I want you to only reply the correction, the improvements and nothing else, do not write explanations. My first sentence is :");
    if(isexplainCodeMonitoring){ //如果翻译按钮开启则关闭
        on_explainCodeButton_clicked();
    }

    if(!isTranslateMonitoring){
        isTranslateMonitoring = true;
        ui->translateButton->setStyleSheet(
            "QPushButton {"
            "   background-color: #CDD1D9;"
            "   border: 1px solid #FFFFFF;"
            "   border-radius: 4px;"
            "}"
            "QPushButton:hover {"
            "   background-color: #F3F4F6;"
            "}"
            );
        qDebug() << "Clipboard monitoring started";
    }else{

        isTranslateMonitoring = false;
        ui->translateButton->setStyleSheet(
            "QPushButton {"
            "   background-color: #F3F4F6;"
            "   border: 1px solid #FFFFFF;"
            "   border-radius: 4px;"
            "}"
            "QPushButton:hover {"
            "   background-color: #CDD1D9;"
            "}"
            );
    }
}

void MainWindow::handleClipboardChange()
{
    // 获取文本内容
    const QString text = clipboard->text();
    const QString inputEditEext = ui->inputEdit->text();
    // 获取图像内容（可选）
    // const QPixmap pixmap = clipboard->pixmap();

    if (!text.isEmpty()) {
        qDebug() << "Clipboard Updated:" << text;
        ui->inputEdit->setText(inputEditEext+text);
    } else {
        qDebug() << "Clipboard Updated: [Non-text data]";
    }
    this->activateWindow();
    this->setWindowState((this->windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
    if(isexplainCodeMonitoring || isTranslateMonitoring){//如果相关按钮开启则自动发送请求
        sendRequest();
        setExpandDialogueWidget();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pinnedButton_clicked()
{
    if (isTopping) {
        // 如果当前是置顶状态，取消置顶
        this->setWindowFlag(Qt::WindowStaysOnTopHint, false);
        isTopping = false;  // 更新状态
        ui->pinnedButton->setStyleSheet(
            "QPushButton {"
            "   background-color: #000000;"
            "}"
            "QPushButton:hover {"
            "   background-color: ##EF4444;"
            "}"
            );
    } else {
        // 如果当前不是置顶状态，置顶
        this->setWindowFlag(Qt::WindowStaysOnTopHint, true);
        isTopping = true;  // 更新状态
        ui->pinnedButton->setStyleSheet(
            "QPushButton {"
            "   background-color: #EF4444;"
            "}"
            "QPushButton:hover {"
            "   background-color: #000000;"
            "}"
            );
    }

    this->show();  // 刷新窗口
}

