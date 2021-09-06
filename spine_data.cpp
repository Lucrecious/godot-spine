#include "spine_data.hpp"

#include <spine/Animation.h>
#include <spine/SkeletonBinary.h>

#include "core/io/json.h"
#include "core/os/file_access.h"
#include "core/project_settings.h"

using namespace animation;

SpineData::SpineData() : _atlas(NULL), _skeleton_data(NULL), _animation_state_data(NULL) {}

void SpineData::_bind_methods() {
    ClassDB::bind_method(D_METHOD("load_data", "path"), &SpineData::load_data);
    ClassDB::bind_method(D_METHOD("num_animations"), &SpineData::num_animations);
    ClassDB::bind_method(D_METHOD("get_animations"), &SpineData::get_animations);

    ClassDB::bind_method(D_METHOD("mix_settings_set", "settings"), &SpineData::spine_mix_settings_set);
    ClassDB::bind_method(D_METHOD("mix_settings_get"), &SpineData::spine_mix_settings_get);

   	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "mix_settings", PROPERTY_HINT_RESOURCE_TYPE, "SpineMixSettings"), "mix_settings_set", "mix_settings_get");
}

void SpineData::load_data(const String& path) {
    _free_data();

    Error err;
    String content = FileAccess::get_file_as_string(path, &err);

    if (err != OK) {
        print_error(path + " not found.");
        return;
    }

    Variant parsed_content;
    String __error_str;
    int __error_line;
    err = JSON::parse(content, parsed_content, __error_str, __error_line);

    if (err != OK) {
        print_error("Error parsing " + path + " as a json.");
        return;
    }

    
    if (parsed_content.get_type() != Variant::DICTIONARY) {
        print_error(path + " is in an invalid type");
        return;
    }

    String resource_directory = path.get_base_dir();
    Dictionary resources = (Dictionary)parsed_content;
    String atlas_path = ProjectSettings::get_singleton()->globalize_path(resource_directory + "/" + (String)resources.get("atlas", ""));
    String skel_path = ProjectSettings::get_singleton()->globalize_path(resource_directory + "/" + (String)resources.get("skel", ""));
    String mix_path = resource_directory + "/" + (String)resources.get("mix", "");

    _atlas = new spine::Atlas(spine::String(atlas_path.utf8().get_data()), &spine_texture_loader);

    if (_atlas->getPages().size() == 0) {
        _free_data();
        print_error(atlas_path + " does not exist.");
        return;
    }

    spine::SkeletonBinary skeleton_binary(_atlas);
    skeleton_binary.setScale(1.0);

    _skeleton_data = skeleton_binary.readSkeletonDataFile(spine::String(skel_path.utf8().get_data()));
    if (!_skeleton_data) {
        print_error(skel_path + " was not able to be loaded.");
        _free_data();
        return;
    }

    _animation_state_data = new spine::AnimationStateData(_skeleton_data);

    if (!mix_path.is_resource_file()) return;
    
    spine_mix_settings_set(ResourceLoader::load(mix_path));
}

Error SpineData::save_data(const String& path, const RES& resource) {
    Error error;
    Ref<SpineData> spine_data_ref = resource.get_ref_ptr();
    JSON json;

    String __error_str;
    int __error_line;
    Variant json_ret;
    error = json.parse(FileAccess::get_file_as_string(path), json_ret, __error_str, __error_line);
    if (error != OK) return error;

    FileAccess* file = FileAccess::open(path, FileAccess::WRITE, &error);
    if (error != OK) {
        if (file) file->close();
        return error;
    }

    if (error != OK || json_ret.get_type() != Variant::DICTIONARY) {
        if (file) file->close();
        return error;
    }

    Dictionary dict = (Dictionary)json_ret;

    if (!spine_data_ref.is_null() && !spine_data_ref->spine_mix_settings_get().is_null()) {
        dict["mix"] = path.get_base_dir().path_to_file(spine_data_ref->spine_mix_settings_get()->get_path().get_file());
    } else {
        dict.erase("mix");
    }

    file->store_string(json.print(dict, "    "));
    file->close();

    return error;
}

PoolStringArray SpineData::get_animations() const {
    PoolStringArray names;

    if (!_skeleton_data) return names;
    
    spine::Vector<spine::Animation*>& animations = _skeleton_data->getAnimations();
    for (int i = 0; i < animations.size(); i++) {
        names.append(String(animations[i]->getName().buffer()));
    }

    return names;
}

spine::Animation* SpineData::get_animation(const String& name) const {
    if (!_skeleton_data) return NULL;
    if (name.empty()) return NULL;

    return _skeleton_data->findAnimation(spine::String(name.utf8().get_data()));
}

int SpineData::num_animations() const {
    if (!_skeleton_data) return 0;
    return _skeleton_data->getAnimations().size();
}

void SpineData::spine_mix_settings_set(Ref<SpineMixSettings> mix_settings) {
    if (mix_settings == _spine_mix_settings) return;
    if (!_animation_state_data) {
        print_error("No Animation State Data... Can't set Mix Settings...");
        return;
    }

    _spine_mix_settings = mix_settings;
}

Ref<SpineMixSettings> SpineData::spine_mix_settings_get() const {
    return _spine_mix_settings;
}

spine::Skeleton* SpineData::create_skeleton() const {
    if (!_skeleton_data) return NULL;

    auto skeleton = new spine::Skeleton(_skeleton_data);

    if (_skeleton_data->getSkins().size() > 0) {
        skeleton->setSkin(_skeleton_data->getSkins()[0]);
    } else {
        skeleton->setSkin(_skeleton_data->getDefaultSkin());
    }

    return skeleton;
}

spine::AnimationState* SpineData::create_state() const {
    if (!_animation_state_data) return NULL;

    auto animation_state = new spine::AnimationState(_animation_state_data);

    return animation_state;
}

void SpineData::apply(spine::Animation* animation, spine::Skeleton& skeleton, float time, bool loop, float alpha, spine::MixBlend mix_blend, spine::MixDirection direction) const {
    if (!animation) {
        print_error( "SpineData::apply animation can't be null");
        return;
    }

    auto sec = time * animation->getDuration();

    animation->apply(skeleton, sec, sec, false, NULL, 1.0, mix_blend, direction);
}

void SpineData::_free_data() {
    if (_atlas) delete _atlas;
    if (_skeleton_data) delete _skeleton_data;
    if (_animation_state_data) delete _animation_state_data;
    _atlas = NULL;
    _skeleton_data = NULL;
    _animation_state_data = NULL;
}

SpineData::~SpineData() {
    _free_data();
}