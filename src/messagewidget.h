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
