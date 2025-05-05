#include "GasSensorSystem.h"

GasSensorSystem gasSystem;

void setup() {
  gasSystem.setup();
}

void loop() {
  gasSystem.update();
}
