#pragma once

struct Bone
{
	Matrix local;
	Matrix offset;
	std::string name;
	std::vector<Bone> children;
	int id;
	Matrix anim;
	//Bone* parent = nullptr;
	//Matrix* parentAnim = nullptr;
};

class Skeleton
{
public:
	Bone* GetBone(const unsigned int ID = 0) const { return _bones[ID]; }
	Bone& GetRootBone() { return _rootBone; }

public:
	void Initialize(const aiScene* paiScene, std::unordered_map<std::string, std::pair<unsigned int, Matrix>>& boneInfo);
	void SetUpSplitBone(const unsigned int maxSplit);
	void SplitBone(const unsigned int ID, const char* boneName);
	void MakeParent(const char* parent, const char* child);

private:
	bool LoadSkeleton(Bone& bone, aiNode* paiNode, std::unordered_map<std::string, std::pair<unsigned int, Matrix>>& boneInfo);

private:
	Bone _rootBone;
	std::vector<Bone*> _bones;
};