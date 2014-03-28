#include <QObject>
#include <QDebug>

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include "ad9520.h"

#include "dac101c085.h"

#define I2C_PATH "/dev/i2c-1"

#define DAC101C085_TRIG_I2C_ADR  0x4D
#define DAC101C085_TRIM_I2C_ADR  0x4E

Dac101c085::Dac101c085(void)
{
    i2c_fd = 0;
}

Dac101c085::~Dac101c085(void)
{
    if (i2c_fd)
        close(i2c_fd);
}

int Dac101c085::i2cOpen(void)
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

int Dac101c085::writeDac(enum dacType type, quint16 value)
{
    char i2cbuf[sizeof(value)];
    int slave_address;

    struct i2c_msg msg[1];
    struct i2c_ioctl_rdwr_data {
        struct i2c_msg *msgs;  /* ptr to array of simple messages */
        int nmsgs;             /* number of messages to exchange */
    } msgst;

    if (!i2c_fd)
        if (i2cOpen())
            return -1;

    if (type == trigger)
        slave_address = DAC101C085_TRIG_I2C_ADR;
    else if (type == trim)
        slave_address = DAC101C085_TRIM_I2C_ADR;
    else {
        qDebug() << "Unrecognized DAC type";
        return -1;
    }

    i2cbuf[0] = ((value & 0xFF00) >> 8);
    i2cbuf[1] = (value & 0xFF);

    msg[0].addr = slave_address;
    msg[0].flags = 0; // no flag means do a write
    msg[0].len = sizeof(value);
    msg[0].buf = i2cbuf;

    msgst.msgs = msg;
    msgst.nmsgs = 1;

    if (ioctl(i2c_fd, I2C_RDWR, &msgst) < 0) {
        perror("Write failed\n");
        return -1;
    }
    return 0;
}

int Dac101c085::readDac(enum dacType type, quint16 *value)
{
    int slave_address;
    struct i2c_msg msg[1];
    struct i2c_ioctl_rdwr_data {
        struct i2c_msg *msgs;  /* ptr to array of simple messages */
        int nmsgs;             /* number of messages to exchange */
    } msgst;

    if (!i2c_fd)
        if (i2cOpen())
            return -1;

    if (type == trigger)
        slave_address = DAC101C085_TRIG_I2C_ADR;
    else if (type == trim)
        slave_address = DAC101C085_TRIM_I2C_ADR;
    else {
        qDebug() << "Unrecognized DAC type";
        return -1;
    }

    // set readback buffer
    msg[0].addr = slave_address;
    msg[0].flags = I2C_M_NOSTART | I2C_M_RD;
    msg[0].len = sizeof(*value);
    msg[0].buf = (char *)value;

    msgst.msgs = msg;
    msgst.nmsgs = 1;

    if (ioctl(i2c_fd, I2C_RDWR, &msgst) < 0) {
        perror("Read failed\n");
        return -1;
    }

    return 0;
}

void Dac101c085::setOffset(quint16 code)
{
    if (code > 0xFFF) {
        qDebug() << "Warning: code larger than 0xFFF; truncating.";
        code = 0xFFF;
    }
  
    writeDac(trim, (code & 0xFFFF));
}
