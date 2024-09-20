/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "EditView.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QStyleOption>
#include <QStyleOptionButton>
#include <QPainter>
#include <QTextEdit>
#include <QDoubleValidator>
#include <QIntValidator>
#include <QScrollArea>
#include <QCheckBox>
#include <QWheelEvent>
#include <QApplication>

#include <openstudio/utilities/core/Assert.hpp>

namespace openstudio {

EditRubyMeasureView::EditRubyMeasureView(bool applyMeasureNow) : QWidget() {
  auto* layout = new QVBoxLayout();
  layout->setContentsMargins(0, 0, 0, 0);
  setLayout(layout);
  auto* scrollArea = new QScrollArea();
  layout->addWidget(scrollArea);
  scrollArea->setWidgetResizable(true);

  auto* scrollWidget = new QWidget();
  scrollWidget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
  scrollArea->setWidget(scrollWidget);

  m_mainVLayout = new QVBoxLayout();
  m_mainVLayout->setContentsMargins(5, 5, 5, 5);
  m_mainVLayout->setSpacing(5);
  m_mainVLayout->setAlignment(Qt::AlignTop);
  scrollWidget->setLayout(m_mainVLayout);

  auto* measureOptionTitleLabel = new QLabel("Name");
  measureOptionTitleLabel->setObjectName("H2");
  m_mainVLayout->addWidget(measureOptionTitleLabel);

  QRegularExpression nameRegex("^\\S.*");
  auto* validator = new QRegularExpressionValidator(nameRegex, this);

  nameLineEdit = new QLineEdit();
  nameLineEdit->setValidator(validator);
  m_mainVLayout->addWidget(nameLineEdit);

  auto* descriptionTitleLabel = new QLabel("Description");
  descriptionTitleLabel->setObjectName("H2");
  m_mainVLayout->addWidget(descriptionTitleLabel);

  descriptionTextEdit = new QTextEdit();
  descriptionTextEdit->setFixedHeight(70);
  descriptionTextEdit->setAcceptRichText(false);
  descriptionTextEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  descriptionTextEdit->setTabChangesFocus(true);
  m_mainVLayout->addWidget(descriptionTextEdit);

  auto* modelerDescriptionTitleLabel = new QLabel("Modeler Description");
  modelerDescriptionTitleLabel->setObjectName("H2");
  m_mainVLayout->addWidget(modelerDescriptionTitleLabel);

  modelerDescriptionTextEdit = new QTextEdit();
  modelerDescriptionTextEdit->setStyleSheet("background: #E6E6E6;");
  modelerDescriptionTextEdit->setFixedHeight(70);
  modelerDescriptionTextEdit->setAcceptRichText(false);
  modelerDescriptionTextEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  modelerDescriptionTextEdit->setTabChangesFocus(true);
  modelerDescriptionTextEdit->setReadOnly(true);
  m_mainVLayout->addWidget(modelerDescriptionTextEdit);

  auto* line2 = new QFrame();
  line2->setFrameShape(QFrame::HLine);
  line2->setFrameShadow(QFrame::Sunken);
  m_mainVLayout->addWidget(line2);

  auto* inputsTitleLabel = new QLabel("Inputs");
  inputsTitleLabel->setObjectName("H2");
  m_mainVLayout->addWidget(inputsTitleLabel);

  m_inputsVLayout = new QVBoxLayout();
  m_inputsVLayout->setContentsMargins(0, 0, 0, 0);
  m_inputsVLayout->setSpacing(10);

  m_mainVLayout->addLayout(m_inputsVLayout);

  m_mainVLayout->addStretch();

  if (applyMeasureNow) {
    nameLineEdit->setReadOnly(true);
    descriptionTextEdit->setReadOnly(true);
    nameLineEdit->setDisabled(true);
    descriptionTextEdit->setDisabled(true);
  }
}

void EditRubyMeasureView::paintEvent(QPaintEvent*) {
  QStyleOption opt;
  opt.initFrom(this);
  QPainter p(this);
  style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void EditRubyMeasureView::addInputView(QWidget* widget) {
  m_inputsVLayout->addWidget(widget);
}

void EditRubyMeasureView::clear() {
  QLayoutItem* child;

  while ((child = m_inputsVLayout->takeAt(0)) != nullptr) {
    QWidget* widget = child->widget();

    OS_ASSERT(widget);

    delete widget;

    delete child;
  }
}

EditNullView::EditNullView(const QString& text) : QWidget() {
  auto* mainVLayout = new QVBoxLayout();
  mainVLayout->setContentsMargins(5, 5, 5, 5);
  mainVLayout->setSpacing(5);
  setLayout(mainVLayout);

  auto* label = new QLabel();
  label->setText(text);
  label->setWordWrap(true);
  label->setAlignment(Qt::AlignCenter);
  mainVLayout->addWidget(label);

  label->setStyleSheet("QLabel { font-size: 24px; font: bold; color: #6D6D6D }");
}

void EditNullView::paintEvent(QPaintEvent* event) {
  QStyleOption opt;
  opt.initFrom(this);
  QPainter p(this);
  style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

DoubleInputView::DoubleInputView() : InputView() {
  auto* vLayout = new QVBoxLayout();
  vLayout->setContentsMargins(0, 0, 0, 0);
  vLayout->setSpacing(5);
  setLayout(vLayout);

  nameLabel = new QLabel();
  nameLabel->setOpenExternalLinks(true);
  nameLabel->setTextFormat(Qt::RichText);
  nameLabel->setWordWrap(true);
  vLayout->addWidget(nameLabel);

  lineEdit = new QLineEdit();
  lineEdit->setValidator(new QDoubleValidator(lineEdit));
  vLayout->addWidget(lineEdit);
}

void DoubleInputView::setName(const std::string& name, const boost::optional<std::string>& units, const boost::optional<std::string>& description) {
  QString text;
  text += QString::fromStdString(name);
  if (units) {
    text += QString::fromStdString(" (" + units.get() + ")");
  }
  if (description) {
    text += QString::fromStdString("<div style=\"font-size:small;margin-top:2px;\">" + description.get() + "</div>");
  }

  nameLabel->setText(text);
}

void DoubleInputView::setIncomplete(bool incomplete) {
  if (incomplete) {
    nameLabel->setStyleSheet("QLabel { color: #DD0A05;}");
  } else {
    nameLabel->setStyleSheet("QLabel { color: black;}");
  }
}

void DoubleInputView::setDisplayValue(const QVariant& value) {
  lineEdit->setText(value.toString());
}

ChoiceInputView::ChoiceInputView() : InputView() {
  auto* vLayout = new QVBoxLayout();
  vLayout->setContentsMargins(0, 0, 0, 0);
  vLayout->setSpacing(5);
  setLayout(vLayout);

  nameLabel = new QLabel();
  nameLabel->setOpenExternalLinks(true);
  nameLabel->setWordWrap(true);
  vLayout->addWidget(nameLabel);

  comboBox = new InputComboBox();
  vLayout->addWidget(comboBox);
}

void ChoiceInputView::setName(const std::string& name, const boost::optional<std::string>& units, const boost::optional<std::string>& description) {
  QString text;
  text += QString::fromStdString(name);
  if (units) {
    text += QString::fromStdString(" (" + units.get() + ")");
  }
  if (description) {
    text += QString::fromStdString("<div style=\"font-size:small;margin-top:2px;\">" + description.get() + "</div>");
  }

  nameLabel->setText(text);
}

void ChoiceInputView::setIncomplete(bool incomplete) {
  if (incomplete) {
    nameLabel->setStyleSheet("QLabel { color: #DD0A05;}");
  } else {
    nameLabel->setStyleSheet("QLabel { color: black;}");
  }
}

void ChoiceInputView::setDisplayValue(const QVariant& value) {
  comboBox->blockSignals(true);

  comboBox->setCurrentIndex(comboBox->findText(value.toString()));

  comboBox->blockSignals(false);
}

BoolInputView::BoolInputView() : InputView() {
  auto* vLayout = new QVBoxLayout();
  vLayout->setContentsMargins(0, 0, 0, 0);
  vLayout->setSpacing(5);
  setLayout(vLayout);

  checkBox = new InputCheckBox();
  vLayout->addWidget(checkBox);
}

void BoolInputView::setName(const std::string& name, const boost::optional<std::string>& units, const boost::optional<std::string>& description) {
  QString text;
  text += QString::fromStdString(name);
  if (units) {
    text += QString::fromStdString(" (" + units.get() + ")");
  }
  if (description) {
    text += QString::fromStdString("<div style=\"font-size:small;margin-top:2px;\">" + description.get() + "</div>");
  }

  checkBox->setText(text);
}

void BoolInputView::setIncomplete(bool incomplete) {
  checkBox->setIncomplete(incomplete);
}

void BoolInputView::setDisplayValue(const QVariant& value) {
  checkBox->setChecked(value.toBool());
}

IntegerInputView::IntegerInputView() : InputView() {
  auto* vLayout = new QVBoxLayout();
  vLayout->setContentsMargins(0, 0, 0, 0);
  vLayout->setSpacing(5);
  setLayout(vLayout);

  nameLabel = new QLabel();
  nameLabel->setOpenExternalLinks(true);
  nameLabel->setWordWrap(true);
  vLayout->addWidget(nameLabel);

  lineEdit = new QLineEdit();
  lineEdit->setValidator(new QIntValidator(lineEdit));
  vLayout->addWidget(lineEdit);
}

void IntegerInputView::setName(const std::string& name, const boost::optional<std::string>& units, const boost::optional<std::string>& description) {
  QString text;
  text += QString::fromStdString(name);
  if (units) {
    text += QString::fromStdString(" (" + units.get() + ")");
  }
  if (description) {
    text += QString::fromStdString("<div style=\"font-size:small;margin-top:2px;\">" + description.get() + "</div>");
  }

  nameLabel->setText(text);
}

void IntegerInputView::setIncomplete(bool incomplete) {
  if (incomplete) {
    nameLabel->setStyleSheet("QLabel { color: #DD0A05;}");
  } else {
    nameLabel->setStyleSheet("QLabel { color: black;}");
  }
}

void IntegerInputView::setDisplayValue(const QVariant& value) {
  lineEdit->setText(value.toString());
}

StringInputView::StringInputView() : InputView() {
  auto* vLayout = new QVBoxLayout();
  vLayout->setContentsMargins(0, 0, 0, 0);
  vLayout->setSpacing(5);
  setLayout(vLayout);

  nameLabel = new QLabel();
  nameLabel->setOpenExternalLinks(true);
  nameLabel->setWordWrap(true);
  vLayout->addWidget(nameLabel);

  lineEdit = new QLineEdit();
  vLayout->addWidget(lineEdit);
}

void StringInputView::setName(const std::string& name, const boost::optional<std::string>& units, const boost::optional<std::string>& description) {
  QString text;
  text += QString::fromStdString(name);
  if (units) {
    text += QString::fromStdString(" (" + units.get() + ")");
  }
  if (description) {
    text += QString::fromStdString("<div style=\"font-size:small;margin-top:2px;\">" + description.get() + "</div>");
  }

  nameLabel->setText(text);
}

void StringInputView::setIncomplete(bool incomplete) {
  if (incomplete) {
    nameLabel->setStyleSheet("QLabel { color: #DD0A05;}");
  } else {
    nameLabel->setStyleSheet("QLabel { color: black;}");
  }
}

void StringInputView::setDisplayValue(const QVariant& value) {
  lineEdit->setText(value.toString());
}

void InputComboBox::wheelEvent(QWheelEvent* e) {
  e->ignore();
}

InputCheckBox::InputCheckBox() : QAbstractButton() {
  setCheckable(true);

  auto* mainHLayout = new QHBoxLayout();
  QStyleOptionButton opt;
  QRect checkRect = style()->subElementRect(QStyle::SE_CheckBoxIndicator, &opt);
  int spacing = 5;
  mainHLayout->setContentsMargins(checkRect.width() + spacing, 0, 0, 0);
  mainHLayout->setSpacing(spacing);
  setLayout(mainHLayout);

  m_label = new QLabel();
  m_label->setOpenExternalLinks(true);
  m_label->setWordWrap(true);
  mainHLayout->addWidget(m_label);

  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
}

InputCheckBox::~InputCheckBox() = default;

void InputCheckBox::setText(const QString& text) {
  m_label->setText(text);

  QAbstractButton::setText(text);
}

void InputCheckBox::paintEvent(QPaintEvent* event) {
  QStyleOptionButton opt;
  opt.initFrom(this);
  QPainter p(this);
  QPalette palette;

  if (isChecked()) {
    opt.state |= QStyle::State_On;
  } else {
    opt.state |= QStyle::State_Off;
  }

  style()->drawControl(QStyle::CE_CheckBox, &opt, &p, this);
}

void InputCheckBox::setIncomplete(bool incomplete) {
  if (incomplete) {
    m_label->setStyleSheet("QLabel { color: #DD0A05;}");
  } else {
    m_label->setStyleSheet("QLabel { color: black;}");
  }
}

}  // namespace openstudio
