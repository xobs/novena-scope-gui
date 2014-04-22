#include <QWidget>

class Scope;
class QSlider;
class QLabel;
class QCheckBox;
class ScopeDataSource;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget * = NULL);

    void start();

public slots:
    void processCheckbox(int state);

private:
    QLabel *d_channel1Label;
    QLabel *d_channel2Label;

    QSlider *d_afeOffsetCh1Slider;
    QLabel  *d_afeOffsetCh1Label;
    QSlider *d_afeFilterCh1Slider;
    QLabel  *d_afeFilterCh1Label;
    QSlider *d_afeAttenuationCh1Slider;
    QLabel  *d_afeAttenuationCh1Label;
    QSlider *d_afeTriggerCh1Slider;
    QLabel  *d_afeTriggerCh1Label;

    QSlider *d_afeOffsetCh2Slider;
    QLabel  *d_afeOffsetCh2Label;
    QSlider *d_afeFilterCh2Slider;
    QLabel  *d_afeFilterCh2Label;
    QSlider *d_afeAttenuationCh2Slider;
    QLabel  *d_afeAttenuationCh2Label;
    QSlider *d_afeTriggerCh2Slider;
    QLabel  *d_afeTriggerCh2Label;

    QCheckBox *d_resetBox;

    Scope *d_scope;
    ScopeDataSource *d_scopeData;
};
