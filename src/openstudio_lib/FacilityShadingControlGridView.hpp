/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) 2008-2019, Alliance for Sustainable Energy, LLC, and other contributors. All rights reserved.
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

#ifndef OPENSTUDIO_FACILITYSHADINGCONTROLGRIDVIEW_HPP
#define OPENSTUDIO_FACILITYSHADINGCONTROLGRIDVIEW_HPP

#include "../shared_gui_components/OSGridController.hpp"

#include "GridViewSubTab.hpp"
#include "OSItem.hpp"

#include <openstudio/model/Model.hpp>

class QComboBox;
class QLineEdit;

namespace openstudio {

class OSQuantityEdit2;

class FacilityShadingControlGridController;

class FacilityShadingControlGridView : public GridViewSubTab
{
  Q_OBJECT

 public:
  FacilityShadingControlGridView(bool isIP, const model::Model& model, QWidget* parent = 0);

  virtual ~FacilityShadingControlGridView() {}

  QLineEdit* m_nameFilter = nullptr;

  QComboBox* m_shadingTypeFilter = nullptr;

  QComboBox* m_shadingControlTypeFilter = nullptr;

  QLineEdit* m_subSurfaceNameFilter = nullptr;

  // QComboBox* m_subSurfaceTypeFilter = nullptr;

 private:
  REGISTER_LOGGER("openstudio.FacilityShadingControlGridView");

  // The model Ctor for ShadingControl takes either a ShadingMaterial or a Construction... Here I instantiate it with a Blind
  virtual void addObject(const openstudio::IddObjectType& iddObjectType) override;

  // Purges empty Shading Controls that have no SubSurfaces
  virtual void purgeObjects(const openstudio::IddObjectType& iddObjectType) override;

  void filterChanged();

  std::set<openstudio::model::ModelObject> m_objectsFilteredByName;

  std::set<openstudio::model::ModelObject> m_objectsFilteredByShadingType;

  std::set<openstudio::model::ModelObject> m_objectsFilteredByShadingControlType;

  std::set<openstudio::model::ModelObject> m_objectsFilteredBySubSurfaceName;

  // std::set<openstudio::model::ModelObject> m_objectsFilteredBySubSurfaceType;

 protected slots:

  virtual void onSelectItem() override;

  virtual void onClearSelection() override;

 private slots:

  void nameFilterChanged();

  void shadingTypeFilterChanged(const QString& text);

  void shadingControlTypeFilterChanged(const QString& text);

  void subSurfaceNameFilterChanged();

  // void subSurfaceTypeFilterChanged(const QString& text);
};

class FacilityShadingControlGridController : public OSGridController
{

  Q_OBJECT

 public:
  FacilityShadingControlGridController(bool isIP, const QString& headerText, IddObjectType iddObjectType, const model::Model& model,
                                       const std::vector<model::ModelObject>& modelObjects);

  virtual ~FacilityShadingControlGridController() {}

  virtual void refreshModelObjects() override;

  virtual void categorySelected(int index) override;

 protected:
  virtual void setCategoriesAndFields() override;

  virtual void addColumns(const QString& category, std::vector<QString>& fields) override;

  virtual void checkSelectedFields() override;

  virtual QString getColor(const model::ModelObject& modelObject) override;

 public slots:

  virtual void onItemDropped(const OSItemId& itemId) override;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_FACILITYSHADINGCONTROLGRIDVIEW_HPP
