#ifndef CONNECTIONPOOL_H_INCLUDED
#define CONNECTIONPOOL_H_INCLUDED

#pragma once
#include <pqxx/pqxx>
#include <mutex>
#include <condition_variable>
#include <stdexcept>
#include <chrono>

using namespace std;

class ConnectionPool {

public:
    ConnectionPool(const string& connStr, int maxSize = 10)
        : connStr_(connStr), available_(maxSize) {}

    // RAII handle - move-only, returns a slot to the pool on destruction
    struct Handle {
        pqxx::connection* conn  = nullptr;
        ConnectionPool*   owner = nullptr;

        Handle() = default;
        Handle(pqxx::connection* c, ConnectionPool* o) : conn(c), owner(o) {}

        // Non-copyable
        Handle(const Handle&)            = delete;
        Handle& operator=(const Handle&) = delete;

        // Movable: nullifies the source so its destructor becomes a no-op
        Handle(Handle&& other) noexcept
            : conn(other.conn), owner(other.owner) {
            other.conn = nullptr;
            other.owner = nullptr;
        }

        Handle& operator=(Handle&& other) noexcept {
            if (this != &other) {
                if (owner && conn) owner->release(conn);
                conn  = other.conn;
                owner = other.owner;
                other.conn  = nullptr;
                other.owner = nullptr;
            }
            return *this;
        }

        ~Handle() {
            if (owner && conn) owner->release(conn);
        }

        pqxx::connection* operator->() { return conn; }
        pqxx::connection& operator*()  { return *conn; }
    };

    Handle acquire() {
        unique_lock<mutex> lock(mu_);

        // Wait up to 5 seconds for a slot
        bool available = cv_.wait_for(lock, chrono::seconds(5),
                                      [this] { return available_ > 0; });
        if (!available)
            throw runtime_error("Connection pool exhausted (timed out after 5s)");

        --available_;
        lock.unlock();

        // Always create a fresh connection — no reuse
        auto conn = new pqxx::connection(connStr_);
        return Handle{conn, this};
    }

private:
    void release(pqxx::connection* conn) {
        // Close the connection properly, then free up the slot
        delete conn;

        unique_lock<mutex> lock(mu_);
        ++available_;
        cv_.notify_one();
    }

    string connStr_;
    int available_;
    mutex mu_;
    condition_variable cv_;
};

#endif // CONNECTIONPOOL_H_INCLUDED
