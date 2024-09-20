/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
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

OSLoadNamePixmapLineEdit::~OSLoadNamePixmapLineEdit() = default;

void OSLoadNamePixmapLineEdit::enableClickFocus() {
  m_lineEdit->enableClickFocus();
}

bool OSLoadNamePixmapLineEdit::hasData() {
  return m_lineEdit->hasData();
}

bool OSLoadNamePixmapLineEdit::locked() const {
  return m_lineEdit->locked();
}

void OSLoadNamePixmapLineEdit::setLocked(bool locked) {
  m_lineEdit->setLocked(locked);
}

boost::optional<model::ModelObject> OSLoadNamePixmapLineEdit::modelObject() const {
  return m_lineEdit->modelObject();
}

void OSLoadNamePixmapLineEdit::bind(const model::ModelObject& modelObject, StringGetter get, boost::optional<StringSetter> set,
                                    boost::optional<NoFailAction> reset, boost::optional<BasicQuery> isDefaulted) {
  m_lineEdit->bind(modelObject, get, set, reset, isDefaulted);

  completeBind();
}

void OSLoadNamePixmapLineEdit::bind(const model::ModelObject& modelObject, OptionalStringGetter get, boost::optional<StringSetter> set,
                                    boost::optional<NoFailAction> reset, boost::optional<BasicQuery> isDefaulted) {
  m_lineEdit->bind(modelObject, get, set, reset, isDefaulted);

  completeBind();
}

void OSLoadNamePixmapLineEdit::bind(const model::ModelObject& modelObject, OptionalStringGetter get,
                                    boost::optional<StringSetterOptionalStringReturn> set, boost::optional<NoFailAction> reset,
                                    boost::optional<BasicQuery> isDefaulted) {
  m_lineEdit->bind(modelObject, get, set, reset, isDefaulted);

  completeBind();
}

void OSLoadNamePixmapLineEdit::bind(const model::ModelObject& modelObject, OptionalStringGetterBoolArg get,
                                    boost::optional<StringSetterOptionalStringReturn> set, boost::optional<NoFailAction> reset,
                                    boost::optional<BasicQuery> isDefaulted) {
  m_lineEdit->bind(modelObject, get, set, reset, isDefaulted);

  completeBind();
}

void OSLoadNamePixmapLineEdit::bind(const model::ModelObject& modelObject, StringGetter get, boost::optional<StringSetterVoidReturn> set,
                                    boost::optional<NoFailAction> reset, boost::optional<BasicQuery> isDefaulted) {
  m_lineEdit->bind(modelObject, get, set, reset, isDefaulted);

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

  auto* layout = new QHBoxLayout();
  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(m_label);
  layout->addWidget(m_lineEdit);
  this->setLayout(layout);
}

void OSLoadNamePixmapLineEdit::setIcon() {
  auto mo = modelObject();
  if (mo) {
    const auto* pixmap = IconLibrary::Instance().findMiniIcon(mo->iddObjectType().value());
    if (pixmap) {
      m_label->setPixmap(*pixmap);
    }
  }
}

}  // namespace openstudio
