/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2004 Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Sam Lantinga
    slouken@libsdl.org
*/



/* PSP thread management routines for SDL */

#include <stdio.h>
#include <stdlib.h>

#include "SDL_error.h"
#include "SDL_thread.h"
#include "../SDL_systhread.h"
#include "../SDL_thread_c.h"
#include <pspkerneltypes.h>
#include <pspthreadman.h>


static int ThreadEntry(SceSize args, void *argp)
{
	SDL_RunThread(*(void **) argp);
	return 0;
}

int SDL_SYS_CreateThread(SDL_Thread *thread, void *args)
{
	SceKernelThreadInfo status;
	int priority = 32;

	/* Set priority of new thread to the same as the current thread */
	status.size = sizeof(SceKernelThreadInfo);
	if (sceKernelReferThreadStatus(sceKernelGetThreadId(), &status) == 0) {
		priority = status.currentPriority;
	}

	thread->handle = sceKernelCreateThread("SDL thread", ThreadEntry, 
					       priority, 0x8000, 
					       PSP_THREAD_ATTR_VFPU, NULL);
	if (thread->handle < 0) {
		SDL_SetError("sceKernelCreateThread() failed");
		return -1;
	}

	sceKernelStartThread(thread->handle, 4, &args);
	return 0;
}

void SDL_SYS_SetupThread(const char *name)
{
	/* Do nothing. */
}

SDL_threadID SDL_ThreadID(void)
{
	return (SDL_threadID) sceKernelGetThreadId();
}

void SDL_SYS_WaitThread(SDL_Thread *thread)
{
	sceKernelWaitThreadEnd(thread->handle, NULL);
	sceKernelDeleteThread(thread->handle);
}

void SDL_SYS_KillThread(SDL_Thread *thread)
{ 
	sceKernelTerminateDeleteThread(thread->handle);
}

int SDL_SYS_SetThreadPriority(SDL_ThreadPriority priority)
{    
    int value;
    
    if (priority == SDL_THREAD_PRIORITY_LOW) {
        value = 19;
    } else if (priority == SDL_THREAD_PRIORITY_HIGH) {
        value = -20;
    } else {
        value = 0;
    }
    
    return sceKernelChangeThreadPriority(sceKernelGetThreadId(),value);

}

/* vim: ts=4 sw=4
 */
