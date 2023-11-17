#include <memory>
#include <QDir>
#include <QFile>
#include <QUuid>
#if defined(Q_OS_IOS)
#include <cstring>
#endif
#include "ssolverutils/fileio.h"
extern "C" {
#include "astrometry/starutil.h"
}
#include "stellar_solver.h"

namespace SW {
    extern std::string CppNSHomeDirectory();
}

const QString Stellar_Solver::IMAGE_ID = "stellar_solver_image";

Stellar_Solver::Stellar_Solver(QObject *parent) : StellarSolver(parent)
{
    init();
}

Stellar_Solver::Stellar_Solver(const FITSImage::Statistic &imagestats, uint8_t const *imageBuffer, QObject *parent) : StellarSolver(imagestats, imageBuffer, parent)
{
    init();
}

Stellar_Solver::Stellar_Solver(ProcessType type, const FITSImage::Statistic &imagestats, const uint8_t *imageBuffer, QObject *parent) : StellarSolver(type, imagestats, imageBuffer, parent)
{
    init();
}

Stellar_Solver::~Stellar_Solver()
{
}

void Stellar_Solver::init()
{
    registerMetaTypes();
    QStringList dirs;
#if defined(Q_OS_ANDROID)
    dirs << "assets:/fits";
#else
    dirs << "fits";
#endif
    QStringList fitFiles = this->getIndexFiles(dirs);
#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
#if defined(Q_OS_ANDROID)
    QString copyFitsDirPath = "./fits";
#elif defined(Q_OS_IOS)
    QString documents = QString::fromUtf8((SW::CppNSHomeDirectory() + std::string("/Documents")).c_str());
    QString copyFitsDirPath = documents + "/fits";
#endif
    QDir copyFitsDir(copyFitsDirPath);
    QStringList copyFitFiles;
    if (!copyFitsDir.exists())
    {
        copyFitsDir.mkpath(copyFitsDirPath);
    }
    for (const QString &file : fitFiles)
    {
        QFile fitFile = QFile(file);
        QString fileName = QFileInfo(file).fileName();
        QString copyFitFileName = copyFitsDirPath + "/" + fileName;
        QFile copyFitFile(copyFitFileName);
        if (!copyFitFile.exists())
        {
            fitFile.copy(copyFitFileName);
            copyFitFile.setPermissions(QFile::ReadOwner);
        }
        copyFitFiles << copyFitFileName;
    }
    fitFiles = copyFitFiles;
#endif
    this->clearIndexFileAndFolderPaths();
    this->setIndexFilePaths(fitFiles);
}

void Stellar_Solver::registerMetaTypes()
{
    qRegisterMetaType<SSolver::SolverType>("SSolver::SolverType");
    qRegisterMetaType<SSolver::ProcessType>("SSolver::ProcessType");
    qRegisterMetaType<SSolver::ExtractorType>("SSolver::ExtractorType");
    qRegisterMetaType<FITSImage::ColorChannel>("FITSImage::ColorChannel");
    qRegisterMetaType<SSolver::ScaleUnits>("SSolver::ScaleUnits");
    qRegisterMetaType<FITSImage::Parity>("FITSImage::Parity");
    qRegisterMetaType<Solution *>("Solution*");
    qRegisterMetaType<QList<Star *>>("QList<Star*>");
}

QImage Stellar_Solver::requestImage(const QString &id)
{
    return this->solverImage;
}

bool Stellar_Solver::loadNewImageBuffer(const QString file)
{
    bool ret = false;
    fileio imageLoader;
    imageLoader.logToSignal = true;
    connect(&imageLoader, &fileio::logOutput, this, [&](QString logText) {
        Q_EMIT this->logOutput(logText);
    });

    //#if defined(Q_OS_ANDROID)
    //    QString copyFitsDirPath = "./fits";
    //#elif defined(Q_OS_IOS)
    //    QString documents = QString::fromUtf8(
    //        (SW::CppNSHomeDirectory() + std::string("/Documents")).c_str());
    //    QString copyFitsDirPath = documents + "/fits";
    //#endif
    //    QDir copyFitsDir(copyFitsDirPath);
    //    QStringList copyFitFiles;
    //    if (!copyFitsDir.exists()) {
    //        copyFitsDir.mkpath(copyFitsDirPath);
    //    }
    //    QFile fitFile = QFile("./fits/test.png");
    //    QString copyFitFileName = copyFitsDirPath + "/" + "test.png";
    //    QFile copyFitFile(copyFitFileName);
    //    if (!copyFitFile.exists()) {
    //        fitFile.copy(copyFitFileName);
    //        copyFitFile.setPermissions(QFile::ReadOwner);
    //    }

    if (imageLoader.loadImage(file)) {
        ret = StellarSolver::loadNewImageBuffer(imageLoader.getStats(),
                                                imageLoader.getImageBuffer());
        if (ret) {
            this->solverImage.load(file);
        }
    }
    return ret;
}

QString Stellar_Solver::generateGUID()
{
    return QUuid::createUuid().toString().replace("{", "").replace("}", "");
}

bool Stellar_Solver::removeFile(const QString file)
{
    bool ret = false;
    if (QFile::exists(file))
    {
        ret = QFile::remove(file);
    }
    return ret;
}

QSize Stellar_Solver::solveImageSize() const
{
    return this->solverImage.size();
}

bool Stellar_Solver::extractImage(const bool calculateHFR, const size_t profileIndex, const FITSImage::ColorChannel color)
{
    bool ret = false;
    Parameters profile = this->getBuiltInProfiles().at(profileIndex >= 0 && profileIndex < this->getBuiltInProfiles().size() ? profileIndex : 4);
    waitingRunning();
    StellarSolver::setProperty("ProcessType", !calculateHFR ? EXTRACT : EXTRACT_WITH_HFR);
    StellarSolver::setProperty("ExtractorType", EXTRACTOR_INTERNAL);
    this->setParameters(profile);
    this->clearSubFrame();
    this->setColorChannel(color);
    connect(this, &StellarSolver::ready, this, &Stellar_Solver::extractorComplete);
    this->start();
    return ret;
}

bool Stellar_Solver::solveImage(const bool scale, const double scale_low, const double scale_height, const ScaleUnits scale_units, const bool position, const double position_ra, const double position_dec, const size_t profileIndex, const FITSImage::ColorChannel color)
{
    bool ret = false;
    Parameters profile = this->getBuiltInProfiles().at(profileIndex >= 0 && profileIndex < this->getBuiltInProfiles().size() ? profileIndex : 4);
    waitingRunning();
    StellarSolver::setProperty("ProcessType", SOLVE);
    StellarSolver::setProperty("ExtractorType", EXTRACTOR_INTERNAL);
    StellarSolver::setProperty("SolverType", SOLVER_STELLARSOLVER);
    this->setParameters(profile);
    this->clearSubFrame();
    this->setColorChannel(color);
    StellarSolver::setProperty("UseScale", scale);
    if (scale)
    {
        this->setSearchScale(scale_low, scale_height, scale_units);
    }
    StellarSolver::setProperty("UsePosition", position);
    if (position)
    {
        this->setSearchPositionRaDec(position_ra, position_dec);
    }
    connect(this, &StellarSolver::ready, this, &Stellar_Solver::solverComplete);
    this->start();
    return ret;
}

void Stellar_Solver::extractorComplete()
{
    disconnect(this, &StellarSolver::ready, this, &Stellar_Solver::extractorComplete);
    if (!this->failed() && this->extractionDone())
    {
        Q_EMIT logOutput("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
        if (this->isCalculatingHFR())
            Q_EMIT logOutput(QString(this->getCommandString() + " with HFR success! Got %1 stars").arg(this->getStarList().size()));
        else
            Q_EMIT logOutput(QString(this->getCommandString() + " success! Got %1 stars").arg(this->getStarList().size()));
        Q_EMIT logOutput("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
        QList<Star *> stars;
        for (FITSImage::Star star : this->getStarList())
        {
            stars << new Star(star);
        }
        Q_EMIT extractorSuccess(stars);
        qDeleteAll(stars);
        stars.clear();
    }
    else
    {
        Q_EMIT logOutput("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
        Q_EMIT logOutput(QString(this->getCommandString() + "failed."));
        Q_EMIT logOutput("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
        Q_EMIT extractorFail();
    }
}

void Stellar_Solver::solverComplete()
{
    disconnect(this, &StellarSolver::ready, this, &Stellar_Solver::solverComplete);
    if (!this->failed() && this->solvingDone())
    {
        Q_EMIT logOutput("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
        Q_EMIT logOutput(QString(this->getCommandString() + " success!"));
        Q_EMIT logOutput("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
        QList<Star *> stars;
        for (FITSImage::Star star : this->getStarListFromSolve())
        {
            stars << new Star(star);
        }
        Q_EMIT solverSuccess(std::make_shared<Solution>(this->getSolution()).get(), stars);
        qDeleteAll(stars);
        stars.clear();
    }
    else
    {
        Q_EMIT logOutput("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
        Q_EMIT logOutput(QString(this->getCommandString() + "failed."));
        Q_EMIT logOutput("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
        Q_EMIT solverFail();
    }
}

void Stellar_Solver::stop()
{
    this->abortAndWait();
}

QPointF Stellar_Solver::solveImageRaDec(double px, double py)
{
    QPointF pixels(px, py);
    FITSImage::wcs_point point;
    return this->getWCSData().pixelToWCS(pixels, point) ? QPointF(point.ra, point.dec) : QPointF(-1, -1);
}

QList<QString> Stellar_Solver::toHMSString(QPointF points)
{
    char rastr[32], decstr[32];
    ra2hmsstring(points.x(), rastr);
    dec2dmsstring(points.y(), decstr);
    return QList<QString>{rastr, decstr};
}

void Stellar_Solver::waitingRunning()
{
    while (this->isRunning())
    {
        qApp->processEvents();
        QThread::sleep(0);
    }
}
