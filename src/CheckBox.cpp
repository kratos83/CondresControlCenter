#include "CheckBox.h"

CheckBox::CheckBox(QWidget *parent) :
    QWidget(parent)
{
    setFixedWidth(70);
    setFixedHeight(20);
    setChecked(false);
    connect(this,&CheckBox::clicked,this,&CheckBox::setChecked);
}

bool CheckBox::isChecked()
{
   return m_enable;
}

void CheckBox::setChecked(bool vero)
{
    m_enable = vero;
}

bool CheckBox::isTristate()
{
    return m_tristate;
}

void CheckBox::setTristate(bool vero)
{
    m_tristate = vero;
}

QString CheckBox::text()
{
    return _text;
}

void CheckBox::setText(QString m_text)
{
    _text = m_text;
}

void CheckBox::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    if(m_enable == true)
        m_enable = false;
    else if(m_enable == false)
        m_enable = true;
    emit clicked(m_enable);
    update();
}

void CheckBox::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    painter = new QPainter(this);
    drawPaint(painter);
}

void CheckBox::drawPaint(QPainter* painter)
{
    //Background
    brush = QBrush(QColor("transparent"),Qt::SolidPattern);
    painter->fillRect(rect(),brush);

    painter->setRenderHints(QPainter::Antialiasing);

    QFont font;
    font.setFamily("Arial");
    font.setPixelSize(13);
    painter->setFont(font);

    if(isChecked())
    {
        brush1 = QBrush(QColor("blue"),Qt::SolidPattern);
        painter->setBrush(brush1);

        painter->drawRoundedRect(0,0,width()-2,height()-2,height()/2,height()/2);

        brush2 = QBrush(QColor("white"),Qt::SolidPattern);
        painter->setBrush(brush2);
        painter->drawEllipse(width()-height(),0,height(),height());

        painter->setPen(Qt::white);
        painter->drawText(width()/4,height()/1.5,"YES");
    }
    else
    {
        brush3 = QBrush(QColor("gray"),Qt::SolidPattern);
        painter->setBrush(brush3);

        painter->drawRoundedRect(0,0,width()-2,height()-2,height()/2,height()/2);

        brush4 = QBrush(QColor("white"),Qt::SolidPattern);
        painter->setBrush(brush4);
        painter->drawEllipse(0,0,height(),height());

        painter->setPen(Qt::white);
        painter->drawText(width()/2,height()/1.5,"NO");
    }
    painter->end();
}
