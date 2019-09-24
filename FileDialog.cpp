/*================================================================================

	�}�b�v�G�f�B�^[FileDialog.cpp]
															Author:Ren Mitsumata
															Date  :2018/10/19
----------------------------------------------------------------------------------

================================================================================*/

#include "FileDialog.h"



FileDialog::FileDialog()
{
}


FileDialog::~FileDialog()
{
}

void FileDialog::Init(HWND hWnd) {
	// �[������
	ofn = { 0 };
	// ������
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFilter = "�}�b�v(*.fbx)\0*.fbx\0���ׂ�(*.*)\0*.*\0\0";
	ofn.lpstrFile = m_FileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrDefExt = "map";
	if (GetOpenFileName(&ofn)) {
		
	
	}
	
	/*
	HINSTANCE     hInstance;
	LPCTSTR       lpstrFilter;
	LPTSTR        lpstrCustomFilter;
	DWORD         nMaxCustFilter;
	DWORD         nFilterIndex;
	LPTSTR        lpstrFile;
	DWORD         nMaxFile;
	LPTSTR        lpstrFileTitle;
	DWORD         nMaxFileTitle;
	LPCTSTR       lpstrInitialDir;
	LPCTSTR       lpstrTitle;
	DWORD         Flags;
	WORD          nFileOffset;
	WORD          nFileExtension;
	LPCTSTR       lpstrDefExt;
	DWORD         lCustData;
	LPOFNHOOKPROC lpfnHook;
	LPCTSTR       lpTemplateName;
} OPENFILENAME;

hInstance �́A�_�C�A���O�{�b�N�X�e���v���[�g��p����ꍇ��
�e���v���[�g���܂ރ��W���[���̃C���X�^���X�n���h�����w�肵�܂�

lpstrFilter �ɂ́A�I��Ώۂ̃t�@�C�����t�B���^�����O���邽�߂̕�������w�肵�܂�
���̕�����́A�\��������\0�t�B���^������ �Ƃ����`�Ŏw�肵�܂�
�\��������̓_�C�A���O�̃��X�g�{�b�N�X�ɕ\������镶�����
�t�B���^������́A�t�B���^�p�^�[�����i����� MS - DOS �Ɠ����j�w�肵�܂�
�Ⴆ�΁A"Text (*.txt)\0*.txt" �Ƃ���΁A�g���q�� txt �̃t�@�C���݂̂��\������܂�

���̑g�ݍ��킹��A�����邱�ƂŁA���X�g�{�b�N�X�ɂ��̑g�ݍ��킹�̏�񂪉��Z����܂�
������̏I�[��m�点��ɂ� NULL ������2�A�����Ďw�肵�܂�

lpstrCustomFilter �́A���[�U�[��`�̃t�B���^������o�b�t�@�ւ̃|�C���^���w�肵�܂�
����́AlpstrFilter ���l�ɕ\��������ƃt�B���^������̑g�ݍ��킹�ł�
�V�X�e���́A�I�����ꂽ�t�B���^�p�^�[�������̃o�b�t�@�ɕۑ����܂�
�\��������͕ς��܂��񂪁A�t�B���^�͍Ō�ɕۑ����ꂽ���̂ɂȂ��Ă���ł��傤

nMaxCustFilter �ɂ́AlpstrCustomFilter �̃o�b�t�@�T�C�Y���w�肵�܂�
���̒����́A�Œ�ł� 40 �������ȏ�̃T�C�Y���w�肵�Ȃ���΂Ȃ�܂���
lpstrCustomFilter �� NULL �̏ꍇ�͖�������܂�

nFilterIndex �ɂ́A�_�C�A���O�̏����ݒ�̃t�B���^�C���f�b�N�X���w�肵�܂�
0 ���w�肷��΁A���[�U�[��`�̃J�X�^���t�B���^��\���܂�

lpstrFile �́A�t�@�C�������i�[����� 256 �����ȏ�̃o�b�t�@�ւ̃|�C���^���w�肵�܂�
���̃o�b�t�@�ɁA�I�����ꂽ�t�@�C���̃t���p�X���i�[����܂�
nMaxFile �ɂ́AlpstrFile ���w���o�b�t�@�T�C�Y���w�肵�܂�

lpstrFileTitle �ɂ́A�t�@�C���^�C�g�����i�[����o�b�t�@�ւ̃|�C���^���w�肵�܂�
�t�@�C���^�C�g���Ƃ́A�h���C�u���ƃf�B���N�g�������������A�t�@�C�����̖̂��O�ł�
�K�v���Ȃ��ꍇ�� NULL ��I�����Ă�������
nMaxFileTitle �ɂ́AlpstrFileTitle ���w���o�b�t�@�T�C�Y���w�肵�܂�

lpstrInitialDir �́A�����̃t�@�C���f�B���N�g����\��������̃|�C���^���w�肵�܂�
���̃����o�� NULL �̏ꍇ�A�I���_�C�A���O�̓J�����g�f�B���N�g����\�����܂�
lpstrTitle �ɂ́A�_�C�A���O�̃^�C�g���o�[�ɕ\�����镶������w�肵�܂�
NULL ���w�肷��΁A�f�t�H���g�̕������^�C�g���o�[�ɕ\������܂�

Flags �́A�_�C�A���O�쐬�t���O���w�肵�܂�
���̃����o�ɂ́A�ȉ��̒萔�̑g�ݍ��킹���w�肷�邱�Ƃ��ł��܂�

�萔	���
OFN_ALLOWMULTISELECT	�u�t�@�C�����v���X�g�{�b�N�X�ŕ����I�����\�ɂ���
��p�e���v���[�g��p���ă_�C�A���O���쐬����ꍇ
�u�t�@�C�����v���X�g�{�b�N�X�̒�`�� LBS_EXTENDEDSEL �l������
���̃t���O��I������� lpstrFile �����o���w���o�b�t�@��
�f�B���N�g���ւ̃p�X�ƁA�I�����ꂽ�S�Ẵt�@�C����
�����āA�t�@�C�����̊Ԃ̓X�y�[�X�ŋ�؂��i�[�����
OFN_EXPLORER �t���O���ݒ肳��Ă���ꍇ��
���ꂼ�� NULL �����ŋ�؂��A�A������ NULL �����ŏI���
OFN_CREATEPROMPT	���ݑ��݂��Ȃ��t�@�C�����쐬���邩�����߂�v�����v�g��\������
OFN_PATHMUSTEXIST �� OFN_FILEMUSTEXIST �t���O���܂�
OFN_ENABLEHOOK	lpfnHook �Ŏw�肳�ꂽ�t�b�N�֐���L���ɂ���
OFN_ENABLETEMPLATE	hInstance �� lpTemplateName �����o�Ŏw�肳�ꂽ
�_�C�A���O�e���v���[�g���܂ރ��\�[�X�̃C���X�^���X�ł��邱�Ƃ�����
OFN_ENABLETEMPLATEHANDLE	hInstance �����o�����[�h�ς݂̃_�C�A���O�{�b�N�X�e���v���[�g���܂�
�������u���b�N���w���Ă��邱�Ƃ�\��
���̃t���O���w�肳��Ă���ꍇ�AlpTemplateName �͖��������
OFN_EXPLORER	�V�����G�N�X�v���[���X�^�C���̃_�C�A���O�{�b�N�X��
�J�X�^�������@��p���邱�Ƃ�����
OFN_FILEMUSTEXIST	�����̃t�@�C�����������͂������Ȃ�
OFN_PATHMUSTEXIST �t���O���܂�
OFN_HIDEREADONLY	�u�ǂݎ���p�v�`�F�b�N�{�b�N�X���B��
OFN_LONGNAMES	�Â��_�C�A���O�{�b�N�X�̂��߂ɁA�����t�@�C������p����
OFN_EXPLORER ���ݒ肳��Ă���ꍇ�͏�ɒ������O�ɂȂ�
OFN_NOCHANGEDIR	�_�C�A���O�{�b�N�X�́A���݂̃f�B���N�g����
�_�C�A���O�{�b�N�X���Ăяo���ꂽ���̃f�B���N�g���ɖ߂�
OFN_NODEREFERENCELINKS	�I�����ꂽ�V���[�g�J�b�g�t�@�C��(.LNK)�̃p�X�ƃt�@�C������
�Ԃ��悤�Ƀ_�C�A���O�{�b�N�X�Ɏw������
OFN_NOLONGNAMES	�u�t�@�C�����v���X�g�{�b�N�X�ɒ����t�@�C������\�����Ȃ�
OFN_NONETWORKBUTTON	�u�l�b�g���[�N�v�{�^�����B��
OFN_NOREADONLYRETURN	�Ԃ��ꂽ�t�@�C���ɑ΂���u�ǂݎ���p�v�`�F�b�N�{�b�N�X��
�`�F�b�N�}�[�N��t���Ȃ�
OFN_NOTESTFILECREATE	�_�C�A���O�{�b�N�X�����O�Ƀt�@�C�����쐬���Ȃ�
���̃t���O�́A�u�ύX�s�ō쐬�v�l�b�g���[�N���L�|�C���g���
�t�@�C����ۑ�����ꍇ�Ɏw�肷��
OFN_NOVALIDATE	�����ȕ������������t�@�C������L���Ƃ݂Ȃ�
OFN_OVERWRITEPROMPT	�ۑ����ɑI�����ꂽ�t�@�C�������݂���ꍇ
���b�Z�[�W�{�b�N�X���\������㏑�������邩�m�F����
OFN_PATHMUSTEXIST	�L���ȃp�X�y�уt�@�C�����łȂ���Γ��͂������Ȃ�
OFN_READONLY	�_�C�A���O�́u�ǂݎ���p�v�`�F�b�N�{�b�N�X���`�F�b�N���邱�Ƃ�\��
OFN_SHAREAWARE	�l�b�g���[�N���L�ᔽ�������� OpenFile() �֐��Ăяo�������s�����ꍇ��
�G���[�𖳎����ď���̃t�@�C������Ԃ�
���̃t���O���w�肳��Ă���ꍇ�ɂ� SHAREVISTRING �ɑ΂���
�o�^���b�Z�[�W�� lParam �p�����[�^�Ŏw�肳�ꂽ�p�X����уt�@�C�����ɑ΂���
NULL �ŏI��镶����̃|�C���^�Ƌ��Ƀt�b�N�֐��ɑ�����
�t�b�N�֐��͎��̂����ꂩ�ŉ������Ȃ���΂Ȃ�Ȃ�

OFN_SHAREFALLTHROUGH - �t�@�C������\������
OFN_SHARENOWARN - �A�N�V�����Ȃ�
OFN_SHAREWARN - �W���x�����b�Z�[�W���o��
OFN_SHOWHELP	�w���v�{�^����\������
���̃t���O��ݒ肷��ꍇ�A�e�E�B���h�E�������Ȃ���΂Ȃ�Ȃ�

nFileOffset �́AlpstrFile ���w����������ɂ�����p�X�̐擪����
�t�@�C�����܂ł� 0 ���琔�����I�t�Z�b�g��\���܂�

nFileExtension �́AlpstrFile ���w����������ɂ�����p�X�̐擪����
�t�@�C�����g���q�܂ł� 0 ���琔�����I�t�Z�b�g��\���܂�
lpstrDetExt �� NULL �ł���΁A�I�[�� NULL �����܂ł̃I�t�Z�b�g���w���܂�
���[�U�[���t�@�C�����̍Ō�̕����Ƃ��� "." ���w�肵���ꍇ�� 0 �ɂȂ�܂�

lpstrDefExt �ɂ́A�f�t�H���g�̊g���q��\��������ւ̃|�C���^���w�肵�܂�
���[�U�[���g���q����͂��Ȃ������ꍇ�A���̊g���q���p�����܂�

lCustData �́A�t�b�N�֐��ɓn���ǉ��f�[�^���w�肵�܂�
lpfnHook �ɂ́A�t�b�N�֐��ւ̃|�C���^���w�肵�܂�
lpTemplateName �́A�_�C�A���O�e���v���[�g���g�p����ꍇ�ɁA�e���v���[�g�����w�肵�܂�
*/


}

