/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_FOUNDATIONKIVASETTINGSWIDGET_HPP
#define OPENSTUDIO_FOUNDATIONKIVASETTINGSWIDGET_HPP

#include <openstudio/model/ModelObject.hpp>
#include <openstudio/model/FoundationKivaSettings.hpp>

#include <QWidget>

class QLabel;

namespace openstudio {

class OSComboBox2;
class OSQuantityEdit2;

class FoundationKivaSettingsWidget : public QWidget
{
  Q_OBJECT

 public:
  explicit FoundationKivaSettingsWidget(bool isIP, QWidget* parent = nullptr);

  void attach(const model::ModelObject& obj);
  void detach();

 signals:
  void toggleUnitsClicked(bool displayIP);

 private:
  bool m_isIP;
  boost::optional<model::FoundationKivaSettings> m_obj;

  OSQuantityEdit2* m_soilConductivity = nullptr;
  OSQuantityEdit2* m_soilDensity = nullptr;
  OSQuantityEdit2* m_soilSpecificHeat = nullptr;
  OSQuantityEdit2* m_groundSolarAbsorptivity = nullptr;
  OSQuantityEdit2* m_groundThermalAbsorptivity = nullptr;
  OSQuantityEdit2* m_groundSurfaceRoughness = nullptr;
  OSQuantityEdit2* m_farFieldWidth = nullptr;
  OSComboBox2* m_deepGroundBoundaryCondition = nullptr;
  OSQuantityEdit2* m_deepGroundDepth = nullptr;
  OSQuantityEdit2* m_minimumCellDimension = nullptr;
  OSQuantityEdit2* m_maximumCellGrowthCoefficient = nullptr;
  OSComboBox2* m_simulationTimestep = nullptr;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_FOUNDATIONKIVASETTINGSWIDGET_HPP
