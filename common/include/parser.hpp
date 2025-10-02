#pragma once
#include <array>
#include <string>
#include <utility>
#include <vector>

// Tipos de materiales
enum class MaterialType { Matte, Metal, Refractive };

// Material
struct Material {
  std::string name;
  MaterialType type;
  std::vector<double> params;  // reflectancia (3), difusión (1) o índice de refracción (1)
};

// Tipos de objetos
enum class ObjectType { Sphere, Cylinder };

// Objeto
struct Object {
  ObjectType type;
  std::vector<double> params;  // para sphere: cx,cy,cz,r   para cylinder: cx,cy,cz,r,ax,ay,az
  std::string material;
  std::string raw_line;  // guarda la línea original (útil para errores/debug)
};

// Configuración global
struct Config {
  int image_width                              = 1'920;
  std::pair<int, int> aspect_ratio             = {16, 9};
  double gamma                                 = 2.2;
  std::array<double, 3> camera_position        = {0, 0, -10};
  std::array<double, 3> camera_target          = {0, 0, 0};
  std::array<double, 3> camera_north           = {0, 1, 0};
  double field_of_view                         = 90.0;
  int samples_per_pixel                        = 20;
  int max_depth                                = 5;
  int material_rng_seed                        = 13;
  int ray_rng_seed                             = 19;
  std::array<double, 3> background_dark_color  = {0.25, 0.5, 1.0};
  std::array<double, 3> background_light_color = {1.0, 1.0, 1.0};
};

// Funciones de parsing
// Lanzan std::runtime_error con mensajes EXACTOS (según enunciado) cuando hay errores.
Config parseConfig(std::string const & filename);
std::pair<std::vector<Material>, std::vector<Object>> parseScene(std::string const & filename);
