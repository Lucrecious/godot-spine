#pragma once

#include <spine/TextureLoader.h>
#include <spine/Atlas.h>

#include "scene/resources/texture.h"

namespace animation
{

class SpineTextureLoader : public spine::TextureLoader {
    Ref<Texture> _texture;

public:
    SpineTextureLoader() { }

    virtual ~SpineTextureLoader() { }

    // Called when the atlas loads the texture of a page.
    void load(spine::AtlasPage& page, const spine::String& path) override {
        _texture = ResourceLoader::load(path.buffer());

        if (_texture.is_null()) return;

        page.setRendererObject(_texture.ptr());

        // store the texture width and height on the spAtlasPage
        // so spine-c can calculate texture coordinates for
        // rendering.
        page.width = _texture->get_width();
        page.height = _texture->get_height();
    }

    void unload(void* texture) override {
        _texture = RES();
    }
};
    
}
