#pragma once

#include "embedded/digitalSettings.hpp"
#include "metro.h"

struct DigitalData {
  bool pneumatic_line_pressure_ = true;
  bool pneumatic_line_pressure_1_ = false;
  bool pneumatic_line_pressure_2_ = false;
  bool asms_on_ = false;
  bool sdc_open_{true}; /*< Detects AATS >*/
};
