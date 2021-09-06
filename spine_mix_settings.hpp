#pragma once

#include <string>
#include <unordered_map>

#include <spine/SpineString.h>

#include "core/resource.h"

namespace animation {
class SpineMixSettings : public Resource {
    GDCLASS(SpineMixSettings, Resource);
    
    float _default;
    Map<String, float> _defaults_out;
    Map<String, float> _defaults_in;
    Map<String, Map<String, float>> _specific_mixes;

protected:
    static void _bind_methods() {
        ClassDB::bind_method(D_METHOD("set_default", "seconds"), &SpineMixSettings::set_default);
        ClassDB::bind_method(D_METHOD("get_default"), &SpineMixSettings::get_default);

        ClassDB::bind_method(D_METHOD("add_mix", "from", "to", "seconds"), &SpineMixSettings::add_mix);

        ADD_PROPERTY(PropertyInfo(Variant::REAL, "default_mix", PROPERTY_HINT_RANGE, "0"), "set_default", "get_default");
    }

public:
    void load_data(String data);

    void set_default(float duration);
    float get_default() const;

    void add_mix(String from_animation, String to_animation, float duration);
    float get_mix(String from_animation, String to_animation) const;
};

}