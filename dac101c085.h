#ifndef __DAC101C085_H__
#define __DAC101C085_H__

class Dac101c085 {

public:
    enum dacAddress {
	channel1Trim = 0x4e,
	channel1Trigger = 0x4d,
	channel2Trim = 0x4c,
	channel2Trigger = 0x0d,
    };

    Dac101c085(enum dacAddress triggerAddr, enum dacAddress trimAddr);
    ~Dac101c085();

    void setOffset(quint16 offset);
    void setTriggerLevel(quint16 offset);

private:
    int i2cOpen(void);
    int writeDac(enum dacAddress address, quint16 value);
    int readDac(enum dacAddress address, quint16 *value);

    int i2c_fd;
    enum dacAddress trimAddr, triggerAddr;
};

#endif /* __DAC101C085_H__ */
