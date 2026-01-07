// 公共头文件
// 这里存放所有的结构体定义和宏定义，供所有文件引用。

#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

/* 
 * 【系统配置宏定义】
 */
#define zbh_MAX_ATHLETES 1000   // 最大运动员数量
#define zbh_FILENAME "sports_db.txt"  // 数据文件名
#define zbh_BACKUP_PREFIX "backup_"   // 备份文件前缀
#define zbh_MAX_COLLEGES 50     // 最大支持的学院数量
#define zbh_PAGE_SIZE 10        // 分页显示每页条数
#define zbh_MAX_RECORDS 100     // 存储的比赛纪录的最大数量

// 定义运动员结构体
typedef struct {
    char zbh_id[20];        // 编号
    char zbh_name[20];      // 姓名
    char zbh_event[20];     // 项目
    char zbh_college[30];   // 学院
    double zbh_prelim;      // 预赛成绩
    double zbh_finalScore;  // 决赛成绩
    int zbh_rank;           // 名次
    int zbh_score;          // 得分
    int zbh_isBrokenRecord; // 是否破纪录
} zbh_Athlete;

// 学院统计结构体
typedef struct {
    char zbh_name[30];      // 学院名称
    int zbh_totalScore;     // 总分
    int zbh_goldCount;      // 金牌数
    int zbh_silverCount;    // 银牌数
    int zbh_bronzeCount;    // 铜牌数
} zbh_CollegeStat;

// 比赛纪录结构体
typedef struct {
    char zbh_event[20];     // 项目名称
    double zbh_record;      // 当前纪录值
    char zbh_type;          // 'R'径赛/'F'田赛
    char zbh_holder[20];    // 纪录保持者描述
} zbh_GameRecord;

#endif