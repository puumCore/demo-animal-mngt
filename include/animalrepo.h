#ifndef ANIMALSERVICE_H_INCLUDED
#define ANIMALSERVICE_H_INCLUDED

#include <vector>
#include <optional>
#include "animal.h"

using namespace std;

class AnimalRepository {
public:
    explicit AnimalRepository(const string& connString);

    vector<Animal>       getAll();
    optional<Animal>     getById(int id);
    Animal               create(const Animal& a);
    Animal               update(int id, const Animal& a);
    bool                 remove(int id);

private:
    string connStr;
};

#endif // ANIMALSERVICE_H_INCLUDED
