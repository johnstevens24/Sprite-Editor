/* Tarnished Pixels
 * CS3505 Spring 2022
 * Assignment 7: Sprite Editor Implementation
*/

#include "previewwindow.h"
#include "ui_previewwindow.h"

/**
 * @brief PreviewWindow::PreviewWindow - Constructor for a previewwindow object.
 * @param model
 * @param parent
 */
PreviewWindow::PreviewWindow(Model& model, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreviewWindow)
{
    ui->setupUi(this);
    connect(&model, &Model::updatePreviewWindow, this, &PreviewWindow::updatePreviewWindow);
}


/**
 * @brief PreviewWindow::~PreviewWindow - Destructor for a previewwindow object.
 */
PreviewWindow::~PreviewWindow()
{
    delete ui;
}


/**
 * @brief PreviewWindow::updatePreviewWindow - updates the image in the preview.
 * @param image - image to add to the window.
 */
void PreviewWindow::updatePreviewWindow(QImage image)
{
    ui->actualSizePreview->setPixmap(QPixmap::fromImage(image));
}
