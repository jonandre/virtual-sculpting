#pragma once
class VoxelBlock
{
public:
	enum BlockType
	{
		BlockType_Empty = 0,

		BlockType_Clay,
		BlockType_ModifiedClay,
		BlockType_ModifiedEmpty,		
		BlockType_NumTypes,
	};

	VoxelBlock();
	~VoxelBlock();

	bool IsActive();
    void SetActive(bool active);

	BlockType GetType();
    void SetType(BlockType type);

	float GetDensity();
    void SetDensity(float dens);
private:
	bool _active;
	BlockType _blockType;
	float _density;
};

