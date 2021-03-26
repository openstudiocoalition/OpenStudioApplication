/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) 2020-2020, OpenStudio Coalition and other contributors. All rights reserved.
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

#include "OSLoadNamePixmapLineEdit.hpp"

#include "OSLineEdit.hpp"

#include "../openstudio_lib/IconLibrary.hpp"
#include "../openstudio_lib/OSItem.hpp"

#include <openstudio/model/ModelObject.hpp>
#include <openstudio/model/ModelObject_Impl.hpp>

#include <QBoxLayout>
#include <QLabel>
#include <QPixmap>

#define MINI_ICON_SIZE 24

namespace openstudio {

OSLoadNamePixmapLineEdit::OSLoadNamePixmapLineEdit(QWidget* parent) : QWidget(parent) {
  createWidgets();
}

OSLoadNamePixmapLineEdit::~OSLoadNamePixmapLineEdit() {}

void OSLoadNamePixmapLineEdit::enableClickFocus() {
  m_lineEdit->enableClickFocus();
}

DeleteType OSLoadNamePixmapLineEdit::deleteType() {
  return m_lineEdit->deleteType();
}

void OSLoadNamePixmapLineEdit::setDeleteType(DeleteType deleteType) {
  m_lineEdit->setDeleteType(deleteType);
}

bool OSLoadNamePixmapLineEdit::hasData() {
  return m_lineEdit->hasData();
}

void OSLoadNamePixmapLineEdit::setLocked(bool locked) {
  m_lineEdit->setLocked(locked);
}

boost::optional<model::ModelObject> OSLoadNamePixmapLineEdit::modelObject() const {
  return m_lineEdit->modelObject();
}

void OSLoadNamePixmapLineEdit::bind(const model::ModelObject& modelObject, StringGetter get, boost::optional<StringSetter> set,
                                    boost::optional<NoFailAction> reset, boost::optional<BasicQuery> isDefaulted,
                                    boost::optional<BasicQuery> isLocked) {
  m_lineEdit->bind(modelObject, get, set, reset, isDefaulted, isLocked);

  completeBind();
}

void OSLoadNamePixmapLineEdit::bind(const model::ModelObject& modelObject, OptionalStringGetter get, boost::optional<StringSetter> set,
                                    boost::optional<NoFailAction> reset, boost::optional<BasicQuery> isDefaulted,
                                    boost::optional<BasicQuery> isLocked) {
  m_lineEdit->bind(modelObject, get, set, reset, isDefaulted, isLocked);

  completeBind();
}

void OSLoadNamePixmapLineEdit::bind(const model::ModelObject& modelObject, OptionalStringGetter get,
                                    boost::optional<StringSetterOptionalStringReturn> set, boost::optional<NoFailAction> reset,
                                    boost::optional<BasicQuery> isDefaulted, boost::optional<BasicQuery> isLocked) {
  m_lineEdit->bind(modelObject, get, set, reset, isDefaulted, isLocked);

  completeBind();
}

void OSLoadNamePixmapLineEdit::bind(const model::ModelObject& modelObject, OptionalStringGetterBoolArg get,
                                    boost::optional<StringSetterOptionalStringReturn> set, boost::optional<NoFailAction> reset,
                                    boost::optional<BasicQuery> isDefaulted, boost::optional<BasicQuery> isLocked) {
  m_lineEdit->bind(modelObject, get, set, reset, isDefaulted, isLocked);

  completeBind();
}

void OSLoadNamePixmapLineEdit::bind(const model::ModelObject& modelObject, StringGetter get, boost::optional<StringSetterVoidReturn> set,
                                    boost::optional<NoFailAction> reset, boost::optional<BasicQuery> isDefaulted,
                                    boost::optional<BasicQuery> isLocked) {
  m_lineEdit->bind(modelObject, get, set, reset, isDefaulted, isLocked);

  completeBind();
}

void OSLoadNamePixmapLineEdit::onItemRemoveClicked() {
  // TODO: remove icon?
}

void OSLoadNamePixmapLineEdit::completeBind() {
  setIcon();
}

void OSLoadNamePixmapLineEdit::unbind() {
  m_lineEdit->unbind();
}

QWidget* OSLoadNamePixmapLineEdit::qwidget() {
  return this;
}

void OSLoadNamePixmapLineEdit::createWidgets() {
  QPixmap m_pixmap(MINI_ICON_SIZE, MINI_ICON_SIZE);

  m_label = new QLabel();
  m_label->setFixedSize(MINI_ICON_SIZE, MINI_ICON_SIZE);
  m_label->setPixmap(m_pixmap);

  m_lineEdit = new OSLineEdit2();

  connect(m_lineEdit, &OSLineEdit2::itemClicked, this, &OSLoadNamePixmapLineEdit::itemClicked);
  connect(m_lineEdit, &OSLineEdit2::objectRemoved, this, &OSLoadNamePixmapLineEdit::objectRemoved);

  auto layout = new QHBoxLayout();
  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(m_label);
  layout->addWidget(m_lineEdit);
  this->setLayout(layout);
}

void OSLoadNamePixmapLineEdit::setIcon() {
  auto mo = modelObject();
  if (mo) {
    auto pixmap = IconLibrary::Instance().findMiniIcon(mo->iddObjectType().value());
    if (pixmap) {
      m_label->setPixmap(*pixmap);
    }
  }
}

}  // namespace openstudio
