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

#ifndef OPENSTUDIO_FACILITYEXTERIOREQUIPMENTGRIDVIEW_HPP
#define OPENSTUDIO_FACILITYEXTERIOREQUIPMENTGRIDVIEW_HPP

#include "../shared_gui_components/OSGridController.hpp"

#include "GridViewSubTab.hpp"
#include "OSItem.hpp"

#include <openstudio/model/Model.hpp>

namespace openstudio {

class FacilityExteriorEquipmentGridController;

class FacilityExteriorEquipmentGridView : public GridViewSubTab
{
  Q_OBJECT

 public:
  FacilityExteriorEquipmentGridView(bool isIP, bool displayAdditionalProps, const model::Model& model, QWidget* parent = 0);

  virtual ~FacilityExteriorEquipmentGridView() {}

 private:
  REGISTER_LOGGER("openstudio.FacilityExteriorEquipmentGridView");

  virtual void addObject(const openstudio::IddObjectType& iddObjectType) override;

  virtual void purgeObjects(const openstudio::IddObjectType& iddObjectType) override;

  void clearSelection();

 protected slots:

  virtual void onSelectItem() override;

  virtual void onClearSelection() override;
};

class FacilityExteriorEquipmentGridController : public OSGridController
{

  Q_OBJECT

 public:
  FacilityExteriorEquipmentGridController(bool isIP, bool displayAdditionalProps, const QString& settingsText, IddObjectType iddObjectType,
                                          const model::Model& model, const std::vector<model::ModelObject>& modelObjects);

  virtual ~FacilityExteriorEquipmentGridController() = default;

  virtual void refreshModelObjects() override;

 protected:
  void setCategoriesAndFields();

  virtual void addColumns(const QString& category, std::vector<QString>& fields) override;

  virtual void checkSelectedFields() override;

  virtual QString getColor(const model::ModelObject& modelObject) override;

 public slots:

  virtual void onCategorySelected(int index) override;

  virtual void onItemDropped(const OSItemId& itemId) override;

  virtual void onComboBoxIndexChanged(int index) override;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_FACILITYEXTERIOREQUIPMENTGRIDVIEW_HPP
