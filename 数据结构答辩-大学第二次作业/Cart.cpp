// 购物车类实现

#include "Cart.h"
#include <iostream>
#include <iomanip>

// 添加产品的函数
void Cart::addItem(const Food& food, int quantity)
{
    for (auto& item : items)
    {
        if (item.first.id == food.id)
        {
            item.second += quantity;
            return;
        }
    }
    items.push_back({food, quantity});
}

// 计算总价的函数
double Cart::calculateTotal() const
{
    double total = 0.0;
    for (const auto& item : items)
    {
        total += item.first.price * item.second;
    }
    return total;
}

// 打印收银小票的函数
void Cart::printReceipt(double cashPaid) const
{
    double total = calculateTotal();
    double change = cashPaid - total;

    std::cout << "\n==================== 肯德基收银小票 ====================\n";
    std::cout << std::left << std::setw(12) << "分类"
              << std::setw(18) << "品名"
              << std::setw(10) << "单价"
              << std::setw(8) << "数量"
              << std::setw(10) << "小计" << "\n";
    std::cout << "------------------------------------------------------\n";
    for (const auto& item : items)
    {
        double itemTotal = item.first.price * item.second;
        std::cout << std::left << std::setw(12) << getCategoryName(item.first.categoryId)
                  << std::setw(18) << item.first.name
                  << std::fixed << std::setprecision(2)
                  << std::setw(10) << item.first.price
                  << std::setw(8) << item.second
                  << std::setw(10) << itemTotal << "\n";
    }
    std::cout << "------------------------------------------------------\n";
    std::cout << "应收金额: " << total << " 元\n";
    std::cout << "顾客付款: " << cashPaid << " 元\n";
    std::cout << "系统找零: " << (change >= 0 ? change : 0.0) << " 元\n";
    std::cout << "=================== 谢谢惠顾，欢迎下次光临 ===================\n";
}

void Cart::clear()
{
    items.clear();
}

bool Cart::isEmpty() const
{
    return items.empty();
}
