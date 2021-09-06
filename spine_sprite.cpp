#include "spine_sprite.hpp"

#include <string>

#include <spine/Atlas.h>
#include <spine/Attachment.h>
#include <spine/ClippingAttachment.h>
#include <spine/MeshAttachment.h>
#include <spine/RegionAttachment.h>
#include <spine/SkeletonClipping.h>
#include <spine/Slot.h>
#include <spine/SlotData.h>

#include "core/os/os.h"

#include "batcher.hpp"
#include "spine_track.hpp"

#include "tools/scopeguard.hpp"

using namespace animation;

void SpineSprite::_bind_methods() {
    ClassDB::bind_method(D_METHOD("spine_data_set", "data"), &SpineSprite::spine_data_set);
    ClassDB::bind_method(D_METHOD("spine_data_get"), &SpineSprite::spine_data_get);

   	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "spine_data", PROPERTY_HINT_RESOURCE_TYPE, "SpineData"), "spine_data_set", "spine_data_get");
}

void SpineSprite::_draw_spine(const SpineData& spine_data, spine::Skeleton& skeleton) {
    int total_tracks = 0;
    Ref<SpineMixSettings> mix_settings = spine_data.spine_mix_settings_get();

    for (int i = 0; i < get_child_count(); i++) {
        auto child = Object::cast_to<SpineTrack>(get_child(i));
        if (!child) continue;

        int track_index = total_tracks++;
        
        String animation_name = _state.animation_get(track_index);
        if (animation_name != child->animation_name_get()) {
            float mix = 0.0;
            if (mix_settings.is_valid()) {
                mix = mix_settings->get_mix(animation_name, child->animation_name_get());
            }
            _state.animation_set(track_index, child->animation_name_get(), mix);
        }

        _state.progress_set(track_index, child->seconds_get());
        _state.alpha_set(track_index, child->alpha_get());
    }

    for (int i = total_tracks; i < _state.track_count(); i++) {
        if (_state.is_empty(i)) continue;
        float mix = 0.0;
        if (mix_settings.is_valid()) {
            mix = mix_settings->get_mix(_state.animation_get(i), "");
        }

        _state.animation_set(i, "", mix);
    }

    _state.apply(skeleton);

    skeleton.updateWorldTransform();

    static const unsigned short quad_indices[] = {0, 1, 2, 2, 3, 0 };

    spine::Vector<float> vertices;
    spine::Vector<float> uvs;
    spine::Vector<unsigned short> triangles;
    Vector<Color> colors;

    auto& slots = skeleton.getDrawOrder();

    VisualServer::get_singleton()->canvas_item_clear(get_canvas_item());
    Batcher batcher(this);

    for (int i = 0; i < slots.size(); i++) {
        auto slot = slots[i];

        // This makes sure the loop ends with ending the clipping if necessary
        ScopeGuard end_clip_guard([this, slot](){ this->_skeleton_clipping.clipEnd(*slot); });

        vertices.clear();
        uvs.clear();
        triangles.clear();
        colors.clear();

        spine::Attachment* attachment = slot->getAttachment();
        if (!attachment) continue;

        // Blend mode is omitted to keep things simple, will add if necessary
        // CanvasItem::BlendMode blend_mode = _convert_blend_mode(slot->getData().getBlendMode());

        Color skeleton_color = _convert_color(skeleton.getColor());
        Color slot_color = _convert_color(slot->getColor());
        Color tint(
            skeleton_color.r * slot_color.r,
            skeleton_color.g * slot_color.g,
            skeleton_color.b * slot_color.b,
            skeleton_color.a * slot_color.a);
            
        Texture* texture = NULL;
        if (attachment->getRTTI().isExactly(spine::RegionAttachment::rtti)) {
            spine::RegionAttachment* region = (spine::RegionAttachment*)attachment;

            texture = (Texture*)((spine::AtlasRegion*)region->getRendererObject())->page->getRendererObject();

            vertices.setSize(8, 0);

            region->computeWorldVertices(slot->getBone(), vertices, 0);

            for (size_t j = 0; j < 8; j+=2) {
                uvs.add(region->getUVs()[j]);
                uvs.add(region->getUVs()[j + 1]);
            }

            for (int j = 0; j < 6; j++) {
                triangles.add(quad_indices[j]);
            }
        } else if (attachment->getRTTI().isExactly(spine::MeshAttachment::rtti)) {
            spine::MeshAttachment* mesh = (spine::MeshAttachment*)attachment;

            vertices.setSize(mesh->getWorldVerticesLength(), 0);
            mesh->computeWorldVertices(*slot, vertices);

            texture = (Texture*)((spine::AtlasRegion*)mesh->getRendererObject())->page->getRendererObject();

            for (size_t j = 0; j < mesh->getWorldVerticesLength(); j+=2) {
                uvs.add(mesh->getUVs()[j]);
                uvs.add(mesh->getUVs()[j + 1]);
            }

            for (int j = 0; j < mesh->getTriangles().size(); j++) {
                triangles.add(mesh->getTriangles()[j]);
            }
        } else if (attachment->getRTTI().isExactly(spine::ClippingAttachment::rtti)) {
            auto clip = (spine::ClippingAttachment*) attachment;
            _skeleton_clipping.clipStart(*slot, clip);
            continue;
        }

        if (!texture) continue;

        Vector<Vector2> input_vertices;
        Vector<Vector2> input_uvs;
        Vector<int> input_triangles;

        if (_skeleton_clipping.isClipping()) {
            _skeleton_clipping.clipTriangles(vertices, triangles, uvs, 2);

            if (_skeleton_clipping.getClippedTriangles().size() <= 0) continue;

            auto& clipped_vertices = _skeleton_clipping.getClippedVertices();
            auto& clipped_uvs = _skeleton_clipping.getClippedUVs();
            auto& clipped_triangles = _skeleton_clipping.getClippedTriangles();

            input_triangles.resize(clipped_triangles.size());
            for (int j = 0; j < clipped_triangles.size(); j++) {
                input_triangles.set(j, clipped_triangles[j]);
            }

            input_vertices.resize(clipped_vertices.size() / 2);
            input_uvs.resize(clipped_uvs.size() / 2);
            colors.resize(clipped_vertices.size() / 2);
            for (int j = 0; j < clipped_vertices.size() / 2; j++) {
                input_vertices.set(j, Vector2(clipped_vertices[j * 2], -clipped_vertices[j * 2 + 1]));
                input_uvs.set(j, Vector2(clipped_uvs[j * 2], clipped_uvs[j * 2 + 1]));
                colors.set(j, tint);
            }
        } else {
            input_triangles.resize(triangles.size());
            for (int j = 0; j < triangles.size(); j++) {
                input_triangles.set(j, triangles[j]);
            }

            input_vertices.resize(vertices.size() / 2);
            input_uvs.resize(uvs.size() / 2);
            colors.resize(vertices.size() / 2);
            for (int j = 0; j < vertices.size() / 2; j++) {
                input_vertices.set(j, Vector2(vertices[j * 2], -vertices[j * 2 + 1]));
                input_uvs.set(j, Vector2(uvs[j * 2], uvs[j * 2 + 1]));
                colors.set(j, tint);
            }
        }

        if (input_triangles.size() <= 0) continue;

        batcher.add(
            input_triangles,
            input_vertices,
            colors,
            input_uvs,
            texture);
    }

    batcher.flush();
    _skeleton_clipping.clipEnd();
}

Color SpineSprite::_convert_color(const spine::Color& color) const {
    return Color(color.r, color.g, color.b, color.a);
}

CanvasItem::BlendMode SpineSprite::_convert_blend_mode(const spine::BlendMode& blend_mode) const {
    switch(blend_mode) {
        case spine::BlendMode_Normal:
            return CanvasItem::BlendMode::BLEND_MODE_MIX;
        case spine::BlendMode_Additive:
            return CanvasItem::BlendMode::BLEND_MODE_ADD;
        case spine::BlendMode_Multiply:
            return CanvasItem::BlendMode::BLEND_MODE_MUL;
        case spine::BlendMode_Screen:
            print_error("Warning: BlendMode_Screen is using PremultAlpha, what's the correct conversion?");
            return CanvasItem::BlendMode::BLEND_MODE_PREMULT_ALPHA;
        default:
            print_error("Unknown Spine BlendMode; disabled");
            return CanvasItem::BlendMode::BLEND_MODE_DISABLED;
    }
}

void SpineSprite::_notification(int what) {
    switch(what) {
        case NOTIFICATION_DRAW:
        {
            if (_spine_data.is_null()) return;
            if (!_skeleton) return;

            uint64_t current = OS::get_singleton()->get_ticks_usec();
            uint64_t delta = current - _last_update_time;
            _state.update((float)(delta / 1000000.0));
            _last_update_time = current;

            _draw_spine(*_spine_data.ptr(), *_skeleton);
            break;
        }
        default: break;
    }
}

void SpineSprite::spine_data_set(Ref<SpineData> spine_data) {
    if (spine_data == _spine_data) return;
    update();
    
    if (_skeleton) delete _skeleton;
    _skeleton = NULL;
    _state.clear();
    _last_update_time = OS::get_singleton()->get_ticks_usec();

    _spine_data = spine_data;
    _change_notify();

    if (spine_data.is_null()) return;

    _skeleton = spine_data->create_skeleton();
    _state.data_set(_spine_data);

    if (!_skeleton) {
        print_error("SpineData is invalid, cannot create Skeleton.");
        return;
    }
}

Ref<SpineData> SpineSprite::spine_data_get() const {
    return _spine_data;
}

SpineSprite::SpineSprite() {
    _spine_data = RES();
    _skeleton = NULL;
    _last_update_time = 0;
}

SpineSprite::~SpineSprite() {
    if (_skeleton) delete _skeleton;
    _spine_data = RES();
}