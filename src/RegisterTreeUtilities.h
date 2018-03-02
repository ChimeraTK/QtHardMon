#ifndef REGISTERTREEUTILITIES_H
#define REGISTERTREEUTILITIES_H

#include <qtreewidget.h>
#include <mtca4u/RegisterPath.h>
#include <mtca4u/RegisterInfo.h>
#include <boost/shared_ptr.hpp>

QTreeWidgetItem * getParentNodeFromTreeWidget(std::string const & nodeName, QTreeWidget *treeWidget);

QTreeWidgetItem * getParentNodeFromTreeWidgetItem(std::string const & nodeName, QTreeWidgetItem *parentTreeWidgetItem);

class RegisterTreeUtilities {

public:
  static QTreeWidgetItem *
  assignToModuleItem(boost::shared_ptr<mtca4u::RegisterInfo> registerInfo,
                     QTreeWidget *treeWidget);
  static QTreeWidgetItem *
  assignToModuleItem(boost::shared_ptr<mtca4u::RegisterInfo> registerInfo,
                     QTreeWidgetItem *parentModuleItem,
                     unsigned int depth = 0);
  static std::string
  getRegisterName(boost::shared_ptr<mtca4u::RegisterInfo> registerInfo);
};


#endif // REGISTERTREEUTILITIES_H
