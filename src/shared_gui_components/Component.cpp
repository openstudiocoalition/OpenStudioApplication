/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "Component.hpp"

#include "../model_editor/Utilities.hpp"
#include "../openstudio_lib/OSAppBase.hpp"
#include "../openstudio_lib/OSDocument.hpp"

#include <openstudio/utilities/bcl/BCLMeasure.hpp>
#include <openstudio/utilities/bcl/LocalBCL.hpp>
#include <openstudio/utilities/core/Assert.hpp>
#include <openstudio/utilities/core/Compare.hpp>
#include <openstudio/utilities/units/Quantity.hpp>
#include <openstudio/utilities/units/Unit.hpp>
#include <openstudio/utilities/time/DateTime.hpp>

#include <openstudio/OpenStudio.hxx>

#include <QBoxLayout>
#include <QCheckBox>
#include <QHeaderView>
#include <QLabel>
#include <QPainter>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QDateTime>

#define OPENSTUDIO_TYPE "OpenStudio Type"

namespace openstudio {

Component::Component(const BCLMeasure& bclMeasure, bool showAbridgedView, bool showCheckBox, QWidget* parent)
  : QAbstractButton(parent),
    m_error(boost::none),
    m_showAbridgedView(showAbridgedView),
    m_showCheckBox(showCheckBox),
    m_checkBox(nullptr),
    m_msg(nullptr) {
  setCheckable(true);
  parseBCLMeasure(bclMeasure);
  if (m_showAbridgedView) {
    createAbridgedLayout();
  } else {
    createCompleteLayout();
  }

  // This measure was created with a newer version of OpenStudio
  if (!m_available) {
    if (m_checkBox) {
      m_checkBox->setChecked(false);
      setCheckBoxEnabled(false);
      m_updateAvailable = false;
      if (m_msg) {
        m_msg->setText("This measure requires a newer version of OpenStudio");
        m_msg->setVisible(true);
      }
    }
  } else if (m_error) {
    // This measures has been pre-filtered and is known to require an update but has an error
    m_checkBox->setChecked(false);
    setCheckBoxEnabled(false);
    m_checkBox->setObjectName("Disabled");
    m_updateAvailable = false;
    if (m_msg) {
      m_msg->setText("This measure cannot be updated because it has an error");
      m_msg->setVisible(true);
    }
  } else {
    // This measures has been pre-filtered and is known to require an update
    m_checkBox->setChecked(false);
    setCheckBoxEnabled(true);
    m_checkBox->setObjectName("UpdateAvailable");
    m_updateAvailable = true;
    if (m_msg) {
      m_msg->setText("An update is available for this measure");
      m_msg->setVisible(true);
    }
  }
}

Component::Component(const BCLSearchResult& bclSearchResult, bool showAbridgedView, bool showCheckBox, QWidget* parent)
  : QAbstractButton(parent),
    m_error(boost::none),
    m_showAbridgedView(showAbridgedView),
    m_showCheckBox(showCheckBox),
    m_checkBox(nullptr),
    m_msg(nullptr) {
  setCheckable(true);
  parseBCLSearchResult(bclSearchResult);
  if (m_showAbridgedView) {
    createAbridgedLayout();
  } else {
    createCompleteLayout();
  }

  if (bclSearchResult.componentType() == "component") {
    // This component has already been downloaded
    if (OSAppBase::instance()->currentDocument()->getLocalComponent(this->uid(), this->versionId())) {
      if (m_checkBox) {
        m_checkBox->setChecked(true);
        setCheckBoxEnabled(false);
        m_updateAvailable = false;
      }
      // This component has an update
    } else if (OSAppBase::instance()->currentDocument()->getLocalComponent(this->uid())) {
      if (m_checkBox) {
        m_checkBox->setChecked(false);
        setCheckBoxUpdateAvailable(true);
        m_updateAvailable = true;
        if (m_msg) {
          m_msg->setText("An update is available for this component");
          m_msg->setVisible(true);
        }
      }
      // This component has not yet been downloaded
    } else {
      if (m_checkBox) {
        m_checkBox->setChecked(false);
        setCheckBoxEnabled(true);
        m_updateAvailable = false;
      }
    }
  } else if (bclSearchResult.componentType() == "measure") {
    // This measure was created with a newer version of OpenStudio
    if (!m_available) {
      if (m_checkBox) {
        m_checkBox->setChecked(false);
        setCheckBoxEnabled(false);
        m_updateAvailable = false;
        if (m_msg) {
          m_msg->setText("This measure requires a newer version of OpenStudio");
          m_msg->setVisible(true);
        }
      }
    } else {
      // This measure has already been downloaded
      if (OSAppBase::instance()->currentDocument()->getLocalMeasure(this->uid(), this->versionId())) {
        if (m_checkBox) {
          m_checkBox->setChecked(true);
          setCheckBoxEnabled(false);
          m_updateAvailable = false;
        }
        // This measure has an update
      } else if (OSAppBase::instance()->currentDocument()->getLocalMeasure(this->uid())) {
        if (m_checkBox) {
          m_checkBox->setChecked(false);
          setCheckBoxUpdateAvailable(true);
          m_updateAvailable = true;
          if (m_msg) {
            m_msg->setText("An update is available for this measure");
            m_msg->setVisible(true);
          }
        }
        // This measure has not yet been downloaded
      } else {
        if (m_checkBox) {
          m_checkBox->setChecked(false);
          setCheckBoxEnabled(true);
          m_updateAvailable = false;
        }
      }
    }
  }
}

Component::Component(bool showAbridgedView, bool showCheckBox, QWidget* parent)
  : QAbstractButton(parent),

    m_error(boost::none),

    m_showAbridgedView(showAbridgedView),
    m_showCheckBox(showCheckBox),
    m_checkBox(nullptr),
    m_msg(nullptr) {
  setCheckable(true);
  if (m_showAbridgedView) {
    createAbridgedLayout();
  } else {
    createCompleteLayout();
  }
}

Component::Component(const Component& other) {
  ///! no self-assignment
  if (this != &other) {
    m_componentType = other.m_componentType;
    m_name = other.m_name;
    m_uid = other.m_uid;
    m_versionId = other.m_versionId;
    m_description = other.m_description;
    m_modelerDescription = other.m_modelerDescription;
    m_versionModified = other.m_versionModified;
    m_fidelityLevel = other.m_fidelityLevel;
    m_error = other.m_error;
    m_attributes = other.m_attributes;
    m_arguments = other.m_arguments;
    m_files = other.m_files;
    m_fileReferences = other.m_fileReferences;
    m_provenances = other.m_provenances;
    m_tags = other.m_tags;
    m_available = other.m_available;
    m_updateAvailable = other.m_updateAvailable;

    m_org = other.m_org;
    m_repo = other.m_repo;
    m_releaseTag = other.m_releaseTag;

    m_showAbridgedView = false;
    m_showCheckBox = false;

    setCheckable(true);
    // if (m_showAbridgedView) {
    //  createAbridgedLayout();
    // } else {
    createCompleteLayout();
    // }
  }
}

// TODO The copy code for both operator= and copy CTOR should be in a common function
Component& Component::operator=(const Component& other) {
  ///! no self-assignment
  if (this != &other) {
    m_componentType = other.m_componentType;
    m_name = other.m_name;
    m_uid = other.m_uid;
    m_versionId = other.m_versionId;
    m_description = other.m_description;
    m_modelerDescription = other.m_modelerDescription;
    m_versionModified = other.m_versionModified;
    m_fidelityLevel = other.m_fidelityLevel;
    m_error = other.m_error;
    m_attributes = other.m_attributes;
    m_arguments = other.m_arguments;
    m_files = other.m_files;
    m_fileReferences = other.m_fileReferences;
    m_provenances = other.m_provenances;
    m_tags = other.m_tags;
    m_available = other.m_available;
    m_updateAvailable = other.m_updateAvailable;

    m_org = other.m_org;
    m_repo = other.m_repo;
    m_releaseTag = other.m_releaseTag;

    m_showAbridgedView = false;
    m_showCheckBox = false;

    setCheckable(true);
    // if (m_showAbridgedView) {
    //   createAbridgedLayout();
    // } else {
    createCompleteLayout();
    // }
  }

  return *this;
}

std::string Component::uid() const {
  return toString(m_uid);
}

std::string Component::versionId() const {
  return toString(m_versionId);
}

QLabel* Component::msg() {
  return m_msg;
}

void Component::setChecked(bool checked) {
  QAbstractButton::setChecked(checked);
  emit clicked(checked);
}

bool Component::checkBoxChecked() const {
  return m_checkBox->isChecked();
}

void Component::setCheckBoxChecked(bool checked) {
  m_checkBox->setChecked(checked);
}

bool Component::checkBoxEnabled() const {
  return m_checkBox->isEnabled();
}

void Component::setCheckBoxEnabled(bool enabled) {
  if (enabled) {
    m_checkBox->setEnabled(true);
    m_checkBox->setObjectName("Enabled");
  } else {
    m_checkBox->setEnabled(false);
    m_checkBox->setObjectName("Disabled");
  }
  m_checkBox->style()->unpolish(m_checkBox);
  m_checkBox->style()->polish(m_checkBox);
}

void Component::setCheckBoxUpdateAvailable(bool enabled) {
  m_checkBox->setEnabled(enabled);
  m_checkBox->setObjectName("UpdateAvailable");
  m_checkBox->style()->unpolish(m_checkBox);
  m_checkBox->style()->polish(m_checkBox);
}

void Component::parseBCLMeasure(const BCLMeasure& bclMeasure) {
  m_componentType = bclMeasure.measureType().enumName().c_str();
  m_name = bclMeasure.displayName().c_str();
  if (m_name.isEmpty()) {
    m_name = bclMeasure.name().c_str();
  }

  m_uid = bclMeasure.uid().c_str();
  m_versionId = bclMeasure.versionId().c_str();
  m_description = bclMeasure.description().c_str();
  m_description = m_description.trimmed();
  m_modelerDescription = bclMeasure.modelerDescription().c_str();
  m_modelerDescription = m_modelerDescription.trimmed();  // alternative: .simplified, removes consecutive whitespaces inside the string as well

  if (auto dt_ = bclMeasure.versionModified()) {
    // m_versionModified = dt_->toXsdDateTime().c_str();
    QDateTime qDT;
    qDT.setSecsSinceEpoch(static_cast<qint64>(dt_->toEpoch()));
    m_versionModified = qDT.toString("dddd MMMM d yyyy HH:mm:ss");
  }

  m_error = bclMeasure.error();
  m_attributes = bclMeasure.attributes();
  m_arguments = bclMeasure.arguments();
  m_fileReferences = bclMeasure.files();
  m_tags = bclMeasure.tags();

  boost::optional<VersionString> minCompatibleVersion;
  boost::optional<VersionString> maxCompatibleVersion;
  for (const BCLFileReference& fileReference : m_fileReferences) {
    if (fileReference.usageType() == "script" && fileReference.softwareProgram() == "OpenStudio") {
      minCompatibleVersion = fileReference.minCompatibleVersion();
      maxCompatibleVersion = fileReference.maxCompatibleVersion();

      if (!minCompatibleVersion) {
        try {
          minCompatibleVersion = VersionString(fileReference.softwareProgramVersion());
        } catch (const std::exception&) {
        }
      }
      break;
    }
  }

  VersionString currentVersion(openStudioVersion());
  m_available =
    !((minCompatibleVersion && (*minCompatibleVersion) > currentVersion) || (maxCompatibleVersion && (*maxCompatibleVersion) < currentVersion));
}

void Component::parseBCLSearchResult(const BCLSearchResult& bclSearchResult) {
  m_componentType = bclSearchResult.componentType().c_str();
  m_name = bclSearchResult.name().c_str();
  m_uid = bclSearchResult.uid().c_str();
  m_versionId = bclSearchResult.versionId().c_str();
  m_description = bclSearchResult.description().c_str();
  m_description = m_description.trimmed();
  m_modelerDescription = bclSearchResult.modelerDescription().c_str();
  m_modelerDescription = m_modelerDescription.trimmed();
  m_fidelityLevel = bclSearchResult.fidelityLevel().c_str();

  if (auto dt_ = bclSearchResult.versionModified()) {
    // m_versionModified = dt_->toXsdDateTime().c_str();
    QDateTime qDT;
    qDT.setSecsSinceEpoch(static_cast<qint64>(dt_->toEpoch()));
    m_versionModified = qDT.toString("dddd MMMM d yyyy HH:mm:ss");
  }
  m_org = bclSearchResult.org().c_str();
  m_repo = bclSearchResult.repo().c_str();
  m_releaseTag = bclSearchResult.releaseTag().c_str();

  // m_error
  m_attributes = bclSearchResult.attributes();
  // TODO: arguments
  m_files = bclSearchResult.files();
  m_provenances = bclSearchResult.provenances();
  m_tags = bclSearchResult.tags();

  boost::optional<VersionString> minCompatibleVersion;
  boost::optional<VersionString> maxCompatibleVersion;
  for (const BCLFile& file : m_files) {
    if (file.usageType() == "script" && file.softwareProgram() == "OpenStudio") {
      minCompatibleVersion = file.minCompatibleVersion();
      maxCompatibleVersion = file.maxCompatibleVersion();

      if (!minCompatibleVersion) {
        try {
          minCompatibleVersion = VersionString(file.identifier());
        } catch (const std::exception&) {
        }
      }
      break;
    }
  }

  VersionString currentVersion(openStudioVersion());
  m_available = true;
  if (minCompatibleVersion && (*minCompatibleVersion) > currentVersion) {
    m_available = false;
  }
  if (maxCompatibleVersion && (*maxCompatibleVersion) < currentVersion) {
    m_available = false;
  }
}

void Component::createAbridgedLayout() {
  m_checkBox = new QCheckBox(this);

  if (!m_showCheckBox) {
    m_checkBox->hide();
  }

  QLabel* label = nullptr;

  QString string;

  auto* leftLayout = new QVBoxLayout();

  string = "Name: ";
  string += m_name;
  label = new QLabel(string);
  leftLayout->addWidget(label);

  for (const Attribute& attribute : m_attributes) {
    string = attribute.name().c_str();
    if (m_componentType == "component") {
      if (string.toStdString() == OPENSTUDIO_TYPE) {
        openstudio::AttributeValueType type = attribute.valueType();
        if (type == AttributeValueType::String) {
          string = attribute.valueAsString().c_str();
          QString temp("Type: ");
          temp += string;
          label = new QLabel(temp);
          leftLayout->addWidget(label);
          break;
        }
      }
    } else if (m_componentType == "measure" || m_componentType == "MeasureType") {
      if (string.toStdString() == "Measure Type") {
        openstudio::AttributeValueType type = attribute.valueType();
        if (type == AttributeValueType::String) {
          string = attribute.valueAsString().c_str();
          QString temp("Measure Type: ");
          temp += string;
          label = new QLabel(temp);
          leftLayout->addWidget(label);
        }
      }
    }
  }

  m_msg = new QLabel(this);
  if (m_error) {
    m_msg->setStyleSheet("color:#F00;font-style:italic;");
  } else {
    m_msg->setStyleSheet("color:#888;font-style:italic;");
  }
  m_msg->setHidden(true);
  leftLayout->addWidget(m_msg);

  auto* mainLayout = new QHBoxLayout();
  mainLayout->addLayout(leftLayout);
  mainLayout->addStretch();
  mainLayout->addWidget(m_checkBox);
  setLayout(mainLayout);
}

void Component::createCompleteLayout() {

  auto* mainLayout = new QVBoxLayout();

  ///! Error
  if (m_error) {
    auto* label = new QLabel("Errors");
    label->setObjectName("H1");
    mainLayout->addWidget(label);

    label = new QLabel(m_error->c_str());
    mainLayout->addWidget(label);

    // TODO: why this empty label?
    label = new QLabel();
    mainLayout->addWidget(label);
  }

  // Description
  if (!m_description.isEmpty()) {
    {
      auto* label = new QLabel("Description");
      label->setObjectName("H1");
      mainLayout->addWidget(label);
    }
    {
      auto* content = new QLabel(m_description);
      content->setWordWrap(true);
      content->setOpenExternalLinks(true);
      mainLayout->addWidget(content);
    }
  }

  if (!m_modelerDescription.isEmpty()) {
    {
      auto* label = new QLabel("Modeler Description");
      label->setObjectName("H1");
      mainLayout->addWidget(label);
    }
    {
      auto* content = new QLabel(m_modelerDescription);
      content->setWordWrap(true);
      content->setOpenExternalLinks(true);
      mainLayout->addWidget(content);
    }
  }

  // Lambda Helpers for tables

  auto createAndRegisterTableWidgetWithTwoColums = [this, &mainLayout]() -> QTableWidget* {
    auto* tableWidget = new QTableWidget(0, 2, this);
    // really don't want the user to select or give focus to any table cells
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    tableWidget->setAlternatingRowColors(true);
    tableWidget->verticalHeader()->hide();
    tableWidget->horizontalHeader()->hide();
    tableWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tableWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    tableWidget->horizontalHeader()->setStretchLastSection(true);
    mainLayout->addWidget(tableWidget);
    return tableWidget;
  };

  auto addRowToTableWidget = [](QTableWidget* tableWidget, const QString& header, const QString& value) {
    tableWidget->insertRow(tableWidget->rowCount());

    auto* item = new QTableWidgetItem(header);
    tableWidget->setItem(tableWidget->rowCount() - 1, 0, item);

    item = new QTableWidgetItem(value);
    tableWidget->setItem(tableWidget->rowCount() - 1, 1, item);
  };

  auto makeTableShowCompletely = [](QTableWidget* tableWidget) {
    // make the table show completely
    const int rowHeight = tableWidget->rowHeight(0);
    const int rowCount = tableWidget->rowCount();
    const int tableHeight = rowHeight * rowCount;
    tableWidget->setFixedHeight(tableHeight);
  };

  //if (!m_attributes.empty()){
  {
    ///! Attributes
    ///! Class BCL only stores double (optional units),
    ///! int (optional units), and string, with their names.
    auto* label = new QLabel("Attributes");
    label->setObjectName("H1");
    mainLayout->addWidget(label);

    auto* tableWidget = createAndRegisterTableWidgetWithTwoColums();
    for (const Attribute& attribute : m_attributes) {

      boost::optional<std::string> optionalUnits = attribute.units();

      const openstudio::AttributeValueType type = attribute.valueType();

      QString display;
      if (type == AttributeValueType::Boolean) {
        display = attribute.valueAsBoolean() ? "true" : "false";
      } else if (type == AttributeValueType::Double) {
        display.setNum(attribute.valueAsDouble());
      } else if (type == AttributeValueType::Integer) {
        display.setNum(attribute.valueAsInteger());
      } else if (type == AttributeValueType::Unsigned) {
        display.setNum(attribute.valueAsUnsigned());
      } else if (type == AttributeValueType::String) {
        display = attribute.valueAsString().c_str();
      } else if (type == AttributeValueType::AttributeVector) {
        AttributeVector attributeVector = attribute.valueAsAttributeVector();
        // TODO handle this case
      } else {
        // should never get here
      }
      if (optionalUnits) {
        display += " ";
        const std::string temp = optionalUnits.get();
        display += temp.c_str();
      }

      addRowToTableWidget(tableWidget, attribute.name().c_str(), display);
    }

    makeTableShowCompletely(tableWidget);

    label = new QLabel();
    mainLayout->addWidget(label);
  }

  /////! Arguments
  if (m_componentType != "component") {
    //if (!m_arguments.empty()){

    auto* label = new QLabel("Arguments");
    label->setObjectName("H1");
    mainLayout->addWidget(label);

    auto* tableWidget = createAndRegisterTableWidgetWithTwoColums();

    for (const BCLMeasureArgument& argument : m_arguments) {

      std::string name = argument.displayName();
      if (name.empty()) {
        name = argument.name();
      }

      std::string type = argument.type();
      boost::optional<std::string> units = argument.units();
      if (units) {
        type += " (" + units.get() + ")";
      }

      addRowToTableWidget(tableWidget, name.c_str(), type.c_str());
    }

    makeTableShowCompletely(tableWidget);

    label = new QLabel();
    mainLayout->addWidget(label);
  }

  /////! Files
  if (!m_files.empty()) {
    auto* label = new QLabel("Files");
    label->setObjectName("H1");
    mainLayout->addWidget(label);
    for (const BCLFile& file : m_files) {
      label = new QLabel(file.filename().c_str());
      mainLayout->addWidget(label);

      //label = new QLabel(file.filetype().c_str());
      //mainLayout->addWidget(label);

      //label = new QLabel(file.softwareProgram().c_str());
      //mainLayout->addWidget(label);

      //label = new QLabel(file.identifier().c_str());
      //mainLayout->addWidget(label);
    }
    for (const BCLFileReference& fileReference : m_fileReferences) {
      label = new QLabel(fileReference.fileName().c_str());
      mainLayout->addWidget(label);

      //label = new QLabel(fileReference.fileType().c_str());
      //mainLayout->addWidget(label);

      //label = new QLabel(fileReference.softwareProgram().c_str());
      //mainLayout->addWidget(label);

      //label = new QLabel(fileReference.softwareProgramVersion().c_str());
      //mainLayout->addWidget(label);
    }

    label = new QLabel();
    mainLayout->addWidget(label);
  }

  ///! Provenances
  //if (!m_provenances.empty()){
  {
    auto* label = new QLabel("Sources");
    label->setObjectName("H1");
    mainLayout->addWidget(label);
    if (m_provenances.empty() && m_org.isEmpty() && m_repo.isEmpty()) {
      label = new QLabel();
      mainLayout->addWidget(label);

    } else {
      auto* tableWidget = createAndRegisterTableWidgetWithTwoColums();
      if (!m_org.isEmpty()) {
        addRowToTableWidget(tableWidget, "Organization", m_org);
      }
      if (!m_repo.isEmpty()) {
        addRowToTableWidget(tableWidget, "Repository", m_repo);
      }
      if (!m_releaseTag.isEmpty()) {
        addRowToTableWidget(tableWidget, "Release Tag", m_releaseTag);
      }

      for (const BCLProvenance& provenance : m_provenances) {
        if (!provenance.author().empty()) {
          addRowToTableWidget(tableWidget, "Author", provenance.author().c_str());
        }
        if (!provenance.comment().empty()) {
          addRowToTableWidget(tableWidget, "Comment", provenance.comment().c_str());
        }
        if (!provenance.datetime().empty()) {
          addRowToTableWidget(tableWidget, "Date & time", provenance.datetime().c_str());
        }
        // TODO: add a separator?
        // addRowToTableWidget(tableWidget, "", "");
      }
      makeTableShowCompletely(tableWidget);

      if (!m_org.isEmpty() && !m_repo.isEmpty()) {
        const QString repo_url = QString("https://github.com/%1/%2").arg(m_org).arg(m_repo);
        const QString link = QString("<a href=\"%1\">%1</a>").arg(repo_url);
        auto* content = new QLabel(link);
        content->setOpenExternalLinks(true);
        mainLayout->addWidget(content);

        if (!m_releaseTag.isEmpty()) {
          const QString direct_url = QString("%1/tree/%2/lib/measures").arg(repo_url).arg(m_releaseTag);
          const QString direct_link = QString("<a href=\"%1\">%1</a>").arg(direct_url);
          auto* contentDirect = new QLabel(direct_link);
          contentDirect->setOpenExternalLinks(true);
          mainLayout->addWidget(contentDirect);
        }
      }
    }
  }

  ///! Tags
  //if (!m_tags.empty())
  {
    auto* label = new QLabel("Tags");
    label->setObjectName("H1");
    mainLayout->addWidget(label);
    for (const std::string& tag : m_tags) {
      label = new QLabel(tag.c_str());
      mainLayout->addWidget(label);

      label = new QLabel();
      mainLayout->addWidget(label);
    }
  }

  {
    auto* label = new QLabel("Version");
    label->setObjectName("H1");
    mainLayout->addWidget(label);

    auto* tableWidget = createAndRegisterTableWidgetWithTwoColums();
    addRowToTableWidget(tableWidget, "UID", m_uid);
    addRowToTableWidget(tableWidget, "Version ID", m_versionId);
    if (!m_versionModified.isEmpty()) {
      addRowToTableWidget(tableWidget, "Version Modified", m_versionModified);
    }
    makeTableShowCompletely(tableWidget);
  }

  setLayout(mainLayout);
}

void Component::paintEvent(QPaintEvent* /*event*/) {
  QPainter p(this);

  if (isChecked()) {
    QLinearGradient linearGrad(QPointF(0, 0), QPointF(0, rect().height()));
    linearGrad.setColorAt(0, QColor(113, 153, 200, 255));
    linearGrad.setColorAt(0.10, QColor(113, 153, 200, 150));
    linearGrad.setColorAt(0.15, QColor(210, 222, 236, 150));
    linearGrad.setColorAt(1.00, QColor(210, 222, 236, 150));

    QBrush brush(linearGrad);
    p.setBrush(brush);
    p.setPen(Qt::NoPen);

    p.drawRect(0, 0, rect().width(), rect().height() - 1);
  }

  p.setPen(QPen(Qt::black));
  p.drawLine(0, rect().height() - 1, rect().width(), rect().height() - 1);
}

///! SLOTS

}  // namespace openstudio
