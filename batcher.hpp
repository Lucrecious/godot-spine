#pragma once

#include <vector>

#include "scene/2d/node_2d.h"

class Batcher {
    struct Batch {
        Vector<int> indices;   
        Vector<Vector2> vertices;
        Vector<Color> colors;
        Vector<Vector2> uvs;
        Ref<Texture> texture;
    };

    std::vector<Batch> _batches;
    Batch _batch;
    Node2D* _canvas_item;

public:
    Batcher(Node2D* node);
    ~Batcher();

    void add(
        const Vector<int>& indices,
        const Vector<Vector2>& vertices,
        const Vector<Color>& colors,
        const Vector<Vector2>& uvs,
        Ref<Texture> texture);
    
    void flush();
};