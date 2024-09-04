/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_PEOPLEINSPECTORVIEW_HPP
#define OPENSTUDIO_PEOPLEINSPECTORVIEW_HPP

#include "ModelObjectInspectorView.hpp"
#include <openstudio/model/PeopleDefinition.hpp>
#include <openstudio/utilities/core/Logger.hpp>

class QGridLayout;
class QPushButton;
class QHBoxLayout;

namespace openstudio {

class OSDoubleEdit2;
class OSLineEdit2;
class OSQuantityEdit2;
class OSSwitch2;
class OSComboBox2;

class PeopleDefinitionInspectorView : public ModelObjectInspectorView
{
  Q_OBJECT

 public:
  PeopleDefinitionInspectorView(bool isIP, const openstudio::model::Model& model, QWidget* parent = nullptr);

  virtual ~PeopleDefinitionInspectorView() {}

 public slots:
  void addExtensible();

  void removeExtensible();

  void toggleUnits(bool displayIP) override;

 protected:
  virtual void onClearSelection() override;

  virtual void onSelectModelObject(const openstudio::model::ModelObject& modelObject) override;

  virtual void onUpdate() override;

  // Disable remove extensible group button if no groups left to remove
  // Disable add extensible group button if can't add more (maxFields)
  void checkButtons();

 private:
  REGISTER_LOGGER("openstudio.PeopleDefinitionInspectorView");

  void attach(const openstudio::model::PeopleDefinition& peopleDefinition);

  void detach();

  void refresh();

  // Adjusts the stretch of rows after adding/removing extensible groups, so that all rows have a stretch factor or 0 (default)
  // except the row following the last row with data that has a strech of 1 => pushes everything up
  void adjustRowStretch();

  OSComboBox2* addThermalComfortModelTypeComboBox(int groupIndex);

  QGridLayout* m_mainGridLayout;
  int lastRowNonExtensible;
  int lastRow;

  OSLineEdit2* m_nameEdit;

  OSDoubleEdit2* m_numberofPeopleEdit;
  OSQuantityEdit2* m_peopleperSpaceFloorAreaEdit;
  OSQuantityEdit2* m_spaceFloorAreaperPersonEdit;
  OSDoubleEdit2* m_fractionRadiantEdit;
  OSDoubleEdit2* m_sensibleHeatFractionEdit;
  OSQuantityEdit2* m_carbonDioxideGenerationRateEdit;

  OSSwitch2* m_enableASHRAE55ComfortWarningsSwitch;
  OSComboBox2* m_meanRadiantTemperatureCalculationTypeComboBox;

  // how to handle the extensible groups
  std::vector<OSComboBox2*> m_thermalComfortModelTypeComboBoxes;
  QPushButton* addBtn;
  QPushButton* removeBtn;

  // For deletion / indexing (really only the vectors could be used)
  QHBoxLayout* lastHBoxLayout;
  QWidget* lastRowWidget;
  std::vector<QHBoxLayout*> m_HBoxLayouts;
  std::vector<QWidget*> m_rowWidgets;

  bool m_isIP;
  boost::optional<model::PeopleDefinition> m_peopleDefinition;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_PEOPLEINSPECTORVIEW_HPP
