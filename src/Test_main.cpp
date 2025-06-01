#include "Test_functions.h"
#include <mongocxx/instance.hpp>
#include "ResultWriter.h"

int main()
{
    mongocxx::instance inst{};
    mongocxx::client connection{mongocxx::uri{}};


    test_key_spliter();
    test_append_kvp_variant();
    test_result_writer_write(connection);
    return 0;
}















/*
*void test_key_spliter()
{
auto keys = key_spliter("a.b.c");
{
if ((keys[0] == "a") and (keys[1] == "b") and (keys[2] == "c")){
std::cout << "Function key_spliter, worcs correctly" <<std::endl;
}
else
{
std::cout << "Function key_spliter, ERROR"<<std::endl;
}
}
}

*/