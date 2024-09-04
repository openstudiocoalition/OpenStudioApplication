/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_CONSTRUCTIONCFACTORUNDERGROUNDWALLINSPECTORVIEW_HPP
#define OPENSTUDIO_CONSTRUCTIONCFACTORUNDERGROUNDWALLINSPECTORVIEW_HPP

#include "ModelObjectInspectorView.hpp"
#include "ModelObjectVectorController.hpp"
#include <openstudio/model/CFactorUndergroundWallConstruction.hpp>

class QComboBox;

namespace openstudio {

class OSLineEdit2;

class OSQuantityEdit2;

class OSComboBox2;

class StandardsInformationConstructionWidget;

class ConstructionCfactorUndergroundWallInspectorView : public ModelObjectInspectorView
{
  Q_OBJECT

 public:
  ConstructionCfactorUndergroundWallInspectorView(bool isIP, const openstudio::model::Model& model, QWidget* parent = nullptr);

  virtual ~ConstructionCfactorUndergroundWallInspectorView() {}

 protected:
  virtual void onClearSelection() override;

  virtual void onSelectModelObject(const openstudio::model::ModelObject& modelObject) override;

  virtual void onUpdate() override;

 private:
  void createLayout();

  void attach(const openstudio::model::CFactorUndergroundWallConstruction& cFactorUndergroundWallConstruction);

  void detach();

  bool m_isIP;

  OSLineEdit2* m_nameEdit;

  StandardsInformationConstructionWidget* m_standardsInformationWidget;

  OSQuantityEdit2* m_cfactorEdit;

  OSQuantityEdit2* m_heightEdit;

  boost::optional<model::CFactorUndergroundWallConstruction> m_cFactorUndergroundWallConstruction;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_CONSTRUCTIONCFACTORUNDERGROUNDWALLINSPECTORVIEW_HPP
