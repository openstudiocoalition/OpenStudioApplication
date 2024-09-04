/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_STANDARDOPAQUEMATERIALINSPECTORVIEW_HPP
#define OPENSTUDIO_STANDARDOPAQUEMATERIALINSPECTORVIEW_HPP

#include "ModelObjectInspectorView.hpp"

#include <openstudio/model/Model.hpp>

class QLabel;

namespace openstudio {

class StandardOpaqueMaterialInspectorView : public ModelObjectInspectorView
{
  Q_OBJECT

 public:
  explicit StandardOpaqueMaterialInspectorView(const model::Model& model, QWidget* parent = nullptr);

  virtual ~StandardOpaqueMaterialInspectorView() {}

 protected:
  virtual void onClearSelection() override;

  virtual void onSelectModelObject(const openstudio::model::ModelObject& modelObject) override;

  virtual void onUpdate() override;

 private:
  QLabel* m_mainLabel;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_STANDARDOPAQUEMATERIALINSPECTORVIEW_HPP
