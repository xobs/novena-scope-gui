#include <QObject>
#include <QDebug>

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include "ad9265.h"

#define AD9265_I2C_ADR  0x1E  // same as FPGA address
#define I2C_PATH "/dev/i2c-2"

Ad9265::Ad9265(void)
{
  i2c_fd = 0;
}

Ad9265::~Ad9265(void)
{
  if (i2c_fd)
    close(i2c_fd);
}

int Ad9265::i2cOpen(void)
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

int Ad9265::ad9265_write_byte( unsigned char address, unsigned char data ) {
  int i2cfd;
  char i2cbuf[4]; 
  int slave_address = AD9265_I2C_ADR;
  unsigned char c, d;
  
  // now do a read of a byte
  i2cfd = open("/dev/i2c-2", O_RDWR);
  if( i2cfd < 0 ) {
    perror("Unable to open /dev/i2c-2\n");
    i2cfd = 0;
    return -1;
  }
  if( ioctl( i2cfd, I2C_SLAVE, slave_address) < 0 ) {
    perror("Unable to set I2C slave device\n" );
    printf( "Address: %02x\n", slave_address );
    return -1;
  }
  
  i2cbuf[0] = FPGA_ADC_WDATA; i2cbuf[1] = (unsigned char) data;
  if( write(i2cfd, i2cbuf, 2) != 2 ) {
    perror("i2c write failed\n");
  }
  
  i2cbuf[0] = FPGA_ADC_ADDR_LSB; i2cbuf[1] = (unsigned char) (address & 0xFF) ;
  if( write(i2cfd, i2cbuf, 2) != 2 ) {
    perror("i2c write failed\n");
  }
  c = (unsigned char)  (((address >> 8) & 0x1F) );
  
  i2cbuf[0] = FPGA_ADC_ADDR_COMIT; i2cbuf[1] = c;
  if( write(i2cfd, i2cbuf, 2) != 2 ) {
    perror("i2c write failed\n");
  }
  c |= FPGA_ADC_COMIT_MSK;
  
  i2cbuf[0] = FPGA_ADC_ADDR_COMIT; i2cbuf[1] = c;
  if( write(i2cfd, i2cbuf, 2) != 2 ) {
    perror("i2c write failed\n");
  }
  
  do {
    d = i2c_smbus_read_byte_data(i2cfd, FPGA_ADC_STAT);
  } while( d & 1 );
  
  // commit is not self-clearing
  c &= ~FPGA_ADC_COMIT_MSK;
  i2cbuf[0] = FPGA_ADC_ADDR_COMIT; i2cbuf[1] = c;
  if( write(i2cfd, i2cbuf, 2) != 2 ) {
    perror("i2c write failed\n");
  }
  
  close( i2cfd );
  
  return 0;
}

int Ad9265::ad9265_read_byte( int address, unsigned char *data ) {
  int i2cfd;
  int slave_address = AD9265_I2C_ADR;
  unsigned char c, d;
  char i2cbuf[256]; // meh too big but meh
  
  
  // now do a read of a byte
  i2cfd = open("/dev/i2c-2", O_RDWR);
  if( i2cfd < 0 ) {
    perror("Unable to open /dev/i2c-2\n");
    i2cfd = 0;
    return -1;
  }
  if( ioctl( i2cfd, I2C_SLAVE, slave_address) < 0 ) {
    perror("Unable to set I2C slave device\n" );
    printf( "Address: %02x\n", slave_address );
    return -1;
  }
  
  i2cbuf[0] = FPGA_ADC_ADDR_LSB; i2cbuf[1] = (unsigned char) (address & 0xFF) ;
  if( write(i2cfd, i2cbuf, 2) != 2 ) {
    perror("i2c write failed\n");
  }
  c = (unsigned char)  (((address >> 8) & 0x1F) | FPGA_ADC_RD_WR_MSK);
  
  i2cbuf[0] = FPGA_ADC_ADDR_COMIT; i2cbuf[1] = c;
  if( write(i2cfd, i2cbuf, 2) != 2 ) {
    perror("i2c write failed\n");
  }
  c |= FPGA_ADC_COMIT_MSK;
  
  i2cbuf[0] = FPGA_ADC_ADDR_COMIT; i2cbuf[1] = c;
  if( write(i2cfd, i2cbuf, 2) != 2 ) {
    perror("i2c write failed\n");
  }
  
  do {
    d = i2c_smbus_read_byte_data(i2cfd, FPGA_ADC_STAT);
  } while( d & 1 );
  
  *data = i2c_smbus_read_byte_data(i2cfd, FPGA_ADC_RBK);
  
  
  // commit is not self-clearing
  c &= ~FPGA_ADC_COMIT_MSK;
  i2cbuf[0] = FPGA_ADC_ADDR_COMIT; i2cbuf[1] = c;
  if( write(i2cfd, i2cbuf, 2) != 2 ) {
    perror("i2c write failed\n");
  }
  
  close( i2cfd );
  
  return 0;
}

void Ad9265::setDefaults(void)
{
  ad9265_write_byte(AD9265_POWERMODE, 0x80); // don't power down
  ad9265_write_byte(AD9265_GLOBALCLK, 0x01); // turn on duty cycle stabilizer
  ad9265_write_byte(AD9265_CLOCKDIV, 0x00); // don't divide the clock
  ad9265_write_byte(AD9265_TRANSFER, 0x01 ); // commit values
  ad9265_write_byte(AD9265_OUTMODE, 0x40 ); // output as LVDS, ANSI levels, don't invert output, offset binary
  ad9265_write_byte(AD9265_TRANSFER, 0x01 ); // commit values
  ad9265_write_byte(AD9265_CLOCKPHASE, 0x00 ); // no delay on clock phase
  ad9265_write_byte(AD9265_DCODELAY, 0x00 ); // no delay on DCO output
  ad9265_write_byte(AD9265_VREF, 0xc0 ); // VREF is 2.0Vp-p
  
  ad9265_write_byte(AD9265_TESTMODE, 0x00 ); // no test mode
  
  ad9265_write_byte(AD9265_TRANSFER, 0x01 ); // commit values
}

void Ad9265::testPattern(void)
{
  ad9265_write_byte(AD9265_POWERMODE, 0x80); // don't power down
  ad9265_write_byte(AD9265_GLOBALCLK, 0x01); // turn on duty cycle stabilizer
  ad9265_write_byte(AD9265_CLOCKDIV, 0x00); // don't divide the clock
  ad9265_write_byte(AD9265_OUTMODE, 0x40 ); // output as LVDS, ANSI levels, don't invert output, offset binary
  ad9265_write_byte(AD9265_TRANSFER, 0x01 ); // commit values
  ad9265_write_byte(AD9265_CLOCKPHASE, 0x00 ); // no delay on clock phase
  ad9265_write_byte(AD9265_DCODELAY, 0x00 ); // no delay on DCO output
  ad9265_write_byte(AD9265_VREF, 0xc0 ); // VREF is 2.0Vp-p
  
  ad9265_write_byte(AD9265_TESTMODE, 0x06 ); // PN 9 sequence
  
  ad9265_write_byte(AD9265_TRANSFER, 0x01 ); // commit values
}
