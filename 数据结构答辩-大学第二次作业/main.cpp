// POS收银系统主控模块

#include <iostream>      // 标准输入输出：cin, cout
#include <limits>        // numeric_limits：配合 ignore 清空输入缓冲区
#include "MenuManager.h" // 菜单管理类，负责菜单数据的增删改查和持久化
#include "Cart.h"        // 购物车类，管理选购商品、计算总价、打印小票

// 安全整数输入：循环提示直到用户输入合法整数
int getSafeInt(const std::string& prompt)
{
    int val;  // 存储用户输入的整数
    while (true)  // 无限循环，只有合法输入才 return 跳出
    {
        std::cout << prompt;  // 显示提示语
        if (std::cin >> val)  // 尝试读取整数，成功则进入分支
        {
            return val;  // 读取成功，返回该值
        }
        std::cout << "【警告】输入无效，请输入一个整数。\n";  // 输入非整数的提示
        std::cin.clear();  // 清除 cin 的错误标志位（如 failbit）
        std::cin.ignore(  // 丢弃输入缓冲区中的无效数据
            std::numeric_limits<std::streamsize>::max(),  // 最多忽略缓冲区最大长度的字符
            '\n'  // 直到遇到换行符为止
        );
    }
}

// 安全浮点数输入：循环提示直到输入非负浮点数
double getSafeDouble(const std::string& prompt)
{
    double val;  // 存储用户输入的浮点数
    while (true)  // 无限循环，直到合法输入才 return
    {
        std::cout << prompt;  // 显示提示语
        if (std::cin >> val && val >= 0)  // 读取成功 且 值非负
        {
            return val;  // 合法输入，返回该值
        }
        std::cout << "【警告】输入无效，请输入一个有效的非负数值。\n";  // 输入不合法提示
        std::cin.clear();  // 清除错误标志位
        std::cin.ignore(  // 清空输入缓冲区
            std::numeric_limits<std::streamsize>::max(),  // 忽略缓冲区所有字符
            '\n'  // 直到换行符
        );
    }
}

// 菜单管理界面：提供食品添加、修改的交互操作
void handleManageMenu(MenuManager& manager)
{
    while (true)  // 循环显示管理子菜单
    {
        std::cout << "\n--- 食品信息管理 ---\n";
        std::cout << "1. 录入/新增食品\n";
        std::cout << "2. 修改食品信息\n";
        std::cout << "3. 返回上一级菜单\n";
        int choice = getSafeInt("请选择操作 (1-3): ");  // 安全获取用户选项

        if (choice == 3) break;  // 选项3：退出管理菜单，返回主菜单

        if (choice == 1)  // 选项1：新增食品
        {
            int cat = getSafeInt("请输入分类编号 (1: 饮料, 2: 汉堡, 3: 小食品): ");
            if (cat < 1 || cat > 3)  // 校验分类编号是否合法
            {
                std::cout << "【警告】无效的分类编号，必须是 1、2 或 3。\n";
                continue;  // 跳过本次循环，重新显示管理菜单
            }
            std::string id, name;
            std::cout << "请输入新食品号 (如 D05, B11): ";
            std::cin >> id;  // 读取食品号
            std::cout << "请输入新食品名 (不允许含空格): ";
            std::cin >> name;  // 用 >> 读取，以空格分隔，故名称不可含空格
            double price = getSafeDouble("请输入单价: ");  // 安全读取单价

            if (manager.addFood(cat, id, name, price))  // 调用 MenuManager 的添加方法
                std::cout << "【成功】食品信息添加完成。\n";
            else
                std::cout << "【错误】该食品号已存在，添加失败。\n";  // 食品号重复
        }
        else if (choice == 2)  // 选项2：修改食品
        {
            std::string id;
            std::cout << "请输入需要修改的食品号: ";
            std::cin >> id;

            const Food* f = manager.findFood(id);  // 查找对应食品号的记录
            if (!f)  // 指针为空，说明未找到
            {
                std::cout << "【错误】未找到此食品号对应的商品。\n";
                continue;  // 回到管理菜单
            }
            // 显示当前信息，方便用户对照修改
            std::cout << "当前信息: [" << getCategoryName(f->categoryId) 
                      << "] 名称: " << f->name << "，单价: " << f->price << "\n";

            int cat = getSafeInt("请输入新分类 (1: 饮料, 2: 汉堡, 3: 小食品): ");
            if (cat < 1 || cat > 3)  // 校验新分类编号
            {
                std::cout << "【警告】分类编号输入错误，修改放弃。\n";
                continue;
            }
            std::string name;
            std::cout << "请输入新商品名称: ";
            std::cin >> name;
            double price = getSafeDouble("请输入新单价: ");

            if (manager.modifyFood(id, cat, name, price))  // 调用修改方法
                std::cout << "【成功】修改已应用。\n";
            else
                std::cout << "【失败】应用更改时发生异常。\n";
        }
        else  // 输入非1/2/3的情况
        {
            std::cout << "【警告】无此选项，请重新选择。\n";
        }
    }
}

// 点餐结算界面：选购商品、收款找零、打印小票
void handleOrder(const MenuManager& manager, Cart& cart)
{
    cart.clear();  // 每次进入点餐流程前清空购物车，避免上次残留
    manager.displayMenu();  // 展示当前完整菜单供顾客选择

    // 选购阶段： 
    while (true)
    {
        std::string id;
        std::cout << "\n请输入要购买的食品号 (输入 Q 或 q 结束并开始结算): ";
        std::cin >> id;
        if (id == "Q" || id == "q") break;  // 大小写Q均可退出选购阶段

        const Food* f = manager.findFood(id);  // 根据食品号查找商品
        if (!f)  // 未找到该食品号
        {
            std::cout << "【警告】输入的食品号不正确，请重新输入！\n";
            continue;  // 提示重新输入
        }

        int qty = getSafeInt("请输入购买数量: ");  // 安全获取购买数量
        if (qty <= 0)  // 数量必须为正整数
        {
            std::cout << "【警告】数量必须大于 0。\n";
            continue;
        }

        cart.addItem(*f, qty);  // 解引用指针，传入 Food 对象和数量到购物车
        std::cout << "【已添加】" << f->name << " x " << qty << " 件。\n";
    }

    // 未选购任何商品则取消本次点餐
    if (cart.isEmpty())
    {
        std::cout << "【提示】未选购任何商品，本次点餐取消。\n";
        return;  // 直接返回主菜单
    }

    // 结算阶段： 
    double total = cart.calculateTotal();  // 计算购物车内所有商品的总金额
    std::cout << "\n应收总金额：" << total << " 元。\n";

    double pay = 0.0;
    while (true)  // 循环收款直到金额足够
    {
        pay = getSafeDouble("请输入顾客付账金额 (元): ");
        if (pay >= total) break;  // 付款金额足够，跳出循环
        std::cout << "【警告】金额不足！还差 " << (total - pay) << " 元，请重新交款。\n";
    }

    cart.printReceipt(pay);  // 打印购物小票并显示找零金额
}

// 程序入口：初始化系统，主菜单循环调度
int main()
{
    MenuManager manager;  // 创建菜单管理器（构造函数自动加载文件数据）
    Cart cart;  // 创建购物车对象

    while (true)  // 主程序主循环
    {
        // 打印系统主界面
        std::cout << "\n=========================================\n";
        std::cout << "         肯德基 POS 终端收银系统         \n";
        std::cout << "=========================================\n";
        std::cout << "  1. 浏览当前餐单\n";
        std::cout << "  2. 点餐与结账结算\n";
        std::cout << "  3. 管理菜单数据 (添加/修改)\n";
        std::cout << "  4. 退出系统\n";
        std::cout << "=========================================\n";
        int choice = getSafeInt("请输入功能编号 (1-4): ");  // 安全获取菜单选择

        switch (choice)  // 根据用户选择分发到不同功能模块
        {
            case 1:
                manager.displayMenu();  // 显示当前菜单
                break;
            case 2:
                handleOrder(manager, cart);  // 进入点餐与结算流程
                break;
            case 3:
                handleManageMenu(manager);  // 进入菜单数据管理
                break;
            case 4:
                std::cout << "【提示】系统已安全退出，谢谢使用。\n";
                return 0;  // 程序正常结束
            default:  // 非1~4的输入
                std::cout << "【警告】无对应指令，请输入 1 ~ 4 之间的整数。\n";
                break;
        }
    }
    return 0;
}
