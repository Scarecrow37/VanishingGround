#include "pch.h"
#include "Skeleton.h"

void Skeleton::Initialize(const aiScene* paiScene, std::unordered_map<std::string, std::pair<unsigned int, Matrix>>& boneInfoTable)
{
    _boneNames.clear();
	LoadSkeleton(_rootBone, paiScene->mRootNode, boneInfoTable);
	_bones.push_back(&_rootBone);

    BoneNameTraverse(_rootBone);
	/*std::queue<Bone*> bfs;
	bfs.push(&_rootBone);

	while (!bfs.empty())
	{
		Bone* bone = bfs.front();
		bfs.pop();

		for (auto& child : bone->children)
		{
			child.parent = bone;
			bfs.push(&child);
		}
	}*/
}

void Skeleton::SetUpSplitBone(const unsigned int maxSplit)
{
	_bones.clear();
	_bones.resize(maxSplit);
}

void Skeleton::SplitBone(const unsigned int ID, const char* boneName)
{	
	std::queue<Bone*> bfs;
	bfs.push(&_rootBone);

	while (!bfs.empty())
	{
		Bone* bone = bfs.front();
		bfs.pop();

		if (bone->Name == boneName)
		{
			_bones[ID] = bone;
			break;
		}

		for (auto& child : bone->Children)
			bfs.push(&child);
	}
}

void Skeleton::MakeParent(const char* parent, const char* child)
{
	std::queue<Bone*> bfs;
	bfs.push(&_rootBone);

	Bone* pParent = nullptr;
	Bone* pChild = nullptr;

	while (!bfs.empty())
	{
		Bone* bone = bfs.front();
		bfs.pop();

		if (bone->Name == parent)
		{
			pParent = bone;
		}

		if (bone->Name == child)
		{
			pChild = bone;
		}

		for (auto& child : bone->Children)
			bfs.push(&child);
	}

	// pChild->parentAnim = &pParent->anim;

	//// 새로운 부모에 pChild 삽입
	//pParent->children.push_back(*pChild);

	//// pChild의 부모에서 자신을 제거
	//std::erase_if(pChild->parent->children, [pChild](const Bone& child) { return pChild->name == child.name; });
}

const std::vector<std::string>& Skeleton::GetBoneNameList() const 
{
    return _boneNames;
}

bool Skeleton::LoadSkeleton(Bone& bone, aiNode* paiNode, std::unordered_map<std::string, std::pair<unsigned int, Matrix>>& boneInfoTable)
{
	if (boneInfoTable.find(paiNode->mName.C_Str()) != boneInfoTable.end())
	{
		bone.Name = paiNode->mName.C_Str();
		bone.ID = boneInfoTable[bone.Name].first;
		bone.Offset = boneInfoTable[bone.Name].second;
		bone.Local = XMMatrixTranspose(XMMATRIX(&paiNode->mTransformation.a1));

		for (unsigned int i = 0; i < paiNode->mNumChildren; i++)
		{
			Bone child;
			if (LoadSkeleton(child, paiNode->mChildren[i], boneInfoTable))
			{
				bone.Children.push_back(child);
			}
		}

		return true;
	}
	else
	{
		for (unsigned int i = 0; i < paiNode->mNumChildren; i++)
		{
			if (LoadSkeleton(bone, paiNode->mChildren[i], boneInfoTable))
			{
				return true;
			}
		}
	}

	return false;
}

void Skeleton::BoneNameTraverse(const Bone& parentBone) 
{
    _boneNames.push_back(parentBone.Name);
    for (const auto& bone : parentBone.Children)
    {
        BoneNameTraverse(bone);
    }
}
