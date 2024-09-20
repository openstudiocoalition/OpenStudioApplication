/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef SHAREDGUICOMPONENTS_LOCALLIBRARYCONTROLLER_HPP
#define SHAREDGUICOMPONENTS_LOCALLIBRARYCONTROLLER_HPP

#include "OSListView.hpp"
#include "OSListController.hpp"
#include "OSCollapsibleView.hpp"
#include "HeaderViews.hpp"
#include "LocalLibrary.hpp"
#include "BaseApp.hpp"

#include <openstudio/utilities/bcl/BCLMeasure.hpp>

#include <QHBoxLayout>
#include <QLabel>
#include <QObject>
#include <QPointer>
#include <QDomDocument>
#include <QSettings>
#include <QSharedPointer>
#include <QVariant>

namespace openstudio {

class OSListView;
class OSDragPixmapData;

class LocalLibraryView;
class LibraryTypeListController;
class LibraryGroupListController;
class LibrarySubGroupListController;
class LibraryListController;
class LibraryItem;

// TODO: Perhaps generalize LibraryGroupItem, LibrarySubGroupItem, LibraryItem and related classes into one tree like set of class.

class LocalLibraryController : public QObject
{
  Q_OBJECT

  friend class LibraryListController;

 public:
  QPointer<LocalLibraryView> localLibraryView;

  explicit LocalLibraryController(openstudio::BaseApp* t_app, bool onlyShowModelMeasures = false);

  virtual ~LocalLibraryController();

  // Returns the selected item from either my measures or bcl measures.  Returns NULL if there is no selection.
  QPointer<LibraryItem> selectedItem() const;

  void reset();

 public slots:

  void showMeasures();

  void showMyMeasuresFolder();

 private slots:
  void addMeasure();
  void duplicateSelectedMeasure();
  void openBclDlg();
  void checkForRemoteBCLUpdates();

 private:
  REGISTER_LOGGER("openstudio.LocalLibraryController");

  BaseApp* m_app;

  QPointer<OSListView> libraryView;

  QSharedPointer<LibraryTypeListController> createLibraryListController(const QDomDocument& taxonomy, LocalLibrary::LibrarySource source);

  QSettings m_settings;

  bool m_onlyShowModelMeasures;
};

class LibraryTypeItem : public OSListItem
{
  Q_OBJECT

 public:
  explicit LibraryTypeItem(const QString& name);
  virtual ~LibraryTypeItem() {}

  QString name() const {
    return m_name;
  }

  QSharedPointer<LibraryGroupListController> libraryGroupListController() const {
    return m_libraryGroupListController;
  }

 private:
  QString m_name;

  QSharedPointer<LibraryGroupListController> m_libraryGroupListController;
};

class LibraryTypeItemDelegate : public OSItemDelegate
{
  QWidget* view(QSharedPointer<OSListItem> dataSource) override;

 public:
  explicit LibraryTypeItemDelegate(BaseApp* t_app);
  virtual ~LibraryTypeItemDelegate() {}

 private:
  BaseApp* m_app;
};

class LibraryTypeListController : public OSListController
{
  Q_OBJECT;

 public:
  virtual ~LibraryTypeListController() {}

  QSharedPointer<OSListItem> itemAt(int i) override;

  int count() override;

  void addItem(QSharedPointer<OSListItem> item);

  void reset();

 private:
  QList<QSharedPointer<LibraryTypeItem>> m_items;
};

class LibraryGroupItem : public OSListItem
{
  Q_OBJECT

 public:
  LibraryGroupItem(const QString& name, BaseApp* t_app);
  virtual ~LibraryGroupItem() {}

  QString name() const {
    return m_name;
  }

  QSharedPointer<LibrarySubGroupListController> librarySubGroupListController() const {
    return m_librarySubGroupListController;
  }

 private:
  QString m_name;

  QSharedPointer<LibrarySubGroupListController> m_librarySubGroupListController;
};

class LibraryGroupItemDelegate : public OSItemDelegate
{
  QWidget* view(QSharedPointer<OSListItem> dataSource) override;

 public:
  explicit LibraryGroupItemDelegate(BaseApp* t_app);
  virtual ~LibraryGroupItemDelegate() {}

 private:
  BaseApp* m_app;
};

class LibraryGroupListController : public OSListController
{
  Q_OBJECT;

 public:
  virtual ~LibraryGroupListController() {}

  QSharedPointer<OSListItem> itemAt(int i) override;

  int count() override;

  void addItem(QSharedPointer<OSListItem> item);

  void reset();

 private:
  QList<QSharedPointer<LibraryGroupItem>> m_items;
};

class LibrarySubGroupItem : public OSListItem
{
  Q_OBJECT

 public:
  LibrarySubGroupItem(const QString& name, const QString& taxonomyTag, LocalLibrary::LibrarySource source, BaseApp* t_app,
                      bool onlyShowModelMeasures);

  virtual ~LibrarySubGroupItem() {}

  QString name() const {
    return m_name;
  }

  QSharedPointer<LibraryListController> libraryListController() const {
    return m_libraryListController;
  }

 signals:

  void libraryItemCountChanged(int count);

 private:
  BaseApp* m_app;
  QString m_name;

  QSharedPointer<LibraryListController> m_libraryListController;
};

class LibrarySubGroupItemDelegate : public OSItemDelegate
{
  QWidget* view(QSharedPointer<OSListItem> dataSource) override;

 public:
  explicit LibrarySubGroupItemDelegate(BaseApp* t_app);
  virtual ~LibrarySubGroupItemDelegate() {}

 private:
  BaseApp* m_app;
};

class LibrarySubGroupListController : public OSListController
{
  Q_OBJECT;

 public:
  explicit LibrarySubGroupListController(BaseApp* t_app);
  virtual ~LibrarySubGroupListController() {}

  QSharedPointer<OSListItem> itemAt(int i) override;

  int count() override;

  void addItem(QSharedPointer<OSListItem> item);

  void reset();

  int libraryItemCount();

 signals:

  void libraryItemCountChanged(int libraryItemCount);

 private:
  BaseApp* m_app;
  QList<QSharedPointer<LibrarySubGroupItem>> m_items;
};

class LibraryItem : public OSListItem
{
  Q_OBJECT

 public:
  LibraryItem(const BCLMeasure& bclMeasure, LocalLibrary::LibrarySource source, BaseApp* t_app);

  virtual ~LibraryItem();

  //Q_PROPERTY(QString name READ name)

  bool hasError() const;

  QString name() const;

  QString displayName() const;

  QString className() const;

  QString description() const;

  QString modelerDescription() const;

  QString error() const;

  UUID uuid() const;

  BCLMeasure m_bclMeasure;

  LocalLibrary::LibrarySource m_source;

  bool isAvailable() const;

 public slots:

  void dragItem(const OSDragPixmapData& dragPixmapData);

 private:
  bool m_available;
  BaseApp* m_app;
};

class LibraryItemDelegate : public OSItemDelegate
{
  Q_OBJECT;

 public:
  explicit LibraryItemDelegate(BaseApp* t_app);
  virtual ~LibraryItemDelegate() {}
  QWidget* view(QSharedPointer<OSListItem> dataSource) override;

 private slots:

  void selectedChanged();

 private:
  BaseApp* m_app;
};

class LibraryListController : public OSListController
{
  Q_OBJECT;

 public:
  LibraryListController(const QString& taxonomyTag, LocalLibrary::LibrarySource source, BaseApp* t_app, bool onlyShowModelMeasures);

  virtual ~LibraryListController() {}

  QSharedPointer<OSListItem> itemAt(int i) override;

  int count() override;

  void reset();

 signals:

  void countChanged(int count);

 private:
  void createItems();

  BaseApp* m_app;

  QString m_taxonomyTag;

  LocalLibrary::LibrarySource m_source;

  std::vector<QSharedPointer<LibraryItem>> m_items;

  bool m_onlyShowModelMeasures;
};

}  // namespace openstudio

#endif  // SHAREDGUICOMPONENTS_LOCALLIBRARYCONTROLLER_HPP
