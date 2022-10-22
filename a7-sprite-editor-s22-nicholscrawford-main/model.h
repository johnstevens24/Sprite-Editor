/* Tarnished Pixels
 * CS3505 Spring 2022
 * Assignment 7: Sprite Editor Implementation
*/

#ifndef MODEL_H
#define MODEL_H

#include <QObject>
#include <vector>
#include <QImage>
#include "clickablelabel.h"
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QVector>
#include <QMessageBox>

class Model : public QObject
{
    Q_OBJECT

public:
    explicit Model(QObject *parent = nullptr);

    void createFirstImage(int width, int height);
    void setCurrentColor(int whichColor);

signals:
    void updateButtonColor(QColor color, int whichColor);
    void updateMainCanvas(QImage image);
    void updatePreviewWindow(QImage image);
    void updateImageLabel(QString string);
    void enableNextImageButton(bool b);
    void enablePreviousImageButton(bool b);
    void enableRemoveImageButton(bool b);
    void updatePlayPauseAnimationButtonText(QString string);
    void setUpProject(int width, int height);
    void createMessagePopup(QString message1, QString message2);

public slots:
    void userSetNewColor(QColor color);
    void setDimensions(int width, int height);
    void setCanvasDimensions(int width, int height);
    void setCurrentTool(std::string tool);
    void pencilClick(QPoint pos);
    void eraserClick(QPoint pos);
    void colorDropperClick(QPoint pos);
    void selectTool(QPoint pos);
    void addImage();
    void duplicateImage();
    void removeImage();
    void nextImage();
    void previousImage();
    void playPauseClicked();
    void fpsSliderChanged(int fps);
    void saveSpriteJson(QString fileName);
    void writeJson();
    void loadSpriteJson(QString fileName);
    void setInitialButtonColors();

private:
    std::vector<QImage> images;
    int currentImage;
    int numberOfImages;
    QColor primaryColor;
    QColor secondaryColor;
    int currentColor;
    int imageHeight;
    int imageWidth;
    int canvasHeight;
    int canvasWidth;
    std::string currentTool;
    void playNextAnimationFrame();
    bool animationPlaying;
    int currentAnimationFrame;
    int msPerFrame;
    void updateCurrentFrameLabel();
    QJsonObject spriteJson;
};
#endif // MODEL_H
