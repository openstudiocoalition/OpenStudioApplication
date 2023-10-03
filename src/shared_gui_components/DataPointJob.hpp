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

#ifndef SHAREDGUICOMPONENTS_DATAPOINTJOB_HPP
#define SHAREDGUICOMPONENTS_DATAPOINTJOB_HPP

#include "HeaderViews.hpp"
#include "OSDialog.hpp"

#include <boost/optional.hpp>

class QLabel;
class QString;

namespace openstudio {

class BCLMeasure;
class DateTime;
class WorkflowJSON;

class DataPointJobHeaderView : public OSHeader
{
  Q_OBJECT

 public:
  DataPointJobHeaderView();

  virtual ~DataPointJobHeaderView() = default;

  void setName(const std::string& name);

  void setLastRunTime(const boost::optional<openstudio::DateTime>& lastRunTime);

  void setStatus(const std::string& status, bool isCanceled);

  void setNA(bool na);

  void setNumWarnings(unsigned numWarnings);

  void setNumErrors(unsigned numErrors);

  QLabel* m_name;
  QLabel* m_lastRunTime;
  QLabel* m_status;
  QLabel* m_na;
  QLabel* m_warnings;
  QLabel* m_errors;
};

class DataPointJobContentView : public QWidget
{
  Q_OBJECT

 public:
  DataPointJobContentView();

  virtual ~DataPointJobContentView() = default;

  void clear();

  void addInitialConditionMessage(const std::string& message);

  void addFinalConditionMessage(const std::string& message);

  void addInfoMessage(const std::string& message);

  void addWarningMessage(const std::string& message);

  void addErrorMessage(const std::string& message);

  void addStdErrorMessage(const std::string& message);

 private:
  static QString formatMessageForHTML(const std::string& t_message);

  QLabel* m_textEdit;
};

class DataPointJobItemView : public OSCollapsibleView
{
  Q_OBJECT

 public:
  DataPointJobItemView();

  virtual ~DataPointJobItemView() = default;

  DataPointJobHeaderView* m_dataPointJobHeaderView;

  DataPointJobContentView* m_dataPointJobContentView;

  void update(const BCLMeasure& bclMeasure, const boost::optional<WorkflowJSON>& outWorkflowJSON, bool canceled);

 protected:
  void paintEvent(QPaintEvent* e) override;
};

}  // namespace openstudio

#endif  // SHAREDGUICOMPONENTS_DATAPOINTJOB_HPP
