// 工具模块头文件

#ifndef UTILS_H
#define UTILS_H

#include "common.h"

int get_visual_width(const char *str);
void print_aligned(const char *str, int width);
void print_highlight(const char *str, const char *keyword, int width);
int isRunningEvent(char *eventName);
int getScoreByRank(int rank);
void swapAthlete(Athlete *a, Athlete *b);
void swapCollege(CollegeStat *a, CollegeStat *b);

#endif