#pragma once

#include <spine/AnimationState.h>
#include <spine/AnimationStateData.h>
#include <spine/Skeleton.h>

#include "core/reference.h"

#include "spine_data.hpp"


namespace animation {

class State {
    Ref<SpineData> _data;
    spine::AnimationState* _internal_state;

public:
    State();
    ~State();

    void data_set(Ref<SpineData> data);
    Ref<SpineData> data_get() const;

    int track_count() const;

    bool is_empty(size_t track_id) const;

    void animation_set(size_t track_id, String name, float mix_sec);
    String animation_get(size_t track_id);

    void progress_set(size_t track_id, double value);
    double progress_get(size_t track_id);

    void alpha_set(size_t track_id, float value);
    float alpha_get(size_t track_id) const;

    void apply(spine::Skeleton& skeleton);
    void update(float delta);

    void clear();
};

}