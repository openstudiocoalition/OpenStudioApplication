#include <benchmark/benchmark.h>

#include "../../model_editor/Application.hpp"
#include "../SpacesSurfacesGridView.hpp"

#include <openstudio/model/Model.hpp>
#include <openstudio/model/Space.hpp>
#include <openstudio/model/Space_Impl.hpp>
#include <openstudio/model/SpaceType.hpp>
#include <openstudio/model/SpaceType_Impl.hpp>
#include <openstudio/model/Surface.hpp>
#include <openstudio/model/Surface_Impl.hpp>
#include <openstudio/utilities/geometry/Point3d.hpp>

using namespace openstudio;
using namespace openstudio::model;

model::Model makeModelWithNSurfaces(size_t nSurfaces) {

  Model m;

  constexpr int nSurfacesperSpace = 6;

  constexpr double floorHeight = 2.0;

  double zOrigin = 0.0;
  for (int i = 0; i <= (nSurfaces / nSurfacesperSpace); ++i) {

    Point3dVector pts{{0, 0, zOrigin}, {0, 1, zOrigin}, {1, 1, zOrigin}, {1, 0, zOrigin}};
    Space::fromFloorPrint(pts, floorHeight, m);
    zOrigin += floorHeight;
  }

  return m;
}

static void BM_SpacesSurfaces(benchmark::State& state) {

  openstudio::Application::instance().application(true);

  model::Model model = makeModelWithNSurfaces(state.range(0));

  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
    auto gridView = std::make_shared<SpacesSurfacesGridView>(false, model);
    openstudio::Application::instance().application(true)->processEvents();
    benchmark::DoNotOptimize(gridView);
  };

  state.SetComplexityN(state.range(0));
}

// This doesn't work, it goes [6, 8, 16...]
//BENCHMARK(BM_SpacesSurfaces)->RangeMultiplier(2)->Range(6, 6<<6)->Unit(benchmark::kMillisecond)->Complexity();
BENCHMARK(BM_SpacesSurfaces)
  ->Arg(6)
  ->Arg(12)
  ->Arg(24)
  ->Arg(48)
  ->Arg(96)
  ->Arg(192)
  ->Arg(384)
  ->Arg(768)
  //->Arg(1536) // 1536 takes 89 secodns AFTER dan's improvements
  ->Unit(benchmark::kMillisecond)
  ->Complexity();
