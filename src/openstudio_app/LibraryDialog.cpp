/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "./LibraryDialog.hpp"
#include "../model_editor/Utilities.hpp"

#include <openstudio/utilities/core/Path.hpp>
#include <QVBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QFileDialog>
#include <QDir>

#include <iostream>

namespace openstudio {

LibraryDialog::LibraryDialog(const std::vector<openstudio::path>& paths, const std::vector<openstudio::path>& defaultPaths,
                             const openstudio::path& resourcesPath)
  : QDialog(), m_defaultPaths(defaultPaths), m_resourcesPath(resourcesPath) {
  auto* mainLayout = new QVBoxLayout();
  setLayout(mainLayout);

  auto* title = new QLabel("Change Default Libraries");
  title->setObjectName("H1");
  mainLayout->addWidget(title);

  m_list = new QListWidget();
  m_list->setTextElideMode(Qt::ElideLeft);
  m_list->setMinimumWidth(600);
  m_list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  m_list->setFocusPolicy(Qt::NoFocus);
  mainLayout->addWidget(m_list);

  auto* addRemoveLayout = new QHBoxLayout();
  mainLayout->addLayout(addRemoveLayout);

  auto* add = new QPushButton("Add");
  addRemoveLayout->addWidget(add, 0);
  connect(add, &QPushButton::clicked, this, &LibraryDialog::onAdd);

  auto* remove = new QPushButton("Remove");
  addRemoveLayout->addWidget(remove, 0);
  connect(remove, &QPushButton::clicked, this, &LibraryDialog::onRemove);

  addRemoveLayout->addStretch(1);

  auto* restore = new QPushButton("Restore Defaults");
  addRemoveLayout->addWidget(restore, 0, Qt::AlignRight);
  connect(restore, &QPushButton::clicked, this, &LibraryDialog::onRestore);

  auto* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  mainLayout->addWidget(buttonBox);

  connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

  for (const auto& path : paths) {
    m_list->addItem(QString::fromStdString(toString(path)));
  }
}

void LibraryDialog::onAdd() {
  auto fileName = QFileDialog::getOpenFileName(nullptr, tr("Select OpenStudio Library"), toQString(m_resourcesPath), tr("OpenStudio Files (*.osm)"));

  if (!fileName.isNull()) {
    m_list->addItem(fileName);
  }
}

void LibraryDialog::onRemove() {
  for (auto* item : m_list->selectedItems()) {
    delete item;
  }
}

void LibraryDialog::onRestore() {
  m_list->clear();

  for (const auto& path : m_defaultPaths) {
    m_list->addItem(QString::fromStdString(toString(path)));
  }
}

std::vector<openstudio::path> LibraryDialog::paths() const {
  std::vector<openstudio::path> result;

  for (int i = 0; i != m_list->count(); ++i) {
    auto stringValue = m_list->item(i)->text();
    result.push_back(toPath(stringValue));
  }

  return result;
}

}  // namespace openstudio
