#ifndef MESSAGEWIDGET_H
#define MESSAGEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>

class MessageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MessageWidget(const QString &message,
                           const QString &time,
                           const QString &priority,
                           QWidget *parent = nullptr);
    QString getMessage() const { return messageLabel->text(); }
    QString getTime() const { return timeLabel->text(); }
    QString getPriority() const
    {
        QString style = priorityIndicator->styleSheet();
        if(style.contains("#EF4444")) return "urgent";
        if(style.contains("#EAB308")) return "high";
        return "normal";
    }

    ~MessageWidget();

signals:
    void deleteClicked(MessageWidget* widget);

private slots:
    void onDeleteClicked();

private:
    void setPriorityColor(const QString &priority);

    // UI组件
    QWidget *priorityIndicator;
    QLabel *timeLabel;
    QLabel *iconLabel;
    QLabel *messageLabel;
    QPushButton *deleteButton;
    QHBoxLayout *layout;
};

#endif // MESSAGEWIDGET_H
