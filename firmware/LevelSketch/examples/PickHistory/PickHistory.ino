#include <LevelSketch.h>
#include "SketchViews.h"

void setup() { LevelSketch::begin(); }
void loop() { LevelSketch::update(SketchViews::paintPickHistory); }
