/* Tarnished Pixels
 * CS3505 Spring 2022
 * Assignment 7: Sprite Editor Implementation
*/

#include "model.h"
#include <QTimer>
#include <iostream>
#include <QFileDialog>

/**
 * @brief Model::Model - Constructor for a model object.
 * @param parent
 */
Model::Model(QObject *parent)
    : QObject{parent}
{
    currentImage = 0;
    numberOfImages = 1;
    currentColor = 1;
    primaryColor = QColor("blue");
    secondaryColor = QColor("red");
    imageHeight = 0;
    imageWidth = 0;
    currentTool = "pencil";
    animationPlaying = false;
    currentAnimationFrame = 0;
    msPerFrame = 500;
}


/**
 * @brief Model::writeJson - This method writes the JSON file for the sprite editor.
 * It records the width and height of the frames, the number of frames, and the color
 * info for each pixel of each frame.
 */
void Model::writeJson()
{
    // These are essentially key value pairs
    spriteJson.insert("height", imageHeight);
    spriteJson.insert("width", imageWidth);
    spriteJson.insert("numberOfFrames", numberOfImages);

    QJsonObject frames;
    // Accesses each frame in the vector of images
    for(int imageIndex = 0; imageIndex < numberOfImages; imageIndex++)
    {
        QJsonArray rows;
        // Accesses each row of pixels in an image
        for(int rowIndex = 0; rowIndex < imageHeight; rowIndex++)
        {
            QJsonArray pixels;
            // Accesses each pixel in a particular row of an image
            for(int pixelIndex = 0; pixelIndex < imageWidth; pixelIndex++)
            {
                QJsonArray colors;
                // Retrieves the color of a particular pixel and saves its r,g,b,a values
                QColor currentPixelColor = images[imageIndex].pixelColor(pixelIndex, rowIndex);
                colors.push_back(currentPixelColor.red());
                colors.push_back(currentPixelColor.green());
                colors.push_back(currentPixelColor.blue());
                colors.push_back(currentPixelColor.alpha());
                // Each pixels index holds an array of r,g,b,a color values
                pixels.push_back(colors);
            }
            // Each rows index holds an array of each pixel in that row
            rows.push_back(pixels);
        }
        // Each frames index holds an array of rows that describe an image
        frames.insert("frame" + QString::number(imageIndex), rows);
    }
    spriteJson.insert("frames", frames);
}


/**
 * @brief Model::saveSpriteJson - This method creates the JSON and saves the file as a .ssp.
 */
void Model::saveSpriteJson(QString fileName)
{
    writeJson();
    QFile saveFile(fileName);
    if (!saveFile.open(QIODevice::WriteOnly))
    {
        qWarning("Couldn't open save file.");
        return;
    }
    QJsonDocument saveDoc(spriteJson);
    saveFile.write(saveDoc.toJson());
}


/**
 * @brief Model::loadSpriteJson - Reads and creates the images of the selected file.
 * Emits signals to update the UI to reflect the loaded file's images.
 */
void Model::loadSpriteJson(QString fileName)
{
    try
    {
        QFile loadFile(fileName);
        if (!loadFile.open(QIODevice::ReadOnly))
        {
            qWarning("Couldn't open load file.");
        }

        // Reads the saved sprite data from the given file and converts the document into an object so it may be accessed.
        QByteArray saveData = loadFile.readAll();
        QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
        QJsonObject savedData = loadDoc.object();

        // Reads the key value pairs from the saved data object into their rightful data members.
        QJsonValue height = savedData.value("height");
        imageHeight = height.toInt();
        QJsonValue width = savedData.value("width");
        imageWidth = width.toInt();
        QJsonValue numberOfFrames = savedData.value("numberOfFrames");
        numberOfImages = numberOfFrames.toInt();

        QJsonObject frames = savedData.value("frames").toObject();
        images.clear();

        // Accesses each frame in the vector of images
        for(int imageIndex = 0; imageIndex < numberOfImages; imageIndex++)
        {
            // Creates a new base image to be drawn on
            QImage image = QImage(imageWidth, imageHeight, QImage::Format_ARGB32);
            images.push_back(image);

            QString rowName = "frame" + QString::number(imageIndex);
            QJsonArray rows = frames.value(rowName).toArray();

            // Accesses each row of pixels in an image
            for(int rowIndex = 0; rowIndex < imageHeight; rowIndex++)
            {
                QJsonArray pixels = rows.at(rowIndex).toArray();
                // Accesses each pixel in a particular row of an image
                for(int pixelIndex = 0; pixelIndex < imageWidth; pixelIndex++)
                {
                    QJsonArray colors = pixels.at(pixelIndex).toArray();
                    int red = colors.at(0).toInt();
                    int green = colors.at(1).toInt();
                    int blue = colors.at(2).toInt();
                    int alpha = colors.at(3).toInt();
                    QColor currentPixelColor(red, green, blue, alpha);
                    // Draws each pixel in the loaded image with the read color data
                    images[imageIndex].setPixelColor(pixelIndex, rowIndex, currentPixelColor);
                }
            }
        }

        // Emits signals to the UI to reflect the images of the loaded file rather than its current state.
        emit updateMainCanvas(images.at(0));
        emit updatePreviewWindow(images.at(0));
        emit setUpProject(imageWidth, imageHeight);
        emit enablePreviousImageButton(false);
        currentImage = 0;
        if (numberOfImages > 1)
        {
            emit enableNextImageButton(true);
            emit enableRemoveImageButton(true);
            updateCurrentFrameLabel();
        }
    }
    catch(...)
    {
        emit createMessagePopup(QString("Error"), QString("Error occured when reading in your saved file.\nMake sure its contents are properly formatted."));
    }
}


/**
 * @brief Model::setCurrentTool - Sets the current tool to be the pencil, eraser, or color dropper tool.
 * @param tool - The pencil, eraser, or color dropper tool.
 */
void Model::setCurrentTool(std::string tool)
{
    currentTool = tool;
}


/**
 * @brief Model::selectTool - Decides which tool's function to use based on the currentTool.
 * @param position - A QPoint of the position of the selected pixel in the main canvas.
 */
void Model::selectTool(QPoint position)
{
    if (currentTool == "pencil")
    {
        pencilClick(position);
    }
    else if (currentTool == "eraser")
    {
        eraserClick(position);
    }
    else
    {
        colorDropperClick(position);
    }
}


/**
 * @brief Model::pencilClick - Updates the image to set the color of the selected pixel to be the
 * primary or secondary color depending on which color is currently selected.
 * @param position - A QPoint of the position of the selected pixel in the main canvas.
 */
void Model::pencilClick(QPoint position)
{
    int xPos = (int) (((float) position.x()/canvasWidth)*imageWidth);
    int yPos = (int) (((float) position.y()/canvasHeight)*imageHeight);
    if(currentColor == 1)
    {
        images[currentImage].setPixelColor(xPos, yPos, primaryColor);
    }
    if(currentColor == 2)
    {
        images[currentImage].setPixelColor(xPos, yPos, secondaryColor);
    }

    emit updateMainCanvas(images[currentImage]);
    if(!animationPlaying)
    {
        emit updatePreviewWindow(images[currentImage]);
    }
}


/**
 * @brief Model::eraserClick - Updates the image to set the selected pixel to be transparent.
 * @param position - A QPoint of the position of the selected pixel in the main canvas.
 */
void Model::eraserClick(QPoint position)
{
    int xPos = (int) (((float) position.x()/canvasWidth)*imageWidth);
    int yPos = (int) (((float) position.y()/canvasHeight)*imageHeight);
    images[currentImage].setPixelColor(xPos, yPos, QColor("transparent"));

    emit updateMainCanvas(images[currentImage]);
    if(!animationPlaying)
    {
        emit updatePreviewWindow(images[currentImage]);
    }
}


/**
 * @brief Model::colorDropperClick - If currentColor = 1, sets primaryColor to be the color of the selected pixel.
 * If currentColor = 2, sets secondaryColor to be the color of the selected pixel.
 * @param position - A QPoint of the position of the selected pixel in the main canvas.
 */
void Model::colorDropperClick(QPoint position)
{
    int xPos = (int) (((float) position.x()/canvasWidth)*imageWidth);
    int yPos = (int) (((float) position.y()/canvasHeight)*imageHeight);
    QColor clickedPixelColor = images[currentImage].pixelColor(xPos, yPos);
    if (clickedPixelColor != Qt::transparent)
    {
        userSetNewColor(clickedPixelColor);
    }
}


/**
 * @brief Model::createFirstImage - Creates the default canvas to be painted on.
 * @param width - width of image
 * @param height - height of image
 */
void Model::createFirstImage(int width, int height)
{
    //store the values for use on the latter frames
    imageWidth = width;
    imageHeight = height;

    QImage image = QImage(width, height, QImage::Format_ARGB32);
    image.fill(QColor(0, 0, 0, 0));
    images.push_back(image);
    currentImage = 0;
    updateCurrentFrameLabel();

    emit updateMainCanvas(image);
    emit updatePreviewWindow(image);
}


/**
 * @brief Model::setCurrentColor - tells the model which color the user is currently using
 * by passing in either a 1 or a 2 from the view
 * @param whichColor - an int
 */
void Model::setCurrentColor(int whichColor)
{
    currentColor = whichColor;
}


/**
 * @brief Model::userSetNewColor - updates the primary and secondary colors in the model
 * when a user selects a new color in the view
 * @param color - the user's new QColor
 */
void Model::userSetNewColor(QColor color)
{
    if(currentColor == 1)
    {
        primaryColor = color;
    }
    if(currentColor == 2)
    {
        secondaryColor = color;
    }

    //emits a signal to the view to add a border to and change the color a color button
    emit updateButtonColor(color, currentColor);
}


/**
 * @brief Model::setDimensions - sets up the canvases based in the inputed width and height. Creates a default
 * empty image to be edited and displayed.
 * @param width - sprite width in pixels
 * @param height - sprite height in pixels
 */
void Model::setDimensions(int width, int height)
{
    imageWidth = width;
    imageHeight = height;
    createFirstImage(width, height);

    emit setUpProject(width, height);
}


/**
 * @brief Model::setCanvasDimensions - takes in and saves the view's main canvas dimensions.
 * @param width - canvas width
 * @param height - canvas height
 */
void Model::setCanvasDimensions(int width, int height)
{
    canvasWidth = width;
    canvasHeight = height;
}


/**
 * @brief Model::addImage - adds a new image to the sprite editor
 */
void Model::addImage()
{
    if(animationPlaying)
    {
        playPauseClicked();
    }

    QImage newImage = QImage(imageWidth, imageHeight, QImage::Format_ARGB32);
    newImage.fill(QColor(0, 0, 0, 0));
    currentImage++;
    images.insert(images.begin()+currentImage, newImage);

    emit updateMainCanvas(images[currentImage]);
    if(!animationPlaying)
    {
        emit updatePreviewWindow(images[currentImage]);
    }

    numberOfImages++;
    updateCurrentFrameLabel();
    emit enablePreviousImageButton(true);
    emit enableRemoveImageButton(true);
}


/**
 * @brief Model::duplicateImage - creates a duplicate of the current image
 */
void Model::duplicateImage()
{
    if(animationPlaying)
    {
        playPauseClicked();
    }

    QImage newImage = images[currentImage].copy();
    currentImage++;
    images.insert(images.begin()+currentImage, newImage);

    emit updateMainCanvas(images[currentImage]);
    if(!animationPlaying)
    {
        emit updatePreviewWindow(images[currentImage]);
    }

    numberOfImages++;
    updateCurrentFrameLabel();
    emit enablePreviousImageButton(true);
    emit enableRemoveImageButton(true);
}


/**
 * @brief Model::removeImage - removes an image from the vector of frames that make up the sprite
 */
void Model::removeImage()
{
    if(animationPlaying)
    {
        playPauseClicked();
    }

    if(numberOfImages != 1)
    {
        images.erase(std::next(images.begin(), currentImage));
        numberOfImages--;
        if(currentImage != 0)
        {
            currentImage--;
            if(currentImage == 0)
            {
                emit enablePreviousImageButton(false);
            }
        }
        updateCurrentFrameLabel();

        emit updateMainCanvas(images[currentImage]);
        emit updatePreviewWindow(images[currentImage]);
        if(numberOfImages == 1)
        {
            emit enableRemoveImageButton(false);
            emit enablePreviousImageButton(false);
            emit enableNextImageButton(false);
        }
    }
}


/**
 * @brief Model::nextImage - iterates to the next image in the vector
 */
void Model::nextImage()
{
    if(currentImage != numberOfImages-1)
    {
        currentImage++;
        updateCurrentFrameLabel();

        emit updateMainCanvas(images[currentImage]);
        if(!animationPlaying)
        {
            emit updatePreviewWindow(images[currentImage]);
        }
        emit enablePreviousImageButton(true);
    }
    if(currentImage == numberOfImages-1)
    {
        emit enableNextImageButton(false);
    }
}


/**
 * @brief Model::previousImage - moves to the previous image in the vector
 */
void Model::previousImage()
{
    if(currentImage != 0)
    {
        currentImage--;
        updateCurrentFrameLabel();

        emit updateMainCanvas(images[currentImage]);
        emit enableNextImageButton(true);
        if(!animationPlaying)
        {
            emit updatePreviewWindow(images[currentImage]);
        }
    }
    if(currentImage == 0)
    {
        emit enablePreviousImageButton(false);
    }
}


/**
 * @brief Model::setInitialButtonColors - Sets the primary and secondary colors to the default colors red and blue.
 */
void Model::setInitialButtonColors()
{
    currentColor = 1;
    primaryColor = QColor("blue");
    secondaryColor = QColor("red");

    emit updateButtonColor(primaryColor, 1);
    emit updateButtonColor(secondaryColor, 2);
}


/**
 * @brief Model::updateCurrentFrameLabel - sends a signal to the view to update the frame label with current info
 */
void Model::updateCurrentFrameLabel()
{
    QString s = "Frame " + QString::number(currentImage+1) + " of " + QString::number(numberOfImages);
    emit updateImageLabel(s);
}


/**
 * @brief Model::playPauseClicked - slot to toggle play/pause, changes text and begins animation.
 */
void Model::playPauseClicked()
{
    if(animationPlaying)
    {
        animationPlaying = false;

        emit updatePreviewWindow(images[currentImage]);
        emit updatePlayPauseAnimationButtonText("Play");
    }
    else
    {
        animationPlaying = true;
        QTimer::singleShot(500, this, &Model::playNextAnimationFrame);
        currentAnimationFrame = currentImage;

        emit updatePlayPauseAnimationButtonText("Pause");
    }
}


/**
 * @brief Model::playNextAnimationFrame - recursive method to play the next frame of the animation. If animation is stopped, resets to current frame.
 */
void Model::playNextAnimationFrame()
{
    if(animationPlaying)
    {
        emit updatePreviewWindow(images[currentAnimationFrame]);
        currentAnimationFrame = (currentAnimationFrame + 1) % images.size();
        QTimer::singleShot(msPerFrame, this, &Model::playNextAnimationFrame);
    }
}


/**
 * @brief Model::fpsSliderChanged - slot to handel changes to the requested fps.
 * @param fps - updated desired frames per second.
 */
void Model::fpsSliderChanged(int fps)
{
    msPerFrame = 1000 / fps;
}
