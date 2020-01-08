#pragma once
#include "capture.h"
#include <map>

class statistics
{
private:
    capture cap;

    template<class K, class V>
    std::map<K, V> &get_map() const;

    template<class T>
    void print_endpoints(std::ostream &os) const;

    template<class T>
    void print_conversations(std::ostream &os) const;

    template<class T>
    void endpoints(const T &e, int caplen, bool tx);

    template<class T>
    void conversations(const T &src, const T &dst, int caplen);

public:
    statistics(const char *file);
    void analyse();
    friend std::ostream &operator<<(std::ostream &os, const statistics &s);
};
