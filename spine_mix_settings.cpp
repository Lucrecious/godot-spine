#include "spine_mix_settings.hpp"

#include "core/os/file_access.h"

using namespace animation;

void SpineMixSettings::set_default(float duration) {
    _default = duration;
}

float SpineMixSettings::get_default() const {
    return _default;
}

void SpineMixSettings::add_mix(String from_animation, String to_animation, float duration) {
    if (from_animation == "*" && to_animation == "*") return;

    if (to_animation == "*") {
        _defaults_out[from_animation] = duration;
        return;
    }

    if (from_animation == "*") {
        _defaults_in[to_animation] = duration;
        return;
    }


    if (!_specific_mixes.find(from_animation)) {
        _specific_mixes[from_animation] = Map<String, float>();
    }

    _specific_mixes[from_animation][to_animation] = duration;
}

float SpineMixSettings::get_mix(String from_animation, String to_animation) const {
    const auto from_mixes = _specific_mixes.find(from_animation);
    if (from_mixes) {
        const auto to_mix = from_mixes->value().find(to_animation);
        if (to_mix) return to_mix->value();
    }

    const auto default_out = _defaults_out.find(from_animation);
    if (default_out) return default_out->value();

    const auto default_in = _defaults_in.find(to_animation);
    if (default_in) return default_in->value();

    return _default;
}

void SpineMixSettings::load_data(String path) {
    Error err;
    String settings = FileAccess::get_file_as_string(path, &err);

    Vector<String> lines =  settings.split("\n", false);

    for (int i = 0; i < lines.size(); i++) {
        Vector<String> params = lines[i].split(" ", false);

        if (params.size() == 1) {
            _default = params[0].to_float();
        } else if (params.size() == 3) {
            add_mix(params[0], params[1], params[2].to_float());
        }
    }
}






