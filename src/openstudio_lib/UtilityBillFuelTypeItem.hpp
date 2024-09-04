/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_UTILITYBILLFUELTYPEITEM_HPP
#define OPENSTUDIO_UTILITYBILLFUELTYPEITEM_HPP

#include "OSCollapsibleItem.hpp"

#include <openstudio/model/Model.hpp>

namespace openstudio {

class FuelType;
class UtilityBillFuelTypeListView;
class OSCollapsibleItemHeader;

class UtilityBillFuelTypeItem : public OSCollapsibleItem
{
  Q_OBJECT

 public:
  UtilityBillFuelTypeItem(OSCollapsibleItemHeader* collapsibleItemHeader, UtilityBillFuelTypeListView* utilityBillFuelTypeListView,
                          QWidget* parent = nullptr);

  virtual ~UtilityBillFuelTypeItem() {}

  IddObjectType iddObjectType() const;

  FuelType fuelType() const;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_UTILITYBILLFUELTYPEITEM_HPP
