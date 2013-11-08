#include "Model.h"
#include "main.h"
#include <vector>
#include <map>
class VoxelChunk;
class VAO;

class GridModel: public Model
{
public:
	GridModel();
	GridModel( int power );

	~GridModel();
	
	int UpdateCellMelt( int i, int j, int k, UINT8 val );
	int UpdateCellAdd( int i, int j, int k, UINT8 val );
	UINT8* GetCells();
	unsigned int GetSize();
	unsigned int GetDimm();
	void UpdateGrid();
	inline  unsigned int GetCellIndex( const Point& pos, unsigned int &x, unsigned int &y, unsigned int &z );
	std::map< unsigned int, VAO* >* GetRenderableCells();
	void ReInitModel( bool clear );
private:
	//GridCell* _cells;
	unsigned int dimm;
	int half_dimm;
	unsigned int size;
	void EnshureMarked(int i, int j, int k);
	
	//std::map< unsigned int, RenderableCell* > _renderable_cells;
	//std::vector< GridCell* > _dirty_cells;
	inline bool EvaluateCell( unsigned int x, unsigned int y, unsigned int z );

	UINT8* _cells;
	bool* _interacted;
	VoxelChunk** _chunks;
	std::vector< VoxelChunk* > _dirty_chunks;
	std::vector< VoxelChunk* > _modified_chunks;
	std::map< unsigned int, VAO* > _renderable_chunks;
	unsigned int chunk_dimm;
	unsigned int chunk_size;
	unsigned int internal_chunk_size;
	unsigned int power_for_chunk;
};