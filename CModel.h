////////////////////////////////////////////////////////////////////
//
//	OpenGL用　Assimpを使用したFBXファイルのモデルのクラス
//	[CModel.h]
//											Author: Ren Mitsumata
////////////////////////////////////////////////////////////////////
#pragma once
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/matrix4x4.h>
#pragma comment (lib,"OpenGL32.lib")
#pragma comment (lib,"GLU32.lib")
#pragma comment (lib,"assimp.lib")

class CModel
{
	struct DEFORM_VERTEX {
		aiVector3D position;
		aiVector3D deformPosition;
		aiVector3D normal;
		aiVector3D deformNormal;
		int boneNum;
		// int boneIndex[4];	←早いほう
		std::string boneName[4];
		float boneWeight[4];

	};

	struct BONE {
		// std::string name;
		aiMatrix4x4 matrix;
		aiMatrix4x4 animationMatrix;
		aiMatrix4x4 offsetMatrix;
	};

private:
	unsigned int texture;
	unsigned int Cnt;
	std::map<std::string, aiQuaternion> m_NodeRotation;
	std::map<std::string, aiVector3D> m_NodePosition;
	const aiScene* pScene = nullptr;
	std::map<std::string, BONE> Bones;
	std::vector<DEFORM_VERTEX>* pDeformVertexs;
	void CreateBone(aiNode * pNode);
	void UpdateBoneMatrix(aiNode * pNode, aiMatrix4x4 matrix);

	void DrawChildrens(aiNode * pNode);

public:
	CModel();
	~CModel();
	void Init(const char* filename);
	void Update(void);
	void Draw(void);
	void Uninit(void);

	

};

