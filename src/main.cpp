#include "Application.h"

int main() {
    Application app{"compress main.cpp -o packed.myzip"};

    app.run();

    return 0;
}
