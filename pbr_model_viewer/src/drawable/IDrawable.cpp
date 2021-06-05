#include "IDrawable.h"

#include <algorithm>

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

void IDrawable::changePixelShader( PixelShader* new_ps )
{
	auto it = std::find_if( m_bindables.begin(), m_bindables.end(), [new_ps] ( IBindable* bindable )
							{
								PixelShader* shader = dynamic_cast<PixelShader*>( bindable );
								if ( shader && shader->getBytecode() != new_ps->getBytecode() )
								{
									return true;
								}
								return false;
							} );
	if ( it != m_bindables.end() )
	{
		m_bindables.erase( it );
		addBindable( new_ps );
	}
}

void IDrawable::addBindable(IBindable* bindable)
{
	m_bindables.push_back(bindable);
}

void IDrawable::deleteBindable( IBindable* bindable )
{
	m_bindables.erase( std::remove_if( m_bindables.begin(), m_bindables.end(), [bindable] ( IBindable* b )
									   {
										   if ( b == bindable )
										   {
											   delete b;
											   return true;
										   }
										   return false;
									   } ), m_bindables.end() );
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
