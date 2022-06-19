#include "GALCreateContextsBase.hpp"
#include "GAL/GALSDL2.hpp"

int main(int argc, char *argv[]) {
    RunFromWindow(R1::GAL::SDL2::CreateInstance,
        "Created SDL2 instance", "Failed to create SDL2 instance",
        R1::GAL::SDL2::GetRequiredWindowFlags());
};
