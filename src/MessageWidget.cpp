// MessageWidget.cpp
#include "MessageWidget.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QPushButton>
#include <QIcon>
#include <QGraphicsDropShadowEffect>
#include <QFontMetrics>

MessageWidget::MessageWidget(const QString &message,
                             const QString &time,
                             const QString &priority,
                             QWidget *parent)
    : QWidget(parent),
    priorityIndicator(new QWidget(this)),
    timeLabel(new QLabel(time, this)),
    iconLabel(new QLabel(this)),
    messageLabel(new QLabel(message, this)),
    deleteButton(new QPushButton(this)),
    layout(new QHBoxLayout(this))
{
    // ================= 基础设置 =================
    setAttribute(Qt::WA_StyledBackground);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    // ================= 容器样式 =================
    setStyleSheet(R"(
        MessageWidget {
            background-color: #F9FAFB;
            border-radius: 8px;
            padding: 12px 5px;
            margin-bottom: 4px;
        }
        MessageWidget > QWidget {
            background-color: transparent;
        }
    )");

    // ================= 阴影效果 =================
    auto shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setBlurRadius(6);
    shadowEffect->setOffset(0, 1);
    shadowEffect->setColor(QColor(0, 0, 0, 0.08));
    setGraphicsEffect(shadowEffect);

    // ================= 组件初始化 =================
    // 1. 优先级指示点（8px直径）
    priorityIndicator->setFixedSize(8, 8);
    priorityIndicator->setStyleSheet("border-radius: 8px;");

    // 2. 日历图标
    iconLabel->setPixmap(QIcon(":/img/calendar.svg").pixmap(16, 16));
    iconLabel->setStyleSheet(R"(
        QLabel {
            background: transparent;
        }
    )");


    // 3. 时间标签
    timeLabel->setStyleSheet(R"(
        QLabel {
            background: transparent;
            color: #6B7280;
            font-size: 14px;
            min-width: 48px;
            margin-left: 8px;
        }
    )");
    timeLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

    // 4. 消息文本
    messageLabel->setStyleSheet(R"(
        QLabel {
            background: transparent;
            color: #374151;
            font-size: 14px;
            margin-left: 12px;
            line-height: 1.5;
            max-width: 200px;
        }
    )");
    messageLabel->setWordWrap(true);
    messageLabel->setAlignment(Qt::AlignVCenter);

    // 5. 删除按钮（悬停效果）
    deleteButton->setIcon(QIcon(":/img/trash.svg"));
    deleteButton->setIconSize(QSize(16, 16));
    deleteButton->setStyleSheet(R"(
        QPushButton {
            background: transparent;
            border: none;
            padding: 8px;
            max-width: 5px;
            max-height: 5px;
            border-radius: 6px;

        }
        QPushButton:hover {
            background-color: #FEE2E2;
        }
        QPushButton:pressed {
            background-color: #FECACA;
        }
    )");

    // ================= 布局系统 =================
    // // 左侧内容布局（8px基准间距）
    QWidget* leftContainer = new QWidget(this);
    QHBoxLayout* leftLayout = new QHBoxLayout(leftContainer);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(8);

    // 组件顺序：红点 -> 图标 -> 时间 -> 消息
    leftLayout->addWidget(priorityIndicator);
    leftLayout->addWidget(iconLabel);
    leftLayout->addWidget(timeLabel);
    leftLayout->addWidget(messageLabel, 1); // 消息标签弹性扩展
    leftContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    // // 主布局（右侧删除按钮）
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(12);
    layout->addWidget(leftContainer);
    layout->addWidget(deleteButton, 0, Qt::AlignRight);

    // ================= 功能配置 =================
    setPriorityColor(priority);
    connect(deleteButton, &QPushButton::clicked, this, &MessageWidget::onDeleteClicked);

    // ================= 动态高度计算 =================
    QFontMetrics metrics(messageLabel->font());
    int availableWidth = 328 - 32; // 328总宽度 - 左右内边距(16*2)
    QRect textRect = metrics.boundingRect(0, 0, availableWidth, 0,
                                          Qt::TextWordWrap, message);
    int lineHeight = metrics.lineSpacing();
    int totalHeight = textRect.height() + 24; // 24=上下内边距(12*2)
    setMinimumHeight(qMax(48, totalHeight));
}

void MessageWidget::setPriorityColor(const QString &priority)
{
    QString colorCode;
    if (priority == "urgent") {
        colorCode = "#EF4444";
    } else if (priority == "high") {
        colorCode = "#EAB308";
    } else {
        colorCode = "#22C55E";
    }
    priorityIndicator->setStyleSheet(
        QString("background-color: %1; border-radius: 4px;").arg(colorCode)
        );
}

void MessageWidget::onDeleteClicked()
{
    emit deleteClicked(this);
}

MessageWidget::~MessageWidget()
{

}
