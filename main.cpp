#include "statistics.h"
#include <iostream>

int main(int argc, char **argv)
{
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <pcap_file>\n";
        return 0;
    }

    try {
        statistics stat(argv[1]);
        stat.analyse();
        std::cout << stat;
    } catch (char *err) {
        std::cout << err << std::endl;
    }

    return 0;
}
