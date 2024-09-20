/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_CONSTRUCTIONAIRBOUNDARYINSPECTORVIEW_HPP
#define OPENSTUDIO_CONSTRUCTIONAIRBOUNDARYINSPECTORVIEW_HPP

#include "ModelObjectInspectorView.hpp"
#include "ModelObjectVectorController.hpp"
#include <openstudio/model/ConstructionAirBoundary.hpp>

class QComboBox;

namespace openstudio {

class OSLineEdit2;

class OSQuantityEdit2;

class OSComboBox2;

class OSDropZone;

class StandardsInformationConstructionWidget;

class ConstructionAirBoundaryInspectorView : public ModelObjectInspectorView
{
  Q_OBJECT

 public:
  ConstructionAirBoundaryInspectorView(bool isIP, const openstudio::model::Model& model, QWidget* parent = nullptr);

  virtual ~ConstructionAirBoundaryInspectorView() {}

 protected:
  virtual void onClearSelection() override;

  virtual void onSelectModelObject(const openstudio::model::ModelObject& modelObject) override;

  virtual void onUpdate() override;

 private:
  void createLayout();

  void attach(const openstudio::model::ConstructionAirBoundary& constructionAirBoundary);

  void detach();

  bool m_isIP;

  OSLineEdit2* m_nameEdit;

  StandardsInformationConstructionWidget* m_standardsInformationWidget;

  OSComboBox2* m_airExchangeMethod;

  OSQuantityEdit2* m_simpleMixingAirChangesPerHour;

  OSDropZone* m_scheduleDropZone;

  boost::optional<model::ConstructionAirBoundary> m_constructionAirBoundary;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_CONSTRUCTIONAIRBOUNDARYINSPECTORVIEW_HPP
