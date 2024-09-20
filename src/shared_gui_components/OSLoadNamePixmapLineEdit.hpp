/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef SHAREDGUICOMPONENTS_OSLOADNAMEPIXMAPLINEEDIT_HPP
#define SHAREDGUICOMPONENTS_OSLOADNAMEPIXMAPLINEEDIT_HPP

#include "FieldMethodTypedefs.hpp"

#include <openstudio/model/Model.hpp>

#include "OSLineEdit.hpp"

class QLabel;

namespace openstudio {

class OSItem;

class OSLoadNamePixmapLineEdit
  : public QWidget
  , public OSLineEdit2Interface
{
  Q_OBJECT

 public:
  explicit OSLoadNamePixmapLineEdit(QWidget* parent = nullptr);

  virtual ~OSLoadNamePixmapLineEdit();

  virtual void enableClickFocus() override;

  virtual bool hasData() override;

  virtual bool locked() const override;

  virtual void setLocked(bool locked) override;

  virtual boost::optional<model::ModelObject> modelObject() const override;

  virtual void bind(const model::ModelObject& modelObject, StringGetter get, boost::optional<StringSetter> set = boost::none,
                    boost::optional<NoFailAction> reset = boost::none, boost::optional<BasicQuery> isDefaulted = boost::none) override;

  virtual void bind(const model::ModelObject& modelObject, OptionalStringGetter get, boost::optional<StringSetter> set = boost::none,
                    boost::optional<NoFailAction> reset = boost::none, boost::optional<BasicQuery> isDefaulted = boost::none) override;

  virtual void bind(const model::ModelObject& modelObject, OptionalStringGetter get,
                    boost::optional<StringSetterOptionalStringReturn> set = boost::none, boost::optional<NoFailAction> reset = boost::none,
                    boost::optional<BasicQuery> isDefaulted = boost::none) override;

  virtual void bind(const model::ModelObject& modelObject, OptionalStringGetterBoolArg get, boost::optional<StringSetterOptionalStringReturn> set,
                    boost::optional<NoFailAction> reset = boost::none, boost::optional<BasicQuery> isDefaulted = boost::none) override;

  virtual void bind(const model::ModelObject& modelObject, StringGetter get, boost::optional<StringSetterVoidReturn> set = boost::none,
                    boost::optional<NoFailAction> reset = boost::none, boost::optional<BasicQuery> isDefaulted = boost::none) override;

  virtual void unbind() override;

  virtual QWidget* qwidget() override;

 signals:

  void itemClicked(OSItem* item);

  void objectRemoved(boost::optional<model::ParentObject> parent);

  void inFocus(bool inFocus, bool hasData);

 public slots:

  virtual void onItemRemoveClicked();

 private:
  void completeBind();
  void createWidgets();
  void setIcon();

  OSLineEdit2* m_lineEdit = nullptr;
  QLabel* m_label = nullptr;
};

}  // namespace openstudio

#endif  // SHAREDGUICOMPONENTS_OSLOADNAMEPIXMAPLINEEDIT_HPP
