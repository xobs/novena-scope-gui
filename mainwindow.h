#include <qwidget.h>

class Plot;
class Knob;
class WheelBox;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget * = NULL);

    void start();

    double amplitude() const;
    double frequency() const;
    double signalInterval() const;

Q_SIGNALS:
    void amplitudeChanged(double);
    void frequencyChanged(double);
    void signalIntervalChanged(double);
    void signalAfeOffsetChanged(double);
    void signalAfeFilterChanged(double);
    void signalAfeAttenuationChanged(double);

private:
    Knob *d_frequencyKnob;
    Knob *d_amplitudeKnob;
    Knob *d_afeOffsetKnob;
    Knob *d_afeFilterKnob;
    Knob *d_afeAttenuationKnob;
    WheelBox *d_timerWheel;
    WheelBox *d_intervalWheel;

    Plot *d_plot;
};
