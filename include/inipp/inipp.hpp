#pragma once

#include <cstdint>
#include <istream>
#include <unordered_map>
#include <string>
#include <regex>
#include <sstream>
#include <utility>

namespace inipp
{
    enum class ItemType { Int64, Float, Str };
    
    class Ini
    {
        friend std::ostream& operator<<(std::ostream& stream, const inipp::Ini& ini);
        
    public:
        class Item
        {
            union Data
            {
                int64_t* as_int64 = nullptr;
                double* as_float;
                std::string* as_str;
            } data;

            ItemType data_type;

            void setData(int64_t val) { data.as_int64 = new int64_t(val); data_type = ItemType::Int64; }
            void setData(double val) { data.as_float = new double(val); data_type = ItemType::Float; }
            void setData(const std::string& val) { data.as_str = new std::string(val); data_type = ItemType::Str; }

        public:
            explicit Item(int64_t val): data_type() { setData(val); }
            explicit Item(double val): data_type() { setData(val); }
            explicit Item(const std::string& val): data_type() { setData(val); }
            Item(): data_type(ItemType::Str) 
            { 
                setData("-- not found data, the ini object has inserted it automatically --"); 
            }
            ~Item()
            {
                switch (data_type)
                {
                case ItemType::Int64:
                    delete data.as_int64;
                    break;
                case ItemType::Float:
                    delete data.as_float;
                    break;
                case ItemType::Str:
                    delete data.as_str;
                    break;
                }
            }

            Item& operator=(const Item& o)
            {
                this->~Item();

                switch (o.type())
                {
                case ItemType::Int64:
                    setData(o.asInt64());
                    break;
                case ItemType::Float:
                    setData(o.asFloat());
                    break;
                case ItemType::Str:
                    setData(o.asStr());
                    break;
                }

                return *this;
            }

            Item& operator=(Item&& o) noexcept
            {
                this->~Item();

                switch (o.type())
                {
                case ItemType::Int64:
                    data.as_int64 = o.data.as_int64;
                    break;
                case ItemType::Float:
                    data.as_float = o.data.as_float;
                    break;
                case ItemType::Str:
                    data.as_str = o.data.as_str;
                    break;
                }

                o.data.as_int64 = nullptr;

                return *this;
            }

            Item(const Item& o): data_type() { *this = o; }
            Item(Item&& o) noexcept: data_type() { *this = o; }

            int64_t asInt64() const { return *data.as_int64; }
            double asFloat() const { return *data.as_float; }
            std::string asStr() const { return *data.as_str; }
            ItemType type() const { return data_type; }
        };
        typedef std::unordered_map<std::string, Item> Section;
        typedef std::unordered_map<std::string, Section> Content;
        
    private:
        Content content{};

    public:
        
        std::string unnamed = "unnamed";
        
        Ini() = default;
        explicit Ini(std::istream& stream, std::string unnamed = "unnamed"): unnamed(std::move(unnamed))
        {
            open(stream);
        }

        void open(std::istream& stream)
        {
            content.clear();

            content.insert(std::make_pair(unnamed, Section{}));
            auto* current = &content[unnamed];
            
            std::string line;

            std::regex sectionParser(R"(^\s*\[([^\[\];]*)\]\s*(;|$))");
            std::regex kvParser(R"(^\s*([^=;]+)\s*=\s*([^=;]+)\s*(;|$))");
            std::regex strParser(R"(^"(.*)\"$)");
            std::regex intRule(R"(^(\+|-|)[0-9]+$)");
            std::regex floatRule(R"(^(\+|-|)([0-9]*\.[0-9]+|[0-9]+\.[0-9]*|[0-9]+)(e(\+|-|)[0-9]+|)$)");

            while (!stream.eof())
            {
                std::getline(stream, line);
                std::smatch result; 
                if (std::regex_match(line, result, sectionParser))
                {
                    content.insert(std::make_pair(result[1], Section{}));
                    current = &content[result[1]];
                }
                else if (std::regex_match(line, result, kvParser))
                {
                    std::string k = result[1];
                    std::string v = result[2];
                    if (std::regex_match(v, result, strParser))
                    {
                        v = result[1];
                        current->insert(std::make_pair(k, v));
                    }
                    else if (std::regex_match(v, intRule))
                    {
                        std::stringstream ss(v);
                        int64_t tmp;
                        ss >> tmp;
                        current->insert(std::make_pair(k, tmp));
                    }
                    else if (std::regex_match(v, floatRule))
                    {
                        std::stringstream ss(v);
                        double tmp;
                        ss >> tmp;
                        current->insert(std::make_pair(k, tmp));
                    }
                    else
                    {
                        current->insert(std::make_pair(k, v));
                    }
                }
            }
        }

        Content::iterator find(const std::string& sectionName) { return content.find(sectionName); }
        Content::const_iterator find(const std::string& sectionName) const { return content.find(sectionName); }
        Section& operator[](const std::string& sectionName) { return content[sectionName]; }
        
        Content::iterator end() { return content.end(); }
        Content::const_iterator end() const { return content.end(); }
        Content::iterator begin() { return content.begin(); }
        Content::const_iterator begin() const { return content.begin(); }
    };
    
    inline std::ostream& operator<<(std::ostream& stream, const Ini::Item& item)
    {
        switch (item.type())
        {
        case inipp::ItemType::Int64:
            stream << item.asInt64();
            break;
        case inipp::ItemType::Float:
            stream << item.asFloat();
            break;
        case inipp::ItemType::Str:
            stream << '"' << item.asStr() << '"';
            break;
        }
        return stream;
    }

    inline std::ostream& operator<<(std::ostream& stream, const Ini& ini)
    {
        for (const auto& sec : ini.content)
        {
            stream << "[" << sec.first << "]" << std::endl;
            for (const auto& kv : sec.second)
            {
                stream << kv.first << "=" << kv.second << std::endl;
            }
            stream << std::endl;
        }
        return stream;
    }

    inline std::istream& operator>>(std::istream& stream, Ini& ini)
    {
        ini.open(stream);
        return stream;
    }

}


