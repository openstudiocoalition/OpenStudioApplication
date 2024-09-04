/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef SHAREDGUICOMPONENTS_BCLMEASUREDIALOG_HPP
#define SHAREDGUICOMPONENTS_BCLMEASUREDIALOG_HPP

#include "OSDialog.hpp"

#include <openstudio/utilities/bcl/BCLMeasure.hpp>

class QLineEdit;
class QTextEdit;
class QLabel;
class QComboBox;
class QRadioButton;
class QListWidget;

namespace openstudio {

class BCLMeasureDialog : public OSDialog
{
  Q_OBJECT

 public:
  /// Constructor of the dialog to create a new BCL Measure
  explicit BCLMeasureDialog(QWidget* parent = nullptr);

  /// Constructor of the dialog to create a copy of an existing BCL Measure
  explicit BCLMeasureDialog(const BCLMeasure& bclMeasure, QWidget* parent = nullptr);

  virtual ~BCLMeasureDialog();

  QSize sizeHint() const override;

  /// After calling exec() and getting appropriate return code
  /// Call createMeasure to create the new measure
  boost::optional<openstudio::BCLMeasure> createMeasure();

 private slots:

  void nameChanged(const QString& newName);

  void measureTypeChanged(const QString& newName);

  void firstLevelTaxonomyChanged(const QString& newName);

 private:
  void init();

  QLineEdit* m_nameLineEdit;
  QLabel* m_classNameLabel;
  QTextEdit* m_descriptionTextEdit;
  QTextEdit* m_modelerDescriptionTextEdit;
  QComboBox* m_measureTypeComboBox;
  QComboBox* m_measureLanguageComboBox;
  QComboBox* m_taxonomyFirstLevelComboBox;
  QComboBox* m_taxonomySecondLevelComboBox;
  QListWidget* m_intendedSoftwareToolListWidget;
  QListWidget* m_intendedUseCaseListWidget;

  boost::optional<BCLMeasure> m_bclMeasureToCopy;
};

}  // namespace openstudio

#endif  // SHAREDGUICOMPONENTS_BCLMEASUREDIALOG_HPP
