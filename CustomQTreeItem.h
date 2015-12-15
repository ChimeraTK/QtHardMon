#ifndef SOURCE_DIRECTORY__CUSTOMQTREEITEM_H_
#define SOURCE_DIRECTORY__CUSTOMQTREEITEM_H_

#include <mtca4u/RegisterInfoMap.h>
#include <mtca4u/MultiplexedDataAccessor.h>
#include <mtca4u/PcieBackend.h>
#include <qtreewidget.h>
#include <qlabel.h>
#include <qtablewidget.h>
#include <typeinfo>
#include "Constants.h"

/**
 * Structure used internally
 */
struct RegisterPropertyGrpBox {
  /// Ptr to form label
  QLabel* registerNameDisplay;
  /// Ptr to form label
  QLabel* moduleDisplay;
  /// Ptr to form label
  QLabel* registerBarDisplay;
  /// Ptr to form label
  QLabel* registerAddressDisplay;
  /// Ptr to form label
  QLabel* registerNElementsDisplay;
  /// Ptr to form label
  QLabel* registerSizeDisplay;
  /// Ptr to form label
  QLabel* registerWidthDisplay;
  /// Ptr to form label
  QLabel* registerFracBitsDisplay;
  /// Ptr to form label
  QLabel* registeSignBitDisplay;

  RegisterPropertyGrpBox()
      : registerNameDisplay(0),
        moduleDisplay(0),
        registerBarDisplay(0),
        registerAddressDisplay(0),
        registerNElementsDisplay(0),
        registerSizeDisplay(0),
        registerWidthDisplay(0),
        registerFracBitsDisplay(0),
        registeSignBitDisplay(0) {}
};

/**
 * Introduced to move around data from the main form to the helper classes
 */
struct TableWidgetData {
  /// Ptr to main form table widget
  QTableWidget* table;
  /// User provisioned value for the maximum number of rows to be displayed in
  /// the table
  unsigned int tableMaxRowCount;
  /// Pointer to the currently opened device.
  boost::shared_ptr<mtca4u::DeviceBackend> device;

  TableWidgetData() : table(0), tableMaxRowCount(0), device() {}
  /**
   * Default constructor
   */
  TableWidgetData(QTableWidget* table_, unsigned int maxRow_,
                  boost::shared_ptr<mtca4u::DeviceBackend> const& device_)
      : table(table_), tableMaxRowCount(maxRow_), device(device_) {}

  /**
   * Default copy constructor: to keep the -Weffc++ flag happy
   */
  TableWidgetData(TableWidgetData const& data)
      : table(data.table),
        tableMaxRowCount(data.tableMaxRowCount),
        device(data.device) {}
  /**
   * Default assignment operator: to keep the -Weffc++ flag happy
   */
  TableWidgetData& operator=(TableWidgetData const& data) {
    table = data.table;
    tableMaxRowCount = data.tableMaxRowCount;
    device = data.device;
    return *this;
  }
};

/**
 * interface class for Qtree custom items
 */
class CustomQTreeItem : public QTreeWidgetItem {
public:
  /**
   * Default constructor
   */
  CustomQTreeItem(const QString& text_, const int type_, QTreeWidget* parent_);
  /**
   * Called when registering to table widget
   */
  CustomQTreeItem(const QString& text_, const int type_,
                  QTreeWidgetItem* parent_);
  /**
   * read in from the card and polpulate the table widget
   */
  virtual void read(TableWidgetData const& tabledata) = 0;
  /**
   * write user updated values from the table to the card
   */
  virtual void write(TableWidgetData const& tabledata) = 0;

  /**
   * update the register properties window of qthardmon
   */
  virtual void updateRegisterProperties(
      RegisterPropertyGrpBox const& grpBox) = 0;

  /**
   * retuen the map file element if applicable
   */
  virtual mtca4u::RegisterInfoMap::RegisterInfo const getRegisterMapElement();
  /**
   *
   */
  virtual ~CustomQTreeItem();

protected:
  /**
   * Helper method
   */
  void clearTable(TableWidgetData const& tabledata);

  /**
   * Helper method
   */
  template <typename T>
  void putValuesIntoTable(TableWidgetData const& tabledata,
                          std::vector<T> const& buffer);

  /**
   * Helper method
   */
  template <typename T>
  std::vector<T> copyValuesFromTable(TableWidgetData const& tabledata,
                                     unsigned int count);

  /**
   * Helper method
   */
  void fillGrpBox(RegisterPropertyGrpBox const& grpBox,
                  mtca4u::RegisterInfoMap::RegisterInfo const& regInfo);
};

/**
 * This class represents a module item entry in the QTreeItem
 */
class ModuleItem : public CustomQTreeItem {
public:
  /**
   * Default constructor
   */
  ModuleItem(const QString& text_, QTreeWidget* parent_ = 0);
  virtual void read(TableWidgetData const& tabledata);
  virtual void write(TableWidgetData const& tabledata);
  virtual void updateRegisterProperties(RegisterPropertyGrpBox const& grpBox);

  /// Data type that represents the module element
  static const int DataType = QTreeWidgetItem::UserType + 1;

private:
  void clearGrpBox(RegisterPropertyGrpBox const& grpBox);
};

/**
 * This class represents a register item entry in the QTreeItem
 */
class RegisterItem : public CustomQTreeItem {
public:
  /**
   * Default constructor
   */
  RegisterItem(const mtca4u::RegisterInfoMap::RegisterInfo& registerInfo,
               const QString& text_, QTreeWidgetItem* parent_ = 0);
  virtual void read(TableWidgetData const& tabledata);
  virtual void write(TableWidgetData const& tabledata);
  virtual void updateRegisterProperties(RegisterPropertyGrpBox const& grpBox);
  virtual mtca4u::RegisterInfoMap::RegisterInfo const getRegisterMapElement();

  /// Data type that represents the RegisterItem element
  static const int DataType = QTreeWidgetItem::UserType + 2;

private:
  mtca4u::RegisterInfoMap::RegisterInfo _registerMapElement;

  std::vector<int> fetchElementsFromCard(TableWidgetData const& tabledata);
  void writeRegisterToDevice(TableWidgetData const& tabledata,
                             std::vector<int> const& buffer);
};

/**
 * This class represents a Multiplexed area descriptor entry in the QTreeItem
 */
class MultiplexedAreaItem : public CustomQTreeItem {
public:
  /**
   * Default constructor
   */
  MultiplexedAreaItem(
      boost::shared_ptr<mtca4u::MultiplexedDataAccessor<double> > const&
          accessor,
      const mtca4u::RegisterInfoMap::RegisterInfo& registerInfo,
      const QString& text_, QTreeWidgetItem* parent_ = 0);

  virtual void read(TableWidgetData const& tabledata);
  virtual void write(TableWidgetData const& tabledata);
  virtual void updateRegisterProperties(RegisterPropertyGrpBox const& grpBox);
  virtual mtca4u::RegisterInfoMap::RegisterInfo const getRegisterMapElement();
  /// return the dmux data accessor.
  boost::shared_ptr<mtca4u::MultiplexedDataAccessor<double> > const&
  getAccessor();

  /// Data type that represents the Mux Area descriptor element
  static const int DataType = QTreeWidgetItem::UserType + 3;

private:
  boost::shared_ptr<mtca4u::MultiplexedDataAccessor<double> > _dataAccessor;
  mtca4u::RegisterInfoMap::RegisterInfo _registerMapElement;
};

/**
 * This class represents a Sequence descriptor entry in the QTreeItem
 */
class SequenceDescriptor : public CustomQTreeItem {
public:
  /**
   * Default constructor
   */
  SequenceDescriptor(const mtca4u::RegisterInfoMap::RegisterInfo& registerInfo,
                     unsigned int sequenceNumber, const QString& text_,
                     QTreeWidgetItem* parent_ = 0);
  virtual void read(TableWidgetData const& tabledata);
  virtual void write(TableWidgetData const& tabledata);
  virtual void updateRegisterProperties(RegisterPropertyGrpBox const& grpBox);
  virtual mtca4u::RegisterInfoMap::RegisterInfo const getRegisterMapElement();

  /// Data type that represents the Sequence descriptor element
  static const int DataType = QTreeWidgetItem::UserType + 4;

private:
  mtca4u::RegisterInfoMap::RegisterInfo _registerMapElement;
  unsigned int _sequenceNumber;

  boost::shared_ptr<mtca4u::MultiplexedDataAccessor<double> > const&
  getAccessor();

  void writeSequenceToCard(
      TableWidgetData const& tabledata,
      boost::shared_ptr<mtca4u::MultiplexedDataAccessor<double> > const&
          accessor);
};

template <typename T>
inline void CustomQTreeItem::putValuesIntoTable(
    const TableWidgetData& tabledata, std::vector<T> const& buffer) {
  int column = 0;
  if (typeid(T) == typeid(int)) {
    column = qthardmon::FIXED_POINT_DISPLAY_COLUMN;
  } else if (typeid(T) == typeid(double)) {
    column = qthardmon::FLOATING_POINT_DISPLAY_COLUMN;
  }

  QTableWidget* table = tabledata.table;
  unsigned int maxRow = tabledata.tableMaxRowCount;

  size_t rowCount = (buffer.size() > maxRow) ? maxRow : buffer.size();
  // Prepare the appropriate number of rows in the table
  table->setRowCount(rowCount);

  for (size_t row = 0; row < rowCount; row++) {
    // Prepare a data item with a QVariant. The QVariant takes care that the
    // type is recognised as
    // int and a proper editor (spin box) is used when editing in the GUI.
    QTableWidgetItem* dataItem = new QTableWidgetItem();
    QTableWidgetItem* rowItem = new QTableWidgetItem();
    std::stringstream rowAsText;
    rowAsText << row;

    rowItem->setText(rowAsText.str().c_str());
    table->setVerticalHeaderItem(row, rowItem);

    if (row == maxRow) { // The register is too large to display. Show that it
                         // is truncated and stop reading
      dataItem->setText("truncated");
      dataItem->setFlags(dataItem->flags() & ~Qt::ItemIsSelectable &
                         ~Qt::ItemIsEditable);
      dataItem->setToolTip("List is truncated. You can change the number of "
                           "words displayed in the preferences.");
      table->setItem(row, 0, dataItem);
      break;
    }

    T registerContent = buffer[row];
    dataItem->setData(0, QVariant(registerContent)); // 0 is the default role
    table->setItem(row, column, dataItem);
  } // for row
}

template <typename T>
inline std::vector<T> CustomQTreeItem::copyValuesFromTable(
    const TableWidgetData& tabledata, unsigned int count) {

  unsigned int maxRowCount = tabledata.tableMaxRowCount;
  QTableWidget const* table = tabledata.table;
  std::vector<T> buffer(count);

  for (unsigned int row = 0; (row < count) && (row < maxRowCount); row++) {
    int registerContent =
        table->item(row, qthardmon::FIXED_POINT_DISPLAY_COLUMN)
            ->data(0 /*default role*/)
            .toInt();
    buffer[row] = registerContent;
  }
  return buffer;
}

#endif /* SOURCE_DIRECTORY__CUSTOMQTREEITEM_H_ */
