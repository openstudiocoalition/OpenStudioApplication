/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_MODELSUBTABVIEW_HPP
#define OPENSTUDIO_MODELSUBTABVIEW_HPP

#include "SubTabView.hpp"

#include <openstudio/model/Model.hpp>
#include <openstudio/model/ModelObject.hpp>

#include <QWidget>

class QPushButton;
class QScrollArea;

namespace openstudio {

class OSItemSelector;
class OSItemSelectorButtons;
class OSItem;
class OSItemId;
class ModelObjectInspectorView;

class ModelSubTabView : public SubTabView
{
  Q_OBJECT

 public:
  ModelSubTabView(OSItemSelector* itemSelector, ModelObjectInspectorView* modelObjectInspectorView, bool showGridViewLayout = false,
                  QWidget* parent = nullptr);

  virtual ~ModelSubTabView() = default;

  ModelObjectInspectorView* modelObjectInspectorView();

 signals:

  void modelObjectSelected(model::OptionalModelObject& modelObject, bool readOnly);

  void dropZoneItemSelected(OSItem* item, bool readOnly);

  void dropZoneItemClicked(OSItem* item);

  void toggleUnitsClicked(bool displayIP);

 protected:
  bool m_isIP;

 protected slots:

  virtual void onDropZoneItemClicked(OSItem* item) override;

 private:
  ModelObjectInspectorView* m_modelObjectInspectorView;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_MODELSUBTABVIEW_HPP
