/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_OSITEMSELECTORBUTTONS_HPP
#define OPENSTUDIO_OSITEMSELECTORBUTTONS_HPP

#include "MainTabView.hpp"

#include <openstudio/model/Model.hpp>
#include <openstudio/model/ModelObject.hpp>

#include <QWidget>

class QPushButton;
class QVBoxLayout;
class QHBoxLayout;

namespace openstudio {

class OSItem;
class OSItemId;
class OSVectorController;
class OSDropZone;

class OSItemSelectorButtons : public QWidget
{
  Q_OBJECT

 public:
  explicit OSItemSelectorButtons(QWidget* parent = nullptr);

  virtual ~OSItemSelectorButtons() {}

  void showDropZone();
  void hideDropZone();
  void enableDropZone();
  void disableDropZone();

  void showAddButton();
  void hideAddButton();
  void enableAddButton();
  void disableAddButton();

  void showCopyButton();
  void hideCopyButton();
  void enableCopyButton();
  void disableCopyButton();

  void showRemoveButton();
  void hideRemoveButton();
  void enableRemoveButton();
  void disableRemoveButton();

  void showPurgeButton();
  void hidePurgeButton();
  void enablePurgeButton();
  void disablePurgeButton();

  void showBclDlgButton();
  void hideBclDlgButton();
  void enableBclDlgButton();
  void disableBclDlgButton();

 signals:

  void itemDropped(const OSItemId& itemId);

  void addClicked();

  void copyClicked();

  void removeClicked();

  void purgeClicked();

  void downloadComponentsClicked();

 protected:
  void paintEvent(QPaintEvent* event) override;

 private:
  QVBoxLayout* m_vLayout;

  OSVectorController* m_dropZoneController;

  OSDropZone* m_dropZone;

  QPushButton* m_addButton;

  QPushButton* m_copyButton;

  QPushButton* m_removeButton;

  QPushButton* m_purgeButton;

  QPushButton* m_openBclDlgButton;

  QPushButton* m_openLibDlgButton;

  QHBoxLayout* m_dropZoneLayout;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_OSITEMSELECTORBUTTONS_HPP
