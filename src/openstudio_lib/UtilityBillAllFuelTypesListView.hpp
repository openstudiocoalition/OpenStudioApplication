/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_UTILITYBILLALLFUELTYPESLISTVIEW_HPP
#define OPENSTUDIO_UTILITYBILLALLFUELTYPESLISTVIEW_HPP

#include "OSCollapsibleItemList.hpp"

#include <openstudio/model/Model.hpp>
#include <openstudio/model/ModelObject.hpp>

#include <openstudio/utilities/data/DataEnums.hpp>

#include <boost/optional.hpp>

namespace openstudio {

class FuelType;

class UtilityBillAllFuelTypesListView : public OSCollapsibleItemList
{
  Q_OBJECT

 public:
  UtilityBillAllFuelTypesListView(const model::Model& model, bool addScrollArea, OSItemType headerType, QWidget* parent = nullptr);

  UtilityBillAllFuelTypesListView(const std::vector<std::pair<FuelType, std::string>>& utilityBillFuelTypesAndNames, const model::Model& model,
                                  bool addScrollArea, OSItemType headerType, QWidget* parent = nullptr);

  virtual ~UtilityBillAllFuelTypesListView() {}

  void addUtilityBillFuelType(const FuelType& fuelType, const std::string& name);

  void addModelObjectType(const IddObjectType& iddObjectType, const std::string& name);

  FuelType currentFuelType() const;

  boost::optional<openstudio::FuelType> selectedFuelType() const;

  virtual boost::optional<openstudio::model::ModelObject> selectedModelObject() const;

 private:
  std::vector<std::pair<FuelType, std::string>> m_utilityBillFuelTypesAndNames;

  model::Model m_model;
  OSItemType m_headerType;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_UTILITYBILLALLFUELTYPESLISTVIEW_HPP
