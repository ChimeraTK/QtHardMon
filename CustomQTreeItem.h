#ifndef SOURCE_DIRECTORY__CUSTOMQTREEITEM_H_
#define SOURCE_DIRECTORY__CUSTOMQTREEITEM_H_

#include <MtcaMappedDevice/mapFile.h>
#include <MtcaMappedDevice/MultiplexedDataAccessor.h>
#include <MtcaMappedDevice/devPCIE.h>
#include <qtreewidget.h>
#include <qlabel.h>
#include <qtablewidget.h>

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


struct TableWidgetData{
	QTableWidget* table;
	unsigned int maxRow;

	TableWidgetData(): table(0), maxRow(0){}
};

/**
 * interface class for Qtree custom items
 */
class CustomQTreeItem : public QTreeWidgetItem {
public:
  /**
   * Default constructor
   */
  CustomQTreeItem(const QString& text_, const int type_,
                  QTreeWidget* parent_);
  CustomQTreeItem(const QString& text_, const int type_,
                  QTreeWidgetItem* parent_);
  /**
   * read in from the card and polpulate the table widget
   */
  virtual void read(TableWidgetData const &tabledata,
                    mtca4u::devPCIE const& device) = 0;
  /**
   * write user updated values from the table to the card
   */
  virtual void write(TableWidgetData const &tabledata,
                     mtca4u::devPCIE const& device) = 0;

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
};

class ModuleItem : public CustomQTreeItem {
public:
  ModuleItem(const QString& text_, QTreeWidget* parent_ = 0);
  virtual void read(TableWidgetData const &tabledata,
                    mtca4u::devPCIE const& device);
  virtual void write(TableWidgetData const &tabledata,
                     mtca4u::devPCIE const& device);
  virtual void updateRegisterProperties(RegsterPropertyGrpBox const& grpBox);

  static const int DataType = QTreeWidgetItem::UserType + 1;
};

class RegisterItem : public CustomQTreeItem {
public:
  RegisterItem(const mtca4u::mapFile::mapElem& registerInfo,
               const QString& text_, QTreeWidgetItem* parent_ = 0);
  virtual void read(TableWidgetData const &tabledata,
                    mtca4u::devPCIE const& device);
  virtual void write(TableWidgetData const &tabledata,
                     mtca4u::devPCIE const& device);
  virtual void updateRegisterProperties(RegsterPropertyGrpBox const& grpBox);
  virtual mtca4u::mapFile::mapElem const getRegisterMapElement();

  static const int DataType = QTreeWidgetItem::UserType + 2;

private:
  mtca4u::mapFile::mapElem _registerMapElement;
};

class MultiplexedAreaItem : public CustomQTreeItem {
public:
  MultiplexedAreaItem(
      boost::shared_ptr<mtca4u::MultiplexedDataAccessor<double> > const&
          accessor,
      const mtca4u::mapFile::mapElem& registerInfo, const QString& text_,
      QTreeWidgetItem* parent_ = 0);

  virtual void read(TableWidgetData const &tabledata,
                    mtca4u::devPCIE const& device);
  virtual void write(TableWidgetData const &tabledata,
                     mtca4u::devPCIE const& device);
  virtual void updateRegisterProperties(RegsterPropertyGrpBox const& grpBox);
  virtual mtca4u::mapFile::mapElem const getRegisterMapElement();
  boost::shared_ptr<mtca4u::MultiplexedDataAccessor<double> > const& getAccessor();

  mtca4u::ptrmapFile const& getPtrToMapFile();
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
  virtual void read(TableWidgetData const &tabledata,
                    mtca4u::devPCIE const& device);
  virtual void write(TableWidgetData const &tabledata,
                     mtca4u::devPCIE const& device);
  virtual void updateRegisterProperties(RegsterPropertyGrpBox const& grpBox);
  virtual mtca4u::mapFile::mapElem const getRegisterMapElement();

  static const int DataType = QTreeWidgetItem::UserType + 4;

private:
  mtca4u::mapFile::mapElem _registerMapElement;
  unsigned int _sequenceNumber;

  boost::shared_ptr<mtca4u::MultiplexedDataAccessor<double> > const & getAccessor();
};
#endif /* SOURCE_DIRECTORY__CUSTOMQTREEITEM_H_ */
