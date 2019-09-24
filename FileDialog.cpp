/*================================================================================

	マップエディタ[FileDialog.cpp]
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

hInstance は、ダイアログボックステンプレートを用いる場合に
テンプレートを含むモジュールのインスタンスハンドルを指定します

lpstrFilter には、選択対象のファイルをフィルタリングするための文字列を指定します
この文字列は、表示文字列\0フィルタ文字列 という形で指定します
表示文字列はダイアログのリストボックスに表示される文字列で
フィルタ文字列は、フィルタパターンを（これは MS - DOS と同じ）指定します
例えば、"Text (*.txt)\0*.txt" とすれば、拡張子が txt のファイルのみが表示されます

この組み合わせを連続することで、リストボックスにその組み合わせの情報が加算されます
文字列の終端を知らせるには NULL 文字を2つ連続して指定します

lpstrCustomFilter は、ユーザー定義のフィルタ文字列バッファへのポインタを指定します
これは、lpstrFilter 同様に表示文字列とフィルタ文字列の組み合わせです
システムは、選択されたフィルタパターンをこのバッファに保存します
表示文字列は変わりませんが、フィルタは最後に保存されたものになっているでしょう

nMaxCustFilter には、lpstrCustomFilter のバッファサイズを指定します
この長さは、最低でも 40 文字分以上のサイズを指定しなければなりません
lpstrCustomFilter が NULL の場合は無視されます

nFilterIndex には、ダイアログの初期設定のフィルタインデックスを指定します
0 を指定すれば、ユーザー定義のカスタムフィルタを表します

lpstrFile は、ファイル名が格納される 256 文字以上のバッファへのポインタを指定します
このバッファに、選択されたファイルのフルパスが格納されます
nMaxFile には、lpstrFile が指すバッファサイズを指定します

lpstrFileTitle には、ファイルタイトルを格納するバッファへのポインタを指定します
ファイルタイトルとは、ドライブ名とディレクトリ名を除いた、ファイル自体の名前です
必要がない場合は NULL を選択してください
nMaxFileTitle には、lpstrFileTitle が指すバッファサイズを指定します

lpstrInitialDir は、初期のファイルディレクトリを表す文字列のポインタを指定します
このメンバが NULL の場合、選択ダイアログはカレントディレクトリを表示します
lpstrTitle には、ダイアログのタイトルバーに表示する文字列を指定します
NULL を指定すれば、デフォルトの文字がタイトルバーに表示されます

Flags は、ダイアログ作成フラグを指定します
このメンバには、以下の定数の組み合わせを指定することができます

定数	解説
OFN_ALLOWMULTISELECT	「ファイル名」リストボックスで複数選択を可能にする
専用テンプレートを用いてダイアログを作成する場合
「ファイル名」リストボックスの定義に LBS_EXTENDEDSEL 値を入れる
このフラグを選択すると lpstrFile メンバが指すバッファに
ディレクトリへのパスと、選択された全てのファイル名
そして、ファイル名の間はスペースで区切られ格納される
OFN_EXPLORER フラグが設定されている場合は
それぞれ NULL 文字で区切られ、連続した NULL 文字で終わる
OFN_CREATEPROMPT	現在存在しないファイルを作成するかを求めるプロンプトを表示する
OFN_PATHMUSTEXIST と OFN_FILEMUSTEXIST フラグも含む
OFN_ENABLEHOOK	lpfnHook で指定されたフック関数を有効にする
OFN_ENABLETEMPLATE	hInstance が lpTemplateName メンバで指定された
ダイアログテンプレートを含むリソースのインスタンスであることを示す
OFN_ENABLETEMPLATEHANDLE	hInstance メンバがロード済みのダイアログボックステンプレートを含む
メモリブロックを指していることを表す
このフラグが指定されている場合、lpTemplateName は無視される
OFN_EXPLORER	新しいエクスプローラスタイルのダイアログボックスの
カスタム化方法を用いることを示す
OFN_FILEMUSTEXIST	既存のファイル名しか入力を許さない
OFN_PATHMUSTEXIST フラグも含む
OFN_HIDEREADONLY	「読み取り専用」チェックボックスを隠す
OFN_LONGNAMES	古いダイアログボックスのために、長いファイル名を用いる
OFN_EXPLORER が設定されている場合は常に長い名前になる
OFN_NOCHANGEDIR	ダイアログボックスは、現在のディレクトリを
ダイアログボックスが呼び出された時のディレクトリに戻す
OFN_NODEREFERENCELINKS	選択されたショートカットファイル(.LNK)のパスとファイル名を
返すようにダイアログボックスに指示する
OFN_NOLONGNAMES	「ファイル名」リストボックスに長いファイル名を表示しない
OFN_NONETWORKBUTTON	「ネットワーク」ボタンを隠す
OFN_NOREADONLYRETURN	返されたファイルに対する「読み取り専用」チェックボックスに
チェックマークを付けない
OFN_NOTESTFILECREATE	ダイアログボックスを閉じる前にファイルを作成しない
このフラグは、「変更不可で作成」ネットワーク共有ポイント上で
ファイルを保存する場合に指定する
OFN_NOVALIDATE	無効な文字が入ったファイル名を有効とみなす
OFN_OVERWRITEPROMPT	保存時に選択されたファイルが存在する場合
メッセージボックスが表示され上書きをするか確認する
OFN_PATHMUSTEXIST	有効なパス及びファイル名でなければ入力を許さない
OFN_READONLY	ダイアログの「読み取り専用」チェックボックスをチェックすることを表す
OFN_SHAREAWARE	ネットワーク共有違反が原因で OpenFile() 関数呼び出しが失敗した場合に
エラーを無視して所定のファイル名を返す
このフラグが指定されている場合には SHAREVISTRING に対する
登録メッセージが lParam パラメータで指定されたパスおよびファイル名に対する
NULL で終わる文字列のポインタと共にフック関数に送られる
フック関数は次のいずれかで応答しなければならない

OFN_SHAREFALLTHROUGH - ファイル名を表示する
OFN_SHARENOWARN - アクションなし
OFN_SHAREWARN - 標準警告メッセージを出す
OFN_SHOWHELP	ヘルプボタンを表示する
このフラグを設定する場合、親ウィンドウを持たなければならない

nFileOffset は、lpstrFile が指す文字列内におけるパスの先頭から
ファイル名までの 0 から数えたオフセットを表します

nFileExtension は、lpstrFile が指す文字列内におけるパスの先頭から
ファイル名拡張子までの 0 から数えたオフセットを表します
lpstrDetExt が NULL であれば、終端の NULL 文字までのオフセットを指します
ユーザーがファイル名の最後の文字として "." を指定した場合は 0 になります

lpstrDefExt には、デフォルトの拡張子を表す文字列へのポインタを指定します
ユーザーが拡張子を入力しなかった場合、この拡張子が用いられます

lCustData は、フック関数に渡す追加データを指定します
lpfnHook には、フック関数へのポインタを指定します
lpTemplateName は、ダイアログテンプレートを使用する場合に、テンプレート名を指定します
*/


}

