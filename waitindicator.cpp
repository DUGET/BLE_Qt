#include "waitindicator.h"
#include <QPainter>

WaitIndicator::WaitIndicator(QWidget* parent) : QWidget(parent) {
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(m_size, m_size);

    connect(&m_timer, &QTimer::timeout, this, [this]()
    {
        m_angle = (m_angle + 30) % 360;
        update();
    });
}

void WaitIndicator::enable() {
    m_timer.start(30);
    show();
}

void WaitIndicator::disable() {
    m_timer.stop();
    hide();
}

void WaitIndicator::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    int padding = m_thickness;
    QRect rect(padding, padding, width() - 2*padding, height() - 2*padding);

    p.setPen(QPen(m_color, m_thickness));
    p.drawArc(rect, m_angle * 16, 270 * 16); // Рисуем дугу 270 градусов
}

void WaitIndicator::enterEvent(QEnterEvent*) {
    enable();
}

void WaitIndicator::leaveEvent(QEvent*) {
    disable();
}
