#include "parser.hpp"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

// --- helpers ---
namespace {

  std::string trim(std::string const & s) {
    size_t a = 0, b = s.size();
    while ((a < b) and (std::isspace(static_cast<unsigned char>(s[a])) != 0)) {
      ++a;
    }
    while ((b > a) and (std::isspace(static_cast<unsigned char>(s[b - 1])) != 0)) {
      --b;
    }
    return s.substr(a, b - a);
  }

  bool is_blank(std::string const & s) {
    return std::ranges::all_of(
        s, [](char c) { return std::isspace(static_cast<unsigned char>(c)) != 0; });
  }

  std::vector<std::string> split_ws(std::string const & line) {
    std::istringstream iss(line);
    std::vector<std::string> toks;
    std::string tok;
    while (iss >> tok) {
      toks.push_back(tok);
    }
    return toks;
  }

  double parse_double_strict(std::string const & tok) {
    try {
      size_t pos = 0;
      double v   = std::stod(tok, &pos);
      if (pos != tok.size()) {
        throw std::invalid_argument("trailing");
      }
      return v;
    } catch (...) {
      throw;
    }
  }

  int parse_int_strict(std::string const & tok) {
    try {
      size_t pos = 0;
      int v      = std::stoi(tok, &pos);
      if (pos != tok.size()) {
        throw std::invalid_argument("trailing");
      }
      return v;
    } catch (...) {
      throw;
    }
  }

  std::string join_from(std::vector<std::string> const & toks, size_t i) {
    std::string r;
    for (size_t j = i; j < toks.size(); ++j) {
      if (j > i) {
        r += " ";
      }
      r += toks[j];
    }
    return r;
  }

}  // anonymous namespace

// --- parseConfig ---
namespace {

  void parse_image_width(std::vector<std::string> const & toks, std::string const & raw,
                         Config & cfg) {
    if (toks.size() < 2) {
      throw std::runtime_error(
          "Error: Invalid value for key: [image_width:]\nLine: \"" + raw + "\"");
    }
    if (toks.size() > 2) {
      throw std::runtime_error(
          "Error: Extra data after configuration value for key: [image_width:]\nExtra: \"" +
          join_from(toks, 2) +
          "\"\nLine: \"" +
          raw +
          "\"");
    }
    int w = parse_int_strict(toks[1]);
    if (w <= 0) {
      throw std::runtime_error(
          "Error: Invalid value for key: [image_width:]\nLine: \"" + raw + "\"");
    }
    cfg.image_width = w;
  }

  void parse_aspect_ratio(std::vector<std::string> const & toks, std::string const & raw,
                          Config & cfg) {
    if (toks.size() < 3) {
      throw std::runtime_error(
          "Error: Invalid value for key: [aspect_ratio:]\nLine: \"" + raw + "\"");
    }
    if (toks.size() > 3) {
      throw std::runtime_error(
          "Error: Extra data after configuration value for key: [aspect_ratio:]\nExtra: \"" +
          join_from(toks, 3) +
          "\"\nLine: \"" +
          raw +
          "\"");
    }
    int a = parse_int_strict(toks[1]);
    int b = parse_int_strict(toks[2]);
    if ((a <= 0) or (b <= 0)) {
      throw std::runtime_error(
          "Error: Invalid value for key: [aspect_ratio:]\nLine: \"" + raw + "\"");
    }
    cfg.aspect_ratio = {a, b};
  }

  void parse_gamma(std::vector<std::string> const & toks, std::string const & raw, Config & cfg) {
    if (toks.size() < 2) {
      throw std::runtime_error("Error: Invalid value for key: [gamma:]\nLine: \"" + raw + "\"");
    }
    if (toks.size() > 2) {
      throw std::runtime_error(
          "Error: Extra data after configuration value for key: [gamma:]\nExtra: \"" +
          join_from(toks, 2) +
          "\"\nLine: \"" +
          raw +
          "\"");
    }
    cfg.gamma = parse_double_strict(toks[1]);
  }

  void parse_camera_position(std::vector<std::string> const & toks, std::string const & raw,
                             Config & cfg) {
    if (toks.size() < 4) {
      throw std::runtime_error(
          "Error: Invalid value for key: [camera_position:]\nLine: \"" + raw + "\"");
    }
    if (toks.size() > 4) {
      throw std::runtime_error(
          "Error: Extra data after configuration value for key: [camera_position:]\nExtra: \"" +
          join_from(toks, 4) +
          "\"\nLine: \"" +
          raw +
          "\"");
    }
    cfg.camera_position = {parse_double_strict(toks[1]), parse_double_strict(toks[2]),
                           parse_double_strict(toks[3])};
  }

  void parse_camera_target(std::vector<std::string> const & toks, std::string const & raw,
                           Config & cfg) {
    if (toks.size() < 4) {
      throw std::runtime_error(
          "Error: Invalid value for key: [camera_target:]\nLine: \"" + raw + "\"");
    }
    if (toks.size() > 4) {
      throw std::runtime_error(
          "Error: Extra data after configuration value for key: [camera_target:]\nExtra: \"" +
          join_from(toks, 4) +
          "\"\nLine: \"" +
          raw +
          "\"");
    }
    cfg.camera_target = {parse_double_strict(toks[1]), parse_double_strict(toks[2]),
                         parse_double_strict(toks[3])};
  }

  void parse_camera_north(std::vector<std::string> const & toks, std::string const & raw,
                          Config & cfg) {
    if (toks.size() < 4) {
      throw std::runtime_error(
          "Error: Invalid value for key: [camera_north:]\nLine: \"" + raw + "\"");
    }
    if (toks.size() > 4) {
      throw std::runtime_error(
          "Error: Extra data after configuration value for key: [camera_north:]\nExtra: \"" +
          join_from(toks, 4) +
          "\"\nLine: \"" +
          raw +
          "\"");
    }
    cfg.camera_north = {parse_double_strict(toks[1]), parse_double_strict(toks[2]),
                        parse_double_strict(toks[3])};
  }

  void parse_field_of_view(std::vector<std::string> const & toks, std::string const & raw,
                           Config & cfg) {
    if (toks.size() < 2) {
      throw std::runtime_error(
          "Error: Invalid value for key: [field_of_view:]\nLine: \"" + raw + "\"");
    }
    if (toks.size() > 2) {
      throw std::runtime_error(
          "Error: Extra data after configuration value for key: [field_of_view:]\nExtra: \"" +
          join_from(toks, 2) +
          "\"\nLine: \"" +
          raw +
          "\"");
    }
    double f = parse_double_strict(toks[1]);
    if ((f <= 0.0) or (f >= 180.0)) {
      throw std::runtime_error(
          "Error: Invalid value for key: [field_of_view:]\nLine: \"" + raw + "\"");
    }
    cfg.field_of_view = f;
  }

  void parse_samples_per_pixel(std::vector<std::string> const & toks, std::string const & raw,
                               Config & cfg) {
    if (toks.size() < 2) {
      throw std::runtime_error(
          "Error: Invalid value for key: [samples_per_pixel:]\nLine: \"" + raw + "\"");
    }
    if (toks.size() > 2) {
      throw std::runtime_error(
          "Error: Extra data after configuration value for key: [samples_per_pixel:]\nExtra: \"" +
          join_from(toks, 2) +
          "\"\nLine: \"" +
          raw +
          "\"");
    }
    int s = parse_int_strict(toks[1]);
    if (s <= 0) {
      throw std::runtime_error(
          "Error: Invalid value for key: [samples_per_pixel:]\nLine: \"" + raw + "\"");
    }
    cfg.samples_per_pixel = s;
  }

  void parse_max_depth(std::vector<std::string> const & toks, std::string const & raw,
                       Config & cfg) {
    if (toks.size() < 2) {
      throw std::runtime_error("Error: Invalid value for key: [max_depth:]\nLine: \"" + raw + "\"");
    }
    if (toks.size() > 2) {
      throw std::runtime_error(
          "Error: Extra data after configuration value for key: [max_depth:]\nExtra: \"" +
          join_from(toks, 2) +
          "\"\nLine: \"" +
          raw +
          "\"");
    }
    int m = parse_int_strict(toks[1]);
    if (m <= 0) {
      throw std::runtime_error("Error: Invalid value for key: [max_depth:]\nLine: \"" + raw + "\"");
    }
    cfg.max_depth = m;
  }

  void parse_material_rng_seed(std::vector<std::string> const & toks, std::string const & raw,
                               Config & cfg) {
    if (toks.size() < 2) {
      throw std::runtime_error(
          "Error: Invalid value for key: [material_rng_seed:]\nLine: \"" + raw + "\"");
    }
    if (toks.size() > 2) {
      throw std::runtime_error(
          "Error: Extra data after configuration value for key: [material_rng_seed:]\nExtra: \"" +
          join_from(toks, 2) +
          "\"\nLine: \"" +
          raw +
          "\"");
    }
    int v = parse_int_strict(toks[1]);
    if (v <= 0) {
      throw std::runtime_error(
          "Error: Invalid value for key: [material_rng_seed:]\nLine: \"" + raw + "\"");
    }
    cfg.material_rng_seed = v;
  }

  void parse_ray_rng_seed(std::vector<std::string> const & toks, std::string const & raw,
                          Config & cfg) {
    if (toks.size() < 2) {
      throw std::runtime_error(
          "Error: Invalid value for key: [ray_rng_seed:]\nLine: \"" + raw + "\"");
    }
    if (toks.size() > 2) {
      throw std::runtime_error(
          "Error: Extra data after configuration value for key: [ray_rng_seed:]\nExtra: \"" +
          join_from(toks, 2) +
          "\"\nLine: \"" +
          raw +
          "\"");
    }
    int v = parse_int_strict(toks[1]);
    if (v <= 0) {
      throw std::runtime_error(
          "Error: Invalid value for key: [ray_rng_seed:]\nLine: \"" + raw + "\"");
    }
    cfg.ray_rng_seed = v;
  }

  void parse_background_dark_color(std::vector<std::string> const & toks, std::string const & raw,
                                   Config & cfg) {
    if (toks.size() < 4) {
      throw std::runtime_error(
          "Error: Invalid value for key: [background_dark_color:]\nLine: \"" + raw + "\"");
    }
    if (toks.size() > 4) {
      throw std::runtime_error("Error: Extra data after configuration value for key: "
                               "[background_dark_color:]\nExtra: \"" +
                               join_from(toks, 4) +
                               "\"\nLine: \"" +
                               raw +
                               "\"");
    }
    double r = parse_double_strict(toks[1]);
    double g = parse_double_strict(toks[2]);
    double b = parse_double_strict(toks[3]);
    if ((r < 0.0) or (r > 1.0) or (g < 0.0) or (g > 1.0) or (b < 0.0) or (b > 1.0)) {
      throw std::runtime_error(
          "Error: Invalid value for key: [background_dark_color:]\nLine: \"" + raw + "\"");
    }
    cfg.background_dark_color = {r, g, b};
  }

  void parse_background_light_color(std::vector<std::string> const & toks, std::string const & raw,
                                    Config & cfg) {
    if (toks.size() < 4) {
      throw std::runtime_error(
          "Error: Invalid value for key: [background_light_color:]\nLine: \"" + raw + "\"");
    }
    if (toks.size() > 4) {
      throw std::runtime_error("Error: Extra data after configuration value for key: "
                               "[background_light_color:]\nExtra: \"" +
                               join_from(toks, 4) +
                               "\"\nLine: \"" +
                               raw +
                               "\"");
    }
    double r = parse_double_strict(toks[1]);
    double g = parse_double_strict(toks[2]);
    double b = parse_double_strict(toks[3]);
    if ((r < 0.0) or (r > 1.0) or (g < 0.0) or (g > 1.0) or (b < 0.0) or (b > 1.0)) {
      throw std::runtime_error(
          "Error: Invalid value for key: [background_light_color:]\nLine: \"" + raw + "\"");
    }
    cfg.background_light_color = {r, g, b};
  }

  void dispatch_config_key(std::vector<std::string> const & toks, std::string const & raw,
                           Config & cfg) {
    std::string const & key = toks[0];
    if (key == "image_width:") {
      parse_image_width(toks, raw, cfg);
    } else if (key == "aspect_ratio:") {
      parse_aspect_ratio(toks, raw, cfg);
    } else if (key == "gamma:") {
      parse_gamma(toks, raw, cfg);
    } else if (key == "camera_position:") {
      parse_camera_position(toks, raw, cfg);
    } else if (key == "camera_target:") {
      parse_camera_target(toks, raw, cfg);
    } else if (key == "camera_north:") {
      parse_camera_north(toks, raw, cfg);
    } else if (key == "field_of_view:") {
      parse_field_of_view(toks, raw, cfg);
    } else if (key == "samples_per_pixel:") {
      parse_samples_per_pixel(toks, raw, cfg);
    } else if (key == "max_depth:") {
      parse_max_depth(toks, raw, cfg);
    } else if (key == "material_rng_seed:") {
      parse_material_rng_seed(toks, raw, cfg);
    } else if (key == "ray_rng_seed:") {
      parse_ray_rng_seed(toks, raw, cfg);
    } else if (key == "background_dark_color:") {
      parse_background_dark_color(toks, raw, cfg);
    } else if (key == "background_light_color:") {
      parse_background_light_color(toks, raw, cfg);
    } else {
      throw std::runtime_error(std::string("Error: Unknown configuration key: [") + key + "]");
    }
  }

}  // anonymous namespace

Config parseConfig(std::string const & filename) {
  Config cfg;
  std::ifstream ifs(filename);
  if (!ifs) {
    throw std::runtime_error("No se pudo abrir archivo: " + filename);
  }

  std::string raw;
  while (std::getline(ifs, raw)) {
    if (is_blank(raw)) {
      continue;
    }
    std::string line = trim(raw);
    auto toks        = split_ws(line);
    if (toks.empty()) {
      continue;
    }
    dispatch_config_key(toks, raw, cfg);
  }

  return cfg;
}

// --- parseScene ---
namespace {

  int find_material(std::vector<Material> const & materials, std::string const & name) {
    for (size_t i = 0; i < materials.size(); ++i) {
      if (materials[i].name == name) {
        return static_cast<int>(i);
      }
    }
    return -1;
  }

  void parse_matte(std::vector<std::string> const & toks, std::string const & raw,
                   std::vector<Material> & materials) {
    if (toks.size() < 5) {
      throw std::runtime_error("Error: Invalid matte material parameters\nLine: \"" + raw + "\"");
    }
    if (toks.size() > 5) {
      throw std::runtime_error(
          "Error: Extra data after configuration value for key: [matte:]\nExtra: \"" +
          join_from(toks, 5) +
          "\"\nLine: \"" +
          raw +
          "\"");
    }
    std::string name = toks[1];
    if (find_material(materials, name) != -1) {
      std::string msg = "Error: Material with name [";
      msg += name;
      msg += "] already exists\nLine: \"";
      msg += raw;
      msg += "\"";
      throw std::runtime_error(msg);
    }
    try {
      double r = parse_double_strict(toks[2]);
      double g = parse_double_strict(toks[3]);
      double b = parse_double_strict(toks[4]);
      if ((r < 0.0) or (r > 1.0) or (g < 0.0) or (g > 1.0) or (b < 0.0) or (b > 1.0)) {
        throw std::invalid_argument("range");
      }
      materials.push_back({
        name, MaterialType::Matte, {r, g, b}
      });
    } catch (...) {
      throw std::runtime_error("Error: Invalid matte material parameters\nLine: \"" + raw + "\"");
    }
  }

  void parse_metal(std::vector<std::string> const & toks, std::string const & raw,
                   std::vector<Material> & materials) {
    if (toks.size() < 6) {
      throw std::runtime_error("Error: Invalid metal material parameters\nLine: \"" + raw + "\"");
    }
    if (toks.size() > 6) {
      throw std::runtime_error(
          "Error: Extra data after configuration value for key: [metal:]\nExtra: \"" +
          join_from(toks, 6) +
          "\"\nLine: \"" +
          raw +
          "\"");
    }
    std::string name = toks[1];
    if (find_material(materials, name) != -1) {
      std::string msg = "Error: Material with name [";
      msg += name;
      msg += "] already exists\nLine: \"";
      msg += raw;
      msg += "\"";
      throw std::runtime_error(msg);
    }
    try {
      double r   = parse_double_strict(toks[2]);
      double g   = parse_double_strict(toks[3]);
      double b   = parse_double_strict(toks[4]);
      double phi = parse_double_strict(toks[5]);
      materials.push_back({
        name, MaterialType::Metal, {r, g, b, phi}
      });
    } catch (...) {
      throw std::runtime_error("Error: Invalid metal material parameters\nLine: \"" + raw + "\"");
    }
  }

  void parse_refractive(std::vector<std::string> const & toks, std::string const & raw,
                        std::vector<Material> & materials) {
    if (toks.size() < 3) {
      throw std::runtime_error(
          "Error: Invalid refractive material parameters\nLine: \"" + raw + "\"");
    }
    if (toks.size() > 3) {
      throw std::runtime_error(
          "Error: Extra data after configuration value for key: [refractive:]\nExtra: \"" +
          join_from(toks, 3) +
          "\"\nLine: \"" +
          raw +
          "\"");
    }
    std::string name = toks[1];
    if (find_material(materials, name) != -1) {
      std::string msg = "Error: Material with name [";
      msg += name;
      msg += "] already exists\nLine: \"";
      msg += raw;
      msg += "\"";
      throw std::runtime_error(msg);
    }
    try {
      double rho = parse_double_strict(toks[2]);
      materials.push_back({name, MaterialType::Refractive, {rho}});
    } catch (...) {
      throw std::runtime_error(
          "Error: Invalid refractive material parameters\nLine: \"" + raw + "\"");
    }
  }

  void parse_sphere(std::vector<std::string> const & toks, std::string const & raw,
                    std::vector<Material> & materials, std::vector<Object> & objects) {
    if (toks.size() < 6) {
      throw std::runtime_error("Error: Invalid sphere parameters\nLine: \"" + raw + "\"");
    }
    if (toks.size() > 6) {
      throw std::runtime_error(
          "Error: Extra data after configuration value for key: [sphere:]\nExtra: \"" +
          join_from(toks, 6) +
          "\"\nLine: \"" +
          raw +
          "\"");
    }
    try {
      double cx       = parse_double_strict(toks[1]);
      double cy       = parse_double_strict(toks[2]);
      double cz       = parse_double_strict(toks[3]);
      double r        = parse_double_strict(toks[4]);
      std::string mat = toks[5];
      if (r <= 0.0) {
        throw std::invalid_argument("radius");
      }
      if (find_material(materials, mat) == -1) {
        std::string msg = "Error: Material not found: [";
        msg += mat;
        msg += "]\nLine: \"";
        msg += raw;
        msg += "\"";
        throw std::runtime_error(msg);
      }
      objects.push_back({
        ObjectType::Sphere, {cx, cy, cz, r},
         mat, raw
      });
    } catch (std::runtime_error const & e) {
      throw;
    } catch (...) {
      throw std::runtime_error("Error: Invalid sphere parameters\nLine: \"" + raw + "\"");
    }
  }

  int get_material_index_or_throw(std::vector<Material> const & materials, std::string const & mat,
                                  std::string const & raw) {
    int idx = find_material(materials, mat);
    if (idx == -1) {
      std::string msg = "Error: Material not found: [";
      msg += mat;
      msg += "]\nLine: \"";
      msg += raw;
      msg += "\"";
      throw std::runtime_error(msg);
    }
    return idx;
  }

  void parse_cylinder(std::vector<std::string> const & toks, std::string const & raw,
                      std::vector<Material> & materials, std::vector<Object> & objects) {
    if (toks.size() < 9) {
      throw std::runtime_error("Error: Invalid cylinder parameters\nLine: \"" + raw + "\"");
    }
    if (toks.size() > 9) {
      throw std::runtime_error(
          "Error: Extra data after configuration value for key: [cylinder:]\nExtra: \"" +
          join_from(toks, 9) +
          "\"\nLine: \"" +
          raw +
          "\"");
    }
    try {
      double cx       = parse_double_strict(toks[1]);
      double cy       = parse_double_strict(toks[2]);
      double cz       = parse_double_strict(toks[3]);
      double r        = parse_double_strict(toks[4]);
      double ax       = parse_double_strict(toks[5]);
      double ay       = parse_double_strict(toks[6]);
      double az       = parse_double_strict(toks[7]);
      std::string mat = toks[8];
      if (r <= 0.0) {
        throw std::invalid_argument("radius");
      }
      get_material_index_or_throw(materials, mat, raw);
      objects.push_back({
        ObjectType::Cylinder, {cx, cy, cz, r, ax, ay, az},
         mat, raw
      });
    } catch (std::runtime_error const & e) {
      throw;
    } catch (...) {
      throw std::runtime_error("Error: Invalid cylinder parameters\nLine: \"" + raw + "\"");
    }
  }

}  // anonymous namespace

namespace {

  void dispatch_scene_entity(std::vector<std::string> const & toks, std::string const & raw,
                             std::vector<Material> & materials, std::vector<Object> & objects) {
    std::string const & key = toks[0];
    if (key == "matte:") {
      parse_matte(toks, raw, materials);
    } else if (key == "metal:") {
      parse_metal(toks, raw, materials);
    } else if (key == "refractive:") {
      parse_refractive(toks, raw, materials);
    } else if (key == "sphere:") {
      parse_sphere(toks, raw, materials, objects);
    } else if (key == "cylinder:") {
      parse_cylinder(toks, raw, materials, objects);
    } else {
      // unknown entity: strip trailing ':' if present for nicer message
      std::string ent = key;
      if (!ent.empty() and ent.back() == ':') {
        ent.pop_back();
      }
      throw std::runtime_error(std::string("Error: Unknown scene entity: ") + ent);
    }
  }

}  // namespace

std::pair<std::vector<Material>, std::vector<Object>> parseScene(std::string const & filename) {
  std::ifstream ifs(filename);
  if (!ifs) {
    throw std::runtime_error("No se pudo abrir archivo: " + filename);
  }

  std::vector<Material> materials;
  std::vector<Object> objects;

  std::string raw;
  while (std::getline(ifs, raw)) {
    if (is_blank(raw)) {
      continue;
    }
    std::string line = trim(raw);
    auto toks        = split_ws(line);
    if (toks.empty()) {
      continue;
    }
    dispatch_scene_entity(toks, raw, materials, objects);
  }

  return {materials, objects};
}
