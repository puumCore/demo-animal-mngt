#pragma once
#include <string>
#include <pqxx/pqxx>
#include <iostream>
#include "../include/animal.h"
#include "../include/animalrepo.h"
#include "../include/connection_pool.h"
#include <stdexcept>

using namespace std;

AnimalRepository::AnimalRepository(ConnectionPool& pool) : pool_(pool) {}

static Animal rowToAnimal(const pqxx::row& row) {
    Animal a;
    a.id    = row["id"].as<int>();
    a.name  = row["name"].as<string>();
    a.breed = row["breed"].as<string>();
    a.age   = row["age"].as<int>();
    a.owner = row["owner"].as<string>();
    return a;
}

vector<Animal> AnimalRepository::getAll() {
    vector<Animal> animals;
    auto datasource = pool_.acquire();
    pqxx::work txn(*datasource);

    try {
        pqxx::result rows = txn.exec("SELECT id, name, breed, age, owner FROM animals ORDER BY id");

        for (const auto& row : rows) {
            animals.push_back(rowToAnimal(row));
        }

        txn.commit();
    } catch (const pqxx::sql_error& e) {
        txn.abort();
        cerr << "SQL error: "    << e.what()  << "\n";
        cerr << "Query was: "    << e.query() << "\n";
    } catch (const exception& e) {
        txn.abort();
        cerr << "Error: " << e.what() << "\n";
        throw;
    }

    return animals;
}

optional<Animal> AnimalRepository::getById(int id) {
    auto datasource = pool_.acquire();
    pqxx::work txn(*datasource);

    try {
        pqxx::result rows = txn.exec_params(
                                "SELECT id, name, breed, age, owner FROM animals WHERE id = $1",
                                id
                            );

        txn.commit();

        if (!rows.empty()) return rowToAnimal(rows[0]);

    } catch (const pqxx::sql_error& e) {
        txn.abort();
        cerr << "SQL error: "    << e.what()  << "\n";
        cerr << "Query was: "    << e.query() << "\n";
    } catch (const exception& e) {
        txn.abort();
        cerr << "Error: " << e.what() << "\n";
        throw;
    }

    return nullopt;
}

Animal AnimalRepository::create(const Animal& a) {
    auto datasource = pool_.acquire();
    pqxx::work txn(*datasource);

    try {
        pqxx::result rows = txn.exec_params(
                                "INSERT INTO animals(name, breed, age, owner) VALUES ($1, $2, $3, $4) RETURNING id, name, breed, age, owner;",
                                a.name, a.breed, a.age, a.owner
                            );

        txn.commit();

        return rowToAnimal(rows[0]);
    } catch (const pqxx::sql_error& e) {
        txn.abort();
        cerr << "SQL error: "    << e.what()  << "\n";
        cerr << "Query was: "    << e.query() << "\n";
        throw;
    } catch (const exception& e) {
        txn.abort();
        cerr << "Error: " << e.what() << "\n";
        throw;
    }
}

Animal AnimalRepository::update(int id, const Animal& a) {
    auto datasource = pool_.acquire();
    pqxx::work txn(*datasource);

    try {
        pqxx::result rows = txn.exec_params(
                                "UPDATE animals SET name=$1, breed=$2, age=$3, owner=$4 "
                                "WHERE id=$5 "
                                "RETURNING id, name, breed, age, owner",
                                a.name, a.breed, a.age, a.owner, id
                            );

        txn.commit();

        if (rows.empty())
            throw runtime_error("animal not found");

        return rowToAnimal(rows[0]);
    } catch (const pqxx::sql_error& e) {
        txn.abort();
        cerr << "SQL error: "    << e.what()  << "\n";
        cerr << "Query was: "    << e.query() << "\n";
        throw;
    } catch (const exception& e) {
        txn.abort();
        cerr << "Error: " << e.what() << "\n";
        throw;
    }
}


bool AnimalRepository::remove(int id) {
    auto datasource = pool_.acquire();
    pqxx::work txn(*datasource);

    try {
        pqxx::result rows = txn.exec_params("DELETE FROM animal WHERE id = $1 RETURNING id", id);

        txn.commit();
        return !rows.empty();   // true = deleted, false = not found
    } catch (const pqxx::sql_error& e) {
        txn.abort();
        cerr << "SQL error: "    << e.what()  << "\n";
        cerr << "Query was: "    << e.query() << "\n";
    } catch (const exception& e) {
        txn.abort();
        cerr << "Error: " << e.what() << "\n";
        throw;
    }

    return false;
}
