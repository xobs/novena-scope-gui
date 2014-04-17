#ifndef __DAC101C085_H__
#define __DAC101C085_H__

class Dac101c085 {

public:
    Dac101c085();
    ~Dac101c085();

    enum dacType {
        trim = 0,
        trigger = 1,
    };
    void setOffset(quint16 offset);
    void setTriggerLevel(quint16 offset);

private:
    int i2cOpen(void);
    int writeDac(enum dacType type, quint16 value);
    int readDac(enum dacType type, quint16 *value);

    int i2c_fd;
};

#endif /* __DAC101C085_H__ */
