#pragma once

#include "scene/main/node.h"

namespace animation {

class SpineSprite;

class SpineTrack : public Node {
    GDCLASS(SpineTrack, Node)

private:
    const StringName NoneAnimation = StringName("<empty>");

    NodePath _spine_sprite_path;
    ObjectID _spine_sprite_id;

    String _animation;
    float _seconds;
    float _alpha;

    void _update_cache();
    SpineSprite* _get_spine_sprite() const;

protected:
    static void _bind_methods();
    void _validate_property(PropertyInfo &property) const;

public:
    SpineTrack();
    ~SpineTrack();
    
    void _notification(int what);

    void spine_sprite_path_set(NodePath path);
    NodePath spine_sprite_path_get() const;

    void animation_name_set(String name);
    String animation_name_get() const;

    void seconds_set(float value);
    float seconds_get() const;

    void alpha_set(float alpha);
    float alpha_get() const;
};

}