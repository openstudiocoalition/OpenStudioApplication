/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef SHAREDGUICOMPONENTS_BUTTONS_HPP
#define SHAREDGUICOMPONENTS_BUTTONS_HPP

#include <QPushButton>

#include <boost/smart_ptr.hpp>

class QPaintEvent;

class QPixmap;

class QTimer;

namespace openstudio {

class ZoomInButton : public QPushButton
{
  Q_OBJECT

 public:
  explicit ZoomInButton(QWidget* parent = nullptr);
  virtual ~ZoomInButton() = default;
};

class ZoomOutButton : public QPushButton
{
  Q_OBJECT

 public:
  explicit ZoomOutButton(QWidget* parent = nullptr);
  virtual ~ZoomOutButton() = default;
};

class OneLevelDownButton : public QPushButton
{
  Q_OBJECT

 public:
  explicit OneLevelDownButton(QWidget* parent = nullptr);
  virtual ~OneLevelDownButton() = default;
};

class OneLevelUpButton : public QPushButton
{
  Q_OBJECT

 public:
  explicit OneLevelUpButton(QWidget* parent = nullptr);
  virtual ~OneLevelUpButton() = default;
};

class GrayButton : public QPushButton
{
  Q_OBJECT

 public:
  explicit GrayButton(QWidget* parent = nullptr);
  virtual ~GrayButton() = default;
};

class BlueButton : public QPushButton
{
  Q_OBJECT

 public:
  explicit BlueButton(QWidget* parent = nullptr);
  virtual ~BlueButton() = default;
};

class AddButton : public QPushButton
{
  Q_OBJECT

 public:
  explicit AddButton(QWidget* parent = nullptr);
  virtual ~AddButton() = default;
};

class SofterAddButton : public QPushButton
{
  Q_OBJECT

 public:
  explicit SofterAddButton(QWidget* parent = nullptr);
  virtual ~SofterAddButton() = default;
};

class RemoveButton : public QPushButton
{
  Q_OBJECT

 public:
  explicit RemoveButton(QWidget* parent = nullptr);
  virtual ~RemoveButton() = default;
};

class SofterRemoveButton : public QPushButton
{
  Q_OBJECT

 public:
  explicit SofterRemoveButton(QWidget* parent = nullptr);
  virtual ~SofterRemoveButton() = default;
};

class AddScriptButton : public QPushButton
{
  Q_OBJECT

 public:
  explicit AddScriptButton(QWidget* parent = nullptr);
  virtual ~AddScriptButton() = default;
};

class DuplicateButton : public QPushButton
{
  Q_OBJECT

 public:
  explicit DuplicateButton(QWidget* parent = nullptr);
  virtual ~DuplicateButton() = default;
};

class SofterDuplicateButton : public QPushButton
{
  Q_OBJECT

 public:
  explicit SofterDuplicateButton(QWidget* parent = nullptr);
  virtual ~SofterDuplicateButton() = default;
};

class UpButton : public QPushButton
{
  Q_OBJECT

 public:
  explicit UpButton(QWidget* parent = nullptr);
  virtual ~UpButton() = default;
};

class DownButton : public QPushButton
{
  Q_OBJECT

 public:
  explicit DownButton(QWidget* parent = nullptr);
  virtual ~DownButton() = default;
};

class MyMeasuresFolderButton : public QPushButton
{
  Q_OBJECT

 public:
  explicit MyMeasuresFolderButton(QWidget* parent = nullptr);
  virtual ~MyMeasuresFolderButton() = default;
};

class CheckForUpdateButton : public QPushButton
{
  Q_OBJECT

 public:
  explicit CheckForUpdateButton(QWidget* parent = nullptr);
  virtual ~CheckForUpdateButton() = default;
};

class OpenDirectoryButton : public QPushButton
{
  Q_OBJECT

 public:
  explicit OpenDirectoryButton(QWidget* parent = nullptr);
  virtual ~OpenDirectoryButton() = default;
};

class CreateOneWithSelectedMeasuresButton : public GrayButton
{
  Q_OBJECT

 public:
  CreateOneWithSelectedMeasuresButton();
  virtual ~CreateOneWithSelectedMeasuresButton() = default;
};

class CreateOneForEachSelectedMeasureButton : public GrayButton
{
  Q_OBJECT

 public:
  CreateOneForEachSelectedMeasureButton();
  virtual ~CreateOneForEachSelectedMeasureButton() = default;
};

class CreateFromFileButton : public GrayButton
{
  Q_OBJECT

 public:
  CreateFromFileButton();
  virtual ~CreateFromFileButton() = default;
};

class PlayButton : public QPushButton
{
  Q_OBJECT

 public:
  enum Status
  {
    IDLE,
    IDLEDISABLED,
    STARTING,
    RUNNING,
    STOPPING,
    ERROR
  };

  explicit PlayButton(QWidget* parent = nullptr);
  virtual ~PlayButton() = default;

  Status status() const;
  void setStatus(const Status& status);

 private:
  Status m_status;
};

class RecalculateButton : public QPushButton
{
  Q_OBJECT

 public:
  explicit RecalculateButton(QWidget* parent = nullptr);
  virtual ~RecalculateButton() = default;
};

}  // namespace openstudio

#endif  // SHAREDGUICOMPONENTS_BUTTONS_HPP
