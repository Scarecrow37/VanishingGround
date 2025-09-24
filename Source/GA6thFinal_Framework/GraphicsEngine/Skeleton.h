#pragma once

struct Bone
{
    Matrix            Offset;
    Matrix            Local;
    std::string       Name;
    std::vector<Bone> Children;
    int               ID;
    // Bone* parent = nullptr;
    // Matrix* parentAnim = nullptr;
};

class Skeleton
{
    friend class FBXConverter;

public:
    Bone* GetBone(const unsigned int ID = 0) const { return _bones[ID]; }
    Bone& GetRootBone() { return _rootBone; }

public:
    void Initialize(const aiScene* paiScene, std::unordered_map<std::string, std::pair<unsigned int, Matrix>>& boneInfo);
    void SetUpSplitBone(const unsigned int maxSplit);
    void SplitBone(const unsigned int ID, const char* boneName);
    void MakeParent(const char* parent, const char* child);
    const std::vector<std::string>& GetBoneNameList() const;



private:
    bool LoadSkeleton(Bone& bone, aiNode* paiNode, std::unordered_map<std::string, std::pair<unsigned int, Matrix>>& boneInfo);
    void BoneNameTraverse(const Bone& parentBone);

private:
    Bone               _rootBone;
    std::vector<Bone*> _bones;
    std::vector<std::string> _boneNames;

};