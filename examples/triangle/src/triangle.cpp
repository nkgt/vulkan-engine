#include "nkgt/engine.hpp"
#include <cstdlib>

int main() {
    nkgt::Engine e("Triangle", 800, 600);
    e.run();

    return EXIT_SUCCESS;
}