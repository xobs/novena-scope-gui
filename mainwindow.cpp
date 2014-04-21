#include <QLabel>
#include <QSlider>
#include <QLayout>

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

#if 0
    d_afeOffsetSlider = new QSlider(Qt::Horizontal, this);
    d_afeOffsetLabel = new QLabel("AFE offset");
    d_afeOffsetSlider->setRange(0, 0xfff);
    d_afeOffsetSlider->setValue(0x700);

    d_afeFilterSlider = new QSlider(Qt::Horizontal, this);
    d_afeFilterLabel = new QLabel("AFE Filter Bandwidth [MHz]");
    d_afeFilterSlider->setRange(0, 750);
    d_afeFilterSlider->setValue(20);

    d_afeAttenuationSlider = new QSlider(Qt::Horizontal, this);
    d_afeAttenuationLabel = new QLabel("AFE Attenuation [dB]");
    d_afeAttenuationSlider->setRange(-20, 0);
    d_afeAttenuationSlider->setValue(20);

    d_afeTriggerSlider = new QSlider(Qt::Horizontal, this);
    d_afeTriggerLabel = new QLabel("AFE Trigger [dB]");
    d_afeTriggerSlider->setRange(0, 0xfff);
    d_afeTriggerSlider->setValue(20);
#endif

#if 0
    QVBoxLayout* vLayout1 = new QVBoxLayout();
    vLayout1->addStretch(10);
    vLayout1->addWidget(d_afeOffsetSlider);
    vLayout1->addWidget(d_afeOffsetLabel);
    vLayout1->addStretch(10);
    vLayout1->addWidget(d_afeFilterSlider);
    vLayout1->addWidget(d_afeFilterLabel);
    vLayout1->addStretch(10);
    vLayout1->addWidget(d_afeAttenuationSlider);
    vLayout1->addWidget(d_afeAttenuationLabel);
    vLayout1->addStretch(10);
    vLayout1->addWidget(d_afeTriggerSlider);
    vLayout1->addWidget(d_afeTriggerLabel);
#endif

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(d_scope, 10);
    //    layout->addLayout(vLayout1);

#if 0
    connect(d_afeOffsetSlider, SIGNAL(valueChanged(int)),
            d_scopeData,     SLOT(setDacOffset(int)));
    connect(d_afeFilterSlider, SIGNAL(valueChanged(int)),
            d_scopeData,     SLOT(setAfeFilter(int)));
    connect(d_afeAttenuationSlider, SIGNAL(valueChanged(int)),
            d_scopeData,            SLOT(setAfeAttenuation(int)));
    connect(d_afeTriggerSlider, SIGNAL(valueChanged(int)),
            d_scopeData,        SLOT(setDacTrigger(int)));
#endif
}

void MainWindow::start()
{
    d_scope->setMode(Scope::autoSampling);
}
