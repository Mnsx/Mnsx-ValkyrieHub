/** 
 * @file MySQLUtils.cpp
 * @author Mnsx_x <xx1527030652@gmail.com>
 * @date 2026/4/20
 */
#include "MySQLUtils.h"

#include <iostream>

using namespace std;

MySQLUtil::MySQLUtil() {

    conn = mysql_init(nullptr);
    if (conn == nullptr) {

        throw runtime_error("mysql_init error");
    }

    if (mysql_real_connect(conn, DB_URL, DB_USER, DB_PASSWD, DB_NAME, 0, nullptr, 0) == nullptr) {

        string errorMsg = mysql_error(conn);
        mysql_close(conn);
        throw runtime_error("mysql_real_connect error" + errorMsg);
    }

    mysql_set_character_set(conn, DB_CHARSET);
    mysql_options(conn, MYSQL_OPT_RECONNECT, &DB_RECONNECT);
}

bool MySQLUtil::reconnect() {

    if (conn != nullptr) {

        mysql_close(conn);
    }

    conn = mysql_init(nullptr);
    if (conn == nullptr) {

        return false;
    }

    if (mysql_real_connect(conn, DB_URL, DB_USER, DB_PASSWD, DB_NAME, 0, nullptr, 0) == nullptr) {

        mysql_close(conn);
        return false;
    }

    mysql_set_character_set(conn, DB_CHARSET);
    mysql_options(conn, MYSQL_OPT_RECONNECT, &DB_RECONNECT);

    return true;
}

bool MySQLUtil::checkConnection() {

    if (conn != nullptr && mysql_ping(conn) == 0) {

        return true;
    }
    return reconnect();
}

MySQLUtil::~MySQLUtil() {

    if (conn != nullptr) {

        mysql_close(conn);
        conn = nullptr;
    }
}

MySQLUtil & MySQLUtil::getInstance() {

    thread_local MySQLUtil instance;
    return instance;
}

bool MySQLUtil::execute(const string & sql) {

    if (!checkConnection()) {

        return false;
    }

    if (mysql_query(conn, sql.c_str()) != 0) {

        return false;
    }

    return true;
}

MySQLResult MySQLUtil::query(const string & sql) {

    if (!checkConnection()) {

        return nullptr;
    }

    if (mysql_query(conn, sql.c_str()) != 0) {

        return nullptr;
    }

    return MySQLResult(mysql_store_result(conn));
}

bool MySQLUtil::startTransaction() {

    return checkConnection() && mysql_query(conn, "START TRANSACTION") == 0;
}

bool MySQLUtil::commit() {

    return checkConnection() && mysql_query(conn, "COMMIT") == 0;
}

bool MySQLUtil::rollback() {

    return checkConnection() && mysql_query(conn, "ROLLBACK") == 0;
}

string MySQLUtil::getError() const {

    return conn ? mysql_error(conn) : "not connected";
}

int MySQLUtil::getErrno() const {

    return conn ? mysql_errno(conn) : -1;
}

int MySQLUtil::getAffectedRows() const {

    return conn ? mysql_affected_rows(conn) : -1;
}

string MySQLUtil::escapeString(const string &str) {

    if (conn == nullptr) {

        return "";
    }

    char * buf = new char[str.size() * 2 + 1];
    mysql_real_escape_string(conn, buf, str.c_str(), str.size());
    string res(buf);
    delete [] buf;
    return res;
}

string MySQLUtil::getCurrentDateTime() {

    time_t now = time(nullptr);
    tm local = *localtime(&now);
    char buffer[20];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &local);
    return string(buffer);
}