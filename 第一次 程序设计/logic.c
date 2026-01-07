// 业务逻辑实现
// 包含了所有的计算、排序、查询和交互逻辑。

#include "logic.h"
#include "globals.h"
#include "utils.h"

// 排序与计算核心算法

/*
就是个小冒泡，O(n^2) ,凑活用。 
*/
void zbh_sortAndCalculate() {
    if (zbh_currentCount == 0) return;

    for (int zbh_i = 0; zbh_i < zbh_currentCount - 1; zbh_i++) {
        for (int zbh_j = 0; zbh_j < zbh_currentCount - 1 - zbh_i; zbh_j++) {
            int zbh_needSwap = 0;
            int zbh_eventCompare = strcmp(zbh_athletes[zbh_j].zbh_event, zbh_athletes[zbh_j+1].zbh_event);
            
            if (zbh_eventCompare > 0) {
                zbh_needSwap = 1; 
            } else if (zbh_eventCompare == 0) {
                int zbh_isRun = zbh_isRunningEvent(zbh_athletes[zbh_j].zbh_event);
                if (zbh_isRun && zbh_athletes[zbh_j].zbh_finalScore > zbh_athletes[zbh_j+1].zbh_finalScore) zbh_needSwap = 1; 
                if (!zbh_isRun && zbh_athletes[zbh_j].zbh_finalScore < zbh_athletes[zbh_j+1].zbh_finalScore) zbh_needSwap = 1; 
            }
            if (zbh_needSwap) zbh_swapAthlete(&zbh_athletes[zbh_j], &zbh_athletes[zbh_j+1]);
        }
    }

    int zbh_eventStartIndex = 0;
    for (int zbh_i = 0; zbh_i < zbh_currentCount; zbh_i++) {
        if (zbh_i == 0 || strcmp(zbh_athletes[zbh_i].zbh_event, zbh_athletes[zbh_i-1].zbh_event) != 0) {
            zbh_athletes[zbh_i].zbh_rank = 1;
            zbh_eventStartIndex = zbh_i; 
        } else {
            int zbh_isRun = zbh_isRunningEvent(zbh_athletes[zbh_i].zbh_event);
            int zbh_isTie = 0;
            
            if (zbh_isRun && zbh_athletes[zbh_i].zbh_finalScore == zbh_athletes[zbh_i-1].zbh_finalScore) zbh_isTie = 1;
            else if (!zbh_isRun && zbh_athletes[zbh_i].zbh_finalScore == zbh_athletes[zbh_i-1].zbh_finalScore) zbh_isTie = 1;

            if (zbh_isTie) zbh_athletes[zbh_i].zbh_rank = zbh_athletes[zbh_i-1].zbh_rank;
            else zbh_athletes[zbh_i].zbh_rank = (zbh_i - zbh_eventStartIndex) + 1;
        }
        
        zbh_athletes[zbh_i].zbh_score = zbh_getScoreByRank(zbh_athletes[zbh_i].zbh_rank);
        zbh_athletes[zbh_i].zbh_isBrokenRecord = 0;
        for (int zbh_j = 0; zbh_j < zbh_recordCount; zbh_j++) {
            if (strcmp(zbh_athletes[zbh_i].zbh_event, zbh_gameRecords[zbh_j].zbh_event) == 0) {
                int zbh_isRun = zbh_isRunningEvent(zbh_athletes[zbh_i].zbh_event);
                if ((zbh_isRun && zbh_athletes[zbh_i].zbh_finalScore < zbh_gameRecords[zbh_j].zbh_record) ||
                    (!zbh_isRun && zbh_athletes[zbh_i].zbh_finalScore > zbh_gameRecords[zbh_j].zbh_record)) {
                    zbh_athletes[zbh_i].zbh_isBrokenRecord = 1;
                    break;
                }
            }
        }
    }
}

void zbh_calculateCollegeStats() {
    zbh_collegeCount = 0;
    for (int zbh_i = 0; zbh_i < zbh_currentCount; zbh_i++) {
        int zbh_found = 0;
        for (int zbh_j = 0; zbh_j < zbh_collegeCount; zbh_j++) {
            if (strcmp(zbh_athletes[zbh_i].zbh_college, zbh_collegeStats[zbh_j].zbh_name) == 0) {
                zbh_collegeStats[zbh_j].zbh_totalScore += zbh_athletes[zbh_i].zbh_score;
                if (zbh_athletes[zbh_i].zbh_rank == 1) zbh_collegeStats[zbh_j].zbh_goldCount++;
                if (zbh_athletes[zbh_i].zbh_rank == 2) zbh_collegeStats[zbh_j].zbh_silverCount++;
                if (zbh_athletes[zbh_i].zbh_rank == 3) zbh_collegeStats[zbh_j].zbh_bronzeCount++;
                zbh_found = 1;
                break;
            }
        }
        if (!zbh_found) {
            strcpy(zbh_collegeStats[zbh_collegeCount].zbh_name, zbh_athletes[zbh_i].zbh_college);
            zbh_collegeStats[zbh_collegeCount].zbh_totalScore = zbh_athletes[zbh_i].zbh_score;
            zbh_collegeStats[zbh_collegeCount].zbh_goldCount = (zbh_athletes[zbh_i].zbh_rank == 1) ? 1 : 0;
            zbh_collegeStats[zbh_collegeCount].zbh_silverCount = (zbh_athletes[zbh_i].zbh_rank == 2) ? 1 : 0;
            zbh_collegeStats[zbh_collegeCount].zbh_bronzeCount = (zbh_athletes[zbh_i].zbh_rank == 3) ? 1 : 0;
            zbh_collegeCount++;
        }
    }
    
    for (int zbh_i = 0; zbh_i < zbh_collegeCount - 1; zbh_i++) {
        for (int zbh_j = 0; zbh_j < zbh_collegeCount - 1 - zbh_i; zbh_j++) {
            if (zbh_collegeStats[zbh_j].zbh_totalScore < zbh_collegeStats[zbh_j+1].zbh_totalScore) {
                zbh_swapCollege(&zbh_collegeStats[zbh_j], &zbh_collegeStats[zbh_j+1]);
            }
        }
    }
}

// 增删改查逻辑
void zbh_addAthlete() {
    if (zbh_currentCount >= zbh_MAX_ATHLETES) {
        printf(">>> 人数已满！\n");
        return;
    }
    zbh_Athlete zbh_a;
    printf("\n--- 新增运动员 ---\n");
    printf("1. 编号 (如 TW001): "); scanf("%s", zbh_a.zbh_id);
    printf("2. 姓名 (如 常凯申): "); scanf("%s", zbh_a.zbh_name);
    printf("3. 项目 (如 睡衣登山大赛): "); scanf("%s", zbh_a.zbh_event);
    printf("4. 学院 (如 黄埔军校(怡红院)): "); scanf("%s", zbh_a.zbh_college);
    printf("5. 预赛成绩: "); scanf("%lf", &zbh_a.zbh_prelim);
    printf("6. 决赛成绩: "); scanf("%lf", &zbh_a.zbh_finalScore);
    getchar();
    zbh_a.zbh_rank = 0; zbh_a.zbh_score = 0; zbh_a.zbh_isBrokenRecord = 0;
    zbh_athletes[zbh_currentCount++] = zbh_a;
    printf(">>> 录入成功！\n");
    zbh_sortAndCalculate(); 
}

void zbh_modifyRecord() {
    if (zbh_currentCount == 0) { printf("\n【提示】暂无数据。\n"); return; }
    char zbh_targetId[20];
    printf("\n--- 修改记录 ---\n请输入要修改的运动员编号: ");
    scanf("%s", zbh_targetId); getchar();
    
    int zbh_foundIndex = -1;
    for (int zbh_i = 0; zbh_i < zbh_currentCount; zbh_i++) {
        if (strcmp(zbh_athletes[zbh_i].zbh_id, zbh_targetId) == 0) { zbh_foundIndex = zbh_i; break; }
    }
    
    if (zbh_foundIndex == -1) { printf(">>> 未找到该编号的运动员。\n"); return; }
    
    printf("\n当前信息：\n");
    printf("1. 编号: %s\n", zbh_athletes[zbh_foundIndex].zbh_id);
    printf("2. 姓名: %s\n", zbh_athletes[zbh_foundIndex].zbh_name);
    printf("3. 项目: %s\n", zbh_athletes[zbh_foundIndex].zbh_event);
    printf("4. 学院: %s\n", zbh_athletes[zbh_foundIndex].zbh_college);
    printf("5. 预赛成绩: %.2f\n", zbh_athletes[zbh_foundIndex].zbh_prelim);
    printf("6. 决赛成绩: %.2f\n", zbh_athletes[zbh_foundIndex].zbh_finalScore);
    printf("\n请输入新的信息（直接回车跳过修改）：\n");
    
    char zbh_input[50];
    printf("新姓名: "); fgets(zbh_input, sizeof(zbh_input), stdin);
    zbh_input[strcspn(zbh_input, "\n")] = 0;
    if (strlen(zbh_input) > 0) strcpy(zbh_athletes[zbh_foundIndex].zbh_name, zbh_input);
    
    printf("新项目: "); fgets(zbh_input, sizeof(zbh_input), stdin);
    zbh_input[strcspn(zbh_input, "\n")] = 0;
    if (strlen(zbh_input) > 0) strcpy(zbh_athletes[zbh_foundIndex].zbh_event, zbh_input);
    
    printf("新学院: "); fgets(zbh_input, sizeof(zbh_input), stdin);
    zbh_input[strcspn(zbh_input, "\n")] = 0;
    if (strlen(zbh_input) > 0) strcpy(zbh_athletes[zbh_foundIndex].zbh_college, zbh_input);
    
    printf("新预赛成绩: "); fgets(zbh_input, sizeof(zbh_input), stdin);
    zbh_input[strcspn(zbh_input, "\n")] = 0;
    if (strlen(zbh_input) > 0) zbh_athletes[zbh_foundIndex].zbh_prelim = atof(zbh_input);
    
    printf("新决赛成绩: "); fgets(zbh_input, sizeof(zbh_input), stdin);
    zbh_input[strcspn(zbh_input, "\n")] = 0;
    if (strlen(zbh_input) > 0) zbh_athletes[zbh_foundIndex].zbh_finalScore = atof(zbh_input);
    
    printf(">>> 修改成功！\n");
    zbh_sortAndCalculate(); 
}

void zbh_deleteRecord() {
    if (zbh_currentCount == 0) { printf("\n【提示】暂无数据。\n"); return; }
    char zbh_targetId[20];
    printf("\n--- 删除记录 ---\n请输入要删除的运动员编号: ");
    scanf("%s", zbh_targetId); getchar();
    
    int zbh_foundIndex = -1;
    for (int zbh_i = 0; zbh_i < zbh_currentCount; zbh_i++) {
        if (strcmp(zbh_athletes[zbh_i].zbh_id, zbh_targetId) == 0) { zbh_foundIndex = zbh_i; break; }
    }
    
    if (zbh_foundIndex == -1) { printf(">>> 未找到该编号的运动员。\n"); return; }
    
    printf("确认删除 %s (%s) 的记录吗？(Y/N): ", zbh_athletes[zbh_foundIndex].zbh_name, zbh_athletes[zbh_foundIndex].zbh_id);
    char zbh_confirm; scanf("%c", &zbh_confirm);
    
    if (zbh_confirm == 'Y' || zbh_confirm == 'y') {
        for (int zbh_i = zbh_foundIndex; zbh_i < zbh_currentCount - 1; zbh_i++) {
            zbh_athletes[zbh_i] = zbh_athletes[zbh_i + 1];
        }
        zbh_currentCount--;
        printf(">>> 删除成功！\n");
        zbh_sortAndCalculate(); 
    } else { printf(">>> 取消删除。\n"); }
}

void zbh_initTestData() {
    if (zbh_currentCount > 0) {
        printf(">>> 系统中已有数据，是否清空并重新加载测试数据？(Y/N): ");
        char zbh_confirm; scanf(" %c", &zbh_confirm); getchar();
        if (zbh_confirm != 'Y' && zbh_confirm != 'y') return;
    }
    zbh_currentCount = 0;
    zbh_Athlete zbh_data[] = {
        {"YD001", "张三", "100米跑", "计算机学院", 11.5, 11.2, 0, 0, 0},
        {"YD002", "李四", "跳远", "软件学院", 5.8, 6.1, 0, 0, 0},
        {"YD003", "王五", "铅球", "网络学院", 10.2, 10.8, 0, 0, 0},
        {"YD004", "赵六", "400米跑", "计算机学院", 56.3, 55.8, 0, 0, 0},
        {"YD005", "钱七", "跳高", "软件学院", 1.65, 1.70, 0, 0, 0},
        {"YD006", "孙八", "100米跑", "体育学院", 10.8, 10.5, 0, 0, 1},
        {"YD007", "周九", "跳远", "文学院", 5.5, 5.9, 0, 0, 0},
        {"YD008", "吴十", "铅球", "理学院", 11.0, 11.5, 0, 0, 0},
        {"YD009", "郑十一", "400米跑", "计算机学院", 55.0, 54.5, 0, 0, 1},
        {"YD010", "王十二", "跳高", "体育学院", 1.75, 1.80, 0, 0, 0},
        {"YD011", "李十三", "100米跑", "软件学院", 11.5, 11.2, 0, 0, 0},
        {"YD012", "张十四", "跳远", "计算机学院", 6.0, 6.2, 0, 0, 0},
        {"YD013", "刘十五", "铅球", "体育学院", 11.5, 12.0, 0, 0, 0},
        {"YD014", "陈十六", "400米跑", "软件学院", 57.0, 56.5, 0, 0, 0},
        {"YD015", "杨十七", "跳高", "文学院", 1.60, 1.65, 0, 0, 0}
    };
    int zbh_size = sizeof(zbh_data) / sizeof(zbh_data[0]);
    for (int zbh_i = 0; zbh_i < zbh_size; zbh_i++) zbh_athletes[zbh_currentCount++] = zbh_data[zbh_i];
    zbh_sortAndCalculate();
    printf(">>> 测试数据已加载（共 %d 条记录）。\n", zbh_size);
}

// 查询功能实现
void zbh_searchByIdOrName() {
    if (zbh_currentCount == 0) { printf("\n【提示】暂无数据。\n"); return; }
    char zbh_keyword[50];
    printf("\n--- 按姓名/编号查询 ---\n请输入姓名或编号: ");
    scanf("%s", zbh_keyword); getchar();
    printf("\n----------------------------------------------------------------------------------------\n");
    zbh_print_aligned("编号", 10); zbh_print_aligned("姓名", 12); zbh_print_aligned("项目", 14);
    zbh_print_aligned("学院", 18); zbh_print_aligned("决赛成绩", 12); zbh_print_aligned("名次", 8);
    zbh_print_aligned("得分", 8); printf("破纪录\n");
    printf("----------------------------------------------------------------------------------------\n");
    int zbh_found = 0;
    for (int zbh_i = 0; zbh_i < zbh_currentCount; zbh_i++) {
        if (strstr(zbh_athletes[zbh_i].zbh_id, zbh_keyword) != NULL || 
            strstr(zbh_athletes[zbh_i].zbh_name, zbh_keyword) != NULL) {
            char zbh_display_id[30], zbh_display_name[30];
            sprintf(zbh_display_id, "%s", zbh_athletes[zbh_i].zbh_id);
            sprintf(zbh_display_name, "%s", zbh_athletes[zbh_i].zbh_name);
            zbh_print_highlight(zbh_display_id, zbh_keyword, 10);
            zbh_print_highlight(zbh_display_name, zbh_keyword, 12);
            zbh_print_aligned(zbh_athletes[zbh_i].zbh_event, 14);
            zbh_print_aligned(zbh_athletes[zbh_i].zbh_college, 18);
            char zbh_scoreStr[20]; sprintf(zbh_scoreStr, "%.2f", zbh_athletes[zbh_i].zbh_finalScore);
            zbh_print_aligned(zbh_scoreStr, 12);
            char zbh_rankStr[10]; sprintf(zbh_rankStr, "%d", zbh_athletes[zbh_i].zbh_rank);
            zbh_print_aligned(zbh_rankStr, 8);
            char zbh_scorePoint[10]; sprintf(zbh_scorePoint, "%d", zbh_athletes[zbh_i].zbh_score);
            zbh_print_aligned(zbh_scorePoint, 8);
            printf("%s\n", zbh_athletes[zbh_i].zbh_isBrokenRecord ? "★" : " ");
            zbh_found = 1;
        }
    }
    printf("----------------------------------------------------------------------------------------\n");
    if (!zbh_found) printf("\n【提示】未找到相关记录。\n");
}

void zbh_searchByEvent() {
    if (zbh_currentCount == 0) { printf("\n【提示】暂无数据。\n"); return; }
    char zbh_eventName[50];
    printf("\n--- 按项目查询 ---\n请输入项目名称: ");
    scanf("%s", zbh_eventName); getchar();
    printf("\n--- %s 项目成绩单 ---\n", zbh_eventName);
    printf("----------------------------------------------------------------------------------------\n");
    zbh_print_aligned("排名", 8); zbh_print_aligned("编号", 10); zbh_print_aligned("姓名", 12);
    zbh_print_aligned("学院", 18); zbh_print_aligned("预赛成绩", 12); zbh_print_aligned("决赛成绩", 12);
    zbh_print_aligned("得分", 8); printf("破纪录\n");
    printf("----------------------------------------------------------------------------------------\n");
    int zbh_found = 0;
    for (int zbh_i = 0; zbh_i < zbh_currentCount; zbh_i++) {
        if (strcmp(zbh_athletes[zbh_i].zbh_event, zbh_eventName) == 0) {
            char zbh_rankStr[10], zbh_prelimStr[20], zbh_finalStr[20], zbh_scoreStr[10];
            sprintf(zbh_rankStr, "%d", zbh_athletes[zbh_i].zbh_rank);
            sprintf(zbh_prelimStr, "%.2f", zbh_athletes[zbh_i].zbh_prelim);
            sprintf(zbh_finalStr, "%.2f", zbh_athletes[zbh_i].zbh_finalScore);
            sprintf(zbh_scoreStr, "%d", zbh_athletes[zbh_i].zbh_score);
            zbh_print_aligned(zbh_rankStr, 8);
            zbh_print_aligned(zbh_athletes[zbh_i].zbh_id, 10);
            zbh_print_aligned(zbh_athletes[zbh_i].zbh_name, 12);
            zbh_print_aligned(zbh_athletes[zbh_i].zbh_college, 18);
            zbh_print_aligned(zbh_prelimStr, 12);
            zbh_print_aligned(zbh_finalStr, 12);
            zbh_print_aligned(zbh_scoreStr, 8);
            printf("%s\n", zbh_athletes[zbh_i].zbh_isBrokenRecord ? "★" : " ");
            zbh_found = 1;
        }
    }
    printf("----------------------------------------------------------------------------------------\n");
    if (!zbh_found) printf("\n【提示】未找到该项目的记录。\n");
}

void zbh_searchByScoreRange() {
    if (zbh_currentCount == 0) { printf("\n【提示】暂无数据。\n"); return; }
    double zbh_minScore, zbh_maxScore;
    printf("\n--- 按成绩区间查询 ---\n请输入最低成绩: "); scanf("%lf", &zbh_minScore);
    printf("请输入最高成绩: "); scanf("%lf", &zbh_maxScore); getchar();
    printf("\n--- 成绩在 %.2f ~ %.2f 之间的运动员 ---\n", zbh_minScore, zbh_maxScore);
    printf("----------------------------------------------------------------------------------------\n");
    zbh_print_aligned("编号", 10); zbh_print_aligned("姓名", 12); zbh_print_aligned("项目", 14);
    zbh_print_aligned("学院", 18); zbh_print_aligned("决赛成绩", 12); printf("单位\n");
    printf("----------------------------------------------------------------------------------------\n");
    int zbh_found = 0;
    for (int zbh_i = 0; zbh_i < zbh_currentCount; zbh_i++) {
        if (zbh_athletes[zbh_i].zbh_finalScore >= zbh_minScore && zbh_athletes[zbh_i].zbh_finalScore <= zbh_maxScore) {
            zbh_print_aligned(zbh_athletes[zbh_i].zbh_id, 10);
            zbh_print_aligned(zbh_athletes[zbh_i].zbh_name, 12);
            zbh_print_aligned(zbh_athletes[zbh_i].zbh_event, 14);
            zbh_print_aligned(zbh_athletes[zbh_i].zbh_college, 18);
            char zbh_scoreStr[20]; sprintf(zbh_scoreStr, "%.2f", zbh_athletes[zbh_i].zbh_finalScore);
            zbh_print_aligned(zbh_scoreStr, 12);
            printf("%s\n", zbh_isRunningEvent(zbh_athletes[zbh_i].zbh_event) ? "秒" : "米");
            zbh_found = 1;
        }
    }
    printf("----------------------------------------------------------------------------------------\n");
    if (!zbh_found) printf("\n【提示】未找到该成绩区间的记录。\n");
}

void zbh_searchByCollege() {
    if (zbh_currentCount == 0) { printf("\n【提示】暂无数据。\n"); return; }
    char zbh_collegeName[50];
    printf("\n--- 按学院查询 ---\n请输入学院名称: "); scanf("%s", zbh_collegeName); getchar();
    int zbh_found = 0; int zbh_totalScore = 0;
    printf("\n--- %s 学院参赛情况 ---\n", zbh_collegeName);
    printf("----------------------------------------------------------------------------------------\n");
    zbh_print_aligned("编号", 10); zbh_print_aligned("姓名", 12); zbh_print_aligned("项目", 14);
    zbh_print_aligned("决赛成绩", 12); zbh_print_aligned("名次", 8); zbh_print_aligned("得分", 8);
    printf("\n----------------------------------------------------------------------------------------\n");
    for (int zbh_i = 0; zbh_i < zbh_currentCount; zbh_i++) {
        if (strcmp(zbh_athletes[zbh_i].zbh_college, zbh_collegeName) == 0) {
            char zbh_scoreStr[20], zbh_rankStr[10], zbh_pointStr[10];
            sprintf(zbh_scoreStr, "%.2f", zbh_athletes[zbh_i].zbh_finalScore);
            sprintf(zbh_rankStr, "%d", zbh_athletes[zbh_i].zbh_rank);
            sprintf(zbh_pointStr, "%d", zbh_athletes[zbh_i].zbh_score);
            zbh_print_aligned(zbh_athletes[zbh_i].zbh_id, 10);
            zbh_print_aligned(zbh_athletes[zbh_i].zbh_name, 12);
            zbh_print_aligned(zbh_athletes[zbh_i].zbh_event, 14);
            zbh_print_aligned(zbh_scoreStr, 12);
            zbh_print_aligned(zbh_rankStr, 8);
            zbh_print_aligned(zbh_pointStr, 8);
            printf("\n");
            zbh_totalScore += zbh_athletes[zbh_i].zbh_score;
            zbh_found = 1;
        }
    }
    printf("----------------------------------------------------------------------------------------\n");
    if (zbh_found) printf("学院总分: %d 分\n", zbh_totalScore);
    else printf("\n【提示】未找到该学院的记录。\n");
}

// 统计分析
void zbh_showCollegeRanking() {
    zbh_calculateCollegeStats();
    printf("\n--- 学院总分排名榜 ---\n");
    printf("================================================================\n");
    zbh_print_aligned("排名", 8); zbh_print_aligned("学院名称", 20);
    zbh_print_aligned("总分", 10); zbh_print_aligned("金牌", 8);
    zbh_print_aligned("银牌", 8); zbh_print_aligned("铜牌", 8); printf("\n");
    printf("================================================================\n");
    for (int zbh_i = 0; zbh_i < zbh_collegeCount; zbh_i++) {
        char zbh_rankStr[10], zbh_scoreStr[10], zbh_goldStr[10], zbh_silverStr[10], zbh_bronzeStr[10];
        sprintf(zbh_rankStr, "%d", zbh_i + 1);
        sprintf(zbh_scoreStr, "%d", zbh_collegeStats[zbh_i].zbh_totalScore);
        sprintf(zbh_goldStr, "%d", zbh_collegeStats[zbh_i].zbh_goldCount);
        sprintf(zbh_silverStr, "%d", zbh_collegeStats[zbh_i].zbh_silverCount);
        sprintf(zbh_bronzeStr, "%d", zbh_collegeStats[zbh_i].zbh_bronzeCount);
        zbh_print_aligned(zbh_rankStr, 8); zbh_print_aligned(zbh_collegeStats[zbh_i].zbh_name, 20);
        zbh_print_aligned(zbh_scoreStr, 10); zbh_print_aligned(zbh_goldStr, 8);
        zbh_print_aligned(zbh_silverStr, 8); zbh_print_aligned(zbh_bronzeStr, 8); printf("\n");
    }
    printf("================================================================\n");
}

void zbh_showRecordBreakers() {
    int zbh_count = 0;
    printf("\n--- 破纪录统计 ---\n");
    printf("================================================================\n");
    zbh_print_aligned("项目", 16); zbh_print_aligned("运动员", 12);
    zbh_print_aligned("学院", 18); zbh_print_aligned("成绩", 12); zbh_print_aligned("纪录类型", 12); printf("\n");
    printf("================================================================\n");
    for (int zbh_i = 0; zbh_i < zbh_currentCount; zbh_i++) {
        if (zbh_athletes[zbh_i].zbh_isBrokenRecord) {
            char zbh_recordType[20] = "未知"; double zbh_oldRecord = 0;
            for (int zbh_j = 0; zbh_j < zbh_recordCount; zbh_j++) {
                if (strcmp(zbh_athletes[zbh_i].zbh_event, zbh_gameRecords[zbh_j].zbh_event) == 0) {
                    strcpy(zbh_recordType, zbh_gameRecords[zbh_j].zbh_holder);
                    zbh_oldRecord = zbh_gameRecords[zbh_j].zbh_record;
                    break;
                }
            }
            zbh_print_aligned(zbh_athletes[zbh_i].zbh_event, 16);
            zbh_print_aligned(zbh_athletes[zbh_i].zbh_name, 12);
            zbh_print_aligned(zbh_athletes[zbh_i].zbh_college, 18);
            char zbh_scoreStr[20]; sprintf(zbh_scoreStr, "%.2f", zbh_athletes[zbh_i].zbh_finalScore);
            zbh_print_aligned(zbh_scoreStr, 12);
            char zbh_recordInfo[30]; sprintf(zbh_recordInfo, "%s(原%.2f)", zbh_recordType, zbh_oldRecord);
            zbh_print_aligned(zbh_recordInfo, 12); printf("\n");
            zbh_count++;
        }
    }
    printf("================================================================\n");
    printf("总计破纪录: %d 人次\n", zbh_count);
}

void zbh_showScoreDistribution() {
    int zbh_rankCount[6] = {0};
    int zbh_scoreSegments[6] = {0}; 
    
    for (int zbh_i = 0; zbh_i < zbh_currentCount; zbh_i++) {
        if (zbh_athletes[zbh_i].zbh_rank >= 1 && zbh_athletes[zbh_i].zbh_rank <= 5) zbh_rankCount[zbh_athletes[zbh_i].zbh_rank]++;
        else zbh_rankCount[0]++;
        int zbh_score = zbh_athletes[zbh_i].zbh_score;
        if (zbh_score <= 5) zbh_scoreSegments[0]++;
        else if (zbh_score <= 10) zbh_scoreSegments[1]++;
        else if (zbh_score <= 15) zbh_scoreSegments[2]++;
        else if (zbh_score <= 20) zbh_scoreSegments[3]++;
        else if (zbh_score <= 25) zbh_scoreSegments[4]++;
        else zbh_scoreSegments[5]++;
    }
    printf("\n--- 成绩分布分析 ---\n1. 名次分布：\n");
    printf("  第1名: %d 人\n  第2名: %d 人\n  第3名: %d 人\n  第4名: %d 人\n  第5名: %d 人\n  无排名: %d 人\n",
           zbh_rankCount[1], zbh_rankCount[2], zbh_rankCount[3], zbh_rankCount[4], zbh_rankCount[5], zbh_rankCount[0]);
    printf("\n2. 得分区间分布：\n");
    printf("  0-5分: %d 人\n  6-10分: %d 人\n  11-15分: %d 人\n  16-20分: %d 人\n  21-25分: %d 人\n  26分以上: %d 人\n",
           zbh_scoreSegments[0], zbh_scoreSegments[1], zbh_scoreSegments[2], zbh_scoreSegments[3], zbh_scoreSegments[4], zbh_scoreSegments[5]);
    printf("\n3. 统计摘要：\n  总参赛人数: %d 人\n  有得分人数: %d 人\n  获奖率: %.1f%%\n", 
           zbh_currentCount, zbh_currentCount - zbh_rankCount[0], (zbh_currentCount - zbh_rankCount[0]) * 100.0 / zbh_currentCount);
}

void zbh_groupCompetition() {
    printf("\n--- 项目分组 ---\n此功能用于将参赛者分配到预赛/决赛组\n");
    typedef struct {
        char zbh_event[20];
        int zbh_count;
        zbh_Athlete *zbh_participants[zbh_MAX_ATHLETES];
    } zbh_EventGroup;
    zbh_EventGroup zbh_groups[50];
    int zbh_groupCount = 0;
    
    for (int zbh_i = 0; zbh_i < zbh_currentCount; zbh_i++) {
        int zbh_found = 0;
        for (int zbh_j = 0; zbh_j < zbh_groupCount; zbh_j++) {
            if (strcmp(zbh_athletes[zbh_i].zbh_event, zbh_groups[zbh_j].zbh_event) == 0) {
                zbh_groups[zbh_j].zbh_participants[zbh_groups[zbh_j].zbh_count++] = &zbh_athletes[zbh_i];
                zbh_found = 1; break;
            }
        }
        if (!zbh_found) {
            strcpy(zbh_groups[zbh_groupCount].zbh_event, zbh_athletes[zbh_i].zbh_event);
            zbh_groups[zbh_groupCount].zbh_count = 1;
            zbh_groups[zbh_groupCount].zbh_participants[0] = &zbh_athletes[zbh_i];
            zbh_groupCount++;
        }
    }
    
    for (int zbh_i = 0; zbh_i < zbh_groupCount; zbh_i++) {
        printf("\n项目：%s\n参赛人数：%d 人\n", zbh_groups[zbh_i].zbh_event, zbh_groups[zbh_i].zbh_count);
        if (zbh_groups[zbh_i].zbh_count <= 8) printf("建议：直接决赛\n");
        else if (zbh_groups[zbh_i].zbh_count <= 16) printf("建议：分2组预赛，每组前4名进决赛\n");
        else if (zbh_groups[zbh_i].zbh_count <= 24) printf("建议：分3组预赛，每组前2名+2个最好成绩进决赛\n");
        else printf("建议：分4组预赛，每组前3名进决赛\n");
        
        for (int zbh_j = 0; zbh_j < zbh_groups[zbh_i].zbh_count - 1; zbh_j++) {
            for (int zbh_k = 0; zbh_k < zbh_groups[zbh_i].zbh_count - 1 - zbh_j; zbh_k++) {
                int zbh_isRun = zbh_isRunningEvent(zbh_groups[zbh_i].zbh_event);
                if ((zbh_isRun && zbh_groups[zbh_i].zbh_participants[zbh_k]->zbh_prelim > zbh_groups[zbh_i].zbh_participants[zbh_k+1]->zbh_prelim) ||
                    (!zbh_isRun && zbh_groups[zbh_i].zbh_participants[zbh_k]->zbh_prelim < zbh_groups[zbh_i].zbh_participants[zbh_k+1]->zbh_prelim)) {
                    zbh_Athlete *zbh_temp = zbh_groups[zbh_i].zbh_participants[zbh_k];
                    zbh_groups[zbh_i].zbh_participants[zbh_k] = zbh_groups[zbh_i].zbh_participants[zbh_k+1];
                    zbh_groups[zbh_i].zbh_participants[zbh_k+1] = zbh_temp;
                }
            }
        }
        printf("预赛成绩排序（前8名）：\n");
        for (int zbh_j = 0; zbh_j < (zbh_groups[zbh_i].zbh_count < 8 ? zbh_groups[zbh_i].zbh_count : 8); zbh_j++) {
            printf("  %d. %s: %.2f\n", zbh_j + 1, zbh_groups[zbh_i].zbh_participants[zbh_j]->zbh_name, zbh_groups[zbh_i].zbh_participants[zbh_j]->zbh_prelim);
        }
        printf("\n");
    }
}

void zbh_validateScores() {
    printf("\n--- 成绩验证 ---\n");
    int zbh_errorCount = 0; int zbh_warningCount = 0;
    for (int zbh_i = 0; zbh_i < zbh_currentCount; zbh_i++) {
        int zbh_isRun = zbh_isRunningEvent(zbh_athletes[zbh_i].zbh_event);
        if (zbh_athletes[zbh_i].zbh_finalScore <= 0) {
            printf("? 错误：%s 的决赛成绩异常（%.2f）\n", zbh_athletes[zbh_i].zbh_name, zbh_athletes[zbh_i].zbh_finalScore);
            zbh_errorCount++;
        }
        if (zbh_isRun && zbh_athletes[zbh_i].zbh_finalScore > 120) {
            printf("? 警告：%s 的 %s 成绩可能过慢（%.2f秒）\n", zbh_athletes[zbh_i].zbh_name, zbh_athletes[zbh_i].zbh_event, zbh_athletes[zbh_i].zbh_finalScore);
            zbh_warningCount++;
        }
        if (!zbh_isRun && zbh_athletes[zbh_i].zbh_finalScore > 20) {
            printf("? 警告：%s 的 %s 成绩可能过大（%.2f米）\n", zbh_athletes[zbh_i].zbh_name, zbh_athletes[zbh_i].zbh_event, zbh_athletes[zbh_i].zbh_finalScore);
            zbh_warningCount++;
        }
        if (zbh_isRun) {
            if (zbh_athletes[zbh_i].zbh_finalScore < zbh_athletes[zbh_i].zbh_prelim * 0.8) {
                printf("? 警告：%s 的决赛成绩比预赛快太多（预赛%.2f，决赛%.2f）\n", zbh_athletes[zbh_i].zbh_name, zbh_athletes[zbh_i].zbh_prelim, zbh_athletes[zbh_i].zbh_finalScore); zbh_warningCount++;
            }
        } else {
            if (zbh_athletes[zbh_i].zbh_finalScore > zbh_athletes[zbh_i].zbh_prelim * 1.2) {
                printf("? 警告：%s 的决赛成绩比预赛好太多（预赛%.2f，决赛%.2f）\n", zbh_athletes[zbh_i].zbh_name, zbh_athletes[zbh_i].zbh_prelim, zbh_athletes[zbh_i].zbh_finalScore); zbh_warningCount++;
            }
        }
    }
    printf("\n验证结果：\n  发现错误：%d 处\n  发现警告：%d 处\n", zbh_errorCount, zbh_warningCount);
    if (zbh_errorCount == 0 && zbh_warningCount == 0) printf("  ? 所有成绩数据正常\n");
}

void zbh_displayAll(int zbh_page) {
    if (zbh_currentCount == 0) { printf("\n【提示】暂无数据。\n"); return; }
    zbh_sortAndCalculate();
    int zbh_start = (zbh_page - 1) * zbh_PAGE_SIZE;
    int zbh_end = zbh_start + zbh_PAGE_SIZE;
    if (zbh_end > zbh_currentCount) zbh_end = zbh_currentCount;
    if (zbh_start >= zbh_currentCount) { printf("\n【提示】没有更多数据了。\n"); return; }
    
    printf("\n----------------------------------------------------------------------------------------\n");
    zbh_print_aligned("编号", 10); zbh_print_aligned("姓名", 12); zbh_print_aligned("项目", 14);
    zbh_print_aligned("学院", 18); zbh_print_aligned("决赛成绩", 12); zbh_print_aligned("名次", 8);
    zbh_print_aligned("得分", 8); printf("纪录\n");
    printf("----------------------------------------------------------------------------------------\n");
    for (int zbh_i = zbh_start; zbh_i < zbh_end; zbh_i++) {
        char zbh_scoreStr[20], zbh_rankStr[10], zbh_pointStr[10];
        sprintf(zbh_scoreStr, "%.2f", zbh_athletes[zbh_i].zbh_finalScore);
        sprintf(zbh_rankStr, "%d", zbh_athletes[zbh_i].zbh_rank);
        sprintf(zbh_pointStr, "%d", zbh_athletes[zbh_i].zbh_score);
        zbh_print_aligned(zbh_athletes[zbh_i].zbh_id, 10);
        zbh_print_aligned(zbh_athletes[zbh_i].zbh_name, 12);
        zbh_print_aligned(zbh_athletes[zbh_i].zbh_event, 14);
        zbh_print_aligned(zbh_athletes[zbh_i].zbh_college, 18);
        zbh_print_aligned(zbh_scoreStr, 12);
        zbh_print_aligned(zbh_rankStr, 8);
        zbh_print_aligned(zbh_pointStr, 8);
        printf("%s\n", zbh_athletes[zbh_i].zbh_isBrokenRecord ? "★" : " ");
        if (zbh_i < zbh_currentCount - 1 && zbh_i < zbh_end - 1 && strcmp(zbh_athletes[zbh_i].zbh_event, zbh_athletes[zbh_i+1].zbh_event) != 0) {
            printf("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
        }
    }
    printf("----------------------------------------------------------------------------------------\n");
    printf("页码: %d/%d (共%d条记录)\n", zbh_page, (zbh_currentCount + zbh_PAGE_SIZE - 1) / zbh_PAGE_SIZE, zbh_currentCount);
}

void zbh_printScoreReport() {
    if (zbh_currentCount == 0) { printf("\n【提示】暂无数据。\n"); return; }
    zbh_sortAndCalculate();
    printf("\n=========== 运动会成绩总表 ===========\n");
    time_t zbh_currentTime = time(NULL);
    printf("打印时间: %s", ctime(&zbh_currentTime));
    printf("=====================================\n");
    int zbh_page = 1; int zbh_start = 0;
    while (zbh_start < zbh_currentCount) {
        printf("\n--- 第 %d 页 ---\n", zbh_page);
        printf("----------------------------------------------------------------------------------------\n");
        zbh_print_aligned("编号", 10); zbh_print_aligned("姓名", 12); zbh_print_aligned("项目", 14);
        zbh_print_aligned("学院", 18); zbh_print_aligned("决赛成绩", 12); zbh_print_aligned("名次", 8);
        zbh_print_aligned("得分", 8); printf("\n");
        printf("----------------------------------------------------------------------------------------\n");
        int zbh_end = (zbh_start + zbh_PAGE_SIZE < zbh_currentCount) ? zbh_start + zbh_PAGE_SIZE : zbh_currentCount;
        for (int zbh_i = zbh_start; zbh_i < zbh_end; zbh_i++) {
            char zbh_scoreStr[20], zbh_rankStr[10], zbh_pointStr[10];
            sprintf(zbh_scoreStr, "%.2f", zbh_athletes[zbh_i].zbh_finalScore);
            sprintf(zbh_rankStr, "%d", zbh_athletes[zbh_i].zbh_rank);
            sprintf(zbh_pointStr, "%d", zbh_athletes[zbh_i].zbh_score);
            zbh_print_aligned(zbh_athletes[zbh_i].zbh_id, 10); zbh_print_aligned(zbh_athletes[zbh_i].zbh_name, 12);
            zbh_print_aligned(zbh_athletes[zbh_i].zbh_event, 14); zbh_print_aligned(zbh_athletes[zbh_i].zbh_college, 18);
            zbh_print_aligned(zbh_scoreStr, 12); zbh_print_aligned(zbh_rankStr, 8); zbh_print_aligned(zbh_pointStr, 8);
            printf("\n");
        }
        printf("----------------------------------------------------------------------------------------\n");
        printf("页码: %d/%d\n", zbh_page, (zbh_currentCount + zbh_PAGE_SIZE - 1) / zbh_PAGE_SIZE);
        zbh_start = zbh_end; zbh_page++;
        if (zbh_start < zbh_currentCount) { printf("\n按回车键继续下一页..."); getchar(); }
    }
}