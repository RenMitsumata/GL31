/*==============================================================================

   FPS�v���ƃt���[���Œ� [main.cpp]
														 Author : Youhei Sato
														 Date   : 2018/08/18
--------------------------------------------------------------------------------
GameProject18������FPS�v���ƃt���[���Œ�𓱓�����
==============================================================================*/
#include <Windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>
#include <string>
#include <map>
#include <vector>
#include <unordered_map>
#include "common.h"
#include "system_timer.h"
#include "texture.h"
#include "CModel.h"
#include "input.h"




#if WIN32
#pragma comment (lib,"OpenGL32.lib")
#pragma comment (lib,"GLU32.lib")
#pragma comment (lib,"assimp.lib")
#else
#pragma comment (lib,"OpenGL64.lib")
#pragma comment (lib,"GLU64.lib")
#endif

/*------------------------------------------------------------------------------
   �萔��`
------------------------------------------------------------------------------*/
#define CLASS_NAME     "GameWindow"       // �E�C���h�E�N���X�̖��O
#define WINDOW_CAPTION "�Q�[���E�B���h�E" // �E�B���h�E�̖��O
#define FPS_MEASUREMENT_TIME (1.0f)       // FPS�v������



/*------------------------------------------------------------------------------
   �v���g�^�C�v�錾
------------------------------------------------------------------------------*/
// �E�B���h�E�v���V�[�W��(�R�[���o�b�N�֐�)
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// �Q�[���̏������֐�
// �߂�l:�������Ɏ��s�����Ƃ�false
bool Initialize(void);
// �Q�[���̍X�V�֐�
void Update(void);
// �Q�[���̕`��֐�
void Draw(void);
// �Q�[���̏I������
void Finalize(void);
// �L���[�u�̕`�揈��
void DrawCube(void);



/*------------------------------------------------------------------------------
   �O���[�o���ϐ��錾
------------------------------------------------------------------------------*/
static HWND g_hWnd;                     // �E�B���h�E�n���h��
static int g_FrameCount = 0;            // �t���[���J�E���^�[
static int g_FPSBaseFrameCount = 0;     // FPS�v���̊�ƂȂ�t���[���J�E���g
static double g_FPSBaseTime = 0.0;      // FPS�v���̊�ƂȂ鎞��
static double g_StaticFrameTime = 0.0f; // �t���[���Œ�p�v������
static float g_FPS = 0.0f;              // FPS
static HDC g_HDC = NULL;
static HGLRC g_HGLRC = NULL;
static GLuint Texture;
static GLuint Texture2;
static GLuint Texture3;
static float xPos;
static float yPos;
static float zPos;
static float rotate;
static int count;
static const GLfloat lightCol[] = { 0.3f,0.3f,0.3f,1.0f };
static const GLfloat lightPos[] = { 0.0f,10.0f,10.0f,0.0f };
static const GLfloat lightDif[] = { 1.0f,1.0f,1.0f,1.0f };
static const GLfloat lightSpe[] = { 1.0f,1.0f,1.0f,1.0f };
static const GLfloat matDif[] = { 1.0f,1.0f,1.0f,1.0f };
static const GLfloat matCol[] = { 1.0f,1.0f,1.0f,1.0f };
static const GLfloat matSpe[] = { 1.0f,1.0f,1.0f,1.0f };
static const GLfloat matEmi[] = { 0.0f,0.0f,0.0f,0.0f };	// �G�~�b�V�u�F���Ȕ����@���i�͂O
static const GLfloat matShi = 0.0f;
std::map<char, GLfloat> CameraEye;
static GLuint* texture;
static bool readOnce;
static bool harinezumi;
static float canonFront = 0.0f;
static float bodyFront = 0.0f;
static int frame = 0;
static CModel Samba;


// std::vector<BONE> g_Bone;



static std::unordered_map<std::string, float> g_Angle;
/*------------------------------------------------------------------------------
   �֐���`
------------------------------------------------------------------------------*/

// ���C���֐�
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// �g�p���Ȃ��ꎞ�ϐ��𖾎�
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// �E�B���h�E�N���X�\���̂̐ݒ�
	WNDCLASS wc = {};
	wc.lpfnWndProc = WndProc;                          // �E�B���h�E�v���V�[�W���̎w��
	wc.lpszClassName = CLASS_NAME;                     // �N���X���̐ݒ�
	wc.hInstance = hInstance;                          // �C���X�^���X�n���h���̎w��
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);          // �}�E�X�J�[�\�����w��
	wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND + 1); // �E�C���h�E�̃N���C�A���g�̈�̔w�i�F��ݒ�

	// �N���X�o�^
	RegisterClass(&wc);


	// �E�B���h�E�X�^�C��
	DWORD window_style = WS_OVERLAPPEDWINDOW & ~(WS_MAXIMIZEBOX | WS_THICKFRAME);

	// ��{��`���W
	RECT window_rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

	// �w�肵���N���C�A���g�̈���m�ۂ��邽�߂ɐV���ȋ�`���W���v�Z
	AdjustWindowRect(&window_rect, window_style, FALSE);

	// �V����Window�̋�`���W���畝�ƍ������Z�o
	int window_width = window_rect.right - window_rect.left;
	int window_height = window_rect.bottom - window_rect.top;

	// �v���C�}�����j�^�[�̉�ʉ𑜓x�擾
	int desktop_width = GetSystemMetrics(SM_CXSCREEN);
	int desktop_height = GetSystemMetrics(SM_CYSCREEN);

	// �f�X�N�g�b�v�̐^�񒆂ɃE�B���h�E�����������悤�ɍ��W���v�Z
	// ��������������A�f�X�N�g�b�v���E�B���h�E���傫���ꍇ�͍���ɕ\��
	int window_x = max((desktop_width - window_width) / 2, 0);
	int window_y = max((desktop_height - window_height) / 2, 0);

	// �E�B���h�E�̐���
	g_hWnd = CreateWindow(
		CLASS_NAME,     // �E�B���h�E�N���X
		WINDOW_CAPTION, // �E�B���h�E�e�L�X�g
		window_style,   // �E�B���h�E�X�^�C��
		window_x,       // �E�B���h�E���Wx
		window_y,       // �E�B���h�E���Wy
		window_width,   // �E�B���h�E�̕�
		window_height,  // �E�B���h�E�̍���
		NULL,           // �e�E�B���h�E�n���h��
		NULL,           // ���j���[�n���h��
		hInstance,      // �C���X�^���X�n���h��
		NULL            // �ǉ��̃A�v���P�[�V�����f�[�^
	);

	if( g_hWnd == NULL ) {
		// �E�B���h�E�n���h�������炩�̗��R�Ő����o���Ȃ�����
		return -1;
	}

	// �w��̃E�B���h�E�n���h���̃E�B���h�E���w��̕��@�ŕ\��
	ShowWindow(g_hWnd, nCmdShow);


	// �Q�[���̏�����(Direct3D�̏�����)
	if( !Initialize() ) {
		// �Q�[���̏������Ɏ��s����
		return -1;
	}
	readOnce = false;

	xPos = 0.0f;
	yPos = 0.0f;
	zPos = 0.0f;
	rotate = 0.0f;
	count = 0;

	// Windows�Q�[���p���C�����[�v
	MSG msg = {}; // msg.message == WM_NULL
	while( WM_QUIT != msg.message ) {
		
		if( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) ) {
			// ���b�Z�[�W������ꍇ�̓��b�Z�[�W������D��
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			// ���݂̃V�X�e�����Ԃ��擾
			double time = SystemTimer_GetTime();

			if( time - g_StaticFrameTime < 1.0 / 60.0 ) {
				// 1 / 60 �b�o���Ă��Ȃ����������
				Sleep(0);
			}
			else {
				// �t���[���Œ�p�̌v�����Ԃ��X�V����
				g_StaticFrameTime = time;

				// �Q�[���̍X�V
				Update();
				// �Q�[���̕`��
				Draw();
			}
		}
	}

	// �Q�[���̏I������(Direct3D�̏I������)
	Finalize();

	return (int)msg.wParam;
}


// �E�B���h�E�v���V�[�W��(�R�[���o�b�N�֐�)
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch( uMsg ) {
		case WM_KEYDOWN:
			if( wParam == VK_ESCAPE ) {
				SendMessage(hWnd, WM_CLOSE, 0, 0); // WM_CLOSE���b�Z�[�W�̑��M
			}
			break;

		case WM_CLOSE:
			if( MessageBox(hWnd, "�{���ɏI�����Ă�낵���ł����H", "�m�F", MB_OKCANCEL | MB_DEFBUTTON2) == IDOK ) {
				DestroyWindow(hWnd); // �w��̃E�B���h�E��WM_DESTROY���b�Z�[�W�𑗂�
			}
			return 0; // DefWindowProc�֐��Ƀ��b�Z�[�W�𗬂����I�����邱�Ƃɂ���ĉ����Ȃ��������Ƃɂ���

		case WM_DESTROY: // �E�B���h�E�̔j�����b�Z�[�W
			PostQuitMessage(0); // WM_QUIT���b�Z�[�W�̑��M
			return 0;
	};

	// �ʏ탁�b�Z�[�W�����͂��̊֐��ɔC����
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


// �Q�[���̏������֐�
bool Initialize(void)
{
	// �V�X�e���^�C�}�[�̏�����
	SystemTimer_Initialize();

	// �V�X�e���^�C�}�[�̋N��
	SystemTimer_Start();

	// FPS�v���p�ϐ��Q�̏�����
	g_FrameCount = g_FPSBaseFrameCount = 0;
	g_FPSBaseTime = SystemTimer_GetTime();
	g_FPS = 0.0f;

	// �t���[���Œ�p�v������
	g_StaticFrameTime = g_FPSBaseTime;


	// OpenGL�̏�����
	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		(PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER),
		PFD_TYPE_RGBA,
		24,
		0,0,0,0,0,0,0,0,0,0,0,0,0,	// 13��
		32,0,0,
		PFD_MAIN_PLANE,
		0,0,0,0
	};

	// Window�f�o�C�X�R���e�L�X�g�̎擾
	g_HDC = GetDC(g_hWnd);
	// PixelFormat�̎擾
	int pixelFormat = ChoosePixelFormat(g_HDC, &pfd);
	SetPixelFormat(g_HDC, pixelFormat, &pfd);
	// GL�R���e�L�X�g�̎擾
	g_HGLRC = wglCreateContext(g_HDC);
	wglMakeCurrent(g_HDC, g_HGLRC);

	// OpenGL�̕`��ݒ�
	glEnable(GL_NORMALIZE);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_LIGHTING);
	
	// ���C�g�̐ݒ�
	
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightCol);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDif);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpe);
	glEnable(GL_LIGHT0);

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matCol);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matDif);
	//glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpe);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, matShi);
	glEnable(GL_FRONT_AND_BACK);
	
	
	
	
	Samba.Init("asset/model/Walking3.fbx");
	


	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Texture = LoadTexture("asset/texture/BG_Space.jpg",FILETYPE_JPEG);
	Texture2 = LoadTexture("asset/texture/wall.tga", FILETYPE_TGA);
	Texture3 = LoadTexture("asset/texture/dice.tga", FILETYPE_TGA);
	CInput::Init();
	harinezumi = false;
	return true;
}

// �Q�[���̍X�V�֐�
void Update(void)
{
	CInput::Update();
	// �t���[���J�E���g�̍X�V
	g_FrameCount++;
	
		frame++;

	
	// ���݂̃V�X�e�����Ԃ��擾
	double time = SystemTimer_GetTime();

	// �O���FPS�v�����Ԃ���K�莞�Ԍo���Ă�����FPS�v��
	if( time - g_FPSBaseTime >= FPS_MEASUREMENT_TIME ) {

		// FPS�v�Z(�O�񂩂�̌o�߃t���[�������o�ߎ���)
		g_FPS = (float)((g_FrameCount - g_FPSBaseFrameCount) / (time - g_FPSBaseTime));
		// FPS�v���̊�ƂȂ�ϐ��̍X�V
		g_FPSBaseTime = time;
		g_FPSBaseFrameCount = g_FrameCount;
	}
	CameraEye['X'] += 0.0f;

	CameraEye['Y'] += 0.0f;
	
	Samba.Update();
	
	
	if (CInput::GetKeyTrigger('N')) {
		if (harinezumi) {
			harinezumi = false;
		}
		else harinezumi = true;
	}
	
	if (CInput::GetKeyPress('A')) {
	//	canonFront += 0.5f;
		g_Angle["Canon"] += 0.5f;
	}
	if (CInput::GetKeyPress('D')) {
	//	canonFront -= 0.5f;
		g_Angle["Canon"] -= 0.5f;
	}
	if (CInput::GetKeyPress(VK_NUMPAD4)) {
		bodyFront += 0.5f;
	}
	if (CInput::GetKeyPress(VK_NUMPAD6)) {
		bodyFront -= 0.5f;
	}
	
	

	count++;
}

// �Q�[���̕`��֐�
void Draw(void)
{
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);	//�@�o�b�N�o�b�t�@�̃N���A�̐ݒ�
	glClearDepth(1.0f);	//�@��Ԑ[���Ƃ���ŏ�����
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	//�@�����Ńo�b�N�o�b�t�@���N���A�����
	/*
	// ���C�e�B���O�̖�����(2�����`��̎��̂�)
	glDisable(GL_LIGHTING);
	// 2D�s��ϊ��i�v���W�F�N�V�����j
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 1);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glBindTexture(GL_TEXTURE_2D, Texture);

	glBegin(GL_TRIANGLE_STRIP);
	//	��������`�揈��������  


	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(500.0f, 100.0f, 0.0f);
	
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(100.0f, 100.0f, 0.0f);
	
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(500.0f, 400.0f, 0.0f);
		
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(100.0f, 400.0f, 0.0f);

	glEnd();

	glBindTexture(GL_TEXTURE_2D, Texture2);
	glBegin(GL_TRIANGLE_STRIP);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(900.0f, 300.0f, 0.0f);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(500.0f, 300.0f, 0.0f);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(900.0f, 550.0f, 0.0f);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(500.0f, 550.0f, 0.0f);


	glBindTexture(GL_TEXTURE_2D, 0);

	// �`�揈���I���@
	glEnd();
	glEnable(GL_LIGHTING);
	*/
	//glDisable(GL_LIGHT0);

	// �r���[�|�[�g
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f,(SCREEN_WIDTH / (float)SCREEN_HEIGHT) , 1.0f, 600.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	gluLookAt(0.0f,300.0f, 200.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.7f, -0.7f);

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matCol);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matDif);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, matShi);

	/*
	// �s����v�b�V������
	glPushMatrix();

	
	glBindTexture(GL_TEXTURE_2D, Texture);
	glBegin(GL_TRIANGLE_STRIP);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glNormal3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(10.0f, 0.0f);
	glVertex3f(2.5f, 0.0f, -2.5f);
	

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glNormal3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-2.5f, 0.0f, -2.5f);
	

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glNormal3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(10.0f, 10.0f);
	glVertex3f(2.5f, 0.0f, 2.5f);
	

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glNormal3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(0.0f, 10.0f);
	glVertex3f(-2.5f, 0.0f, 2.5f);
	
	

	// �`�揈���I���@
	glEnd();

	// �s����|�b�v����
	glPopMatrix();
	*/

	

	//-----��������L���[�u�̕`�揈��-----//
	glBindTexture(GL_TEXTURE_2D, Texture3);
	  
	/*
	//
	//�@�@�[�ł��邮��
	//

	// �s����v�b�V������
	glPushMatrix();

	// �L���[�u�̃��[���h���W�ϊ�
	glTranslatef(-2.0f,0.5f,2.0f);
	glRotatef((count * 5.0f), 0.0f, 1.0f, 0.0f);
	// �L���[�u��`��
	DrawCube();
	
	// �s����|�b�v����
	glPopMatrix();


	//
	//�@�A�[�ŏ㉺�ړ�
	//

	// �s����v�b�V������
	glPushMatrix();

	// �L���[�u�̃��[���h���W�ϊ�
	glTranslatef(-2.0f, 1.5f + sinf(count / 8.0f), -2.0f);
	// �L���[�u��`��
	DrawCube();

	// �s����|�b�v����
	glPopMatrix();

	//
	//�@�B�[�Ŋg��k��
	//

	// �s����v�b�V������
	glPushMatrix();

	// �L���[�u�̃��[���h���W�ϊ�
	glTranslatef(2.5f - (1.0f + sinf(count / 8.0f) / 2) / 2, (1.0f + sinf(count / 8.0f) / 2) / 2, -2.5f + (1.0f + sinf(count / 8.0f) / 2) / 2);
	glScalef(1.0f + sinf(count / 8.0f) / 2, 1.0f + sinf(count / 8.0f) / 2, 1.0f + sinf(count / 8.0f) / 2);

	// �L���[�u��`��
	DrawCube();

	// �s����|�b�v����
	glPopMatrix();

	//
	//�@�C��]���Ȃ���ړ�
	//

	// �s����v�b�V������
	glPushMatrix();

	// �L���[�u�̃��[���h���W�ϊ�
	glTranslatef(cosf(count / 10.0f) * 1.0f,1.0f,sinf(count / 10.0f)* 1.0f);
	glRotatef(count * 5, 1.0f, 1.0f, -1.0f);

	// �L���[�u��`��
	DrawCube();

	// �s����|�b�v����
	glPopMatrix();


	*/

	frame++;



	// �s��̖ʓ|

	// ���b�V���̖ʓ|

	// �}�e���A���̖ʓ|
	glPushMatrix();
	//glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	//glScalef(0.01f, 0.01f, 0.01f);
	//glTranslatef(0.0f, 100.0f, 200.0f);
	//DrawChildrens(g_pScene->mRootNode);
	Samba.Draw();
	glPopMatrix();
	/*
	// �n���l�Y�~
	if (harinezumi) {

		glPushMatrix();
		glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
		glBegin(GL_LINES);
		
		for (normalList nrm : _Normallist) {
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
			glNormal3f(0.0f, 0.0f, 1.0f);
			glVertex3f(nrm.pos.x, nrm.pos.y, nrm.pos.z);

			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
			glNormal3f(0.0f, 0.0f, 1.0f);
			glVertex3f(nrm.pos.x + nrm.normalVec.x * 3.0f, nrm.pos.y + nrm.normalVec.y * 3.0f, nrm.pos.z + nrm.normalVec.z * 3.0f);
		}
		
		glEnd();
		glPopMatrix();

	}
	*/
	glBindTexture(GL_TEXTURE_2D, 0);
	//glEnable(GL_LIGHT0);

	SwapBuffers(g_HDC);

}

// �Q�[���̏I������
void Finalize(void)
{
	CInput::Uninit();
	g_Angle.clear();
	Samba.Uninit();
	// OpenGL�̌ォ������
	DeleteTexture();
	wglMakeCurrent(NULL, NULL);
	ReleaseDC(g_hWnd, g_HDC);
	wglDeleteContext(g_HGLRC);

}









void DrawCube(void) {
	glPushMatrix();
	for (int i = 0; i < 4; i++) {
		glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
		glBegin(GL_TRIANGLE_STRIP);

		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glNormal3f(0.0f, 1.0f, 0.0f);
		glTexCoord2f((i+1)*0.25f, 0.33f);
		glVertex3f(0.5f, 0.5f, -0.5f);

		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glNormal3f(0.0f, 1.0f, 0.0f);
		glTexCoord2f(i*0.25f, 0.33f);
		glVertex3f(-0.5f, 0.5f, -0.5f);

		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glNormal3f(0.0f, 1.0f, 0.0f);
		glTexCoord2f((i + 1)*0.25f, 0.66f);
		glVertex3f(0.5f, 0.5f, 0.5f);

		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glNormal3f(0.0f, 1.0f, 0.0f);
		glTexCoord2f(i*0.25f, 0.66f);
		glVertex3f(-0.5f, 0.5f, 0.5f);
		glEnd();
	}
	glPopMatrix();

	glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
	glBegin(GL_TRIANGLE_STRIP);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glNormal3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(0.5f, 0.0f);
	glVertex3f(0.5f, 0.5f, -0.5f);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glNormal3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(0.25f, 0.0f);
	glVertex3f(-0.5f, 0.5f, -0.5f);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glNormal3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(0.5f, 0.33f);
	glVertex3f(0.5f, 0.5f, 0.5f);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glNormal3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(0.25f, 0.33f);
	glVertex3f(-0.5f, 0.5f, 0.5f);

	glEnd();

	glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
	glBegin(GL_TRIANGLE_STRIP);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glNormal3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(0.5f, 0.66f);
	glVertex3f(0.5f, 0.5f, -0.5f);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glNormal3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(0.25f, 0.66f);
	glVertex3f(-0.5f, 0.5f, -0.5f);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glNormal3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(0.5f, 1.0f);
	glVertex3f(0.5f, 0.5f, 0.5f);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glNormal3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(0.25f, 1.0f);
	glVertex3f(-0.5f, 0.5f, 0.5f);

	glEnd();

}









/*
 �s��X�^�b�N
  glLoad�`�@�X�^�b�N���N���A���A1�ԏ�ɍs���ς�
   gluLookat
   gluPerspective
   glTranslatef
   glRotatef
   glScalef
   glMultiMatrixf

   �J�����g�̈ړ�
   glPushMatrix()
   �J�����g���R�s�[���ĉ���
   glPopMatrix()
   �J�����g�����

   glMatrixMode��MODELVIEW��Projection�ɕ�����Ă��遨�X�^�b�N����

   �@�P�ʍs������
   glMatrixIdentity();
   �A�r���[�s������
   glLookAt();
   �B�v�b�V������
   glPushMatrix();
   �C��̕`�悪�I�������|�b�v���Ă�
   glPopMatrix();
   �C���[�W�Ƃ��Ă�
   �r���[�s����R�s�[���J�����g�ړ����L���[�u��`�恨�|�b�v�ŃL���[�u�`��p�̍s��Ͳ��
   ���r���[�s�񂪎c���Ă邩��ė��p�ł���I���܂��v�b�V�����āc���J��Ԃ�
   ���f���̐e�q�\�������₷���I

*/

/*
	�ۑ�02
	�E�n�ʂ̒[�ł��̏��]
	�E�n�ʂ̒[�ŏ㉺�ړ�
	�E�n�ʂ̒[�Ŋg��k��
	�E��]���Ȃ���ړ�
*/

/*
	���C�g�̐ݒ�
	���C�g�̍��W�A�A���r�G���g�E�f�B�t���[�Y�E�X�y�L����
	W���W��0:�^���I���s����
		1:�_����
*/

/*
 ����̍ċN�֐����m�[�h�����}�b�v�ɕۑ����Ă���
	std::unordered_map<std::string,float> g_Angles;
	g_Angles["�m�[�h��"] = 0.0f;
	if(){
		g_Angles["�m�[�h��"]+=1.0f;
	}
*/