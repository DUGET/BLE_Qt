#ifndef SMARTPUSHBUTTON_H
#define SMARTPUSHBUTTON_H

#include <QPushButton>

class SmartPushButton : public QPushButton
{
    Q_OBJECT

public:
    explicit SmartPushButton(QWidget *parent = nullptr);

    void toggleDisabled();
};

#endif // SMARTPUSHBUTTON_H
