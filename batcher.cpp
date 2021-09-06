#include "batcher.hpp"

Batcher::Batcher(Node2D* node) : _canvas_item(node) {}

Batcher::~Batcher() {}

void Batcher::add(
    const Vector<int>& indices,
    const Vector<Vector2>& vertices,
    const Vector<Color>& colors,
    const Vector<Vector2>& uvs,
    Ref<Texture> texture) {
        
    if (texture.is_null()) return;
    
    if (_batch.texture.is_null()) _batch.texture = texture;
    
    if (texture->get_rid() != _batch.texture->get_rid()
    || (vertices.size() + _batch.vertices.size()) >= 1024) {
        _batches.push_back(_batch);
        
        _batch.indices.clear();
        _batch.vertices.clear();
        _batch.colors.clear();
        _batch.uvs.clear();

        _batch.texture = texture;
    }

    int vertex_count = _batch.vertices.size();
    for (int i = 0; i < indices.size(); i++) {
        _batch.indices.push_back(indices[i] + vertex_count);
    }

    _batch.vertices.append_array(vertices);
    _batch.colors.append_array(colors);
    _batch.uvs.append_array(uvs);
}

void Batcher::flush() {
    if (_batch.texture.is_null()) return;

    _batches.push_back(_batch);
    _batch = Batch();

    for (int i = 0; i < _batches.size(); i++) {
        const Batch& batch = _batches.at(i);

        VisualServer::get_singleton()->canvas_item_add_triangle_array(
            _canvas_item->get_canvas_item(),
            batch.indices,
            batch.vertices,
            batch.colors,
            batch.uvs,
            Vector<int>(),
            Vector<float>(),
            batch.texture->get_rid());
    }

    _batches.clear();
}