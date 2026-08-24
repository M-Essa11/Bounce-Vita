#ifndef BOUNCE_PLATFORM_H
#define BOUNCE_PLATFORM_H

#include <stdint.h>

typedef uint32_t u32;

#ifdef BOUNCE_VITA
#include <psp2/ctrl.h>
#define PSP_CTRL_SELECT   SCE_CTRL_SELECT
#define PSP_CTRL_START    SCE_CTRL_START
#define PSP_CTRL_UP       SCE_CTRL_UP
#define PSP_CTRL_RIGHT    SCE_CTRL_RIGHT
#define PSP_CTRL_DOWN     SCE_CTRL_DOWN
#define PSP_CTRL_LEFT     SCE_CTRL_LEFT
#define PSP_CTRL_LTRIGGER SCE_CTRL_LTRIGGER
#define PSP_CTRL_RTRIGGER SCE_CTRL_RTRIGGER
#define PSP_CTRL_TRIANGLE SCE_CTRL_TRIANGLE
#define PSP_CTRL_CIRCLE   SCE_CTRL_CIRCLE
#define PSP_CTRL_CROSS    SCE_CTRL_CROSS
#define PSP_CTRL_SQUARE   SCE_CTRL_SQUARE
#else
#include <pspctrl.h>
#include <psptypes.h>
#endif

#endif
