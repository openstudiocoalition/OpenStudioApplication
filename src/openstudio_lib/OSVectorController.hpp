/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_OSVECTORCONTROLLER_HPP
#define OPENSTUDIO_OSVECTORCONTROLLER_HPP

#include "OSItem.hpp"

#include <QWidget>
#include <openstudio/model/ComponentData.hpp>
#include <openstudio/model/Model.hpp>
#include <openstudio/model/ModelObject.hpp>
#include <vector>
#include <openstudio/nano/nano_signal_slot.hpp>  // Signal-Slot replacement

class QMutex;

namespace openstudio {

class OSVectorController
  : public QObject
  , public Nano::Observer
{
  Q_OBJECT

 public:
  // in the future, OSVectorController's will be managed by shared_ptr's on parent controller
  // for now, each controller must be parented by a widget
  OSVectorController();

  virtual ~OSVectorController();

 public slots:

  void reportItems();

  void removeItem(OSItem* item);

  void replaceItem(OSItem* currentItem, const OSItemId& replacementItemId);

  void drop(const OSItemId& itemId);

  void makeNewItem();

 signals:

  void itemIds(const std::vector<OSItemId>& itemIds);

  void selectedItemId(const OSItemId& itemId);

 protected:
  virtual std::vector<OSItemId> makeVector() = 0;

  virtual void onRemoveItem(OSItem* item);

  virtual void onReplaceItem(OSItem* currentItem, const OSItemId& replacementItemId);

  virtual void onDrop(const OSItemId& itemId);

  virtual void onMakeNewItem();

 private:
  bool m_reportScheduled;

  QMutex* m_reportItemsMutex;

 private slots:

  void reportItemsImpl();
};

}  // namespace openstudio

#endif  // OPENSTUDIO_OSVECTORCONTROLLER_HPP
