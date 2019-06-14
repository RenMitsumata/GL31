#include "texture.h"

static unsigned int texture[2];	// 2�ȏ�Ǘ��������ꍇ�͔z��ɂ���
static unsigned int count = 0;

unsigned int LoadTexture(const char * filename)
{
	FILE* fp = fopen(filename,"rb");
	if (fp == NULL)	{ return 0xFFFFFFFF; }
	unsigned char header[18];
	fread(header, sizeof(header), 1, fp);

	if (header[1] == 1) {
		MessageBox(NULL, "�摜��ǂݍ��߂܂���B", "�G���[", MB_ICONHAND | MB_OK);
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
		MessageBox(NULL, "�摜��ǂݍ��߂܂���B", "�G���[", MB_ICONHAND | MB_OK);
		fclose(fp);
		return 0;
	}
	

	//fread(pImage, w*h*compType, 1, fp);
	unsigned char* pImage = new unsigned char[w * h * compType];;

	if (header[2] >= 9) {
		//�@�A�����k����
		int i = 0;

		while (i < (w * h)) {
			unsigned char rang;	//�@�A���A�܂��͔�A���̉�
			fread(&rang,sizeof(unsigned char),1, fp);
			if (rang & 0x80) {
				//�@�A���Ȃ璷�����擾
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
			}	//�@���������O�X

			//�@�������͔�A��
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
	}	//�@���k�̏ꍇ
	else {
		//�@�񈳏k�̏ꍇ
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

	//�@�ǂݍ��ݕ��@�����ォ��E���łȂ��ꍇ�A����ւ���
	
	if (header[17] ^ 0x04) {
		//�㉺����ւ�
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
		//�@���E����ւ�
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




	
	//�@�e�N�X�`������
	glGenTextures(1, &texture[count]);
	glBindTexture(GL_TEXTURE_2D, texture[count]);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	//�@���肩�����A�T���v���[�X�e�[�g
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);	//�@OpenGL�ł́AUV���W�ł͂Ȃ��AST���W�B
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	//�@���̊֐��́A�^�[�Q�b�g�A����ݒ肷��H�A�ݒ肵�����l�̏�

	//�@�~�b�v�}�b�v���g�p���Ȃ��ꍇ
/*
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	if (compType == 4) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pImage);
	}
	else {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, pImage);
	}
*/
	//�@�~�b�v�}�b�v���g�p����ꍇ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	if (compType == 4) {
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, w, h, GL_RGBA, GL_UNSIGNED_BYTE, pImage);
	}
	else {
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, w, h, GL_RGB, GL_UNSIGNED_BYTE, pImage);
	}
	
	glBindTexture(GL_TEXTURE_2D, NULL);	//�@�A���o�C���h����
	delete[] pImage;
	count++;
	return texture[count - 1];
}

void DeleteTexture(void){
	glDeleteTextures(count, &texture[count]);
}


/*
�@0�o�C�g�ڂ͖���
 �J���[�}�b�v�^�C�v���P�Ȃ�ǂ߂Ȃ����ăG���[���o���ׂ�
 �C���[�W�^�C�v�i�d�v�j�@0x02����Ȃ�������Ή����Ȃ��Ă悢
 16�o�C�g�ڂ�24��32����Ȃ�������؂�
 17�o�C�g��
 4bit�ځ@�i�[�����@0�F���@1�F��
 5bit�ځ@�i�[�����@0�F���@1�F��
 ���Ƃ͂ǂ��ł��悢

 18�o�C�g�ڂ���̓f�[�^�������Ă���
 �P�s�N�Z���̃r�b�g����24�r�b�g���R�o�C�g:B-G-R
 �@�@�@�@�@�@�@�@�@�@�@32�r�b�g���S�o�C�g:B-G-R-A
�@�o�b�t�@�ʁ@w�~h�~3 or 4

*/