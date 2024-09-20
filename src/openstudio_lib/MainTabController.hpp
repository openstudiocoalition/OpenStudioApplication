/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_MAINTABCONTROLLER_HPP
#define OPENSTUDIO_MAINTABCONTROLLER_HPP

#include "../shared_gui_components/OSQObjectController.hpp"

#include <openstudio/model/Model.hpp>
#include <openstudio/model/ModelObject.hpp>

namespace openstudio {

class MainTabView;
class OSItem;

class MainTabController : public OSQObjectController
{
  Q_OBJECT

 public:
  explicit MainTabController(MainTabView* mainContentWidget);

  virtual ~MainTabController();

  MainTabView* mainContentWidget() const;

 private:
  MainTabView* m_mainContentWidget;

 signals:

  void modelObjectSelected(model::OptionalModelObject& modelObject, bool readOnly);

  void dropZoneItemSelected(OSItem* item, bool readOnly);

  void dropZoneItemClicked(OSItem* item);

  void toggleUnitsClicked(bool displayIP);

  void toggleDisplayAdditionalPropsClicked(bool displayAdditionalProps);

  void itemRemoveClicked(OSItem*);

  void downloadComponentsClicked();

  void openLibDlgClicked();

 public slots:

  virtual void setSubTab(int index){};
};

}  // namespace openstudio

#endif  // OPENSTUDIO_MAINTABCONTROLLER_HPP
