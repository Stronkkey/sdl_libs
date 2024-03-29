#include <scene/resources/uid_texture.hpp>
#include <servers/rendering_server.hpp>

sdl::UidTexture::UidTexture() {
}

sdl::UidTexture::UidTexture(const uid from_uid) {
	texture_uid = from_uid;
}

sdl::Vector2i sdl::UidTexture::get_size(const std::unique_ptr<RenderingServer> &rendering_server) const {
	Optional<RenderingServer::TextureInfo> texture_info;
	if (rendering_server)
		texture_info = rendering_server->get_texture_info_from_uid(texture_uid);
	
	return texture_info ? texture_info->size : Vector2i();
}

SDL_Texture *sdl::UidTexture::get_texture(const std::unique_ptr<RenderingServer> &rendering_server) const {
	if (rendering_server) {
		Optional<RenderingServer::TextureInfo> texture_info = rendering_server->get_texture_info_from_uid(texture_uid);
		return texture_info ? texture_info->texture : nullptr;
	}

	return nullptr;
}

void sdl::UidTexture::draw(const std::unique_ptr<RenderingServer> &rendering_server,
	const uid texture_uid,
	const uid canvas_item_uid,
	const SDL_RendererFlip flip,
	const ColorV &modulate,
	const Transform2D &transform) const {
	rendering_server->canvas_item_add_texture(texture_uid, canvas_item_uid, flip, modulate, transform);
}

void sdl::UidTexture::draw_region(const std::unique_ptr<RenderingServer> &rendering_server,
	const uid texture_uid,
	const uid canvas_item_uid,
	const Rect2i &src_region,
	const SDL_RendererFlip flip,
	const ColorV &modulate,
	const Transform2D &transform ) const {
	rendering_server->canvas_item_add_texture_region(texture_uid, canvas_item_uid, src_region, flip, modulate, transform);
}

void sdl::UidTexture::load_from_path(const std::unique_ptr<RenderingServer> &rendering_server, const String &file_path) {
	Optional<uid> text_uid = rendering_server->load_texture_from_path(file_path);
	texture_uid = text_uid.value_or(0);
}
