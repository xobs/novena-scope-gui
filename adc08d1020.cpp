#include <QObject>
#include <QDebug>

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include "adc08d1020.h"

#define ADC08D1020_I2C_ADR  0x1E  // same as FPGA address
#define I2C_PATH "/dev/i2c-2"

Adc08d1020::Adc08d1020(void)
{
    i2c_fd = 0;
}

Adc08d1020::~Adc08d1020(void)
{
    if (i2c_fd)
        close(i2c_fd);
}

int Adc08d1020::i2cOpen(void)
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

int Adc08d1020::writeRegister(quint8 address, quint16 data)
{
    char i2cbuf[4]; 
    int slave_address = ADC08D1020_I2C_ADR;

    struct i2c_msg msg[1];
    struct i2c_ioctl_rdwr_data {
        struct i2c_msg *msgs;  /* ptr to array of simple messages */
        int nmsgs;             /* number of messages to exchange */
    } msgst;

    if (!i2c_fd)
        if (i2cOpen())
            return -1;

    //////// write data, address, and commit using multi-byte write
    i2cbuf[0] = FPGA_ADC_WDATA_LSB;
    i2cbuf[1] = data & 0xFF;
    i2cbuf[2] = (data >> 8) & 0xFF;
    i2cbuf[3] = (address & 0xF) | 0x10;

    msg[0].addr = slave_address;
    msg[0].flags = 0; // no flag means do a write
    msg[0].len = 4;
    msg[0].buf = i2cbuf;

    msgst.msgs = msg;	
    msgst.nmsgs = 1;

    if (ioctl(i2c_fd, I2C_RDWR, &msgst) < 0){
        perror("adc08d1020 write failed");
        return -1;
    }

    //////// now clear the commit, it's not self-clearing
    i2cbuf[0] = FPGA_ADC_COMMIT_ADR;
    i2cbuf[1] = address & 0xF;

    msg[0].addr = slave_address;
    msg[0].flags = 0; // no flag means do a write
    msg[0].len = 2;
    msg[0].buf = i2cbuf;

    msgst.msgs = msg;	
    msgst.nmsgs = 1;

    if (ioctl(i2c_fd, I2C_RDWR, &msgst) < 0){
        perror("Write-clear failed\n");
        return -1;
    }
  
      return 0;
}

int Adc08d1020::read(quint8 address, quint8 *byte)
{
    return readBuffer(address, byte, 1);
}

int Adc08d1020::readBuffer(quint8 address, quint8 *bytes, int count)
{
    int slave_address = ADC08D1020_I2C_ADR;
    unsigned char buff[1];
    struct i2c_msg msg[2];
    struct i2c_ioctl_rdwr_data {
        struct i2c_msg *msgs;  /* ptr to array of simple messages */
        int nmsgs;             /* number of messages to exchange */
    } msgst;


    if (!i2c_fd)
        if (i2cOpen())
            return -1;

    buff[0] = address;

    // set address for read
    msg[0].addr = slave_address;
    msg[0].flags = 0; // no flag means do a write
    msg[0].len = 1;
    msg[0].buf = (char *) buff;

    // set readback buffer
    msg[1].addr = slave_address;
    msg[1].flags = I2C_M_NOSTART | I2C_M_RD;
    msg[1].len = count;
    msg[1].buf = (char *)bytes;

    msgst.msgs = msg;
    msgst.nmsgs = 2;

    if (ioctl(i2c_fd, I2C_RDWR, &msgst) < 0) {
        perror("Read failed\n");
        return -1;
    }

    return 0;
}

// returns 1 if cal is running
int Adc08d1020::calibrationStatus(void)
{
    quint8 status;
    if (read(FPGA_ADC_STAT, &status))
        return -1;

    // note inversion, since level converter in hw is inverting
    if (status & 0x02)
        return 0;
    return 1;
}

void Adc08d1020::setDefaults(void)
{
    // set config
    writeRegister(ADC08D1020_CONFIG, 0xB3FF);
    // 1
    // 0
    // 1 nSD -- set for DCLK + OR output
    // 1 DCS -- duty cycle stabilizer on
    // 0 DCP -- DDR data changes on edges, set to 1 to change mid-phase
    // 0 nDE -- DDR enable, write 0 to enable DDR mode
    // 1 OV -- 1=output voltage set to 720mVp-p; 0=510mVp-p
    // 1 OED -- demux control - set non-demux mode, so the delay outputs are not used
    // 1111_1111

    // set extended config
    writeRegister(ADC08D1020_EXTCONFIG, 0x3FF);
    // 0  - no test pattern, set 1 for TP
    // 0  - keep resistor trim on
    // 0  - normal dual channel mode (set 1 for DES)
    // 0  - input select -- both on I channel if in DES
    // 0  
    // 0  - DLF -- set only if ADC running < 900MHz
    // 11_1111_1111

}

void Adc08d1020::calibrate(void)
{
  // initiate a calibration
  writeRegister( ADC08D1020_EXTCONFIG, 0x43FF ); // RTD has to be high for cal to run
  writeRegister( ADC08D1020_CAL, 0xFFFF ); // set the cal bit
  // clear the bit, it's not self clearing
  writeRegister( ADC08D1020_CAL, 0x7FFF ); // clear the cal bit
  writeRegister( ADC08D1020_EXTCONFIG, 0x03FF ); // reset RTD
  
  while (calibrationStatus())  // wait for cal to finish
    ;
}

void Adc08d1020::testPattern(void)
{
    // set extended config
    writeRegister( ADC08D1020_EXTCONFIG, 0x83FF );
    // 1  - no test pattern, set 1 for TP
    // 0  - keep resistor trim on
    // 0  - normal dual channel mode (set 1 for DES)
    // 0  - input select -- both on I channel if in DES
    // 0  
    // 0  - DLF -- set only if ADC running < 900MHz
    // 11_1111_1111
}
