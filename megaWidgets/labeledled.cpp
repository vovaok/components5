#include "labeledled.h"

LabeledLed::LabeledLed(QWidget *parent) :
    QWidget(parent)
{
    mLed = new Led();
    mLabel = new QLabel();
    init();
}

LabeledLed::LabeledLed(QString label, QColor color, int size, QWidget *parent) :
    QWidget(parent)
{
    mLed = new Led(color);
    mLed->setMinimumSize(size, size);
    mLabel = new QLabel(label);
    init();
}

void LabeledLed::init()
{
    QHBoxLayout *lay = new QHBoxLayout;
    setLayout(lay);
    lay->addWidget(mLed, 0, Qt::AlignLeft | Qt::AlignVCenter);
    lay->addWidget(mLabel, 0, Qt::AlignLeft | Qt::AlignVCenter);
    lay->addStretch(1);
    lay->setMargin(0);
    lay->setSpacing(2);
    lay->setContentsMargins(0, 0, 0, 0);
}
