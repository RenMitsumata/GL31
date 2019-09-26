////////////////////////////////////////////////////////////////////
//
//	OpenGL�p�@Assimp���g�p����FBX�t�@�C���̃��f���̃N���X
//	[CModel.cpp]
//											Author: Ren Mitsumata
////////////////////////////////////////////////////////////////////
#include <Windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>

#include <vector>
#include <map>
#include <unordered_map>
#include "texture.h"
#include "CModel.h"



CModel::CModel()
{
}


CModel::~CModel()
{
}

void CModel::Init(const char * filename)
{
	Cnt = 0;
	// FBX�t�@�C���̓ǂݍ���
	pScene = aiImportFile(filename, aiProcessPreset_TargetRealtime_MaxQuality);
	if (pScene == nullptr) {
		char filestring[256];
		lstrcpy(filestring, filename);
		char exp[32] = { "�̓ǂݍ��݂Ɏ��s���܂���" };
		lstrcat(filestring, exp);
		MessageBox(nullptr, filestring, "Assimp", MB_OK | MB_ICONHAND);
		exit(1);
	}

	// �}�e���A���f�[�^�̍쐬
	int material = pScene->mNumMaterials;
	//texture = new unsigned int[material];

	for (int i = 0; i < material; i++) {
		aiString path;

		if (pScene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS) {
			// �}�e���A���ɉ摜������
			// �摜�͓����t�@�C���H�O���t�@�C���H
			if (path.data[0] == '*') {
				// FBX�����ɉ摜�t�@�C��������i�o�[�W�����ɂ���ĈقȂ�̂Œ��Ӂj

				int id = atoi(&path.data[1]);
				texture = LoadTextureFromMemory((const unsigned char*)pScene->mTextures[id]->pcData, pScene->mTextures[id]->mWidth);

			}
			else {
				std::string texPath = path.data;
				size_t pos = texPath.find_last_of("\\/");
				std::string headerPath = texPath.substr(0, pos + 1);
				headerPath += path.data;
				texPath.c_str();	// string�̐擪�A�h���X���擾�ł���
				texture = LoadTexture(headerPath.c_str(), 2);
			}
		}
		
	}

	// �{�[���f�[�^�̍쐬
	CreateBone(pScene->mRootNode);

	// �A�j���[�V�����p�z��̍쐬�i�Ƃ肠����0�t���[���ڂ̃f�[�^�����j
	pDeformVertexs = new std::vector<DEFORM_VERTEX>[pScene->mNumMeshes];
	for (auto m = 0; m < pScene->mNumMeshes; m++) {
		aiMesh* pMesh = pScene->mMeshes[m];
		for (auto v = 0; v < pMesh->mNumVertices; v++) {
			DEFORM_VERTEX defVertex;
			defVertex.position = pMesh->mVertices[v];
			defVertex.deformPosition = pMesh->mVertices[v];
			defVertex.normal = pMesh->mNormals[v];
			defVertex.deformNormal = pMesh->mNormals[v];
			defVertex.boneNum = 0;
			for (auto b = 0; b < 4; b++) {
				//defVertex.boneNum = b;
				defVertex.boneName[b] = {};
				defVertex.boneWeight[b] = 0.0f;
			}
			pDeformVertexs[m].push_back(defVertex);
		}
		for (auto b = 0; b < pMesh->mNumBones; b++) {
			aiBone* pBone = pMesh->mBones[b];
			Bones[pBone->mName.C_Str()].offsetMatrix = pBone->mOffsetMatrix;
			for (auto w = 0; w < pBone->mNumWeights; w++) {
				aiVertexWeight weight = pBone->mWeights[w];
				pDeformVertexs[m][weight.mVertexId].boneWeight[pDeformVertexs[m][weight.mVertexId].boneNum] = weight.mWeight;
				pDeformVertexs[m][weight.mVertexId].boneName[pDeformVertexs[m][weight.mVertexId].boneNum] = pBone->mName.C_Str();
				pDeformVertexs[m][weight.mVertexId].boneNum++;
				if (pDeformVertexs[m][weight.mVertexId].boneNum > 4) {
					MessageBox(nullptr, "���̃��f���f�[�^�͐������\������Ȃ��\��������܂�", "�x��", MB_OK);
				}
			}
		}

	}




}

void CModel::Update(void)
{
	Cnt++;
	
	// 0�Ԗڂ̃A�j���[�V�������擾
	if (pScene->HasAnimations()) {

		/*�@�����̃A�j���[�V�������擾����ꍇ
		for(int i=0;i<g_pScene->mNumAnimations;i++){
			aiAnimation* pAnimation = new aiAnimation[g_pScene->mNumAnimations];
			pAnimation = g_pScene->mAnimations[i];
		}
		*/
		aiAnimation* pAnimation = pScene->mAnimations[0];
		for (auto c = 0; c < pAnimation->mNumChannels; c++) {
			aiNodeAnim* pNodeAnim = pAnimation->mChannels[c];

			int f = Cnt % pNodeAnim->mNumRotationKeys;
			
			int p = Cnt % pNodeAnim->mNumPositionKeys;

			int s = Cnt % pNodeAnim->mNumScalingKeys;

			/*
			m_NodeRotation[pNodeAnim->mNodeName.C_Str()] = pNodeAnim->mRotationKeys[1].mValue;

			m_NodePosition[pNodeAnim->mNodeName.C_Str()] = pNodeAnim->mPositionKeys[1].mValue;
			*/

			
			BONE* pBone = &Bones[pNodeAnim->mNodeName.C_Str()];

			
			// ��]�i�N�H�[�^�j�I���j
			aiQuaternion rot = pNodeAnim->mRotationKeys[f].mValue;	// mRotationKeys[]�̔z��ԍ��́A�A�j���[�V�����t���[��������
			// �ړ�
			aiVector3D pos = pNodeAnim->mPositionKeys[p].mValue;	// mPositionKeys[]�̔z��ԍ��́A�A�j���[�V�����t���[��������
			// �X�P�[���l
			aiVector3D scaling = aiVector3D(1.0f,1.0f,1.0f);	// mScalingKeys[]�̔z��ԍ��́A�A�j���[�V�����t���[��������


			// �s��ɂ��ă{�[���f�[�^�Ƃ��Ċi�[����
			pBone->animationMatrix = aiMatrix4x4(scaling, rot, pos);
			
		}


		// �e���_�̍��W�ϊ��i�{���̓V�F�[�_�����ׂ��j
		for (unsigned int m = 0; m < pScene->mNumMeshes; m++)
		{
			for (auto& vertex : pDeformVertexs[m])
			{
				aiMatrix4x4 matrix[4];
				aiMatrix4x4 outMatrix;
				matrix[0] = Bones[vertex.boneName[0]].matrix;
				matrix[1] = Bones[vertex.boneName[1]].matrix;
				matrix[2] = Bones[vertex.boneName[2]].matrix;
				matrix[3] = Bones[vertex.boneName[3]].matrix;

				//�E�F�C�g���l�����ă}�g���N�X�Z�o
				{
					outMatrix.a1 = matrix[0].a1 * vertex.boneWeight[0]
						+ matrix[1].a1 * vertex.boneWeight[1]
						+ matrix[2].a1 * vertex.boneWeight[2]
						+ matrix[3].a1 * vertex.boneWeight[3];

					outMatrix.a2 = matrix[0].a2 * vertex.boneWeight[0]
						+ matrix[1].a2 * vertex.boneWeight[1]
						+ matrix[2].a2 * vertex.boneWeight[2]
						+ matrix[3].a2 * vertex.boneWeight[3];

					outMatrix.a3 = matrix[0].a3 * vertex.boneWeight[0]
						+ matrix[1].a3 * vertex.boneWeight[1]
						+ matrix[2].a3 * vertex.boneWeight[2]
						+ matrix[3].a3 * vertex.boneWeight[3];

					outMatrix.a4 = matrix[0].a4 * vertex.boneWeight[0]
						+ matrix[1].a4 * vertex.boneWeight[1]
						+ matrix[2].a4 * vertex.boneWeight[2]
						+ matrix[3].a4 * vertex.boneWeight[3];



					outMatrix.b1 = matrix[0].b1 * vertex.boneWeight[0]
						+ matrix[1].b1 * vertex.boneWeight[1]
						+ matrix[2].b1 * vertex.boneWeight[2]
						+ matrix[3].b1 * vertex.boneWeight[3];

					outMatrix.b2 = matrix[0].b2 * vertex.boneWeight[0]
						+ matrix[1].b2 * vertex.boneWeight[1]
						+ matrix[2].b2 * vertex.boneWeight[2]
						+ matrix[3].b2 * vertex.boneWeight[3];

					outMatrix.b3 = matrix[0].b3 * vertex.boneWeight[0]
						+ matrix[1].b3 * vertex.boneWeight[1]
						+ matrix[2].b3 * vertex.boneWeight[2]
						+ matrix[3].b3 * vertex.boneWeight[3];

					outMatrix.b4 = matrix[0].b4 * vertex.boneWeight[0]
						+ matrix[1].b4 * vertex.boneWeight[1]
						+ matrix[2].b4 * vertex.boneWeight[2]
						+ matrix[3].b4 * vertex.boneWeight[3];



					outMatrix.c1 = matrix[0].c1 * vertex.boneWeight[0]
						+ matrix[1].c1 * vertex.boneWeight[1]
						+ matrix[2].c1 * vertex.boneWeight[2]
						+ matrix[3].c1 * vertex.boneWeight[3];

					outMatrix.c2 = matrix[0].c2 * vertex.boneWeight[0]
						+ matrix[1].c2 * vertex.boneWeight[1]
						+ matrix[2].c2 * vertex.boneWeight[2]
						+ matrix[3].c2 * vertex.boneWeight[3];

					outMatrix.c3 = matrix[0].c3 * vertex.boneWeight[0]
						+ matrix[1].c3 * vertex.boneWeight[1]
						+ matrix[2].c3 * vertex.boneWeight[2]
						+ matrix[3].c3 * vertex.boneWeight[3];

					outMatrix.c4 = matrix[0].c4 * vertex.boneWeight[0]
						+ matrix[1].c4 * vertex.boneWeight[1]
						+ matrix[2].c4 * vertex.boneWeight[2]
						+ matrix[3].c4 * vertex.boneWeight[3];



					outMatrix.d1 = matrix[0].d1 * vertex.boneWeight[0]
						+ matrix[1].d1 * vertex.boneWeight[1]
						+ matrix[2].d1 * vertex.boneWeight[2]
						+ matrix[3].d1 * vertex.boneWeight[3];

					outMatrix.d2 = matrix[0].d2 * vertex.boneWeight[0]
						+ matrix[1].d2 * vertex.boneWeight[1]
						+ matrix[2].d2 * vertex.boneWeight[2]
						+ matrix[3].d2 * vertex.boneWeight[3];

					outMatrix.d3 = matrix[0].d3 * vertex.boneWeight[0]
						+ matrix[1].d3 * vertex.boneWeight[1]
						+ matrix[2].d3 * vertex.boneWeight[2]
						+ matrix[3].d3 * vertex.boneWeight[3];

					outMatrix.d4 = matrix[0].d4 * vertex.boneWeight[0]
						+ matrix[1].d4 * vertex.boneWeight[1]
						+ matrix[2].d4 * vertex.boneWeight[2]
						+ matrix[3].d4 * vertex.boneWeight[3];

				}

				vertex.deformPosition = vertex.position;
				vertex.deformPosition *= outMatrix;


				//�@���ϊ��p�Ɉړ��������폜
				outMatrix.a4 = 0.0f;
				outMatrix.b4 = 0.0f;
				outMatrix.c4 = 0.0f;

				vertex.deformNormal = vertex.normal;
				vertex.deformNormal *= outMatrix;
			}
		}

		// �ċA�I�Ƀ{�[���f�[�^���X�V����
		UpdateBoneMatrix(pScene->mRootNode, aiMatrix4x4());

	}
}

void CModel::Draw(void)
{

	aiNode* pNode = pScene->mRootNode;			// ���[�g�m�[�h�i��ԏ�̐e�p�[�c�j������Ă���
	
	aiMatrix4x4 matrix = pNode->mTransformation;	// �s��i�ʒu�j������Ă���
	aiTransposeMatrix4(&matrix);					// �s���]�u����(DirectX(����n)->openGL(�E��n))
	//glMultMatrixf((float*)&matrix);					// �s�����Z
	//aiVector3D size;
	//matrix.Scaling(size, matrix);
	//glScalef(1.0f, 3.0f, 1.0f);
	//glRotatef(270.0f, 0.0f, 1.0f, 0.0f);
	//glTranslatef(0.0f, 3.0f, 3.0f);


	


	DrawChildrens(pNode);


	/*
	aiMatrix4x4 matrix = pNode->mTransformation;	// �s��i�ʒu�j������Ă���
	aiTransposeMatrix4(&matrix);					// �s���]�u����(DirectX(����n)->openGL(�E��n))
	glMultMatrixf((float*)&matrix);					// �s���float��
	*/
	glBindTexture(GL_TEXTURE_2D, 0);
	

}

void CModel::Uninit(void)
{
	delete[] pDeformVertexs;
	aiReleaseImport(pScene);
}

void CModel::CreateBone(aiNode* pNode) {
	BONE bone;
	Bones[pNode->mName.C_Str()] = bone;	// �m�[�h�̖��O���{�[���������ɂ��� = �{�[����
	for (int i = 0; i < pNode->mNumChildren; i++) {
		CreateBone(pNode->mChildren[i]);
	}
}

void CModel::UpdateBoneMatrix(aiNode* pNode, aiMatrix4x4 matrix) {
	BONE* pBone = &Bones[pNode->mName.C_Str()];
	aiMatrix4x4 worldMatrix;
	worldMatrix = matrix;
	worldMatrix *= pBone->animationMatrix;
	//worldMatrix = worldMatrix * pBone->animationMatrix;
	pBone->matrix = worldMatrix;
	pBone->matrix *= pBone->offsetMatrix;
	for (int n = 0; n < pNode->mNumChildren; n++) {
		UpdateBoneMatrix(pNode->mChildren[n], worldMatrix);
	}

}

void CModel::DrawChildrens(aiNode* pNode) {
	glPushMatrix();

	




	aiMatrix4x4 matrix = pNode->mTransformation;	// �s��i�ʒu�j������Ă���
	aiTransposeMatrix4(&matrix);					// �s���]�u����(DirectX(����n)->openGL(�E��n))
	//glScalef(matrix.a1,matrix.b2, matrix.c3);
	//glMultMatrixf((float*)&Bones[pNode->mName.C_Str()].animationMatrix);
	glMultMatrixf((float*)&matrix);					// �s���float��

	//glMultMatrixf((float*)&Bones[pNode->mName.C_Str()].animationMatrix);






	for (int n = 0; n < pNode->mNumMeshes; n++) {
		
		unsigned int m = pNode->mMeshes[n];

		const aiMesh* pMesh = pScene->mMeshes[pNode->mMeshes[n]];

		// �}�e���A��
		const aiMaterial* pMat = pScene->mMaterials[pMesh->mMaterialIndex];

		aiColor4D diffuse;

		aiGetMaterialColor(pMat, AI_MATKEY_COLOR_DIFFUSE, &diffuse);

		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, (float*)&diffuse);

		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, (float*)&diffuse);

		// �e�N�X�`���̐ݒ�
		aiString path;

		pMat->GetTexture(aiTextureType_DIFFUSE, 0, &path);

		std::vector<DEFORM_VERTEX>* pVertices = &pDeformVertexs[m];

		glBindTexture(GL_TEXTURE_2D, texture);


		glBegin(GL_TRIANGLES);


		for (int t = 0; t < pMesh->mNumFaces; t++) {

			const aiFace* pFace = &(pMesh->mFaces[t]);

			if (pFace->mNumIndices != 3) {
				MessageBox(NULL, "3���_�ȊO�̃t�F�[�X���܂܂�Ă��܂�", "�x��", MB_OK);
			}

			for (int i = 0; i < pFace->mNumIndices; i++) {

				int index = pFace->mIndices[i];
				DEFORM_VERTEX* pVertex = &((*pVertices)[index]);

				glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
				//glColor4f(pVertex->boneWeight[0], pVertex->boneWeight[1], pVertex->boneWeight[2], 1.0f);
				glNormal3f(pVertex->deformNormal.x, pVertex->deformNormal.y, pVertex->deformNormal.z);
				if (pMesh->HasTextureCoords(0)) {
					glTexCoord2f(pMesh->mTextureCoords[0][index].x, 1.0f - pMesh->mTextureCoords[0][index].y);
				}
				glVertex3f(pVertex->deformPosition.x, pVertex->deformPosition.y, pVertex->deformPosition.z);
			}


		}

		glEnd();		

	}

	for (int r = 0; r < pNode->mNumChildren; r++) {
		DrawChildrens(pNode->mChildren[r]);
	}

	


	glPopMatrix();

}
/*
	�}�e���A������aiScene��aiMesh����

*/