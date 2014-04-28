#ifndef __AD9265_H__
#define __AD9265_H__

// CPU-FPGA I2C API mapping
#define FPGA_ADC_WDATA      0x4
#define FPGA_ADC_ADDR_LSB   0x5
#define FPGA_ADC_ADDR_COMIT 0x6
#define FPGA_ADC_RBK        0x42
#define FPGA_ADC_STAT       0x43

#define FPGA_ADC_RD_WR_MSK  0x20
#define FPGA_ADC_COMIT_MSK  0x40

// address locations of various commands for ADC
#define AD9265_SPICONFIG         0x0
#define AD9265_CHIPID            0x1
#define AD9265_CHIPGRADE         0x2
#define AD9265_TRANSFER          0xFF
#define AD9265_POWERMODE         0x8
#define AD9265_GLOBALCLK         0x9
#define AD9265_CLOCKDIV          0xB
#define AD9265_TESTMODE          0xD
#define AD9265_BIST              0xE
#define AD9265_OUTMODE           0x14
#define AD9265_CLOCKPHASE        0x16
#define AD9265_DCODELAY          0x17
#define AD9265_VREF              0x18
#define AD9265_BISTLSB           0x24
#define AD9265_BISTMSB           0x25
#define AD9265_DITHER            0x30
#define AD9265_SYNC              0x100

class Ad9265 {
    
public:
    Ad9265();
    ~Ad9265();

    void setDefaults(void);
    void testPattern(void);

private:
    int ad9265_read_byte( int address, unsigned char *data );
    int ad9265_write_byte( unsigned char address, unsigned char data );
    int i2cOpen(void);

    int i2c_fd;
};
#endif /* __AD9265_H__ */
