#include "unocard.h"

unocard::unocard()
{

}

void unocard::setupImage(bool isBack)
{
    if (isBack)
    {
        pixmap=QPixmap(":/Uno/Back");
        label->setScaledContents(true);
        label->setPixmap(pixmap);
        label->setFixedSize(176,256);
    }
    else
    {
        pixmap=QPixmap(cheminImage);
        label->setScaledContents(true);
        label->setPixmap(pixmap);
        label->setFixedSize(176,256);
    }
}
