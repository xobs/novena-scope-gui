#include <QDebug>
#include <QPainter>
#include <QPaintEvent>
#include <QImage>
#include "scope.h"
#include "scopedatasource.h"

Scope::Scope(QWidget *parent):
    QWidget(parent)
{
    setAttribute(Qt::WA_StaticContents);
    myPenWidth = 1;
    myPenColor = Qt::blue;

    dataSource = NULL;

    resampleTimer = new QTimer(this);
    connect(resampleTimer, SIGNAL(timeout()),
            this,          SLOT(doResample()));

    backBuffer = new QImage();
}

Scope::~Scope()
{
    delete resampleTimer;
}

void Scope::setDataSource(ScopeDataSource *source)
{
    if (dataSource)
        dataSource->disconnect(this, SLOT(gotScopeData(const QByteArray, const QByteArray)));

    dataSource = source;
    connect(dataSource, SIGNAL(scopeData(const QByteArray, const QByteArray)),
            this,       SLOT(gotScopeData(const QByteArray, const QByteArray)));
}

void Scope::setMode(enum samplingMode mode)
{
    currentMode = mode;
    if (currentMode == autoSampling)
        resampleTimer->start(redrawInterval);
}

void Scope::setSamplingFrequency(quint32 nsec)
{
}

void Scope::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QRect dirtyRect = event->rect();
    painter.drawImage(dirtyRect, *backBuffer, dirtyRect);
}

void Scope::resizeEvent(QResizeEvent *event)
{
    if (width() > backBuffer->width() || height() > backBuffer->height()) {
        int newWidth = qMax(width() + 128, backBuffer->width());
        int newHeight = qMax(height() + 128, backBuffer->height());
        delete backBuffer;
        backBuffer = new QImage(newWidth, newHeight, QImage::Format_RGB32);
        update();
    }
    QWidget::resizeEvent(event);
}

void Scope::setRedrawFrequency(quint32 msec)
{
    redrawInterval = msec;
}

void Scope::doResample(void)
{
    dataSource->getData(4096);
}

void Scope::gotScopeData(const QByteArray channel1, const QByteArray channel2)
{
    int i;
    int lastY;
    int samples;
    QPainter painter(backBuffer);

    backBuffer->fill(Qt::black);
    painter.setPen(QPen(myPenColor, myPenWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    lastY = channel1.at(0);
    samples = channel1.size();
    for (i = 1; i < samples; i++) {
        int thisY = channel1.at(i);
        painter.drawLine(i - 1, lastY, i, thisY);
        lastY = thisY;
    }

    /*
    painter.setPen(QPen(Qt::red, myPenWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    lastY = channel2.at(0);
    samples = channel2.size();
    for (i = 1; i < samples; i++) {
        int thisY = channel2.at(i);
        painter.drawLine(i - 1, lastY, i, thisY);
        lastY = thisY;
    }
    */

    if (currentMode == autoSampling)
        resampleTimer->start(redrawInterval);

    update();
    return;
}
