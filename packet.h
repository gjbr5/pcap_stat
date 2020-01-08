#pragma once
#include <iostream>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

typedef struct ether
{
    uint8_t addr[ETH_ALEN];

    ether(const struct ether_addr &e);
    ether(const uint8_t e[ETH_ALEN]);

    bool operator<(const struct ether &o) const;
    friend std::ostream &operator<<(std::ostream &os, const ether &eth);
} ether;

typedef struct sock
{
    in_addr_t ip;
    in_port_t port;

    sock(in_addr_t ip, in_port_t port);

    bool operator<(const struct sock &o) const;
    friend std::ostream &operator<<(std::ostream &os, const sock &sock);
} sock;
