#include "PlotWindow.h"
#include <QLabel>

#include <QGridLayout>
#include <QMessageBox>

#include "QtHardMon.h"

// the plotting library is optional
#if(USE_QWT)
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_map.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_canvas.h>
#endif

#include <iostream>
#include "Constants.h"

PlotWindow::PlotWindow(QtHardMon *hardMon)
    : QWidget(hardMon, Qt::Window), _plotWindowForm(), _hardMon(hardMon),
      _plotFrameLayout(NULL)
#if (USE_QWT)
      , _qwtPlot(NULL)
      , _zoomer(NULL)
      , _curve1(NULL)
      , _myData(NULL)
#endif
{
  _plotWindowForm.setupUi(this);
  setWindowIcon(QIcon(":/DESY_logo_nofade.png"));

  // a layout for the plotFrame
  _plotFrameLayout = new QGridLayout(_plotWindowForm.plotFrame);

#if(USE_QWT)

  // disable the GUI elements which are not implemented yet
  _plotWindowForm.accumulatePlotsLabel->setEnabled(false);
  _plotWindowForm.accumulatePlotsSpinBox->setEnabled(false);
  _plotWindowForm.plotToNewWindowCheckBox->setEnabled(false);

  _qwtPlot = new QwtPlot(_plotWindowForm.plotFrame);
  _myData = new QwtPointSeriesData;
  _curve1 = new QwtPlotCurve("Curve 1");
  _curve1->attach(_qwtPlot);
 
  _plotFrameLayout->addWidget(_qwtPlot);
  
  connect(_plotWindowForm.plotButton, SIGNAL(clicked()),
	  this, SLOT(plot()));

#else //USE_QWT

  // Without QWT plotting is not possible. Show a warning instead and disable all functionality.
  QLabel * noQwtLabel = new QLabel("QtHardMon has been compiled without QWT.\n Plotting is disabled in this build.",
				   _plotWindowForm.plotFrame);
  _plotFrameLayout->addWidget(noQwtLabel);

  _plotWindowForm.plotButton->setEnabled(false);
  _plotWindowForm.accumulatePlotsLabel->setEnabled(false);
  _plotWindowForm.accumulatePlotsSpinBox->setEnabled(false);
  _plotWindowForm.plotToNewWindowCheckBox->setEnabled(false);
  _plotWindowForm.plotAfterReadCheckBox->setEnabled(false);

#endif //USE_QWT
}

PlotWindow::~PlotWindow()
{
}

void PlotWindow::closeEvent( QCloseEvent * event_ )
{
  QWidget::closeEvent(event_);
  emit  plotWindowClosed();
}

void PlotWindow::plot()
{
#if(USE_QWT)
  QVector<QPointF> samples;

  //FIXME: use a data vector here. This also overcomes the truncation limitation (wanted?)
  //Use the minimum of rowCount and maxWords. for truncated lists the last entry is invalid
  for (int row = 0; row < std::min(_hardMon->_hardMonForm.valuesTableWidget->rowCount(),
				   static_cast<int>(_hardMon->_maxWords));
       ++row)
  {
    QTableWidgetItem *tableWidgetItem 
      = _hardMon->_hardMonForm.valuesTableWidget->item(row, qthardmon::FLOATING_POINT_DISPLAY_COLUMN);

    if (!tableWidgetItem)
    {
      // strange, this should not happen. print a warning message end stop plotting
      QMessageBox::critical(this, tr("QtHardMon: Error creating plot"), QString("Value in row ")
			    +QString::number(row) + " does not exist.");
      return;
    }

    bool conversionOk;
    // The 0 in data(0) is the policy.
    int value = tableWidgetItem->data(0).toInt(&conversionOk);
    
    if (!conversionOk)
    {
      QMessageBox::critical(this, tr("QtHardMon: Error creating plot"),
			    QString("Value in row ")+QString::number(row) + " is invalid.");
      return;
    }

    samples.push_back(QPoint(row, value));
  }
  
  _myData->setSamples(samples);

  _curve1->setData(_myData);

  _qwtPlot->setAxisAutoScale(0);
  _qwtPlot->setAxisAutoScale(2);

  _qwtPlot->replot();

  if (_zoomer)
    delete _zoomer;

  _zoomer = new QwtPlotZoomer( _qwtPlot->canvas() );
  _zoomer->setMousePattern( QwtEventPattern::MouseSelect2,
  			   Qt::RightButton, Qt::ControlModifier );
  _zoomer->setMousePattern( QwtEventPattern::MouseSelect3,
  			   Qt::RightButton );

#endif //USE_QWT

}

bool PlotWindow::plotAfterReadIsChecked()
{
  return _plotWindowForm.plotAfterReadCheckBox->isChecked();
}

void PlotWindow::setPlotAfterRead(bool checked)
{
  return _plotWindowForm.plotAfterReadCheckBox->setChecked(checked);
}

