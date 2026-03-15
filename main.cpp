#include <iostream>
#include <map>
#include <string>
#include <atomic>
#include <cstdlib>
#include <stdexcept>
#include <nlohmann/json.hpp>
#include "include/env.h"
#include "include/crow_all.h"
#include "include/animalrepo.h"
#include "include/animal.h"
#include "include/json_middleware.h"

using json = nlohmann::json;
using namespace std;

int main() {
    load_env_file(".env");  // call this first, before anything else

    const char* service_port  = getenv("SERVER_PORT");

    const char* pg_host  = getenv("PG_HOST");
    const char* pg_port  = getenv("PG_PORT");
    const char* pg_db  = getenv("PG_DB_NAME");
    const char* pg_user  = getenv("PG_USERNAME");
    const char* pg_pwd  = getenv("PG_PWD");

    cout << "Hello world!" << endl;

    crow::App<JsonMiddleware> app;

    string dbConnUrl = format("host={} port={} dbname={} user={} password={}", pg_host, pg_port, pg_db,pg_user, pg_pwd);

    AnimalRepository repo(dbConnUrl);

    CROW_ROUTE(app, "/api/v1/all").methods(crow::HTTPMethod::Get)
    ([&]() {
        try {
            auto animals = repo.getAll();

            json arr  = json::array();

            for (const auto& a : animals) arr.push_back(animalToJson(a));

            return crow::response(200, arr.dump());
        } catch (const exception& e) {
            return crow::response(500, string(R"({"error":")") + e.what() + R"("})");
        }
    });

    CROW_ROUTE(app, "/api/v1/add").methods(crow::HTTPMethod::Post)
    ([&](const crow::request& req) {
        try {
            auto body = json::parse(req.body, nullptr, false);
            if (body.is_discarded())
                return crow::response(400, R"({"error":"Invalid JSON"})");

            if (!body.contains("name") || !body.contains("breed") ||
                    !body.contains("age")  || !body.contains("owner"))
                return crow::response(400, R"({"error":"Missing required fields"})");

            Animal a;
            a.name  = body["name"].get<string>();
            a.breed = body["breed"].get<string>();
            a.age   = body["age"].get<int>();
            a.owner = body["owner"].get<string>();

            auto created = repo.create(a);

            return crow::response(201, animalToJson(created).dump());
        } catch (const exception& e) {
            return crow::response(500, string(R"({"error":")") + e.what() + R"("})");
        }
    });

    app.port(stoi(service_port)).multithreaded().run();

    return 0;
}
