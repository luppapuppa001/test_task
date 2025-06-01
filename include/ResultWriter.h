#pragma once
#include <ranges>
#include <variant>
#include <string>
#include <tuple>
#include <vector>
#include <iostream>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/uri.hpp>

template<typename Key, typename Value, typename R>
concept KVRange =
        std::ranges::range<R> &&
        requires(R r)
{
    { std::get<0>(*std::ranges::begin(r)) } -> std::convertible_to<Key>;
    { std::get<1>(*std::ranges::begin(r)) } -> std::convertible_to<Value>;
};

using value_type = std::variant<std::int64_t, double, std::string>;

template<typename T>
concept Dictionary = KVRange<std::string, value_type, T>;

using Connection = mongocxx::client&;

inline std::vector<std::string> key_spliter(const std::string& key)
{
    std::vector<std::string> stringHolder;
    stringHolder.push_back("");
    for (auto it = key.begin(); it != key.end(); ++it)
    {
        if (*it == '.')
            stringHolder.push_back("");
        else
            stringHolder.back() += *it;
    }
    return stringHolder;
}

template<typename Builder>
void append_kvp_variant(Builder& builder, const std::string& key, const value_type& value) {
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int64_t>) {
            builder.append(bsoncxx::builder::basic::kvp(key, arg));
        } else if constexpr (std::is_same_v<T, double>) {
            builder.append(bsoncxx::builder::basic::kvp(key, arg));
        } else if constexpr (std::is_same_v<T, std::string>) {
            builder.append(bsoncxx::builder::basic::kvp(key, arg));
        } else {
            throw std::runtime_error("Unsupported type in variant");
        }
    }, value);
}

class ResultWriter
{
    mongocxx::collection collection_;
    bsoncxx::oid document_id_;

public:
    /**
     * adssadas
     * @tparam T adsa
     * @param connection
     * @param metadata
     * @return
     */
    template<Dictionary T>
    explicit ResultWriter(Connection connection, const T &metadata)
    {
        collection_ = connection["test"]["results"];
        //collection_.delete_many({});


        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::make_document;

        auto builder = bsoncxx::builder::basic::document{};
        for (const auto& [key, value] : metadata)
        {
            std::vector<std::string> keys = key_spliter(key);
            if (keys.size() == 1)
            {
                append_kvp_variant(builder, keys[0], value);
            }
            else
            {
                bsoncxx::builder::basic::document nested_builder{};
                append_kvp_variant(nested_builder, keys.back(), value);
                bsoncxx::document::value nested_doc = nested_builder.extract();
                for (int i = static_cast<int>(keys.size()) - 2; i > 0; --i)
                {
                    bsoncxx::builder::basic::document outer_builder{};
                    outer_builder.append(kvp(keys[i], nested_doc.view()));
                    nested_doc = outer_builder.extract();
                }
                builder.append(kvp(keys[0], nested_doc.view()));
            }
        }

        auto doc = make_document(
            kvp("metadata", builder.extract()),
            kvp("results", bsoncxx::builder::basic::array{})
        );

        // Сохраняем документ в коллекцию
        collection_ = (connection)["test"]["results"];
        auto result = collection_.insert_one(doc.view());
        document_id_ = result->inserted_id().get_oid().value;

        std::cout << "Inserted metadata document with _id: " << document_id_.to_string() << std::endl;
    }

    template<Dictionary T>
    void write(const T &result)
    {
        using bsoncxx::builder::basic::kvp;
        bsoncxx::builder::basic::document builder;

        for (const auto& [key, value] : result)
        {
            std::vector<std::string> keys = key_spliter(key);
            if (keys.size() == 1)
            {
                append_kvp_variant(builder, keys[0], value);
            }
            else
            {
                bsoncxx::builder::basic::document nested_builder{};
                append_kvp_variant(nested_builder, keys.back(), value);
                bsoncxx::document::value nested_doc = nested_builder.extract();
                for (int i = static_cast<int>(keys.size()) - 2; i > 0; --i)
                {
                    bsoncxx::builder::basic::document outer_builder{};
                    outer_builder.append(kvp(keys[i], nested_doc.view()));
                    nested_doc = outer_builder.extract();
                }
                builder.append(kvp(keys[0], nested_doc.view()));
            }
        }

        bsoncxx::document::value result_doc = builder.extract();

        collection_.update_one(
            bsoncxx::builder::basic::make_document(kvp("_id", document_id_)),
            bsoncxx::builder::basic::make_document(
                kvp("$push", bsoncxx::builder::basic::make_document(
                    kvp("results", result_doc.view())
                ))
            )
        );
        std::cout << "Appended result to document _id: " << document_id_.to_string() << std::endl;
    }
};
