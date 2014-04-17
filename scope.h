#include <QWidget>
#include <QTimer>

class ScopeDataSource;
class QImage;

class Scope : public QWidget
{
    Q_OBJECT

public:
    Scope(QWidget *parent = NULL);
    virtual ~Scope();
    enum samplingMode {
        autoSampling,
        singleShot,
        triggerSampling,
        samplingOff,
    };
    void setDataSource(ScopeDataSource *source);

public slots:
    void setMode(enum samplingMode);
    void setSamplingFrequency(quint32 nsec);
    void setRedrawFrequency(quint32 msec);
    void doResample(void);
    void gotScopeData(const QByteArray channel1, const QByteArray channel2);

protected:
    void resizeEvent(QResizeEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    ScopeDataSource *dataSource;
    int myPenWidth;
    QColor myPenColor;
    QTimer *resampleTimer;
    quint32 redrawInterval;
    QImage *backBuffer;
    enum samplingMode currentMode;
};
