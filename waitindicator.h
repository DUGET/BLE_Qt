#ifndef WAITINDICATOR_H
#define WAITINDICATOR_H

#include <QWidget>
#include <QTimer>

class WaitIndicator : public QWidget
{
    Q_OBJECT
public:
    explicit WaitIndicator(QWidget *parent = nullptr);

    void enable();
    void disable();

protected:
    void paintEvent(QPaintEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    QTimer m_timer;
    QColor m_color = Qt::green;
    int m_angle = 0;
    int m_size = 20;
    int m_thickness = 3;

signals:
};

#endif // WAITINDICATOR_H
