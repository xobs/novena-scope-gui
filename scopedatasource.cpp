#include <netlink/genl/genl.h>
#include <netlink/genl/family.h>
#include <netlink/genl/ctrl.h>
#include <netlink/msg.h>
#include <netlink/attr.h>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>
#include <unistd.h>

#include "scopedatasource.h"
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
    KOSAGI_CMD_POWER_OFF,
    KOSAGI_CMD_POWER_ON,
    KOSAGI_CMD_FPGA_ASSERT_RESET,
    KOSAGI_CMD_FPGA_DEASSERT_RESET,
    KOSAGI_CMD_TRIGGER_SAMPLE,
    __KOSAGI_CMD_MAX,
};

/* list of valid command attributes */
enum kosagi_fpga_attributes {
    KOSAGI_ATTR_NONE,
    KOSAGI_ATTR_FPGA_DATA,
    KOSAGI_ATTR_MESSAGE,
    __KOSAGI_ATTR_MAX,
};

ScopeDataSource::ScopeDataSource(QObject *parent):
    QObject(parent),
    d_frequency(5),
    d_amplitude(20)
{
    handle = NULL;
    cache = NULL;
    id = NULL;
    nhdr = NULL;
    bufferData = NULL;
    bufferDataSize = 0;

    //    pll = new Ad9520();
    //    adc = new Adc08d1020();
    //    dac = new Dac101c085();
    //    vga = new Lmh6518();

    openDevice();
}

ScopeDataSource::~ScopeDataSource()
{
    if (nhdr)
        free(nhdr);
    delete vga;
    delete dac;
    delete adc;
    delete pll;
}

void ScopeDataSource::setFrequency(int frequency)
{
    d_frequency = frequency;
}

int ScopeDataSource::frequency() const
{
    return d_frequency;
}

void ScopeDataSource::setAmplitude(int amplitude)
{
    d_amplitude = amplitude;
}

int ScopeDataSource::amplitude() const
{
    return d_amplitude;
}

void ScopeDataSource::setDacOffset(int offset)
{
    dac->setOffset(offset);
}

void ScopeDataSource::setDacTrigger(int trigger)
{
    dac->setTriggerLevel(trigger);
}

void ScopeDataSource::setAfeAttenuation(int attenuation)
{
    vga->setAttenuation(attenuation);
}

void ScopeDataSource::setAfeFilter(int filter)
{
    vga->setFilter(filter);
}

bool ScopeDataSource::openDevice(void)
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

#if 0
    pll->selfConfig(Ad9520::Speed500Mhz);
    adc->setDefaults();
    adc->calibrate();
    vga->setAuxPower(true);
    dac->setOffset(0x700);
    vga->setFilter(20);
    vga->setAttenuation(20);
#endif

    return true;
}

struct nl_msg *ScopeDataSource::allocMsg(int cmd)
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

int ScopeDataSource::sendReadRequest(void)
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

int ScopeDataSource::doReadRequest(void)
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

    if (genlmsg_len(ghdr) != DATA_SIZE) {
        fprintf(stderr, "Warning: Wanted %d bytes, read %d bytes\n",
            DATA_SIZE, genlmsg_len(ghdr));
        nhdr = NULL;
        free(nhdr);
        return -1;
    }

    bufferDataSize = genlmsg_len(ghdr);
    bufferData = (quint32 *)genlmsg_user_data(ghdr, 0);
    bufferDataPtr = (quint8 *)bufferData;

    return 0;
}

int ScopeDataSource::getData(int samples)
{
  quint16 *adc_samples;
  unsigned short sample;
  unsigned short swap;
    QByteArray channel1, channel2;

    if (sendReadRequest())
        return -1;
    if (doReadRequest())
        return -1;

    while (bufferDataSize > 0) {
      sample = *adc_samples++;
      channel1.append(sample);
      bufferDataSize -= sizeof(*adc_samples);
    }

    //    emit scopeData(channel1, channel2);
    emit scopeData(channel1, channel2);
    return 0;
}
