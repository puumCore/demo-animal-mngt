#ifndef ANIMALSERVICE_H_INCLUDED
#define ANIMALSERVICE_H_INCLUDED

#include <vector>
#include <optional>
#include "animal.h"
#include "connection_pool.h"

using namespace std;

class AnimalRepository {

public:
    explicit AnimalRepository(ConnectionPool& pool);

    vector<Animal>       getAll();
    optional<Animal>     getById(int id);
    Animal               create(const Animal& a);
    Animal               update(int id, const Animal& a);
    bool                 remove(int id);

private:
    ConnectionPool& pool_;
};

#endif // ANIMALSERVICE_H_INCLUDED
