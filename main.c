//based on procfw cIPL inslaller

#include <pspsdk.h>
#include <pspkernel.h>
#include <pspctrl.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <kubridge.h>
#include <psppower.h>

#include "includes/pspipl_update.h"
#include "includes/ipl_block_large.h"//PRO FULL IPL WITH SLIM IPL
#include "includes/ipl_block_largeME.h"//ME FULL IPL WITH SLIM IPL
#include "includes/ipl_block_01g.h"//PRO IPL HEADER WITH FAT IPL
#include "includes/ipl_block_01gME.h"//ME IPL HEADER WITH FAT IPL

PSP_MODULE_INFO("IPLSwitcher", 0x0800, 1, 0); 
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_VSH);

#define printf pspDebugScreenPrintf

u32 sceSysregGetTachyonVersion(void);// NID HASH: 0xE2A5D1EE
int scePowerRequestStandby();
int model, devkit;
static u8 orig_ipl[0x24000] __attribute__((aligned(64)));

void ErrorExit(int milisecs, char *fmt, ...) 
{	
	char msg[256];

	va_list list;
	va_start(list, fmt);
	vsprintf(msg, fmt, list);
	va_end(list);
	
	printf(msg);
	sceKernelDelayThread(milisecs * 1000);
	sceKernelExitGame(); 
}

void flash_ipl(int size, int me)
{
	printf("Flashing IPL...");

	if(pspIplUpdateClearIpl() < 0)
		ErrorExit(3000, "Failed in clear IPL!\n");

	if(pspIplUpdateSetIpl(me ? ipl_block_largeME : ipl_block_large, size + 0x4000) < 0)
		ErrorExit(3000, "Failed in write IPL!\n");

	printf("Done...\n");
}

int is_ta88v3(void)
{
	return (model == 1/*PSP 2k*/ && sceSysregGetTachyonVersion() == 0x00600000 /*TA-088v3 tachyon*/) ? 1 : 0;
}

int main() 
{
	SceUID kpspident, mod;
	devkit = sceKernelDevkitVersion();
	model = kuKernelGetModel();
	
	//initializing debuglib screen
	pspDebugScreenInit();

	//loading/starting kpspident.prx by jas0nuk & yoti. Need for check TA-088v3
	kpspident = pspSdkLoadStartModule("kpspident.prx", PSP_MEMORY_PARTITION_KERNEL);
	if(kpspident < 0)//error if can't load/start kpspident.prx
		ErrorExit(2000, "Can't start kpspident.prx\n");
	
	//loading/starting ipl_update.prx by neur0n
	mod = sceKernelLoadModule("ipl_update.prx", 0, NULL);
	if(mod < 0)//error if can't load ipl_update.prx
		ErrorExit(2000, "Can't load ipl_update.prx\n");
	
	mod = sceKernelStartModule(mod, 0, NULL, NULL, NULL);
	if(mod < 0)//error if can't load ipl_update.prx
		ErrorExit(2000, "Can't start ipl_update.prx\n");

	//detect 088v3 model
	if(!(model == 0 || model == 1) || is_ta88v3())
		ErrorExit(2000, "You mobo don't supported...\n");

	//check FW, because of this program only for 6.60
	if(devkit != 0x06060010)//6.60
		ErrorExit(2000, "This program only for FW6.60...\n");

	//chech size of installed IPL
	int size = pspIplUpdateGetIpl(orig_ipl);

	//print topic
	printf(
		"\nCustomIPL Switcher\n"
		"Current IPL: %s\n\n"
		" Press X to install cIPL PRO CFW.\n"
		" Press [] to install cIPL ME CFW.\n"
		" Press R to cancel\n\n", size == 0x24000 ? "Custom" : "Original");

	(void)size_ipl_block_large;
	
	while(1)
	{
        SceCtrlData pad;
        sceCtrlReadBufferPositive(&pad, 1);

		if(pad.Buttons & PSP_CTRL_CROSS)
		{
			//verifying installed procfw
			if(sceIoOpen("flash0:/vsh/module/_recovery.prx", PSP_O_RDONLY, 777) < 0)
				ErrorExit(2000, "Please install PRO CFW. Exiting...\n");

			if(model == 0)
				memcpy(ipl_block_large, ipl_block_01g, 0x4000);

			if(size == 0x24000)
			{
				size -= 0x4000;
				memmove(ipl_block_large + 0x4000, orig_ipl + 0x4000, size);
			}
			else if(size == 0x20000)
				memmove(ipl_block_large + 0x4000, orig_ipl, size);

			//install procfw IPL 
			flash_ipl(size, 0);

			break; 
		}
		else if(pad.Buttons & PSP_CTRL_SQUARE)
		{
			//verifying installed mecfw
			if(sceIoOpen("flash0:/vsh/module/recovery.prx", PSP_O_RDONLY, 777) < 0)
				ErrorExit(2000, "Please install ME CFW. Exiting...\n");
		
			if(model == 0)
				memcpy(ipl_block_largeME, ipl_block_01gME, 0x4000);

			if(size == 0x24000)
			{
				size -= 0x4000;
				memmove(ipl_block_largeME + 0x4000, orig_ipl + 0x4000, size);
			}
			else if(size == 0x20000)
				memmove(ipl_block_largeME + 0x4000, orig_ipl, size);

			//install procfw IPL 
			flash_ipl(size, 1);

			break; 
		}
		else if(pad.Buttons & PSP_CTRL_RTRIGGER)
			ErrorExit(2000, "Exiting...\n");
	}

	printf("Install Complete! ShuttingDown...");
	sceKernelDelayThread(1000 * 1000 * 3);
	scePowerRequestStandby();//shutdown

	return 0;
}