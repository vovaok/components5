#include "labeledled.h"

LabeledLed::LabeledLed(QWidget *parent) :
    QHBoxLayout(parent)
{
    mLed = new Led();
    mLabel = new QLabel();
    addWidget(mLed, 0, Qt::AlignLeft | Qt::AlignVCenter);
    addWidget(mLabel, 0, Qt::AlignLeft | Qt::AlignVCenter);
    addStretch(1);
    setMargin(0);
    setSpacing(2);
    setContentsMargins(0, 0, 0, 0);
}

LabeledLed::LabeledLed(QString label, QColor color, int size, QWidget *parent) :
    QHBoxLayout(parent)
{
    mLed = new Led(color);
    mLed->setMinimumSize(size, size);
    mLabel = new QLabel(label);
    addWidget(mLed, 0, Qt::AlignLeft | Qt::AlignVCenter);
    addWidget(mLabel, 0, Qt::AlignLeft | Qt::AlignVCenter);
    addStretch(1);
    setMargin(0);
    setSpacing(2);
    setContentsMargins(0, 0, 0, 0);
}
