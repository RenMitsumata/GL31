/*================================================================================

	マップエディタ[FileDialog.cpp]
															Author:Ren Mitsumata
															Date  :2018/10/19
----------------------------------------------------------------------------------

================================================================================*/
#include <Windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>

#include <vector>
#include <map>
#include <unordered_map>
#include "texture.h"
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/matrix4x4.h>
#pragma comment (lib,"OpenGL32.lib")
#pragma comment (lib,"GLU32.lib")
#pragma comment (lib,"assimp.lib")
#include "CModel.h"
#include "FileDialog.h"

FileDialog::FileDialog()
{
}


FileDialog::~FileDialog()
{
}

const char* FileDialog::Init(HWND hWnd) {
	// ゼロ埋め
	ofn = { 0 };
	// 初期化
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFilter = "マップ(*.fbx)\0*.fbx\0すべて(*.*)\0*.*\0\0";
	ofn.lpstrFile = m_FileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrDefExt = "map";
	if (GetOpenFileName(&ofn)) {		
		return m_FileName;
	}
	else {
		return nullptr;
	}

}

