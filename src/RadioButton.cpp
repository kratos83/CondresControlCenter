#include "RadioButton.h"

RadioButton::RadioButton(QWidget *parent) :
  QWidget(parent)
{
  setFixedHeight(20);
  setFixedWidth(70);
  setChecked(false);
  connect(this,&RadioButton::clicked,this,&RadioButton::setChecked);
}

bool RadioButton::isChecked()
{
 return m_enable;
}

void RadioButton::setIcon(QIcon icon)
{
        Q_UNUSED(icon)
}

void RadioButton::setChecked(bool vero)
{
  m_enable = vero;
}

QString RadioButton::text()
{
  return _text;
}

void RadioButton::setText(QString m_text)
{
  _text = m_text;
}

void RadioButton::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    if(m_enable == true)
        m_enable = false;
    else if(m_enable == false)
        m_enable = true;
    emit clicked(m_enable);
    update();
}

void RadioButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    painter = new QPainter(this);
    drawPaint(painter);
}

void RadioButton::drawPaint(QPainter* painter)
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
        brush1 = QBrush(QColor("lightgreen"),Qt::SolidPattern);
        painter->setBrush(brush1);

        painter->drawRoundedRect(0,0,width()-2,height()-2,height()/2,height()/2);

        brush2 = QBrush(QColor("white"),Qt::SolidPattern);
        painter->setBrush(brush2);
        painter->drawEllipse(width()-height(),0,height(),height());

        painter->setPen(Qt::black);
        painter->drawText(width()/4,height()/1.5,"SI");
    }
    else
    {
        brush3 = QBrush(QColor(255,135,99),Qt::SolidPattern);
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
