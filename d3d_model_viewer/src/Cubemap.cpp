#include "Cubemap.h"

#include <string>

#include "Graphics.h"
#include <Vertex.h>
#include <bindable/VertexShader.h>
#include <bindable/VertexBuffer.h>
#include <bindable/PixelShader.h>
#include <bindable/IndexBuffer.h>
#include <bindable/InputLayout.h>
#include <bindable/TextureCube.h>
#include <bindable/TextureSampler.h>

#include "stb_image.h"

Cubemap::Cubemap(Graphics& gfx, std::string path)
	: IDrawable()
{
	Vertex* data = new Vertex[8];
	float size = 100.0f;
	data[0].position = { -size, -size, -size };
	data[1].position = { size, -size, -size };
	data[2].position = { -size, size, -size };
	data[3].position = { size, size, -size };
	data[4].position = { -size, -size, size };
	data[5].position = { size, -size, size };
	data[6].position = { -size, size, size };
	data[7].position = { size, size, size };

	UINT index_data[36] = {
				0,1,2, 2,1,3,
				1,5,3, 3,5,7,
				2,3,6, 3,7,6,
				4,7,5, 4,6,7,
				0,2,4, 2,6,4,
				0,4,1, 1,4,5
	};

	setMesh(new VertexBuffer(gfx, data, 8), new IndexBuffer(gfx, index_data, 36));

	addBindable(new PixelShader(gfx, "cubemap_ps.cso"));
	addBindable(new TextureSampler(gfx, 0));
	addBindable(new TextureCube(gfx, path));
}

Cubemap::~Cubemap()
{

}
