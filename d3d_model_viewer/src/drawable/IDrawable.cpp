#include "IDrawable.h"

IDrawable::IDrawable()
	: m_vertices(nullptr)
	, m_indices(nullptr)
{
}

IDrawable::~IDrawable()
{
	for (IBindable* bindable : m_bindables)
	{
		delete bindable;
	}
	delete m_vertices;
	delete m_indices;
}

void IDrawable::setMesh(VertexBuffer* vertices, IndexBuffer* indices)
{
	if (m_vertices && vertices)
	{
		delete m_vertices;
	}
	if (m_indices && indices)
	{
		delete m_indices;
	}
	m_vertices = vertices;
	m_indices = indices;
}

void IDrawable::addBindable(IBindable* bindable)
{
	m_bindables.push_back(bindable);
}

void IDrawable::draw(Graphics& gfx)
{
	if (m_vertices && m_indices)
	{
		for (IBindable* bindable : m_bindables)
		{
			bindable->bind(gfx);
		}
		m_vertices->bind(gfx);
		m_indices->bind(gfx);
		gfx.drawIndexed(m_indices->getIndexCount());
	}
}
