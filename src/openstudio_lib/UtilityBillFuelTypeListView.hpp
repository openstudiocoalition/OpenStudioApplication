/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_UTILITYBILLFUELTYPELISTVIEW_HPP
#define OPENSTUDIO_UTILITYBILLFUELTYPELISTVIEW_HPP

#include "OSItemList.hpp"
#include "OSVectorController.hpp"

#include "../model_editor/QMetaTypes.hpp"
#include <openstudio/model/Model.hpp>
#include <openstudio/model/ModelObject.hpp>

#include <openstudio/utilities/data/DataEnums.hpp>

namespace openstudio {

class FuelType;

class UtilityBillFuelTypeListController : public OSVectorController
{
  Q_OBJECT

 public:
  UtilityBillFuelTypeListController(const model::Model& model, const openstudio::FuelType& fuelType);

  virtual ~UtilityBillFuelTypeListController() {}

  IddObjectType iddObjectType() const;

  FuelType fuelType() const;

 private slots:
  void objectAdded(std::shared_ptr<openstudio::detail::WorkspaceObject_Impl>, const openstudio::IddObjectType&, const openstudio::UUID&);
  void objectRemoved(std::shared_ptr<openstudio::detail::WorkspaceObject_Impl>, const openstudio::IddObjectType&, const openstudio::UUID&);

 protected:
  virtual std::vector<OSItemId> makeVector() override;

 private:
  openstudio::IddObjectType m_iddObjectType;
  openstudio::FuelType m_fuelType;
  model::Model m_model;
};

class UtilityBillFuelTypeListView : public OSItemList
{
  Q_OBJECT

 public:
  UtilityBillFuelTypeListView(const model::Model& model, const openstudio::FuelType& fuelType, bool addScrollArea, QWidget* parent = nullptr);

  virtual ~UtilityBillFuelTypeListView() {}

  boost::optional<openstudio::model::ModelObject> selectedModelObject() const;

  IddObjectType iddObjectType() const;

  FuelType fuelType() const;

 private:
  openstudio::FuelType m_fuelType;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_UTILITYBILLFUELTYPELISTVIEW_HPP
