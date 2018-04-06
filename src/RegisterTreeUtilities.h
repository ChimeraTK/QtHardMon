#ifndef REGISTERTREEUTILITIES_H
#define REGISTERTREEUTILITIES_H

#include <qtreewidget.h>
#include <ChimeraTK/RegisterPath.h>
#include <ChimeraTK/RegisterInfo.h>
#include <boost/shared_ptr.hpp>

QTreeWidgetItem * getParentNodeFromTreeWidget(std::string const & nodeName, QTreeWidget *treeWidget);

QTreeWidgetItem * getParentNodeFromTreeWidgetItem(std::string const & nodeName, QTreeWidgetItem *parentTreeWidgetItem);

class RegisterTreeUtilities {

public:
  /// @attention Can return nullptr of the register path has no hierarchy.
  static QTreeWidgetItem * getDeepestBranchNode(boost::shared_ptr<ChimeraTK::RegisterInfo> registerInfo,
                                                QTreeWidget *treeWidget);
  
  static QTreeWidgetItem * getDeepestBranchNode(boost::shared_ptr<ChimeraTK::RegisterInfo> registerInfo,
                                                QTreeWidgetItem *parentModuleItem,
                                                unsigned int depth = 0);

  static std::string getRegisterName(boost::shared_ptr<ChimeraTK::RegisterInfo> registerInfo);
};


#endif // REGISTERTREEUTILITIES_H
