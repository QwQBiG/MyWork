#include "common.h"
#include "globals.h"
#include "utils.h"
#include "file_ops.h"
#include "logic.h"
#include "menus.h"

int main() {
    initGameRecords(); // 加载预设的纪录标准
    loadFromFile();    // 读取上次保存的数据
    
    int choice, subChoice;
    int currentPage = 1;
    
    printf("\n");
    printf(" ══════════════════════════════════════════════════════\n");
    printf("             运动会管理系统\n");
    printf(" ══════════════════════════════════════════════════════\n");
    
    while (1) {
        menu();
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n'); // 清除缓冲区
            continue;
        }
        
        switch (choice) {
            case 0: 
                saveToFile(); 
                printf("\n感谢使用运动会管理系统！\n");
                return 0;
            case 1: addAthlete(); break;
            case 2: 
                printf("\n当前第 %d 页", currentPage);
                displayAll(currentPage);
                printf("\n操作: N-下一页 P-上一页 R-返回: ");
                char nav; scanf(" %c", &nav); getchar();
                if (nav == 'N' || nav == 'n') {
                    if (currentPage * PAGE_SIZE < currentCount) currentPage++;
                } else if (nav == 'P' || nav == 'p') {
                    if (currentPage > 1) currentPage--;
                }
                break;
            case 3: 
                queryMenu(); scanf("%d", &subChoice);
                switch (subChoice) {
                    case 1: searchByIdOrName(); break;
                    case 2: searchByEvent(); break;
                    case 3: searchByScoreRange(); break;
                    case 4: searchByCollege(); break;
                    case 0: break;
                    default: printf(">>> 无效选择！\n");
                }
                break;
            case 4: 
                statMenu(); scanf("%d", &subChoice);
                switch (subChoice) {
                    case 1: showCollegeRanking(); break;
                    case 2: showRecordBreakers(); break;
                    case 3: showScoreDistribution(); break;
                    case 0: break;
                    default: printf(">>> 无效选择！\n");
                }
                break;
            case 5: 
                reportMenu(); scanf("%d", &subChoice);
                switch (subChoice) {
                    case 1: printScoreReport(); break;
                    case 2: exportToCSV(); break;
                    case 3: generateCertificate(); break;
                    case 0: break;
                    default: printf(">>> 无效选择！\n");
                }
                break;
            case 6: 
                manageMenu(); scanf("%d", &subChoice);
                switch (subChoice) {
                    case 1: groupCompetition(); break;
                    case 2: validateScores(); break;
                    case 0: break;
                    default: printf(">>> 无效选择！\n");
                }
                break;
            case 7: 
                maintainMenu(); scanf("%d", &subChoice);
                switch (subChoice) {
                    case 1: modifyRecord(); break;
                    case 2: deleteRecord(); break;
                    case 3: batchImport(); break;
                    case 4: backupData(); break;
                    case 5: initTestData(); break;
                    case 0: break;
                    default: printf(">>> 无效选择！\n");
                }
                break;
            case 8: 
                printf("\n--- 系统信息 ---\n");
                printf("当前数据量: %d 条记录\n", currentCount);
                printf("最大容量: %d 条记录\n", MAX_ATHLETES);
                printf("数据文件: %s\n", FILENAME);
                printf("每页显示: %d 条记录\n", PAGE_SIZE);
                printf("查看功能讲解请输入 1，退出请输入 0\n");
                scanf("%d", &subChoice);
                switch (subChoice) {
                    case 1: message(); break;
                    case 0: break;
                    default: printf(">>> 无效选择！\n");
                }
                break;
            default: printf(">>> 无效选择！\n");
        }
    }
    return 0;
}