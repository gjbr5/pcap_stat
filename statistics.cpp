#include "statistics.h"
#include <arpa/inet.h>
#include <utility>

typedef struct packet_amount
{
    int packets;
    int bytes;
    struct packet_amount operator+=(int bytes)
    {
        this->packets += 1;
        this->bytes += bytes;
        return *this;
    }
    packet_amount() : packets(0), bytes(0) {}
} amount;

typedef struct stat_data
{
    amount tx;
    amount rx;
} stat_data;

statistics::statistics(const char *file) : cap(file) {}

template<class K, class V>
std::map<K, V> &statistics::get_map() const
{
    static std::map<K, V> map;
    return map;
}

template<class T>
void statistics::endpoints(const T &key, int caplen, bool tx)
{
    std::map<T, stat_data> &map = get_map<T, stat_data>();

    auto it = map.find(key);
    if (it != map.end()) {
        tx ? it->second.tx += caplen : it->second.rx += caplen;
    } else {
        stat_data new_data;
        tx ? new_data.tx += caplen : new_data.rx += caplen;
        map.insert(std::make_pair(key, new_data));
    }
}

template<class T>
void statistics::conversations(const T &src, const T &dst, int caplen)
{
    endpoints(src, caplen, true);
    endpoints(dst, caplen, false);

    std::map<std::pair<T, T>, stat_data> &map = get_map<std::pair<T, T>, stat_data>();

    if (src < dst) {
        std::pair<T, T> key = std::make_pair(src, dst);
        auto it = map.find(key);
        if (it != map.end())
            it->second.tx += caplen;
        else {
            stat_data new_data;
            new_data.tx += caplen;
            map.insert(std::make_pair(key, new_data));
        }
    } else {
        std::pair<T, T> key = std::make_pair(dst, src);
        auto it = map.find(key);
        if (it != map.end())
            it->second.rx += caplen;
        else {
            stat_data new_data;
            new_data.rx += caplen;
            map.insert(std::make_pair(key, new_data));
        }
    }
}

void statistics::analyse()
{
    while (true) {
        // Capture
        int caplen = 0;
        const uint8_t *p = cap.next(caplen);
        if (p == nullptr)
            break;

        int offset = 0;

        // Ethernet
        const ether_header *eth = reinterpret_cast<const ether_header *>(p);
        offset += sizeof(ether_header);
        conversations<ether>(eth->ether_shost, eth->ether_dhost, caplen);
        if (eth->ether_type != ntohs(ETHERTYPE_IP))
            continue;

        // IP
        const iphdr *ip = reinterpret_cast<const iphdr *>(p + offset);
        offset += ip->ihl << 2;
        conversations<in_addr_t>(ip->saddr, ip->daddr, caplen);
        if (ip->protocol != IPPROTO_TCP)
            continue;

        // TCP
        const tcphdr *tcp = reinterpret_cast<const tcphdr *>(p + offset);
        offset += tcp->th_off << 2;
        conversations<sock>(sock(ip->saddr, tcp->source), sock(ip->daddr, tcp->dest), caplen);
    }
}

template<class T>
void statistics::print_endpoints(std::ostream &os) const
{
    std::map<T, stat_data> &endpoints = get_map<T, stat_data>();
    os << "---- EndPoint(" << endpoints.size() << ") ----\n";

    os << "Address\t\t\tPkts\tBytes\tTxPkts\tTxBytes\tRxPkts\tRxBytes\n";

    for (auto &i : endpoints) {
        stat_data &stat = i.second;
        os << i.first;
        os << "\t" << stat.tx.packets + stat.rx.packets << "\t" << stat.tx.bytes + stat.rx.bytes;
        os << "\t" << stat.tx.packets << "\t" << stat.tx.bytes;
        os << "\t" << stat.rx.packets << "\t" << stat.rx.bytes << "\n";
    }
}

template<>
void statistics::print_endpoints<in_addr_t>(std::ostream &os) const
{
    std::map<in_addr_t, stat_data> &endpoints = get_map<in_addr_t, stat_data>();
    os << "---- EndPoint(" << endpoints.size() << ") ----\n";

    os << "Address\t\tPkts\tBytes\tTxPkts\tTxBytes\tRxPkts\tRxBytes\n";

    for (auto &i : endpoints) {
        stat_data &stat = i.second;
        os << inet_ntoa({i.first});
        os << "\t" << stat.tx.packets + stat.rx.packets << "\t" << stat.tx.bytes + stat.rx.bytes;
        os << "\t" << stat.tx.packets << "\t" << stat.tx.bytes;
        os << "\t" << stat.rx.packets << "\t" << stat.rx.bytes << "\n";
    }
}

template<class T>
void statistics::print_conversations(std::ostream &os) const
{
    std::map<std::pair<T, T>, stat_data> &conversations = get_map<std::pair<T, T>, stat_data>();
    os << "---- Conversation(" << conversations.size() << ") ----\n";

    os << "Address A\t\tAddress B\t\tPkts\tBytes\tABPkts\tABBytes\tBAPkts\tBABytes\n";
    for (auto &i : conversations) {
        os << i.first.first << "\t" << i.first.second;
        os << "\t" << i.second.tx.packets + i.second.rx.packets << "\t" << i.second.tx.bytes + i.second.rx.bytes;
        os << "\t" << i.second.tx.packets << "\t" << i.second.tx.bytes;
        os << "\t" << i.second.rx.packets << "\t" << i.second.rx.bytes << "\n";
    }
}

template<>
void statistics::print_conversations<in_addr_t>(std::ostream &os) const
{
    std::map<std::pair<in_addr_t, in_addr_t>, stat_data> &conversations
        = get_map<std::pair<in_addr_t, in_addr_t>, stat_data>();
    os << "---- Conversation(" << conversations.size() << ") ----\n";

    os << "Address A\tAddress B\tPkts\tBytes\tABPkts\tABBytes\tBAPkts\tBABytes\n";

    for (auto &i : conversations) {
        os << inet_ntoa({i.first.first}) << "\t" << inet_ntoa({i.first.second});
        os << "\t" << i.second.tx.packets + i.second.rx.packets << "\t" << i.second.tx.bytes + i.second.rx.bytes;
        os << "\t" << i.second.tx.packets << "\t" << i.second.tx.bytes;
        os << "\t" << i.second.rx.packets << "\t" << i.second.rx.bytes << "\n";
    }
}

std::ostream &operator<<(std::ostream &os, const statistics &s)
{
    os << "**** Ethernet ****\n";
    s.print_endpoints<ether>(os);
    s.print_conversations<ether>(os);

    os << '\n';

    os << "**** IP ****\n";
    s.print_endpoints<in_addr_t>(os);
    s.print_conversations<in_addr_t>(os);

    os << '\n';

    os << "**** TCP ****\n";
    s.print_endpoints<sock>(os);
    s.print_conversations<sock>(os);
    return os;
}
