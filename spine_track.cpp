#include "spine_track.hpp"

#include <string>

#include "spine_sprite.hpp"

using namespace animation;

void SpineTrack::_notification(int what) {
    switch(what) {
        case NOTIFICATION_ENTER_TREE:
        {
            _update_cache();
        }
    }
}

void SpineTrack::_bind_methods() {
    ClassDB::bind_method(D_METHOD("spine_sprite_path_set", "path"), &SpineTrack::spine_sprite_path_set);
    ClassDB::bind_method(D_METHOD("spine_sprite_path_get"), &SpineTrack::spine_sprite_path_get);

    ClassDB::bind_method(D_METHOD("animation_set", "name"), &SpineTrack::animation_name_set);
    ClassDB::bind_method(D_METHOD("animation_get"), &SpineTrack::animation_name_get);

    ClassDB::bind_method(D_METHOD("seconds_set", "value"), &SpineTrack::seconds_set);
    ClassDB::bind_method(D_METHOD("seconds_get"), &SpineTrack::seconds_get);

    ClassDB::bind_method(D_METHOD("alpha_set", "value"), &SpineTrack::alpha_set);
    ClassDB::bind_method(D_METHOD("alpha_get"), &SpineTrack::alpha_get);
    
    ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "spine_sprite_path"), "spine_sprite_path_set", "spine_sprite_path_get");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "animation"), "animation_set", "animation_get");
    ADD_PROPERTY(PropertyInfo(Variant::REAL, "seconds", PROPERTY_HINT_RANGE, "0,100,0.1"), "seconds_set", "seconds_get");
    ADD_PROPERTY(PropertyInfo(Variant::REAL, "alpha", PROPERTY_HINT_RANGE, "0,1,0.01"), "alpha_set", "alpha_get");
}

void SpineTrack::_validate_property(PropertyInfo &property) const {
    Ref<SpineData> data;
    SpineSprite* sprite = _get_spine_sprite();
    if (sprite) {
        data = sprite->spine_data_get();
    }

    if (property.name == "animation") {

        property.hint = PROPERTY_HINT_ENUM;

        PoolStringArray animations;
        if (!data.is_null() && data.is_valid()) {
            animations = data->get_animations();
        }
        animations.insert(0, String(NoneAnimation));

        property.hint_string = animations.join(",");
    }

    if (property.name == "seconds") {
        property.hint = PROPERTY_HINT_RANGE;
        property.hint_string = "0,1,.0333";

        if (data.is_null() || !data.is_valid()) return;

        spine::Animation* animation = data->get_animation(_animation);

        if (!animation) return;

        property.hint_string = String(("0," + std::to_string(animation->getDuration()) + "," + std::to_string(animation->getDuration()/100)).c_str());
    }
}

SpineTrack::SpineTrack() {
    _spine_sprite_path = NodePath();
    _spine_sprite_id = ObjectID();
    _animation = "";
    _seconds = 0;
    _alpha = 1;
}

SpineTrack::~SpineTrack() {
}

void SpineTrack::spine_sprite_path_set(NodePath path) {
    if (path == _spine_sprite_path) return;

    _spine_sprite_id = ObjectID();
    _spine_sprite_path = path;

    _change_notify();
    
    _update_cache();

    SpineSprite* sprite = _get_spine_sprite();

    if (!sprite) return;

    sprite->update();
}

NodePath SpineTrack::spine_sprite_path_get() const {
    return _spine_sprite_path;
}

void SpineTrack::animation_name_set(String name) {
    if (_animation == name) return;

    _animation = name;
    _change_notify();

    seconds_set(0.0);

    SpineSprite* sprite = _get_spine_sprite();

    if (!sprite) return;

    sprite->update();
}

String SpineTrack::animation_name_get() const {
    return _animation;
}

void SpineTrack::seconds_set(float value) {
    if (_seconds == value) return;

    _seconds = value;
    _change_notify("seconds");

    SpineSprite* sprite = _get_spine_sprite();
    if (!sprite) return;

    sprite->update();

}

float SpineTrack::seconds_get() const {
    return _seconds;
}


void SpineTrack::alpha_set(float value) {
    if (_alpha == value) return;

    _alpha = value;
    _change_notify("alpha");

    SpineSprite* sprite = _get_spine_sprite();
    if (!sprite) return;

    sprite->update();
}

float SpineTrack::alpha_get() const {
    return _alpha;
}

void SpineTrack::_update_cache() {
    SpineSprite* sprite = Object::cast_to<SpineSprite>(get_node_or_null(_spine_sprite_path));
    if (!sprite) return;
    _spine_sprite_id = sprite->get_instance_id();
}

SpineSprite* SpineTrack::_get_spine_sprite() const {
    if (!_spine_sprite_id) return NULL;
    return Object::cast_to<SpineSprite>(ObjectDB::get_instance(_spine_sprite_id));
}
