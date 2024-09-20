/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_VARIABLESTABVIEW_HPP
#define OPENSTUDIO_VARIABLESTABVIEW_HPP

#include "MainTabView.hpp"
#include "../model_editor/QMetaTypes.hpp"
#include <openstudio/model/Model.hpp>
#include <openstudio/model/OutputVariable.hpp>
#include <openstudio/nano/nano_signal_slot.hpp>  // Signal-Slot replacement
#include <boost/optional.hpp>

class QCheckBox;
class QComboBox;
class QPushButton;
class QVBoxLayout;
class QLineEdit;

namespace openstudio {
class OSSwitch2;
class OSComboBox2;
class ProgressBarWithError;
class VariableListItem
  : public QWidget
  , public Nano::Observer
{
  Q_OBJECT;

 public:
  VariableListItem(const std::string& t_name, const std::string& t_keyValue, const boost::optional<openstudio::model::OutputVariable>& t_variable,
                   const openstudio::model::Model& t_model);

  virtual ~VariableListItem() {}

  bool matchesText(const QString& text, bool useRegex = false) const;

  // If the OSSwitch2 button is on
  bool isVariableEnabled() const;

  void setReportingFrequency(const std::string& freq);

 public slots:
  void setVariableEnabled(bool);

 private slots:
  void onOffClicked(bool);
  void indexChanged(const QString& t_frequency);

 private:
  REGISTER_LOGGER("openstudio.VariableListItem");

  std::string m_name;
  std::string m_keyValue;
  boost::optional<openstudio::model::OutputVariable> m_variable;
  openstudio::model::Model m_model;

  OSComboBox2* m_combobox;
  OSSwitch2* m_onOffButton;
};

class VariablesList
  : public QWidget
  , public Nano::Observer
{
  Q_OBJECT;

 public:
  explicit VariablesList(openstudio::model::Model t_model);
  virtual ~VariablesList();

 private slots:
  void onAdded(const WorkspaceObject&, const openstudio::IddObjectType&, const openstudio::UUID&);
  void onRemoved(const WorkspaceObject&, const openstudio::IddObjectType&, const openstudio::UUID&);

  void allOnClicked();
  void allOffClicked();

  void enableAll(bool);
  void updateVariableList();

  void onSearchTextEdited(const QString& text);

  void applyFrequencyToAllVisibleClicked();

 private:
  REGISTER_LOGGER("openstudio.VariablesList");
  openstudio::model::Model m_model;

  QLineEdit* m_searchBox;
  QCheckBox* m_searchUseRegex;
  QPushButton* m_displayAllBtn;
  QPushButton* m_displayOnlyEnabledBtn;
  QPushButton* m_displayOnlyDisabledBtn;
  QPushButton* m_allOnBtn;
  QPushButton* m_allOffBtn;
  QComboBox* m_frequencyComboBox;
  QPushButton* m_applyFrequencyBtn;
  ProgressBarWithError* m_progressBar;
  QVBoxLayout* m_listLayout;

  bool m_dirty;
  bool m_searchActive;
  QString m_searchText;
  std::vector<VariableListItem*> m_variables;
};

class VariablesTabView : public MainTabView
{
  Q_OBJECT;

 public:
  explicit VariablesTabView(openstudio::model::Model t_model, QWidget* parent = nullptr);

  virtual ~VariablesTabView() {}

 private:
  REGISTER_LOGGER("openstudio.VariablesTabView");
};

}  // namespace openstudio

#endif  // OPENSTUDIO_VARIABLESTABVIEW_HPP
