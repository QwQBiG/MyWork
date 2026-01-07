// 全局变量定义
// 这里是全局变量真正分配内存的地方。

#ifndef GLOBALS_H
#define GLOBALS_H

#include "common.h"

// 使用 extern 声明，告诉编译器这些变量在别的地方定义了
extern zbh_Athlete zbh_athletes[zbh_MAX_ATHLETES];
extern zbh_CollegeStat zbh_collegeStats[zbh_MAX_COLLEGES];
extern zbh_GameRecord zbh_gameRecords[zbh_MAX_RECORDS];
extern int zbh_currentCount;
extern int zbh_collegeCount;
extern int zbh_recordCount;

#endif