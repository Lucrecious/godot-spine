#pragma once

#include <spine/Animation.h>
#include <spine/AnimationState.h>
#include <spine/AnimationStateData.h>
#include <spine/Atlas.h>
#include <spine/MixBlend.h>
#include <spine/MixDirection.h>
#include <spine/Skeleton.h>
#include <spine/SkeletonData.h>

#include "core/resource.h"

#include "spine_mix_settings.hpp"
#include "spine_texture_loader.hpp"

namespace animation {

class SpineData : public Resource {
    GDCLASS(SpineData, Resource)
    SpineTextureLoader spine_texture_loader;

    spine::Atlas* _atlas;
    spine::SkeletonData* _skeleton_data;
    spine::AnimationStateData* _animation_state_data;
    
    Ref<SpineMixSettings> _spine_mix_settings;

    void _free_data();

protected:
    static void _bind_methods();

public:
    void load_data(const String& path);
    Error save_data(const String& path, const RES& resource);

    PoolStringArray get_animations() const;
    spine::Animation* get_animation(const String& name) const;

    spine::Skeleton* create_skeleton() const;
    spine::AnimationState* create_state() const;
    void apply(spine::Animation* animation, spine::Skeleton& skeleton, float time, bool loop, float alpha, spine::MixBlend mix_blend, spine::MixDirection direction) const;

    int num_animations() const;

    SpineData();
    virtual ~SpineData();

    void spine_mix_settings_set(Ref<SpineMixSettings> mix_settings);
    Ref<SpineMixSettings> spine_mix_settings_get() const;
};

}