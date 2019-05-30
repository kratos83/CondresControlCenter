#ifndef CHECKBOX_H
#define CHECKBOX_H

#include <QtWidgets>

class CheckBox : public QWidget
{
    Q_OBJECT

public:
    explicit CheckBox(QWidget *parent = nullptr);

public slots:
    bool isChecked();
    void setChecked(bool vero);
    QString text();
    void setText(QString m_text);
    bool isTristate();
    void setTristate(bool vero);

private:
    bool m_enable, m_tristate;
    QPainter *painter;
    QBrush brush,brush1, brush2,brush3,brush4;
    QString _text;

protected:
    void paintEvent(QPaintEvent *event);
    void drawPaint(QPainter *painter);
    void mousePressEvent(QMouseEvent *event);

signals:
    void clicked(bool);
};

#endif // CHECKBOX_H
