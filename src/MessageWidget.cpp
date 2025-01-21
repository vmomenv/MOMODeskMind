// MessageWidget.cpp
#include "MessageWidget.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QPushButton>
#include <QIcon>
#include <QGraphicsDropShadowEffect>

MessageWidget::MessageWidget(const QString &message,
                             const QString &priority,
                             QWidget *parent)
    : QWidget(parent),
    priorityIndicator(new QWidget(this)),
    iconLabel(new QLabel(this)),
    messageLabel(new QLabel(message, this)),
    deleteButton(new QPushButton(this)),
    layout(new QHBoxLayout(this))
{
    // 基本样式设置
    setAttribute(Qt::WA_StyledBackground); // 启用样式表继承
    setMinimumHeight(40);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // 容器样式
    setStyleSheet(R"(
        MessageWidget {
            background-color: #F9FAFB;
            border-radius: 8px;
            padding: 8px;
        }
    )");

    // 添加阴影效果
    auto shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setBlurRadius(8);
    shadowEffect->setOffset(0, 2);
    shadowEffect->setColor(QColor(0, 0, 0, 10));
    setGraphicsEffect(shadowEffect);

    // 优先级指示点
    priorityIndicator->setFixedSize(10, 10);
    priorityIndicator->setStyleSheet("border-radius: 5px;");

    // 日历图标（需要资源文件支持）
    iconLabel->setPixmap(QIcon(":/img/calendar.svg").pixmap(14, 16));
    // iconLabel->setStyleSheet("color: #6B7280;");

    // 消息文本样式
    messageLabel->setStyleSheet(R"(
        QLabel {
            color: #374151;
            font-size: 16px;
            margin-left: 0px;
        }
    )");
    messageLabel->setWordWrap(true);

    // 删除按钮样式
    deleteButton->setIcon(QIcon(":/img/trash.svg"));
    deleteButton->setIconSize(QSize(14, 16));
    deleteButton->setStyleSheet(R"(
        QPushButton {
            background: transparent;
            border: none;
            padding: 4px;
            min-width: 14px;
            min-height: 24px;
            border-radius: 1px;
            color: #EF4444;
        }
        QPushButton:hover {
            background-color: #FEE2E2;
            color: #DC2626;
        }
    )");

    // 构建左侧内容布局
    QWidget* leftContainer = new QWidget(this);
    QHBoxLayout* leftLayout = new QHBoxLayout(leftContainer);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(12);
    leftLayout->addWidget(priorityIndicator);
    leftLayout->addWidget(iconLabel);
    leftLayout->addWidget(messageLabel);
    leftContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    // 主布局配置
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(16);
    layout->addWidget(leftContainer);
    layout->addWidget(deleteButton, 0, Qt::AlignRight);

    // 设置优先级颜色
    setPriorityColor(priority);

    // 连接信号槽
    connect(deleteButton, &QPushButton::clicked, this, &MessageWidget::onDeleteClicked);
}

void MessageWidget::setPriorityColor(const QString &priority)
{
    QString colorCode;
    if (priority == "urgent") {
        colorCode = "#EF4444";  // 红色
    } else if (priority == "high") {
        colorCode = "#EAB308";  // 黄色
    } else {  // 默认非紧急
        colorCode = "#22C55E";  // 绿色
    }

    priorityIndicator->setStyleSheet(
        QString("background-color: %1; border-radius: 5px;").arg(colorCode)
        );
}

void MessageWidget::onDeleteClicked()
{
    qDebug() << "请求删除项目:" << messageLabel->text();
    emit deleteClicked(this);  // 传递自身指针
}

MessageWidget::~MessageWidget()
{
    // 自动释放子组件（Qt对象树机制）
}
