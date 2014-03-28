#include <QObject>
#include <QDebug>

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include "ad9520.h"

#include "lmh6518.h"

#define I2C_PATH "/dev/i2c-1"
#define LMH6518_I2C_ADR  0x28

Lmh6518::Lmh6518(void)
{
    i2c_fd = 0;
    attenuation = 0;
    filter = 0;
    auxPower = 0;
    preampAttenuation = 0;
    preampFilter = 0;
    preampAuxPower = 0;
}

Lmh6518::~Lmh6518(void)
{
    if (i2c_fd)
        close(i2c_fd);
}

int Lmh6518::i2cOpen(void)
{
    if (i2c_fd)
        close(i2c_fd);
    i2c_fd = open(I2C_PATH, O_RDWR);
    if (i2c_fd < 0) {
        perror("Unable to open " I2C_PATH);
        i2c_fd = 0;
        return 1;
    }
    return 0;
}

int Lmh6518::write(quint16 value)
{
    char i2cbuf[sizeof(value)];
    int slave_address = LMH6518_I2C_ADR;

    struct i2c_msg msg[1];
    struct i2c_ioctl_rdwr_data {
        struct i2c_msg *msgs;  /* ptr to array of simple messages */
        int nmsgs;             /* number of messages to exchange */
    } msgst;

    if (!i2c_fd)
        if (i2cOpen())
            return -1;

    i2cbuf[0] = ((value & 0xFF00) >> 8);
    i2cbuf[1] = (value & 0xFF);

    msg[0].addr = slave_address;
    msg[0].flags = 0; // no flag means do a write
    msg[0].len = sizeof(value);
    msg[0].buf = i2cbuf;

    msgst.msgs = msg;
    msgst.nmsgs = 1;

    if (ioctl(i2c_fd, I2C_RDWR, &msgst) < 0) {
        perror("Write failed");
        return -1;
    }
    return 0;
}

/*
  auxpwr - turn the aux channel (for trigger) on or off. 1 = on, 0 = off
  filter - set filter bandwidth, in MHz; 0 = infinity
  premap - preamp in low or high gain. 0 = low gain, 1 = high gain
  attenuation - set attenuation, in dB (minus values expected, from 0 to minus 20 dB)
 */

int Lmh6518::updateGain(bool auxpwr, quint32 filter,
                        bool preamp, qint32 attenuation)
{
    quint16 data = 0;

    if (auxpwr)
        data |= (LMH6518_AUXPWR_ON << LMH6518_AUXPWR_BIT);
    else
        data |= (LMH6518_AUXPWR_OFF << LMH6518_AUXPWR_BIT);

    if (filter == 0 || filter > 750)
        data |= (LMH6518_FILT_FULL << LMH6518_FILT_BIT);
    else if (filter <= 20)
        data |= (LMH6518_FILT_20MHZ << LMH6518_FILT_BIT);
    else if (filter <= 100)
        data |= (LMH6518_FILT_100MHZ << LMH6518_FILT_BIT);
    else if (filter <= 200)
        data |= (LMH6518_FILT_200MHZ << LMH6518_FILT_BIT);
    else if (filter <= 350)
        data |= (LMH6518_FILT_350MHZ << LMH6518_FILT_BIT);
    else if (filter <= 650)
        data |= (LMH6518_FILT_650MHZ << LMH6518_FILT_BIT);
    else if (filter <= 750)
        data |= (LMH6518_FILT_750MHZ << LMH6518_FILT_BIT);
    else
        data |= (LMH6518_FILT_FULL << LMH6518_FILT_BIT);

    if (preamp)
        data |= (LMH6518_PREAMP_HG << LMH6518_PREAMP_BIT);
    else
        data |= (LMH6518_PREAMP_LG << LMH6518_PREAMP_BIT);

    if (attenuation >= 0)
        data |= (LMH6518_ATTEN_0DB << LMH6518_ATTEN_BIT);
    else if (attenuation >= -2)
        data |= (LMH6518_ATTEN_2DB << LMH6518_ATTEN_BIT);
    else if (attenuation >= -4)
        data |= (LMH6518_ATTEN_4DB << LMH6518_ATTEN_BIT);
    else if (attenuation >= -6)
        data |= (LMH6518_ATTEN_6DB << LMH6518_ATTEN_BIT);
    else if (attenuation >= -8)
        data |= (LMH6518_ATTEN_8DB << LMH6518_ATTEN_BIT);
    else if (attenuation >= -10)
        data |= (LMH6518_ATTEN_10DB << LMH6518_ATTEN_BIT);
    else if (attenuation >= -12)
        data |= (LMH6518_ATTEN_12DB << LMH6518_ATTEN_BIT);
    else if (attenuation >= -14)
        data |= (LMH6518_ATTEN_14DB << LMH6518_ATTEN_BIT);
    else if (attenuation >= -16)
        data |= (LMH6518_ATTEN_16DB << LMH6518_ATTEN_BIT);
    else if (attenuation >= -18)
        data |= (LMH6518_ATTEN_18DB << LMH6518_ATTEN_BIT);
    else
        data |= (LMH6518_ATTEN_20DB << LMH6518_ATTEN_BIT);

    return write(data);
}

int Lmh6518::setPreampAuxPower(bool power)
{
    preampAuxPower = power;
    return updateGain(preampAuxPower, preampFilter, true, preampAttenuation);
}

int Lmh6518::setAuxPower(bool power)
{
    auxPower = power;
    return updateGain(auxPower, filter, false, attenuation);
}

int Lmh6518::setPreampFilter(quint32 filter)
{
    preampFilter = filter;
    return updateGain(preampAuxPower, preampFilter, true, preampAttenuation);
}

int Lmh6518::setFilter(quint32 f)
{
    filter = f;
    return updateGain(auxPower, filter, false, attenuation);
}

int Lmh6518::setPreampAttenuation(qint32 attenuation)
{
    preampAttenuation = attenuation;
    return updateGain(preampAuxPower, preampFilter, true, preampAttenuation);
}

int Lmh6518::setAttenuation(qint32 a)
{
    attenuation = a;
    return updateGain(auxPower, filter, false, attenuation);
}
