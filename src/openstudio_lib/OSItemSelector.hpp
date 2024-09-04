/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_OSITEMSELECTOR_HPP
#define OPENSTUDIO_OSITEMSELECTOR_HPP

#include "MainTabView.hpp"

#include <boost/smart_ptr.hpp>
#include <boost/optional.hpp>
#include <QWidget>

namespace openstudio {

class OSItem;
class OSItemId;

class OSItemSelector : public QWidget
{
  Q_OBJECT

 public:
  explicit OSItemSelector(QWidget* parent = nullptr);

  virtual ~OSItemSelector() {}

  virtual OSItem* selectedItem() const = 0;

 signals:

  void itemSelected(OSItem* item);

  void itemRemoveClicked(OSItem* item);

  void itemReplacementDropped(OSItem* currentItem, const OSItemId& replacementItemId);

  void selectionCleared();
};

}  // namespace openstudio

#endif  // OPENSTUDIO_OSITEMSELECTOR_HPP
