#include <iostream>
#include "ResultWriter.h"
#include <cassert>
#include <mongocxx/client.hpp>
#include <bsoncxx/json.hpp>


/*Функуия test_key_spliter тестирует, функцию key_spliter описанную в классе ResultWriter,
 *предоставляются исходные данные, после вызова функции полученный вектор проверяется,
 *на корректность, путем последовательного сравнения, каждого элемента с каждым
 */
void test_key_spliter()
{
    auto keys = ResultWriter::key_spliter("a.b.c");
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


/*
 * Функция test_append_kvp_variant проверяет корректность работы функции append_kvp_variant,
 * которая добавляет значения различных типов (int64_t, double, string) в BSON-документ.
 * После добавления данные извлекаются и сравниваются с ожидаемыми значениями.
 */
void test_append_kvp_variant() {
    using bsoncxx::builder::basic::document;

    document builder;
    ResultWriter::append_kvp_variant(builder, "int", int64_t{42});
    ResultWriter::append_kvp_variant(builder, "double", 3.14);
    ResultWriter::append_kvp_variant(builder, "string", std::string("hello"));

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


/*
 * Функция test_ResultWriter_constructor_and_write делает проверку корректности записи данных
 * metadata и results, в ней есть исходные данные, которые передаются в конструктор класса и в метод write,
 * после чего записанные данные получаются и сравниваются с исходными.
 */

void test_ResultWriter_constructor_and_write(mongocxx::client& client) {
    using namespace bsoncxx::builder::basic;

    auto collection = client["test"]["results"];
    collection.delete_many({});

    std::unordered_map<std::string, value_type> metadata = {
        {"meta1.level1", "info"},
        {"std.level2.inner", 123},
        {"m.level2.deep.value", 3.14},
        {"description", "test document"},
        {"version", int64_t{2}}
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

    auto metadata_view = view["metadata"].get_document().view();

    bool metadata_flag =
        metadata_view["meta1"].get_document().view()["level1"].get_string().value.to_string() == "info" &&
        metadata_view["std"].get_document().view()["level2"].get_document().view()["inner"].get_int64() == 123 &&
        metadata_view["m"].get_document().view()["level2"].get_document().view()["deep"]
            .get_document().view()["value"].get_double() == 3.14 &&
        metadata_view["description"].get_string().value.to_string() == "test document" &&
        metadata_view["version"].get_int64() == 2;

    auto results_array = view["results"].get_array().value;
    auto inserted_result = results_array.begin()->get_document().view();

    bool results_flag =
        inserted_result["a"].get_int64() == 10 &&
        inserted_result["b"].get_document().view()["inner"].get_double() == 3.14 &&
        inserted_result["c"].get_string().value.to_string() == "value";

    if (metadata_flag && results_flag) {
        std::cout << "ResultWriter Constructor + Method .write: WORKS CORRECTLY"<<std::endl;
    } else {
        std::cout << "Constructor of Method .write: ERROR" << std::endl;
        if (!metadata_flag)
            std::cout << "Metadata check failed"<< std::endl;
        if (!results_flag)
            std::cout << "Results check failed" << std::endl;
    }
}