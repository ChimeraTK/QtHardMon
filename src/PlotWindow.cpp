// SPDX-FileCopyrightText: Deutsches Elektronen-Synchrotron DESY, MSK, ChimeraTK Project <chimeratk-support@desy.de>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "PlotWindow.h"

#include "PreferencesProvider.h"
#include "QtHardMon.h"
#include <QtCharts/qlineseries.h>
#include <QtCharts/QtCharts>

#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>

PlotWindow::PlotWindow(QtHardMon* hardMon)
: QWidget(hardMon, Qt::Window), _plotWindowForm(), _hardMon(hardMon), _plotFrameLayout(nullptr), _chart(new QChart),
  _series(new QLineSeries) {
  _plotWindowForm.setupUi(this);
  setWindowIcon(QIcon(":/ChimeraTK_Logo_whitebg.png"));

  // disable the GUI elements which are not implemented yet
  _plotWindowForm.accumulatePlotsLabel->setEnabled(false);
  _plotWindowForm.accumulatePlotsSpinBox->setEnabled(false);
  _plotWindowForm.plotToNewWindowCheckBox->setEnabled(false);

  // a layout for the plotFrame
  _plotFrameLayout = new QGridLayout(_plotWindowForm.plotFrame);

  // For some reason, if color is set to 0,0,0 it will ignore it and show the default blue
  _series->setColor(QColor::fromRgb(1, 0, 0));
  _chart->layout()->setContentsMargins(0, 0, 0, 0);
  _chart->legend()->setVisible(false);

  auto* view = new QChartView(_chart);
  view->setRenderHint(QPainter::RenderHint::Antialiasing, false);
  view->setRubberBand(QChartView::RubberBand::RectangleRubberBand);

  _plotFrameLayout->addWidget(view);
  connect(_plotWindowForm.plotButton, SIGNAL(clicked()), this, SLOT(plot()));
}

void PlotWindow::closeEvent(QCloseEvent* event_) {
  QWidget::closeEvent(event_);
  emit plotWindowClosed();
}

void PlotWindow::plot() {
  QVector<QPointF> samples;

  PreferencesProvider& preferencesProvider = PreferencesProviderSingleton::Instance();

  // the plot window might be active while no register is selected. Just don't
  // do anything in this case.
  if(!(_hardMon->currentAccessorModel_)) {
    return;
  }

  // We use the data model here. It automatically always gives the cooked data
  // and the right size. In addition, if the conversion to double fails, we get
  // this information and don't display.
  //@todo Get the data type and show a message why data cannot be plotted
  // note: In rowCount we can use an invalid model index.
  for(int row = 0; row <
      std::min(_hardMon->currentAccessorModel_->rowCount(QModelIndex()), preferencesProvider.getValue<int>("maxWords"));
      ++row) {
    auto dataPoint =
        _hardMon->currentAccessorModel_->data(_hardMon->currentAccessorModel_->index(row, 0 /* =cooked data column*/));

    // If the conversion cannot be executed, the value is 0. No need to do
    // special handling here. For strings for example canConvert is true, and
    // "3" actually is converted to 3.0, while "hello world" will result in 0.
    // All other currently supported user data types should be properly
    // convertible to double (void is still to come, we will see). Anyway, with
    // QVariants default behaviour to return 0 we should be fine.
    double value = dataPoint.value<double>();

    samples.push_back(QPointF(row, value));
  }

  _series->replace(samples);
  _chart->removeSeries(_series);
  _chart->addSeries(_series);
  _chart->createDefaultAxes();
  for(auto* axis : _chart->axes()) {
    axis->setLabelsFont(this->font());
  }
}

bool PlotWindow::plotAfterReadIsChecked() {
  return _plotWindowForm.plotAfterReadCheckBox->isChecked();
}

void PlotWindow::setPlotAfterRead(bool checked) {
  return _plotWindowForm.plotAfterReadCheckBox->setChecked(checked);
}
