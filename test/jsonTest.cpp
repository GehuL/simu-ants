#include <fstream>
#include <iostream>
#include <vector>
#include <array>
#include <string>
#include "../external/json.hpp"

using json = nlohmann::json;

namespace ns {
    class address {
      private:
        std::string street;
        int housenumber;
        int postcode;
      public:
        virtual ~address(){};
        virtual void pure() = 0;
        virtual std::string toString() {return std::to_string(housenumber) + "/" + std::to_string(postcode) + "/" + street; };

        friend void to_json(json& j, const address& p) 
        {
          j["street"] = p.street;
          j["housenumber"] = p.housenumber;
          j["postcode"] = p.postcode;
        }

        friend void from_json(const json& j, address& p)
        {
          j.at("street").get_to(p.street);
          j.at("housenumber").get_to(p.housenumber);
          j.at("postcode").get_to(p.postcode);
        }

    };

    class Bar: public address
    {
    private:

        float test;
        std::string toString() override 
        {
          return address::toString() + "/" + std::to_string(test);
        };

        friend void to_json(json& j, const Bar& p) 
        {
          to_json(j, static_cast<const address&>(p));
          j["test"] = p.test;
        }

        void pure() override {};

        friend void from_json(const json& j, Bar& p)
        {
          from_json(j, static_cast<address&>(p));
          j.at("test").get_to(p.test);
        }

      public:
        explicit Bar(int i = -1) {};
        virtual ~Bar() {};
    };
}

int main()
{
    std::fstream file("save.json", std::ios_base::out);

    std::vector<ns::Bar> houses{};
    houses.push_back(ns::Bar()); 

    json j = houses;
    j += ns::Bar();

    file << j;
    file.close();

    std::fstream in("save.json", std::ios_base::in);
    json json = json::parse(in);

    std::vector<ns::Bar> vec = json;

    for(ns::address& a: vec)
        std::cout << a.toString() << std::endl;
}

