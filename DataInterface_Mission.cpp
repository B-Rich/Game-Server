#include<winsock2.h>
#include<stdio.h>
#include"DataInterface.h"

void cb_dataInterface_Mission_getMissionList(MYSQL *dbCon, diJob_missionListData_t *job, void *cb, void *param)
{
	char queryText[1024];
	job->outMissionCount = 0;
	job->outMissionList = NULL;

	wsprintf(queryText, "SELECT "
		"missionId,dispenserNPC,collectorNPC"
		" FROM mission");

	// execute query
	if( mysql_query(dbCon, queryText) )
	{
		printf("Error in query\n");
		while(1) Sleep(1000);	
	}
	MYSQL_RES *dbResult = mysql_store_result(dbCon);
	MYSQL_ROW dbRow;
	// allocate mission data
	int missionCount = (int)mysql_num_rows(dbResult);
	di_missionData_t *missionDataList = (di_missionData_t*)malloc(sizeof(di_missionData_t) * missionCount);
	
	int idx = 0;
	while((dbRow = mysql_fetch_row(dbResult)))
	{
		di_missionData_t *missionData = missionDataList+idx;
		idx++;

		unsigned int mission_id;
		unsigned long long mission_dispenserNPC;
		unsigned long long mission_collectorNPC;

		int idx = 0;
		sscanf(dbRow[idx], "%u", &mission_id); idx++;
		sscanf(dbRow[idx], "%I64u", &mission_dispenserNPC); idx++;
		sscanf(dbRow[idx], "%I64u", &mission_collectorNPC); idx++;
		missionData->missionId = mission_id;
		missionData->collectorNPC = mission_collectorNPC;
		missionData->dispenserNPC = mission_dispenserNPC;
	}
	mysql_free_result(dbResult);
	job->outMissionList = missionDataList;
	job->outMissionCount = missionCount;
	// do callback
	((void (*)(void*,void*))cb)(param, job);
	// free data
	free(missionDataList);
	dataInterface_freeJob(job);
}

void dataInterface_Mission_getMissionList(void (*cb)(void *param, diJob_missionListData_t *jobData), void *param)
{	
	diJob_missionListData_t *job = (diJob_missionListData_t*)dataInterface_allocJob(sizeof(diJob_missionListData_t));
	dataInterface_queueJob(job, cb_dataInterface_Mission_getMissionList, cb, param);
}