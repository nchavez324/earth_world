#include "city.h"

namespace earth_world {

City::City(std::string name, std::string country_name, SpherePoint2 location)
    : name_(name), country_name_(country_name), location_(location) {}

}  // namespace earth_world
