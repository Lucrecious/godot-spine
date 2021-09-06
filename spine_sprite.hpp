#pragma once

#include <spine/BlendMode.h>
#include <spine/Color.h>
#include <spine/Skeleton.h>
#include <spine/SkeletonClipping.h>

#include "scene/2d/node_2d.h"

#include "state.hpp"
#include "spine_data.hpp"

namespace animation {

class SpineSprite : public Node2D {
    GDCLASS(SpineSprite, Node2D)

    Ref<SpineData> _spine_data;

    spine::Skeleton* _skeleton;

    spine::SkeletonClipping _skeleton_clipping;

    void _draw_spine(const SpineData& spine_data, spine::Skeleton& skeleton);

    Color _convert_color(const spine::Color& color) const;
    BlendMode _convert_blend_mode(const spine::BlendMode& blend_mode) const;

    State _state;

    uint64_t _last_update_time;


protected:
    static void _bind_methods();

public:
    void _notification(int what);

    void spine_data_set(Ref<SpineData> spine_data);
    Ref<SpineData> spine_data_get() const;

    SpineSprite();
    ~SpineSprite();
};

};