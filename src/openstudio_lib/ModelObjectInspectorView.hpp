/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_MODELOBJECTINSPECTORVIEW_HPP
#define OPENSTUDIO_MODELOBJECTINSPECTORVIEW_HPP

#include <openstudio/model/ComponentData.hpp>

#include <openstudio/model/Model.hpp>
#include <openstudio/model/ModelObject.hpp>

#include <boost/smart_ptr.hpp>
#include <boost/optional.hpp>
#include <QWidget>
#include "OSInspectorView.hpp"

class QLabel;
class QStackedWidget;

namespace openstudio {

class OSItem;

class ModelObjectInspectorView : public OSInspectorView
{
  Q_OBJECT

 public:
  ModelObjectInspectorView(const openstudio::model::Model& model, bool addScrollArea, QWidget* parent = nullptr);

  virtual ~ModelObjectInspectorView() {}

  // override if your implementation supports multiple object selection
  // (eg, via GridView)
  virtual bool supportsMultipleObjectSelection() const {
    return false;
  }
  virtual std::set<model::ModelObject> selectedObjects() const;
  void update();

 signals:

  void toggleUnitsClicked(bool displayIP);

  void toggleDisplayAdditionalPropsClicked(bool displayAdditionalProps);

  void itemSelected(OSItem* item);

  void selectionCleared();

  void itemsRequested();

  void dropZoneItemClicked(OSItem* item);

  void onChange();

 public slots:

  void selectModelObject(const openstudio::model::ModelObject& modelObject);

 protected:
  virtual void onSelectItem(OSItem* item) override;

  virtual void onClearSelection() override;

  virtual void onSelectModelObject(const openstudio::model::ModelObject& modelObject) = 0;

  boost::optional<openstudio::model::ModelObject> modelObject() const;

  model::Model m_model;

 protected slots:

  // does nothing, override in derived class
  virtual void toggleUnits(bool displayIP);

  // does nothing, override in derived class
  virtual void toggleDisplayAdditionalProps(bool displayAdditionalProps);

 private:
  boost::optional<openstudio::model::ModelObject> m_modelObject;
};

class DefaultInspectorView : public ModelObjectInspectorView
{
  Q_OBJECT

 public:
  explicit DefaultInspectorView(const model::Model& model, QWidget* parent = nullptr);

  virtual ~DefaultInspectorView() {}

 protected:
  virtual void onClearSelection() override;

  virtual void onSelectModelObject(const openstudio::model::ModelObject& modelObject) override;

  virtual void onUpdate() override;

 private:
  QLabel* m_mainLabel;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_MODELOBJECTINSPECTORVIEW_HPP
