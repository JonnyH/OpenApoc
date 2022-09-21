#include "framework/renderer.h"

#include "framework/logger.h"
#include "library/sp.h"
#include <utility>

namespace OpenApoc
{

RendererSurfaceBinding::RendererSurfaceBinding(Renderer &r, sp<Surface> s)
    : prevBinding(r.getSurface()), r(r)
{
	r.setSurface(std::move(s));
}

RendererSurfaceBinding::~RendererSurfaceBinding() { r.setSurface(prevBinding); }

Renderer::~Renderer() = default;

RendererImageData::~RendererImageData() = default;

sp<Image> RendererImageData::readBack()
{
	LogWarning("NOT IMPLEMENTED");
	return nullptr;
}

}; // namespace OpenApoc
