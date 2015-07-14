/*
 * customQTreeItemInterface.h
 *
 *  Created on: Jul 14, 2015
 *      Author: varghese
 */

#ifndef SOURCE_DIRECTORY__CUSTOMQTREEITEM_H_
#define SOURCE_DIRECTORY__CUSTOMQTREEITEM_H_

#include <MtcaMappedDevice/mapFile.h>
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

/**
 * interface class for Qtree custom items
 */
class CustomQTreeItem : public QTreeWidgetItem {
public:
  /**
   * Default constructor
   */
  CustomQTreeItem(QTreeWidget* parent_, const QString& text_, const int type_);
  CustomQTreeItem(QTreeWidgetItem* parent_, const QString& text_,
                  const int type_);
  /**
   * read in from the card and polpulate the table widget
   */
  virtual void read(QTableWidget* const tablewidget) = 0;
  /**
   * write user updated values from the table to the card
   */
  virtual void write(QTableWidget* const tablewidget) = 0;

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
  ModuleItem(QTreeWidget* parent_, const QString& text_);
  virtual void read(QTableWidget* const tablewidget);
  virtual void write(QTableWidget* const tablewidget);
  virtual void updateRegisterProperties(RegsterPropertyGrpBox const& grpBox);

  static const int DataType = QTreeWidgetItem::UserType + 1;
};

class RegisterItem : public CustomQTreeItem {
public:
  RegisterItem(const mtca4u::mapFile::mapElem& registerInfo, QTreeWidgetItem* parent_,
                const QString& text_);
  virtual void read(QTableWidget* const tablewidget);
  virtual void write(QTableWidget* const tablewidget);
  virtual void updateRegisterProperties(RegsterPropertyGrpBox const& grpBox);
  virtual mtca4u::mapFile::mapElem const getRegisterMapElement();

  static const int DataType = QTreeWidgetItem::UserType + 2;

private:
  mtca4u::mapFile::mapElem _registerMapElement;
};
#endif /* SOURCE_DIRECTORY__CUSTOMQTREEITEM_H_ */
