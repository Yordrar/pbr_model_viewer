#pragma once

#include <vector>

#include <bindable/IBindable.h>
#include <bindable/VertexBuffer.h>
#include <bindable/IndexBuffer.h>
#include <bindable/PixelShader.h>
#include <Graphics.h>

class IDrawable
{
public:
	IDrawable();
	virtual ~IDrawable();

	void setMesh(VertexBuffer* vertices, IndexBuffer* indices);
	void changePixelShader( PixelShader* new_ps );
	virtual void addBindable( IBindable* bindable );
	virtual void deleteBindable( IBindable* bindable );
	virtual void draw(Graphics& gfx);

private:
	VertexBuffer* m_vertices;
	IndexBuffer* m_indices;
	std::vector<IBindable*> m_bindables;
};