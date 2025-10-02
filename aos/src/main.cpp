// aos/src/main.cpp
#include "parser.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <fstream>
#include <iostream>
#include <span>
#include <string_view>
#include <vector>

namespace {

  // Convierte valor en [0,1] -> 0..255 con corrección gamma
  int to_u8(double v, double gamma) {
    v                = std::clamp(v, 0.0, 1.0);
    double corrected = std::pow(v, 1.0 / gamma);
    int iv           = static_cast<int>(std::floor(corrected * 255.0 + 0.5));
    return std::clamp(iv, 0, 255);
  }

}  // namespace

namespace {

  // Escribe PPM (variante P3) siguiendo la especificación del enunciado:
  // la segunda línea es "número de líneas" (height) seguido de "número de columnas" (width)
  void write_ppm(std::string const & path, int width, int height,
                 std::vector<std::array<int, 3>> const & pixels) {
    std::ofstream ofs(path);
    if (!ofs) {
      std::cerr << "Error: Could not open output file: " << path << "\n";
      std::exit(3);
    }
    ofs << "P3\n" << height << " " << width << "\n255\n";
    for (int r = 0; r < height; ++r) {
      for (int c = 0; c < width; ++c) {
        auto p =
            pixels[static_cast<size_t>(r) * static_cast<size_t>(width) + static_cast<size_t>(c)];
        ofs << p[0] << " " << p[1] << " " << p[2] << "\n";
      }
    }
  }

}  // namespace

namespace {

  int validate_args(std::span<char *> args) {
    if (args.size() != 4) {
      std::cerr << "Error: Invalid number of arguments: " << (args.size() - 1) << "\n";
      return 1;
    }
    return 0;
  }

  std::vector<std::array<int, 3>> generate_gradient_image(Config const & cfg, int & width,
                                                          int & height) {
    width  = cfg.image_width;
    height = (cfg.image_width * cfg.aspect_ratio.second) / cfg.aspect_ratio.first;
    std::vector<std::array<int, 3>> pixels(static_cast<size_t>(width) *
                                           static_cast<size_t>(height));
    for (int y = 0; y < height; ++y) {
      double t = (height == 1) ? 0.0 : double(y) / double(height - 1);
      for (int x = 0; x < width; ++x) {
        double rr = (1.0 - t) * cfg.background_light_color[0] + t * cfg.background_dark_color[0];
        double gg = (1.0 - t) * cfg.background_light_color[1] + t * cfg.background_dark_color[1];
        double bb = (1.0 - t) * cfg.background_light_color[2] + t * cfg.background_dark_color[2];
        pixels[static_cast<size_t>(y) * static_cast<size_t>(width) + static_cast<size_t>(x)] = {
          to_u8(rr, cfg.gamma), to_u8(gg, cfg.gamma), to_u8(bb, cfg.gamma)};
      }
    }
    return pixels;
  }

  int run(int argc, char ** argv) {
    std::span<char *> args(argv, static_cast<size_t>(argc));
    int arg_status = validate_args(args);
    if (arg_status != 0) {
      return arg_status;
    }

    std::string_view cfg_path   = args[1];
    std::string_view scene_path = args[2];
    std::string_view out_path   = args[3];

    try {
      Config cfg                = parseConfig(std::string(cfg_path));
      auto [materials, objects] = parseScene(std::string(scene_path));

      int width = 0, height = 0;
      auto pixels = generate_gradient_image(cfg, width, height);

      write_ppm(std::string(out_path), width, height, pixels);
      std::cout << "Wrote " << out_path << " (" << width << "x" << height << ")\n";
    } catch (std::exception const & e) {
      std::cerr << e.what() << "\n";
      return 2;
    }
    return 0;
  }

}  // end anonymous namespace

int main(int argc, char ** argv) {
  return run(argc, argv);
}
