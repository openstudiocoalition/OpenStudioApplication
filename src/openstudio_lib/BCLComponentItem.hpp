/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_BCLCOMPONENTITEM_HPP
#define OPENSTUDIO_BCLCOMPONENTITEM_HPP

#include "OSItem.hpp"
#include <openstudio/utilities/bcl/BCLComponent.hpp>

namespace openstudio {

OSItemId bclComponentToItemId(const BCLComponent& component);

class BCLComponentItem : public OSItem
{
  Q_OBJECT

 public:
  BCLComponentItem(const BCLComponent& component, OSItemType type, QWidget* parent = nullptr);
  virtual ~BCLComponentItem() {}

  virtual bool equal(const openstudio::OSItem*) const override;

 private:
  BCLComponent m_bclComponent;

  //openstudio::Handle m_handle;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_BCLCOMPONENTITEM_HPP
