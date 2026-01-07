// 工具模块头文件

#ifndef UTILS_H
#define UTILS_H

#include "common.h"

int zbh_get_visual_width(const char *zbh_str);
void zbh_print_aligned(const char *zbh_str, int zbh_width);
void zbh_print_highlight(const char *zbh_str, const char *zbh_keyword, int zbh_width);
int zbh_isRunningEvent(char *zbh_eventName);
int zbh_getScoreByRank(int zbh_rank);
void zbh_swapAthlete(zbh_Athlete *zbh_a, zbh_Athlete *zbh_b);
void zbh_swapCollege(zbh_CollegeStat *zbh_a, zbh_CollegeStat *zbh_b);

#endif