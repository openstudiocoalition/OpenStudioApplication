/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_STANDARDSINFORMATIONMATERIALWIDGET_HPP
#define OPENSTUDIO_STANDARDSINFORMATIONMATERIALWIDGET_HPP

#include <openstudio/model/Material.hpp>

#include <openstudio/model/StandardsInformationMaterial.hpp>

#include <QWidget>

class QComboBox;

class QGridLayout;

class QLabel;

namespace openstudio {

namespace model {

class Material;

}

class OSComboBox2;

class OSLineEdit2;

class StandardsInformationMaterialWidget : public QWidget
{
  Q_OBJECT

 public:
  StandardsInformationMaterialWidget(bool isIP, QGridLayout* mainGridLayout, int& row);

  virtual ~StandardsInformationMaterialWidget();

  void attach(openstudio::model::Material& material);

  void detach();

  void showComposite();

  void hideComposite();

  void enableComposite();

  void disableComposite();

 private:
  QComboBox* m_standard = nullptr;

  QComboBox* m_standardSource = nullptr;

  QComboBox* m_standardsCategory = nullptr;

  QComboBox* m_standardsIdentifier = nullptr;

  QComboBox* m_compositeFramingMaterial = nullptr;
  QLabel* m_compositeFramingMaterialLabel = nullptr;

  QComboBox* m_compositeFramingConfiguration = nullptr;
  QLabel* m_compositeFramingConfigurationLabel = nullptr;

  QComboBox* m_compositeFramingDepth = nullptr;
  QLabel* m_compositeFramingDepthLabel = nullptr;

  QComboBox* m_compositeFramingSize = nullptr;
  QLabel* m_compositeFramingSizeLabel = nullptr;

  QComboBox* m_compositeCavityInsulation = nullptr;
  QLabel* m_compositeCavityInsulationLabel = nullptr;

  bool m_isIP = false;

  boost::optional<openstudio::model::Material> m_material;

  boost::optional<openstudio::model::StandardsInformationMaterial> m_standardsInformation;

  bool m_populateFieldsRequested = false;

  void requestPopulateFields();

 public slots:

  void toggleUnits(bool displayIP);

  void populateFields();

 private slots:

  void standardChanged(const QString& text);
  void populateStandards();

  void standardSourceChanged(const QString& text);
  void populateStandardSources();

  void standardsCategoryChanged(const QString& text);
  void populateStandardsCategories();

  void standardsIdentifierChanged(const QString& text);
  void populateStandardsIdentifier();

  void compositeFramingMaterialChanged(const QString& text);
  void populateCompositeFramingMaterial();

  void compositeFramingConfigurationChanged(const QString& text);
  void populateCompositeFramingConfiguration();

  void compositeFramingDepthChanged(const QString& text);
  void populateCompositeFramingDepth();

  void compositeFramingSizeChanged(const QString& text);
  void populateCompositeFramingSize();

  void compositeCavityInsulationChanged(const QString& text);
  void populateCompositeCavityInsulation();
};

}  // namespace openstudio

#endif  // OPENSTUDIO_STANDARDSINFORMATIONMATERIALWIDGET_HPP
