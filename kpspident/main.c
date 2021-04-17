#include <pspsdk.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <psploadcore.h>
#include <pspiofilemgr.h>
#include <pspdisplay.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

PSP_MODULE_INFO("kpspident", 0x1006, 1, 0);
PSP_MAIN_THREAD_ATTR(0);

u32 sceSysreg_driver_E2A5D1EE(void);

u32 sceSysregGetTachyonVersion(void)
{
	int k1 = pspSdkSetK1(0);
	int err = sceSysreg_driver_E2A5D1EE();
	pspSdkSetK1(k1);

	return err;
}

int module_start(SceSize args, void *argp)
{
	return 0;
}

int module_stop(void)
{
	return 0;
}
