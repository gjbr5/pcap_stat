#include "capture.h"

capture::capture(const char *file)
{
    handle = pcap_open_offline(file, errbuf);
    if (handle == nullptr)
        throw errbuf;
}

const uint8_t *capture::next(int &caplen)
{
    struct pcap_pkthdr *header;
    const uint8_t *p;
    int ret = pcap_next_ex(handle, &header, &p);
    caplen = header->caplen;
    if (ret <= 0)
        return nullptr;
    return p;
}

capture::~capture()
{
    pcap_close(handle);
}
