#include "common.h"
#include "globals.h"
#include "utils.h"
#include "file_ops.h"
#include "logic.h"
#include "menus.h"

int main() {
    zbh_initGameRecords(); // 加载预设的纪录标准
    zbh_loadFromFile();    // 读取上次保存的数据
    
    int zbh_choice, zbh_subChoice;
    int zbh_currentPage = 1;
    
    printf("\n");
    printf(" ══════════════════════════════════════════════════════\n");
    printf("             zbh运动会管理系统zbh\n");
    printf(" ══════════════════════════════════════════════════════\n");
    
    while (1) {
        zbh_menu();
        if (scanf("%d", &zbh_choice) != 1) {
            while (getchar() != '\n'); // 清除缓冲区
            continue;
        }
        
        switch (zbh_choice) {
            case 0: 
                zbh_saveToFile(); 
                printf("\n感谢使用zbh运动会管理系统zbh！\n");
                return 0;
            case 1: zbh_addAthlete(); break;
            case 2: 
                printf("\n当前第 %d 页", zbh_currentPage);
                zbh_displayAll(zbh_currentPage);
                printf("\n操作: N-下一页 P-上一页 R-返回: ");
                char zbh_nav; scanf(" %c", &zbh_nav); getchar();
                if (zbh_nav == 'N' || zbh_nav == 'n') {
                    if (zbh_currentPage * zbh_PAGE_SIZE < zbh_currentCount) zbh_currentPage++;
                } else if (zbh_nav == 'P' || zbh_nav == 'p') {
                    if (zbh_currentPage > 1) zbh_currentPage--;
                }
                break;
            case 3: 
                zbh_queryMenu(); scanf("%d", &zbh_subChoice);
                switch (zbh_subChoice) {
                    case 1: zbh_searchByIdOrName(); break;
                    case 2: zbh_searchByEvent(); break;
                    case 3: zbh_searchByScoreRange(); break;
                    case 4: zbh_searchByCollege(); break;
                    case 0: break;
                    default: printf(">>> 无效选择！\n");
                }
                break;
            case 4: 
                zbh_statMenu(); scanf("%d", &zbh_subChoice);
                switch (zbh_subChoice) {
                    case 1: zbh_showCollegeRanking(); break;
                    case 2: zbh_showRecordBreakers(); break;
                    case 3: zbh_showScoreDistribution(); break;
                    case 0: break;
                    default: printf(">>> 无效选择！\n");
                }
                break;
            case 5: 
                zbh_reportMenu(); scanf("%d", &zbh_subChoice);
                switch (zbh_subChoice) {
                    case 1: zbh_printScoreReport(); break;
                    case 2: zbh_exportToCSV(); break;
                    case 3: zbh_generateCertificate(); break;
                    case 0: break;
                    default: printf(">>> 无效选择！\n");
                }
                break;
            case 6: 
                zbh_manageMenu(); scanf("%d", &zbh_subChoice);
                switch (zbh_subChoice) {
                    case 1: zbh_groupCompetition(); break;
                    case 2: zbh_validateScores(); break;
                    case 0: break;
                    default: printf(">>> 无效选择！\n");
                }
                break;
            case 7: 
                zbh_maintainMenu(); scanf("%d", &zbh_subChoice);
                switch (zbh_subChoice) {
                    case 1: zbh_modifyRecord(); break;
                    case 2: zbh_deleteRecord(); break;
                    case 3: zbh_batchImport(); break;
                    case 4: zbh_backupData(); break;
                    case 5: zbh_initTestData(); break;
                    case 0: break;
                    default: printf(">>> 无效选择！\n");
                }
                break;
            case 8: 
                printf("\n--- 系统信息 ---\n");
                printf("当前数据量: %d 条记录\n", zbh_currentCount);
                printf("最大容量: %d 条记录\n", zbh_MAX_ATHLETES);
                printf("数据文件: %s\n", zbh_FILENAME);
                printf("每页显示: %d 条记录\n", zbh_PAGE_SIZE);
                printf("查看功能讲解请输入 1，退出请输入 0\n");
                scanf("%d", &zbh_subChoice);
                switch (zbh_subChoice) {
                    case 1: zbh_message(); break;
                    case 0: break;
                    default: printf(">>> 无效选择！\n");
                }
                break;
            default: printf(">>> 无效选择！\n");
        }
    }
    return 0;
}