// 购物车类头文件

#ifndef CART_H
#define CART_H

#include "Food.h"
#include <vector>
#include <utility>

// 定义类 Cart（购物车类）
// 功能：管理顾客选择的食品、数量、计算总价、打印小票
// 说明：使用 pair 将食品信息与购买数量绑定存储
class Cart
{
private:
    // 成员变量：items（购物清单）
    // 类型：vector<pair<Food, int>>
    // 说明：
    //   - pair 的第一个元素：Food 对象，存储食品的完整信息
    //   - pair 的第二个元素：int，存储该食品的购买数量
    // 示例：{ {汉堡, 2}, {饮料, 1} } 表示2个汉堡 + 1个饮料
    std::vector<std::pair<Food, int>> items;  // 存储已购食品及数量

public:
    // addItem：添加商品到购物车
    void addItem(const Food& food, int quantity);

    // calculateTotal：计算购物车总金额
    double calculateTotal() const;

    // printReceipt：打印收银小票
    void printReceipt(double cashPaid) const;

    // clear：清空购物车
    void clear();

    // isEmpty：判断购物车是否为空
    bool isEmpty() const;
};

#endif
