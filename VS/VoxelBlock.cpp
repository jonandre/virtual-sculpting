#include "VoxelBlock.h"


VoxelBlock::VoxelBlock(void)
{
	_active = false;
	_blockType = VoxelBlock::BlockType_Empty;
	_density = 1.0;
}


VoxelBlock::~VoxelBlock(void)
{
}

bool VoxelBlock::IsActive()
{
	return _active;
}
 
void VoxelBlock::SetActive(bool active)
{
	_active = active;
}

VoxelBlock::BlockType VoxelBlock::GetType()
{
	return _blockType;
}

void VoxelBlock::SetType(BlockType type)
{
	_blockType = type;
}


float VoxelBlock::GetDensity()
{
	return _density;
}


void VoxelBlock::SetDensity(float dens)
{
	_density = dens;
}