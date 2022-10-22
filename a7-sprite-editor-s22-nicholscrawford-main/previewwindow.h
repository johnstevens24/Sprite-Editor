/* Tarnished Pixels
 * CS3505 Spring 2022
 * Assignment 7: Sprite Editor Implementation
*/

#ifndef PREVIEWWINDOW_H
#define PREVIEWWINDOW_H

#include <QDialog>
#include "model.h"

namespace Ui {
class PreviewWindow;
}

class PreviewWindow : public QDialog
{
    Q_OBJECT

public:
    explicit PreviewWindow(Model& model, QWidget *parent = nullptr);
    ~PreviewWindow();

private:
    Ui::PreviewWindow *ui;

public slots:
    void updatePreviewWindow(QImage image);

};
#endif // PREVIEWWINDOW_H
