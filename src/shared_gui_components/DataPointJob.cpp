/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) 2020-2023, OpenStudio Coalition and other contributors. All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
*  following conditions are met:
*
*  (1) Redistributions of source code must retain the above copyright notice, this list of conditions and the following
*  disclaimer.
*
*  (2) Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following
*  disclaimer in the documentation and/or other materials provided with the distribution.
*
*  (3) Neither the name of the copyright holder nor the names of any contributors may be used to endorse or promote products
*  derived from this software without specific prior written permission from the respective party.
*
*  (4) Other than as required in clauses (1) and (2), distributions in any form of modifications or other derivative works
*  may not use the "OpenStudio" trademark, "OS", "os", or any other confusingly similar designation without specific prior
*  written permission from Alliance for Sustainable Energy, LLC.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER(S) AND ANY CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
*  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
*  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER(S), ANY CONTRIBUTORS, THE UNITED STATES GOVERNMENT, OR THE UNITED
*  STATES DEPARTMENT OF ENERGY, NOR ANY OF THEIR EMPLOYEES, BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
*  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
*  USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
*  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
*  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***********************************************************************************************************************/

#include "DataPointJob.hpp"

#include "../model_editor/Utilities.hpp"

#include <openstudio/utilities/core/Assert.hpp>
#include <openstudio/utilities/filetypes/WorkflowJSON.hpp>
#include <openstudio/utilities/filetypes/WorkflowStep.hpp>
#include <openstudio/utilities/filetypes/WorkflowStepResult.hpp>

#include <openstudio/utilities/time/DateTime.hpp>

#include <QBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QString>
#include <QStyleOption>

namespace openstudio {

DataPointJobHeaderView::DataPointJobHeaderView()
  : OSHeader(new HeaderToggleButton()),
    m_name(nullptr),
    m_lastRunTime(nullptr),
    m_status(nullptr),
    m_na(nullptr),
    m_warnings(nullptr),
    m_errors(nullptr) {
  auto* mainHLayout = new QHBoxLayout();
  mainHLayout->setContentsMargins(15, 5, 5, 5);
  mainHLayout->setSpacing(5);
  mainHLayout->setAlignment(Qt::AlignLeft);
  setLayout(mainHLayout);

  mainHLayout->addWidget(toggleButton);

  m_name = new QLabel();

  mainHLayout->addWidget(m_name);
  mainHLayout->addStretch();

  m_lastRunTime = new QLabel();
  m_lastRunTime->setFixedWidth(150);
  mainHLayout->addWidget(m_lastRunTime);

  m_status = new QLabel();
  m_status->setFixedWidth(75);
  mainHLayout->addWidget(m_status);

  m_na = new QLabel();
  m_na->setFixedWidth(50);
  mainHLayout->addWidget(m_na);

  m_warnings = new QLabel();
  m_warnings->setFixedWidth(75);
  mainHLayout->addWidget(m_warnings);

  m_errors = new QLabel();
  m_errors->setFixedWidth(75);
  mainHLayout->addWidget(m_errors);
}

void DataPointJobHeaderView::setName(const std::string& name) {
  m_name->setText(toQString(name));
}

void DataPointJobHeaderView::setLastRunTime(const boost::optional<openstudio::DateTime>& lastRunTime) {
  if (lastRunTime) {
    std::string s = lastRunTime->toString();
    m_lastRunTime->setText(toQString(s));
  } else {
    m_lastRunTime->setText("Not Started");
  }
}

void DataPointJobHeaderView::setStatus(const std::string& status, bool isCanceled) {
  if (!isCanceled) {
    m_status->setText(toQString(status));
  } else {
    m_status->setText("Canceled");
  }
}

void DataPointJobHeaderView::setNA(bool na) {
  QString text;
  QString naStyle;
  if (na) {
    text = "   NA";
    naStyle = "QLabel { color : #C47B06; }";
  }
  m_na->setText(text);
  m_na->setStyleSheet(naStyle);
}

void DataPointJobHeaderView::setNumWarnings(unsigned numWarnings) {
  QString warningsStyle;
  if (numWarnings > 0) {
    warningsStyle = "QLabel { color : #C47B06; }";
  }
  m_warnings->setText(QString::number(numWarnings) + QString(numWarnings == 1 ? " Warning" : " Warnings"));
  m_warnings->setStyleSheet(warningsStyle);
}

void DataPointJobHeaderView::setNumErrors(unsigned numErrors) {
  QString errorsStyle;
  if (numErrors > 0) {
    errorsStyle = "QLabel { color : red; }";
  }
  m_errors->setText(QString::number(numErrors) + QString(numErrors == 1 ? " Error" : " Errors"));
  m_errors->setStyleSheet(errorsStyle);
}

DataPointJobContentView::DataPointJobContentView() : m_textEdit(new QLabel()) {
  auto* mainHLayout = new QHBoxLayout();
  mainHLayout->setContentsMargins(15, 5, 5, 5);
  mainHLayout->setSpacing(0);
  mainHLayout->setAlignment(Qt::AlignLeft);
  setLayout(mainHLayout);

  m_textEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  m_textEdit->setWordWrap(true);
  m_textEdit->setOpenExternalLinks(true);

  mainHLayout->addWidget(m_textEdit);
}

void DataPointJobContentView::clear() {
  m_textEdit->setText("");
}

QString DataPointJobContentView::formatMessageForHTML(const std::string& t_message) {
  QString str = QString::fromStdString(t_message);
  str.replace("\n", "<br>");
  return str;
}

void DataPointJobContentView::addInitialConditionMessage(const std::string& message) {
  QString html = m_textEdit->text();
  html += QString("<b style=\"color:blue\">Initial Condition</b>: ") + formatMessageForHTML(message) + QString("<br></br>");
  m_textEdit->setText(html);
}

void DataPointJobContentView::addFinalConditionMessage(const std::string& message) {
  QString html = m_textEdit->text();
  html += QString("<b style=\"color:blue\">Final Condition</b>: ") + formatMessageForHTML(message) + QString("<br></br>");
  m_textEdit->setText(html);
}

void DataPointJobContentView::addInfoMessage(const std::string& message) {
  QString html = m_textEdit->text();
  html += QString("<b style=\"color:green\">Info</b>: ") + formatMessageForHTML(message) + QString("<br></br>");
  m_textEdit->setText(html);
}

void DataPointJobContentView::addWarningMessage(const std::string& message) {
  QString html = m_textEdit->text();
  html += QString("<b style=\"color:#C47B06\">Warning</b>: ") + formatMessageForHTML(message) + QString("<br></br>");
  m_textEdit->setText(html);
}

void DataPointJobContentView::addErrorMessage(const std::string& message) {
  QString html = m_textEdit->text();
  html += QString("<b style=\"color:red\">Error</b>: ") + formatMessageForHTML(message) + QString("<br></br>");
  m_textEdit->setText(html);
}

void DataPointJobContentView::addStdErrorMessage(const std::string& message) {
  QString html = m_textEdit->text();
  html += formatMessageForHTML(message) + QString("<br></br>");
  m_textEdit->setText(html);
}

DataPointJobItemView::DataPointJobItemView()
  : OSCollapsibleView(true), m_dataPointJobHeaderView(new DataPointJobHeaderView()), m_dataPointJobContentView(new DataPointJobContentView()) {
  setStyleSheet("openstudio--pat--DataPointJobItemView { background: #C3C3C3; margin-left:10px; }");

  setHeader(m_dataPointJobHeaderView);

  setContent(m_dataPointJobContentView);
}

void DataPointJobItemView::paintEvent(QPaintEvent* /*e*/) {
  QStyleOption opt;
  opt.initFrom(this);
  QPainter p(this);
  style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void DataPointJobItemView::update(const BCLMeasure& bclMeasure, const boost::optional<WorkflowJSON>& outWorkflowJSON, bool canceled) {
  OS_ASSERT(m_dataPointJobHeaderView);

  m_dataPointJobHeaderView->setName(bclMeasure.className());

  m_dataPointJobHeaderView->m_na->setText("");
  m_dataPointJobHeaderView->m_warnings->setText("");
  m_dataPointJobHeaderView->m_errors->setText("");

  OS_ASSERT(m_dataPointJobContentView);
  m_dataPointJobContentView->clear();

  if (!outWorkflowJSON) {
    // unknown error
    return;
  }

  if (!outWorkflowJSON->completedStatus() || outWorkflowJSON->completedStatus().get() != "Success") {
    // error
  }

  boost::optional<DateTime> completedAt = outWorkflowJSON->completedAt();
  if (completedAt) {
    m_dataPointJobHeaderView->setLastRunTime(*completedAt);
  }

  boost::optional<std::string> completedStatus = outWorkflowJSON->completedStatus();
  if (completedStatus) {
    m_dataPointJobHeaderView->setStatus(*completedStatus, canceled);
  } else {
    m_dataPointJobHeaderView->setStatus("Unknown", canceled);
  }

  for (const auto& step : outWorkflowJSON->workflowSteps()) {

    boost::optional<WorkflowStepResult> result = step.result();
    if (!result) {
      continue;
    }

    boost::optional<std::string> initialCondition = result->stepInitialCondition();
    if (initialCondition) {
      m_dataPointJobContentView->addInitialConditionMessage(*initialCondition);
    }

    boost::optional<std::string> finalCondition = result->stepFinalCondition();
    if (finalCondition) {
      m_dataPointJobContentView->addFinalConditionMessage(*finalCondition);
    }

    const std::vector<std::string> errors = result->stepErrors();
    m_dataPointJobHeaderView->setNumErrors(errors.size());
    for (const std::string& errorMessage : errors) {
      m_dataPointJobContentView->addErrorMessage(errorMessage);
    }

    const std::vector<std::string> warnings = result->stepWarnings();
    m_dataPointJobHeaderView->setNumWarnings(warnings.size());
    for (const std::string& warningMessage : warnings) {
      m_dataPointJobContentView->addWarningMessage(warningMessage);
    }

    const std::vector<std::string> infos = result->stepInfo();
    // m_dataPointJobHeaderView->setNumInfos(infos.size());
    for (const std::string& info : infos) {
      m_dataPointJobContentView->addInfoMessage(info);
    }

    // there should only be on step so this is ok
    boost::optional<StepResult> stepResult = result->stepResult();
    if (stepResult && stepResult->value() == StepResult::NA) {
      m_dataPointJobHeaderView->setNA(true);
    } else {
      m_dataPointJobHeaderView->setNA(false);
    }
  }
}

}  // namespace openstudio
