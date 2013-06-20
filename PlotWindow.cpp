#include "PlotWindow.h"
#include <QLabel>

#include <QGridLayout>
#include <QMessageBox>

#include "QtHardMon.h"

// the plotting library is optional
#if(USE_QWT)
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#endif

PlotWindow::PlotWindow(QtHardMon * hardMon)
  : QWidget(hardMon, Qt::Window), _hardMon(hardMon)
{
  _plotWindowForm.setupUi(this);
  setWindowIcon(  QIcon(":/DESY_logo_nofade.png") );

  // a layout for the plotFrame
  _plotFrameLayout = new QGridLayout(_plotWindowForm.plotFrame);

#if(USE_QWT)
  _qwtPlot = NULL;

  // disable the GUI elements which are not implemented yet
  _plotWindowForm.accumulatePlotsLabel->setEnabled(false);
  _plotWindowForm.accumulatePlotsSpinBox->setEnabled(false);
  _plotWindowForm.plotToNewWindowCheckBox->setEnabled(false);

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

void PlotWindow::closeEvent( QCloseEvent * event )
{
  QWidget::closeEvent(event);
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
    QTableWidgetItem *tableWidgetItem = _hardMon->_hardMonForm.valuesTableWidget->item(row,0); // always column 0
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
  
  QwtPointSeriesData* myData = new QwtPointSeriesData;
  myData->setSamples(samples);
 
  QwtPlotCurve *curve1 = new QwtPlotCurve("Curve 1");

  // at this point the curve takes ownership of the data object
  curve1->setData(myData);

  // replace the current plot.
  delete _qwtPlot;
  _qwtPlot = new QwtPlot(_plotWindowForm.plotFrame); 
  _plotFrameLayout->addWidget(_qwtPlot);
 
  // at this point the curve is attached to the plot, which will delete it when it goes out of scope
  curve1->attach(_qwtPlot);
 
  _qwtPlot->replot();

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

