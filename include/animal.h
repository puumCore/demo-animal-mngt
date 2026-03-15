#ifndef ANIMAL_H_INCLUDED
#define ANIMAL_H_INCLUDED

#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;

struct Animal {
    int id;
    string name;
    string breed;
    int age;
    string owner;
};

inline json animalToJson(const Animal& a) {
    return {
        {"id",    a.id},
        {"name",  a.name},
        {"breed", a.breed},
        {"age",   a.age},
        {"owner", a.owner}
    };
}

#endif // ANIMAL_H_INCLUDED
