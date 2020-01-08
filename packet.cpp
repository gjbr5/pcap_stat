#include "packet.h"
#include <arpa/inet.h>
#include <iomanip>
#include <string.h>

ether::ether(const struct ether_addr &e)
{
    memcpy(addr, e.ether_addr_octet, ETH_ALEN);
}

ether::ether(const uint8_t e[ETH_ALEN])
{
    memcpy(addr, e, ETH_ALEN);
}

bool ether::operator<(const struct ether &o) const
{
    return memcmp(addr, o.addr, ETH_ALEN) < 0;
}

std::ostream &operator<<(std::ostream &os, const ether &eth)
{
    std::ios_base::fmtflags f = os.flags();

    for (int i = 0; i < ETH_ALEN - 1; i++)
        os << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(eth.addr[i]) << ":";
    os << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(eth.addr[ETH_ALEN - 1]);

    os.flags(f);

    return os;
}

sock::sock(in_addr_t ip, in_port_t port) : ip(ip), port(port) {}

bool sock::operator<(const struct sock &o) const
{
    return ip == o.ip ? port < o.port : ip < o.ip;
}

std::ostream &operator<<(std::ostream &os, const sock &sock)
{
    os << inet_ntoa({sock.ip}) << ":" << ntohs(sock.port);
    return os;
}
