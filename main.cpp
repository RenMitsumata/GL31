/*==============================================================================

   FPS計測とフレーム固定 [main.cpp]
                                                         Author : Youhei Sato
                                                         Date   : 2018/08/18
--------------------------------------------------------------------------------
GameProject18を元にFPS計測とフレーム固定を導入する
==============================================================================*/
#include <Windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>
#include <string>
#include "common.h"
#include "system_timer.h"
#include "texture.h"
#include "input.h"
#include <map>
#include <vector>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/matrix4x4.h>


#if WIN32
#pragma comment (lib,"OpenGL32.lib")
#pragma comment (lib,"GLU32.lib")
#pragma comment (lib,"assimp.lib")
#else
#pragma comment (lib,"OpenGL64.lib")
#pragma comment (lib,"GLU64.lib")
#endif

/*------------------------------------------------------------------------------
   定数定義
------------------------------------------------------------------------------*/
#define CLASS_NAME     "GameWindow"       // ウインドウクラスの名前
#define WINDOW_CAPTION "ゲームウィンドウ" // ウィンドウの名前
#define FPS_MEASUREMENT_TIME (1.0f)       // FPS計測時間

/*------------------------------------------------------------------------------
   プロトタイプ宣言
------------------------------------------------------------------------------*/
// ウィンドウプロシージャ(コールバック関数)
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// ゲームの初期化関数
// 戻り値:初期化に失敗したときfalse
static bool Initialize(void);
// ゲームの更新関数
static void Update(void);
// ゲームの描画関数
static void Draw(void);
// ゲームの終了処理
static void Finalize(void);
// キューブの描画処理
static void DrawCube(void);

static void DrawMesh(const aiScene* pScene);


/*------------------------------------------------------------------------------
   グローバル変数宣言
------------------------------------------------------------------------------*/
static HWND g_hWnd;                     // ウィンドウハンドル
static int g_FrameCount = 0;            // フレームカウンター
static int g_FPSBaseFrameCount = 0;     // FPS計測の基となるフレームカウント
static double g_FPSBaseTime = 0.0;      // FPS計測の基となる時間
static double g_StaticFrameTime = 0.0f; // フレーム固定用計測時間
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
static const GLfloat matEmi[] = { 0.0f,0.0f,0.0f,0.0f };	// エミッシブ：自己発光　普段は０
static const GLfloat matShi = 0.0f;
std::map<char, GLfloat> CameraEye;
static const aiScene* g_pScene = nullptr;
static GLuint* texture;
static bool readOnce;
static bool harinezumi;
struct normalList {
	aiVector3D normalVec;
	aiVector3D pos;
};

static std::vector<normalList> _Normallist;
/*------------------------------------------------------------------------------
   関数定義
------------------------------------------------------------------------------*/

// メイン関数
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // 使用しない一時変数を明示
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // ウィンドウクラス構造体の設定
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;                          // ウィンドウプロシージャの指定
    wc.lpszClassName = CLASS_NAME;                     // クラス名の設定
    wc.hInstance = hInstance;                          // インスタンスハンドルの指定
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);          // マウスカーソルを指定
    wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND + 1); // ウインドウのクライアント領域の背景色を設定

    // クラス登録
    RegisterClass(&wc);


    // ウィンドウスタイル
    DWORD window_style = WS_OVERLAPPEDWINDOW & ~(WS_MAXIMIZEBOX | WS_THICKFRAME);

    // 基本矩形座標
    RECT window_rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

    // 指定したクライアント領域を確保するために新たな矩形座標を計算
    AdjustWindowRect(&window_rect, window_style, FALSE);

    // 新たなWindowの矩形座標から幅と高さを算出
    int window_width = window_rect.right - window_rect.left;
    int window_height = window_rect.bottom - window_rect.top;

    // プライマリモニターの画面解像度取得
    int desktop_width = GetSystemMetrics(SM_CXSCREEN);
    int desktop_height = GetSystemMetrics(SM_CYSCREEN);

    // デスクトップの真ん中にウィンドウが生成されるように座標を計算
    // ※ただし万が一、デスクトップよりウィンドウが大きい場合は左上に表示
    int window_x = max((desktop_width - window_width) / 2, 0);
    int window_y = max((desktop_height - window_height) / 2, 0);

    // ウィンドウの生成
    g_hWnd = CreateWindow(
        CLASS_NAME,     // ウィンドウクラス
        WINDOW_CAPTION, // ウィンドウテキスト
        window_style,   // ウィンドウスタイル
        window_x,       // ウィンドウ座標x
        window_y,       // ウィンドウ座標y
        window_width,   // ウィンドウの幅
        window_height,  // ウィンドウの高さ
        NULL,           // 親ウィンドウハンドル
        NULL,           // メニューハンドル
        hInstance,      // インスタンスハンドル
        NULL            // 追加のアプリケーションデータ
    );

    if( g_hWnd == NULL ) {
        // ウィンドウハンドルが何らかの理由で生成出来なかった
        return -1;
    }

    // 指定のウィンドウハンドルのウィンドウを指定の方法で表示
    ShowWindow(g_hWnd, nCmdShow);


	// ゲームの初期化(Direct3Dの初期化)
	if( !Initialize() ) {
        // ゲームの初期化に失敗した
		return -1;
	}
	readOnce = false;

	xPos = 0.0f;
	yPos = 0.0f;
	zPos = 0.0f;
	rotate = 0.0f;
	count = 0;

    // Windowsゲーム用メインループ
    MSG msg = {}; // msg.message == WM_NULL
    while( WM_QUIT != msg.message ) {
        
		if( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) ) {
            // メッセージがある場合はメッセージ処理を優先
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
		else {
			// 現在のシステム時間を取得
			double time = SystemTimer_GetTime();

			if( time - g_StaticFrameTime < 1.0 / 60.0 ) {
			    // 1 / 60 秒経っていなかったら空回り
				Sleep(0);
			}
			else {
				// フレーム固定用の計測時間を更新する
				g_StaticFrameTime = time;

			    // ゲームの更新
				Update();
				// ゲームの描画
				Draw();
			}
        }
    }

	// ゲームの終了処理(Direct3Dの終了処理)
	Finalize();

    return (int)msg.wParam;
}


// ウィンドウプロシージャ(コールバック関数)
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch( uMsg ) {
        case WM_KEYDOWN:
            if( wParam == VK_ESCAPE ) {
                SendMessage(hWnd, WM_CLOSE, 0, 0); // WM_CLOSEメッセージの送信
            }
            break;

        case WM_CLOSE:
            if( MessageBox(hWnd, "本当に終了してよろしいですか？", "確認", MB_OKCANCEL | MB_DEFBUTTON2) == IDOK ) {
                DestroyWindow(hWnd); // 指定のウィンドウにWM_DESTROYメッセージを送る
            }
            return 0; // DefWindowProc関数にメッセージを流さず終了することによって何もなかったことにする

        case WM_DESTROY: // ウィンドウの破棄メッセージ
            PostQuitMessage(0); // WM_QUITメッセージの送信
            return 0;
    };

    // 通常メッセージ処理はこの関数に任せる
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


// ゲームの初期化関数
bool Initialize(void)
{
	// システムタイマーの初期化
	SystemTimer_Initialize();

	// システムタイマーの起動
	SystemTimer_Start();

	// FPS計測用変数群の初期化
	g_FrameCount = g_FPSBaseFrameCount = 0;
	g_FPSBaseTime = SystemTimer_GetTime();
	g_FPS = 0.0f;

	// フレーム固定用計測時間
	g_StaticFrameTime = g_FPSBaseTime;


	// OpenGLの初期化
	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		(PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER),
		PFD_TYPE_RGBA,
		24,
		0,0,0,0,0,0,0,0,0,0,0,0,0,	// 13個
		32,0,0,
		PFD_MAIN_PLANE,
		0,0,0,0
	};

	// Windowデバイスコンテキストの取得
	g_HDC = GetDC(g_hWnd);
	// PixelFormatの取得
	int pixelFormat = ChoosePixelFormat(g_HDC, &pfd);
	SetPixelFormat(g_HDC, pixelFormat, &pfd);
	// GLコンテキストの取得
	g_HGLRC = wglCreateContext(g_HDC);
	wglMakeCurrent(g_HDC, g_HGLRC);

	// OpenGLの描画設定
	glEnable(GL_NORMALIZE);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_LIGHTING);
	
	// ライトの設定
	
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
	
//g_pScene = aiImportFile("asset/model/Pronama-chan_Ver3/FBX(Ver.3)/PronamaChan.fbx", aiProcessPreset_TargetRealtime_MaxQuality);
	//g_pScene = aiImportFile("asset/model/test.fbx", aiProcessPreset_TargetRealtime_MaxQuality);
	//g_pScene = aiImportFile("asset/model/AC Cobra/Shelby.obj", aiProcessPreset_TargetRealtime_MaxQuality);
	//g_pScene = aiImportFile("asset/model/tank/tank.x", aiProcessPreset_TargetRealtime_MaxQuality);
	//g_pScene = aiImportFile("asset/model/coaster.fbx", aiProcessPreset_TargetRealtime_MaxQuality);
	g_pScene = aiImportFile("asset/model/dragon/Dragon 2.5_fbx.fbx", aiProcessPreset_TargetRealtime_MaxQuality);
	
	if (g_pScene == nullptr) {
		MessageBox(g_hWnd, "モデルファイルが読み込めません", "Assimp", MB_OK | MB_ICONHAND);
		exit(1);
	}

	int material = g_pScene->mNumMaterials;
	texture = new GLuint[material];

	for (int i = 0; i < material; i++) {
		aiString path;
		

		g_pScene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, 0, &path);
		// マテリアルに画像がある
		std::string texPath = path.data;
		size_t pos = texPath.find_last_of("\\/");
		std::string headerPath = texPath.substr(0, pos + 1);
		headerPath += path.data;
		texPath.c_str();	// stringの先頭アドレスを取得できる
		texture[i] = LoadTexture(headerPath.c_str(), 2);
	}
	
	/*
		unorderedmap
	
	*/

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

// ゲームの更新関数
void Update(void)
{
	CInput::Update();
	// フレームカウントの更新
	g_FrameCount++;

	// 現在のシステム時間を取得
	double time = SystemTimer_GetTime();

	// 前回のFPS計測時間から規定時間経っていたらFPS計測
	if( time - g_FPSBaseTime >= FPS_MEASUREMENT_TIME ) {

		// FPS計算(前回からの経過フレーム数÷経過時間)
		g_FPS = (float)((g_FrameCount - g_FPSBaseFrameCount) / (time - g_FPSBaseTime));
		// FPS計測の基となる変数の更新
		g_FPSBaseTime = time;
		g_FPSBaseFrameCount = g_FrameCount;
	}
	CameraEye['X'] += 0.0f;

	CameraEye['Y'] += 0.0f;
	

	if (CInput::GetKeyTrigger('N')) {
		if (harinezumi) {
			harinezumi = false;
		}
		else harinezumi = true;
	}

	count++;
}

// ゲームの描画関数
void Draw(void)
{
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);	//　バックバッファのクリアの設定
	glClearDepth(1.0f);	//　一番深いところで初期化
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	//　ここでバックバッファがクリアされる
	/*
	// ライティングの無効化(2次元描画の時のみ)
	glDisable(GL_LIGHTING);
	// 2D行列変換（プロジェクション）
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 1);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glBindTexture(GL_TEXTURE_2D, Texture);

	glBegin(GL_TRIANGLE_STRIP);
	//	ここから描画処理を書く  


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

	// 描画処理終了　
	glEnd();
	glEnable(GL_LIGHTING);
	*/
	//glDisable(GL_LIGHT0);

	// ビューポート
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f,(SCREEN_WIDTH / (float)SCREEN_HEIGHT) , 1.0f, 600.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	gluLookAt(0.0f,50.0f, 50.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.7f, -0.7f);


	


	
	


	



	// 行列をプッシュする
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
	
	

	// 描画処理終了　
	glEnd();

	// 行列をポップする
	glPopMatrix();


	//-----ここからキューブの描画処理-----//
	glBindTexture(GL_TEXTURE_2D, Texture3);
	  

	//
	//　①端でぐるぐる
	//

	// 行列をプッシュする
	glPushMatrix();

	// キューブのワールド座標変換
	glTranslatef(-2.0f,0.5f,2.0f);
	glRotatef((count * 5.0f), 0.0f, 1.0f, 0.0f);
	// キューブを描画
	DrawCube();
	
	// 行列をポップする
	glPopMatrix();


	//
	//　②端で上下移動
	//

	// 行列をプッシュする
	glPushMatrix();

	// キューブのワールド座標変換
	glTranslatef(-2.0f, 1.5f + sinf(count / 8.0f), -2.0f);
	// キューブを描画
	DrawCube();

	// 行列をポップする
	glPopMatrix();

	//
	//　③端で拡大縮小
	//

	// 行列をプッシュする
	glPushMatrix();

	// キューブのワールド座標変換
	glTranslatef(2.5f - (1.0f + sinf(count / 8.0f) / 2) / 2, (1.0f + sinf(count / 8.0f) / 2) / 2, -2.5f + (1.0f + sinf(count / 8.0f) / 2) / 2);
	glScalef(1.0f + sinf(count / 8.0f) / 2, 1.0f + sinf(count / 8.0f) / 2, 1.0f + sinf(count / 8.0f) / 2);

	// キューブを描画
	DrawCube();

	// 行列をポップする
	glPopMatrix();

	//
	//　④回転しながら移動
	//

	// 行列をプッシュする
	glPushMatrix();

	// キューブのワールド座標変換
	glTranslatef(cosf(count / 10.0f) * 1.0f,1.0f,sinf(count / 10.0f)* 1.0f);
	glRotatef(count * 5, 1.0f, 1.0f, -1.0f);

	// キューブを描画
	DrawCube();

	// 行列をポップする
	glPopMatrix();


	// 行列の面倒

	// メッシュの面倒

	// マテリアルの面倒



	DrawMesh(g_pScene);
	
	
	// ハリネズミ
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

	glBindTexture(GL_TEXTURE_2D, 0);
	//glEnable(GL_LIGHT0);

	SwapBuffers(g_HDC);

}

// ゲームの終了処理
void Finalize(void)
{
	CInput::Uninit();
	if (g_pScene != nullptr) {
		aiReleaseImport(g_pScene);
	}
	// OpenGLの後かたずけ
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



void DrawMesh(const aiScene* pScene) {
	void DrawChildrens(aiNode* pNode, const aiScene* pScene);

	aiNode* pNode = g_pScene->mRootNode;			// ルートノード（一番上の親パーツ）を取ってくる
	glPushMatrix();
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	//glScalef(1.0f, 3.0f, 1.0f);
	//glRotatef(270.0f, 0.0f, 1.0f, 0.0f);
	//glTranslatef(0.0f, 3.0f, 3.0f);
	

	glBegin(GL_TRIANGLES);
	

	DrawChildrens(pNode,pScene);

	glEnd();
	aiMatrix4x4 matrix = pNode->mTransformation;	// 行列（位置）を取ってくる
	aiTransposeMatrix4(&matrix);					// 行列を転置する(DirectX(左手系)->openGL(右手系))
	glMultMatrixf((float*)&matrix);					// 行列をfloatに
	glBindTexture(GL_TEXTURE_2D, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matCol);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matDif);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, matShi);
	glPopMatrix();
	readOnce = true;
}

void DrawChildrens(aiNode* pNode, const aiScene* g_pScene) {
	glPushMatrix();
	aiMatrix4x4 matrix = pNode->mTransformation;	// 行列（位置）を取ってくる
	aiTransposeMatrix4(&matrix);					// 行列を転置する(DirectX(左手系)->openGL(右手系))
	
	glMultMatrixf((float*)&matrix);					// 行列をfloatに
	for (int n = 0; n < pNode->mNumChildren; n++) {
		DrawChildrens(pNode->mChildren[n], g_pScene);
	}
	for (int n = 0; n < pNode->mNumMeshes; n++) {
		const aiMesh* pMesh = g_pScene->mMeshes[pNode->mMeshes[n]];
		const aiMaterial* mat = g_pScene->mMaterials[pMesh->mMaterialIndex];

		glBindTexture(GL_TEXTURE_2D, texture[pMesh->mMaterialIndex]);
		aiColor4D diffuse;
		
		aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &diffuse);
		GLfloat glDiffuse[] = { diffuse.r,diffuse.g,diffuse.b,diffuse.a };
		
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, (float*)glDiffuse);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, (float*)glDiffuse);
		


		for (int fn = 0; fn < pMesh->mNumFaces; fn++) {
			const aiFace* pFace = &pMesh->mFaces[fn];
			for (int i = 0; i < pFace->mNumIndices; i++) {
				int index = pFace->mIndices[i];
				
				

				glColor4f(glDiffuse[0],glDiffuse[1],glDiffuse[2],glDiffuse[3]);
				glNormal3f(pMesh->mNormals[index].x, pMesh->mNormals[index].y, pMesh->mNormals[index].z);
				
				
				if (pMesh->HasTextureCoords(0)) {
					glTexCoord2f(pMesh->mTextureCoords[0][index].x, pMesh->mTextureCoords[0][index].y);	// １頂点に複数のtexcoordを持っているなら、forで回す
				}

				glVertex3f(pMesh->mVertices[index].x, pMesh->mVertices[index].y, pMesh->mVertices[index].z);
				

				if (!readOnce) {

					normalList pushlist = {
					aiVector3D({ pMesh->mNormals[index].x, pMesh->mNormals[index].y, pMesh->mNormals[index].z }),
					aiVector3D({pMesh->mVertices[index].x, pMesh->mVertices[index].y, pMesh->mVertices[index].z})

					};
					_Normallist.push_back(pushlist);
				}
				
			}
		}

	}
	glPopMatrix();
}
/*
	マテリアル情報はaiSceneのaiMeshごと

*/



/*
　行列スタック
 　glLoad～　スタックをクリアし、1番上に行列を積む
   gluLookat
   gluPerspective
   glTranslatef
   glRotatef
   glScalef
   glMultiMatrixf

   カレントの移動
   glPushMatrix()
   カレントをコピーして下へ
   glPopMatrix()
   カレントを上へ

   glMatrixModeがMODELVIEWとProjectionに分かれている→スタックが別

   ①単位行列を作る
   glMatrixIdentity();
   ②ビュー行列を作る
   glLookAt();
   ③プッシュする
   glPushMatrix();
   ④一つの描画が終わったらポップを呼ぶ
   glPopMatrix();
   イメージとしては
   ビュー行列をコピー＆カレント移動→キューブを描画→ポップでキューブ描画用の行列はｲﾗﾈ
   →ビュー行列が残ってるから再利用できる！→またプッシュして…を繰り返し
   モデルの親子構造も作りやすい！

*/

/*
	課題02
	・地面の端でその場回転
	・地面の端で上下移動
	・地面の端で拡大縮小
	・回転しながら移動
*/

/*
	ライトの設定
	ライトの座標、アンビエント・ディフューズ・スペキュラ
	W座標に0:疑似的平行光源
	　　　 1:点光源
*/