/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_SPACETYPEINSPECTORVIEW_HPP
#define OPENSTUDIO_SPACETYPEINSPECTORVIEW_HPP

#include "ModelObjectInspectorView.hpp"

namespace openstudio {

class SpaceTypesGridView;

class SpaceTypeInspectorView : public ModelObjectInspectorView
{
  Q_OBJECT

 public:
  SpaceTypeInspectorView(bool isIP, const openstudio::model::Model& model, QWidget* parent = nullptr);

  virtual ~SpaceTypeInspectorView() {}

  virtual bool supportsMultipleObjectSelection() const override {
    return true;
  }
  virtual std::set<model::ModelObject> selectedObjects() const override;

 protected:
  virtual void onClearSelection() override;

  virtual void onSelectModelObject(const openstudio::model::ModelObject& modelObject) override;

  virtual void onUpdate() override;

 private:
  void refresh();

  bool m_isIP;
  SpaceTypesGridView* m_gridView;

 public slots:

  void toggleUnits(bool displayIP) override;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_SPACETYPEINSPECTORVIEW_HPP
