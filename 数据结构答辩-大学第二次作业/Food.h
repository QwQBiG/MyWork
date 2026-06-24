// 食品实体定义

#ifndef FOOD_H
#define FOOD_H

#include <string>

// 食品结构体
struct Food
{
    int categoryId;    // 分类编号 (1: 饮料, 2: 汉堡, 3: 小食品)
    std::string id;    // 食品编号 (如 D01, B01)
    std::string name;  // 食品名 (名称中不要加空格)
    double price;      // 单价
};

// 获取分类名称辅助函数
inline std::string getCategoryName(int catId)
{
    switch (catId)
    {
        case 1: return "饮料";
        case 2: return "汉堡";
        case 3: return "小食品";
        default: return "未知分类";
    }
}

#endif
