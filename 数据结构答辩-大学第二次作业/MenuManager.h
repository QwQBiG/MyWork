//  菜单管理头文件

#ifndef MENU_MANAGER_H  // 如果 MENU_MANAGER_H 没有被定义过
#define MENU_MANAGER_H  // 就定义它 

#include "Food.h"
#include <vector>
#include <string>

// 一个 类 （菜单的） 
class MenuManager
{
// 私有接口： 
private:  
    // 私有成员： 
    
    // 菜单数据容器
    std::vector<Food> menuList;  // 内存中的菜单数据，程序运行时操作的都是它
    // 数据文件路径
    std::string filename;  // 构造函数指定文件名，读写操作都基于这个文件
    
    // 私有方法： 
    
    // 从文件读取
    void loadFromFile();  // 从 filename 读取数据，加载到 menuList
    //写入文件
    void saveToFile() const;  // 把 menuList 的数据写入 filename


// 公有接口：
public:
	// 构造函数： 
	// 这个就是默认的文件名（menu.txt） 
	// 内部应该调用 loadFromFile() 初始化 menuList
	// 所有的用 const & 传参，都是在避免拷贝字符串 
    MenuManager(const std::string& file = "menu.txt");  
	
    // 获取菜单
    const std::vector<Food>& getMenuList() const;
    // 添加食品： 
    // 参数对应 Food 结构体的四个字段
    // 返回 bool 表示成功或失败（如 ID 重复、参数不合法）
    // 成功后内部应调用 saveToFile() 持久化
    bool addFood(int categoryId, const std::string& id, const std::string& name, double price);
    // 修改食品：
	// 根据 id 找到对应食品，修改其内容
	// 返回 bool 表示是否找到并修改成功
	// 修改后也需要保存到文件 
	bool modifyFood(const std::string& id, int categoryId, const std::string& name, double price);
    // 查找食品：
	// 根据 id 查找，返回指针
	// 找到了返回指向该 Food 的指针，没找到返回 nullptr
	// 返回 const Food*，调用者不能通过指针修改数据，很好 
	const Food* findFood(const std::string& id) const;
    // 显示菜单
	void displayMenu() const;
};

#endif  // 结束条件判断、
// #ifndef / #define / #endif	防止头文件被重复包含，避免重复定义错误 
