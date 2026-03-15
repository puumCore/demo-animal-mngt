#ifndef JSON_MIDDLEWARE_H_INCLUDED
#define JSON_MIDDLEWARE_H_INCLUDED

#pragma once
#include "crow_all.h"

struct JsonMiddleware {
    struct context {};

    void before_handle(crow::request& req, crow::response& res, context& ctx) {
        // nothing needed before
    }

    void after_handle(crow::request& req, crow::response& res, context& ctx) {
        res.add_header("Content-Type", "application/json");
    }
};

#endif // JSON_MIDDLEWARE_H_INCLUDED
