/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_SUBTABCONTROLLER_HPP
#define OPENSTUDIO_SUBTABCONTROLLER_HPP

#include "../shared_gui_components/OSQObjectController.hpp"

#include <openstudio/model/Model.hpp>

#include <QObject>

namespace openstudio {

class OSItem;
class OSItemId;
class SubTabView;

class SubTabController : public OSQObjectController
{
  Q_OBJECT

 public:
  explicit SubTabController(SubTabView* subTabView);
  virtual ~SubTabController() {}

  SubTabView* subTabView();
  const SubTabView* subTabView() const;

 protected:
  virtual void onSelectItem(OSItem* item);

  virtual void onClearSelection();

  virtual void onAddItem() = 0;

  virtual void onCopyItem() = 0;

  virtual void onRemoveItem(OSItem* item) = 0;

  virtual void onReplaceItem(OSItem* item, const OSItemId& replacementItemId) = 0;

  virtual void onPurgeItems() = 0;

  virtual void onDrop(const OSItemId& itemId) = 0;

  virtual void onInspectItem(OSItem* item) = 0;

  SubTabView* m_subTabView;

 signals:

  void downloadComponentsClicked();

  void openLibDlgClicked();

 public slots:

  void removeItem(OSItem* item);

 protected slots:

  void selectItem(OSItem* item);

 private slots:

  void replaceItem(OSItem* item, const OSItemId& replacementItemId);

  void clearSelection();

  void handleDrop(const OSItemId&);

  void doAdd();

  void doCopy();

  void doRemove();

  void doPurge();

  void inspectItem(OSItem* item);

 private:
  boost::optional<openstudio::model::ModelObject> selectedModelObject() const;

  openstudio::IddObjectType currentIddObjectType() const;

  openstudio::model::Model m_model;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_SUBTABCONTROLLER_HPP
