#include <QWidget>

class Scope;
class QSlider;
class QLabel;
class ScopeDataSource;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget * = NULL);

    void start();

signals:
    void signalIntervalChanged(double);
    void signalAfeOffsetChanged(double);
    void signalAfeFilterChanged(double);
    void signalAfeAttenuationChanged(double);
    void signalAfeTriggerChanged(double);

private:
    QSlider *d_afeOffsetSlider;
    QLabel  *d_afeOffsetLabel;
    QSlider *d_afeFilterSlider;
    QLabel  *d_afeFilterLabel;
    QSlider *d_afeAttenuationSlider;
    QLabel  *d_afeAttenuationLabel;
    QSlider *d_afeTriggerSlider;
    QLabel  *d_afeTriggerLabel;

    Scope *d_scope;
    ScopeDataSource *d_scopeData;
};
