/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "FoundationKivaSettingsWidget.hpp"

#include "../shared_gui_components/OSComboBox.hpp"
#include "../shared_gui_components/OSQuantityEdit.hpp"

#include <openstudio/model/FoundationKivaSettings_Impl.hpp>
#include <openstudio/utilities/core/String.hpp>

#include <QGridLayout>
#include <QLabel>
#include <QVBoxLayout>

namespace openstudio {

FoundationKivaSettingsWidget::FoundationKivaSettingsWidget(bool isIP, QWidget* parent) : QWidget(parent), m_isIP(isIP) {
  auto* mainLayout = new QVBoxLayout();
  mainLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
  mainLayout->setContentsMargins(10, 10, 10, 10);
  mainLayout->setSpacing(20);
  setLayout(mainLayout);

  auto* titleLabel = new QLabel("Foundation:Kiva:Settings");
  titleLabel->setObjectName("H2");
  mainLayout->addWidget(titleLabel);

  auto* grid = new QGridLayout();
  grid->setContentsMargins(0, 0, 0, 0);
  grid->setSpacing(10);
  mainLayout->addLayout(grid);

  int row = 0;

  auto addQuantityRow = [&](const QString& label, OSQuantityEdit2*& edit, const std::string& modelUnits, const std::string& siUnits,
                            const std::string& ipUnits) {
    grid->addWidget(new QLabel(label), row, 0);
    edit = new OSQuantityEdit2(modelUnits, siUnits, ipUnits, m_isIP);
    connect(this, &FoundationKivaSettingsWidget::toggleUnitsClicked, edit, &OSQuantityEdit2::onUnitSystemChange);
    grid->addWidget(edit, row, 1, Qt::AlignLeft);
    ++row;
  };

  auto addComboRow = [&](const QString& label, OSComboBox2*& combo) {
    grid->addWidget(new QLabel(label), row, 0);
    combo = new OSComboBox2();
    grid->addWidget(combo, row, 1, Qt::AlignLeft);
    ++row;
  };

  addQuantityRow(tr("Soil Conductivity"), m_soilConductivity, "W/m*K", "W/m*K", "Btu/hr*ft*R");
  addQuantityRow(tr("Soil Density"), m_soilDensity, "kg/m^3", "kg/m^3", "lb/ft^3");
  addQuantityRow(tr("Soil Specific Heat"), m_soilSpecificHeat, "J/kg*K", "J/kg*K", "Btu/lb*R");
  addQuantityRow(tr("Ground Solar Absorptivity"), m_groundSolarAbsorptivity, "", "", "");
  addQuantityRow(tr("Ground Thermal Absorptivity"), m_groundThermalAbsorptivity, "", "", "");
  addQuantityRow(tr("Ground Surface Roughness"), m_groundSurfaceRoughness, "m", "m", "ft");
  addQuantityRow(tr("Far-Field Width"), m_farFieldWidth, "m", "m", "ft");
  addComboRow(tr("Deep-Ground Boundary Condition"), m_deepGroundBoundaryCondition);
  addQuantityRow(tr("Deep-Ground Depth"), m_deepGroundDepth, "m", "m", "ft");
  addQuantityRow(tr("Minimum Cell Dimension"), m_minimumCellDimension, "m", "m", "ft");
  addQuantityRow(tr("Maximum Cell Growth Coefficient"), m_maximumCellGrowthCoefficient, "", "", "");
  addComboRow(tr("Simulation Timestep"), m_simulationTimestep);

  connect(this, &FoundationKivaSettingsWidget::toggleUnitsClicked, this, [this](bool isIP) { m_isIP = isIP; });

  mainLayout->addStretch();
}

void FoundationKivaSettingsWidget::detach() {
  m_soilConductivity->unbind();
  m_soilDensity->unbind();
  m_soilSpecificHeat->unbind();
  m_groundSolarAbsorptivity->unbind();
  m_groundThermalAbsorptivity->unbind();
  m_groundSurfaceRoughness->unbind();
  m_farFieldWidth->unbind();
  m_deepGroundBoundaryCondition->unbind();
  m_deepGroundDepth->unbind();
  m_minimumCellDimension->unbind();
  m_maximumCellGrowthCoefficient->unbind();
  m_simulationTimestep->unbind();
}

void FoundationKivaSettingsWidget::attach(const model::ModelObject& obj) {
  detach();
  m_obj = obj.cast<model::FoundationKivaSettings>();

  m_soilConductivity->bind(m_isIP, *m_obj, DoubleGetter([this]() { return m_obj->soilConductivity(); }),
                           boost::optional<DoubleSetter>([this](double v) { return m_obj->setSoilConductivity(v); }),
                           boost::optional<NoFailAction>([this]() { m_obj->resetSoilConductivity(); }), boost::none, boost::none,
                           boost::optional<BasicQuery>([this]() { return m_obj->isSoilConductivityDefaulted(); }));

  m_soilDensity->bind(m_isIP, *m_obj, DoubleGetter([this]() { return m_obj->soilDensity(); }),
                      boost::optional<DoubleSetter>([this](double v) { return m_obj->setSoilDensity(v); }),
                      boost::optional<NoFailAction>([this]() { m_obj->resetSoilDensity(); }), boost::none, boost::none,
                      boost::optional<BasicQuery>([this]() { return m_obj->isSoilDensityDefaulted(); }));

  m_soilSpecificHeat->bind(m_isIP, *m_obj, DoubleGetter([this]() { return m_obj->soilSpecificHeat(); }),
                           boost::optional<DoubleSetter>([this](double v) { return m_obj->setSoilSpecificHeat(v); }),
                           boost::optional<NoFailAction>([this]() { m_obj->resetSoilSpecificHeat(); }), boost::none, boost::none,
                           boost::optional<BasicQuery>([this]() { return m_obj->isSoilSpecificHeatDefaulted(); }));

  m_groundSolarAbsorptivity->bind(m_isIP, *m_obj, DoubleGetter([this]() { return m_obj->groundSolarAbsorptivity(); }),
                                  boost::optional<DoubleSetter>([this](double v) { return m_obj->setGroundSolarAbsorptivity(v); }),
                                  boost::optional<NoFailAction>([this]() { m_obj->resetGroundSolarAbsorptivity(); }), boost::none, boost::none,
                                  boost::optional<BasicQuery>([this]() { return m_obj->isGroundSolarAbsorptivityDefaulted(); }));

  m_groundThermalAbsorptivity->bind(m_isIP, *m_obj, DoubleGetter([this]() { return m_obj->groundThermalAbsorptivity(); }),
                                    boost::optional<DoubleSetter>([this](double v) { return m_obj->setGroundThermalAbsorptivity(v); }),
                                    boost::optional<NoFailAction>([this]() { m_obj->resetGroundThermalAbsorptivity(); }), boost::none, boost::none,
                                    boost::optional<BasicQuery>([this]() { return m_obj->isGroundThermalAbsorptivityDefaulted(); }));

  m_groundSurfaceRoughness->bind(m_isIP, *m_obj, DoubleGetter([this]() { return m_obj->groundSurfaceRoughness(); }),
                                 boost::optional<DoubleSetter>([this](double v) { return m_obj->setGroundSurfaceRoughness(v); }),
                                 boost::optional<NoFailAction>([this]() { m_obj->resetGroundSurfaceRoughness(); }), boost::none, boost::none,
                                 boost::optional<BasicQuery>([this]() { return m_obj->isGroundSurfaceRoughnessDefaulted(); }));

  m_farFieldWidth->bind(m_isIP, *m_obj, DoubleGetter([this]() { return m_obj->farFieldWidth(); }),
                        boost::optional<DoubleSetter>([this](double v) { return m_obj->setFarFieldWidth(v); }),
                        boost::optional<NoFailAction>([this]() { m_obj->resetFarFieldWidth(); }), boost::none, boost::none,
                        boost::optional<BasicQuery>([this]() { return m_obj->isFarFieldWidthDefaulted(); }));

  m_deepGroundBoundaryCondition->bind<std::string>(
    *m_obj, static_cast<std::string (*)(const std::string&)>(&openstudio::toString),
    []() -> std::vector<std::string> { return {"Autoselect", "ZeroFlux", "GroundWater"}; }, [this]() { return m_obj->deepGroundBoundaryCondition(); },
    [this](const std::string& s) { return m_obj->setDeepGroundBoundaryCondition(s); },
    boost::optional<NoFailAction>([this]() { m_obj->resetDeepGroundBoundaryCondition(); }),
    boost::optional<BasicQuery>([this]() { return m_obj->isDeepGroundBoundaryConditionAutoselected(); }));

  m_deepGroundDepth->bind(m_isIP, *m_obj, OptionalDoubleGetter([this]() { return m_obj->deepGroundDepth(); }),
                          boost::optional<DoubleSetter>([this](double v) { return m_obj->setDeepGroundDepth(v); }), boost::none, boost::none,
                          boost::optional<NoFailAction>([this]() { m_obj->autocalculateDeepGroundDepth(); }), boost::none, boost::none,
                          boost::optional<BasicQuery>([this]() { return m_obj->isDeepGroundDepthAutocalculated(); }));

  m_minimumCellDimension->bind(m_isIP, *m_obj, DoubleGetter([this]() { return m_obj->minimumCellDimension(); }),
                               boost::optional<DoubleSetter>([this](double v) { return m_obj->setMinimumCellDimension(v); }),
                               boost::optional<NoFailAction>([this]() { m_obj->resetMinimumCellDimension(); }), boost::none, boost::none,
                               boost::optional<BasicQuery>([this]() { return m_obj->isMinimumCellDimensionDefaulted(); }));

  m_maximumCellGrowthCoefficient->bind(m_isIP, *m_obj, DoubleGetter([this]() { return m_obj->maximumCellGrowthCoefficient(); }),
                                       boost::optional<DoubleSetter>([this](double v) { return m_obj->setMaximumCellGrowthCoefficient(v); }),
                                       boost::optional<NoFailAction>([this]() { m_obj->resetMaximumCellGrowthCoefficient(); }), boost::none,
                                       boost::none,
                                       boost::optional<BasicQuery>([this]() { return m_obj->isMaximumCellGrowthCoefficientDefaulted(); }));

  m_simulationTimestep->bind<std::string>(
    *m_obj, static_cast<std::string (*)(const std::string&)>(&openstudio::toString),
    []() -> std::vector<std::string> { return {"Hourly", "Timestep"}; }, [this]() { return m_obj->simulationTimestep(); },
    [this](const std::string& s) { return m_obj->setSimulationTimestep(s); },
    boost::optional<NoFailAction>([this]() { m_obj->resetSimulationTimestep(); }),
    boost::optional<BasicQuery>([this]() { return m_obj->isSimulationTimestepDefaulted(); }));
}

}  // namespace openstudio
