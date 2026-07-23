// 文件操作实现

#include "file_ops.h"
#include "globals.h"
#include "utils.h"
#include "logic.h" // 需要用到排序函数

void initGameRecords() {
    recordCount = 5;
    strcpy(gameRecords[0].event, "100米跑");
    gameRecords[0].record = 10.5;
    gameRecords[0].type = 'R';
    strcpy(gameRecords[0].holder, "校纪录");
    
    strcpy(gameRecords[1].event, "跳远");
    gameRecords[1].record = 6.8;
    gameRecords[1].type = 'F';
    strcpy(gameRecords[1].holder, "赛会纪录");
    
    strcpy(gameRecords[2].event, "铅球");
    gameRecords[2].record = 12.5;
    gameRecords[2].type = 'F';
    strcpy(gameRecords[2].holder, "校纪录");
    
    strcpy(gameRecords[3].event, "400米跑");
    gameRecords[3].record = 50.0;
    gameRecords[3].type = 'R';
    strcpy(gameRecords[3].holder, "赛会纪录");
    
    strcpy(gameRecords[4].event, "跳高");
    gameRecords[4].record = 1.9;
    gameRecords[4].type = 'F';
    strcpy(gameRecords[4].holder, "校纪录");
}

void loadFromFile() {
    FILE *fp = fopen(FILENAME, "r");
    if (!fp) {
        printf(">>> 未找到数据文件，将创建新文件。\n");
        return;
    }
    
    fscanf(fp, "%d\n", &currentCount);
    for (int i = 0; i < currentCount; i++) {
        fscanf(fp, "%s %s %s %s %lf %lf %d %d %d\n", 
               athletes[i].id,
               athletes[i].name, 
               athletes[i].event, 
               athletes[i].college, 
               &athletes[i].prelim,
               &athletes[i].finalScore,
               &athletes[i].rank,
               &athletes[i].score,
               &athletes[i].isBrokenRecord);
    }
    fclose(fp);
    printf(">>> 已加载 %d 条记录。\n", currentCount);
}

void saveToFile() {
    FILE *fp = fopen(FILENAME, "w");
    if (!fp) {
        printf(">>> 保存失败！\n");
        return;
    }
    
    fprintf(fp, "%d\n", currentCount);
    for (int i = 0; i < currentCount; i++) {
        fprintf(fp, "%s %s %s %s %.2f %.2f %d %d %d\n", 
                athletes[i].id, 
                athletes[i].name, 
                athletes[i].event, 
                athletes[i].college, 
                athletes[i].prelim, 
                athletes[i].finalScore,
                athletes[i].rank,
                athletes[i].score,
                athletes[i].isBrokenRecord);
    }
    fclose(fp);
    printf(">>> 已保存 %d 条记录到文件。\n", currentCount);
}

void backupData() {
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    char backupFile[100];
    sprintf(backupFile, "%s%04d%02d%02d_%02d%02d%02d.txt", 
            BACKUP_PREFIX,
            tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
            tm->tm_hour, tm->tm_min, tm->tm_sec);
    
    FILE *src = fopen(FILENAME, "r");
    FILE *dst = fopen(backupFile, "w");
    
    if (!src || !dst) {
        printf(">>> 备份失败！\n");
        return;
    }
    
    char ch;
    while ((ch = fgetc(src)) != EOF) {
        fputc(ch, dst);
    }
    
    fclose(src);
    fclose(dst);
    printf(">>> 数据已备份到: %s\n", backupFile);
}

void batchImport() {
    printf("\n--- 批量导入 ---\n");
    printf("请将CSV文件放在程序目录下，命名为 import.csv\n");
    printf("格式：编号,姓名,项目,学院,预赛成绩,决赛成绩\n");
    printf("是否继续？(Y/N): ");
    
    char confirm;
    scanf(" %c", &confirm);
    getchar();
    
    if (confirm != 'Y' && confirm != 'y') return;
    
    FILE *fp = fopen("import.csv", "r");
    if (!fp) {
        printf(">>> 未找到 import.csv 文件！\n");
        return;
    }
    
    int importCount = 0;
    char line[200];
    fgets(line, sizeof(line), fp); // 跳过标题
    
    while (fgets(line, sizeof(line), fp)) {
        if (currentCount >= MAX_ATHLETES) {
            printf(">>> 达到最大容量，停止导入。\n");
            break;
        }
        
        line[strcspn(line, "\n")] = 0;
        char *token = strtok(line, ",");
        if (!token) continue;
        strcpy(athletes[currentCount].id, token);
        
        token = strtok(NULL, ","); if (!token) continue;
        strcpy(athletes[currentCount].name, token);
        
        token = strtok(NULL, ","); if (!token) continue;
        strcpy(athletes[currentCount].event, token);
        
        token = strtok(NULL, ","); if (!token) continue;
        strcpy(athletes[currentCount].college, token);
        
        token = strtok(NULL, ","); if (!token) continue;
        athletes[currentCount].prelim = atof(token);
        
        token = strtok(NULL, ","); if (!token) continue;
        athletes[currentCount].finalScore = atof(token);
        
        athletes[currentCount].rank = 0;
        athletes[currentCount].score = 0;
        athletes[currentCount].isBrokenRecord = 0;
        
        currentCount++;
        importCount++;
    }
    
    fclose(fp);
    sortAndCalculate();
    printf(">>> 成功导入 %d 条记录。\n", importCount);
}

void exportToCSV() {
    if (currentCount == 0) {
        printf("\n【提示】暂无数据。\n");
        return;
    }
    
    sortAndCalculate();
    FILE *fp = fopen("export.csv", "w");
    if (!fp) {
        printf(">>> 导出失败！\n");
        return;
    }
    
    fprintf(fp, "编号,姓名,项目,学院,预赛成绩,决赛成绩,名次,得分,是否破纪录\n");
    for (int i = 0; i < currentCount; i++) {
        fprintf(fp, "%s,%s,%s,%s,%.2f,%.2f,%d,%d,%s\n",
                athletes[i].id,
                athletes[i].name,
                athletes[i].event,
                athletes[i].college,
                athletes[i].prelim,
                athletes[i].finalScore,
                athletes[i].rank,
                athletes[i].score,
                athletes[i].isBrokenRecord ? "是" : "否");
    }
    fclose(fp);
    printf(">>> 已导出 %d 条记录到 export.csv\n", currentCount);
}

void generateCertificate() {
    if (currentCount == 0) {
        printf("\n【提示】暂无数据。\n");
        return;
    }
    
    char targetId[20];
    printf("\n--- 生成获奖证书 ---\n");
    printf("请输入运动员编号: ");
    scanf("%s", targetId);
    getchar();
    
    int foundIndex = -1;
    for (int i = 0; i < currentCount; i++) {
        if (strcmp(athletes[i].id, targetId) == 0) {
            foundIndex = i;
            break;
        }
    }
    
    if (foundIndex == -1) {
        printf(">>> 未找到该运动员。\n");
        return;
    }
    
    if (athletes[foundIndex].rank > 5) {
        printf(">>> 该运动员未获得前五名，无法生成证书。\n");
        return;
    }
    
    FILE *fp = fopen("certificate.txt", "w");
    if (!fp) {
        printf(">>> 生成证书失败！\n");
        return;
    }
    
    fprintf(fp, "\n=============================================\n");
    fprintf(fp, "           运动会获奖证书\n");
    fprintf(fp, "=============================================\n\n");
    fprintf(fp, "授予：%s 同学\n", athletes[foundIndex].name);
    fprintf(fp, "学院：%s\n\n", athletes[foundIndex].college);
    fprintf(fp, "在 %s 项目中\n", athletes[foundIndex].event);
    
    char medal[10], chineseRank[10];
    switch (athletes[foundIndex].rank) {
        case 1: strcpy(medal, "金牌"); strcpy(chineseRank, "第一名"); break;
        case 2: strcpy(medal, "银牌"); strcpy(chineseRank, "第二名"); break;
        case 3: strcpy(medal, "铜牌"); strcpy(chineseRank, "第三名"); break;
        case 4: strcpy(medal, "证书"); strcpy(chineseRank, "第四名"); break;
        case 5: strcpy(medal, "证书"); strcpy(chineseRank, "第五名"); break;
        default: strcpy(medal, "证书"); strcpy(chineseRank, "优秀奖");
    }
    
    fprintf(fp, "荣获 %s，成绩：%.2f %s\n", 
            chineseRank,
            athletes[foundIndex].finalScore,
            isRunningEvent(athletes[foundIndex].event) ? "秒" : "米");
    
    if (athletes[foundIndex].isBrokenRecord) {
        fprintf(fp, "★ 破纪录成绩 ★\n");
    }
    
    fprintf(fp, "\n得分：%d 分\n\n", athletes[foundIndex].score);
    fprintf(fp, "特发此证，以资鼓励！\n\n运动会组委会\n");
    time_t currentTime = time(NULL);
    fprintf(fp, "%s", ctime(&currentTime));
    fprintf(fp, "=============================================\n");
    
    fclose(fp);
    printf(">>> 证书已生成到 certificate.txt\n");
}