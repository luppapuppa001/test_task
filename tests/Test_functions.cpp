#include <iostream>
#include "ResultWriter.h"
#include <cassert>
#include <mongocxx/client.hpp>
#include <bsoncxx/json.hpp>



void test_key_spliter()
{
    auto keys = key_spliter("a.b.c");
    {
        if ((keys[0] == "a") and (keys[1] == "b") and (keys[2] == "c")){
            std::cout << "Function key_spliter: WORKS CORRECTLY" <<std::endl;
        }
        else
        {
            std::cout << "Function key_spliter: ERROR"<<std::endl;
        }
    }
}

void test_append_kvp_variant() {
    using bsoncxx::builder::basic::document;

    document builder;
    append_kvp_variant(builder, "int", int64_t{42});
    append_kvp_variant(builder, "double", 3.14);
    append_kvp_variant(builder, "string", std::string("hello"));

    auto doc = builder.extract();
    auto view = doc.view();

    bool flag =
        view["int"].get_int64() == 42 &&
            view["double"].get_double() == 3.14 &&
                view["string"].get_string().value.to_string() == "hello";
    if (flag) {
        std::cout << "Function append_kvp_variant_basic: WORKS CORRECTLY"<< std::endl;
    } else {
        std::cout << "Function append_kvp_variant_basic: ERROR"<<std::endl;
    }
}

void test_result_writer_write(mongocxx::client& client) {
    using namespace bsoncxx::builder::basic;

    auto collection = client["test"]["results"];
    collection.delete_many({});

    std::unordered_map<std::string, value_type> metadata = {
        {"meta", "data"}
    };

    std::unordered_map<std::string, value_type> result = {
        {"a", 10},
        {"b.inner", 3.14},
        {"c", "value"}
    };

    ResultWriter writer(client, metadata);
    writer.write(result);

    auto doc_opt = collection.find_one({});
    assert(doc_opt);
    auto view = doc_opt->view();

    auto results_array = view["results"].get_array().value;
    auto inserted_result = results_array.begin()->get_document().view();

    document expected_builder;
    expected_builder.append(kvp("a", 10));
    expected_builder.append(kvp("b", make_document(kvp("inner", 3.14))));
    expected_builder.append(kvp("c", "value"));

    auto expected_view = expected_builder.extract();

    bool flag =
     inserted_result["a"].get_int64() == 10 &&
     inserted_result["b"].get_document().view()["inner"].get_double() == 3.14 &&
     inserted_result["c"].get_string().value.to_string() == "value";

    if (flag) {
        std::cout << "Method test_result_writer_write: WORKS CORRECTLY"<<std::endl;
    } else
    {
        std::cout << "Method test_result_writer_write: ERROR"<<std::endl;
    }
}