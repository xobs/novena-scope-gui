#include <QLabel>
#include <QSlider>
#include <QLayout>
#include <QCheckBox>

#include "scopedatasource.h"
#include "mainwindow.h"
#include "scope.h"

MainWindow::MainWindow(QWidget *parent):
    QWidget(parent)
{
    const quint32 intervalLength = 100; // milliseconds

    d_scope = new Scope(this);
    d_scope->setRedrawFrequency(intervalLength);

    d_scopeData = new ScopeDataSource(this);
    d_scope->setDataSource(d_scopeData);

    d_channel1Label = new QLabel("Channel 1");
    d_channel2Label = new QLabel("Channel 2");

    d_afeOffsetCh1Slider = new QSlider(Qt::Horizontal, this);
    d_afeOffsetCh1Label = new QLabel("AFE offset");
    d_afeOffsetCh1Slider->setRange(0, 0xfff);
    d_afeOffsetCh1Slider->setValue(0x700);

    d_afeFilterCh1Slider = new QSlider(Qt::Horizontal, this);
    d_afeFilterCh1Label = new QLabel("AFE Filter Bandwidth [MHz]");
    d_afeFilterCh1Slider->setRange(0, 750);
    d_afeFilterCh1Slider->setValue(20);

    d_afeAttenuationCh1Slider = new QSlider(Qt::Horizontal, this);
    d_afeAttenuationCh1Label = new QLabel("AFE Attenuation [dB]");
    d_afeAttenuationCh1Slider->setRange(-20, 0);
    d_afeAttenuationCh1Slider->setValue(20);

    d_afeTriggerCh1Slider = new QSlider(Qt::Horizontal, this);
    d_afeTriggerCh1Label = new QLabel("AFE Trigger [dB]");
    d_afeTriggerCh1Slider->setRange(0, 0xfff);
    d_afeTriggerCh1Slider->setValue(20);

    d_afeOffsetCh2Slider = new QSlider(Qt::Horizontal, this);
    d_afeOffsetCh2Label = new QLabel("AFE offset");
    d_afeOffsetCh2Slider->setRange(0, 0xfff);
    d_afeOffsetCh2Slider->setValue(0x700);

    d_afeFilterCh2Slider = new QSlider(Qt::Horizontal, this);
    d_afeFilterCh2Label = new QLabel("AFE Filter Bandwidth [MHz]");
    d_afeFilterCh2Slider->setRange(0, 750);
    d_afeFilterCh2Slider->setValue(20);

    d_afeAttenuationCh2Slider = new QSlider(Qt::Horizontal, this);
    d_afeAttenuationCh2Label = new QLabel("AFE Attenuation [dB]");
    d_afeAttenuationCh2Slider->setRange(-20, 0);
    d_afeAttenuationCh2Slider->setValue(20);

    d_afeTriggerCh2Slider = new QSlider(Qt::Horizontal, this);
    d_afeTriggerCh2Label = new QLabel("AFE Trigger [dB]");
    d_afeTriggerCh2Slider->setRange(0, 0xfff);
    d_afeTriggerCh2Slider->setValue(20);

    d_resetBox = new QCheckBox("Reset AFE", this);


    QVBoxLayout *vLayoutCh1 = new QVBoxLayout();
    vLayoutCh1->addWidget(d_channel1Label);
    vLayoutCh1->addStretch(10);
    vLayoutCh1->addWidget(d_afeOffsetCh1Slider);
    vLayoutCh1->addWidget(d_afeOffsetCh1Label);
    vLayoutCh1->addStretch(10);
    vLayoutCh1->addWidget(d_afeFilterCh1Slider);
    vLayoutCh1->addWidget(d_afeFilterCh1Label);
    vLayoutCh1->addStretch(10);
    vLayoutCh1->addWidget(d_afeAttenuationCh1Slider);
    vLayoutCh1->addWidget(d_afeAttenuationCh1Label);
    vLayoutCh1->addStretch(10);
    vLayoutCh1->addWidget(d_afeTriggerCh1Slider);
    vLayoutCh1->addWidget(d_afeTriggerCh1Label);
    vLayoutCh1->addStretch(10);

    QVBoxLayout *vLayoutCh2 = new QVBoxLayout();
    vLayoutCh2->addWidget(d_channel2Label);
    vLayoutCh2->addStretch(10);
    vLayoutCh2->addWidget(d_afeOffsetCh2Slider);
    vLayoutCh2->addWidget(d_afeOffsetCh2Label);
    vLayoutCh2->addStretch(10);
    vLayoutCh2->addWidget(d_afeFilterCh2Slider);
    vLayoutCh2->addWidget(d_afeFilterCh2Label);
    vLayoutCh2->addStretch(10);
    vLayoutCh2->addWidget(d_afeAttenuationCh2Slider);
    vLayoutCh2->addWidget(d_afeAttenuationCh2Label);
    vLayoutCh2->addStretch(10);
    vLayoutCh2->addWidget(d_afeTriggerCh2Slider);
    vLayoutCh2->addWidget(d_afeTriggerCh2Label);
    vLayoutCh2->addStretch(10);

    QHBoxLayout *panelChannels = new QHBoxLayout();
    panelChannels->addLayout(vLayoutCh1, 2);
    panelChannels->addLayout(vLayoutCh2, 2);

    QVBoxLayout *rightWidgets = new QVBoxLayout();
    rightWidgets->addLayout(panelChannels);
    rightWidgets->addStretch(10);
    rightWidgets->addWidget(d_resetBox);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(d_scope, 10);
    layout->addLayout(rightWidgets, 2);

    connect(d_afeOffsetCh1Slider, SIGNAL(valueChanged(int)),
            d_scopeData,          SLOT(setDacOffset1(int)));
    connect(d_afeTriggerCh1Slider, SIGNAL(valueChanged(int)),
            d_scopeData,           SLOT(setDacTrigger1(int)));
    connect(d_afeFilterCh1Slider, SIGNAL(valueChanged(int)),
            d_scopeData,          SLOT(setAfeFilter1(int)));
    connect(d_afeAttenuationCh1Slider, SIGNAL(valueChanged(int)),
            d_scopeData,               SLOT(setAfeAttenuation1(int)));

    connect(d_afeOffsetCh2Slider, SIGNAL(valueChanged(int)),
            d_scopeData,          SLOT(setDacOffset2(int)));
    connect(d_afeTriggerCh2Slider, SIGNAL(valueChanged(int)),
            d_scopeData,           SLOT(setDacTrigger2(int)));
    connect(d_afeFilterCh2Slider, SIGNAL(valueChanged(int)),
            d_scopeData,          SLOT(setAfeFilter2(int)));
    connect(d_afeAttenuationCh2Slider, SIGNAL(valueChanged(int)),
            d_scopeData,               SLOT(setAfeAttenuation2(int)));

    connect(d_resetBox, SIGNAL(stateChanged(int)),
            this,       SLOT(processCheckbox(int)));
}

void MainWindow::start()
{
    d_scope->setMode(Scope::autoSampling);
}

void MainWindow::processCheckbox(int state)
{
    if (state)
        d_scopeData->setResetState(ScopeDataSource::enterReset);
    else
        d_scopeData->setResetState(ScopeDataSource::exitReset);
}
