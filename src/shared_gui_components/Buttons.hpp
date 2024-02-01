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

}  // namespace openstudio

#endif  // SHAREDGUICOMPONENTS_BUTTONS_HPP
