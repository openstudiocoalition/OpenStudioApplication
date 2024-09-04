/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_CONSTRUCTIONINSPECTORVIEW_HPP
#define OPENSTUDIO_CONSTRUCTIONINSPECTORVIEW_HPP

#include "ModelObjectInspectorView.hpp"
#include <openstudio/model/Construction.hpp>

class QComboBox;

namespace openstudio {

namespace model {
class ConstructionBase;
}

class ConstructionObjectVectorController;

class OSDropZone;

class OSLineEdit2;

class OSComboBox2;

class StandardsInformationConstructionWidget;

class ConstructionInspectorView : public ModelObjectInspectorView
{
  Q_OBJECT

 public:
  ConstructionInspectorView(bool isIP, const openstudio::model::Model& model, QWidget* parent = nullptr);

  virtual ~ConstructionInspectorView() {}

 protected:
  virtual void onClearSelection() override;

  virtual void onSelectModelObject(const openstudio::model::ModelObject& modelObject) override;

  virtual void onUpdate() override;

 private:
  void createLayout();

  void attach(openstudio::model::Construction& construction);

  void detach();

  bool m_isIP;

  OSLineEdit2* m_nameEdit;

  StandardsInformationConstructionWidget* m_standardsInformationWidget;

  OSDropZone* m_constructionDZ;

  ConstructionObjectVectorController* m_constructionVC;

  boost::optional<model::Construction> m_construction;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_CONSTRUCTIONINSPECTORVIEW_HPP
