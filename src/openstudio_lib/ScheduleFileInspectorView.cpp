/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "ScheduleFileInspectorView.hpp"

#include "../shared_gui_components/OSLineEdit.hpp"
#include "../shared_gui_components/OSIntegerEdit.hpp"
#include "../shared_gui_components/OSComboBox.hpp"
#include "../shared_gui_components/OSSwitch.hpp"
#include "../model_editor/Utilities.hpp"

#include <openstudio/model/ScheduleFile.hpp>
#include <openstudio/model/ScheduleFile_Impl.hpp>
#include <openstudio/model/ExternalFile.hpp>

#include <openstudio/utilities/core/Assert.hpp>
#include <openstudio/utilities/core/Filesystem.hpp>
#include <openstudio/utilities/core/Compare.hpp>

#include <QGridLayout>
#include <QLabel>
#include <QStackedWidget>
#include <QLineEdit>
#include <QFile>
#include <QTextStream>
#include <QPlainTextEdit>
#include <QPushButton>
#include <Qt>  // Qt namespace, for SplitBehaviorFlags

namespace openstudio {

// ScheduleFileInspectorView

ScheduleFileInspectorView::ScheduleFileInspectorView(const openstudio::model::Model& model, QWidget* parent)
  : ModelObjectInspectorView(model, true, parent) {
  createLayout();
}

void ScheduleFileInspectorView::createLayout() {
  auto* hiddenWidget = new QWidget();
  this->stackedWidget()->addWidget(hiddenWidget);

  auto* visibleWidget = new QWidget();
  this->stackedWidget()->addWidget(visibleWidget);

  auto* mainGridLayout = new QGridLayout();
  mainGridLayout->setContentsMargins(7, 7, 7, 7);
  mainGridLayout->setSpacing(14);
  visibleWidget->setLayout(mainGridLayout);

  int row = mainGridLayout->rowCount();

  QLabel* label = nullptr;

  // Name

  label = new QLabel("Name: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label, row, 0);

  ++row;

  m_nameEdit = new OSLineEdit2();
  mainGridLayout->addWidget(m_nameEdit, row, 0, 1, 3);

  // FilePath
  ++row;

  label = new QLabel("FilePath: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label, row, 0);

  ++row;

  m_filePath = new QLineEdit();
  m_filePath->setReadOnly(true);
  mainGridLayout->addWidget(m_filePath, row, 0, 1, 3);

  ++row;

  QVBoxLayout* vLayout = nullptr;

  // Column Number
  vLayout = new QVBoxLayout();

  label = new QLabel("Column Number: ");
  label->setObjectName("H2");
  vLayout->addWidget(label);

  m_columnNumber = new OSIntegerEdit2();
  vLayout->addWidget(m_columnNumber);

  mainGridLayout->addLayout(vLayout, row, 0);

  // Rows to Skip
  vLayout = new QVBoxLayout();

  label = new QLabel("Rows to Skip at Top: ");
  label->setObjectName("H2");
  vLayout->addWidget(label);

  m_rowstoSkipatTop = new OSIntegerEdit2();
  vLayout->addWidget(m_rowstoSkipatTop);

  mainGridLayout->addLayout(vLayout, row, 1);

  // Will be done in a lambda below, otherwise there's a race condition, the modelObject itself isn't modified when refreshContent is called
  // connect(m_rowstoSkipatTop, &OSIntegerEdit2::editingFinished, this, &ScheduleFileInspectorView::refreshContent);

  ++row;

  // Number of Hours of Data
  vLayout = new QVBoxLayout();

  label = new QLabel("Number of Hours of Data: ");
  label->setObjectName("H2");
  vLayout->addWidget(label);

  m_numberofHoursofData = new OSIntegerEdit2();
  vLayout->addWidget(m_numberofHoursofData);

  mainGridLayout->addLayout(vLayout, row, 0);

  // Column Separator
  vLayout = new QVBoxLayout();

  label = new QLabel("Column Separator: ");
  label->setObjectName("H2");
  vLayout->addWidget(label);

  m_columnSeparator = new OSComboBox2();
  m_columnSeparator->addItem("Comma");
  m_columnSeparator->addItem("Tab");
  m_columnSeparator->addItem("Space");
  m_columnSeparator->addItem("Semicolon");
  m_columnSeparator->setEnabled(true);
  vLayout->addWidget(m_columnSeparator);

  mainGridLayout->addLayout(vLayout, row, 1);

  ++row;

  // Interpolate
  vLayout = new QVBoxLayout();

  label = new QLabel("Interpolate to Timestep: ");
  label->setObjectName("H2");
  vLayout->addWidget(label);

  m_interpolatetoTimestep = new OSSwitch2();
  vLayout->addWidget(m_interpolatetoTimestep);

  mainGridLayout->addLayout(vLayout, row, 0);

  // Minutes per Item
  vLayout = new QVBoxLayout();

  label = new QLabel("Minutes per Item: ");
  label->setObjectName("H2");
  vLayout->addWidget(label);

  m_minutesperItem = new OSComboBox2();
  for (const auto& val : model::ScheduleFile::minutesperItemValues()) {
    m_minutesperItem->addItem(QString::fromStdString(val));
  }
  m_minutesperItem->setEnabled(true);
  vLayout->addWidget(m_minutesperItem);

  mainGridLayout->addLayout(vLayout, row, 1);

  ++row;

  // Adjust Schedule for Daylight Savings
  vLayout = new QVBoxLayout();

  label = new QLabel("Adjust Schedule for Daylight Savings: ");
  label->setObjectName("H2");
  vLayout->addWidget(label);

  m_adjustScheduleforDaylightSavings = new OSSwitch2();
  vLayout->addWidget(m_adjustScheduleforDaylightSavings);

  mainGridLayout->addLayout(vLayout, row, 0);

  // Translate File With Relative Path
  vLayout = new QVBoxLayout();

  label = new QLabel("Translate File With Relative Path: ");
  label->setObjectName("H2");
  vLayout->addWidget(label);

  m_translateFileWithRelativePath = new OSSwitch2();
  vLayout->addWidget(m_translateFileWithRelativePath);

  mainGridLayout->addLayout(vLayout, row, 1);

  ++row;

  auto* line = new QFrame();
  line->setFrameShape(QFrame::HLine);
  line->setFrameShadow(QFrame::Sunken);
  mainGridLayout->addWidget(line, row, 0, 1, -1);
  row++;

  label = new QLabel("Content: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label, row, 0);

  ++row;

  label = new QLabel("Number of Lines in file: ");
  label->setObjectName("H3");
  mainGridLayout->addWidget(label, row, 0);

  m_numLines = new QLineEdit();
  m_numLines->setReadOnly(true);
  mainGridLayout->addWidget(m_numLines, row, 1, 1, 1);

  m_error = new QLabel();
  m_error->setStyleSheet("QLabel { color : red; font: bold }");
  m_error->hide();
  mainGridLayout->addWidget(m_error, row, 2, 1, 1);

  ++row;

  label = new QLabel("Display All File Content: ");
  label->setObjectName("H3");
  mainGridLayout->addWidget(label, row, 0);

  // Mimic an OSSwitch2
  m_displayAllContentSwitch = new QPushButton();
  m_displayAllContentSwitch->setAcceptDrops(false);
  m_displayAllContentSwitch->setFlat(true);
  m_displayAllContentSwitch->setFixedSize(63, 21);
  m_displayAllContentSwitch->setObjectName("OnOffSliderButton");
  m_displayAllContentSwitch->setCheckable(true);
  mainGridLayout->addWidget(m_displayAllContentSwitch, row, 1, 1, 1);

  connect(m_displayAllContentSwitch, &QPushButton::toggled, [this](bool) {
    m_displayAllContent = !m_displayAllContent;
    refreshContent();
  });

  ++row;

  m_contentLines = new QPlainTextEdit();
  m_contentLines->setReadOnly(true);
  const QFont f = QFontDatabase::systemFont(QFontDatabase::FixedFont);
  m_contentLines->setFont(f);

  mainGridLayout->addWidget(m_contentLines, row, 0, 1, 5);

  ++row;

  // Stretch

  mainGridLayout->setRowStretch(100, 100);

  mainGridLayout->setColumnStretch(4, 3);
  mainGridLayout->setColumnStretch(5, 1);
}

void ScheduleFileInspectorView::onClearSelection() {
  ModelObjectInspectorView::onClearSelection();  // call parent implementation
  detach();
}

void ScheduleFileInspectorView::onSelectModelObject(const openstudio::model::ModelObject& modelObject) {
  detach();
  auto sch = modelObject.cast<model::ScheduleFile>();
  attach(sch);
  refresh();
}

void ScheduleFileInspectorView::onUpdate() {
  refresh();
}

void ScheduleFileInspectorView::attach(openstudio::model::ScheduleFile& sch) {
  m_sch = sch;

  // m_nameEdit->bind(sch,"name");
  m_nameEdit->bind(
    *m_sch, OptionalStringGetter(std::bind(&model::ScheduleFile::name, m_sch.get_ptr(), true)),
    boost::optional<StringSetterOptionalStringReturn>(std::bind(&model::ScheduleFile::setName, m_sch.get_ptr(), std::placeholders::_1)));

  // OSIntegerEdit2
  m_columnNumber->bind(*m_sch, IntGetter(std::bind(&model::ScheduleFile::columnNumber, m_sch.get_ptr())),
                       boost::optional<IntSetter>([this](int value) -> bool {
                         bool result = m_sch->setColumnNumber(value);
                         if (result) {
                           refreshContent();
                           refreshError();
                         }
                         return result;
                       }));

  m_rowstoSkipatTop->bind(*m_sch, IntGetter(std::bind(&model::ScheduleFile::rowstoSkipatTop, m_sch.get_ptr())),
                          boost::optional<IntSetter>([this](int value) -> bool {
                            bool result = m_sch->setRowstoSkipatTop(value);
                            if (result) {
                              refreshContent();
                              refreshError();
                            }
                            return result;
                          })
                          // boost::optional<IntSetter>(std::bind(&model::ScheduleFile::setRowstoSkipatTop, m_sch.get_ptr(), std::placeholders::_1))
  );

  m_numberofHoursofData->bind(*m_sch, OptionalIntGetter(std::bind(&model::ScheduleFile::numberofHoursofData, m_sch.get_ptr())),
                              boost::optional<IntSetter>([this](int value) -> bool {
                                bool result = m_sch->setNumberofHoursofData(value);
                                if (result) {
                                  refreshError();
                                }
                                return result;
                              }));

  // OSComboBox2

  m_columnSeparator->bind<std::string>(
    *m_sch, static_cast<std::string (*)(const std::string&)>(&openstudio::toString),
    // ScheduleFile::columnSeparatorValues does not exist: https://github.com/NREL/OpenStudio/issues/5246
    []() {
      return std::vector<std::string>{"Comma", "Tab", "Space", "Semicolon"};
    },
    std::bind(&model::ScheduleFile::columnSeparator, m_sch.get_ptr()),
    [this](const std::string& value) -> bool {
      bool result = m_sch->setColumnSeparator(value);
      if (result) {
        refreshContent();
      }
      return result;
    },
    boost::none, boost::none);

  m_minutesperItem->bind<std::string>(
    *m_sch,
    static_cast<std::string (*)(const std::string&)>(&openstudio::toString),  // toString
    &model::ScheduleFile::minutesperItemValues,                               // choices
    // Getter
    // This returns an optional std::string when it really has a default...
    // OptionalStringGetter(std::bind(&model::ScheduleFile::minutesperItem, m_sch.get_ptr())),
    [sch_ptr = m_sch.get_ptr()]() -> std::string { return sch_ptr->minutesperItem().get(); },
    // Setter, another weirdness, but for backward compat it takes a std::string or an int... and the std::string one is deprecated
    // std::bind(&model::ScheduleFile::setMinutesperItem, m_sch.get_ptr(), std::placeholders::_1),
    [this](const std::string& minutesperItem) {
      bool result = m_sch->setMinutesperItem(std::stoi(minutesperItem));
      if (result) {
        refreshError();
      }
      return result;
    },
    boost::optional<NoFailAction>(std::bind(&model::ScheduleFile::resetMinutesperItem, m_sch.get_ptr())),     // reset
    boost::optional<BasicQuery>(std::bind(&model::ScheduleFile::isMinutesperItemDefaulted, m_sch.get_ptr()))  // isDefaulted

  );

  // OSSwitch2

  m_interpolatetoTimestep->bind(
    *m_sch, std::bind(&model::ScheduleFile::interpolatetoTimestep, m_sch.get_ptr()),
    boost::optional<BoolSetter>(std::bind(&model::ScheduleFile::setInterpolatetoTimestep, m_sch.get_ptr(), std::placeholders::_1)),
    boost::optional<NoFailAction>(std::bind(&model::ScheduleFile::resetInterpolatetoTimestep, m_sch.get_ptr())),
    boost::optional<BasicQuery>(std::bind(&model::ScheduleFile::isInterpolatetoTimestepDefaulted, m_sch.get_ptr())));

  m_adjustScheduleforDaylightSavings->bind(
    *m_sch, std::bind(&model::ScheduleFile::adjustScheduleforDaylightSavings, m_sch.get_ptr()),
    boost::optional<BoolSetter>(std::bind(&model::ScheduleFile::setAdjustScheduleforDaylightSavings, m_sch.get_ptr(), std::placeholders::_1)),
    boost::optional<NoFailAction>(std::bind(&model::ScheduleFile::resetAdjustScheduleforDaylightSavings, m_sch.get_ptr())),
    boost::optional<BasicQuery>(std::bind(&model::ScheduleFile::isAdjustScheduleforDaylightSavingsDefaulted, m_sch.get_ptr())));

  m_translateFileWithRelativePath->bind(
    *m_sch, std::bind(&model::ScheduleFile::translateFileWithRelativePath, m_sch.get_ptr()),
    boost::optional<BoolSetter>(std::bind(&model::ScheduleFile::setTranslateFileWithRelativePath, m_sch.get_ptr(), std::placeholders::_1)),
    boost::optional<NoFailAction>(std::bind(&model::ScheduleFile::resetTranslateFileWithRelativePath, m_sch.get_ptr())),
    boost::optional<BasicQuery>(std::bind(&model::ScheduleFile::isTranslateFileWithRelativePathDefaulted, m_sch.get_ptr())));

  refreshContent();

  this->stackedWidget()->setCurrentIndex(1);
}

void ScheduleFileInspectorView::detach() {
  this->stackedWidget()->setCurrentIndex(0);

  m_nameEdit->unbind();
  m_columnNumber->unbind();
  m_rowstoSkipatTop->unbind();
  m_numberofHoursofData->unbind();
  m_columnSeparator->unbind();
  m_interpolatetoTimestep->unbind();
  m_minutesperItem->unbind();
  m_adjustScheduleforDaylightSavings->unbind();
  m_translateFileWithRelativePath->unbind();

  m_sch = boost::none;
}

void ScheduleFileInspectorView::refresh() {}

void ScheduleFileInspectorView::refreshError() {
  const int minutesperItem = std::stoi(m_sch->minutesperItem().get());
  const int numHours = m_sch->numberofHoursofData().value_or(8760);
  const int expectedRecords = numHours * 60 / minutesperItem;

  const int rowstoSkipatTop = m_sch->rowstoSkipatTop();
  const int totalLines = m_lines.size();
  const int numRecords = totalLines - rowstoSkipatTop;
  if (numRecords != expectedRecords) {
    m_error->setText(QString("Expected %1 records but only %2 are selected").arg(expectedRecords).arg(numRecords));
    m_error->show();
  } else {
    m_error->hide();
  }
}

void ScheduleFileInspectorView::refreshContent() {
  // QLineEdit
  m_filePath->setText(toQString(m_sch->externalFile().fileName()));

  openstudio::path fpath = m_sch->externalFile().filePath();
  m_contentLines->clear();

  boost::system::error_code ec;
  if (openstudio::filesystem::is_regular_file(fpath, ec)) {
    const int rowstoSkipatTop = m_sch->rowstoSkipatTop();

    const int colNum = m_sch->columnNumber() - 1;  // Turn 1-indexed to 0-indexed

    QString sep;
    Qt::SplitBehavior behavior = Qt::KeepEmptyParts;
    if (openstudio::istringEqual(m_sch->columnSeparator(), "Comma")) {
      sep = ',';
    } else if (openstudio::istringEqual(m_sch->columnSeparator(), "Tab")) {
      sep = '\t';
    } else if (openstudio::istringEqual(m_sch->columnSeparator(), "Space")) {
      sep = ' ';  // QRegularExpression("\\s+")
      behavior = Qt::SkipEmptyParts;
    } else if (openstudio::istringEqual(m_sch->columnSeparator(), "Semicolon")) {
      sep = ';';
    }  // Fixed: not handled

    if (m_lines.isEmpty()) {
      m_lines.clear();

      QFile inputFile(toQString(fpath));

      if (inputFile.open(QIODevice::ReadOnly)) {
        int curLine = 0;

        QTextStream in(&inputFile);
        while (!in.atEnd()) {  //  && curLine < read_n_lines) {
          m_lines.append(in.readLine());
          ++curLine;
        }
        inputFile.close();
      }
      m_numLines->setText(QString::number(m_lines.size()));
    }

    const int read_n_lines = std::min(std::max(rowstoSkipatTop + 2, 10), static_cast<int>(m_lines.size()));

    auto constructStyledLine = [&sep, &behavior, &colNum](const QString& line, const QString& color) {
      QStringList vals = line.split(sep, behavior);
      QString str;
      for (int j = 0; j < vals.size(); ++j) {
        QString val = vals[j];
        if (j == colNum) {
          str.append(QString("<span style='color: %1'>%2</span>").arg(color).arg(val));
        } else {
          str.append(QString("<span style='color: gray'>%1</span>").arg(val));
        }
        if (j < vals.size() - 1) {
          str.append(",");
        }
      }
      return str;
    };

    int curLine = 0;
    for (const QString& line : m_lines) {
      if (curLine < rowstoSkipatTop) {
        m_contentLines->appendHtml(constructStyledLine(line, "black"));
      } else if (m_displayAllContent || (curLine < read_n_lines) || (curLine > m_lines.size() - 5)) {
        m_contentLines->appendHtml(constructStyledLine(line, "green"));
      } else if (curLine == read_n_lines) {
        m_contentLines->appendHtml("<strong>[...]</strong>");
      }
      ++curLine;
    }
  } else {
    m_contentLines->setPlainText(QString("File not found at '%1'").arg(toQString(fpath)));
  }
}

}  // namespace openstudio
