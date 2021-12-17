#ifndef arc_all_hpp
#define arc_all_hpp

#include "bxdf.hpp"
#include "graph.hpp"
#include "shape.hpp"
#include "camera.hpp"
//#include "object.hpp"
//#include "render.hpp"
#include "medium.hpp"
#include "tracer.hpp"
#include "scene.hpp"
#include "mapping.hpp"
#include "image.hpp"
//#include "tree.hpp"
#include "sampler.hpp"
//#include "surface.hpp"
#include "vecmath.hpp"
//#include "material.hpp"
#include "spectrum.hpp"
//#include "objloader.hpp"

#include "bxdfs/lambert.hpp"
#include "bxdfs/biggx.hpp"
#include "bxdfs/ggx.hpp"

#include "shapes/sphere.hpp"
#include "shapes/flat.hpp"
//#include "shapes/disc.hpp"
#include "shapes/triangle.hpp"

#include "tracers/bdpt.hpp"
//#include "tracers/sppm.hpp"

#include "cameras/perspective.hpp"
#include "cameras/actinometer.hpp"

#include "lights/uniform.hpp"

#include "mediums/transparent.hpp"
//#include "mediums/scatter.hpp"

#endif /* arc_all_hpp */
