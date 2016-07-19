#include <iostream>
#include <vector>

const std::string Names[] = {"NW", "NE", "SW", "SE"};

struct Rectangle {
    float x;
    float y;
    float width;
    float height;

    Rectangle() {}
    Rectangle(float x, float y, float width, float height) : x(x), y(y), width(width), height(height) {}

    bool contains(const Rectangle& other) {
        return other.x >= x && other.y >= y &&
                other.width <= width && other.height <= height;
    }

    bool intersects(const Rectangle& other) {
        return x < (other.x + other.width) && (x + width) > other.x &&
                y < (other.y + other.height) && (y + height) > other.y;
    }

    void print() {
        std::cout << "{" << x << ", " << y << ", " << width << ", " << height << "}\n";
    }
};

struct Quadtree {
    Rectangle box;
    Quadtree* children[4];
    std::vector<Rectangle*> objects;

    Quadtree(Rectangle box) : box(box) {
        for (int i = 0; i < 4; i++) {
            children[i] = nullptr;
        }
    }

    Quadtree(float x, float y, float width, float height) : Quadtree(Rectangle(x, y, width, height)) {}

    void insert(Rectangle& rect) {
        for (int i = 0; i < 4; i++) {
            if (children[i] != nullptr && children[i]->box.contains(rect)) {
                children[i]->insert(rect);
                return;
            }
        }

        //Rectangle does not fit within children or there's no children
        objects.push_back(&rect);
    }

    void subdivide() {
        float halfWidth = box.width / 2.0;
        float halfHeight = box.height / 2.0;

        for (int i = 0; i < 4; i++) {
            children[i] = new Quadtree(Rectangle((i % 2) * halfWidth, (i / 2) * halfHeight, halfWidth, halfHeight));
        }

        std::vector<Rectangle*> copy(objects);
        objects.clear();
        for (Rectangle* object : copy) {
            insert(*object);
        }
    }

    void print(int depth = 0) {
        std::cout << "\nObjects at depth " << depth << ": " << objects.size() << "\n";
        for (Rectangle* rect : objects) {
            rect->print();
        }

        for (int i = 0; i < 4; i++) {
            std::cout << "At " << Names[i] << ": ";
            if (children[i] == nullptr) {
                std::cout << "Nothing!\n";
            } else {
                children[i]->print(depth + 1);
            }
        }
    }

    std::vector<Rectangle*> retrieve(const Rectangle& rect) {
        std::vector<Rectangle*> candidates(objects);

        for (int i = 0; i < 4; i++) {
            if (children[i] != nullptr && children[i]->box.intersects(rect)) {
                std::vector<Rectangle*> childCandidates = children[i]->retrieve(rect);

                candidates.insert(candidates.end(), childCandidates.begin(), childCandidates.end());
            }
        }

        return candidates;
    }
};

int main() {
    Quadtree tree(0, 0, 500, 500);

    std::vector<Rectangle> rects{
        {0, 0, 200, 200},
        {0, 300, 50, 50}
    };

    for (Rectangle& r : rects) {
        tree.insert(r);
    }

    tree.print();
    tree.subdivide();
    std::cout << "Divided!\n";
    tree.print();

    for (Rectangle* r : tree.retrieve(Rectangle(0, 250, 3, 60))) {
        r->print();
    }

    std::cout << "done\n";


}

