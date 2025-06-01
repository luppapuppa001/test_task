#include <mongocxx/instance.hpp>
#include "ResultWriter.h"
#include <map>
#include <string>

int main()
{
    mongocxx::instance inst{};
    mongocxx::client connection{mongocxx::uri{}};

    std::map<std::string, value_type> metadata = {
        {"key1", 2},
        {"key2.inner_key", 2.0},
        {"key3", "value3"}
    };

    ResultWriter writer(connection, metadata);

    std::map<std::string, value_type> result1 = {
        {"key1", 1},
        {"key2", 2},
        {"key3.boom", 3}
    };

    writer.write(result1);

    std::map<std::string, value_type> result2 = {
        {"key1.q.e.r", 234.2},
        {"key2.tttt", 5},
        {"key3", .32332}
    };

    writer.write(result2);

    std::map<std::string, value_type> result3 = {
        {"key5.q44", 234.2},
        {"key6.tt", 5},
        {"key7.234", "sfdfdsfds"}
    };

    writer.write(result3);
}
