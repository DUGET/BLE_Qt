#include "smartpushbutton.h"

SmartPushButton::SmartPushButton(QWidget *parent)
{}

void SmartPushButton::toggleDisabled()
{
    if(isEnabled())
    {
        setDisabled(false);
    }
    else
    {
        setDisabled(true);
    }
}
