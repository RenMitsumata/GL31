/*================================================================================

	マップエディタ[FileDialog.h]
															Author:Ren Mitsumata
															Date  :2018/10/19
----------------------------------------------------------------------------------

================================================================================*/

#ifndef FILEDIALOG_H
#define FILEDIALOG_H

#include <Windows.h>
#include <stdio.h>
#include <CommCtrl.h>

class CModel;

class FileDialog
{
private:
	OPENFILENAME ofn;
	char m_FileName[MAX_PATH];
public:
	FileDialog();
	virtual ~FileDialog();
	const char* Init(HWND hWnd);
};

#endif // <--- FILEDIALOG_H