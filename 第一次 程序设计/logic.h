// 业务逻辑头文件

#ifndef LOGIC_H
#define LOGIC_H

#include "common.h"

void zbh_sortAndCalculate();
void zbh_calculateCollegeStats();
void zbh_addAthlete();
void zbh_modifyRecord();
void zbh_deleteRecord();
void zbh_initTestData();

void zbh_searchByIdOrName();
void zbh_searchByEvent();
void zbh_searchByScoreRange();
void zbh_searchByCollege();

void zbh_showCollegeRanking();
void zbh_showRecordBreakers();
void zbh_showScoreDistribution();

void zbh_groupCompetition();
void zbh_validateScores();
void zbh_displayAll(int zbh_page);
void zbh_printScoreReport();

#endif