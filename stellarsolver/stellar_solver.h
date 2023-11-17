#pragma once

#include <QObject>
#include <QImage>
#include "stellarsolver/stellarsolver.h"
#include "stellarsolver/classdefinitions.h"

class Stellar_Solver : public StellarSolver
{
     Q_OBJECT

public:
     static const QString IMAGE_ID;
private:
     QImage solverImage;
public:
     /**
      * @brief StellarSolver This constructor makes a StellarSolver without any image or information
      * @param parent The parent of this StellarSolver, allows it to be deleted with its parent, defaults to nullptr
      */
     explicit Stellar_Solver(QObject *parent = nullptr);
     /**
      * @brief StellarSolver This constructor makes a StellarSolver with just the image information
      * @param imagestats Information about the imagebuffer provided
      * @param imageBuffer The imagebuffer to be processed
      * @param parent The parent of this StellarSolver, allows it to be deleted with its parent, defaults to nullptr
      */
     explicit Stellar_Solver(const FITSImage::Statistic &imagestats, uint8_t const *imageBuffer, QObject *parent = nullptr);
     /**
      * @brief StellarSolver This constructor makes a StellarSolver with the ProcessType included
      * @param type The type of process you are planning to perform on the image
      * @param imagestats Information about the imagebuffer provided
      * @param imageBuffer The imagebuffer to be processed
      * @param parent The parent of this StellarSolver, allows it to be deleted with its parent, defaults to nullptr
      */
     explicit Stellar_Solver(ProcessType type, const FITSImage::Statistic &imagestats, uint8_t const *imageBuffer, QObject *parent = nullptr);
     // This deletes the StellarSolver
     ~Stellar_Solver();
     /**
      * @brief loadNewImageBuffer loads a new image buffer for StellarSolver to process
      * @param file The image file
      * @return whether or not it succesfully loaded a new image.  It will not be successful if you try to load a null image buffer or if a process is running.
      */
     Q_INVOKABLE bool loadNewImageBuffer(const QString file);
     /**
      * @brief create uuid
      * @return uuid 
      */
     Q_INVOKABLE QString generateGUID();
     /**
      * @brief remove file
      * @param file The image file
      * @return whether or not it succesfully removed file.
      */
     Q_INVOKABLE bool removeFile(const QString file);
     /**
      * @brief solve image size
      * @return image size
      */
     Q_INVOKABLE QSize solveImageSize() const;
     /**
      * @brief extract Performs Star Extraction on the image.
      * @param calculateHFR If true, it will also calculated Half-Flux Radius for each detected star. HFR calculations can be very CPU-intensive.
      * @param profileIndex profile index 0 ~ 7
      * @param color color channel
      * @return whether or not it succesfully
      */
     Q_INVOKABLE bool extractImage(const bool calculateHFR = false, const size_t profileIndex = 4, const FITSImage::ColorChannel color = FITSImage::GREEN);
     /**
      * @brief solve Plate Solves the image.
      * @param scale If true, set the Search Scale range to speed up the solver based on the ScaleUnits QString
      * @param scale_low The low end of the search range in the specified units
      * @param scale_height The high end of the search range in the specified units
      * @param scale_units The specified units for the range.
      * @param position If true, sets the search RA/DEC/Radius to speed up the solver when plate solving
      * @param position_ra The Right Ascension in decimal hours
      * @param position_dec The Declination in decimal degrees
      * @param profileIndex profileIndex profile index 0 ~ 7
      * @param color color color channel
      * @return whether or not it succesfully
      */
     Q_INVOKABLE bool solveImage(const bool scale = false, const double scale_low = 0.0, const double scale_height = 0.0, const ScaleUnits scale_units = SSolver::DEG_WIDTH, const bool position = false, const double position_ra = 0.0, const double position_dec = 0.0, const size_t profileIndex = 4, const FITSImage::ColorChannel color = FITSImage::GREEN);
     /**
     * @brief abort will abort the Star Extraction or Plate Solving process and wait synchronously till that is done.
     */
     Q_INVOKABLE void stop();
     /**
      * @brief get solve image ra/dec
      * @param px pixel x
      * @param py pixel y
      * @return ra/dec
      */
     Q_INVOKABLE QPointF solveImageRaDec(double px, double py);
     /**
      * @brief to HMS String
      * @param points
      * @return ra/dec hh:mm:ss string
      */
     Q_INVOKABLE QList<QString> toHMSString(QPointF points);
private:
     void init();
     void registerMetaTypes();
     void waitingRunning();
     QImage requestImage(const QString &id);

private Q_SLOTS:
     void extractorComplete();
     void solverComplete();

Q_SIGNALS:
     void extractorSuccess(QList<Star*> stars);
     void extractorFail();
     void solverSuccess(Solution* sulution, QList<Star*> stars);
     void solverFail();
};
