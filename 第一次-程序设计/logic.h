// 业务逻辑头文件

#ifndef LOGIC_H
#define LOGIC_H

#include "common.h"

void sortAndCalculate();
void calculateCollegeStats();
void addAthlete();
void modifyRecord();
void deleteRecord();
void initTestData();

void searchByIdOrName();
void searchByEvent();
void searchByScoreRange();
void searchByCollege();

void showCollegeRanking();
void showRecordBreakers();
void showScoreDistribution();

void groupCompetition();
void validateScores();
void displayAll(int page);
void printScoreReport();

#endif