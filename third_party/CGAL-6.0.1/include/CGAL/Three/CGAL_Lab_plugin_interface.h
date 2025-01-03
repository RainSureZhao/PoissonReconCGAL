// Copyright (c) 2009-2015  GeometryFactory Sarl (France)
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org).
//
// $URL: https://github.com/CGAL/cgal/blob/v6.0.1/Three/include/CGAL/Three/CGAL_Lab_plugin_interface.h $
// $Id: include/CGAL/Three/CGAL_Lab_plugin_interface.h 50cfbde3b84 $
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-Commercial
//
//
// Author(s)     : Laurent RINEAU

//! \file CGAL_Lab_plugin_interface.h
#ifndef LAB_DEMO_PLUGIN_INTERFACE_H
#define LAB_DEMO_PLUGIN_INTERFACE_H

#include <CGAL/license/Three.h>


#include <QString>
#include <QList>
#include <QtPlugin>
#include <QDebug>

class QAction;
class QMainWindow;
class Messages_interface;
namespace CGAL {
namespace Three {
class Scene_interface;
  /*!
   * This virtual class provides the basic functions used for making a plugin.
   */
class CGAL_Lab_plugin_interface
{
public:
  //! \brief initializes the plugin
  //! This function acts like a constructor. This is where the attributes must be initialized.
  //! The Message_interface allows to print warnings or errors on the screen and the `Console` widget.
  virtual void init(QMainWindow* , CGAL::Three::Scene_interface* , Messages_interface*) = 0;

  //! \brief indicates if an action is usable or not.
  //! This function usually tests the type of the selected item to determine if `action` can be applied to it,
  //! but not necessarily.
  //! @returns \c true if `action` can be called in the current state, \c false
  //! otherwise
  virtual bool applicable(QAction* action) const = 0;
  //!Contains all the plugin's actions.
  virtual QList<QAction*> actions() const = 0;
  //!\brief is called when the application is closed.
  //! Override this function if you need to perform a specific action
  //! when the application is closed, like hide the widgets if you don't want
  //! their visibility to be saved.
  virtual void closure() {
 }
protected :
};
}
}
Q_DECLARE_INTERFACE(CGAL::Three::CGAL_Lab_plugin_interface,
                    "com.geometryfactory.CGALLab.PluginInterface/1.0")

#endif // LAB_DEMO_PLUGIN_INTERFACE_H
