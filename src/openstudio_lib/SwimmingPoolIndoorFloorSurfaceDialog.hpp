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

#ifndef OPENSTUDIO_SWIMMINGPOOLINDOORFLOORSURFACEDIALOG_HPP
#define OPENSTUDIO_SWIMMINGPOOLINDOORFLOORSURFACEDIALOG_HPP

#include <openstudio/model/Model.hpp>
#include <openstudio/model/Surface.hpp>
#include <openstudio/model/SwimmingPoolIndoor.hpp>

#include "../shared_gui_components/OSDialog.hpp"

class QLabel;
class QComboBox;

namespace openstudio {

class SwimmingPoolIndoorFloorSurfaceDialog : public OSDialog
{
  Q_OBJECT

 public:
  explicit SwimmingPoolIndoorFloorSurfaceDialog(model::SwimmingPoolIndoor pool, QWidget* parent = nullptr);

  virtual ~SwimmingPoolIndoorFloorSurfaceDialog() = default;

  boost::optional<model::Surface> floorSurface() const;

 private slots:

  void onCurrentIndexChanged(int index);

 private:
  // void createWidgets();

  model::SwimmingPoolIndoor m_pool;
  model::Model m_model;
  boost::optional<model::Surface> m_floorSurface;

  QComboBox* m_floorSurfaceComboBox;
  QLabel* m_spaceName;
  QLabel* m_thermalZoneName;

 protected slots:

  virtual void on_okButton(bool checked) override;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_SWIMMINGPOOLINDOORFLOORSURFACEDIALOG_HPP
