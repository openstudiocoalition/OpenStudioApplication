/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_CONSTRUCTIONFFACTORGROUNDFLOORINSPECTORVIEW_HPP
#define OPENSTUDIO_CONSTRUCTIONFFACTORGROUNDFLOORINSPECTORVIEW_HPP

#include "ModelObjectInspectorView.hpp"
#include "ModelObjectVectorController.hpp"
#include <openstudio/model/FFactorGroundFloorConstruction.hpp>

class QComboBox;

namespace openstudio {

class OSLineEdit2;

class OSQuantityEdit2;

class OSComboBox2;

class StandardsInformationConstructionWidget;

class ConstructionFfactorGroundFloorInspectorView : public ModelObjectInspectorView
{
  Q_OBJECT

 public:
  ConstructionFfactorGroundFloorInspectorView(bool isIP, const openstudio::model::Model& model, QWidget* parent = nullptr);

  virtual ~ConstructionFfactorGroundFloorInspectorView() {}

 protected:
  virtual void onClearSelection() override;

  virtual void onSelectModelObject(const openstudio::model::ModelObject& modelObject) override;

  virtual void onUpdate() override;

 private:
  void createLayout();

  void attach(const openstudio::model::FFactorGroundFloorConstruction& fFactorGroundFloorConstruction);

  void detach();

  bool m_isIP;

  OSLineEdit2* m_nameEdit;

  StandardsInformationConstructionWidget* m_standardsInformationWidget;

  OSQuantityEdit2* m_ffactorEdit;

  OSQuantityEdit2* m_areaEdit;

  OSQuantityEdit2* m_perimeterExposedEdit;

  boost::optional<model::FFactorGroundFloorConstruction> m_fFactorGroundFloorConstruction;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_CONSTRUCTIONFFACTORGROUNDFLOORINSPECTORVIEW_HPP
