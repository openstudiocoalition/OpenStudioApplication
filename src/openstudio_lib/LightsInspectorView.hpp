/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_LIGHTSINSPECTORVIEW_HPP
#define OPENSTUDIO_LIGHTSINSPECTORVIEW_HPP

#include "ModelObjectInspectorView.hpp"
#include <openstudio/model/LightsDefinition.hpp>

namespace openstudio {

class OSLineEdit2;

class OSQuantityEdit2;

class OSDropZone;

class LightsDefinitionInspectorView : public ModelObjectInspectorView
{
  Q_OBJECT

 public:
  LightsDefinitionInspectorView(bool isIP, const openstudio::model::Model& model, QWidget* parent = nullptr);

  virtual ~LightsDefinitionInspectorView() {}

 protected:
  virtual void onClearSelection() override;

  virtual void onSelectModelObject(const openstudio::model::ModelObject& modelObject) override;

  virtual void onUpdate() override;

 private:
  void attach(const openstudio::model::LightsDefinition& lightsDefinition);

  void detach();

  void refresh();

  OSLineEdit2* m_nameEdit;

  OSQuantityEdit2* m_lightingLevelEdit;

  OSQuantityEdit2* m_wattsPerSpaceFloorAreaEdit;

  OSQuantityEdit2* m_wattsPerPersonEdit;

  OSQuantityEdit2* m_fractionRadiantEdit;

  OSQuantityEdit2* m_fractionVisibleEdit;

  OSQuantityEdit2* m_returnAirFractionEdit;

  bool m_isIP;

  boost::optional<model::LightsDefinition> m_lightsDefinition;

 public slots:

  void toggleUnits(bool displayIP) override;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_LIGHTSINSPECTORVIEW_HPP
