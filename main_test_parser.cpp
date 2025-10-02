#include "common/include/parser.hpp"
#include <filesystem>
#include <iostream>

#include <span>
#include <string_view>

int main(int argc, char ** argv) {
  std::span<char *> args(argv, static_cast<std::size_t>(argc));
  if (args.size() != 3) {
    std::string execName = std::filesystem::path(std::string_view(args[0])).string();
    std::cerr << "Usage: " << std::filesystem::path(execName).filename().string()
              << " <config.txt> <scene.txt>\n";
    return 1;
  }
  try {
    std::string_view config_path = args[1];
    std::string_view scene_path  = args[2];
    Config cfg                   = parseConfig(std::string(config_path));
    auto [mats, objs]            = parseScene(std::string(scene_path));
    std::cout << "Config loaded. image_width=" << cfg.image_width << ", gamma=" << cfg.gamma
              << "\n";
    std::cout << "Materials: " << mats.size() << "\n";
    for (auto & m : mats) {
      std::cout << " - " << m.name << "\n";
    }
    std::cout << "Objects: " << objs.size() << "\n";
    for (auto & o : objs) {
      std::cout << " - " << (o.type == ObjectType::Sphere ? "Sphere" : "Cylinder")
                << " material=" << o.material << "\n";
    }
    return 0;
  } catch (std::exception const & e) {
    std::cerr << e.what() << "\n";
    return 2;
  }
}
