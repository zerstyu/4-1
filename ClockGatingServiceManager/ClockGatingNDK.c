//#include <jni.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>

#define TRUE		0
#define FALSE		-1
#define DEVICE_NAME	"/dev/EXYNOS4210_ClockGatingDrv"
#define BUFSIZE 	1024
#define MAX 20

typedef enum
{
	IP_TV = 1,
	IP_CAM,
	IP_MFC,
	NUM_OF_BLOCK
}BlockType;

typedef struct
{
	char	  strID[40];
	BlockType ptPart[NUM_OF_BLOCK];
}MessageStruct;

typedef struct
{
	unsigned short IP_CAM;
	unsigned short IP_TV;
	unsigned short IP_MFC;
}BlockStatus;

MessageStruct stMessage;

// server test
static char recv_1[20];
struct termios oldtio, newtio;
int listenfd, connfd, n;
char recvbuffer[BUFSIZE];
char sendbuffer[BUFSIZE];
int recvlen;

//file-descripter for ClockGating Driver
static int fdCGDrv;

void PrintMenu(void)
{
	printf("-----------------------------------------\n");
	printf("-----------------------------------------\n");
	printf("--  Check all CLKGATE_Register  : 'C'  --\n");
	printf("--  ClockGatingTest( p, v )     : 'T'  --\n");
	printf("--  Help                        : 'H'  --\n");
	printf("--  Exit                        : 'X'  --\n");
	printf("-----------------------------------------\n");
	printf("-----------------------------------------\n");
}

void Drvioctl(int a, int b);

int main(void)
{
	socklen_t clilen;
	//char recvline[10];
	struct sockaddr_in servaddr, cliaddr;
	char *ptr;
	char temp[4096][4096];
	int i = 0, l = 0, k = 0, j = 0, h = 0;
	char *out[4096];
	char *outstr;

	unsigned int    ulAddress;
    unsigned long   ulData;
    char Command;
	int idNum = 0;

	// id & module
	char PID[64][64];
	char PModule[64][64];
	char PModuleset[64][64];
	char tempModule[40];
	char tempModule2[40];


	// Module Buffer
	int ModuleBuffer[18];

	for( i = 0; i < 18; i++)
		ModuleBuffer[i] = 0;




	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(7777);

	if( bind(listenfd, (struct sockaddr_in *)&servaddr, sizeof(servaddr)) < 0)
	{
		fprintf(stderr, "[ERROR] Cannot bind a socket\n");
		return 0;
	}
	listen(listenfd, 5);
	
	printf("Listen : %d\n", listenfd);
	clilen = sizeof(cliaddr);

	
	
	
	recvlen = 0;
	memset( PModuleset, 0, sizeof(PModuleset));
	memset( PID, 0, sizeof(PID));
	memset( tempModule, 0, sizeof(tempModule));
	
	
while(1) {

	connfd = accept(listenfd, (struct sockaddr_in *)&cliaddr, &clilen);

	if( connfd < 0 )
	{
		fprintf(stderr, "[ERROR] %d\n", connfd);
		return 0;
	}


	memset( recvbuffer, 0, sizeof(recvbuffer) );
	memset( PModule, 0, sizeof(PModule));


	recvlen = recv(connfd, recvbuffer, BUFSIZE, 0);

	for(i = 0; i < recvlen - 1; i++) {
		recvbuffer[i] = recvbuffer[i+1];
	}
	recvbuffer[recvlen-1] = '\0';

   	/*받은 데이터를 출력*/
    printf("%s\n",recvbuffer);

	ptr = recvbuffer;

	fflush(stdout);
	
	outstr = strtok(recvbuffer,"|");

	i = 0;
	j = 0;
	k = 0;
	h = 0;


	while(outstr != NULL)
	{
		printf("%s\n", outstr);
	
		if( h == 0) {
			while(outstr[j] != '\0') {
				PID[idNum][k] = outstr[j++];
				k++;
			}
			h++;
		}
		else
		{
			j = 0;
			k = 0;
			while(outstr[j] != '\0') {
				PModule[i][k] = outstr[j++];
				k++;
			}
			i++;
		}

		outstr = (char*)strtok(NULL, "|");
	
	}

	printf("\n%s", PID[0]);

	for( h = 0; h < 3; h++) {
		printf("%s\n",PModule[h]);
	}


	printf("receive length : %d\n", recvlen);

	if(recvlen > 39){ // MORE DEVICE NAME Length 
		for(j = 0; j <= i; j++) {
			printf("\n%s", PModule[j]);
			
			if((strcmp(PModule[j], "leftbus")) == 0)
			{
				ModuleBuffer[0] ++;
				Drvioctl( 0, 0);
			}
			if((strcmp(PModule[j], "rightbus")) == 0)
			{
				ModuleBuffer[1] ++;
				Drvioctl( 1, 0);
			}
			if((strcmp(PModule[j], "cam")) == 0)
			{
				ModuleBuffer[2] ++;
				Drvioctl( 2, 0);
			}
			if((strcmp(PModule[j], "tv")) == 0)
			{
				ModuleBuffer[3] ++;
				Drvioctl( 3, 0);
			}
			if((strcmp(PModule[j], "mfc")) == 0)
			{
				ModuleBuffer[4] ++;
				Drvioctl( 4, 0);
			}
			if((strcmp(PModule[j], "g3d")) == 0)
			{
				ModuleBuffer[5] ++;
				Drvioctl( 5, 0);
			}
			if((strcmp(PModule[j], "image")) == 0)
			{
				ModuleBuffer[6] ++;
				Drvioctl( 6, 0);
			}
			if((strcmp(PModule[j], "lcd0")) == 0)
			{
				ModuleBuffer[7] ++;
				Drvioctl( 7, 0);
			}
			if((strcmp(PModule[j], "lcd1")) == 0)
			{
				ModuleBuffer[8] ++;
				Drvioctl( 8, 0);
			}
			if((strcmp(PModule[j], "fsys")) == 0)
			{
				ModuleBuffer[9] ++;
				Drvioctl( 9, 0);
			}
			if((strcmp(PModule[j], "gps")) == 0)
			{
				ModuleBuffer[10] ++;
				Drvioctl( 10, 0);
			}
			if((strcmp(PModule[j], "peril")) == 0)
			{
				ModuleBuffer[11] ++;
				Drvioctl( 11, 0);
			}
			if((strcmp(PModule[j], "perir")) == 0)
			{
				ModuleBuffer[12] ++;
				Drvioctl( 12, 0);
			}
			if((strcmp(PModule[j], "dmc")) == 0)
			{
				ModuleBuffer[13] ++;
				Drvioctl( 13, 0);
			}
			if((strcmp(PModule[j], "cpu")) == 0)
			{
				ModuleBuffer[14] ++;
				Drvioctl( 14, 0);
			}
			if((strcmp(PModule[j], "sclkcam")) == 0)
			{
				ModuleBuffer[15] ++;
				Drvioctl( 15, 0);
			}
			if((strcmp(PModule[j], "sclkcpu")) == 0)
			{
				ModuleBuffer[16] ++;
				Drvioctl( 16, 0);
			}
			if((strcmp(PModule[j], "block")) == 0)
			{
				ModuleBuffer[17] ++;
				Drvioctl( 17, 0);
			}

			// save modules in moduleSetBuffer = index : idNum
			
			strcat(PModuleset[idNum], "|");
			strcat(PModuleset[idNum], PModule[j]);
			
		}
		
		printf(" PModlue[LAST] : %s\n", PModule[2]); 
		printf(" PModuleset : %s\n", PModuleset[idNum]);					
	}
	else // receive ID
	{
		
		recvbuffer[0] = '0';
		printf("1:%s\n", recvbuffer);
		printf("2:%s\n", PID[0]);
	
	
		for(h = 0; h < idNum; h++) {
			for(i = 0; i < recvlen; i++) {
				if(recvbuffer[i] != PID[h][i])
					break;
				if(i == recvlen -1)
					k = h;
			}
		}

		printf("search line : %d\n", k);
		printf("Modules : %s\n", PModuleset[k]);
	
		for(i = 0; i < 30; i++) {
			tempModule[i] = PModuleset[k][i+1];			
		}
		printf(" tempModule : %s\n", tempModule);
	
		memset( tempModule2, 0, sizeof(tempModule2));

		k = 0;
			for(j = 0; j < 50; j++) {
				if(tempModule[j] == '|') {
					
					printf("tempModule2 : %s\n", tempModule2);
	
					h = strcmp(tempModule2, "cam");
					printf("cam o, x : %d\n", h); 


					if((strcmp(tempModule2, "leftbus")) == 0)
				    {
					    --ModuleBuffer[0] ;
					    Drvioctl( 0, -1);
					}
					if((strcmp(tempModule2, "rightbus")) == 0)
					{
						--ModuleBuffer[1] ;
					    Drvioctl( 1, -1);
					}
					if((strcmp(tempModule2, "cam")) == 0)
					{
					    --ModuleBuffer[2] ;
					    Drvioctl( 2, -1);
					}
					if((strcmp(tempModule2, "tv")) == 0)
					{
					    --ModuleBuffer[3] ;
					    Drvioctl( 3, -1);
					}
					if((strcmp(tempModule2, "mfc")) == 0)
					{
					    --ModuleBuffer[4] ;
					    Drvioctl( 4, -1);
					}
					if((strcmp(tempModule2, "g3d")) == 0)
					{
					    --ModuleBuffer[5] ;
					    Drvioctl( 5, -1);
					}
					if((strcmp(tempModule2, "image")) == 0)
					{
					    --ModuleBuffer[6] ;
					    Drvioctl( 6, -1);
					}
					if((strcmp(tempModule2, "lcd0")) == 0)
					{
					    --ModuleBuffer[7] ;
					    Drvioctl( 7, -1);
					}
					if((strcmp(tempModule2, "lcd1")) == 0)
					{
					    --ModuleBuffer[8] ;
					    Drvioctl( 8, -1);
					}
					if((strcmp(tempModule2, "fsys")) == 0)
					{
					    --ModuleBuffer[9] ;
					    Drvioctl( 9, -1);
					}
					if((strcmp(tempModule2, "gps")) == 0)
					{
					    --ModuleBuffer[10] ;
					    Drvioctl( 10, -1);
                  	}
				    if((strcmp(tempModule2, "peril")) == 0)
				    {
				        --ModuleBuffer[11] ;
					    Drvioctl( 11, -1);
					}
					if((strcmp(tempModule2, "perir")) == 0)
					{
					    --ModuleBuffer[12] ;
					    Drvioctl( 12, -1);
					}
					if((strcmp(tempModule2, "dmc")) == 0)
					{
					    --ModuleBuffer[13] ;
					    Drvioctl( 13, -1);
                    }
		            if((strcmp(tempModule2, "cpu")) == 0)
		            {
		                --ModuleBuffer[14] ;
		                Drvioctl( 14, -1);
		            }
		            if((strcmp(tempModule2, "sclkcam")) == 0)
		            {
				        --ModuleBuffer[15] ;
				        Drvioctl( 15, -1);
				    }
				    if((strcmp(tempModule2, "sclkcpu")) == 0)
				    {
				        --ModuleBuffer[16] ;
					    Drvioctl( 16, -1);
			 		}
                    if((strcmp(tempModule2, "block")) == 0)
	                {
		                --ModuleBuffer[17]; 
		                Drvioctl( 17, -1);
		            }
						
							
					k = 0;
				}
				else {
					tempModule2[k] = tempModule[j];
					k++;
				}
			}


		//exit(0);

	
		
	}
		memset( recvbuffer, 0, sizeof(recvbuffer) );
		idNum++;
		printf("-> All Gates Clock Enable.\n");
		ioctl( fdCGDrv, 0 );
		close(connfd);
	}
EXIT:
	close(fdCGDrv);
	close(connfd);
	close(listenfd);
	return 0;
}

void Drvioctl(int a, int b)
{
	fdCGDrv = open( DEVICE_NAME, O_RDWR, 0777 );
	if( fdCGDrv < 0 )
	{
		printf("[ERROR]Open the Clock-Gating Driver is failed.\n");
		printf(" fdCGDrv : %d\n", fdCGDrv);
		 
		return FALSE;
	}
	
	printf("Open the Clock-Gating Driver is success!\n\n");

	switch(a) // on
	{
		case 0:
			if(b == 0)
				ioctl( fdCGDrv, 0x04800, 0x00000000 );
			else
				ioctl( fdCGDrv, 0x04800, 0xFFFFFFFF );
			break;
	
		case 1:
	        if(b == 0)
	            ioctl( fdCGDrv, 0x08800, 0x00000000 );
	        else
	            ioctl( fdCGDrv, 0x08800, 0xFFFFFFFF );
	        break;
	
		case 2:
	        if(b == 0)
	            ioctl( fdCGDrv, 0x0C920, 0x00000000 );
	        else
	            ioctl( fdCGDrv, 0x0C920, 0xFFFFFFFF );
	        break;

		case 3:
			if(b == 0)
			    ioctl( fdCGDrv, 0x0C924, 0x00000000 );
			else
			    ioctl( fdCGDrv, 0x0C924, 0xFFFFFFFF );
			break;

		case 4:
			if(b == 0)
			    ioctl( fdCGDrv, 0x0C928, 0x00000000 );
			else
			    ioctl( fdCGDrv, 0x0C928, 0xFFFFFFFF );
			break;

		case 5:
	        if(b == 0)
		        ioctl( fdCGDrv, 0x0C92C, 0x00000000 );
	        else
		        ioctl( fdCGDrv, 0x0C92C, 0xFFFFFFFF );
	        break;

		case 6:
		    if(b == 0)
		        ioctl( fdCGDrv, 0x0C930, 0x00000000 );
	        else
		        ioctl( fdCGDrv, 0x0C930, 0xFFFFFFFF );
	        break;

		case 7:
	        if(b == 0)
		        ioctl( fdCGDrv, 0x0C934, 0x00000000 );
	        else
		        ioctl( fdCGDrv, 0x0C934, 0xFFFFFFFF );
	        break;

		case 8:
	        if(b == 0)
		        ioctl( fdCGDrv, 0x0C938, 0x00000000 );
	        else
		        ioctl( fdCGDrv, 0x0C938, 0xFFFFFFFF );
	        break;

		case 9:
	        if(b == 0)
		        ioctl( fdCGDrv, 0x0C940, 0x00000000 );
	        else
		        ioctl( fdCGDrv, 0x0C940, 0xFFFFFFFF );
	        break;
	
		case 10:
	        if(b == 0)
		        ioctl( fdCGDrv, 0x0C94C, 0x00000000 );
	        else
		        ioctl( fdCGDrv, 0x0C94C, 0xFFFFFFFF );
	        break;

		case 11:
	        if(b == 0)
		        ioctl( fdCGDrv, 0x0C950, 0x00000000 );
	        else
		        ioctl( fdCGDrv, 0x0C950, 0xFFFFFFFF );
	        break;

		case 12:
	        if(b == 0)
		        ioctl( fdCGDrv, 0x0C960, 0x00000000 );
	        else
		        ioctl( fdCGDrv, 0x0C960, 0xFFFFFFFF );
	        break;

		case 13:
	        if(b == 0)
		        ioctl( fdCGDrv, 0x10900, 0x00000000 );
	        else
		        ioctl( fdCGDrv, 0x10900, 0xFFFFFFFF );
	        break;

		case 14:
	        if(b == 0)
		        ioctl( fdCGDrv, 0x14900, 0x00000000 );
	        else
		        ioctl( fdCGDrv, 0x14900, 0xFFFFFFFF );
	        break;


		case 15:
	        if(b == 0)
		        ioctl( fdCGDrv, 0x0C820, 0x00000000 );
	        else
		        ioctl( fdCGDrv, 0x0C820, 0xFFFFFFFF );
	        break;

		case 16:
	        if(b == 0)
		        ioctl( fdCGDrv, 0x14800, 0x00000000 );
	        else
		        ioctl( fdCGDrv, 0x14800, 0xFFFFFFFF );
	        break;

		case 17:
	        if(b == 0)
		        ioctl( fdCGDrv, 0x0C970, 0x00000000 );
	        else
		        ioctl( fdCGDrv, 0x0C970, 0xFFFFFFFF );
	        break;
	}
}
