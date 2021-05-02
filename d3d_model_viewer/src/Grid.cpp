#include "Grid.h"

#include "Graphics.h"

Grid::Grid()
{
	/*
	// Initialize grid
	data = new Vertex[4000];
	for (int i = 0; i < 2000; i += 2) {
		data[i].position.x = -500.0f;
		data[i].position.y = 0.0f;
		data[i].position.z = -500.0f + i;
		data[i].normal.x = 0.0f;
		data[i].normal.y = 1.0f;
		data[i].normal.z = 0.0f;

		data[i + 1].position.x = 500.0f;
		data[i + 1].position.y = 0.0f;
		data[i + 1].position.z = -500.0f + i;
		data[i + 1].normal.x = 0.0f;
		data[i + 1].normal.y = 1.0f;
		data[i + 1].normal.z = 0.0f;
	}
	for (int i = 2000; i < 4000; i += 2) {
		data[i].position.x = -500.0f + i - 2000.0f;
		data[i].position.y = 0.0f;
		data[i].position.z = -500.0f;
		data[i].normal.x = 0.0f;
		data[i].normal.y = 1.0f;
		data[i].normal.z = 0.0f;

		data[i + 1].position.x = -500.0f + i - 2000.0f;
		data[i + 1].position.y = 0.0f;
		data[i + 1].position.z = 500.0f;
		data[i + 1].normal.x = 0.0f;
		data[i + 1].normal.y = 1.0f;
		data[i + 1].normal.z = 0.0f;
	}
	data_buffer = Graphics::get()->create_buffer(data, 4000 * sizeof(Vertex), D3D11_BIND_VERTEX_BUFFER);
	*/
}

Grid::~Grid()
{
	/*
	delete data;
	data_buffer->Release();
	*/
}

void Grid::draw()
{
	/*
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	// Draw grid in xz plane
	// Set vertex buffer of grid
	Graphics::get()->d3d_context->IASetVertexBuffers(0, 1, &data_buffer, &stride, &offset);
	// Set primitive topology type to line list
	Graphics::get()->d3d_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	//Draw grid
	Graphics::get()->d3d_context->Draw(4000, 0);
	*/
}
