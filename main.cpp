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
#include "common.h"
#include "system_timer.h"
#include "texture.h"

#if WIN32
#pragma comment (lib,"OpenGL32.lib")
#pragma comment (lib,"GLU32.lib")
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
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Texture = LoadTexture("asset/texture/kouya.tga");
	Texture2 = LoadTexture("asset/texture/keion_bg.tga");
    return true;
}

// ゲームの更新関数
void Update(void)
{

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

	
}

// ゲームの描画関数
void Draw(void)
{
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);	//　バックバッファのクリアの設定
	glClearDepth(1.0f);	//　一番深いところで初期化
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	//　ここでバックバッファがクリアされる

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
	/*	ここから描画処理を書く  */


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

	glBindTexture(GL_TEXTURE_2D, Texture2);

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

	/*  描画処理終了　*/
	glEnd();
	glEnable(GL_LIGHTING);

	SwapBuffers(g_HDC);
	   	  
}

// ゲームの終了処理
void Finalize(void)
{
	// OpenGLの後かたずけ
	DeleteTexture();
	wglMakeCurrent(NULL, NULL);
	ReleaseDC(g_hWnd, g_HDC);
	wglDeleteContext(g_HGLRC);

}
