/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef SHAREDGUICOMPONENTS_EDITCONTROLLER_HPP
#define SHAREDGUICOMPONENTS_EDITCONTROLLER_HPP

#include <openstudio/measure/OSArgument.hpp>
#include <openstudio/utilities/core/Logger.hpp>
#include <QObject>
#include <QPointer>
#include <QSharedPointer>

class QWidget;

namespace openstudio {

class OSViewSwitcher;

class EditRubyMeasureView;
class EditNullView;
class InputController;
class InputView;
class BaseApp;

namespace measuretab {

class MeasureStepItem;

}

class EditController : public QObject
{
  Q_OBJECT

 public:
  QPointer<OSViewSwitcher> editView;

  QPointer<EditRubyMeasureView> editRubyMeasureView;

  explicit EditController(bool applyMeasureNow = false);

  virtual ~EditController();

  void setMeasureStepItem(measuretab::MeasureStepItem* measureStepItem, BaseApp* t_app);

  measuretab::MeasureStepItem* measureStepItem() const;

  // Show something when no RubyMeasure is selected
  void reset();

 private slots:

  void updateDescription();

 private:
  REGISTER_LOGGER("openstudio.pat.EditController");

  QPointer<EditNullView> m_editNullView;

  std::vector<QSharedPointer<InputController>> m_inputControllers;

  QPointer<measuretab::MeasureStepItem> m_measureStepItem;
};

class InputController : public QObject
{
  Q_OBJECT

 public:
  InputController(EditController* editController, measure::OSArgument argument, BaseApp* t_app);

  virtual ~InputController();

  QPointer<InputView> inputView;

 private slots:

  void setValue(const QString& text);

  void setValue(bool value);

  void setValueForIndex(int index);

 private:
  bool isItOKToClearResults();

  bool isArgumentIncomplete() const;

  BaseApp* m_app;

  QPointer<EditController> m_editController;

  measure::OSArgument m_argument;
};

}  // namespace openstudio

#endif  // SHAREDGUICOMPONENTS_EDITCONTROLLER_HPP
