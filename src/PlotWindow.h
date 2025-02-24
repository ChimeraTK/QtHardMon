// SPDX-FileCopyrightText: Deutsches Elektronen-Synchrotron DESY, MSK, ChimeraTK Project <chimeratk-support@desy.de>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "ui_PlotWindowForm.h"
#include <QtCharts/QtCharts>

#include <QRectF>
#include <QStack>

class QtHardMon;
class QwtPlot;
class QwtPlotZoomer;
class QwtPlotCurve;
class QwtPointSeriesData;

/** The plot windows shows the plot itself and some steering buttons for the
 * plotting.
 *  This class also implements all the plotting functionality.
 *
 *  The plotting is implemented with QWT. If this is not available the plot is
 * replaced with an info
 *  text that QWT is not available and all the buttons are disabled.
 *
 */
class PlotWindow : public QWidget {
  Q_OBJECT

 public:
  /** The constructor */
  explicit PlotWindow(QtHardMon* hardMon);
  /* The destructor. Need not be virtual because we have no virtual functions */
  ~PlotWindow() override = default;

  /** Function to query the status of the plot after read checkbox. */
  bool plotAfterReadIsChecked();

 protected:
  /** Reimplemented to emit the plotWindowClosed signal
   */
  void closeEvent(QCloseEvent* event_) override;

 public slots:
  /** Slot which performs the actual plotting.
   */
  void plot();

  /** Set the check mark of the plotAfterRead check box
   */
  void setPlotAfterRead(bool checked = true);

 signals:
  /** A signal which is emitted when the window is closed.
   */
  void plotWindowClosed();

 private:
  Ui::PlotWindowForm _plotWindowForm; //< The GUI form which hold all the widgets.

  /** Pointer to the HardMon which contains this plot window. Needed to access
   * the data to be plotted.
   */
  QtHardMon* _hardMon;

  /// A layout so the plot can automatically adapt its size.
  QGridLayout* _plotFrameLayout;

  // Disable copy constructor and assignment operator
  // This  class should'nt need copying
  Q_DISABLE_COPY(PlotWindow) // Easy way to get around -Weffc++ warning:
                             // class QtHardMon’ has pointer data members -
                             // but does not override copy constructor and
                             // assignment operator

  QChart* _chart;
  QLineSeries* _series;
};
