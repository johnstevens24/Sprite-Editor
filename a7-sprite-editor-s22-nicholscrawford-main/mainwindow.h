/* Tarnished Pixels
 * CS3505 Spring 2022
 * Assignment 7: Sprite Editor Implementation
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QColorDialog>
#include <QPalette>
#include <QFileDialog>
#include <model.h>
#include "previewwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(Model& m, PreviewWindow& pw, QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void setPixel(int x, int y, QColor color, QImage image);
    void setDimensions(int width, int height);
    void colorSelected(QColor color);
    void chooseCurrentColorButton(int which);
    void setCanvasDimensions(int width, int height);
    void toolSelected(std::string tool);
    void playPauseClicked();
    void fpsSliderChanged(int fps);
    void showPreview();
    void loadFileSelected(QString filename);
    void saveFileSelected(QString filename);
    void changeColorsToDefault();

private slots:
    void on_SelectNewColorButton_clicked();
    void updateButtonColor(QColor color, int whichButton);
    void setUpProject(int imageWidth, int imageHeight);
    void on_setDimensionsButton_clicked();
    void on_PrimaryColorButton_clicked();
    void on_SecondaryColorButton_clicked();
    void updateMainCanvas(QImage image);
    void updatePreviewWindow(QImage image);
    void on_pencilRadioButton_clicked();
    void on_eraserRadioButton_clicked();
    void on_colorDropperRadioButton_clicked();
    void updateImageLabel(QString s);
    void on_playPauseAnimationButton_clicked();
    void updatePlayPauseAnimationButtonText(QString text);
    void on_fpsSlider_sliderMoved(int position);
    void on_actualSizePreview_clicked();
    void createMessagePopup(QString message1, QString message2);
    void on_load();
    void on_save();

private:
    Ui::MainWindow *ui;
    PreviewWindow& previewWindow;
    QSize mainCanvasDefaultSize;
    QSize previewWindowDefaultSize;
};
#endif // MAINWINDOW_H
