#ifndef RADIOBUTTON_H
#define RADIOBUTTON_H

#include <QtWidgets>

class RadioButton : public QWidget
{
    Q_OBJECT

public:
    RadioButton(QWidget *parent=0);

public slots:
    bool isChecked();
    void setChecked(bool vero);
    QString text();
    void setText(QString m_text);
    void setIcon(QIcon icon);
protected:
    void paintEvent(QPaintEvent *event);
    void drawPaint(QPainter *painter);
    void mousePressEvent(QMouseEvent *event);
private:
    bool m_enable, m_tristate;
    QPainter *painter;
    QBrush brush,brush1, brush2,brush3,brush4;
    QString _text;
signals:
    void clicked(bool);
};

#endif // RADIOBUTTON_H
