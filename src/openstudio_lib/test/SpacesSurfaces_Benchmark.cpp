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

using namespace openstudio;

static void BM_SpacesSurfaces(benchmark::State& state) {

  openstudio::Application::instance().application(true);

  model::Model model = model::exampleModel();
  auto spaces = model.getConcreteModelObjects<model::Space>();
  auto spaceTypes = model.getConcreteModelObjects<model::SpaceType>();
  auto surfaces = model.getConcreteModelObjects<model::Surface>();

  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
    auto gridView = std::make_shared<SpacesSurfacesGridView>(false, model);
    openstudio::Application::instance().application(true)->processEvents();
    benchmark::DoNotOptimize(gridView);
  };

}

BENCHMARK(BM_SpacesSurfaces);
