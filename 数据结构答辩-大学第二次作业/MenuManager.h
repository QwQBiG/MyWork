// 菜单管理类头文件

#ifndef MENU_MANAGER_H
#define MENU_MANAGER_H

#include "Food.h"
#include <vector>
#include <string>

// iqwqi: 引入 MySQL C API 头文件
#include <mysql.h>

// 一个类：管理菜单模块
class MenuManager
{
private:
    // 私有成员变量

    // 菜单数据库
    std::vector<Food> menuList;  // 内存中的菜单数据，程序运行时操作的副本
    // 持久化文件路径
    std::string filename;        // 构造函数指定文件名，用于读写文本持久化文件

    // MySQL 连接成员
    MYSQL* mysql;              // MySQL 连接指针
    std::string dbHost;        // 数据库服务器地址
    std::string dbUser;        // 数据库用户名
    std::string dbPassword;    // 数据库密码
    std::string dbName;        // 数据库名
    unsigned int dbPort;       // 数据库端口

    // 私有方法

    // 从文件读取
    void loadFromFile();        // 从 filename 读取数据，恢复到 menuList
    // 写入文件
    void saveToFile() const;    // 将 menuList 数据写入 filename

public:
    // 构造函数
    MenuManager(const std::string& file = "menu.txt");

    // 析构函数：关闭 MySQL 连接等
    ~MenuManager();

    // 连接 MySQL 数据库
    bool connectMySQL(const std::string& host = "localhost",
                      const std::string& user = "root",
                      const std::string& password = "123456",
                      const std::string& database = "kfc_pos",
                      unsigned int port = 3306);

    // 关闭 MySQL 连接，释放资源
    void closeMySQL();

    // 检查 MySQL 中 menu 表是否存在，不存在则创建简单表结构
    bool initMySQLTable() const;

    // iqwqi: 从 MySQL 数据库中读取菜单
    void loadFromMySQL();

    // iqwqi: 推送本地菜单到 MySQL
    void saveToMySQL() const;

    // 获取菜单
    const std::vector<Food>& getMenuList() const;
    // 添加食品
    // 参数对应 Food 结构体的各字段
    // 返回 bool 表示成功或失败（如 ID 重复或参数不合法）
    // 成功后内部调用 saveToFile() 持久化
    bool addFood(int categoryId, const std::string& id, const std::string& name, double price);
    // 修改食品
    // 根据 id 找到对应食品，修改其属性
    // 返回 bool 表示是否找到并修改成功
    // 修改后也需要保存到文件
    bool modifyFood(const std::string& id, int categoryId, const std::string& name, double price);
    // 查找食品
    // 根据 id 查找，返回指针
    // 找到了返回指向 Food 的常指针，没找到返回 nullptr
    // 返回 const Food*，调用者不能通过指针修改数据，很好
    const Food* findFood(const std::string& id) const;
    // 显示菜单
    void displayMenu() const;

    // iqwqi: 写入 menu_data.json 供前端展示
    void saveToJson() const;
};

#endif
