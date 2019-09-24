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

class FileDialog
{
private:
	OPENFILENAME ofn = {};
	char m_FileName[MAX_PATH];
public:
	FileDialog();
	virtual ~FileDialog();
	void Init(HWND hWnd);
};

#endif // <--- FILEDIALOG_H