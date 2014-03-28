#include <qwt_math.h>
#include <math.h>

#if QT_VERSION < 0x040600
#define qFastSin(x) ::sin(x)
#endif

#include <netlink/genl/genl.h>
#include <netlink/genl/family.h>
#include <netlink/genl/ctrl.h>
#include <netlink/msg.h>
#include <netlink/attr.h>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>
#include <unistd.h>

#include "samplingthread.h"
#include "signaldata.h"
#include "ad9520.h"
#include "adc08d1020.h"
#include "dac101c085.h"
#include "lmh6518.h"
    
#define FAMILY_NAME "kosagi-fpga"
#define DATA_SIZE 4096

/* list of valid commands */
enum kosagi_fpga_commands {
        KOSAGI_CMD_UNSPEC,
        KOSAGI_CMD_SEND,
        KOSAGI_CMD_READ,
        __KOSAGI_CMD_MAX,
};

/* list of valid command attributes */
enum kosagi_fpga_attributes {
        KOSAGI_ATTR_NONE,
        KOSAGI_ATTR_FPGA_DATA,
        KOSAGI_ATTR_MESSAGE,
        __KOSAGI_ATTR_MAX,
};

SamplingThread::SamplingThread(QObject *parent):
    QwtSamplingThread(parent),
    d_frequency(5.0),
    d_amplitude(20.0)
{
    handle = NULL;
    cache = NULL;
    id = NULL;
    nhdr = NULL;
    bufferData = NULL;
    bufferDataSize = 0;

    pll = new Ad9520();
    adc = new Adc08d1020();
    dac = new Dac101c085();
    vga = new Lmh6518();

    openDevice();
}

SamplingThread::~SamplingThread()
{
    if (nhdr)
        free(nhdr);
    delete vga;
    delete dac;
    delete adc;
    delete pll;
}

void SamplingThread::setFrequency(double frequency)
{
    d_frequency = frequency;
}

double SamplingThread::frequency() const
{
    return d_frequency;
}

void SamplingThread::setAmplitude(double amplitude)
{
    d_amplitude = amplitude;
}

double SamplingThread::amplitude() const
{
    return d_amplitude;
}

void SamplingThread::setAfeOffset(double offset)
{
    dac->setOffset(offset);
}

void SamplingThread::setAfeAttenuation(double attenuation)
{
    vga->setAttenuation(attenuation);
}

void SamplingThread::setAfeFilter(double filter)
{
    vga->setFilter(filter);
}

bool SamplingThread::openDevice(void)
{
    int ret;

    handle = nl_socket_alloc();
    if (!handle) {
        fprintf(stderr, "Failed to allocate netlink handle\n");
        return false;
    }

    ret = genl_connect(handle);
    if (ret) {
        fprintf(stderr, "Failed to connect to generic netlink: %s\n",
                nl_geterror(ret));
        return false;
    }

    ret = genl_ctrl_alloc_cache(handle, &cache);
    if (ret) {
        fprintf(stderr, "Failed to allocate generic netlink cache\n");
        return false;
    }

    id = genl_ctrl_search_by_name(cache, FAMILY_NAME);
    if (!id) {
        fprintf(stderr, "Family %s not found\n", FAMILY_NAME);
        return false;
    }

    ret = nl_socket_set_msg_buf_size(handle, 2 * DATA_SIZE);
    if (ret < 0) {
        fprintf(stderr, "Failed to set buffer size: %s\n",
                nl_geterror(ret));
        return false;
    }

    nl_socket_disable_auto_ack(handle);

    pll->selfConfig(Ad9520::Speed500Mhz);
    adc->calibrate();
    vga->setAuxPower(true);

    return true;
}

struct nl_msg *SamplingThread::allocMsg(int cmd)
{
    struct nl_msg *msg;
    void *header;

    msg = nlmsg_alloc_size(2 * DATA_SIZE);
    if (!msg) {
        fprintf(stderr, "Unable to alloc nlmsg\n");
        return NULL;
    }

    header = genlmsg_put(msg, NL_AUTO_PORT, NL_AUTO_SEQ,
            genl_family_get_id(id),
            0, NLM_F_REQUEST, cmd, 1);
    if (!header) {
        fprintf(stderr, "Unable to call genlmsg_put()\n");
        nlmsg_free(msg);
        return NULL;
    }

    return msg;
}

int SamplingThread::sendReadRequest(void)
{
    struct nl_msg *msg;
    int ret;

    msg = allocMsg(KOSAGI_CMD_READ);
    if (!msg)
        return -1;

    ret = nl_send_auto(handle, msg);
    if (ret < 0) {
        fprintf(stderr, "Unable to send msg: %s\n", nl_geterror(ret));
        nlmsg_free(msg);
        return ret;
    }
    nlmsg_free(msg);
    return 0;
}

int SamplingThread::doReadRequest(void)
{
    int ret;
    struct sockaddr_nl nla;
    struct genlmsghdr *ghdr;

    if (nhdr)
        free(nhdr);
    nhdr = NULL;

    ret = nl_recv(handle, &nla, (unsigned char **)&nhdr, NULL);
    if (ret < 0) {
        fprintf(stderr, "Unable to receive data: %s\n", nl_geterror(ret));
        nhdr = NULL;
        return -1;
    }

    ghdr = (genlmsghdr *)nlmsg_data((const nlmsghdr *)nhdr);
    bufferData = (quint32 *)genlmsg_user_data(ghdr, 0);
    bufferDataPtr = (quint8 *)bufferData + 8;

    if (genlmsg_len(ghdr) != DATA_SIZE) {
        fprintf(stderr, "Warning: Wanted %d bytes, read %d bytes\n",
            DATA_SIZE, genlmsg_len(ghdr));
        nhdr = NULL;
        free(nhdr);
        return -1;
    }

    bufferDataSize = genlmsg_len(ghdr);
    return 0;
}

void SamplingThread::sample(double elapsed)
{
    if ( d_frequency > 0.0 )
    {
        /* Read more data, if necessary */
        if (bufferDataSize < 1) {
            if (sendReadRequest())
                return;
            if (doReadRequest())
                return;
        }

        const QPointF s(elapsed, *bufferDataPtr++);
        bufferDataSize--;

        SignalData::instance().append(s);

        /* Skip channel #2 */
        if (!(bufferDataSize & 7)) {
            bufferDataSize -= 8;
            bufferDataPtr += 8;
        }
    }
}
