// 全局变量定义
// 这里是全局变量真正分配内存的地方。

#ifndef GLOBALS_H
#define GLOBALS_H

#include "common.h"

// 使用 extern 声明，告诉编译器这些变量在别的地方定义了
extern Athlete athletes[MAX_ATHLETES];
extern CollegeStat collegeStats[MAX_COLLEGES];
extern GameRecord gameRecords[MAX_RECORDS];
extern int currentCount;
extern int collegeCount;
extern int recordCount;

#endif