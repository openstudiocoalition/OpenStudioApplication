/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "BCLMeasureDialog.hpp"

#include "../model_editor/UserSettings.hpp"
#include "../model_editor/Utilities.hpp"

// TODO: delete once the Labs CLI is the default
#include "../openstudio_lib/OSAppBase.hpp"
#include "../openstudio_lib/OSDocument.hpp"
#include "../openstudio_lib/MainWindow.hpp"

#include <openstudio/utilities/core/Assert.hpp>
#include <openstudio/utilities/core/StringHelpers.hpp>
#include <openstudio/utilities/core/PathHelpers.hpp>

#include <QBoxLayout>
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>
#include <QTextEdit>

namespace openstudio {

BCLMeasureDialog::BCLMeasureDialog(QWidget* parent) : OSDialog(false, parent) {
  setWindowTitle("Create New Measure");
  setWindowModality(Qt::ApplicationModal);

  init();

  m_nameLineEdit->setText("New Measure");
  m_descriptionTextEdit->setText(
    "Replace this text with an explanation of what the measure does in terms that can be understood by a general building professional audience "
    "(building owners, architects, engineers, contractors, etc.).  This description will be used to create reports aimed at convincing the owner "
    "and/or design team to implement the measure in the actual building design.  For this reason, the description may include details about how the "
    "measure would be implemented, along with explanations of qualitative benefits associated with the measure.  It is good practice to include "
    "citations in the measure if the description is taken from a known source or if specific benefits are listed.");
  m_modelerDescriptionTextEdit->setText(
    "Replace this text with an explanation for the energy modeler specifically.  It should explain how the measure is modeled, including any "
    "requirements about how the baseline model must be set up, major assumptions, citations of references to applicable modeling resources, etc.  "
    "The energy modeler should be able to read this description and understand what changes the measure is making to the model and why these changes "
    "are being made.  Because the Modeler Description is written for an expert audience, using common abbreviations for brevity is good practice.");

  m_taxonomyFirstLevelComboBox->setCurrentIndex(0);
  firstLevelTaxonomyChanged(m_taxonomyFirstLevelComboBox->currentText());
}

BCLMeasureDialog::BCLMeasureDialog(const BCLMeasure& bclMeasure, QWidget* parent) : m_bclMeasureToCopy(bclMeasure) {
  setWindowTitle("Copy Measure");

  init();

  m_nameLineEdit->setText(toQString(bclMeasure.displayName() + " Copy"));
  m_descriptionTextEdit->setText(toQString(bclMeasure.description()));
  m_modelerDescriptionTextEdit->setText(toQString(bclMeasure.modelerDescription()));

  {
    QString measureTypeString;
    const MeasureType measureType = bclMeasure.measureType();
    if (measureType == MeasureType::ModelMeasure) {
      measureTypeString = "OpenStudio Measure";
    } else if (measureType == MeasureType::EnergyPlusMeasure) {
      measureTypeString = "EnergyPlus Measure";
    } else if (measureType == MeasureType::UtilityMeasure) {
      measureTypeString = "Utility Measure";
    } else if (measureType == MeasureType::ReportingMeasure) {
      measureTypeString = "Reporting Measure";
    }
    const int index = m_measureTypeComboBox->findText(measureTypeString);
    m_measureTypeComboBox->setCurrentIndex(index);
  }

  {
    QString measureLanguageString = "Ruby";
    const MeasureLanguage measureLanguage = bclMeasure.measureLanguage();
    if (measureLanguage == MeasureLanguage::Python) {
      measureLanguageString = "Python";
    }
    const int index = m_measureLanguageComboBox->findText(measureLanguageString);
    m_measureLanguageComboBox->setCurrentIndex(index);
    // Display the Language, but do not allow editing it!
    m_measureLanguageComboBox->setEnabled(false);
  }

  QStringList taxonomyParts = toQString(bclMeasure.taxonomyTag()).split('.');
  if (!taxonomyParts.empty()) {
    const int firstIndex = m_taxonomyFirstLevelComboBox->findText(taxonomyParts[0]);
    m_taxonomyFirstLevelComboBox->setCurrentIndex(firstIndex);
    firstLevelTaxonomyChanged(m_taxonomyFirstLevelComboBox->currentText());

    const int secondIndex = m_taxonomySecondLevelComboBox->findText(taxonomyParts[1]);
    m_taxonomySecondLevelComboBox->setCurrentIndex(secondIndex);
  }

  const std::vector<std::string> intendedSoftwareTools = bclMeasure.intendedSoftwareTools();
  QList<QListWidgetItem*> items = m_intendedSoftwareToolListWidget->findItems(".*", Qt::MatchRegularExpression);
  for (QListWidgetItem* item : items) {
    const std::string intendedSoftwareTool = toString(item->text());
    if (std::find(intendedSoftwareTools.begin(), intendedSoftwareTools.end(), intendedSoftwareTool) == intendedSoftwareTools.end()) {
      item->setCheckState(Qt::Unchecked);
    } else {
      item->setCheckState(Qt::Checked);
    }
  }

  const std::vector<std::string> intendedUseCases = bclMeasure.intendedUseCases();
  items = m_intendedUseCaseListWidget->findItems(".*", Qt::MatchRegularExpression);
  for (QListWidgetItem* item : items) {
    const std::string intendedUseCase = toString(item->text());
    if (std::find(intendedUseCases.begin(), intendedUseCases.end(), intendedUseCase) == intendedUseCases.end()) {
      item->setCheckState(Qt::Unchecked);
    } else {
      item->setCheckState(Qt::Checked);
    }
  }
}

BCLMeasureDialog::~BCLMeasureDialog() = default;

QSize BCLMeasureDialog::sizeHint() const {
  return {770, 540};
}

boost::optional<openstudio::BCLMeasure> BCLMeasureDialog::createMeasure() {
  const openstudio::path umd = userMeasuresDir();

  if (isNetworkPath(umd) && !isNetworkPathAvailable(umd)) {
    QMessageBox::information(this, "Cannot Create Measure",
                             "Your My Measures Directory appears to be on a network drive that is not currently available.\nYou can change your "
                             "specified My Measures Directory using 'Preferences->Change My Measures Directory'.",
                             QMessageBox::Ok);
    return boost::none;
  }

  const std::string name = toString(m_nameLineEdit->text());
  const std::string className = BCLMeasure::makeClassName(name);
  std::string lowerClassName = toUnderscoreCase(className);
  const std::string description = toString(m_descriptionTextEdit->toPlainText());
  const std::string modelerDescription = toString(m_modelerDescriptionTextEdit->toPlainText());

  const std::string measureTypeStr = toString(m_measureTypeComboBox->currentText());
  MeasureType measureType;
  if (measureTypeStr == "OpenStudio Measure") {
    measureType = MeasureType::ModelMeasure;
  } else if (measureTypeStr == "EnergyPlus Measure") {
    measureType = MeasureType::EnergyPlusMeasure;
  } else if (measureTypeStr == "Utility Measure") {
    measureType = MeasureType::UtilityMeasure;
  } else if (measureTypeStr == "Reporting Measure") {
    measureType = MeasureType::ReportingMeasure;
  }

  const std::string measureLanguageStr = toString(m_measureLanguageComboBox->currentText());
  MeasureLanguage measureLanguage = MeasureLanguage::Ruby;
  if (measureLanguageStr == "Python") {
    measureLanguage = MeasureLanguage::Python;

    const bool useClassicCLI = OSAppBase::instance()->currentDocument()->mainWindow()->useClassicCLI();
    if (useClassicCLI) {
      QMessageBox::information(
        this, "Python Measures not available in Classic CLI",
        "Python Measures are not supported in the Classic CLI.\nYou can change CLI version using 'Preferences->Use Classic CLI'.", QMessageBox::Ok);
    }
  }

  QString folderName = toQString(lowerClassName).append("/");
  openstudio::path measureDir = umd / toPath(folderName);

  // prompt user ???
  if (openstudio::filesystem::exists(measureDir)) {
    int i = 1;
    while (openstudio::filesystem::exists(measureDir)) {
      folderName = toQString(lowerClassName).append(" ").append(QString::number(i)).append("/");
      measureDir = umd / toPath(folderName);
      ++i;
    }
    // DLM: do we want to alter the class name to indicate this copy?
    //className = className + toString(QString::number(i));
    //lowerClassName = lowerClassName + toString(QString::number(i));
  }

  QStringList taxonomyParts;
  const QString taxonomyFirstLevel = m_taxonomyFirstLevelComboBox->currentText();
  const QString taxonomySecondLevel = m_taxonomySecondLevelComboBox->currentText();
  taxonomyParts << taxonomyFirstLevel;
  if (!taxonomySecondLevel.isEmpty()) {
    taxonomyParts << taxonomySecondLevel;
  }
  const std::string taxonomyTag = toString(taxonomyParts.join("."));

  std::vector<Attribute> attributes;

  QList<QListWidgetItem*> items = m_intendedSoftwareToolListWidget->findItems(".*", Qt::MatchRegularExpression);
  for (QListWidgetItem* item : items) {
    if (item->checkState() == Qt::Checked) {
      std::string intendedSoftwareTool = toString(item->text());
      attributes.emplace_back("Intended Software Tool", std::move(intendedSoftwareTool));
    }
  }

  items = m_intendedUseCaseListWidget->findItems(".*", Qt::MatchRegularExpression);
  for (QListWidgetItem* item : items) {
    if (item->checkState() == Qt::Checked) {
      std::string intendedUseCase = toString(item->text());
      attributes.emplace_back("Intended Use Case", std::move(intendedUseCase));
    }
  }

  boost::optional<BCLMeasure> result;
  if (m_bclMeasureToCopy) {
    // have measure to copy, use clone
    result = m_bclMeasureToCopy->clone(measureDir);
    if (result) {
      result->changeUID();

      // change the files on disk
      result->updateMeasureScript(m_bclMeasureToCopy->measureType(), measureType, m_bclMeasureToCopy->measureLanguage(),
                                  m_bclMeasureToCopy->measureLanguage(), m_bclMeasureToCopy->className(), className, name, description,
                                  modelerDescription);

      result->updateMeasureTests(m_bclMeasureToCopy->className(), className);

      result->checkForUpdatesFiles();

      // change the xml
      lowerClassName = toUnderscoreCase(className);

      result->setName(lowerClassName);
      result->setClassName(className);
      result->setDisplayName(name);
      result->setDescription(description);
      result->setModelerDescription(modelerDescription);
      result->setArguments(m_bclMeasureToCopy->arguments());
      result->setTaxonomyTag(taxonomyTag);
      result->setMeasureType(measureType);
      result->setMeasureLanguage(measureLanguage);

      // xml checksum is out of date

      for (const Attribute& attribute : attributes) {
        result->addAttribute(attribute);
      }

      result->save();
    }
  } else {
    try {
      // starting new measure
      result = BCLMeasure(name, className, measureDir, taxonomyTag, measureType, description, modelerDescription, measureLanguage);

      for (const Attribute& attribute : attributes) {
        result->addAttribute(attribute);
      }

      result->save();
    } catch (std::exception&) {
    }
  }

  return result;
}

void BCLMeasureDialog::nameChanged(const QString& newName) {
  const std::string className = BCLMeasure::makeClassName(toString(newName));
  m_classNameLabel->setText(toQString(className));
}

void BCLMeasureDialog::measureTypeChanged(const QString& newName) {
  if (newName == "Reporting Measure") {
    const int index = m_taxonomyFirstLevelComboBox->findText("Reporting");
    m_taxonomyFirstLevelComboBox->setCurrentIndex(index);
  } else {
    // DLM: do we want to toggle this back?
    m_taxonomyFirstLevelComboBox->setCurrentIndex(0);
  }

  if (newName == "OpenStudio Measure") {
    // DLM: do we want to toggle this back?
    QList<QListWidgetItem*> items =
      m_intendedSoftwareToolListWidget->findItems("Apply Measure Now", Qt::MatchFixedString);  // NOLINT(misc-const-correctness)
    for (QListWidgetItem* item : items) {
      item->setCheckState(Qt::Checked);
    }
  } else {
    QList<QListWidgetItem*> items = m_intendedSoftwareToolListWidget->findItems("Apply Measure Now", Qt::MatchFixedString);
    for (QListWidgetItem* item : items) {
      item->setCheckState(Qt::Unchecked);
    }
  }
}

void BCLMeasureDialog::firstLevelTaxonomyChanged(const QString& newName) {
  m_taxonomySecondLevelComboBox->clear();
  m_taxonomySecondLevelComboBox->setEnabled(false);

  const std::vector<std::string> secondLevelTerms = BCLMeasure::suggestedSecondLevelTaxonomyTerms(toString(newName));

  if (!secondLevelTerms.empty()) {
    for (const std::string& secondLevelTerm : secondLevelTerms) {
      m_taxonomySecondLevelComboBox->addItem(toQString(secondLevelTerm));
    }
    m_taxonomySecondLevelComboBox->setCurrentIndex(0);
    m_taxonomySecondLevelComboBox->setEnabled(true);
  }
}

void BCLMeasureDialog::init() {
#ifdef Q_OS_DARWIN
  const int btnHeight = 44;
#else
  const int btnHeight = 34;
#endif

  QPushButton* cancelButton = this->cancelButton();
  cancelButton->setText("Cancel");
  cancelButton->setMinimumHeight(btnHeight);
  cancelButton->setDefault(true);

  QPushButton* okButton = this->okButton();
  okButton->setText("Create Measure and \n Open for Editing");
  okButton->setMinimumHeight(btnHeight);

  auto* vLayout = new QVBoxLayout;
  vLayout->setContentsMargins(QMargins(0, 0, 0, 0));

  auto* label = new QLabel;
  label->setText("Name:");
  label->setObjectName("H2");
  vLayout->addWidget(label);
  m_nameLineEdit = new QLineEdit(this);
  vLayout->addWidget(m_nameLineEdit);

  auto* tempHLayout = new QHBoxLayout;
  label = new QLabel;
  label->setText("Class Name:");
  label->setObjectName("H2");
  tempHLayout->addWidget(label);
  m_classNameLabel = new QLabel(this);
  tempHLayout->addWidget(m_classNameLabel);
  tempHLayout->addStretch();
  vLayout->addLayout(tempHLayout);
  vLayout->addSpacing(10);

  label = new QLabel;
  label->setText("Description:");
  label->setObjectName("H2");
  vLayout->addWidget(label);
  m_descriptionTextEdit = new QTextEdit(this);
  m_descriptionTextEdit->setAcceptRichText(false);
  m_descriptionTextEdit->setTabChangesFocus(true);
  vLayout->addWidget(m_descriptionTextEdit);
  vLayout->addSpacing(10);

  label = new QLabel;
  label->setText("Modeler Description:");
  label->setObjectName("H2");
  vLayout->addWidget(label);
  m_modelerDescriptionTextEdit = new QTextEdit(this);
  m_modelerDescriptionTextEdit->setAcceptRichText(false);
  m_modelerDescriptionTextEdit->setTabChangesFocus(true);
  vLayout->addWidget(m_modelerDescriptionTextEdit);
  vLayout->addSpacing(10);

  tempHLayout = new QHBoxLayout;
  vLayout->addLayout(tempHLayout);

  auto* tempVLayout = new QVBoxLayout;

  label = new QLabel;
  label->setText("Measure Type:");
  label->setObjectName("H2");
  tempVLayout->addWidget(label);
  m_measureTypeComboBox = new QComboBox(this);
  m_measureTypeComboBox->addItem("OpenStudio Measure");
  m_measureTypeComboBox->addItem("EnergyPlus Measure");
  //m_measureTypeComboBox->addItem("Utility Measure"); // Disable for now
  m_measureTypeComboBox->addItem("Reporting Measure");
  m_measureTypeComboBox->setCurrentIndex(0);
  tempVLayout->addWidget(m_measureTypeComboBox);
  tempVLayout->addSpacing(10);
  tempHLayout->addLayout(tempVLayout);

  tempVLayout = new QVBoxLayout;

  label = new QLabel;
  label->setText("Measure Language:");
  label->setObjectName("H2");
  tempVLayout->addWidget(label);
  m_measureLanguageComboBox = new QComboBox(this);
  m_measureLanguageComboBox->addItem("Ruby");
  if (!OSAppBase::instance()->currentDocument()->mainWindow()->useClassicCLI()) {
    m_measureLanguageComboBox->addItem("Python");
  }
  m_measureLanguageComboBox->setCurrentIndex(0);
  tempVLayout->addWidget(m_measureLanguageComboBox);
  tempVLayout->addSpacing(10);
  tempHLayout->addLayout(tempVLayout);

  tempVLayout = new QVBoxLayout;

  label = new QLabel;
  label->setText("Taxonomy:");
  label->setObjectName("H2");
  tempVLayout->addWidget(label);
  m_taxonomyFirstLevelComboBox = new QComboBox(this);

  const std::vector<std::string> firstLevelTerms = BCLMeasure::suggestedFirstLevelTaxonomyTerms();
  for (const std::string& firstLevelTerm : firstLevelTerms) {
    m_taxonomyFirstLevelComboBox->addItem(toQString(firstLevelTerm));
  }

  m_taxonomySecondLevelComboBox = new QComboBox(this);
  auto* tempHLayout2 = new QHBoxLayout;
  tempHLayout2->addWidget(m_taxonomyFirstLevelComboBox);
  tempHLayout2->addWidget(m_taxonomySecondLevelComboBox);
  tempVLayout->addLayout(tempHLayout2);
  tempVLayout->addSpacing(10);
  tempHLayout->addLayout(tempVLayout);

  tempHLayout = new QHBoxLayout;
  vLayout->addLayout(tempHLayout);

  tempVLayout = new QVBoxLayout;

  label = new QLabel;
  label->setText("Intended Software Tools:");
  label->setObjectName("H2");
  tempVLayout->addWidget(label);
  m_intendedSoftwareToolListWidget = new QListWidget(this);
  tempVLayout->addWidget(m_intendedSoftwareToolListWidget);
  QStringList intendedSoftwareTools;
  for (const std::string& suggestedTool : BCLMeasure::suggestedIntendedSoftwareTools()) {
    intendedSoftwareTools.append(toQString(suggestedTool));
  }
  QStringListIterator it(intendedSoftwareTools);
  while (it.hasNext()) {
    const QString intendedSoftwareTool = it.next();
    auto* listItem = new QListWidgetItem(intendedSoftwareTool, m_intendedSoftwareToolListWidget);
    // DLM: defaults per David
    if (intendedSoftwareTool == "Analysis Spreadsheet") {
      listItem->setCheckState(Qt::Unchecked);
    } else {
      listItem->setCheckState(Qt::Checked);
    }
    listItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    m_intendedSoftwareToolListWidget->addItem(listItem);
  }
  tempHLayout->addLayout(tempVLayout);

  tempVLayout = new QVBoxLayout;

  label = new QLabel;
  label->setText("Intended Use Cases:");
  label->setObjectName("H2");
  tempVLayout->addWidget(label);
  m_intendedUseCaseListWidget = new QListWidget();
  tempVLayout->addWidget(m_intendedUseCaseListWidget);
  QStringList intendedUseCases;
  for (const std::string& suggestedUseCase : BCLMeasure::suggestedIntendedUseCases()) {
    intendedUseCases.append(toQString(suggestedUseCase));
  }
  it = QStringListIterator(intendedUseCases);
  while (it.hasNext()) {
    const QString intendedUseCase = it.next();
    auto* listItem = new QListWidgetItem(intendedUseCase, m_intendedUseCaseListWidget);
    // DLM: default to unchecked per David
    listItem->setCheckState(Qt::Unchecked);
    listItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    m_intendedUseCaseListWidget->addItem(listItem);
  }
  tempHLayout->addLayout(tempVLayout);

  QBoxLayout* upperLayout = this->upperLayout();
  upperLayout->addLayout(vLayout);
  upperLayout->addStretch();

  connect(m_nameLineEdit, &QLineEdit::textChanged, this, &BCLMeasureDialog::nameChanged);

  connect(m_measureTypeComboBox, static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::currentTextChanged), this,
          &BCLMeasureDialog::measureTypeChanged);

  connect(m_taxonomyFirstLevelComboBox, static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::currentTextChanged), this,
          &BCLMeasureDialog::firstLevelTaxonomyChanged);

  this->setWindowModality(Qt::ApplicationModal);
  //this->setSizeGripEnabled(true);

#ifdef Q_OS_DARWIN
  setWindowFlags(Qt::FramelessWindowHint);
#else
  setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
#endif
}

}  // namespace openstudio
