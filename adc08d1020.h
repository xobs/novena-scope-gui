#ifndef __ADC08D1020_H__
#define __ADC08D1020_H__

// CPU-FPGA I2C API mapping
#define FPGA_ADC_WDATA_LSB  0x4
#define FPGA_ADC_WDATA_MSB  0x5
#define FPGA_ADC_COMMIT_ADR 0x6
#define FPGA_ADC_STAT       0x42

// address locations of various commands for ADC
#define ADC08D1020_CAL           0x0
#define ADC08D1020_CONFIG        0x1
#define ADC08D1020_I_OFFSET      0x2
#define ADC08D1020_I_FSADJ       0x3
#define ADC08D1020_EXTCONFIG     0x9
#define ADC08D1020_Q_OFFSET      0xA
#define ADC08D1020_Q_FSADJ       0xB
#define ADC08D1020_FINE_PHASE    0xE
#define ADC08D1020_COARSE_PHASE  0xF

class Adc08d1020 {
    
public:
    Adc08d1020();
    ~Adc08d1020();

    void setDefaults(void);
    int calibrationStatus(void);
    void calibrate(void);
    void testPattern(void);

private:
    int writeRegister(quint16 address, quint16 data);
    int read(quint16 address, quint8 *byte);
    int readBuffer(quint16 address, quint8 *bytes, int count);
    int i2cOpen(void);

    int i2c_fd;
};
#endif /* __ADC08D1020_H__ */
