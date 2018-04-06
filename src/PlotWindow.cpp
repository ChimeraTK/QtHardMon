#include "PlotWindow.h"
#include <QLabel>

#include <QGridLayout>
#include <QMessageBox>

#include "PreferencesProvider.h"
#include "QtHardMon.h"

// the plotting library is optional
#if (USE_QWT)
#include <qwt_plot.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_zoomer.h>
#include <qwt_scale_map.h>
#endif

#include <iostream>

PlotWindow::PlotWindow(QtHardMon *hardMon)
    : QWidget(hardMon, Qt::Window), _plotWindowForm(), _hardMon(hardMon),
      _plotFrameLayout(NULL)
#if (USE_QWT)
      ,
      _qwtPlot(NULL), _zoomer(NULL), _curve1(NULL), _myData(NULL)
#endif
{
  _plotWindowForm.setupUi(this);
  setWindowIcon(QIcon(":/DESY_logo_nofade.svg"));

  // a layout for the plotFrame
  _plotFrameLayout = new QGridLayout(_plotWindowForm.plotFrame);

#if (USE_QWT)

  // disable the GUI elements which are not implemented yet
  _plotWindowForm.accumulatePlotsLabel->setEnabled(false);
  _plotWindowForm.accumulatePlotsSpinBox->setEnabled(false);
  _plotWindowForm.plotToNewWindowCheckBox->setEnabled(false);

  _qwtPlot = new QwtPlot(_plotWindowForm.plotFrame);
  _myData = new QwtPointSeriesData;
  _curve1 = new QwtPlotCurve("Curve 1");
  _curve1->attach(_qwtPlot);

  _plotFrameLayout->addWidget(_qwtPlot);

  connect(_plotWindowForm.plotButton, SIGNAL(clicked()), this, SLOT(plot()));

#else // USE_QWT

  // Without QWT plotting is not possible. Show a warning instead and disable
  // all functionality.
  QLabel *noQwtLabel = new QLabel("QtHardMon has been compiled without QWT.\n "
                                  "Plotting is disabled in this build.",
                                  _plotWindowForm.plotFrame);
  _plotFrameLayout->addWidget(noQwtLabel);

  _plotWindowForm.plotButton->setEnabled(false);
  _plotWindowForm.accumulatePlotsLabel->setEnabled(false);
  _plotWindowForm.accumulatePlotsSpinBox->setEnabled(false);
  _plotWindowForm.plotToNewWindowCheckBox->setEnabled(false);
  _plotWindowForm.plotAfterReadCheckBox->setEnabled(false);

#endif // USE_QWT
}

PlotWindow::~PlotWindow() {}

void PlotWindow::closeEvent(QCloseEvent *event_) {
  QWidget::closeEvent(event_);
  emit plotWindowClosed();
}

void PlotWindow::plot() {
#if (USE_QWT)
  QVector<QPointF> samples;

  PreferencesProvider &preferencesProvider =
      PreferencesProviderSingleton::Instance();

  // the plot window might be active while no register is selected. Just don't do anything in this case.
  if (!(_hardMon->currentAccessorModel_)){
    return;
  }
  
  // We use the data model here. It automatically always gives the coocked data and the right size.
  // In addition, if the coversion to double fails, we get this information and don't display.
  //@todo Get the data type and show a message why data cannot be plotted
  // note: In rowCount we can use an invalid model index.
  for (int row = 0;
       row < std::min(_hardMon->currentAccessorModel_->rowCount(QModelIndex()),
                      preferencesProvider.getValue<int>("maxWords"));
       ++row){
    auto dataPoint = _hardMon->currentAccessorModel_->data(  _hardMon->currentAccessorModel_->index(row, 0 /* =coocked data column*/));

    // If the conversion cannot be executed, the value is 0. No need to do special handling here.
    // For strings for example canConvert is true, and "3" actually is converted to 3.0,
    // while "hello world" will result in 0. All other currently supported user data types should
    // be properly convertible to double (void is still to come, we will see).
    // Anyway, with QVariants default behaviour to return 0 we should be fine.
    double value = dataPoint.value<double>();

    samples.push_back(QPointF(row, value));
  }

  _myData->setSamples(samples);

  _curve1->setData(_myData);

  _qwtPlot->setAxisAutoScale(0);
  _qwtPlot->setAxisAutoScale(2);

  _qwtPlot->replot();

  if (_zoomer)
    delete _zoomer;

  _zoomer = new QwtPlotZoomer(_qwtPlot->canvas());
  _zoomer->setMousePattern(QwtEventPattern::MouseSelect2, Qt::RightButton,
                           Qt::ControlModifier);
  _zoomer->setMousePattern(QwtEventPattern::MouseSelect3, Qt::RightButton);

#endif // USE_QWT
}

bool PlotWindow::plotAfterReadIsChecked() {
  return _plotWindowForm.plotAfterReadCheckBox->isChecked();
}

void PlotWindow::setPlotAfterRead(bool checked) {
  return _plotWindowForm.plotAfterReadCheckBox->setChecked(checked);
}
