/** 
 * @file MySQLUtils.h
 * @author Mnsx_x <xx1527030652@gmail.com>
 * @date 2026/4/20
 * @description 
 */
#ifndef MNSX_UTILS_MYSQLUTIL_H
#define MNSX_UTILS_MYSQLUTIL_H

#include <memory>
#include <mysql/mysql.h>
#include <string>

const static char * DB_URL = "127.0.0.1";       // 数据库地址
const static char * DB_USER = "root";           // 数据库用户名称
const static char * DB_PASSWD = "123123";       // 数据库密码
const static char * DB_NAME = "mnsx_valkyrie";        // 数据库名称
const static char * DB_CHARSET = "utf8mb4";     // 数据库默认字符集
const static int DB_RECONNECT = 1;                  // 数据库是否自动重连 1-是 0-否

/**
 * 自定义智能指针删除器封装MYSQL_res *
 */
struct MySQLResDeleter {
    void operator()(MYSQL_RES * res) const {

        if (res != nullptr) {

            mysql_free_result(res);
        }
    }
};

/**
 * 处理MySQL数据集的智能指针别名
 */
using MySQLResult = std::unique_ptr<MYSQL_RES, MySQLResDeleter>;

class MySQLUtil {

private:
    /**
     * MySQL文件操作符指针
     */
    MYSQL * conn;

    /**
     * 私有构造函数，创建数据库句柄，链接数据库，设置字符集，设置数据库自动重连
     */
    MySQLUtil();

    /**
     * 禁止复制构造函数
     */
    MySQLUtil(const MySQLUtil &) = delete;

    /**
     * 禁止赋值构造函数
     */
    MySQLUtil &operator=(const MySQLUtil &) = delete;

    /**
     * 重新连接数据库
     * @return 重连是否成功
     */
    bool reconnect();

    /**
     * 检查数据库链接情况
     * @return 0-链接 1-断开连接
     */
    bool checkConnection();

public:
    /**
     * 析构函数
     */
    ~MySQLUtil();

    /**
     * 获取MySQLUtil工具类的单例对象
     * @return MySQLUtil类对象
     */
    static MySQLUtil & getInstance();

    /**
     * 执行非查询语句
     * @param sql SQL语句
     * @return 受影响的行数，-1是出现错误
     */
    bool execute(const std::string & sql);

    /**
     * 执行查询语句
     * @param sql SQL语句
     * @return 返回MYSQL数据集，使用智能指针封装，无需释放
     */
     MySQLResult query(const std::string & sql);

    /**
     * 开启事务
     * @return 开启事务是否成功
     */
    bool startTransaction();

    /**
     * 提交事务
     * @return 提交事务是否成功
     */
    bool commit();

    /**
     * 回滚事务
     * @return 回滚事务是否成功
     */
    bool rollback();

    /**
     * 获取错误信息
     * @return 错误信息
     */
    std::string getError() const;

    /**
     * 获取错误码
     * @return 错误码
     */
    int getErrno() const;

    /**
     * 返回受影响的函数
     * @return 受影响的行数
     */
    int getAffectedRows() const;

    /**
     * 防止SQL注入
     * @param str 未处理SQL注入的SQL语句
     * @return 安全的SQL语句
     */
    std::string escapeString(const std::string & str);

    /**
     * 返回当前时间字符串，可以直接插入到数据库中的DateTime字段
     * @return DateTime字符串
     */
    static std::string getCurrentDateTime();
};


#endif //MNSX_UTILS_MYSQLUTIL_H