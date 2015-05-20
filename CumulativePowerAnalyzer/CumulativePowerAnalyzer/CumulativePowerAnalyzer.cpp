#include <stdio.h>
#include <visa.h>
#include <time.h>

#define EXPCNT	132

int main()
{
	clock_t clkStart, clkFinish;
	double dbGetData, dbMAX, dbMIN, dbAverage, dbCumulative;

	ViSession rm, session;
	viOpenDefaultRM(&rm);

	dbCumulative = 0;
	dbMAX = 0;
	dbMIN = 0xFFFFFFFFF;

	clkStart = clock();

	for( int i = 0; i < EXPCNT; ++i )
	{
//		viOpen(rm, "USB0::0x0957::0x0618::tw47500260::0::INSTR", VI_NULL, VI_NULL, &session);
		viOpen(rm, "Ag34405A", VI_NULL, VI_NULL, &session);

		viPrintf(session, "RST\n");
		viPrintf(session, "CONF:CURR:DC 10\n");

		viPrintf(session, "READ?\n");
		viScanf(session, "%lf", &dbGetData);

		//////////////////////////////////////////////////////
		dbCumulative += dbGetData;
		
		if( dbMAX < dbGetData )
			dbMAX = dbGetData;
		if( dbMIN > dbGetData )
			dbMIN = dbGetData;
		//////////////////////////////////////////////////////

		fflush(stdout);
		printf("[Current] %5.5lf\n", dbGetData);

		viPrintf(session, "INIT\n" );
		viPrintf(session, "ABORt\n");
	}

	clkFinish = clock();

	printf("\n\n");
	printf("[MAX : %5.5lf] ", dbMAX);
	printf("[MIN : %5.5lf] ", dbMIN);
	printf("[CUMULATIVE : %5.5lf] ", dbCumulative);
	printf("[AVERAGE : %5.5lf]\n", (dbCumulative / (double)EXPCNT) );
	printf("[Count : %d] ", EXPCNT);
	printf("[Duration : %5.5lf(s)] ", (double)(clkFinish - clkStart) / CLOCKS_PER_SEC);


	viClose(session);

	getchar();
}