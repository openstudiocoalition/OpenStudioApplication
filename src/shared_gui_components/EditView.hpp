/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef SHAREDGUICOMPONENTS_EDITVIEW_HPP
#define SHAREDGUICOMPONENTS_EDITVIEW_HPP

#include <QAbstractButton>
#include <QComboBox>
#include <QLabel>
#include <QWidget>

#include <boost/optional.hpp>
#include <openstudio/utilities/core/Filesystem.hpp>

class QLineEdit;
class QPushButton;
class QTextEdit;
class QVBoxLayout;

namespace openstudio {

class InputCheckBox;

class EditNullView : public QWidget
{
  Q_OBJECT

 public:
  explicit EditNullView(const QString& text = "Select a Measure to Edit");
  virtual ~EditNullView() = default;

 protected:
  void paintEvent(QPaintEvent*) override;
};

class EditRubyMeasureView : public QWidget
{
  Q_OBJECT

 public:
  explicit EditRubyMeasureView(bool applyMeasureNow);
  virtual ~EditRubyMeasureView() = default;

  QLineEdit* nameLineEdit;

  QTextEdit* descriptionTextEdit;

  QTextEdit* modelerDescriptionTextEdit;

  void addInputView(QWidget* widget);

  // Clear all of the information specific to a particular ruby perturbation
  void clear();

 protected:
  void paintEvent(QPaintEvent*) override;

 private:
  std::vector<QWidget*> m_inputViews;

  QVBoxLayout* m_mainVLayout;

  QVBoxLayout* m_inputsVLayout;
};

class InputView : public QWidget
{
 public:
  virtual void setIncomplete(bool incomplete) {}

  virtual void setDisplayValue(const QVariant& value) {}
};

class DoubleInputView : public InputView
{
  Q_OBJECT

 public:
  DoubleInputView();
  virtual ~DoubleInputView() = default;

  QLineEdit* lineEdit;

  void setName(const std::string& name, const boost::optional<std::string>& units, const boost::optional<std::string>& description);

  void setIncomplete(bool incomplete) override;

  void setDisplayValue(const QVariant& value) override;

 private:
  QLabel* nameLabel;
};

class ChoiceInputView : public InputView
{
  Q_OBJECT

 public:
  ChoiceInputView();
  virtual ~ChoiceInputView() = default;

  QComboBox* comboBox;

  void setName(const std::string& name, const boost::optional<std::string>& units, const boost::optional<std::string>& description);

  void setIncomplete(bool incomplete) override;

  void setDisplayValue(const QVariant& value) override;

 private:
  QLabel* nameLabel;
};

class BoolInputView : public InputView
{
  Q_OBJECT

 public:
  BoolInputView();
  virtual ~BoolInputView() = default;

  InputCheckBox* checkBox;

  void setName(const std::string& name, const boost::optional<std::string>& units, const boost::optional<std::string>& description);

  void setIncomplete(bool incomplete) override;

  void setDisplayValue(const QVariant& value) override;
};

class IntegerInputView : public InputView
{
  Q_OBJECT

 public:
  IntegerInputView();
  virtual ~IntegerInputView() = default;

  QLineEdit* lineEdit;

  void setName(const std::string& name, const boost::optional<std::string>& units, const boost::optional<std::string>& description);

  void setIncomplete(bool incomplete) override;

  void setDisplayValue(const QVariant& value) override;

 private:
  QLabel* nameLabel;
};

class StringInputView : public InputView
{
  Q_OBJECT

 public:
  StringInputView();
  virtual ~StringInputView() = default;

  QLineEdit* lineEdit;

  void setName(const std::string& name, const boost::optional<std::string>& units, const boost::optional<std::string>& description);

  void setIncomplete(bool incomplete) override;

  void setDisplayValue(const QVariant& value) override;

 private:
  QLabel* nameLabel;
};

class InputComboBox : public QComboBox
{
  Q_OBJECT

 protected:
  void wheelEvent(QWheelEvent* e) override;
};

class InputCheckBox : public QAbstractButton
{
  Q_OBJECT

 public:
  InputCheckBox();

  virtual ~InputCheckBox() = default;

  void setText(const QString& text);

  void setIncomplete(bool incomplete);

 protected:
  void paintEvent(QPaintEvent* event) override;

 private:
  QLabel* m_label;
};

class PathInputView : public InputView
{
  Q_OBJECT

 public:
  PathInputView(const std::string& extension, bool isRead);
  virtual ~PathInputView() = default;

  QLineEdit* lineEdit;
  QPushButton* selectPathButton;

  void setName(const std::string& name, const boost::optional<std::string>& units, const boost::optional<std::string>& description);

  void setIncomplete(bool incomplete) override;

  void setDisplayValue(const QVariant& value) override;

 signals:
  void selectedPathChanged(const openstudio::path& p);

 private slots:
  void onSelectPathButtonClicked();

 private:
  QLabel* nameLabel;
  QString m_extension;
  bool m_isRead;
};

}  // namespace openstudio

#endif  // SHAREDGUICOMPONENTS_EDITVIEW_HPP
