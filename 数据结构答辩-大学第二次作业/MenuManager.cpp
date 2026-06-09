// 菜单管理实现

#include "MenuManager.h"
#include <fstream>   // file stream 文件流的输入输出 
#include <iostream>  // input/output stream
#include <iomanip>   // input/output manipulators 控制输出格式的 对齐、精度

// 构造函数：初始化菜单管理器：
// 参数：file - 数据存储文件名，默认为 "menu.txt"
// 功能：加载已有数据文件，若文件为空则初始化预置菜单并保存
MenuManager::MenuManager(const std::string& file) : filename(file)
{
	// 尝试从文件加载已保存的菜单数据
    loadFromFile();  
    
    // 若文件为空，则初始化默认的预置数据
    if (menuList.empty())
	{
        menuList =
		{
            // 4种饮料 Drink
            {1, "D01", "可口可乐", 6.0}, {1, "D02", "芬达", 6.0}, 
            {1, "D03", "橙汁", 8.0}, {1, "D04", "美年达", 6.0},
            // 10种汉堡 Burger
            {2, "B01", "巨无霸", 22.0}, {2, "B02", "鸡腿堡", 18.5}, 
            {2, "B03", "双鸡堡", 17.5}, {2, "B04", "牛肉堡", 26.0},
            {2, "B05", "板烧堡", 21.0}, {2, "B06", "吉士堡", 15.0}, 
            {2, "B07", "烤鸡堡", 20.0}, {2, "B08", "麦香鱼", 18.0},
            {2, "B09", "培根堡", 23.0}, {2, "B10", "蔬菜堡", 14.0},
            // 3种小食品 Snack
            {3, "S01", "冰激凌", 8.0}, {3, "S02", "薯条", 12.0}, 
            {3, "S03", "香草脆皮", 9.5}
        };
        
        // 保存 
        saveToFile();
    }
}

// 从文件加载菜单数据： 
// 功能：打开数据文件，逐行读取食品信息并存入 menuList 容器
// 说明：文件不存在时直接返回，不进行任何操作，这就是考虑到健壮性~ 
void MenuManager::loadFromFile()
{
	// 创建输入文件流对象，尝试打开数据文件
    std::ifstream file(filename);
    
    // 检查文件是否成功打开
    // 若文件不存在或无法访问，直接返回
    if (!file.is_open()) return;
    
    // 清空当前内存中的菜单列表，准备重新加载
    menuList.clear();  // 这个是在同名头文件里面定义好的类成员，可clear 
    
    // 创建临时 Food 对象，用于存储每次读取的数据
    Food item;
    
    // 使用流提取运算符逐行读取文件内容
    // 文件格式：分类编号 食品号 食品名称 单价
    // 注意：由于名称中不含空格，可以用 >> 直接读取哦 
    while (file >> item.categoryId >> item.id >> item.name >> item.price)
	{
		// 将读取到的有效数据添加到菜单列表末尾
        menuList.push_back(item);
    }
    // 关闭文件 
    file.close();
}

// 将菜单数据保存到文件： 
// 功能：遍历 menuList 容器，将所有食品信息写入数据文件
// 说明：const 成员函数，保证不会修改对象成员变量
void MenuManager::saveToFile() const
{
	// 创建输出文件流对象，以覆盖模式打开数据文件
    std::ofstream file(filename);
    
    // 检查文件是否成功打开
    if (!file.is_open())
	{
		// 使用 cerr 标准错误流输出错误信息（无缓冲，立即显示
        std::cerr << "【错误】无法保存数据至文件：" << filename << std::endl;
        return;
    }
    
    // 范围for + const auto& 表示只读引用，避免拷贝，提高效率
    for (const auto& item : menuList)
	{
		// 按固定格式写入：分类编号 食品号 食品名称 单价 换行
        // 各字段之间用空格分隔，便于后续读取
        file << item.categoryId << " " 
			 << item.id << " " 
			 << item.name << " " 
			 << item.price << "\n";
    }
    // 关闭文件，确保数据完整写入磁盘
    file.close();
}

// 获取菜单列表（只读访问）： 
// 返回值：常量引用，指向存储菜单数据的 vector 容器
// 说明：返回 const 引用既能避免拷贝开销，又能保护内部数据不被修改
const std::vector<Food>& MenuManager::getMenuList() const
{
    return menuList;
}

// 添加新食品： 
// 参数：categoryId - 分类编号，id - 食品编号，name - 食品名称，price - 单价
// 返回值：true 表示添加成功，false 表示食品号已存在导致添加失败
// 说明：添加前先检查食品号是否重复，保证数据的唯一性
bool MenuManager::addFood(int categoryId, const std::string& id, const std::string& name, double price)
{
	// 调用 findFood 查找是否已存在相同食品号的记录
    // 若返回非空指针，说明食品号重复，添加失败
    if (findFood(id) != nullptr) return false; // 食品号已存在
    
    // 初始化 
    menuList.push_back({categoryId, id, name, price});
    
    // 立刻保存 
    saveToFile();
    
    // 返回 true 表示添加操作成功
    return true;
}

// 修改已有食品信息： 
// 参数：id - 要修改的食品编号，其余参数为新的属性值
// 返回值：true 表示找到并成功修改，false 表示未找到对应食品
// 说明：使用引用遍历，直接修改原始数据，避免拷贝
bool MenuManager::modifyFood(const std::string& id, int categoryId, const std::string& name, double price)
{
	// & 可以修改内容 
    for (auto& item : menuList)
	{
		// 逐一更新该食品的所有属性
        if (item.id == id)
		{
            item.categoryId = categoryId;  // 更新分类编号
            item.name = name;              // 更新食品名称
            item.price = price;            // 更新单价
            
            // 修改后立即保存到文件
            saveToFile();
            
			// 修改成功            
            return true;
        }
    }
    // 循环结束未找到匹配的食品号，返回 false 表示修改失败
    return false;
}

// 根据食品号查找食品： 
// 参数：id - 要查找的食品编号
// 返回值：指向找到的 Food 对象的常量指针，未找到返回 nullptr
// 说明：还是返回 const 指针，防止外部通过指针修改内部数据
const Food* MenuManager::findFood(const std::string& id) const
{
    for (const auto& item : menuList)
	{
		 // 找到匹配项，返回其地址
   		 // &item 获取当前元素的地址
        if (item.id == id) return &item;
    }
    // 未找到匹配的食品号，返回空指针
    return nullptr;
}

// 格式化显示完整菜单
// 功能：按表格形式在控制台输出所有食品的分类、编号、名称和单价
// 说明：使用 iomanip 库函数实现对齐和格式化显示
void MenuManager::displayMenu() const
{
    std::cout << "\n====================== 肯德基菜单 ======================\n";
    
    // left：设置左对齐；setw(n)：设置字段宽度为 n 个字符
    std::cout << std::left << std::setw(12) << "分类编号" 
              << std::setw(12) << "食品号" 
              << std::setw(22) << "食品名称" 
              << std::setw(10) << "单价(元)" << "\n";
    std::cout << "--------------------------------------------------------\n";
        // 遍历菜单列表，逐行输出每个食品的详细信息
    for (const auto& item : menuList)
    {
        // 输出分类信息：格式为 "分类名(分类编号)"
        // getCategoryName 将数字编号转换为中文名称
        // to_string 将数字转换为字符串进行拼接
        std::cout << std::left 
                  << std::setw(12) << getCategoryName(item.categoryId) 
                                   + "(" + std::to_string(item.categoryId) + ")"
                  << std::setw(12) << item.id           // 食品号
                  << std::setw(22) << item.name         // 食品名称
                  // fixed：固定小数位数显示  setprecision(2)：保留两位小数
                  << std::fixed << std::setprecision(2) 
                  << std::setw(10) << item.price        // 单价
                  << "\n";
    }
    std::cout << "========================================================\n";
}
