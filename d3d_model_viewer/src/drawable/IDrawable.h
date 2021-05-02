#pragma once

#include <vector>

#include <bindable/IBindable.h>
#include <bindable/VertexBuffer.h>
#include <bindable/IndexBuffer.h>
#include <Graphics.h>

class IDrawable
{
public:
	IDrawable();
	virtual ~IDrawable();

	virtual void setMesh(VertexBuffer* vertices, IndexBuffer* indices);
	virtual void addBindable(IBindable* bindable);
	virtual void draw(Graphics& gfx);

private:
	VertexBuffer* m_vertices;
	IndexBuffer* m_indices;
	std::vector<IBindable*> m_bindables;
};