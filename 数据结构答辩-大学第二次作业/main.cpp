// POS终端系统主模块

#include <iostream>
#include <limits>
#include "MenuManager.h"
#include "Cart.h"

#ifdef _WIN32
#include <windows.h>
#endif

// 安全整数输入：循环提示直到用户输入合法值
int getSafeInt(const std::string& prompt)
{
    int val;
    while (true)
    {
        std::cout << prompt;
        if (std::cin >> val)
        {
            return val;
        }
        std::cout << "【警告】输入无效，请输入一个整数！\n";
        std::cin.clear();
        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(),
            '\n'
        );
    }
}

// 安全浮点数输入：循环提示直到输入非负数
double getSafeDouble(const std::string& prompt)
{
    double val;
    while (true)
    {
        std::cout << prompt;
        if (std::cin >> val && val >= 0)
        {
            return val;
        }
        std::cout << "【警告】输入无效，请输入一个有效的非负数值！\n";
        std::cin.clear();
        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(),
            '\n'
        );
    }
}

// 菜单管理界面：提供食品添加、修改的交互功能
void handleManageMenu(MenuManager& manager)
{
    while (true)
    {
        std::cout << "\n--- 食品信息管理 ---\n";
        std::cout << "1. 录入/添加食品\n";
        std::cout << "2. 修改食品信息\n";
        std::cout << "3. 返回上一级菜单\n";
        int choice = getSafeInt("请选择操作 (1-3): ");

        if (choice == 3) break;

        if (choice == 1)
        {
            int cat = getSafeInt("请输入分类号 (1: 饮料, 2: 汉堡, 3: 小食品): ");
            if (cat < 1 || cat > 3)
            {
                std::cout << "【警告】无效的分类号，请输入 1、2 或 3！\n";
                continue;
            }
            std::string id, name;
            std::cout << "请输入食品编号 (如 D05, B11): ";
            std::cin >> id;
            std::cout << "请输入食品名 (名称中不要加空格): ";
            std::cin >> name;
            double price = getSafeDouble("请输入单价: ");

            if (manager.addFood(cat, id, name, price))
                std::cout << "【成功】食品信息添加完成。\n";
            else
                std::cout << "【错误】该食品编号已存在，操作失败。\n";
        }
        else if (choice == 2)
        {
            std::string id;
            std::cout << "请输入需要修改的食品编号: ";
            std::cin >> id;

            const Food* f = manager.findFood(id);
            if (!f)
            {
                std::cout << "【错误】未找到该食品编号对应的商品！\n";
                continue;
            }
            std::cout << "当前信息: [" << getCategoryName(f->categoryId)
                      << "] 名称: " << f->name << "，单价: " << f->price << "\n";

            int cat = getSafeInt("请输入新分类 (1: 饮料, 2: 汉堡, 3: 小食品): ");
            if (cat < 1 || cat > 3)
            {
                std::cout << "【警告】将保留原分类不做修改。\n";
                continue;
            }
            std::string name;
            std::cout << "请输入新商品名称: ";
            std::cin >> name;
            double price = getSafeDouble("请输入新单价: ");

            if (manager.modifyFood(id, cat, name, price))
                std::cout << "【成功】修改已应用。\n";
            else
                std::cout << "【失败】应用修改时出现异常。\n";
        }
        else
        {
            std::cout << "【警告】无此选项，请重新选择。\n";
        }
    }
}

// 点餐交互界面：选择商品、收银结算、打印小票
void handleOrder(const MenuManager& manager, Cart& cart)
{
    cart.clear();
    manager.displayMenu();

    // 选菜阶段
    while (true)
    {
        std::string id;
        std::cout << "\n请输入要购买的食品编号 (输入 Q 或 q 开始结算): ";
        std::cin >> id;
        if (id == "Q" || id == "q") break;

        const Food* f = manager.findFood(id);
        if (!f)
        {
            std::cout << "【警告】该食品编号不正确，请重新输入！\n";
            continue;
        }

        int qty = getSafeInt("请输入购买数量: ");
        if (qty <= 0)
        {
            std::cout << "【警告】数量不能为 0！\n";
            continue;
        }

        cart.addItem(*f, qty);
        std::cout << "【添加】" << f->name << " x " << qty << " 份\n";
    }

    // 未选任何商品，取消本次点单
    if (cart.isEmpty())
    {
        std::cout << "【提示】未选择任何商品，本次点单取消。\n";
        return;
    }

    // 结算阶段
    double total = cart.calculateTotal();
    std::cout << "\n应收总金额：" << total << " 元。\n";

    double pay = 0.0;
    while (true)
    {
        pay = getSafeDouble("请输入顾客付款金额 (元): ");
        if (pay >= total) break;
        std::cout << "【警告】金额不足！还需 " << (total - pay) << " 元，请重新输入。\n";
    }

    cart.printReceipt(pay);
}

// 主入口：初始化系统并进入菜单循环
int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(936);   // iqwqi: set console to GBK (source files are GBK)
#endif

    MenuManager manager;
    Cart cart;

    // iqwqi: auto connect MySQL and load menu data at startup
    std::cout << "[iqwqi] Connecting MySQL ... ";
    if (manager.connectMySQL("localhost", "root", "zbh123456", "kfc_pos", 3306))
    {
        std::cout << "OK" << std::endl;
        std::cout << "[iqwqi] Init menu table ... ";
        if (manager.initMySQLTable())
            std::cout << "OK" << std::endl;
        else
            std::cout << "FAIL" << std::endl;
        std::cout << "[iqwqi] Load from MySQL ... ";
        manager.loadFromMySQL();
        // iqwqi: push default data to MySQL if table was empty
        manager.saveToMySQL();
        std::cout << "OK (loaded " << manager.getMenuList().size() << " items)" << std::endl;
        // iqwqi: generate JSON so frontend can fetch it
        manager.saveToJson();
    }
    else
    {
        std::cout << "FAIL (fallback to local file)" << std::endl;
    }

    while (true)
    {
        std::cout << "\n=========================================\n";
        std::cout << "         肯德基 POS 终端收银系统         \n";
        std::cout << "=========================================\n";
        std::cout << "  1. 浏览当前菜单\n";
        std::cout << "  2. 开启收银结算\n";
        std::cout << "  3. 进入菜单管理 (添加/修改)\n";
        std::cout << "  4. 退出系统\n";
        std::cout << "=========================================\n";
        int choice = getSafeInt("请输入功能编号 (1-4): ");

        switch (choice)
        {
            case 1:
                manager.displayMenu();
                break;
            case 2:
                handleOrder(manager, cart);
                break;
            case 3:
                handleManageMenu(manager);
                break;
            case 4:
                std::cout << "【提示】系统已安全退出，谢谢使用。\n";
                return 0;
            default:
                std::cout << "【警告】无对应指令，请输入 1 ~ 4 之间的数字。\n";
                break;
        }
    }
    return 0;
}
