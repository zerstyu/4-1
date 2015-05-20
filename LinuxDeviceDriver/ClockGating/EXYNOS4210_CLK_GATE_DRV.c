///////////////////////////////////////////////////////////////////////////
//									 //
//	This Driver is operated on Linux Kernel 3.0x or upper version.	 //
//									 //
//		Developed by D.C. LEE in H.U.F.S. May, 2013		 //
//									 //
///////////////////////////////////////////////////////////////////////////

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/errno.h>	// For Error Code
#include <linux/slab.h>		// For kmalloc()
#include <linux/device.h>
#include <asm/uaccess.h>
#include <linux/unistd.h>	// For Delay
#include <linux/cdev.h>		// Linux 2.6x or upper version

// Register Definition for Clock Gating
#include "EXYNOS4210_CLK_GATE_DRV.h"

#define DEVICE_NAME			"EXYNOS4210_ClockGatingDrv"
#define CLASS_NAME			"S4210ClkGatingDrv"
#define AUTHOR				"D.C. LEE <dong880510@naver.com>"
#define DESCRIPTION			"EXYNOS4210 Clock0Gating Drvier"
#define VERSION				"0.1"

#define MODE_CHECK			0
#define MODE_TEST			1

// In the Headers
#define TRUE				0
#define FALSE				-1

#define ReadRegister(p)		(*(unsigned long *)(p))
#define WriteRegister(p, v)	(*(unsigned long *)(p) = (v))

static dev_t stDev;
static struct cdev stCDev;
struct class *stClass;

typedef struct
{
	unsigned long IP_LEFTBUS;
	unsigned long IP_RIGHTBUS;
	unsigned long IP_CAM;
	unsigned long IP_TV;
	unsigned long IP_MFC;
	unsigned long IP_G3D;
	unsigned long IP_IMAGE;
	unsigned long IP_LCD0;
	unsigned long IP_LCD1;
	unsigned long IP_FSYS;
	unsigned long IP_GPS;
	unsigned long IP_PERIL;
	unsigned long IP_PERIR;
	unsigned long IP_DMC;
	unsigned long IP_CPU;

	unsigned long SCLKCAM;
	unsigned long SCLKCPU;
	unsigned long BLOCK;
}EXYNOS4_CLKGATE_REGSTAT;

EXYNOS4_CLKGATE_REGSTAT RegStat;

static int 	EXYNOS4210_ClockGatingDrv_init(void);
static int 	EXYNOS4210_ClockGatingDrv_exit(void);
int 		EXYNOS4210_ClockGatingDrv_open( struct inode *inode_p,
											struct file *fp );
int 		EXYNOS4210_ClockGatingDrv_release(	struct inode *inode_p,
												struct file *fp );
int 		EXYNOS4210_ClockGatingDrv_ioctl
								( struct file *fp,
								  unsigned int command, unsigned long args );

void 		CheckAllClockGates(void);
void 		WriteRegisterDelay(void);

// File operation Structure
// The '=' is generating structure at build
// And, Using the Structure anywhere in Traslate Area
static const struct file_operations fops =
{
	.open			= EXYNOS4210_ClockGatingDrv_open,
	.release		= EXYNOS4210_ClockGatingDrv_release,
	.unlocked_ioctl	= EXYNOS4210_ClockGatingDrv_ioctl
};

static int EXYNOS4210_ClockGatingDrv_init()
{
	int _ERR_;

	printk("-------------------------------------------------------------\n");
	printk("---                                                       ---\n");
	printk("---           ClockGatingDriver for EXYNOS4210            ---\n");
	printk("---                                                       ---\n");
	printk("-------------------------------------------------------------\n");

	_ERR_ = alloc_chrdev_region( &stDev, 0, 1, DEVICE_NAME );

	if( _ERR_ < 0 )
	{
		printk("[ERROR] Regist of EXYNOS4210_ClockGatingDrv failed.\n");
		printk("   - Error Code : %d\n", _ERR_);
		return FALSE;
	}
	
	printk("Regist of EXYNOS4210_ClockGatingDrv is success!\n");

	stClass = class_create( THIS_MODULE, DEVICE_NAME );

	cdev_init( &stCDev, &fops );
	_ERR_ = cdev_add( &stCDev, stDev, 1 );

	if( _ERR_ < 0 )
	{
		printk("[ERROR] The Device class Create and Add failed.\n");
		return FALSE;
	}

	device_create( stClass, NULL, stDev, NULL, DEVICE_NAME );

	printk("Create a Clock-Gating Driver for EXYNOS4210 is success!\n");
	printk("-------------------------------------------------------------\n\n");

	return TRUE;
}

static int EXYNOS4210_ClockGatingDrv_exit()
{
	unregister_chrdev_region( stDev, 1 );

	cdev_del( &stCDev );
	device_destroy( stClass, stCDev.dev );
	class_destroy( stClass );

	printk("Un-regist of EXYNOS4210_ClkGatingDrv.\n");
	printk("-------------------------------------------------------------\n\n");

	return TRUE;
}

int EXYNOS4210_ClockGatingDrv_open( struct inode *inode_p, struct file *fp )
{
	printk("Open EXYNOS4210_ClkGatingDrv\n");

	return TRUE;
}

int EXYNOS4210_ClockGatingDrv_release( struct inode *inode_p, struct file *fp )
{
	printk("Release EXYNOS4210_ClkGatingDrv\n");

	return TRUE;
}

int EXYNOS4210_ClockGatingDrv_ioctl( struct file *fp, unsigned int command, unsigned long args )
{
	switch( command )
	{
		// Check all Clock-Gating Registers
		case MODE_CHECK:
		{
			CheckAllClockGates();

			break;
		}
		// ClockGating for Test
		default:
		{
			printk("------[ Test for CLKGATE ]------\n");
			printk("-----command : %d, args : %d----\n", command, args);
			WriteRegister( EXYNOS_CLKREG(command), args );
			WriteRegisterDelay();
			printk("--------------------------------\n");

			break;
		}
	}

	return TRUE;
}

void CheckAllClockGates(void)
{
	RegStat.IP_LEFTBUS	= ReadRegister( EXYNOS4_CLKGATE_IP_LEFTBUS);
	RegStat.IP_RIGHTBUS	= ReadRegister( EXYNOS4_CLKGATE_IP_RIGHTBUS);
	RegStat.IP_CAM		= ReadRegister( EXYNOS4_CLKGATE_IP_CAM	);
	RegStat.IP_TV		= ReadRegister( EXYNOS4_CLKGATE_IP_TV	);
	RegStat.IP_MFC		= ReadRegister( EXYNOS4_CLKGATE_IP_MFC	);
	RegStat.IP_G3D		= ReadRegister( EXYNOS4_CLKGATE_IP_G3D	);
	RegStat.IP_IMAGE	= ReadRegister( EXYNOS4_CLKGATE_IP_IMAGE);
	RegStat.IP_LCD0		= ReadRegister( EXYNOS4_CLKGATE_IP_LCD0	);
	RegStat.IP_LCD1		= ReadRegister( EXYNOS4_CLKGATE_IP_LCD1 );
	RegStat.IP_FSYS		= ReadRegister( EXYNOS4_CLKGATE_IP_FSYS	);
	RegStat.IP_GPS		= ReadRegister( EXYNOS4_CLKGATE_IP_GPS	);
	RegStat.IP_PERIL	= ReadRegister( EXYNOS4_CLKGATE_IP_PERIL);
	RegStat.IP_PERIR	= ReadRegister( EXYNOS4_CLKGATE_IP_PERIR);
	RegStat.IP_DMC		= ReadRegister( EXYNOS4_CLKGATE_IP_DMC	);
	RegStat.IP_CPU		= ReadRegister( EXYNOS4_CLKGATE_IP_CPU	);
	RegStat.SCLKCAM		= ReadRegister( EXYNOS4_CLKGATE_SCLKCAM	);
	RegStat.SCLKCPU		= ReadRegister( EXYNOS4_CLKGATE_SCLKCPU	);
	RegStat.BLOCK		= ReadRegister( EXYNOS4_CLKGATE_BLOCK	);

	printk("------[ Status of Clock-Gating Register ]------\n");
	printk("\tIP_LEFTBUS    : %lX\n", RegStat.IP_LEFTBUS	);
	printk("\tIP_RIGHTBUS   : %lX\n", RegStat.IP_RIGHTBUS	);
	printk("\tIP_CAM        : %lX\n", RegStat.IP_CAM		);
	printk("\tIP_TV         : %lX\n", RegStat.IP_TV			);
	printk("\tIP_MFC        : %lX\n", RegStat.IP_MFC		);
	printk("\tIP_G3D        : %lX\n", RegStat.IP_G3D		);
	printk("\tIP_IMAGE      : %lX\n", RegStat.IP_IMAGE		);
	printk("\tIP_LCD0       : %lX\n", RegStat.IP_LCD0		);
	printk("\tIP_LCD1       : %lX\n", RegStat.IP_LCD1		);
	printk("\tIP_FSYS       : %lX\n", RegStat.IP_FSYS		);
	printk("\tIP_GPS        : %lX\n", RegStat.IP_GPS		);
	printk("\tIP_PERIL      : %lX\n", RegStat.IP_PERIL		);	
	printk("\tIP_PERIR      : %lX\n", RegStat.IP_PERIR		);	
	printk("\tIP_DMC        : %lX\n", RegStat.IP_DMC		);	
	printk("\tIP_CPU        : %lX\n", RegStat.IP_CPU		);	
	printk("-----------------------------------------------\n");
}

// It used for safety modifying register
void WriteRegisterDelay()
{
	int 		 i, j;
	volatile int Dummy=0;

	for( i=0; i<1000; i++ )
		for( j=0; j<1000; j++ )
			Dummy++;	
}

// Below is module init / exit for Kernel
module_init( EXYNOS4210_ClockGatingDrv_init );
module_exit( EXYNOS4210_ClockGatingDrv_exit );

MODULE_AUTHOR(AUTHOR);
MODULE_DESCRIPTION(DESCRIPTION);
MODULE_VERSION(VERSION);
MODULE_LICENSE("Dual BSD/GPL");

