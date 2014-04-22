#ifndef __LMH6518_H__
#define __LMH6518_H__

#define LMH6518_AUXPWR_BIT  10
#define LMH6518_AUXPWR_ON   0
#define LMH6518_AUXPWR_OFF  1

#define LMH6518_FILT_BIT   6
#define LMH6518_FILT_FULL  0
#define LMH6518_FILT_20MHZ 1
#define LMH6518_FILT_100MHZ 2
#define LMH6518_FILT_200MHZ 3
#define LMH6518_FILT_350MHZ 4
#define LMH6518_FILT_650MHZ 5
#define LMH6518_FILT_750MHZ 6

#define LMH6518_PREAMP_BIT  4
#define LMH6518_PREAMP_LG   0
#define LMH6518_PREAMP_HG   1

#define LMH6518_ATTEN_BIT  0
// 0 dB attenuation (big signal passed through)
#define LMH6518_ATTEN_0DB  0    
#define LMH6518_ATTEN_2DB  1
#define LMH6518_ATTEN_4DB  2
#define LMH6518_ATTEN_6DB  3
#define LMH6518_ATTEN_8DB  4
#define LMH6518_ATTEN_10DB  5
#define LMH6518_ATTEN_12DB  6
#define LMH6518_ATTEN_14DB  7
#define LMH6518_ATTEN_16DB  8
#define LMH6518_ATTEN_18DB  9
// -20 dB attenuation (big signal turned into small signal)
#define LMH6518_ATTEN_20DB  10

class Lmh6518 {

public:
    enum addr {
	    channel1Addr = 0x28,
	    channel2Addr = 0x2a,
    };

    Lmh6518(enum addr addr);
    ~Lmh6518();

    int setPreampAuxPower(bool auxPower);
    int setAuxPower(bool auxPower);
    int setPreampFilter(quint32 filter);
    int setFilter(quint32 filter);
    int setPreampAttenuation(qint32 attenuation);
    int setAttenuation(qint32 attenuation);

private:
    int i2c_fd;
    enum addr i2cAddr;
    int write(quint16 data);
    int i2cOpen(void);
    int updateGain(bool auxpwr, quint32 filter,
                   bool preamp, qint32 attenuation);

    quint32 preampAuxPower;
    quint32 auxPower;
    quint32 preampFilter;
    quint32 filter;
    qint32 preampAttenuation;
    qint32 attenuation;
};

#endif /* __LMH6518_H__ */
