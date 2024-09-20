/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_CONSTRUCTIONINTERNALSOURCEINSPECTORVIEW_HPP
#define OPENSTUDIO_CONSTRUCTIONINTERNALSOURCEINSPECTORVIEW_HPP

#include "ModelObjectInspectorView.hpp"
#include <openstudio/model/ConstructionWithInternalSource.hpp>

class QComboBox;

namespace openstudio {

class ConstructionObjectVectorController;

class OSDropZone;

class OSIntegerEdit2;

class OSLineEdit2;

class OSQuantityEdit2;

class OSComboBox2;

class StandardsInformationConstructionWidget;

class ConstructionInternalSourceInspectorView : public ModelObjectInspectorView
{
  Q_OBJECT

 public:
  ConstructionInternalSourceInspectorView(bool isIP, const openstudio::model::Model& model, QWidget* parent = nullptr);

  virtual ~ConstructionInternalSourceInspectorView() {}

 protected:
  virtual void onClearSelection() override;

  virtual void onSelectModelObject(const openstudio::model::ModelObject& modelObject) override;

  virtual void onUpdate() override;

 private:
  void createLayout();

  void attach(const openstudio::model::ConstructionWithInternalSource& constructionWithInternalSource);

  void detach();

  bool m_isIP;

  OSLineEdit2* m_nameEdit;

  StandardsInformationConstructionWidget* m_standardsInformationWidget;

  OSDropZone* m_constructionDZ;

  ConstructionObjectVectorController* m_constructionVC;

  OSIntegerEdit2* m_sourcePresentAfterLayerNumberEdit;

  OSIntegerEdit2* m_temperatureCalculationRequestedAfterLayerNumberEdit;

  OSIntegerEdit2* m_dimensionsForTheCTFCalculationEdit;

  OSQuantityEdit2* m_tubeSpacingEdit;

  boost::optional<model::ConstructionWithInternalSource> m_constructionWithInternalSource;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_CONSTRUCTIONINTERNALSOURCEINSPECTORVIEW_H
