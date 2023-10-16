#pragma once

#include <QObject>
#include "stellarsolver/structuredefinitions.h"

class Star : public QObject
{
    Q_OBJECT
    Q_PROPERTY(float x MEMBER x)
    Q_PROPERTY(float y MEMBER y)
    Q_PROPERTY(float mag MEMBER mag)
    Q_PROPERTY(float flux MEMBER flux)
    Q_PROPERTY(float peak MEMBER peak)
    Q_PROPERTY(float HFR MEMBER HFR)
    Q_PROPERTY(float a MEMBER a)
    Q_PROPERTY(float b MEMBER b)
    Q_PROPERTY(float theta MEMBER theta)
    Q_PROPERTY(float ra MEMBER ra)
    Q_PROPERTY(float dec MEMBER dec)
    Q_PROPERTY(int numPixels MEMBER numPixels)

public:
    explicit Star(QObject *parent = nullptr) : QObject(parent)
    {
    }
    explicit Star(FITSImage::Star star, QObject *parent = nullptr) : QObject(parent)
    {
        this->x = star.x;
        this->y = star.y;
        this->mag = star.mag;
        this->flux = star.flux;
        this->peak = star.peak;
        this->HFR = star.HFR;
        this->a = star.a;
        this->b = star.b;
        this->theta = star.theta;
        this->ra = star.ra;
        this->dec = star.dec;
        this->numPixels = star.numPixels;
    }
    ~Star()
    {
    }

private:
    float x;       // The x position of the star in Pixels
    float y;       // The y position of the star in Pixels
    float mag;     // The magnitude of the star, note that this is a relative magnitude based on the star extraction options.
    float flux;    // The calculated total flux
    float peak;    // The peak value of the star
    float HFR;     // The half flux radius of the star
    float a;       // The semi-major axis of the star
    float b;       // The semi-minor axis of the star
    float theta;   // The angle of orientation of the star
    float ra;      // The right ascension of the star
    float dec;     // The declination of the star
    int numPixels; // The number of pixels occupied by the star in the image.
};

class Solution : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double fieldWidth MEMBER fieldWidth)
    Q_PROPERTY(double fieldHeight MEMBER fieldHeight)
    Q_PROPERTY(double ra MEMBER ra)
    Q_PROPERTY(double dec MEMBER dec)
    Q_PROPERTY(double orientation MEMBER orientation)
    Q_PROPERTY(double pixscale MEMBER pixscale)
    Q_PROPERTY(FITSImage::Parity parity MEMBER parity)
    Q_PROPERTY(double raError MEMBER raError)
    Q_PROPERTY(double decError MEMBER decError)

public:
    explicit Solution(QObject *parent = nullptr) : QObject(parent)
    {
    }
    explicit Solution(FITSImage::Solution solution, QObject *parent = nullptr) : QObject(parent)
    {
        this->fieldWidth = solution.fieldWidth;
        this->fieldHeight = solution.fieldHeight;
        this->ra = solution.ra;
        this->dec = solution.dec;
        this->orientation = solution.orientation;
        this->pixscale = solution.pixscale;
        this->parity = solution.parity;
        this->raError = solution.raError;
        this->decError = solution.decError;
    }
    ~Solution()
    {
    }

private:
    double fieldWidth;        // The calculated width of the field in arcminutes
    double fieldHeight;       // The calculated height of the field in arcminutes
    double ra;                // The Right Ascension of the center of the field in degrees
    double dec;               // The Declination of the center of the field in degrees
    double orientation;       // The orientation angle of the image from North in degrees
    double pixscale;          // The pixel scale of the image in arcseconds per pixel
    FITSImage::Parity parity; // The parity of the solved image. (Whether it has been flipped)  JPEG images tend to have negative parity while FITS files tend to have positive parity.
    double raError;           // The error between the search_ra position and the solution ra position in arcseconds
    double decError;          // The error between the search_dec position and the solution dec position in arcseconds
};
