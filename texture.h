#ifndef TEXTURE_H
#define TEXTURE_H

#include <Windows.h>
#include <stdio.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "common.h"
#include "system_timer.h"

#if WIN32
#pragma comment (lib,"OpenGL32.lib")
#pragma comment (lib,"GLU32.lib")
#else
#pragma comment (lib,"OpenGL64.lib")
#pragma comment (lib,"GLU64.lib")
#endif

enum FILETYPE {
	FILETYPE_TGA = 0,
	FILETYPE_PNG,
	FILETYPE_JPEG
};


unsigned int LoadTexture(const char* filename, unsigned int e_FILETYPE);
void DeleteTexture(void);

#endif