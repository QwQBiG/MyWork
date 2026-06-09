// 购物车结算实现
//
// 注：我在同名头文件里面加注释了 ，可以参照 

#include "Cart.h"
#include <iostream>
#include <iomanip>

// 添加菜品的函数 
void Cart::addItem(const Food& food, int quantity)  
{
    for (auto& item : items)
	{
        if (item.first.id == food.id)  // 因为用的是pair 
		{
            item.second += quantity;  // 看看 Cart.h 的注释叭，写的很明确了 
            return;
        }
    }
    items.push_back({food, quantity});  // 因为 vector<pair<int, int>>  
}

// 一个计算总价的函数
double Cart::calculateTotal() const   
{
    double total = 0.0;
    for (const auto& item : items)
	{
        total += item.first.price * item.second;  // 价格 * 数量 
    }
    return total;
}

// 打印水单的函数 
void Cart::printReceipt(double cashPaid) const
{
    double total = calculateTotal();
    double change = cashPaid - total;  // 找零 

    std::cout << "\n==================== 肯德基结账单 ====================\n";
    std::cout << std::left << std::setw(12) << "分类" 
              << std::setw(18) << "名称" 
              << std::setw(10) << "单价" 
              << std::setw(8) << "数量" 
              << std::setw(10) << "总价" << "\n";
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
    std::cout << "应付金额: " << total << " 元\n";
    std::cout << "顾客交款: " << cashPaid << " 元\n";
    std::cout << "系统找零: " << (change >= 0 ? change : 0.0) << " 元\n";
    std::cout << "=================== 谢谢光临，欢迎下次光临 ===================\n";
}

void Cart::clear()
{
    items.clear();
}

bool Cart::isEmpty() const
{
    return items.empty();
}
