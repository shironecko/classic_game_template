#include <render_core/i_render_context.h>
#include <render_core/render_queue.h>

namespace cgt
{

class Tilemap
{
public:
	static std::unique_ptr<Tilemap> LoadFrom(const char* path, render::IRenderContext& render, const char* parentFolder);

	explicit Tilemap(tmx_map* map, render::IRenderContext& render, const char* parentFolder);
	~Tilemap();

	void Render(render::RenderQueue& renderQueue);

private:
	void RenderLayer(render::RenderQueue& queue, const tmx_layer& layer);
	void RenderTile(render::RenderQueue& queue, const tmx_tile& tile, u32 x, u32 y);

	tmx_map* m_Map;
	std::unordered_map<const char*, render::TextureHandle> m_Textures;
};

}