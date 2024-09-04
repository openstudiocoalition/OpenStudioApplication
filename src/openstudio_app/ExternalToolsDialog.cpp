/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "./ExternalToolsDialog.hpp"

#include "../model_editor/Utilities.hpp"

#include <openstudio/utilities/core/Path.hpp>

#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QFileDialog>
#include <QDir>

#include <iostream>

namespace openstudio {

ExternalToolsDialog::ExternalToolsDialog(const openstudio::path& t_dviewPath) : m_dviewPathLineEdit(new QLineEdit(this)) {

  auto* mainLayout = new QGridLayout();
  setLayout(mainLayout);
  mainLayout->setColumnMinimumWidth(1, 400);

  int row = 0;
  auto* title = new QLabel("Change External Tools");
  title->setObjectName("H1");
  mainLayout->addWidget(title, row, 0, 1, 3);

  // Dview
  ++row;
  mainLayout->addWidget(new QLabel("Path to DView"), row, 0);

  m_dviewPathLineEdit->setText(QString::fromStdString(toString(t_dviewPath)));
  mainLayout->addWidget(m_dviewPathLineEdit, row, 1);

  auto* changeDviewButton = new QPushButton("Change");
  connect(changeDviewButton, &QPushButton::clicked, this, [this] { ExternalToolsDialog::onChangeClicked(m_dviewPathLineEdit, "DView"); });
  mainLayout->addWidget(changeDviewButton, row, 2);

  // Note JM 2019-07-11: Other Tool: example
  //++row;
  //mainLayout->addWidget(new QLabel("Path to Another Tool"), row, 0);

  //m_otherToolPathLineEdit = new QLineEdit(this);
  //mainLayout->addWidget(m_otherToolPathLineEdit, row, 1);
  //QPushButton * changeOtherButton = new QPushButton("Change");
  //connect(changeOtherButton, &QPushButton::clicked, this, [this]{ ExternalToolsDialog::onChangeClicked(m_otherToolPathLineEdit, "openstudio"); });
  //mainLayout->addWidget(changeOtherButton, row, 2);

  // Buttons Ok/Cancel
  ++row;
  auto* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  mainLayout->addWidget(buttonBox, row, 0, 1, 3);

  connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

void ExternalToolsDialog::onChangeClicked(QLineEdit* t_lineEdit, const QString& toolName) {

#if defined _WIN32
  QString filter = toolName + QString(" (") + toolName + QString("*.exe)");
#elif defined Q_OS_DARWIN
  // TODO: definitely not going to work! Thanks Apple!
  QString filter = toolName + QString(" (") + toolName + QString("*.app)");
#else
  const QString& filter = toolName;
#endif

  LOG_FREE(Debug, "OpenStudioApp", "ExternalToolsDialog::onChangeClicked: file filter = " << openstudio::toString(filter));

  auto fileName = QFileDialog::getOpenFileName(nullptr, tr("Select Path to ") + toolName, QDir().homePath(), filter);

  if (!fileName.isNull()) {
    t_lineEdit->setText(fileName);
  }
}

openstudio::path ExternalToolsDialog::dviewPath() const {
  return openstudio::toPath(m_dviewPathLineEdit->text());
}

//openstudio::path ExternalToolsDialog::otherToolPath() const {
//return openstudio::toPath(m_otherToolPathLineEdit->text());
//}

}  // namespace openstudio
