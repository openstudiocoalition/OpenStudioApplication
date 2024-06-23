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

#include "OSCategoryPlaceholder.hpp"

#include <openstudio/utilities/core/Assert.hpp>

#include <QBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QStyleOption>

#include <iostream>

namespace openstudio {

OSCategoryPlaceholder::OSCategoryPlaceholder(const std::string& text, QWidget* parent) : QWidget(parent) {
  setFixedHeight(40);
  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  setObjectName("OSCategoryPlaceholder");

  this->setProperty("style", "0");
  this->setStyleSheet("QWidget#OSCategoryPlaceholder[style=\"0\"] { background-color: #95B3DE; border-bottom: 1px solid black; }");

  auto* mainHLayout = new QHBoxLayout();
  mainHLayout->setContentsMargins(9, 0, 9, 0);
  setLayout(mainHLayout);

  // Label

  m_textLabel = new QLabel(QString::fromStdString(text));
  m_textLabel->setWordWrap(true);
  m_textLabel->setObjectName("OSCategoryPlaceholderText");
  m_textLabel->setStyleSheet("QLabel#OSCategoryPlaceholderText { font-size: 14px; color: white; }");
  mainHLayout->addWidget(m_textLabel, 10);

  mainHLayout->addStretch();
}

QString OSCategoryPlaceholder::text() const {
  return m_textLabel->text();
}

void OSCategoryPlaceholder::paintEvent(QPaintEvent* event) {
  QStyleOption opt;
  opt.initFrom(this);
  QPainter p(this);
  style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

}  // namespace openstudio
