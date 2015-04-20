#ifndef QTHARDMON_PLOT_WINDOW_H
#define QTHARDMON_PLOT_WINDOW_H

#include "ui_PlotWindowForm.h"

class QtHardMon;
class QwtPlot;
class QwtPlotZoomer;
class QwtPlotCurve;

/** The plot windows shows the plot itself and some steering buttons for the plotting.
 *  This class also implements all the plotting functionality.
 *
 *  The plotting is implemented with QWT. If this is not available the plot is replaced with an info
 *  text that QWT is not available and all the buttons are disabled.
 *
 */
class PlotWindow: public QWidget
{
 Q_OBJECT

 public:
  /** The constructor */
  PlotWindow(QtHardMon * hardMon);
  /* The destructor. Need not be virtual because we have no virtual functions */
  virtual ~PlotWindow();

  /** Function to query the status of the plot after read checkbox. */
  bool plotAfterReadIsChecked();

 protected:
  /** Reimplemented to emit the plotWindowClosed signal
   */
  virtual void closeEvent ( QCloseEvent * event_ );
  
public slots:
  /** Slot which performs the actual plotting.
   */
  void plot();

  /** Set the check mark of the plotAfterRead check box
   */
  void setPlotAfterRead(bool checked=true);
  
 signals:
  /** A signal which is emitted when the window is closed.
   */
  void plotWindowClosed();

 private:
  Ui::PlotWindowForm _plotWindowForm; //< The GUI form which hold all the widgets.

  /** Pointer to the HardMon which contains this plot window. Needed to access the data to be plotted.
   */
  QtHardMon * _hardMon; 

  /// A layout so the plot can automatically adapt its size.
  QGridLayout * _plotFrameLayout;

#if(USE_QWT)
  QwtPlot * _qwtPlot;
  QwtPlotZoomer* zoomer;
  QwtPlotCurve *curve1;
#endif
  
};

#endif //QTHARDMON_PLOT_WINDOW_H
