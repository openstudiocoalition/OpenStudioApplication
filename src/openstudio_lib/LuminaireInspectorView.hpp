/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_LUMINAIREINSPECTORVIEW_HPP
#define OPENSTUDIO_LUMINAIREINSPECTORVIEW_HPP

#include "ModelObjectInspectorView.hpp"
#include <openstudio/model/LuminaireDefinition.hpp>

namespace openstudio {

class OSLineEdit2;

class OSQuantityEdit2;

class OSDropZone;

class LuminaireDefinitionInspectorView : public ModelObjectInspectorView
{
  Q_OBJECT

 public:
  LuminaireDefinitionInspectorView(bool isIP, const openstudio::model::Model& model, QWidget* parent = nullptr);

  virtual ~LuminaireDefinitionInspectorView() {}

 protected:
  virtual void onClearSelection() override;

  virtual void onSelectModelObject(const openstudio::model::ModelObject& modelObject) override;

  virtual void onUpdate() override;

 private:
  void attach(const openstudio::model::LuminaireDefinition& luminaireDefinition);

  void detach();

  void refresh();

  OSLineEdit2* m_nameEdit;

  OSQuantityEdit2* m_lightingPowerEdit;

  OSQuantityEdit2* m_fractionRadiantEdit;

  OSQuantityEdit2* m_fractionVisibleEdit;

  OSQuantityEdit2* m_returnAirFractionEdit;

  bool m_isIP;

  boost::optional<model::LuminaireDefinition> m_luminaireDefinition;

 public slots:

  void toggleUnits(bool displayIP) override;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_LUMINAIREINSPECTORVIEW_HPP
