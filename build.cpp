#include "nobpp.hpp"

int main() {
    nobpp::CommandBuilder builder = nobpp::CommandBuilder();

    builder.set_language(nobpp::Language::cpp)
        .set_target_os(nobpp::TargetOS::windows)
        .set_mode(nobpp::Mode::release)
        .set_optimization_level(nobpp::OptimizationLevel::o3)
        .add_options({"-ffast-math"})
        .add_file("./test.cpp")
        .set_build_dir("./bin")
        .set_output("test")
        .run();

    nobpp::CommandBuilder builder1 = nobpp::CommandBuilder();

    builder1.set_language(nobpp::Language::cpp)
        .set_target_os(nobpp::TargetOS::windows)
        .set_optimization_level(nobpp::OptimizationLevel::o3)
        .add_options({"-ffast-math"})
        .add_file("./test.cpp")
        .add_files("./src")
        .set_build_dir("./bin")
        .set_output("test");

    nobpp::CommandBuilder builder2 = nobpp::CommandBuilder();

    builder2.set_language(nobpp::Language::cpp)
        .set_target_os(nobpp::TargetOS::windows)
        .set_optimization_level(nobpp::OptimizationLevel::o3)
        .add_options({"-ffast-math"})
        .add_file("./test.cpp")
        .add_files("./src")
        .set_build_dir("./bin")
        .set_output("test2");

    nobpp::CommandBuilder builder3 = nobpp::CommandBuilder();

    builder3.set_language(nobpp::Language::cpp)
        .set_target_os(nobpp::TargetOS::windows)
        .set_optimization_level(nobpp::OptimizationLevel::o3)
        .add_options({"-ffast-math"})
        .add_file("./test.cpp")
        .add_files("./src")
        .set_build_dir("./bin")
        .set_output("test3");

    nobpp::CommandQueue(2)
        .add_builder(builder1)
        .add_builder(builder2)
        .add_builder(builder3);

    return 0;
}
