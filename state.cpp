#include "state.hpp"

using namespace animation;

State::State() : _internal_state(NULL) {}

State::~State() {
    if (!_internal_state) return;
    delete _internal_state;
}

int State::track_count() const {
    if (!_internal_state) return 0;
    return _internal_state->getTracks().size();
}

bool State::is_empty(size_t track_id) const {
    if (!_internal_state) return true;

    spine::TrackEntry* track = _internal_state->getCurrent(track_id);
    if (!track) return true;

    spine::Animation* animation = track->getAnimation();
    if (!animation) return true;

    spine::String name = animation->getName();

    return name == "<empty>";
}

void State::data_set(Ref<SpineData> data) {
    if (_internal_state) delete _internal_state;

    _internal_state = NULL;
    _data = data;

    if (data.is_null() || !data.is_valid()) return;
    _internal_state = data->create_state();
}

Ref<SpineData> State::data_get() const {
    return _data;
}

void State::animation_set(size_t track_id, String name, float mix_sec) {
    if (!_internal_state) return;
    if (_data.is_null() || !_data.is_valid()) return;

    spine::Animation* anim = _data->get_animation(name);

    spine::TrackEntry* track_entry = NULL;
    if (!anim) {
        if (is_empty(track_id)) return;
        track_entry = _internal_state->setEmptyAnimation(track_id, 0);
    } else {
        track_entry = _internal_state->setAnimation(track_id, anim, false);
    }

    if (!track_entry) return;
    track_entry->setMixDuration(mix_sec);
}

String State::animation_get(size_t track_id) {
    if (!_internal_state) return "";
    if (!_internal_state->getCurrent(track_id)) return "";
    if (!_internal_state->getCurrent(track_id)->getAnimation()) return "";

    return String(_internal_state->getCurrent(track_id)->getAnimation()->getName().buffer());
}

void State::progress_set(size_t track_id, double value) {
    if (!_internal_state) return;

    spine::TrackEntry* track = _internal_state->getCurrent(track_id);
    if (!track) return;

    track->setTrackTime(0);

    spine::Animation* animation = track->getAnimation();

    if (!animation) return;

    track->setTrackTime(value);
}

double State::progress_get(size_t track_id) {
    if (!_internal_state) return 0;

    spine::TrackEntry* track = _internal_state->getCurrent(track_id);
    if (!track) return 0;

    spine::Animation* animation = track->getAnimation();
    if (!animation) return 0;

    if (animation->getDuration() == 0) return 0;

    return track->getTrackTime();
}

void State::alpha_set(size_t track_id, float value) {
    if (!_internal_state) return;

    spine::TrackEntry* track = _internal_state->getCurrent(track_id);
    if (!track) return;

    track->setAlpha(value);
}

float State::alpha_get(size_t track_id) const {
    if (!_internal_state) return 0.0;

    spine::TrackEntry* track = _internal_state->getCurrent(track_id);
    if (!track) return 0.0;

    return track->getAlpha();
}

void State::apply(spine::Skeleton& skeleton) {
    if (!_internal_state) return;
    _internal_state->apply(skeleton);
}

void State::update(float delta) {
    if (!_internal_state) return;
    _internal_state->update(delta);
}

void State::clear() {
    if (_internal_state) delete _internal_state;
    _internal_state = NULL;
    _data = RES();
}