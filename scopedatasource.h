#include <QObject>

#include <netlink/genl/genl.h>
#include <netlink/genl/family.h>
#include <netlink/genl/ctrl.h>
#include <netlink/msg.h>
#include <netlink/attr.h>

class Ad9520;
class Adc08d1020;
class Dac101c085;
class Lmh6518;

class ScopeDataSource : public QObject
{
    Q_OBJECT

public:
    ScopeDataSource(QObject *parent = NULL);
    ~ScopeDataSource(void);

    int frequency() const;
    int amplitude() const;

    enum resetState {
        enterReset,
        exitReset,
    };

public slots:
    void setAmplitude(int);
    void setFrequency(int);
    void setDacOffset1(int);
    void setDacTrigger1(int);
    void setAfeFilter1(int);
    void setAfeAttenuation1(int);
    void setDacOffset2(int);
    void setDacTrigger2(int);
    void setAfeFilter2(int);
    void setAfeAttenuation2(int);
    int getData(int samples);
    int setResetState(enum resetState);

protected:

private:
    bool openDevice(void);
    struct nl_msg *allocMsg(int cmd);
    int triggerSample(void);
    int sendReadRequest(void);
    int doReadRequest(void);
    int setDefaults(void);

    struct nl_sock *handle;
    struct nl_cache *cache;
    struct genl_family *id;
    nlmsghdr *nhdr;
    void *bufferData;
    quint8 *bufferDataPtr;
    int bufferDataSize;

    int d_frequency;
    int d_amplitude;
    Ad9520 *pll;
    Adc08d1020 *adc;
    Dac101c085 *dac1;
    Dac101c085 *dac2;
    Lmh6518 *vga1;
    Lmh6518 *vga2;

signals:
    void scopeData(const QByteArray channel1, const QByteArray channel2);
};
