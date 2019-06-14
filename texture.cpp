#include "texture.h"

static unsigned int texture[2];	// 2個以上管理したい場合は配列にする
static unsigned int count = 0;

unsigned int LoadTexture(const char * filename)
{
	FILE* fp = fopen(filename,"rb");
	if (fp == NULL)	{ return 0xFFFFFFFF; }
	unsigned char header[18];
	fread(header, sizeof(header), 1, fp);

	if (header[1] == 1) {
		MessageBox(NULL, "画像を読み込めません。", "エラー", MB_ICONHAND | MB_OK);
		fclose(fp);
		return 0;
	}

	unsigned short w = *((unsigned short*)&header[12]);
	unsigned short h = header[15] * 256 + header[14];

	int compType;
	if (header[16] == 24) {
		compType = 3;
	}
	else if (header[16] == 32) {
		compType = 4;
	}
	else {
		MessageBox(NULL, "画像を読み込めません。", "エラー", MB_ICONHAND | MB_OK);
		fclose(fp);
		return 0;
	}
	

	//fread(pImage, w*h*compType, 1, fp);
	unsigned char* pImage = new unsigned char[w * h * compType];;

	if (header[2] >= 9) {
		//　連長圧縮あり
		int i = 0;

		while (i < (w * h)) {
			unsigned char rang;	//　連続、または非連続の回数
			fread(&rang,sizeof(unsigned char),1, fp);
			if (rang & 0x80) {
				//　連続なら長さを取得
				rang &= 0x7F;
				rang++;

				for (int j = 0; j < rang; i++, j++) {
					if (j == 0) {
						if (compType == 4) {
							fread(&pImage[i * 4 + 2], sizeof(unsigned char), 1, fp);
							fread(&pImage[i * 4 + 1], sizeof(unsigned char), 1, fp);
							fread(&pImage[i * 4 + 0], sizeof(unsigned char), 1, fp);
							fread(&pImage[i * 4 + 3], sizeof(unsigned char), 1, fp);
						}
						else {
							fread(&pImage[i * 3 + 2], sizeof(unsigned char), 1, fp);
							fread(&pImage[i * 3 + 1], sizeof(unsigned char), 1, fp);
							fread(&pImage[i * 3 + 0], sizeof(unsigned char), 1, fp);
						}
					}
					else {
						if (compType == 4) {
							pImage[i * 4 + 2] = pImage[i * 4 - 2];
							pImage[i * 4 + 1] = pImage[i * 4 - 3];
							pImage[i * 4 + 0] = pImage[i * 4 - 4];
							pImage[i * 4 + 3] = pImage[i * 4 - 1];
						}
						else {
							pImage[i * 3 + 2] = pImage[i * 3 - 1];
							pImage[i * 3 + 1] = pImage[i * 3 - 2];
							pImage[i * 3 + 0] = pImage[i * 3 - 3];
						}
					}
				}
			}	//　ランレングス

			//　こっちは非連続
			else {
				rang++;
				for (int j = 0; j < rang; i++, j++) {
					if (compType == 4) {
						fread(&pImage[i * 4 + 2], sizeof(unsigned char), 1, fp);
						fread(&pImage[i * 4 + 1], sizeof(unsigned char), 1, fp);
						fread(&pImage[i * 4 + 0], sizeof(unsigned char), 1, fp);
						fread(&pImage[i * 4 + 3], sizeof(unsigned char), 1, fp);
					}
					else {
						fread(&pImage[i * 3 + 2], sizeof(unsigned char), 1, fp);
						fread(&pImage[i * 3 + 1], sizeof(unsigned char), 1, fp);
						fread(&pImage[i * 3 + 0], sizeof(unsigned char), 1, fp);
					}
				}
			}


		}		
	}	//　圧縮の場合
	else {
		//　非圧縮の場合
		for (int i = 0; i < w * h; i++) {
			if (compType == 4) {
				pImage[i * 4 + 2] = fgetc(fp);
				pImage[i * 4 + 1] = fgetc(fp);
				pImage[i * 4 + 0] = fgetc(fp);
				pImage[i * 4 + 3] = fgetc(fp);
			}
			else {
				pImage[i * 3 + 2] = fgetc(fp);
				pImage[i * 3 + 1] = fgetc(fp);
				pImage[i * 3 + 0] = fgetc(fp);
			}
		}
			
	}

	
	fclose(fp);

	//　読み込み方法が左上から右下でない場合、入れ替える
	
	if (header[17] ^ 0x04) {
		//上下入れ替え
		if (compType == 4) {
			unsigned char swapBuffer[4];
			for (int i = 0; i < (h / 2); i++) {
				for (int j = 0; j < w; j++) {
					memcpy(swapBuffer, &pImage[(i * w + j) * compType], sizeof(unsigned char) * compType);
					memcpy(&pImage[(i * w + j) * compType], &pImage[(((h - 1) - i) * w + j)* compType], sizeof(unsigned char) * compType);
					memcpy(&pImage[(((h - 1) - i) * w + j)* compType], swapBuffer, sizeof(unsigned char) * compType);
				}
			}
		}
		else {
			unsigned char swapBuffer[3];
			for (int i = 0; i < (h / 2); i++) {
				for (int j = 0; j < w; j++) {
					
					memcpy(swapBuffer, &pImage[(i * w + j) * compType], sizeof(unsigned char) * compType);
					memcpy(&pImage[(i * w + j) * compType], &pImage[(((h - 1) - i) * w + j)* compType], sizeof(unsigned char) * compType);
					memcpy(&pImage[(((h - 1) - i) * w + j)* compType], swapBuffer, sizeof(unsigned char) * compType);
					
				}
			}
		}
	
	}
	
	if (header[17] & 0x08) {
		//　左右入れ替え
		if (compType == 4) {
			unsigned char swapBuffer[4];
			for (int i = 0; i < h; i++) {
				for (int j = 0; j < (2 / w); j++) {
					
					memcpy(swapBuffer, &pImage[(i * w + j) * compType], sizeof(unsigned char) * compType);
					memcpy(&pImage[(i * w + j) * compType], &pImage[(i * w + (w - 1) - j)* compType], sizeof(unsigned char) * compType);
					memcpy(&pImage[(i * w + (w - 1) - j)* compType], swapBuffer, sizeof(unsigned char) * compType);
				}
			}
		}
		else {
			unsigned char swapBuffer[3];
			for (int i = 0; i < h; i++) {
				for (int j = 0; j < (2 / w); j++) {
					
					memcpy(swapBuffer, &pImage[(i * w + j) * compType], sizeof(unsigned char) * compType);
					memcpy(&pImage[(i * w + j) * compType], &pImage[(i * w + (w - 1) - j)* compType], sizeof(unsigned char) * compType);
					memcpy(&pImage[(i * w + (w - 1) - j)* compType], swapBuffer, sizeof(unsigned char) * compType);
				}
			}
		}

	}




	
	//　テクスチャ生成
	glGenTextures(1, &texture[count]);
	glBindTexture(GL_TEXTURE_2D, texture[count]);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	//　くりかえし、サンプラーステート
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);	//　OpenGLでは、UV座標ではなく、ST座標。
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	//　この関数は、ターゲット、何を設定する？、設定したい値の順

	//　ミップマップを使用しない場合
/*
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	if (compType == 4) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pImage);
	}
	else {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, pImage);
	}
*/
	//　ミップマップを使用する場合
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	if (compType == 4) {
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, w, h, GL_RGBA, GL_UNSIGNED_BYTE, pImage);
	}
	else {
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, w, h, GL_RGB, GL_UNSIGNED_BYTE, pImage);
	}
	
	glBindTexture(GL_TEXTURE_2D, NULL);	//　アンバインドする
	delete[] pImage;
	count++;
	return texture[count - 1];
}

void DeleteTexture(void){
	glDeleteTextures(count, &texture[count]);
}


/*
　0バイト目は無視
 カラーマップタイプが１なら読めないってエラーを出すべき
 イメージタイプ（重要）　0x02じゃなかったら対応しなくてよい
 16バイト目が24か32じゃなかったら切る
 17バイト目
 4bit目　格納方向　0：→　1：←
 5bit目　格納方向　0：↑　1：↓
 あとはどうでもよい

 18バイト目からはデータが入っている
 １ピクセルのビット数が24ビット→３バイト:B-G-R
 　　　　　　　　　　　32ビット→４バイト:B-G-R-A
　バッファ量　w×h×3 or 4

*/