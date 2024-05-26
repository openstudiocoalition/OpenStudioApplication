/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) 2020-2023, OpenStudio Coalition and other contributors. All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
*  following conditions are met:
*
*  (1) Redistributions of source code must retain the above copyright notice, this list of conditions and the following
*  disclaimer.
*
*  (2) Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following
*  disclaimer in the documentation and/or other materials provided with the distribution.
*
*  (3) Neither the name of the copyright holder nor the names of any contributors may be used to endorse or promote products
*  derived from this software without specific prior written permission from the respective party.
*
*  (4) Other than as required in clauses (1) and (2), distributions in any form of modifications or other derivative works
*  may not use the "OpenStudio" trademark, "OS", "os", or any other confusingly similar designation without specific prior
*  written permission from Alliance for Sustainable Energy, LLC.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER(S) AND ANY CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
*  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
*  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER(S), ANY CONTRIBUTORS, THE UNITED STATES GOVERNMENT, OR THE UNITED
*  STATES DEPARTMENT OF ENERGY, NOR ANY OF THEIR EMPLOYEES, BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
*  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
*  USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
*  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
*  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***********************************************************************************************************************/

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

int main(int argc, char* argv[]) {
  Q_INIT_RESOURCE(openstudio);
  auto app = openstudio::Application::instance().application(true);

  QTimer::singleShot(0, [&]() {
    ::benchmark::Initialize(&argc, argv);
    ::benchmark::RunSpecifiedBenchmarks();
    app->exit(0);
  });

  return app->exec();
}

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
