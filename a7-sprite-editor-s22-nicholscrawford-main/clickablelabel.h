/* Tarnished Pixels
 * CS3505 Spring 2022
 * Assignment 7: Sprite Editor Implementation
*/

#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QLabel>
#include <QFlags>

class ClickableLabel : public QLabel
{
Q_OBJECT

public:
ClickableLabel(QWidget *parent = 0, Qt::WindowFlags f = Qt::WindowFlags());
ClickableLabel(const QString &text, QWidget *parent=0, Qt::WindowFlags f = Qt::WindowFlags());
~ClickableLabel();

protected:
void mousePressEvent(QMouseEvent *ev);

signals:
void clicked(QPoint pos);

};
#endif // CLICKABLELABEL_H
