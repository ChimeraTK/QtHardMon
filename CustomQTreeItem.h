#ifndef SOURCE_DIRECTORY__CUSTOMQTREEITEM_H_
#define SOURCE_DIRECTORY__CUSTOMQTREEITEM_H_

#include <MtcaMappedDevice/mapFile.h>
#include <MtcaMappedDevice/MultiplexedDataAccessor.h>
#include <MtcaMappedDevice/devPCIE.h>
#include <qtreewidget.h>
#include <qlabel.h>
#include <qtablewidget.h>
#include <typeinfo>
#include "Constants.h"

struct RegsterPropertyGrpBox {
  QLabel* registerNameDisplay;
  QLabel* moduleDisplay;
  QLabel* registerBarDisplay;
  QLabel* registerAddressDisplay;
  QLabel* registerNElementsDisplay;
  QLabel* registerSizeDisplay;
  QLabel* registerWidthDisplay;
  QLabel* registerFracBitsDisplay;
  QLabel* registeSignBitDisplay;

  RegsterPropertyGrpBox()
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

struct TableWidgetData {
  QTableWidget* table;
  unsigned int tableMaxRowCount;
  boost::shared_ptr<mtca4u::devBase> device;

  TableWidgetData() : table(0), tableMaxRowCount(0), device() {}
  TableWidgetData(QTableWidget* table_, unsigned int maxRow_,
                  boost::shared_ptr<mtca4u::devBase> const& device_)
      : table(table_), tableMaxRowCount(maxRow_), device(device_) {}

  TableWidgetData(TableWidgetData const& data)
      : table(data.table),
        tableMaxRowCount(data.tableMaxRowCount),
        device(data.device) {}

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
      RegsterPropertyGrpBox const& grpBox) = 0;

  /**
   * retuen the map file element if applicable
   */
  virtual mtca4u::mapFile::mapElem const getRegisterMapElement();
  /**
   *
   */
  virtual ~CustomQTreeItem();

protected:
  void fillTableWithDummyValues(TableWidgetData const& tableData);
  void createTableRowEntries(TableWidgetData const& tabledata,
                             unsigned int rows = 0);

  template <typename T>
  void putValuesIntoTable(TableWidgetData const& tabledata,
                          std::vector<T> buffer);
  void fillGrpBox(RegsterPropertyGrpBox const& grpBox,
                  mtca4u::mapFile::mapElem const& regInfo);
};

class ModuleItem : public CustomQTreeItem {
public:
  ModuleItem(const QString& text_, QTreeWidget* parent_ = 0);
  virtual void read(TableWidgetData const& tabledata);
  virtual void write(TableWidgetData const& tabledata);
  virtual void updateRegisterProperties(RegsterPropertyGrpBox const& grpBox);

  static const int DataType = QTreeWidgetItem::UserType + 1;
};

class RegisterItem : public CustomQTreeItem {
public:
  RegisterItem(const mtca4u::mapFile::mapElem& registerInfo,
               const QString& text_, QTreeWidgetItem* parent_ = 0);
  virtual void read(TableWidgetData const& tabledata);
  virtual void write(TableWidgetData const& tabledata);
  virtual void updateRegisterProperties(RegsterPropertyGrpBox const& grpBox);
  virtual mtca4u::mapFile::mapElem const getRegisterMapElement();

  static const int DataType = QTreeWidgetItem::UserType + 2;

private:
  mtca4u::mapFile::mapElem _registerMapElement;

  std::vector<int> fetchElementsFromCard(TableWidgetData const& tabledata);
};

class MultiplexedAreaItem : public CustomQTreeItem {
public:
  MultiplexedAreaItem(
      boost::shared_ptr<mtca4u::MultiplexedDataAccessor<double> > const&
          accessor,
      const mtca4u::mapFile::mapElem& registerInfo, const QString& text_,
      QTreeWidgetItem* parent_ = 0);

  virtual void read(TableWidgetData const& tabledata);
  virtual void write(TableWidgetData const& tabledata);
  virtual void updateRegisterProperties(RegsterPropertyGrpBox const& grpBox);
  virtual mtca4u::mapFile::mapElem const getRegisterMapElement();
  boost::shared_ptr<mtca4u::MultiplexedDataAccessor<double> > const&
  getAccessor();

  static const int DataType = QTreeWidgetItem::UserType + 3;

private:
  boost::shared_ptr<mtca4u::MultiplexedDataAccessor<double> > _dataAccessor;
  mtca4u::mapFile::mapElem _registerMapElement;
};

class SequenceDescriptor : public CustomQTreeItem {
public:
  SequenceDescriptor(const mtca4u::mapFile::mapElem& registerInfo,
                     unsigned int sequenceNumber, const QString& text_,
                     QTreeWidgetItem* parent_ = 0);
  virtual void read(TableWidgetData const& tabledata);
  virtual void write(TableWidgetData const& tabledata);
  virtual void updateRegisterProperties(RegsterPropertyGrpBox const& grpBox);
  virtual mtca4u::mapFile::mapElem const getRegisterMapElement();

  static const int DataType = QTreeWidgetItem::UserType + 4;

private:
  mtca4u::mapFile::mapElem _registerMapElement;
  unsigned int _sequenceNumber;

  boost::shared_ptr<mtca4u::MultiplexedDataAccessor<double> > const&
  getAccessor();
};

template <typename T>
inline void CustomQTreeItem::putValuesIntoTable(
    const TableWidgetData& tabledata, std::vector<T> buffer) {
  int column = 0;
  if (typeid(T) == typeid(int)) {
    column = qthardmon::FIXED_POINT_DISPLAY_COLUMN;
  } else if (typeid(T) == typeid(double)) {
    column = qthardmon::FLOATING_POINT_DISPLAY_COLUMN;
  }

  QTableWidget* table = tabledata.table;
  unsigned int maxRow = tabledata.tableMaxRowCount;

  for (unsigned int row = 0; row < buffer.size(); row++) {
    // Prepare a data item with a QVariant. The QVariant takes care that the
    // type is recognised as
    // int and a proper editor (spin box) is used when editing in the GUI.
    QTableWidgetItem* dataItem = new QTableWidgetItem();

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
    // int registerContent = (readError?-1:inputBuffer[row]);
    T registerContent = buffer[row];

    dataItem->setData(0, QVariant(registerContent)); // 0 is the default role
    table->setItem(row, column, dataItem);
  } // for row
}

#endif /* SOURCE_DIRECTORY__CUSTOMQTREEITEM_H_ */
