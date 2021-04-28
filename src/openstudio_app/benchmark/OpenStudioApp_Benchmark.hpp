#ifndef OPENSTUDIOAPP_BENCHMARK_H
#define OPENSTUDIOAPP_BENCHMARK_H

#include <QObject>
#include <QtTest>
#include <QtWidgets>
#include "../OpenStudioApp.hpp"

class OpenStudioAppBenchmark : public QObject
{
    Q_OBJECT

//  public:
//    OpenStudioAppBenchmark();

  private slots:
    void init();
    void simple();
    //void multiple_data();
    //void multiple();
    //void series_data();
    //void series();
  private:
    //openstudio::OpenStudioApp *mApp = nullptr;
};

#endif /* ifndef OPENSTUDIOAPP_BENCHMARK_H */
