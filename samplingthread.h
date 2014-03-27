#include <qwt_sampling_thread.h>

#include <netlink/genl/genl.h>
#include <netlink/genl/family.h>
#include <netlink/genl/ctrl.h>
#include <netlink/msg.h>
#include <netlink/attr.h>

class SamplingThread: public QwtSamplingThread
{
    Q_OBJECT

public:
    SamplingThread(QObject *parent = NULL);
    ~SamplingThread(void);

    double frequency() const;
    double amplitude() const;

public Q_SLOTS:
    void setAmplitude(double);
    void setFrequency(double);

protected:
    virtual void sample(double elapsed);

private:
    bool openDevice(void);
    struct nl_msg *allocMsg(int cmd);
    int sendReadRequest(void);
    int doReadRequest(void);

    struct nl_sock *handle;
    struct nl_cache *cache;
    struct genl_family *id;
    nlmsghdr *nhdr;
    void *bufferData;
    quint8 *bufferDataPtr;
    int bufferDataSize;

    double d_frequency;
    double d_amplitude;
};
