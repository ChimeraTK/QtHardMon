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

/**
 * interface class for Qtree custom items
 */
class CustomQTreeItem : public QTreeWidgetItem {
public:
  /**
   * Default constructor
   */
  CustomQTreeItem(QTreeWidget* parent_, const QString& text_, const int type);
  /**
   * read in from the card and polpulate the table widget
   */
  virtual void read() = 0;
  /**
   * write user updated values from the table to the card
   */
  virtual void write() = 0;

  /**
   * update the register properties window of qthardmon
   */
  virtual void updateRegisterProperties() = 0;

  /**
   * retuen the map file element if applicable
   */
  virtual mtca4u::mapFile::mapElem getRegisterMapElement();
  /**
   *
   */
  virtual ~CustomQTreeItem();
};

class ModuleEntry : public CustomQTreeItem {
public:
  ModuleEntry(QTreeWidget* parent_, const QString& text_);
  virtual void read();
  virtual void write();
  virtual void updateRegisterProperties();
  static const int DataType = QTreeWidgetItem::UserType + 1;
};

#endif /* SOURCE_DIRECTORY__CUSTOMQTREEITEM_H_ */
