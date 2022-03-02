#ifndef REGISTERTREEUTILITIES_H
#define REGISTERTREEUTILITIES_H

#include <ChimeraTK/RegisterInfo.h>
#include <ChimeraTK/RegisterPath.h>
#include <boost/shared_ptr.hpp>
#include <qtreewidget.h>

QTreeWidgetItem* getParentNodeFromTreeWidget(std::string const& nodeName, QTreeWidget* treeWidget);

QTreeWidgetItem* getParentNodeFromTreeWidgetItem(std::string const& nodeName, QTreeWidgetItem* parentTreeWidgetItem);

class RegisterTreeUtilities {
 public:
  /// @attention Can return nullptr of the register path has no hierarchy.
  static QTreeWidgetItem* getDeepestBranchNode(const ChimeraTK::RegisterInfo& registerInfo, QTreeWidget* treeWidget);

  static QTreeWidgetItem* getDeepestBranchNode(
      const ChimeraTK::RegisterInfo& registerInfo, QTreeWidgetItem* parentModuleItem, unsigned int depth = 0);

  static std::string getRegisterName(const ChimeraTK::RegisterInfo& registerInfo);
};

#endif // REGISTERTREEUTILITIES_H
