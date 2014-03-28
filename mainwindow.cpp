#include "mainwindow.h"
#include "plot.h"
#include "knob.h"
#include "wheelbox.h"
#include <qwt_scale_engine.h>
#include <qlabel.h>
#include <qlayout.h>

MainWindow::MainWindow(QWidget *parent):
    QWidget(parent)
{
    const double intervalLength = 1.0; // seconds

    d_plot = new Plot(this);
    d_plot->setIntervalLength(intervalLength);

    d_amplitudeKnob = new Knob("Amplitude", 0.0, 1.0, this);
    d_amplitudeKnob->setValue(160.0);
    
    d_frequencyKnob = new Knob("Frequency [Hz]", 0.1, 20.0, this);
    d_frequencyKnob->setValue(17.8);

    d_afeOffsetKnob = new Knob("AFE offset", 0, 0xfff, this);
    d_afeOffsetKnob->setValue(0x700);

    d_afeFilterKnob = new Knob("AFE Filter Bandwidth [MHz]", 0, 750, this);
    d_afeFilterKnob->setValue(20);

    d_afeAttenuationKnob = new Knob("AFE Attenuation [dB]", -20, 0, this);
    d_afeAttenuationKnob->setValue(20);

    d_intervalWheel = new WheelBox("Displayed [s]", 0.1, 5.0, .1, this);
    d_intervalWheel->setValue(intervalLength);

    d_timerWheel = new WheelBox("Sample Interval [ms]", 0.1, 20.0, 0.1, this);
    d_timerWheel->setValue(10.0);

    QVBoxLayout* vLayout1 = new QVBoxLayout();
    vLayout1->addWidget(d_intervalWheel);
    vLayout1->addWidget(d_timerWheel);
    vLayout1->addStretch(10);
    vLayout1->addWidget(d_amplitudeKnob);
    vLayout1->addWidget(d_frequencyKnob);
    vLayout1->addWidget(d_afeOffsetKnob);
    vLayout1->addWidget(d_afeFilterKnob);
    vLayout1->addWidget(d_afeAttenuationKnob);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(d_plot, 10);
    layout->addLayout(vLayout1);

    connect(d_amplitudeKnob, SIGNAL(valueChanged(double)),
        SIGNAL(amplitudeChanged(double)));
    connect(d_frequencyKnob, SIGNAL(valueChanged(double)),
        SIGNAL(frequencyChanged(double)));
    connect(d_timerWheel, SIGNAL(valueChanged(double)),
        SIGNAL(signalIntervalChanged(double)));
    connect(d_afeOffsetKnob, SIGNAL(valueChanged(double)),
        SIGNAL(signalAfeOffsetChanged(double)));
    connect(d_afeFilterKnob, SIGNAL(valueChanged(double)),
        SIGNAL(signalAfeFilterChanged(double)));
    connect(d_afeAttenuationKnob, SIGNAL(valueChanged(double)),
        SIGNAL(signalAfeTrimPreampChanged(double)));

    connect(d_intervalWheel, SIGNAL(valueChanged(double)),
        d_plot, SLOT(setIntervalLength(double)) );
}

void MainWindow::start()
{
    d_plot->start();
}

double MainWindow::frequency() const
{
    return d_frequencyKnob->value();
}

double MainWindow::amplitude() const
{
    return d_amplitudeKnob->value();
}

double MainWindow::signalInterval() const
{
    return d_timerWheel->value();
}
