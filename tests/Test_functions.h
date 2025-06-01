#pragma once
#include <mongocxx/client.hpp>

void test_key_spliter();
void test_append_kvp_variant();
void test_ResultWriter_constructor_and_write(mongocxx::client& connection);