/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "VariablesTabView.hpp"

#include <openstudio/model/Model_Impl.hpp>
#include <openstudio/model/OutputVariable.hpp>
#include <openstudio/model/ModelObject_Impl.hpp>
#include <openstudio/model/OutputVariable_Impl.hpp>

#include "../model_editor/Utilities.hpp"
#include "../shared_gui_components/OSDialog.hpp"
#include "../shared_gui_components/ProgressBarWithError.hpp"

#include <openstudio/utilities/sql/SqlFileEnums.hpp>

#include <QCheckBox>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRegularExpression>
#include <QScrollArea>
#include <QTimer>
#include <QVBoxLayout>

#include "../shared_gui_components/OSSwitch.hpp"
#include "../shared_gui_components/OSComboBox.hpp"

#include <openstudio/utilities/idd/IddEnums.hxx>
#include <utility>

namespace openstudio {

VariableListItem::VariableListItem(const std::string& t_name, const std::string& t_keyValue,
                                   const boost::optional<openstudio::model::OutputVariable>& t_variable, const openstudio::model::Model& t_model)
  : m_name(t_name), m_keyValue(t_keyValue), m_variable(t_variable), m_model(t_model) {

  auto* vbox = new QVBoxLayout();
  vbox->setContentsMargins(0, 0, 0, 0);
  vbox->setSpacing(0);

  auto* hline = new QFrame();
  hline->setFrameShape(QFrame::HLine);
  hline->setFrameShadow(QFrame::Sunken);

  auto* hbox = new QHBoxLayout();
  hbox->setContentsMargins(10, 10, 10, 10);
  hbox->setSpacing(10);

  m_onOffButton = new OSSwitch2();
  connect(m_onOffButton, &OSSwitch2::clicked, this, &VariableListItem::onOffClicked);
  hbox->addWidget(m_onOffButton);
  m_onOffButton->setChecked(m_variable.is_initialized());

  hbox->addWidget(new QLabel(openstudio::toQString(m_name + ",")));
  hbox->addWidget(new QLabel(openstudio::toQString(m_keyValue)));
  hbox->addStretch(10);
  m_combobox = new OSComboBox2();
  m_combobox->setFixedWidth(200);
  connect(m_combobox, static_cast<void (OSComboBox2::*)(const QString&)>(&OSComboBox2::currentTextChanged), this, &VariableListItem::indexChanged);
  if (m_variable) {
    // m_combobox->bind(*m_variable, "reportingFrequency");
    m_combobox->bind<std::string>(
      *m_variable, static_cast<std::string (*)(const std::string&)>(&openstudio::toString),
      std::bind(&model::OutputVariable::reportingFrequencyValues), std::bind(&model::OutputVariable::reportingFrequency, m_variable.get_ptr()),
      std::bind(&model::OutputVariable::setReportingFrequency, m_variable.get_ptr(), std::placeholders::_1),
      boost::optional<NoFailAction>(std::bind(&model::OutputVariable::resetReportingFrequency, m_variable.get_ptr())),
      boost::optional<BasicQuery>(std::bind(&model::OutputVariable::isReportingFrequencyDefaulted, m_variable.get_ptr())));
  }

  hbox->addWidget(m_combobox);

  vbox->addWidget(hline);
  vbox->addLayout(hbox);
  // vbox->addStretch();
  setLayout(vbox);
  onOffClicked(m_variable.is_initialized());
}

VariablesList::~VariablesList() {

  for (auto itr = m_variables.begin(); itr != m_variables.end(); ++itr) {
    //(*itr)->setParent(0);
    //(*itr)->setStyleSheet("");
    m_listLayout->removeWidget(*itr);
    delete (*itr);
  }

  delete m_allOnBtn;
  delete m_allOffBtn;
  delete m_listLayout;
}

void VariableListItem::indexChanged(const QString& t_frequency) {
  if (m_variable) {
    m_variable->setReportingFrequency(openstudio::toString(t_frequency));
  }
}

void VariableListItem::setVariableEnabled(bool t_enabled) {
  m_onOffButton->setChecked(t_enabled);
  onOffClicked(t_enabled);
}

bool VariableListItem::isVariableEnabled() const {
  return m_onOffButton->isChecked();
}

void VariableListItem::setReportingFrequency(const std::string& freq) {
  if (m_variable) {
    m_variable->setReportingFrequency(freq);
  }
}

void VariableListItem::onOffClicked(bool t_on) {
  LOG(Debug, "onOffClicked " << t_on);
  m_combobox->setEnabled(t_on);

  if (t_on) {
    if (!m_variable) {
      openstudio::model::OutputVariable outputVariable(m_name, m_model);
      outputVariable.setReportingFrequency("Hourly");
      outputVariable.setKeyValue(m_keyValue);
      m_variable = outputVariable;

      m_combobox->bind<std::string>(
        *m_variable, static_cast<std::string (*)(const std::string&)>(&openstudio::toString),
        std::bind(&model::OutputVariable::reportingFrequencyValues), std::bind(&model::OutputVariable::reportingFrequency, m_variable.get_ptr()),
        std::bind(&model::OutputVariable::setReportingFrequency, m_variable.get_ptr(), std::placeholders::_1),
        boost::optional<NoFailAction>(std::bind(&model::OutputVariable::resetReportingFrequency, m_variable.get_ptr())),
        boost::optional<BasicQuery>(std::bind(&model::OutputVariable::isReportingFrequencyDefaulted, m_variable.get_ptr())));
    }
  } else {
    if (m_variable) {
      m_combobox->unbind();
      m_variable->remove();
      m_variable = boost::none;
    }
  }
}

VariablesList::VariablesList(openstudio::model::Model t_model) : m_model(t_model), m_dirty(true), m_searchActive(false) {
  t_model.getImpl<openstudio::model::detail::Model_Impl>()->addWorkspaceObject.connect<VariablesList, &VariablesList::onAdded>(this);

  t_model.getImpl<openstudio::model::detail::Model_Impl>()->removeWorkspaceObject.connect<VariablesList, &VariablesList::onRemoved>(this);

  // TODO: should probably use a GridLayout, this is getting out of control

  auto* vbox = new QVBoxLayout();
  vbox->setContentsMargins(10, 10, 10, 10);
  vbox->setSpacing(10);
  setLayout(vbox);

  vbox->addWidget(new QLabel("<b>" + tr("Select Output Variables") + "</b>"));

  auto* displayHLayout = new QHBoxLayout();
  m_displayAllBtn = new QPushButton(tr("All"));
  m_displayAllBtn->setFlat(true);
  m_displayAllBtn->setObjectName("StandardGrayButton");
  m_displayAllBtn->setCheckable(true);
  m_displayAllBtn->setChecked(true);

  m_displayOnlyEnabledBtn = new QPushButton(tr("Enabled"));
  m_displayOnlyEnabledBtn->setFlat(true);
  m_displayOnlyEnabledBtn->setObjectName("StandardGrayButton");
  m_displayOnlyEnabledBtn->setCheckable(true);
  m_displayOnlyEnabledBtn->setChecked(false);

  m_displayOnlyDisabledBtn = new QPushButton(tr("Disabled"));
  m_displayOnlyDisabledBtn->setFlat(true);
  m_displayOnlyDisabledBtn->setObjectName("StandardGrayButton");
  m_displayOnlyDisabledBtn->setCheckable(true);
  m_displayOnlyDisabledBtn->setChecked(false);

  displayHLayout->addWidget(m_displayAllBtn);
  displayHLayout->addWidget(m_displayOnlyEnabledBtn);
  displayHLayout->addWidget(m_displayOnlyDisabledBtn);
  connect(m_displayAllBtn, &QPushButton::clicked, [this]() {
    m_displayOnlyEnabledBtn->setChecked(false);
    m_displayOnlyDisabledBtn->setChecked(false);
    this->onSearchTextEdited(this->m_searchText);
  });

  connect(m_displayOnlyEnabledBtn, &QPushButton::clicked, [this]() {
    m_displayAllBtn->setChecked(false);
    m_displayOnlyDisabledBtn->setChecked(false);
    this->onSearchTextEdited(this->m_searchText);
  });

  connect(m_displayOnlyDisabledBtn, &QPushButton::clicked, [this]() {
    m_displayAllBtn->setChecked(false);
    m_displayOnlyEnabledBtn->setChecked(false);
    this->onSearchTextEdited(this->m_searchText);
  });

  // displayHLayout->addStretch();

  auto* vline = new QFrame();
  vline->setFrameShape(QFrame::VLine);
  vline->setFrameShadow(QFrame::Sunken);
  displayHLayout->addWidget(vline);

  displayHLayout->addWidget(new QLabel(tr("Filter Variables")));
  m_searchBox = new QLineEdit();
  m_searchBox->setClearButtonEnabled(true);
  displayHLayout->addWidget(m_searchBox);
  connect(m_searchBox, &QLineEdit::textEdited, this, &VariablesList::onSearchTextEdited);

  m_searchUseRegex = new QCheckBox();
  m_searchUseRegex->setText(tr("Use Regex"));
  m_searchUseRegex->setChecked(false);
  connect(m_searchUseRegex, &QCheckBox::stateChanged, [this](int) { this->onSearchTextEdited(this->m_searchText); });
  displayHLayout->addWidget(m_searchUseRegex);
  displayHLayout->addStretch();
  vbox->addLayout(displayHLayout);

  vbox->addWidget(new QLabel("<b>" + tr("Update Visible Variables") + "</b>"));

  auto* applyHLayout = new QHBoxLayout();
  m_allOnBtn = new QPushButton(tr("All On"));
  m_allOnBtn->setFlat(true);
  m_allOnBtn->setObjectName("StandardGrayButton");
  applyHLayout->addWidget(m_allOnBtn);
  connect(m_allOnBtn, &QPushButton::clicked, this, &VariablesList::allOnClicked);

  m_allOffBtn = new QPushButton(tr("All Off"));
  m_allOffBtn->setFlat(true);
  m_allOffBtn->setObjectName("StandardGrayButton");
  applyHLayout->addWidget(m_allOffBtn);
  connect(m_allOffBtn, &QPushButton::clicked, this, &VariablesList::allOffClicked);

  m_applyFrequencyBtn = new QPushButton(tr("Apply Frequency"));
  m_applyFrequencyBtn->setFlat(true);
  m_applyFrequencyBtn->setObjectName("StandardGrayButton");
  connect(m_applyFrequencyBtn, &QPushButton::clicked, this, &VariablesList::applyFrequencyToAllVisibleClicked);
  applyHLayout->addWidget(m_applyFrequencyBtn);

  m_frequencyComboBox = new QComboBox;
  m_frequencyComboBox->addItem(tr("Detailed"), "Detailed");
  m_frequencyComboBox->addItem(tr("Timestep"), "Timestep");
  m_frequencyComboBox->addItem(tr("Hourly"), "Hourly");
  m_frequencyComboBox->addItem(tr("Daily"), "Daily");
  m_frequencyComboBox->addItem(tr("Monthly"), "Monthly");
  m_frequencyComboBox->addItem(tr("RunPeriod"), "RunPeriod");
  m_frequencyComboBox->addItem(tr("Annual"), "Annual");
  m_frequencyComboBox->setCurrentIndex(2);
  applyHLayout->addWidget(m_frequencyComboBox);

  applyHLayout->addStretch();
  m_progressBar = new ProgressBarWithError();
  m_progressBar->setFixedWidth(200);
  m_progressBar->setVisible(false);
  applyHLayout->addWidget(m_progressBar);

  vbox->addLayout(applyHLayout);

  m_listLayout = new QVBoxLayout();
  vbox->addLayout(m_listLayout);
  vbox->addStretch();

  updateVariableList();
}

void VariablesList::allOnClicked() {
  enableAll(true);
}

void VariablesList::allOffClicked() {
  enableAll(false);
}

void VariablesList::enableAll(bool t_enabled) {
  this->setEnabled(false);
  m_progressBar->setRange(0, m_variables.size());
  m_progressBar->setVisible(true);

  int num = 0;
  for (auto& m_variable : m_variables) {
    if (m_variable->isVisible()) {
      m_variable->setVariableEnabled(t_enabled);
    }
    m_progressBar->setValue(++num);
  }

  m_progressBar->setVisible(false);
  this->setEnabled(true);
}

void VariablesList::applyFrequencyToAllVisibleClicked() {
  const std::string freqStr = m_frequencyComboBox->currentData().toString().toStdString();

  this->setEnabled(false);
  m_progressBar->setRange(0, m_variables.size());
  m_progressBar->setVisible(true);

  int num = 0;
  for (auto& m_variable : m_variables) {
    if (m_variable->isVisible()) {
      m_variable->setReportingFrequency(freqStr);
    }
    m_progressBar->setValue(++num);
  }

  m_progressBar->setVisible(false);
  this->setEnabled(true);
}

void VariablesList::onAdded(const WorkspaceObject&, const openstudio::IddObjectType& type, const openstudio::UUID&) {
  LOG(Debug, "onAdded: " << type.valueName());

  /// \todo if the user is able to add an output variable through some other means it will not show up here and now
  if (type != openstudio::IddObjectType::OS_Output_Variable) {
    if (!m_dirty) {
      m_dirty = true;
      QTimer::singleShot(0, this, &VariablesList::updateVariableList);
    }
  }
}

void VariablesList::onRemoved(const WorkspaceObject& /*unused*/, const openstudio::IddObjectType& type, const openstudio::UUID& /*unused*/) {
  LOG(Debug, "onRemoved " << type.valueName());

  /// \todo if the user is remove to add an output variable through some other means it will not show up here and now
  if (type != openstudio::IddObjectType::OS_Output_Variable) {
    if (!m_dirty) {
      m_dirty = true;
      QTimer::singleShot(0, this, &VariablesList::updateVariableList);
    }
  }
}

struct PotentialOutputVariable
{
  std::string name;
  std::string keyValue;
  boost::optional<openstudio::model::OutputVariable> variable;
};

bool VariableListItem::matchesText(const QString& text, bool useRegex) const {

  const auto qName = QString::fromStdString(m_name);
  if (qName.contains(text, Qt::CaseInsensitive)) {
    return true;
  }
  QString qKeyValue;
  if (!m_keyValue.empty() && m_keyValue != "*") {
    qKeyValue = QString::fromStdString(m_keyValue);
    if (qKeyValue.contains(text, Qt::CaseInsensitive)) {
      return true;
    }
  }

  if (useRegex) {
    const QRegularExpression re(text, QRegularExpression::CaseInsensitiveOption);
    if (auto m = re.match(qName); m.hasMatch()) {
      return true;
    }
    if (!qKeyValue.isEmpty()) {
      if (auto m = re.match(qKeyValue); m.hasMatch()) {
        return true;
      }
    }
  }
  return false;
}

void VariablesList::onSearchTextEdited(const QString& text) {
  m_searchText = text;
  m_searchActive = !text.isEmpty();

  const bool displayOnlyEnabled = m_displayOnlyEnabledBtn->isChecked();
  const bool displayOnlyDisabled = m_displayOnlyDisabledBtn->isChecked();
  for (auto& variableListItemPtr : m_variables) {
    if (displayOnlyEnabled && !variableListItemPtr->isVariableEnabled()) {
      variableListItemPtr->hide();
    } else if (displayOnlyDisabled && variableListItemPtr->isVariableEnabled()) {
      variableListItemPtr->hide();
    } else if (m_searchActive && !variableListItemPtr->matchesText(text, m_searchUseRegex->isChecked())) {
      variableListItemPtr->hide();
    } else {
      variableListItemPtr->show();
    }
  }
}

void VariablesList::updateVariableList() {
  m_variables.clear();
  while (m_listLayout->count() > 0) {
    QLayoutItem* qli = m_listLayout->takeAt(0);
    delete qli->widget();
    delete qli;
  }

  // map of variable name + keyValue to PotentialOutputVariable
  std::map<std::string, PotentialOutputVariable> potentialOutputVariableMap;

  // make list of all potential variables
  for (const openstudio::model::ModelObject& modelObject : m_model.getModelObjects<openstudio::model::ModelObject>()) {
    for (const std::string& variableName : modelObject.outputVariableNames()) {
      //LOG(Debug, "Found variableName: " << variableName);
      std::string variableNameKeyValue = variableName + "*";
      if (potentialOutputVariableMap.count(variableNameKeyValue) == 0) {
        PotentialOutputVariable pov;
        pov.name = variableName;
        pov.keyValue = "*";
        potentialOutputVariableMap.insert(std::pair<std::string, PotentialOutputVariable>(variableNameKeyValue, pov));
      }
    }
  }

  // add all variables to map, allow only one variable per variable name + keyValue in this application
  for (openstudio::model::OutputVariable outputVariable : m_model.getConcreteModelObjects<openstudio::model::OutputVariable>()) {
    const std::string variableName = outputVariable.variableName();
    const std::string keyValue = outputVariable.keyValue();
    const std::string variableNameKeyValue = variableName + keyValue;

    if (potentialOutputVariableMap.count(variableNameKeyValue) == 0) {
      // DLM: this was causing too much trouble because it kept deleting variables added by users
      // there is no place for this outputvariable with the current objects, delete it.
      //outputVariable.remove();

      // user defined variable, add it to the list
      PotentialOutputVariable pov;
      pov.name = outputVariable.variableName();
      pov.keyValue = outputVariable.keyValue();
      pov.variable = outputVariable;
      potentialOutputVariableMap.insert(std::pair<std::string, PotentialOutputVariable>(variableNameKeyValue, pov));

    } else {

      if (potentialOutputVariableMap[variableNameKeyValue].variable) {
        // already have output variable for this name + keyName, then remove this object
        outputVariable.remove();
      } else {
        // this is a predefined variable

        // make sure that key value is set to '*'
        // DLM: we know this is already '*' because all predefined variables are set to '*'
        //outputVariable.setKeyValue("*");

        potentialOutputVariableMap[variableNameKeyValue].variable = outputVariable;
      }
    }
  }

  for (const auto& itr : potentialOutputVariableMap) {
    const PotentialOutputVariable& pov = itr.second;

    //LOG(Debug, "Creating VariableListItem for: " << pov.name << ", " << pov.keyValue);
    auto* li = new VariableListItem(pov.name, pov.keyValue, pov.variable, m_model);
    m_listLayout->addWidget(li);
    m_variables.push_back(li);
  }

  m_listLayout->addStretch();

  m_dirty = false;
}

VariablesTabView::VariablesTabView(openstudio::model::Model t_model, QWidget* parent)
  : MainTabView("Output Variables", MainTabView::MAIN_TAB, parent) {
  auto* scrollarea = new QScrollArea();
  auto* vl = new VariablesList(std::move(t_model));
  scrollarea->setWidget(vl);
  scrollarea->setWidgetResizable(true);
  addTabWidget(scrollarea);
  vl->setAutoFillBackground(false);
}

}  // namespace openstudio
