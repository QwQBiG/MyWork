// 文件操作实现

#include "file_ops.h"
#include "globals.h"
#include "utils.h"
#include "logic.h" // 需要用到排序函数

void zbh_initGameRecords() {
    zbh_recordCount = 5;
    strcpy(zbh_gameRecords[0].zbh_event, "100米跑");
    zbh_gameRecords[0].zbh_record = 10.5;
    zbh_gameRecords[0].zbh_type = 'R';
    strcpy(zbh_gameRecords[0].zbh_holder, "校纪录");
    
    strcpy(zbh_gameRecords[1].zbh_event, "跳远");
    zbh_gameRecords[1].zbh_record = 6.8;
    zbh_gameRecords[1].zbh_type = 'F';
    strcpy(zbh_gameRecords[1].zbh_holder, "赛会纪录");
    
    strcpy(zbh_gameRecords[2].zbh_event, "铅球");
    zbh_gameRecords[2].zbh_record = 12.5;
    zbh_gameRecords[2].zbh_type = 'F';
    strcpy(zbh_gameRecords[2].zbh_holder, "校纪录");
    
    strcpy(zbh_gameRecords[3].zbh_event, "400米跑");
    zbh_gameRecords[3].zbh_record = 50.0;
    zbh_gameRecords[3].zbh_type = 'R';
    strcpy(zbh_gameRecords[3].zbh_holder, "赛会纪录");
    
    strcpy(zbh_gameRecords[4].zbh_event, "跳高");
    zbh_gameRecords[4].zbh_record = 1.9;
    zbh_gameRecords[4].zbh_type = 'F';
    strcpy(zbh_gameRecords[4].zbh_holder, "校纪录");
}

void zbh_loadFromFile() {
    FILE *zbh_fp = fopen(zbh_FILENAME, "r");
    if (!zbh_fp) {
        printf(">>> 未找到数据文件，将创建新文件。\n");
        return;
    }
    
    fscanf(zbh_fp, "%d\n", &zbh_currentCount);
    for (int zbh_i = 0; zbh_i < zbh_currentCount; zbh_i++) {
        fscanf(zbh_fp, "%s %s %s %s %lf %lf %d %d %d\n", 
               zbh_athletes[zbh_i].zbh_id,
               zbh_athletes[zbh_i].zbh_name, 
               zbh_athletes[zbh_i].zbh_event, 
               zbh_athletes[zbh_i].zbh_college, 
               &zbh_athletes[zbh_i].zbh_prelim,
               &zbh_athletes[zbh_i].zbh_finalScore,
               &zbh_athletes[zbh_i].zbh_rank,
               &zbh_athletes[zbh_i].zbh_score,
               &zbh_athletes[zbh_i].zbh_isBrokenRecord);
    }
    fclose(zbh_fp);
    printf(">>> 已加载 %d 条记录。\n", zbh_currentCount);
}

void zbh_saveToFile() {
    FILE *zbh_fp = fopen(zbh_FILENAME, "w");
    if (!zbh_fp) {
        printf(">>> 保存失败！\n");
        return;
    }
    
    fprintf(zbh_fp, "%d\n", zbh_currentCount);
    for (int zbh_i = 0; zbh_i < zbh_currentCount; zbh_i++) {
        fprintf(zbh_fp, "%s %s %s %s %.2f %.2f %d %d %d\n", 
                zbh_athletes[zbh_i].zbh_id, 
                zbh_athletes[zbh_i].zbh_name, 
                zbh_athletes[zbh_i].zbh_event, 
                zbh_athletes[zbh_i].zbh_college, 
                zbh_athletes[zbh_i].zbh_prelim, 
                zbh_athletes[zbh_i].zbh_finalScore,
                zbh_athletes[zbh_i].zbh_rank,
                zbh_athletes[zbh_i].zbh_score,
                zbh_athletes[zbh_i].zbh_isBrokenRecord);
    }
    fclose(zbh_fp);
    printf(">>> 已保存 %d 条记录到文件。\n", zbh_currentCount);
}

void zbh_backupData() {
    time_t zbh_t = time(NULL);
    struct tm *zbh_tm = localtime(&zbh_t);
    char zbh_backupFile[100];
    sprintf(zbh_backupFile, "%s%04d%02d%02d_%02d%02d%02d.txt", 
            zbh_BACKUP_PREFIX,
            zbh_tm->tm_year + 1900, zbh_tm->tm_mon + 1, zbh_tm->tm_mday,
            zbh_tm->tm_hour, zbh_tm->tm_min, zbh_tm->tm_sec);
    
    FILE *zbh_src = fopen(zbh_FILENAME, "r");
    FILE *zbh_dst = fopen(zbh_backupFile, "w");
    
    if (!zbh_src || !zbh_dst) {
        printf(">>> 备份失败！\n");
        return;
    }
    
    char zbh_ch;
    while ((zbh_ch = fgetc(zbh_src)) != EOF) {
        fputc(zbh_ch, zbh_dst);
    }
    
    fclose(zbh_src);
    fclose(zbh_dst);
    printf(">>> 数据已备份到: %s\n", zbh_backupFile);
}

void zbh_batchImport() {
    printf("\n--- 批量导入 ---\n");
    printf("请将CSV文件放在程序目录下，命名为 import.csv\n");
    printf("格式：编号,姓名,项目,学院,预赛成绩,决赛成绩\n");
    printf("是否继续？(Y/N): ");
    
    char zbh_confirm;
    scanf(" %c", &zbh_confirm);
    getchar();
    
    if (zbh_confirm != 'Y' && zbh_confirm != 'y') return;
    
    FILE *zbh_fp = fopen("import.csv", "r");
    if (!zbh_fp) {
        printf(">>> 未找到 import.csv 文件！\n");
        return;
    }
    
    int zbh_importCount = 0;
    char zbh_line[200];
    fgets(zbh_line, sizeof(zbh_line), zbh_fp); // 跳过标题
    
    while (fgets(zbh_line, sizeof(zbh_line), zbh_fp)) {
        if (zbh_currentCount >= zbh_MAX_ATHLETES) {
            printf(">>> 达到最大容量，停止导入。\n");
            break;
        }
        
        zbh_line[strcspn(zbh_line, "\n")] = 0;
        char *zbh_token = strtok(zbh_line, ",");
        if (!zbh_token) continue;
        strcpy(zbh_athletes[zbh_currentCount].zbh_id, zbh_token);
        
        zbh_token = strtok(NULL, ","); if (!zbh_token) continue;
        strcpy(zbh_athletes[zbh_currentCount].zbh_name, zbh_token);
        
        zbh_token = strtok(NULL, ","); if (!zbh_token) continue;
        strcpy(zbh_athletes[zbh_currentCount].zbh_event, zbh_token);
        
        zbh_token = strtok(NULL, ","); if (!zbh_token) continue;
        strcpy(zbh_athletes[zbh_currentCount].zbh_college, zbh_token);
        
        zbh_token = strtok(NULL, ","); if (!zbh_token) continue;
        zbh_athletes[zbh_currentCount].zbh_prelim = atof(zbh_token);
        
        zbh_token = strtok(NULL, ","); if (!zbh_token) continue;
        zbh_athletes[zbh_currentCount].zbh_finalScore = atof(zbh_token);
        
        zbh_athletes[zbh_currentCount].zbh_rank = 0;
        zbh_athletes[zbh_currentCount].zbh_score = 0;
        zbh_athletes[zbh_currentCount].zbh_isBrokenRecord = 0;
        
        zbh_currentCount++;
        zbh_importCount++;
    }
    
    fclose(zbh_fp);
    zbh_sortAndCalculate();
    printf(">>> 成功导入 %d 条记录。\n", zbh_importCount);
}

void zbh_exportToCSV() {
    if (zbh_currentCount == 0) {
        printf("\n【提示】暂无数据。\n");
        return;
    }
    
    zbh_sortAndCalculate();
    FILE *zbh_fp = fopen("export.csv", "w");
    if (!zbh_fp) {
        printf(">>> 导出失败！\n");
        return;
    }
    
    fprintf(zbh_fp, "编号,姓名,项目,学院,预赛成绩,决赛成绩,名次,得分,是否破纪录\n");
    for (int zbh_i = 0; zbh_i < zbh_currentCount; zbh_i++) {
        fprintf(zbh_fp, "%s,%s,%s,%s,%.2f,%.2f,%d,%d,%s\n",
                zbh_athletes[zbh_i].zbh_id,
                zbh_athletes[zbh_i].zbh_name,
                zbh_athletes[zbh_i].zbh_event,
                zbh_athletes[zbh_i].zbh_college,
                zbh_athletes[zbh_i].zbh_prelim,
                zbh_athletes[zbh_i].zbh_finalScore,
                zbh_athletes[zbh_i].zbh_rank,
                zbh_athletes[zbh_i].zbh_score,
                zbh_athletes[zbh_i].zbh_isBrokenRecord ? "是" : "否");
    }
    fclose(zbh_fp);
    printf(">>> 已导出 %d 条记录到 export.csv\n", zbh_currentCount);
}

void zbh_generateCertificate() {
    if (zbh_currentCount == 0) {
        printf("\n【提示】暂无数据。\n");
        return;
    }
    
    char zbh_targetId[20];
    printf("\n--- 生成获奖证书 ---\n");
    printf("请输入运动员编号: ");
    scanf("%s", zbh_targetId);
    getchar();
    
    int zbh_foundIndex = -1;
    for (int zbh_i = 0; zbh_i < zbh_currentCount; zbh_i++) {
        if (strcmp(zbh_athletes[zbh_i].zbh_id, zbh_targetId) == 0) {
            zbh_foundIndex = zbh_i;
            break;
        }
    }
    
    if (zbh_foundIndex == -1) {
        printf(">>> 未找到该运动员。\n");
        return;
    }
    
    if (zbh_athletes[zbh_foundIndex].zbh_rank > 5) {
        printf(">>> 该运动员未获得前五名，无法生成证书。\n");
        return;
    }
    
    FILE *zbh_fp = fopen("certificate.txt", "w");
    if (!zbh_fp) {
        printf(">>> 生成证书失败！\n");
        return;
    }
    
    fprintf(zbh_fp, "\n=============================================\n");
    fprintf(zbh_fp, "           运动会获奖证书\n");
    fprintf(zbh_fp, "=============================================\n\n");
    fprintf(zbh_fp, "授予：%s 同学\n", zbh_athletes[zbh_foundIndex].zbh_name);
    fprintf(zbh_fp, "学院：%s\n\n", zbh_athletes[zbh_foundIndex].zbh_college);
    fprintf(zbh_fp, "在 %s 项目中\n", zbh_athletes[zbh_foundIndex].zbh_event);
    
    char zbh_medal[10], zbh_chineseRank[10];
    switch (zbh_athletes[zbh_foundIndex].zbh_rank) {
        case 1: strcpy(zbh_medal, "金牌"); strcpy(zbh_chineseRank, "第一名"); break;
        case 2: strcpy(zbh_medal, "银牌"); strcpy(zbh_chineseRank, "第二名"); break;
        case 3: strcpy(zbh_medal, "铜牌"); strcpy(zbh_chineseRank, "第三名"); break;
        case 4: strcpy(zbh_medal, "证书"); strcpy(zbh_chineseRank, "第四名"); break;
        case 5: strcpy(zbh_medal, "证书"); strcpy(zbh_chineseRank, "第五名"); break;
        default: strcpy(zbh_medal, "证书"); strcpy(zbh_chineseRank, "优秀奖");
    }
    
    fprintf(zbh_fp, "荣获 %s，成绩：%.2f %s\n", 
            zbh_chineseRank,
            zbh_athletes[zbh_foundIndex].zbh_finalScore,
            zbh_isRunningEvent(zbh_athletes[zbh_foundIndex].zbh_event) ? "秒" : "米");
    
    if (zbh_athletes[zbh_foundIndex].zbh_isBrokenRecord) {
        fprintf(zbh_fp, "★ 破纪录成绩 ★\n");
    }
    
    fprintf(zbh_fp, "\n得分：%d 分\n\n", zbh_athletes[zbh_foundIndex].zbh_score);
    fprintf(zbh_fp, "特发此证，以资鼓励！\n\n运动会组委会\n");
    time_t zbh_currentTime = time(NULL);
    fprintf(zbh_fp, "%s", ctime(&zbh_currentTime));
    fprintf(zbh_fp, "=============================================\n");
    
    fclose(zbh_fp);
    printf(">>> 证书已生成到 certificate.txt\n");
}