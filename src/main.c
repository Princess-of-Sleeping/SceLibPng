
#include <psp2/kernel/modulemgr.h>

int *_sceLibcErrnoLoc(void);

int *__errno(void){
	// static int errno = 0;
	// return &errno;
	return _sceLibcErrnoLoc();
}

int module_start(SceSize args, void *argp){
	return SCE_KERNEL_START_SUCCESS;
}
