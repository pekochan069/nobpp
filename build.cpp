#include "nobpp.hpp"

int main() {
    nobpp::CommandBuilder builder = nobpp::CommandBuilder();

    builder.set_language(nobpp::Language::cpp)
        .set_target_os(nobpp::TargetOS::windows)
        .set_optimization_level(nobpp::OptimizationLevel::o3)
        .add_options({"-ffast-math"})
        .add_file("./test.cpp")
        .set_output("test")
        .run();

    return 0;
}
