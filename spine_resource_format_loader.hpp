#pragma once

#include "core/io/resource_loader.h"
#include "core/io/resource_saver.h"

#include "spine_data.hpp"

namespace animation {

class ResourceFormatLoaderSpine : public ResourceFormatLoader {
    GDCLASS(ResourceFormatLoaderSpine, ResourceFormatLoader)

public:
    RES load(const String& p_path, const String& p_original_path, Error* r_error) {
        Ref<SpineData> spine_data = memnew(SpineData);
        if (r_error) {
            *r_error = OK;
        }

        spine_data->load_data(p_path);
        return spine_data;
    }

    virtual void get_recognized_extensions(List<String>* r_extensions) const {
        if (!r_extensions->find("spine-animation")) {
                r_extensions->push_back("spine-animation");
        }
    }

    virtual String get_resource_type(const String& p_path) const {
        String extension = p_path.get_extension().to_lower();

        if (extension == "spine-animation") {
            return "SpineData";
        }

        return "";
    }

    virtual bool handles_type(const String &p_type) const {
        return p_type == "SpineData";
    }
};

class ResourceFormatSaverSpine : public ResourceFormatSaver {
    virtual Error save(const String& path, const RES& resource, uint32_t flags = 0) {
        Ref<SpineData> data = memnew(SpineData);
        Error error = data->save_data(path, resource);
        if (error != OK) {
            print_error("Error when trying to save the same ");
        }
        return error;
    }

    virtual bool recognize(const RES& resource) const {
        return Object::cast_to<SpineData>(*resource) != NULL;
    }

    virtual void get_recognized_extensions(const RES& resource, List<String>* r_extensions) const {
        if (Object::cast_to<SpineData>(*resource)) {
            r_extensions->push_back("spine-animation");
        }
    }
};

class ResourceFormatLoaderSpineMixSettings : public ResourceFormatLoader {
    GDCLASS(ResourceFormatLoaderSpineMixSettings, ResourceFormatLoader)

public:
    RES load(const String& p_path, const String& p_original_path, Error* r_error) {
        Ref<SpineMixSettings> mix_settings = memnew(SpineMixSettings);
        if (r_error) {
            *r_error = OK;
        }

        mix_settings->load_data(p_path);
        return mix_settings;
    }

    virtual void get_recognized_extensions(List<String>* r_extensions) const {
        if (!r_extensions->find("skel-mix")) {
                r_extensions->push_back("skel-mix");
        }
    }

    virtual String get_resource_type(const String& p_path) const {
        String extension = p_path.get_extension().to_lower();

        if (extension == "skel-mix") {
            return "SpineMixSettings";
        }

        return "";
    }

    virtual bool handles_type(const String &p_type) const {
        return p_type == "SpineMixSettings";
    }
};

}