// 菜单管理类实现

#include "MenuManager.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#ifdef _WIN32
#include <windows.h>
#endif

// iqwqi: convert GBK string to UTF-8 for JSON output
static std::string gbkToUtf8(const std::string& gbk)
{
#ifdef _WIN32
    if (gbk.empty()) return gbk;
    int wlen = MultiByteToWideChar(936, 0, gbk.c_str(), -1, nullptr, 0);
    if (wlen <= 0) return gbk;
    std::wstring wstr(wlen, L'\0');
    MultiByteToWideChar(936, 0, gbk.c_str(), -1, &wstr[0], wlen);
    int ulen = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    if (ulen <= 0) return gbk;
    std::string utf8(ulen, '\0');
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &utf8[0], ulen, nullptr, nullptr);
    if (!utf8.empty() && utf8.back() == '\0') utf8.pop_back();
    return utf8;
#else
    return gbk;
#endif
}

// 构造函数：初始化菜单数据库
// 参数：file - 数据存储文件名，默认为 "menu.txt"
// 功能：尝试加载已有文件，若文件为空则初始化预置菜单数据
MenuManager::MenuManager(const std::string& file)
    : filename(file),
      mysql(nullptr),
      dbHost("localhost"),
      dbUser("root"),
      dbPassword("123456"),
      dbName("kfc_pos"),
      dbPort(3306)
{
    // 尝试从文件加载已保存的菜单数据
    loadFromFile();

    // 若文件为空，则初始化默认的预置数据
    if (menuList.empty())
    {
        menuList =
        {
            // 4种饮料 Drink
            {1, "D01", "可口可乐", 6.0}, {1, "D02", "雪碧", 6.0},
            {1, "D03", "果汁", 8.0}, {1, "D04", "美年达", 6.0},
            // 10种汉堡 Burger
            {2, "B01", "香辣堡", 22.0}, {2, "B02", "劲脆堡", 18.5},
            {2, "B03", "双层堡", 17.5}, {2, "B04", "牛扒堡", 26.0},
            {2, "B05", "烤汁堡", 21.0}, {2, "B06", "芝士堡", 15.0},
            {2, "B07", "鳕鱼堡", 20.0}, {2, "B08", "鸡肉卷", 18.0},
            {2, "B09", "豪华堡", 23.0}, {2, "B10", "蔬菜堡", 14.0},
            // 3种小食品 Snack
            {3, "S01", "鸡米花", 8.0}, {3, "S02", "薯条", 12.0},
            {3, "S03", "葡式蛋挞", 9.5}
        };

        // 保存
        saveToFile();
    }
}

// 从文件加载菜单数据
// 功能：打开持久化文件，读取食品信息并填充 menuList 容器
// 说明：文件不存在时直接返回，不做任何操作（考虑鲁棒性）
void MenuManager::loadFromFile()
{
    // 构造输入文件流对象，尝试打开文件
    std::ifstream file(filename);

    // 检查文件是否成功打开
    // 若文件不存在或无法访问，直接返回
    if (!file.is_open()) return;

    // 清空当前内存中的菜单列表，准备重新加载
    menuList.clear();

    // 声明临时 Food 变量，用于存储每次读取的数据
    Food item;

    // 使用流提取运算符逐行读取文件内容
    // 文件格式：分类号 食品编号 食品名称 单价
    // 注意：食品名称中不能有空格，可以用 >> 直接读取
    while (file >> item.categoryId >> item.id >> item.name >> item.price)
    {
        // 将读取到的有效记录添加到菜单列表末尾
        menuList.push_back(item);
    }
    // 关闭文件
    file.close();
}

// 将菜单数据保存到文件
// 功能：遍历 menuList 容器，将所有食品信息写入持久化文件
// 说明：const 成员函数，保证不修改对象成员变量
void MenuManager::saveToFile() const
{
    // 构造输出文件流，以覆写模式打开文件
    std::ofstream file(filename);

    // 检查文件是否成功打开
    if (!file.is_open())
    {
        // 使用 cerr 标准错误输出错误信息，无缓冲，即时显示
        std::cerr << "错误：无法创建输出文件！" << filename << std::endl;
        return;
    }

    // 范围for + const auto& 表示只读引用，避免拷贝提高效率
    for (const auto& item : menuList)
    {
        // 按固定格式写入：分类号 食品编号 食品名称 单价 换行
        // 各字段之间用空格分隔，便于后续读取
        file << item.categoryId << " "
             << item.id << " "
             << item.name << " "
             << item.price << "\n";
    }
    // 关闭文件，确保数据完整写入
    file.close();
}

// 获取菜单列表（只读访问）
// 返回值：常量引用，指向存储菜单数据的 vector 容器
// 说明：返回 const 引用既能避免拷贝开销，又能保证内部数据不被修改
const std::vector<Food>& MenuManager::getMenuList() const
{
    return menuList;
}

// 添加新食品
// 参数：categoryId - 分类编号；id - 食品编号；name - 食品名称；price - 单价
// 返回值：true 表示添加成功；false 表示食品编号已存在导致操作失败
// 说明：添加前先检查食品编号是否重复，保证数据的唯一性
bool MenuManager::addFood(int categoryId, const std::string& id, const std::string& name, double price)
{
    // 调用 findFood 检查是否已存在相同食品编号的记录
    // 若返回非空指针，说明食品编号重复，操作失败
    if (findFood(id) != nullptr) return false;

    // 初始化
    menuList.push_back({categoryId, id, name, price});

    // 即时保存
    saveToFile();
    // 同步 MySQL
    saveToMySQL();
    // iqwqi: 同步 menu_data.json
    saveToJson();

    // 返回 true 表示添加操作成功
    return true;
}

// 修改已有食品信息
// 参数：id - 要修改的食品编号；其余为新的属性值
// 返回值：true 表示找到并成功修改；false 表示未找到对应食品
// 说明：使用引用变量直接修改原始数据，避免拷贝
bool MenuManager::modifyFood(const std::string& id, int categoryId, const std::string& name, double price)
{
    // & 引用修改底层
    for (auto& item : menuList)
    {
        // 逐一匹配食品编号找到目标
        if (item.id == id)
        {
            item.categoryId = categoryId;  // 更新分类
            item.name = name;              // 更新名称
            item.price = price;            // 更新单价

            // 修改后立即保存到文件
            saveToFile();
            // 同步 MySQL
            saveToMySQL();
            // iqwqi: 同步 menu_data.json
            saveToJson();

            // 修改成功
            return true;
        }
    }
    // 循环结束仍未找到匹配的食品编号，返回 false 表示修改失败
    return false;
}

// 根据食品编号查找食品
// 参数：id - 要查找的食品编号
// 返回值：指向找到的 Food 对象的常量指针，未找到返回 nullptr
// 说明：返回的是 const 指针，防止外部通过指针修改内部数据
const Food* MenuManager::findFood(const std::string& id) const
{
    for (const auto& item : menuList)
    {
         // 找到匹配项，返回其地址
         // &item 获取当前元素的地址
        if (item.id == id) return &item;
    }
    // 未找到匹配的食品编号，返回空指针
    return nullptr;
}

// 析构函数：关闭 MySQL 连接，释放资源
MenuManager::~MenuManager()
{
    closeMySQL();
}

// 连接 MySQL 数据库
// 使用 MySQL C API 完成连接
bool MenuManager::connectMySQL(const std::string& host,
                               const std::string& user,
                               const std::string& password,
                               const std::string& database,
                               unsigned int port)
{
    // 先关闭之前的连接再重连
    closeMySQL();

    // 记录连接参数，方便后续使用
    dbHost = host;
    dbUser = user;
    dbPassword = password;
    dbName = database;
    dbPort = port;

    // 初始化 MySQL 连接
    mysql = mysql_init(nullptr);
    if (mysql == nullptr)
    {
        std::cerr << "MySQL 初始化失败" << std::endl;
        return false;
    }

    // iqwqi: try connecting WITH database first
    if (mysql_real_connect(mysql,
                           dbHost.c_str(),
                           dbUser.c_str(),
                           dbPassword.c_str(),
                           dbName.c_str(),
                           dbPort,
                           nullptr,
                           0) != nullptr)
    {
        mysql_set_character_set(mysql, "gbk");   // iqwqi: gbk to match source file encoding
        return true;
    }

    // iqwqi: database may not exist, connect WITHOUT database and create it
    std::cerr << "[iqwqi] DB '" << dbName << "' not found, creating ... ";
    mysql_close(mysql);
    mysql = mysql_init(nullptr);
    if (mysql_real_connect(mysql, dbHost.c_str(), dbUser.c_str(),
                           dbPassword.c_str(), nullptr, dbPort, nullptr, 0) == nullptr)
    {
        std::cerr << "MySQL 连接失败: " << mysql_error(mysql) << std::endl;
        mysql_close(mysql);
        mysql = nullptr;
        return false;
    }
    // iqwqi: drop old db and recreate with gbk charset
    std::string dropDB = "DROP DATABASE IF EXISTS `" + dbName + "`;";
    mysql_query(mysql, dropDB.c_str());
    std::string createDB = "CREATE DATABASE `" + dbName +
                           "` CHARACTER SET gbk COLLATE gbk_chinese_ci;";
    if (mysql_query(mysql, createDB.c_str()) != 0)
    {
        std::cerr << "FAIL: " << mysql_error(mysql) << std::endl;
        mysql_close(mysql);
        mysql = nullptr;
        return false;
    }
    mysql_close(mysql);

    // iqwqi: reconnect with the newly created database
    mysql = mysql_init(nullptr);
    if (mysql_real_connect(mysql, dbHost.c_str(), dbUser.c_str(),
                           dbPassword.c_str(), dbName.c_str(), dbPort, nullptr, 0) == nullptr)
    {
        std::cerr << "FAIL: " << mysql_error(mysql) << std::endl;
        mysql_close(mysql);
        mysql = nullptr;
        return false;
    }
    std::cerr << "OK" << std::endl;
    mysql_set_character_set(mysql, "gbk");   // iqwqi: gbk to match source file encoding
    return true;
}

// 关闭 MySQL 连接，释放资源
void MenuManager::closeMySQL()
{
    if (mysql != nullptr)
    {
        mysql_close(mysql);
        mysql = nullptr;
    }
}

// 创建 menu 表的简单表结构
bool MenuManager::initMySQLTable() const
{
    if (mysql == nullptr)
    {
        std::cerr << "未连接到 MySQL，无法建表" << std::endl;
        return false;
    }

    const char* createSQL =
        "CREATE TABLE IF NOT EXISTS menu ("
        "category_id INT NOT NULL,"
        "food_id VARCHAR(10) PRIMARY KEY,"
        "food_name VARCHAR(50) NOT NULL,"
        "price DECIMAL(8,2) NOT NULL"
        ") ENGINE=InnoDB DEFAULT CHARSET=gbk;";  // iqwqi: gbk to match source encoding

    if (mysql_query(mysql, createSQL) != 0)
    {
        std::cerr << "创建 menu 表失败: " << mysql_error(mysql) << std::endl;
        return false;
    }
    return true;
}

// 从 MySQL 的 menu 表中读取菜单数据
void MenuManager::loadFromMySQL()
{
    if (mysql == nullptr) return;

    const char* selectSQL = "SELECT category_id, food_id, food_name, price FROM menu;";
    if (mysql_query(mysql, selectSQL) != 0)
    {
        std::cerr << "查询 menu 表失败: " << mysql_error(mysql) << std::endl;
        return;
    }

    MYSQL_RES* result = mysql_store_result(mysql);
    if (result == nullptr) return;

    // iqwqi: only overwrite local data if MySQL has rows
    my_ulonglong rowCount = mysql_num_rows(result);
    if (rowCount == 0)
    {
        mysql_free_result(result);
        return;
    }

    menuList.clear();
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result)) != nullptr)
    {
        Food item;
        item.categoryId = std::stoi(row[0]);
        item.id = row[1];
        item.name = row[2];
        item.price = std::stod(row[3]);
        menuList.push_back(item);
    }

    mysql_free_result(result);
}

// 将当前菜单数据保存到 MySQL 的 menu 表
void MenuManager::saveToMySQL() const
{
    if (mysql == nullptr) return;

    // 先清空表再重新保存，然后插入当前数据
    const char* truncateSQL = "TRUNCATE TABLE menu;";
    if (mysql_query(mysql, truncateSQL) != 0)
    {
        std::cerr << "清空 menu 表失败: " << mysql_error(mysql) << std::endl;
        return;
    }

    // 逐条食品数据 INSERT 插入
    for (const auto& item : menuList)
    {
        std::string sql = "INSERT INTO menu (category_id, food_id, food_name, price) VALUES (" +
                          std::to_string(item.categoryId) + ", '" +
                          item.id + "', '" +
                          item.name + "', " +
                          std::to_string(item.price) + ");";
        if (mysql_query(mysql, sql.c_str()) != 0)
        {
            std::cerr << "插入食品" << item.id << "失败: " << mysql_error(mysql) << std::endl;
        }
    }
}

// iqwqi: 写入 menu_data.json 文件供前端 JS fetch
void MenuManager::saveToJson() const
{
    std::ofstream file("menu_data.json");
    if (!file.is_open())
    {
        std::cerr << "[iqwqi] Cannot write menu_data.json" << std::endl;
        return;
    }
    file << "[\n";
    for (size_t i = 0; i < menuList.size(); ++i)
    {
        const auto& f = menuList[i];
        file << "  {"
             << "\"categoryId\":" << f.categoryId << ", "
             << "\"id\":\"" << f.id << "\", "
             << "\"name\":\"" << gbkToUtf8(f.name) << "\", "
             << "\"price\":" << f.price;
        if (i + 1 < menuList.size()) file << "},"; else file << "}";
        file << "\n";
    }
    file << "]\n";
    file.close();
}

// 格式化显示全部菜单
// 功能：以表格形式在控制台列出所有食品的分类、编号、名称和单价
// 说明：使用 iomanip 库函数实现对齐和格式化显示
void MenuManager::displayMenu() const
{
    std::cout << "\n====================== 肯德基菜单 ======================\n";

    // left: 左对齐；setw(n): 设置字段宽度为 n 个字符
    std::cout << std::left << std::setw(12) << "分类"
              << std::setw(12) << "食品编号"
              << std::setw(22) << "食品名称"
              << std::setw(10) << "单价(元)" << "\n";
    std::cout << "--------------------------------------------------------\n";
    // 遍历菜单列表，逐行输出每个食品的详细信息
    for (const auto& item : menuList)
    {
        // 显示分类信息，格式为 "分类名(分类号)"
        // getCategoryName 将编号转为分类名称
        // to_string 将编号转为字符串以便拼接
        std::cout << std::left
                  << std::setw(12) << getCategoryName(item.categoryId)
                                   + "(" + std::to_string(item.categoryId) + ")"
                  << std::setw(12) << item.id           // 食品编号
                  << std::setw(22) << item.name         // 食品名称
                  // fixed: 固定小数位显示  setprecision(2): 保留两位小数
                  << std::fixed << std::setprecision(2)
                  << std::setw(10) << item.price        // 单价
                  << "\n";
    }
    std::cout << "========================================================\n";
}
