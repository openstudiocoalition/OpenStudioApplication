/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_STANDARDSINFORMATIONCONSTRUCTIONWIDGET_HPP
#define OPENSTUDIO_STANDARDSINFORMATIONCONSTRUCTIONWIDGET_HPP

#include <openstudio/model/ConstructionBase.hpp>
#include <openstudio/model/StandardsInformationConstruction.hpp>

#include <QWidget>

class QComboBox;

class QGridLayout;

class QLabel;

namespace openstudio {

namespace model {

class ConstructionBase;

}

class OSLineEdit2;

class OSComboBox2;

class OSSwitch2;

class StandardsInformationConstructionWidget : public QWidget
{
  Q_OBJECT

 public:
  StandardsInformationConstructionWidget(bool isIP, QGridLayout* mainGridLayout, int& row);

  virtual ~StandardsInformationConstructionWidget();

  void attach(openstudio::model::ConstructionBase& construction);

  void detach();

  void showFenestration();

  void hideFenestration();

  void enableFenestration();

  void disableFenestration();

 public slots:

  void toggleUnits(bool displayIP);

 private slots:

  void standardChanged(const QString& text);
  void populateStandards();

  void standardSourceChanged(const QString& text);
  void populateStandardSources();

  void intendedSurfaceTypeChanged(const QString& text);

  void standardsConstructionTypeChanged(const QString& text);
  void populateStandardsConstructionType();

 private:
  QComboBox* m_standard = nullptr;

  QComboBox* m_standardSource = nullptr;

  OSComboBox2* m_intendedSurfaceType = nullptr;

  QComboBox* m_standardsConstructionType = nullptr;

  OSComboBox2* m_fenestrationType = nullptr;
  QLabel* m_fenestrationTypeLabel = nullptr;

  OSComboBox2* m_fenestrationAssemblyContext = nullptr;
  QLabel* m_fenestrationAssemblyContextLabel = nullptr;

  OSComboBox2* m_fenestrationNumberOfPanes = nullptr;
  QLabel* m_fenestrationNumberOfPanesLabel = nullptr;

  OSComboBox2* m_fenestrationFrameType = nullptr;
  QLabel* m_fenestrationFrameTypeLabel = nullptr;

  OSComboBox2* m_fenestrationDividerType = nullptr;
  QLabel* m_fenestrationDividerTypeLabel = nullptr;

  OSComboBox2* m_fenestrationTint = nullptr;
  QLabel* m_fenestrationTintLabel = nullptr;

  OSComboBox2* m_fenestrationGasFill = nullptr;
  QLabel* m_fenestrationGasFillLabel = nullptr;

  OSSwitch2* m_fenestrationLowEmissivityCoating = nullptr;
  QLabel* m_fenestrationLowEmissivityCoatingLabel = nullptr;

  bool m_isIP = false;
  bool m_fenestrationEnabled = false;

  boost::optional<openstudio::model::ConstructionBase> m_construction;

  boost::optional<openstudio::model::StandardsInformationConstruction> m_standardsInformation;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_STANDARDSINFORMATIONCONSTRUCTIONWIDGET_HPP
