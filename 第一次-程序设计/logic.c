// 业务逻辑实现
// 包含了所有的计算、排序、查询和交互逻辑。

#include "logic.h"
#include "globals.h"
#include "utils.h"

// 排序与计算核心算法

/*
就是个小冒泡，O(n^2) ,凑活用。 
*/
void sortAndCalculate() {
    if (currentCount == 0) return;

    for (int i = 0; i < currentCount - 1; i++) {
        for (int j = 0; j < currentCount - 1 - i; j++) {
            int needSwap = 0;
            int eventCompare = strcmp(athletes[j].event, athletes[j+1].event);
            
            if (eventCompare > 0) {
                needSwap = 1; 
            } else if (eventCompare == 0) {
                int isRun = isRunningEvent(athletes[j].event);
                if (isRun && athletes[j].finalScore > athletes[j+1].finalScore) needSwap = 1; 
                if (!isRun && athletes[j].finalScore < athletes[j+1].finalScore) needSwap = 1; 
            }
            if (needSwap) swapAthlete(&athletes[j], &athletes[j+1]);
        }
    }

    int eventStartIndex = 0;
    for (int i = 0; i < currentCount; i++) {
        if (i == 0 || strcmp(athletes[i].event, athletes[i-1].event) != 0) {
            athletes[i].rank = 1;
            eventStartIndex = i; 
        } else {
            int isRun = isRunningEvent(athletes[i].event);
            int isTie = 0;
            
            if (isRun && athletes[i].finalScore == athletes[i-1].finalScore) isTie = 1;
            else if (!isRun && athletes[i].finalScore == athletes[i-1].finalScore) isTie = 1;

            if (isTie) athletes[i].rank = athletes[i-1].rank;
            else athletes[i].rank = (i - eventStartIndex) + 1;
        }
        
        athletes[i].score = getScoreByRank(athletes[i].rank);
        athletes[i].isBrokenRecord = 0;
        for (int j = 0; j < recordCount; j++) {
            if (strcmp(athletes[i].event, gameRecords[j].event) == 0) {
                int isRun = isRunningEvent(athletes[i].event);
                if ((isRun && athletes[i].finalScore < gameRecords[j].record) ||
                    (!isRun && athletes[i].finalScore > gameRecords[j].record)) {
                    athletes[i].isBrokenRecord = 1;
                    break;
                }
            }
        }
    }
}

void calculateCollegeStats() {
    collegeCount = 0;
    for (int i = 0; i < currentCount; i++) {
        int found = 0;
        for (int j = 0; j < collegeCount; j++) {
            if (strcmp(athletes[i].college, collegeStats[j].name) == 0) {
                collegeStats[j].totalScore += athletes[i].score;
                if (athletes[i].rank == 1) collegeStats[j].goldCount++;
                if (athletes[i].rank == 2) collegeStats[j].silverCount++;
                if (athletes[i].rank == 3) collegeStats[j].bronzeCount++;
                found = 1;
                break;
            }
        }
        if (!found) {
            strcpy(collegeStats[collegeCount].name, athletes[i].college);
            collegeStats[collegeCount].totalScore = athletes[i].score;
            collegeStats[collegeCount].goldCount = (athletes[i].rank == 1) ? 1 : 0;
            collegeStats[collegeCount].silverCount = (athletes[i].rank == 2) ? 1 : 0;
            collegeStats[collegeCount].bronzeCount = (athletes[i].rank == 3) ? 1 : 0;
            collegeCount++;
        }
    }
    
    for (int i = 0; i < collegeCount - 1; i++) {
        for (int j = 0; j < collegeCount - 1 - i; j++) {
            if (collegeStats[j].totalScore < collegeStats[j+1].totalScore) {
                swapCollege(&collegeStats[j], &collegeStats[j+1]);
            }
        }
    }
}

// 增删改查逻辑
void addAthlete() {
    if (currentCount >= MAX_ATHLETES) {
        printf(">>> 人数已满！\n");
        return;
    }
    Athlete a;
    printf("\n--- 新增运动员 ---\n");
    printf("1. 编号 (如 TW001): "); scanf("%s", a.id);
    printf("2. 姓名 (如 常凯申): "); scanf("%s", a.name);
    printf("3. 项目 (如 睡衣登山大赛): "); scanf("%s", a.event);
    printf("4. 学院 (如 黄埔军校(怡红院)): "); scanf("%s", a.college);
    printf("5. 预赛成绩: "); scanf("%lf", &a.prelim);
    printf("6. 决赛成绩: "); scanf("%lf", &a.finalScore);
    getchar();
    a.rank = 0; a.score = 0; a.isBrokenRecord = 0;
    athletes[currentCount++] = a;
    printf(">>> 录入成功！\n");
    sortAndCalculate(); 
}

void modifyRecord() {
    if (currentCount == 0) { printf("\n【提示】暂无数据。\n"); return; }
    char targetId[20];
    printf("\n--- 修改记录 ---\n请输入要修改的运动员编号: ");
    scanf("%s", targetId); getchar();
    
    int foundIndex = -1;
    for (int i = 0; i < currentCount; i++) {
        if (strcmp(athletes[i].id, targetId) == 0) { foundIndex = i; break; }
    }
    
    if (foundIndex == -1) { printf(">>> 未找到该编号的运动员。\n"); return; }
    
    printf("\n当前信息：\n");
    printf("1. 编号: %s\n", athletes[foundIndex].id);
    printf("2. 姓名: %s\n", athletes[foundIndex].name);
    printf("3. 项目: %s\n", athletes[foundIndex].event);
    printf("4. 学院: %s\n", athletes[foundIndex].college);
    printf("5. 预赛成绩: %.2f\n", athletes[foundIndex].prelim);
    printf("6. 决赛成绩: %.2f\n", athletes[foundIndex].finalScore);
    printf("\n请输入新的信息（直接回车跳过修改）：\n");
    
    char input[50];
    printf("新姓名: "); fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;
    if (strlen(input) > 0) strcpy(athletes[foundIndex].name, input);
    
    printf("新项目: "); fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;
    if (strlen(input) > 0) strcpy(athletes[foundIndex].event, input);
    
    printf("新学院: "); fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;
    if (strlen(input) > 0) strcpy(athletes[foundIndex].college, input);
    
    printf("新预赛成绩: "); fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;
    if (strlen(input) > 0) athletes[foundIndex].prelim = atof(input);
    
    printf("新决赛成绩: "); fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;
    if (strlen(input) > 0) athletes[foundIndex].finalScore = atof(input);
    
    printf(">>> 修改成功！\n");
    sortAndCalculate(); 
}

void deleteRecord() {
    if (currentCount == 0) { printf("\n【提示】暂无数据。\n"); return; }
    char targetId[20];
    printf("\n--- 删除记录 ---\n请输入要删除的运动员编号: ");
    scanf("%s", targetId); getchar();
    
    int foundIndex = -1;
    for (int i = 0; i < currentCount; i++) {
        if (strcmp(athletes[i].id, targetId) == 0) { foundIndex = i; break; }
    }
    
    if (foundIndex == -1) { printf(">>> 未找到该编号的运动员。\n"); return; }
    
    printf("确认删除 %s (%s) 的记录吗？(Y/N): ", athletes[foundIndex].name, athletes[foundIndex].id);
    char confirm; scanf("%c", &confirm);
    
    if (confirm == 'Y' || confirm == 'y') {
        for (int i = foundIndex; i < currentCount - 1; i++) {
            athletes[i] = athletes[i + 1];
        }
        currentCount--;
        printf(">>> 删除成功！\n");
        sortAndCalculate(); 
    } else { printf(">>> 取消删除。\n"); }
}

void initTestData() {
    if (currentCount > 0) {
        printf(">>> 系统中已有数据，是否清空并重新加载测试数据？(Y/N): ");
        char confirm; scanf(" %c", &confirm); getchar();
        if (confirm != 'Y' && confirm != 'y') return;
    }
    currentCount = 0;
    Athlete data[] = {
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
    int size = sizeof(data) / sizeof(data[0]);
    for (int i = 0; i < size; i++) athletes[currentCount++] = data[i];
    sortAndCalculate();
    printf(">>> 测试数据已加载（共 %d 条记录）。\n", size);
}

// 查询功能实现
void searchByIdOrName() {
    if (currentCount == 0) { printf("\n【提示】暂无数据。\n"); return; }
    char keyword[50];
    printf("\n--- 按姓名/编号查询 ---\n请输入姓名或编号: ");
    scanf("%s", keyword); getchar();
    printf("\n----------------------------------------------------------------------------------------\n");
    print_aligned("编号", 10); print_aligned("姓名", 12); print_aligned("项目", 14);
    print_aligned("学院", 18); print_aligned("决赛成绩", 12); print_aligned("名次", 8);
    print_aligned("得分", 8); printf("破纪录\n");
    printf("----------------------------------------------------------------------------------------\n");
    int found = 0;
    for (int i = 0; i < currentCount; i++) {
        if (strstr(athletes[i].id, keyword) != NULL || 
            strstr(athletes[i].name, keyword) != NULL) {
            char display_id[30], display_name[30];
            sprintf(display_id, "%s", athletes[i].id);
            sprintf(display_name, "%s", athletes[i].name);
            print_highlight(display_id, keyword, 10);
            print_highlight(display_name, keyword, 12);
            print_aligned(athletes[i].event, 14);
            print_aligned(athletes[i].college, 18);
            char scoreStr[20]; sprintf(scoreStr, "%.2f", athletes[i].finalScore);
            print_aligned(scoreStr, 12);
            char rankStr[10]; sprintf(rankStr, "%d", athletes[i].rank);
            print_aligned(rankStr, 8);
            char scorePoint[10]; sprintf(scorePoint, "%d", athletes[i].score);
            print_aligned(scorePoint, 8);
            printf("%s\n", athletes[i].isBrokenRecord ? "★" : " ");
            found = 1;
        }
    }
    printf("----------------------------------------------------------------------------------------\n");
    if (!found) printf("\n【提示】未找到相关记录。\n");
}

void searchByEvent() {
    if (currentCount == 0) { printf("\n【提示】暂无数据。\n"); return; }
    char eventName[50];
    printf("\n--- 按项目查询 ---\n请输入项目名称: ");
    scanf("%s", eventName); getchar();
    printf("\n--- %s 项目成绩单 ---\n", eventName);
    printf("----------------------------------------------------------------------------------------\n");
    print_aligned("排名", 8); print_aligned("编号", 10); print_aligned("姓名", 12);
    print_aligned("学院", 18); print_aligned("预赛成绩", 12); print_aligned("决赛成绩", 12);
    print_aligned("得分", 8); printf("破纪录\n");
    printf("----------------------------------------------------------------------------------------\n");
    int found = 0;
    for (int i = 0; i < currentCount; i++) {
        if (strcmp(athletes[i].event, eventName) == 0) {
            char rankStr[10], prelimStr[20], finalStr[20], scoreStr[10];
            sprintf(rankStr, "%d", athletes[i].rank);
            sprintf(prelimStr, "%.2f", athletes[i].prelim);
            sprintf(finalStr, "%.2f", athletes[i].finalScore);
            sprintf(scoreStr, "%d", athletes[i].score);
            print_aligned(rankStr, 8);
            print_aligned(athletes[i].id, 10);
            print_aligned(athletes[i].name, 12);
            print_aligned(athletes[i].college, 18);
            print_aligned(prelimStr, 12);
            print_aligned(finalStr, 12);
            print_aligned(scoreStr, 8);
            printf("%s\n", athletes[i].isBrokenRecord ? "★" : " ");
            found = 1;
        }
    }
    printf("----------------------------------------------------------------------------------------\n");
    if (!found) printf("\n【提示】未找到该项目的记录。\n");
}

void searchByScoreRange() {
    if (currentCount == 0) { printf("\n【提示】暂无数据。\n"); return; }
    double minScore, maxScore;
    printf("\n--- 按成绩区间查询 ---\n请输入最低成绩: "); scanf("%lf", &minScore);
    printf("请输入最高成绩: "); scanf("%lf", &maxScore); getchar();
    printf("\n--- 成绩在 %.2f ~ %.2f 之间的运动员 ---\n", minScore, maxScore);
    printf("----------------------------------------------------------------------------------------\n");
    print_aligned("编号", 10); print_aligned("姓名", 12); print_aligned("项目", 14);
    print_aligned("学院", 18); print_aligned("决赛成绩", 12); printf("单位\n");
    printf("----------------------------------------------------------------------------------------\n");
    int found = 0;
    for (int i = 0; i < currentCount; i++) {
        if (athletes[i].finalScore >= minScore && athletes[i].finalScore <= maxScore) {
            print_aligned(athletes[i].id, 10);
            print_aligned(athletes[i].name, 12);
            print_aligned(athletes[i].event, 14);
            print_aligned(athletes[i].college, 18);
            char scoreStr[20]; sprintf(scoreStr, "%.2f", athletes[i].finalScore);
            print_aligned(scoreStr, 12);
            printf("%s\n", isRunningEvent(athletes[i].event) ? "秒" : "米");
            found = 1;
        }
    }
    printf("----------------------------------------------------------------------------------------\n");
    if (!found) printf("\n【提示】未找到该成绩区间的记录。\n");
}

void searchByCollege() {
    if (currentCount == 0) { printf("\n【提示】暂无数据。\n"); return; }
    char collegeName[50];
    printf("\n--- 按学院查询 ---\n请输入学院名称: "); scanf("%s", collegeName); getchar();
    int found = 0; int totalScore = 0;
    printf("\n--- %s 学院参赛情况 ---\n", collegeName);
    printf("----------------------------------------------------------------------------------------\n");
    print_aligned("编号", 10); print_aligned("姓名", 12); print_aligned("项目", 14);
    print_aligned("决赛成绩", 12); print_aligned("名次", 8); print_aligned("得分", 8);
    printf("\n----------------------------------------------------------------------------------------\n");
    for (int i = 0; i < currentCount; i++) {
        if (strcmp(athletes[i].college, collegeName) == 0) {
            char scoreStr[20], rankStr[10], pointStr[10];
            sprintf(scoreStr, "%.2f", athletes[i].finalScore);
            sprintf(rankStr, "%d", athletes[i].rank);
            sprintf(pointStr, "%d", athletes[i].score);
            print_aligned(athletes[i].id, 10);
            print_aligned(athletes[i].name, 12);
            print_aligned(athletes[i].event, 14);
            print_aligned(scoreStr, 12);
            print_aligned(rankStr, 8);
            print_aligned(pointStr, 8);
            printf("\n");
            totalScore += athletes[i].score;
            found = 1;
        }
    }
    printf("----------------------------------------------------------------------------------------\n");
    if (found) printf("学院总分: %d 分\n", totalScore);
    else printf("\n【提示】未找到该学院的记录。\n");
}

// 统计分析
void showCollegeRanking() {
    calculateCollegeStats();
    printf("\n--- 学院总分排名榜 ---\n");
    printf("================================================================\n");
    print_aligned("排名", 8); print_aligned("学院名称", 20);
    print_aligned("总分", 10); print_aligned("金牌", 8);
    print_aligned("银牌", 8); print_aligned("铜牌", 8); printf("\n");
    printf("================================================================\n");
    for (int i = 0; i < collegeCount; i++) {
        char rankStr[10], scoreStr[10], goldStr[10], silverStr[10], bronzeStr[10];
        sprintf(rankStr, "%d", i + 1);
        sprintf(scoreStr, "%d", collegeStats[i].totalScore);
        sprintf(goldStr, "%d", collegeStats[i].goldCount);
        sprintf(silverStr, "%d", collegeStats[i].silverCount);
        sprintf(bronzeStr, "%d", collegeStats[i].bronzeCount);
        print_aligned(rankStr, 8); print_aligned(collegeStats[i].name, 20);
        print_aligned(scoreStr, 10); print_aligned(goldStr, 8);
        print_aligned(silverStr, 8); print_aligned(bronzeStr, 8); printf("\n");
    }
    printf("================================================================\n");
}

void showRecordBreakers() {
    int count = 0;
    printf("\n--- 破纪录统计 ---\n");
    printf("================================================================\n");
    print_aligned("项目", 16); print_aligned("运动员", 12);
    print_aligned("学院", 18); print_aligned("成绩", 12); print_aligned("纪录类型", 12); printf("\n");
    printf("================================================================\n");
    for (int i = 0; i < currentCount; i++) {
        if (athletes[i].isBrokenRecord) {
            char recordType[20] = "未知"; double oldRecord = 0;
            for (int j = 0; j < recordCount; j++) {
                if (strcmp(athletes[i].event, gameRecords[j].event) == 0) {
                    strcpy(recordType, gameRecords[j].holder);
                    oldRecord = gameRecords[j].record;
                    break;
                }
            }
            print_aligned(athletes[i].event, 16);
            print_aligned(athletes[i].name, 12);
            print_aligned(athletes[i].college, 18);
            char scoreStr[20]; sprintf(scoreStr, "%.2f", athletes[i].finalScore);
            print_aligned(scoreStr, 12);
            char recordInfo[30]; sprintf(recordInfo, "%s(原%.2f)", recordType, oldRecord);
            print_aligned(recordInfo, 12); printf("\n");
            count++;
        }
    }
    printf("================================================================\n");
    printf("总计破纪录: %d 人次\n", count);
}

void showScoreDistribution() {
    int rankCount[6] = {0};
    int scoreSegments[6] = {0}; 
    
    for (int i = 0; i < currentCount; i++) {
        if (athletes[i].rank >= 1 && athletes[i].rank <= 5) rankCount[athletes[i].rank]++;
        else rankCount[0]++;
        int score = athletes[i].score;
        if (score <= 5) scoreSegments[0]++;
        else if (score <= 10) scoreSegments[1]++;
        else if (score <= 15) scoreSegments[2]++;
        else if (score <= 20) scoreSegments[3]++;
        else if (score <= 25) scoreSegments[4]++;
        else scoreSegments[5]++;
    }
    printf("\n--- 成绩分布分析 ---\n1. 名次分布：\n");
    printf("  第1名: %d 人\n  第2名: %d 人\n  第3名: %d 人\n  第4名: %d 人\n  第5名: %d 人\n  无排名: %d 人\n",
           rankCount[1], rankCount[2], rankCount[3], rankCount[4], rankCount[5], rankCount[0]);
    printf("\n2. 得分区间分布：\n");
    printf("  0-5分: %d 人\n  6-10分: %d 人\n  11-15分: %d 人\n  16-20分: %d 人\n  21-25分: %d 人\n  26分以上: %d 人\n",
           scoreSegments[0], scoreSegments[1], scoreSegments[2], scoreSegments[3], scoreSegments[4], scoreSegments[5]);
    printf("\n3. 统计摘要：\n  总参赛人数: %d 人\n  有得分人数: %d 人\n  获奖率: %.1f%%\n", 
           currentCount, currentCount - rankCount[0], (currentCount - rankCount[0]) * 100.0 / currentCount);
}

void groupCompetition() {
    printf("\n--- 项目分组 ---\n此功能用于将参赛者分配到预赛/决赛组\n");
    typedef struct {
        char event[20];
        int count;
        Athlete *participants[MAX_ATHLETES];
    } EventGroup;
    EventGroup groups[50];
    int groupCount = 0;
    
    for (int i = 0; i < currentCount; i++) {
        int found = 0;
        for (int j = 0; j < groupCount; j++) {
            if (strcmp(athletes[i].event, groups[j].event) == 0) {
                groups[j].participants[groups[j].count++] = &athletes[i];
                found = 1; break;
            }
        }
        if (!found) {
            strcpy(groups[groupCount].event, athletes[i].event);
            groups[groupCount].count = 1;
            groups[groupCount].participants[0] = &athletes[i];
            groupCount++;
        }
    }
    
    for (int i = 0; i < groupCount; i++) {
        printf("\n项目：%s\n参赛人数：%d 人\n", groups[i].event, groups[i].count);
        if (groups[i].count <= 8) printf("建议：直接决赛\n");
        else if (groups[i].count <= 16) printf("建议：分2组预赛，每组前4名进决赛\n");
        else if (groups[i].count <= 24) printf("建议：分3组预赛，每组前2名+2个最好成绩进决赛\n");
        else printf("建议：分4组预赛，每组前3名进决赛\n");
        
        for (int j = 0; j < groups[i].count - 1; j++) {
            for (int k = 0; k < groups[i].count - 1 - j; k++) {
                int isRun = isRunningEvent(groups[i].event);
                if ((isRun && groups[i].participants[k]->prelim > groups[i].participants[k+1]->prelim) ||
                    (!isRun && groups[i].participants[k]->prelim < groups[i].participants[k+1]->prelim)) {
                    Athlete *temp = groups[i].participants[k];
                    groups[i].participants[k] = groups[i].participants[k+1];
                    groups[i].participants[k+1] = temp;
                }
            }
        }
        printf("预赛成绩排序（前8名）：\n");
        for (int j = 0; j < (groups[i].count < 8 ? groups[i].count : 8); j++) {
            printf("  %d. %s: %.2f\n", j + 1, groups[i].participants[j]->name, groups[i].participants[j]->prelim);
        }
        printf("\n");
    }
}

void validateScores() {
    printf("\n--- 成绩验证 ---\n");
    int errorCount = 0; int warningCount = 0;
    for (int i = 0; i < currentCount; i++) {
        int isRun = isRunningEvent(athletes[i].event);
        if (athletes[i].finalScore <= 0) {
            printf("? 错误：%s 的决赛成绩异常（%.2f）\n", athletes[i].name, athletes[i].finalScore);
            errorCount++;
        }
        if (isRun && athletes[i].finalScore > 120) {
            printf("? 警告：%s 的 %s 成绩可能过慢（%.2f秒）\n", athletes[i].name, athletes[i].event, athletes[i].finalScore);
            warningCount++;
        }
        if (!isRun && athletes[i].finalScore > 20) {
            printf("? 警告：%s 的 %s 成绩可能过大（%.2f米）\n", athletes[i].name, athletes[i].event, athletes[i].finalScore);
            warningCount++;
        }
        if (isRun) {
            if (athletes[i].finalScore < athletes[i].prelim * 0.8) {
                printf("? 警告：%s 的决赛成绩比预赛快太多（预赛%.2f，决赛%.2f）\n", athletes[i].name, athletes[i].prelim, athletes[i].finalScore); warningCount++;
            }
        } else {
            if (athletes[i].finalScore > athletes[i].prelim * 1.2) {
                printf("? 警告：%s 的决赛成绩比预赛好太多（预赛%.2f，决赛%.2f）\n", athletes[i].name, athletes[i].prelim, athletes[i].finalScore); warningCount++;
            }
        }
    }
    printf("\n验证结果：\n  发现错误：%d 处\n  发现警告：%d 处\n", errorCount, warningCount);
    if (errorCount == 0 && warningCount == 0) printf("  ? 所有成绩数据正常\n");
}

void displayAll(int page) {
    if (currentCount == 0) { printf("\n【提示】暂无数据。\n"); return; }
    sortAndCalculate();
    int start = (page - 1) * PAGE_SIZE;
    int end = start + PAGE_SIZE;
    if (end > currentCount) end = currentCount;
    if (start >= currentCount) { printf("\n【提示】没有更多数据了。\n"); return; }
    
    printf("\n----------------------------------------------------------------------------------------\n");
    print_aligned("编号", 10); print_aligned("姓名", 12); print_aligned("项目", 14);
    print_aligned("学院", 18); print_aligned("决赛成绩", 12); print_aligned("名次", 8);
    print_aligned("得分", 8); printf("纪录\n");
    printf("----------------------------------------------------------------------------------------\n");
    for (int i = start; i < end; i++) {
        char scoreStr[20], rankStr[10], pointStr[10];
        sprintf(scoreStr, "%.2f", athletes[i].finalScore);
        sprintf(rankStr, "%d", athletes[i].rank);
        sprintf(pointStr, "%d", athletes[i].score);
        print_aligned(athletes[i].id, 10);
        print_aligned(athletes[i].name, 12);
        print_aligned(athletes[i].event, 14);
        print_aligned(athletes[i].college, 18);
        print_aligned(scoreStr, 12);
        print_aligned(rankStr, 8);
        print_aligned(pointStr, 8);
        printf("%s\n", athletes[i].isBrokenRecord ? "★" : " ");
        if (i < currentCount - 1 && i < end - 1 && strcmp(athletes[i].event, athletes[i+1].event) != 0) {
            printf("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
        }
    }
    printf("----------------------------------------------------------------------------------------\n");
    printf("页码: %d/%d (共%d条记录)\n", page, (currentCount + PAGE_SIZE - 1) / PAGE_SIZE, currentCount);
}

void printScoreReport() {
    if (currentCount == 0) { printf("\n【提示】暂无数据。\n"); return; }
    sortAndCalculate();
    printf("\n=========== 运动会成绩总表 ===========\n");
    time_t currentTime = time(NULL);
    printf("打印时间: %s", ctime(&currentTime));
    printf("=====================================\n");
    int page = 1; int start = 0;
    while (start < currentCount) {
        printf("\n--- 第 %d 页 ---\n", page);
        printf("----------------------------------------------------------------------------------------\n");
        print_aligned("编号", 10); print_aligned("姓名", 12); print_aligned("项目", 14);
        print_aligned("学院", 18); print_aligned("决赛成绩", 12); print_aligned("名次", 8);
        print_aligned("得分", 8); printf("\n");
        printf("----------------------------------------------------------------------------------------\n");
        int end = (start + PAGE_SIZE < currentCount) ? start + PAGE_SIZE : currentCount;
        for (int i = start; i < end; i++) {
            char scoreStr[20], rankStr[10], pointStr[10];
            sprintf(scoreStr, "%.2f", athletes[i].finalScore);
            sprintf(rankStr, "%d", athletes[i].rank);
            sprintf(pointStr, "%d", athletes[i].score);
            print_aligned(athletes[i].id, 10); print_aligned(athletes[i].name, 12);
            print_aligned(athletes[i].event, 14); print_aligned(athletes[i].college, 18);
            print_aligned(scoreStr, 12); print_aligned(rankStr, 8); print_aligned(pointStr, 8);
            printf("\n");
        }
        printf("----------------------------------------------------------------------------------------\n");
        printf("页码: %d/%d\n", page, (currentCount + PAGE_SIZE - 1) / PAGE_SIZE);
        start = end; page++;
        if (start < currentCount) { printf("\n按回车键继续下一页..."); getchar(); }
    }
}