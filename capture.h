#pragma once
#include "packet.h"
#include <pcap.h>

class capture
{
private:
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *handle;

public:
    capture(const char *file);
    const uint8_t *next(int &caplen);

    ~capture();
};
