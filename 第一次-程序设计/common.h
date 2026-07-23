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
#define MAX_ATHLETES 1000   // 最大运动员数量
#define FILENAME "sports_db.txt"  // 数据文件名
#define BACKUP_PREFIX "backup_"   // 备份文件前缀
#define MAX_COLLEGES 50     // 最大支持的学院数量
#define PAGE_SIZE 10        // 分页显示每页条数
#define MAX_RECORDS 100     // 存储的比赛纪录的最大数量

// 定义运动员结构体
typedef struct {
    char id[20];        // 编号
    char name[20];      // 姓名
    char event[20];     // 项目
    char college[30];   // 学院
    double prelim;      // 预赛成绩
    double finalScore;  // 决赛成绩
    int rank;           // 名次
    int score;          // 得分
    int isBrokenRecord; // 是否破纪录
} Athlete;

// 学院统计结构体
typedef struct {
    char name[30];      // 学院名称
    int totalScore;     // 总分
    int goldCount;      // 金牌数
    int silverCount;    // 银牌数
    int bronzeCount;    // 铜牌数
} CollegeStat;

// 比赛纪录结构体
typedef struct {
    char event[20];     // 项目名称
    double record;      // 当前纪录值
    char type;          // 'R'径赛/'F'田赛
    char holder[20];    // 纪录保持者描述
} GameRecord;

#endif