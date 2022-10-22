/* Tarnished Pixels
 * CS3505 Spring 2022
 * Assignment 7: Sprite Editor Implementation
*/

#include "clickablelabel.h"
#include <QMouseEvent>

/**
 * @brief ClickableLabel::ClickableLabel - Constructor for a clickablelabel.
 * @param parent
 * @param f
 */
ClickableLabel::ClickableLabel(QWidget *parent, Qt::WindowFlags f):QLabel(parent,f){}


/**
 * @brief ClickableLabel::ClickableLabel - Constructor for a clickablelabel.
 * @param text
 * @param parent
 * @param f
 */
ClickableLabel::ClickableLabel(const QString &text, QWidget *parent, Qt::WindowFlags f):QLabel(text,parent,f){}


/**
 * @brief ClickableLabel::~ClickableLabel - Destructor for a clickeablelabel.
 */
ClickableLabel::~ClickableLabel(){}


/**
 * @brief ClickableLabel::mousePressEvent - registers and emits the mouse position of where it was clicked
 * @param ev - mouse press event
 */
void ClickableLabel::mousePressEvent(QMouseEvent *ev)
{
    emit clicked(ev->pos());
}
