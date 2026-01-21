#ifndef CONCUSSION_MATH_H
#define CONCUSSION_MATH_H

#include <Arduino.h>

// Rowson & Duma Combined Probability formula
float combinedProbability(float PLA_g, float PRA_rad_s2) {
  float x = 10.2f + 0.0433f * PLA_g + 0.000873f * PRA_rad_s2 - 0.000000920f * PLA_g * PRA_rad_s2;
  return 1.0f / (1.0f + expf(x));
}

const char* riskLabel(float cp) {
  if (cp >= 0.50f) return "HIGH";
  if (cp >= 0.20f)  return "MEDIUM";
  return "LOW";
}

#endif