// 食品实体定义

#ifndef FOOD_H  // 如果 FOOD_H 没有被定义过
#define FOOD_H  // 就定义它

#include <string>

// 食品结构体
struct Food
{
    int categoryId;    // 分类编号 (1: 饮料, 2: 汉堡, 3: 小食品)
    std::string id;    // 食品号 (如 D01, B01)
    std::string name;  // 食品名 (名称中不要加空格) 
    double price;      // 单价
};

// 获取分类名称辅助函数
inline std::string getCategoryName(int catId)  // inline：允许函数在头文件中定义而不引发链接冲突 
{
    switch (catId)
	{
        case 1: return "饮料";
        case 2: return "汉堡";
        case 3: return "小食品";
        default: return "未知分类";  // 健壮性啦 
    }
}

#endif  // 结束条件判断
// #ifndef / #define / #endif	防止头文件被重复包含，避免重复定义错误
