#include "register_types.h"

#include <spine/Extension.h>

#include "core/class_db.h"
#include "spine_data.hpp"
#include "spine_sprite.hpp"
#include "spine_track.hpp"
#include "spine_resource_format_loader.hpp"

static Ref<animation::ResourceFormatLoaderSpine> spine_loader;
static Ref<animation::ResourceFormatLoaderSpineMixSettings> spine_mix_loader;
static Ref<animation::ResourceFormatSaverSpine> spine_saver;

spine::SpineExtension* spine::getDefaultExtension() {
   return new spine::DefaultSpineExtension();
}

void register_spine_types() {
    ClassDB::register_class<animation::SpineData>();
    ClassDB::register_class<animation::SpineSprite>();
    ClassDB::register_class<animation::SpineTrack>();
    ClassDB::register_class<animation::SpineMixSettings>();

    spine_loader.instance();
    spine_saver.instance();
    spine_mix_loader.instance();

    ResourceLoader::add_resource_format_loader(spine_loader);
    ResourceSaver::add_resource_format_saver(spine_saver);
    ResourceLoader::add_resource_format_loader(spine_mix_loader);
}

void unregister_spine_types() {
    ResourceLoader::remove_resource_format_loader(spine_loader);
    ResourceSaver::remove_resource_format_saver(spine_saver);
    ResourceLoader::remove_resource_format_loader(spine_mix_loader);

    spine_loader.unref();
    spine_saver.unref();
    spine_mix_loader.unref();
}
