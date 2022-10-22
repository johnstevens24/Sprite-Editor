/* Tarnished Pixels
 * CS3505 Spring 2022
 * Assignment 7: Sprite Editor Implementation
*/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "clickablelabel.h"
#include <QPixmap>
#include <iostream>
#include "previewwindow.h"

/**
 * @brief MainWindow::MainWindow - Constructor for the main window and establishes connections between model and view (main window).
 * @param model
 * @param pw
 * @param parent
 */
MainWindow::MainWindow(Model& model, PreviewWindow& pw, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , previewWindow(pw)
{
    // Initial settings
    ui->setupUi(this);
    mainCanvasDefaultSize = ui->mainCanvas->size();
    previewWindowDefaultSize = ui->previewWindow->size();
    updateButtonColor(QColor("blue"), 1);
    updateButtonColor(QColor("red"), 2);
    // Set dimensions and enable/disable buttons
    connect(this, &MainWindow::setDimensions, &model, &Model::setDimensions);
    connect(this, &MainWindow::setCanvasDimensions, &model, &Model::setCanvasDimensions);
    connect(&model, &Model::setUpProject, this, &MainWindow::setUpProject);
    // Update button colors
    connect(&model, &Model::updateButtonColor, this, &MainWindow::updateButtonColor);
    connect(this, &MainWindow::colorSelected, &model, &Model::userSetNewColor);
    connect(this, &MainWindow::chooseCurrentColorButton, &model, &Model::setCurrentColor);
    connect(this, &MainWindow::changeColorsToDefault, &model, &Model::setInitialButtonColors);
    // Update visual components
    connect(&model, &Model::updateMainCanvas, this, &MainWindow::updateMainCanvas);
    connect(&model, &Model::updatePreviewWindow, this, &MainWindow::updatePreviewWindow);
    connect(&model, &Model::updateImageLabel, this, &MainWindow::updateImageLabel);
    // Tools
    connect(this, &MainWindow::toolSelected, &model, &Model::setCurrentTool);
    connect(ui->mainCanvas, &ClickableLabel::clicked, &model, &Model::selectTool);
    // Switching between images
    connect(ui->nextFrameButton, &QPushButton::clicked, &model, &Model::nextImage);
    connect(ui->previousFrameButton, &QPushButton::clicked, &model, &Model::previousImage);
    connect(&model, &Model::enablePreviousImageButton, ui->previousFrameButton, &QPushButton::setEnabled);
    connect(&model, &Model::enableNextImageButton, ui->nextFrameButton, &QPushButton::setEnabled);
    // Add, remove, duplicate images
    connect(ui->addFrameButton, &QPushButton::clicked, &model, &Model::addImage);
    connect(ui->removeFrameButton, &QPushButton::clicked, &model, &Model::removeImage);
    connect(&model, &Model::enableRemoveImageButton, ui->removeFrameButton, &QPushButton::setEnabled);
    connect(ui->duplicateFrameButton, &QPushButton::clicked, &model, &Model::duplicateImage);
    // Animation
    connect(&model, &Model::updatePlayPauseAnimationButtonText, this, &MainWindow::updatePlayPauseAnimationButtonText);
    connect(this, &MainWindow::playPauseClicked, &model, &Model::playPauseClicked);
    connect(this, &MainWindow::fpsSliderChanged, &model, &Model::fpsSliderChanged);
    // Saving and loading files
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::on_save);
    connect(this, &MainWindow::saveFileSelected, &model, &Model::saveSpriteJson);
    connect(ui->actionOpen_File, &QAction::triggered, this, &MainWindow::on_load);
    connect(this, &MainWindow::loadFileSelected, &model, &Model::loadSpriteJson);
    connect(&model, &Model::createMessagePopup, this, &MainWindow::createMessagePopup);
}


/**
 * @brief MainWindow::~MainWindow - Destructor for the main window
 */
MainWindow::~MainWindow()
{
    delete ui;
}


/**
 * @brief MainWindow::on_SelectNewColorButton_clicked - creates a QColorDialogBox, recieves the user's chosen color,
 * and then returns that color to the model to be stored/used.
 */
void MainWindow::on_SelectNewColorButton_clicked()
{
    //Qt::white just sets the QColorDialog to be on white when it opens. Could be set to any color.
    QColor color = QColorDialog::getColor(Qt::white, this, "Choose color");

    emit colorSelected(color);
}


/**
 * @brief MainWindow::updateButtonColor - recieves a signal emitted from the model and updates the color/stylesheet
 * one of the color buttons.
 * @param color - the new color
 * @param whichButton - a 1 or 2 depending on which color button
 */
void MainWindow::updateButtonColor(QColor color, int whichButton)
{
    QString newColor("border: 2px solid; border-radius: 2px; border-color: rgb(0,0,0);background: #" + QString(color.red() < 16? "0" : "") + QString::number(color.red(),16) + QString(color.green() < 16? "0" : "") + QString::number(color.green(),16) + QString(color.blue() < 16? "0" : "") + QString::number(color.blue(),16) + ";");

    if(whichButton == 1)
    {
        ui->PrimaryColorButton->setStyleSheet(newColor);
    }
    if(whichButton == 2)
    {
        ui->SecondaryColorButton->setStyleSheet(newColor);
    }
}


/**
 * @brief MainWindow::setUpProject - sets up the main canvas, preview canvas, and activates the correct
 * buttons to allow editing of the pixels, frames, and to save projects.
 * @param imageWidth - sprite image width in pixels.
 * @param imageHeight - sprite image height in pixels.
 */
void MainWindow::setUpProject(int imageWidth, int imageHeight)
{
    //sets all the ui objects to allow for editing of the sprite but not the dimensions
    ui->heightSpinBox->setValue(imageHeight);
    ui->widthSpinBox->setValue(imageWidth);
    ui->heightSpinBox->setEnabled(false);
    ui->widthSpinBox->setEnabled(false);
    ui->setDimensionsButton->setEnabled(false);
    ui->PrimaryColorButton->setEnabled(true);
    ui->SecondaryColorButton->setEnabled(true);
    ui->pencilRadioButton->setEnabled(true);
    ui->pencilRadioButton->setChecked(true);
    ui->eraserRadioButton->setEnabled(true);
    ui->colorDropperRadioButton->setEnabled(true);
    ui->mainCanvas->setEnabled(true);
    ui->SelectNewColorButton->setEnabled(true);
    ui->previewWindow->setEnabled(true);
    ui->addFrameButton->setEnabled(true);
    ui->duplicateFrameButton->setEnabled(true);
    ui->fpsSlider->setEnabled(true);
    ui->playPauseAnimationButton->setEnabled(true);
    ui->actualSizePreview->setEnabled(true);
    ui->actionSave->setEnabled(true);

    emit changeColorsToDefault();
    emit setCanvasDimensions(ui->mainCanvas->width(), ui->mainCanvas->height());
}


/**
 * @brief MainWindow::on_setDimensionsButton_clicked - takes in the the users width and height
 * for the sprite image and sends it to the model.
 */
void MainWindow::on_setDimensionsButton_clicked()
{
    int height = ui->heightSpinBox->value();
    int width = ui->widthSpinBox->value();

    emit setDimensions(width, height);
}


/**
 * @brief MainWindow::on_PrimaryColorButton_clicked - adds a border to the primary color button, removes a border
 * from the secondary color button, and sends a signal to the model about which one is now selected.
 */
void MainWindow::on_PrimaryColorButton_clicked()
{
    //adds a border to the primary color button
    QString temp1 = ui->PrimaryColorButton->styleSheet();
    temp1 += "border: 2px solid; border-radius: 2px; border-color: rgb(0,0,0);";
    ui->PrimaryColorButton->setStyleSheet(temp1);
    //removes the border from the secondary color button
    QString temp2 = ui->SecondaryColorButton->styleSheet();
    temp2.remove("border: 2px solid; border-radius: 2px; border-color: rgb(0,0,0);");
    ui->SecondaryColorButton->setStyleSheet(temp2);

    emit chooseCurrentColorButton(1);
}


/**
 * @brief MainWindow::on_PrimaryColorButton_clicked - adds a border to the secondary color button, removes a border
 * from the primary color button, and sends a signal to the model about which one is now selected.
 */
void MainWindow::on_SecondaryColorButton_clicked()
{
    //adds a border to the secondary color button
    QString temp1 = ui->SecondaryColorButton->styleSheet();
    temp1 += "border: 2px solid; border-radius: 2px; border-color: rgb(0,0,0);";
    ui->SecondaryColorButton->setStyleSheet(temp1);
    //removes a border from the primary color button
    QString temp2 = ui->PrimaryColorButton->styleSheet();
    temp2.remove("border: 2px solid; border-radius: 2px; border-color: rgb(0,0,0);");
    ui->PrimaryColorButton->setStyleSheet(temp2);

    emit chooseCurrentColorButton(2);
}


/**
 * @brief MainWindow::updateMainCanvas - Updates the main canvas in the UI to match the state of the image.
 * This is called whenever changes are made to the image.
 * @param image - The current image being editted and displayed.
 */
void MainWindow::updateMainCanvas(QImage image)
{
    QImage reimage = image.scaled(mainCanvasDefaultSize, Qt::KeepAspectRatio, Qt::FastTransformation);
    ui->mainCanvas->setFixedSize(reimage.size());
    ui->mainCanvas->setPixmap(QPixmap::fromImage(reimage));
}


/**
 * @brief MainWindow::updatePreviewWindow - Updates the smaller preview window in the UI to match the state of the image.
 * This is called whenever changes are made to the image.
 * @param image - The current image being editted and displayed.
 */
void MainWindow::updatePreviewWindow(QImage image)
{
    QImage reimage = image.scaled(previewWindowDefaultSize, Qt::KeepAspectRatio, Qt::FastTransformation);
    ui->previewWindow->setFixedSize(reimage.size());
    ui->previewWindow->setPixmap(QPixmap::fromImage(reimage));
}


/**
 * @brief MainWindow::on_pencilRadioButton_clicked - Emits signal the pencil radio button has been clicked.
 */
void MainWindow::on_pencilRadioButton_clicked()
{
    emit toolSelected("pencil");
}


/**
 * @brief MainWindow::on_eraserRadioButton_clicked - Emits signal the eraser radio button has been clicked.
 */
void MainWindow::on_eraserRadioButton_clicked()
{
    emit toolSelected("eraser");
}


/**
 * @brief MainWindow::on_colorDropperRadioButton_clicked - Emits signal the color dropper radio button has been clicked.
 */
void MainWindow::on_colorDropperRadioButton_clicked()
{
    emit toolSelected("color dropper");
}


/**
 * @brief MainWindow::updateImageLabel - receives a QString from the model and uses it to update the image
 * label in the view
 * @param s - the QString to display
 */
void MainWindow::updateImageLabel(QString s)
{
    ui->currentFrameLabel->setText(s);
}


/**
 * @brief MainWindow::on_playPauseAnimationButton_clicked - emits the play/pause toggle.
 */
void MainWindow::on_playPauseAnimationButton_clicked()
{
    emit playPauseClicked();
}


/**
 * @brief MainWindow::updatePlayPauseAnimationButtonText - changes the button text to the input text
 * @param text - text to change button to.
 */
void MainWindow::updatePlayPauseAnimationButtonText(QString text)
{
    ui->playPauseAnimationButton->setText(text);
}


/**
 * @brief MainWindow::on_fpsSlider_sliderMoved emits the signal that the fps slider has changed.
 * @param fps - fps that it has been set to.
 */
void MainWindow::on_fpsSlider_sliderMoved(int fps)
{
    emit fpsSliderChanged(fps);
}


/**
 * @brief MainWindow::on_actualSizePreview_clicked - shows the actual size preview window.
 */
void MainWindow::on_actualSizePreview_clicked()
{
    previewWindow.show();
    emit showPreview();
}


/**
 * @brief MainWindow::on_load - Opens the file dialog prompt for opening files.
 */
void MainWindow::on_load()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                               "/home/",
                               tr("Sprites (*.ssp)"));

    if(fileName != "")
    {
        emit loadFileSelected(fileName);
    }
}


/**
 * @brief MainWindow::on_save - Opens the file dialog prompt for saving files.
 */
void MainWindow::on_save()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                               "/home/",
                               tr("Sprites (*.ssp)"));

    emit saveFileSelected(fileName);
}


/**
 * @brief MainWindow::createMessagePopup - creates a pop up dialog box to inform the user of something
 * @param message1 - the message to be displayed at the top of the popup dialog box
 * @param message2 - the message to be displayed inside the popup dialog box
 */
void MainWindow::createMessagePopup(QString message1, QString message2)
{
    QWidget tempWidget;
    //creates a popup if trying to read a corrupt file
    QMessageBox::warning(&tempWidget, message1, message2);
}
