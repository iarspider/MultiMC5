/* Copyright 2013-2015 MultiMC Contributors
 *
 * Authors: Andrew Okin
 *          Peterix
 *          Orochimarufan <orochimarufan.x3@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "MultiMC.h"
#include "BuildConfig.h"

#include "MainWindow.h"

#include <QtCore/QVariant>
#include <QtCore/QUrl>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>

#include <QtGui/QKeyEvent>

#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QWidgetAction>
#include <QtWidgets/QProgressDialog>
#include <QtWidgets/QShortcut>

#include <BaseInstance.h>
#include <Env.h>
#include <InstanceList.h>
#include <MMCZip.h>
#include <auth/flows/AuthenticateTask.h>
#include <auth/flows/RefreshTask.h>
#include <icons/IconList.h>
#include <java/JavaUtils.h>
#include <java/JavaInstallList.h>
#include <launch/LaunchTask.h>
#include <minecraft/MinecraftVersionList.h>
#include <minecraft/LwjglVersionList.h>
#include <minecraft/SkinUtils.h>
#include <net/URLConstants.h>
#include <net/NetJob.h>
#include <net/CacheDownload.h>
#include <news/NewsChecker.h>
#include <notifications/NotificationChecker.h>
#include <resources/Resource.h>
#include <tools/BaseProfiler.h>
#include <updater/DownloadTask.h>
#include <updater/UpdateChecker.h>
#include <DesktopServices.h>

#include "InstancePageProvider.h"
#include "InstanceProxyModel.h"
#include "JavaCommon.h"
#include "LaunchInteraction.h"
#include "SettingsUI.h"
#include "groupview/GroupView.h"
#include "groupview/InstanceDelegate.h"
#include "widgets/LabeledToolButton.h"
#include "widgets/ServerStatus.h"
#include "dialogs/NewInstanceDialog.h"
#include "dialogs/ProgressDialog.h"
#include "dialogs/AboutDialog.h"
#include "dialogs/VersionSelectDialog.h"
#include "dialogs/CustomMessageBox.h"
#include "dialogs/IconPickerDialog.h"
#include "dialogs/CopyInstanceDialog.h"
#include "dialogs/AccountSelectDialog.h"
#include "dialogs/UpdateDialog.h"
#include "dialogs/EditAccountDialog.h"
#include "dialogs/NotificationDialog.h"
#include "dialogs/ExportInstanceDialog.h"

class MainWindow::Ui
{
public:
	QAction *actionAddInstance;
	QAction *actionViewInstanceFolder;
	QAction *actionRefresh;
	QAction *actionViewCentralModsFolder;
	QAction *actionCheckUpdate;
	QAction *actionSettings;
	QAction *actionReportBug;
	QAction *actionPatreon;
	QAction *actionMoreNews;
	QAction *actionAbout;
	QAction *actionLaunchInstance;
	QAction *actionRenameInstance;
	QAction *actionChangeInstGroup;
	QAction *actionChangeInstIcon;
	QAction *actionEditInstNotes;
	QAction *actionEditInstance;
	QAction *actionViewSelectedInstFolder;
	QAction *actionDeleteInstance;
	QAction *actionConfig_Folder;
	QAction *actionCAT;
	QAction *actionREDDIT;
	QAction *actionDISCORD;
	QAction *actionCopyInstance;
	QAction *actionManageAccounts;
	QAction *actionLaunchInstanceOffline;
	QAction *actionScreenshots;
	QAction *actionInstanceSettings;
	QAction *actionExportInstance;
	QWidget *centralWidget;
	QHBoxLayout *horizontalLayout;
	QToolBar *mainToolBar;
	QStatusBar *statusBar;
	QToolBar *instanceToolBar;
	QToolBar *newsToolBar;

	void setupUi(QMainWindow *MainWindow)
	{
		if (MainWindow->objectName().isEmpty())
		{
			MainWindow->setObjectName(QStringLiteral("MainWindow"));
		}
		MainWindow->resize(694, 563);
		MainWindow->setWindowIcon(MMC->getThemedIcon("multimc"));
		actionAddInstance = new QAction(MainWindow);
		actionAddInstance->setObjectName(QStringLiteral("actionAddInstance"));
		actionAddInstance->setIcon(MMC->getThemedIcon("new"));
		actionViewInstanceFolder = new QAction(MainWindow);
		actionViewInstanceFolder->setObjectName(QStringLiteral("actionViewInstanceFolder"));
		actionViewInstanceFolder->setIcon(MMC->getThemedIcon("viewfolder"));
		actionRefresh = new QAction(MainWindow);
		actionRefresh->setObjectName(QStringLiteral("actionRefresh"));
		actionRefresh->setIcon(MMC->getThemedIcon("refresh"));
		actionViewCentralModsFolder = new QAction(MainWindow);
		actionViewCentralModsFolder->setObjectName(QStringLiteral("actionViewCentralModsFolder"));
		actionViewCentralModsFolder->setIcon(MMC->getThemedIcon("centralmods"));
		if(BuildConfig.UPDATER_ENABLED)
		{
			actionCheckUpdate = new QAction(MainWindow);
			actionCheckUpdate->setObjectName(QStringLiteral("actionCheckUpdate"));
			actionCheckUpdate->setIcon(MMC->getThemedIcon("checkupdate"));
		}
		actionSettings = new QAction(MainWindow);
		actionSettings->setObjectName(QStringLiteral("actionSettings"));
		actionSettings->setIcon(MMC->getThemedIcon("settings"));
		actionSettings->setMenuRole(QAction::PreferencesRole);
		actionReportBug = new QAction(MainWindow);
		actionReportBug->setObjectName(QStringLiteral("actionReportBug"));
		actionReportBug->setIcon(MMC->getThemedIcon("bug"));
		actionPatreon = new QAction(MainWindow);
		actionPatreon->setObjectName(QStringLiteral("actionPatreon"));
		actionPatreon->setIcon(MMC->getThemedIcon("patreon"));
		actionMoreNews = new QAction(MainWindow);
		actionMoreNews->setObjectName(QStringLiteral("actionMoreNews"));
		actionMoreNews->setIcon(MMC->getThemedIcon("news"));
		actionAbout = new QAction(MainWindow);
		actionAbout->setObjectName(QStringLiteral("actionAbout"));
		actionAbout->setIcon(MMC->getThemedIcon("about"));
		actionAbout->setMenuRole(QAction::AboutRole);
		actionLaunchInstance = new QAction(MainWindow);
		actionLaunchInstance->setObjectName(QStringLiteral("actionLaunchInstance"));
		actionRenameInstance = new QAction(MainWindow);
		actionRenameInstance->setObjectName(QStringLiteral("actionRenameInstance"));
		actionChangeInstGroup = new QAction(MainWindow);
		actionChangeInstGroup->setObjectName(QStringLiteral("actionChangeInstGroup"));
		actionChangeInstIcon = new QAction(MainWindow);
		actionChangeInstIcon->setObjectName(QStringLiteral("actionChangeInstIcon"));
		actionChangeInstIcon->setEnabled(true);
		actionChangeInstIcon->setIcon(QIcon(":/icons/instances/infinity"));
		actionChangeInstIcon->setIconVisibleInMenu(true);
		actionEditInstNotes = new QAction(MainWindow);
		actionEditInstNotes->setObjectName(QStringLiteral("actionEditInstNotes"));
		actionEditInstance = new QAction(MainWindow);
		actionEditInstance->setObjectName(QStringLiteral("actionEditInstance"));
		actionViewSelectedInstFolder = new QAction(MainWindow);
		actionViewSelectedInstFolder->setObjectName(QStringLiteral("actionViewSelectedInstFolder"));
		actionDeleteInstance = new QAction(MainWindow);
		actionDeleteInstance->setObjectName(QStringLiteral("actionDeleteInstance"));
		actionConfig_Folder = new QAction(MainWindow);
		actionConfig_Folder->setObjectName(QStringLiteral("actionConfig_Folder"));
		actionCAT = new QAction(MainWindow);
		actionCAT->setObjectName(QStringLiteral("actionCAT"));
		actionCAT->setCheckable(true);
		actionCAT->setIcon(MMC->getThemedIcon("cat"));
		actionREDDIT = new QAction(MainWindow);
		actionREDDIT->setObjectName(QStringLiteral("actionREDDIT"));
		actionREDDIT->setIcon(MMC->getThemedIcon("reddit-alien"));
		actionDISCORD = new QAction(MainWindow);
		actionDISCORD->setObjectName(QStringLiteral("actionDISCORD"));
		actionDISCORD->setIcon(MMC->getThemedIcon("discord"));
		actionCopyInstance = new QAction(MainWindow);
		actionCopyInstance->setObjectName(QStringLiteral("actionCopyInstance"));
		actionCopyInstance->setIcon(MMC->getThemedIcon("copy"));
		actionManageAccounts = new QAction(MainWindow);
		actionManageAccounts->setObjectName(QStringLiteral("actionManageAccounts"));
		actionLaunchInstanceOffline = new QAction(MainWindow);
		actionLaunchInstanceOffline->setObjectName(QStringLiteral("actionLaunchInstanceOffline"));
		actionScreenshots = new QAction(MainWindow);
		actionScreenshots->setObjectName(QStringLiteral("actionScreenshots"));
		actionInstanceSettings = new QAction(MainWindow);
		actionInstanceSettings->setObjectName(QStringLiteral("actionInstanceSettings"));
		actionExportInstance = new QAction(MainWindow);
		actionExportInstance->setObjectName(QStringLiteral("actionExportInstance"));
		centralWidget = new QWidget(MainWindow);
		centralWidget->setObjectName(QStringLiteral("centralWidget"));
		horizontalLayout = new QHBoxLayout(centralWidget);
		horizontalLayout->setSpacing(0);
		horizontalLayout->setContentsMargins(11, 11, 11, 11);
		horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
		horizontalLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
		horizontalLayout->setContentsMargins(0, 0, 0, 0);
		MainWindow->setCentralWidget(centralWidget);
		mainToolBar = new QToolBar(MainWindow);
		mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
		mainToolBar->setMovable(false);
		mainToolBar->setAllowedAreas(Qt::TopToolBarArea);
		mainToolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
		mainToolBar->setFloatable(false);
		MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
		statusBar = new QStatusBar(MainWindow);
		statusBar->setObjectName(QStringLiteral("statusBar"));
		MainWindow->setStatusBar(statusBar);
		instanceToolBar = new QToolBar(MainWindow);
		instanceToolBar->setObjectName(QStringLiteral("instanceToolBar"));
		instanceToolBar->setEnabled(true);
		instanceToolBar->setAllowedAreas(Qt::LeftToolBarArea | Qt::RightToolBarArea);
		instanceToolBar->setIconSize(QSize(80, 80));
		instanceToolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
		instanceToolBar->setFloatable(false);
		MainWindow->addToolBar(Qt::RightToolBarArea, instanceToolBar);
		newsToolBar = new QToolBar(MainWindow);
		newsToolBar->setObjectName(QStringLiteral("newsToolBar"));
		newsToolBar->setMovable(false);
		newsToolBar->setAllowedAreas(Qt::BottomToolBarArea);
		newsToolBar->setIconSize(QSize(16, 16));
		newsToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
		newsToolBar->setFloatable(false);
		MainWindow->addToolBar(Qt::BottomToolBarArea, newsToolBar);

		mainToolBar->addAction(actionAddInstance);
		mainToolBar->addAction(actionCopyInstance);
		mainToolBar->addSeparator();
		mainToolBar->addAction(actionViewInstanceFolder);
		mainToolBar->addAction(actionViewCentralModsFolder);
		mainToolBar->addAction(actionRefresh);
		mainToolBar->addSeparator();
		if(BuildConfig.UPDATER_ENABLED)
		{
			mainToolBar->addAction(actionCheckUpdate);
		}
		mainToolBar->addAction(actionSettings);
		mainToolBar->addSeparator();
		mainToolBar->addAction(actionReportBug);
		mainToolBar->addAction(actionAbout);
		mainToolBar->addSeparator();
		mainToolBar->addAction(actionPatreon);
		mainToolBar->addAction(actionREDDIT);
		mainToolBar->addAction(actionDISCORD);
		mainToolBar->addAction(actionCAT);
		instanceToolBar->addAction(actionChangeInstIcon);
		instanceToolBar->addAction(actionLaunchInstance);
		instanceToolBar->addAction(actionLaunchInstanceOffline);
		instanceToolBar->addAction(actionChangeInstGroup);
		instanceToolBar->addSeparator();
		instanceToolBar->addAction(actionEditInstance);
		instanceToolBar->addAction(actionInstanceSettings);
		instanceToolBar->addAction(actionEditInstNotes);
		instanceToolBar->addAction(actionScreenshots);
		instanceToolBar->addSeparator();
		instanceToolBar->addAction(actionViewSelectedInstFolder);
		instanceToolBar->addAction(actionConfig_Folder);
		instanceToolBar->addSeparator();
		instanceToolBar->addAction(actionExportInstance);
		instanceToolBar->addAction(actionDeleteInstance);
		newsToolBar->addAction(actionMoreNews);

		retranslateUi(MainWindow);

		QMetaObject::connectSlotsByName(MainWindow);
	} // setupUi

	void retranslateUi(QMainWindow *MainWindow)
	{
		MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MultiMC 5", 0));
		actionAddInstance->setText(QApplication::translate("MainWindow", "Add Instance", 0));
		actionAddInstance->setToolTip(QApplication::translate("MainWindow", "Add a new instance.", 0));
		actionAddInstance->setStatusTip(QApplication::translate("MainWindow", "Add a new instance.", 0));
		actionViewInstanceFolder->setText(QApplication::translate("MainWindow", "View Instance Folder", 0));
		actionViewInstanceFolder->setToolTip(QApplication::translate("MainWindow", "Open the instance folder in a file browser.", 0));
		actionViewInstanceFolder->setStatusTip(QApplication::translate("MainWindow", "Open the instance folder in a file browser.", 0));
		actionRefresh->setText(QApplication::translate("MainWindow", "Refresh", 0));
		actionRefresh->setToolTip(QApplication::translate("MainWindow", "Reload the instance list.", 0));
		actionRefresh->setStatusTip(QApplication::translate("MainWindow", "Reload the instance list.", 0));
		actionViewCentralModsFolder->setText(QApplication::translate("MainWindow", "View Central Mods Folder", 0));
		actionViewCentralModsFolder->setToolTip(QApplication::translate("MainWindow", "Open the central mods folder in a file browser.", 0));
		actionViewCentralModsFolder->setStatusTip(QApplication::translate("MainWindow", "Open the central mods folder in a file browser.", 0));
		if(BuildConfig.UPDATER_ENABLED)
		{
			actionCheckUpdate->setText(QApplication::translate("MainWindow", "Check for Updates", 0));
			actionCheckUpdate->setToolTip(QApplication::translate("MainWindow", "Check for new updates for MultiMC", 0));
			actionCheckUpdate->setStatusTip(QApplication::translate("MainWindow", "Check for new updates for MultiMC", 0));
		}
		actionSettings->setText(QApplication::translate("MainWindow", "Settings", 0));
		actionSettings->setToolTip(QApplication::translate("MainWindow", "Change settings.", 0));
		actionSettings->setStatusTip(QApplication::translate("MainWindow", "Change settings.", 0));
		actionReportBug->setText(QApplication::translate("MainWindow", "Report a Bug", 0));
		actionReportBug->setToolTip(QApplication::translate("MainWindow", "Open the bug tracker to report a bug with MultiMC.", 0));
		actionReportBug->setStatusTip(QApplication::translate("MainWindow", "Open the bug tracker to report a bug with MultiMC.", 0));
		actionPatreon->setText(QApplication::translate("MainWindow", "Support us on Patreon!", 0));
		actionPatreon->setToolTip(QApplication::translate("MainWindow", "Open the MultiMC Patreon page.", 0));
		actionPatreon->setStatusTip(QApplication::translate("MainWindow", "Open the MultiMC Patreon page.", 0));
		actionMoreNews->setText(QApplication::translate("MainWindow", "More News", 0));
		actionMoreNews->setIconText(QApplication::translate("MainWindow", "More news...", 0));
		actionMoreNews->setToolTip(QApplication::translate("MainWindow", "Open the MultiMC development blog to read more news about MultiMC.", 0));
		actionMoreNews->setStatusTip(QApplication::translate("MainWindow", "Open the MultiMC development blog to read more news about MultiMC.", 0));
		actionAbout->setText(QApplication::translate("MainWindow", "About MultiMC", 0));
		actionAbout->setToolTip(QApplication::translate("MainWindow", "View information about MultiMC.", 0));
		actionAbout->setStatusTip(QApplication::translate("MainWindow", "About MultiMC", 0));
		actionLaunchInstance->setText(QApplication::translate("MainWindow", "Play", 0));
		actionLaunchInstance->setToolTip(QApplication::translate("MainWindow", "Launch the selected instance.", 0));
		actionLaunchInstance->setStatusTip(QApplication::translate("MainWindow", "Launch the selected instance.", 0));
		actionRenameInstance->setText(QApplication::translate("MainWindow", "Instance Name", 0));
		actionRenameInstance->setToolTip(QApplication::translate("MainWindow", "Rename the selected instance.", 0));
		actionRenameInstance->setStatusTip(QApplication::translate("MainWindow", "Rename the selected instance.", 0));
		actionChangeInstGroup->setText(QApplication::translate("MainWindow", "Change Group", 0));
		actionChangeInstGroup->setToolTip(QApplication::translate("MainWindow", "Change the selected instance's group.", 0));
		actionChangeInstGroup->setStatusTip(QApplication::translate("MainWindow", "Change the selected instance's group.", 0));
		actionChangeInstIcon->setText(QApplication::translate("MainWindow", "Change Icon", 0));
		actionChangeInstIcon->setToolTip(QApplication::translate("MainWindow", "Change the selected instance's icon.", 0));
		actionChangeInstIcon->setStatusTip(QApplication::translate("MainWindow", "Change the selected instance's icon.", 0));
		actionEditInstNotes->setText(QApplication::translate("MainWindow", "Edit Notes", 0));
		actionEditInstNotes->setToolTip(QApplication::translate("MainWindow", "Edit the notes for the selected instance.", 0));
		actionEditInstNotes->setStatusTip(QApplication::translate("MainWindow", "Edit the notes for the selected instance.", 0));
		actionEditInstance->setText(QApplication::translate("MainWindow", "Edit Instance", 0));
		actionEditInstance->setIconText(QApplication::translate("MainWindow", "Edit Instance", 0));
		actionEditInstance->setToolTip(QApplication::translate("MainWindow", "Change the instance settings, mods and versions.", 0));
		actionEditInstance->setStatusTip(QApplication::translate("MainWindow", "Change the instance settings, mods and versions.", 0));
		actionViewSelectedInstFolder->setText(QApplication::translate("MainWindow", "Instance Folder", 0));
		actionViewSelectedInstFolder->setToolTip(QApplication::translate("MainWindow", "Open the selected instance's root folder in a file browser.", 0));
		actionViewSelectedInstFolder->setStatusTip(QApplication::translate("MainWindow", "Open the selected instance's root folder in a file browser.", 0));
		actionDeleteInstance->setText(QApplication::translate("MainWindow", "Delete", 0));
		actionDeleteInstance->setToolTip(QApplication::translate("MainWindow", "Delete the selected instance.", 0));
		actionDeleteInstance->setStatusTip(QApplication::translate("MainWindow", "Delete the selected instance.", 0));
		actionConfig_Folder->setText(QApplication::translate("MainWindow", "Config Folder", 0));
		actionConfig_Folder->setToolTip(QApplication::translate("MainWindow", "Open the instance's config folder", 0));
		actionCAT->setText(QApplication::translate("MainWindow", "Meow", 0));
		actionCAT->setToolTip(QApplication::translate("MainWindow", "<html><head/><body><p align=\"center\">It's a fluffy kitty :3</p></body></html>", 0));
		actionREDDIT->setText(QApplication::translate("MainWindow", "Reddit", 0));
		actionREDDIT->setToolTip(QApplication::translate("MainWindow", "Open MultiMC subreddit", 0));
		actionDISCORD->setText(QApplication::translate("MainWindow", "Discord", 0));
		actionDISCORD->setToolTip(QApplication::translate("MainWindow", "Open MultiMC discord voice chat", 0));
		actionCopyInstance->setText(QApplication::translate("MainWindow", "Copy Instance", 0));
		actionCopyInstance->setToolTip(QApplication::translate("MainWindow", "Copy the selected instance.", 0));
		actionCopyInstance->setStatusTip(QApplication::translate("MainWindow", "Add a new instance.", 0));
		actionManageAccounts->setText(QApplication::translate("MainWindow", "Manage Accounts", 0));
		actionManageAccounts->setToolTip(QApplication::translate("MainWindow", "Manage your Mojang or Minecraft accounts.", 0));
		actionLaunchInstanceOffline->setText(QApplication::translate("MainWindow", "Play Offline", 0));
		actionLaunchInstanceOffline->setToolTip(QApplication::translate("MainWindow", "Launch the selected instance in offline mode.", 0));
		actionLaunchInstanceOffline->setStatusTip(QApplication::translate("MainWindow", "Launch the selected instance.", 0));
		actionScreenshots->setText(QApplication::translate("MainWindow", "Manage Screenshots", 0));
		actionScreenshots->setToolTip(
			QApplication::translate("MainWindow", "<html><head/><body><p>View and upload screenshots for this instance</p></body></html>", 0));
		actionInstanceSettings->setText(QApplication::translate("MainWindow", "Instance Settings", 0));
		actionInstanceSettings->setToolTip(QApplication::translate("MainWindow", "Change the settings specific to the instance", 0));
		actionExportInstance->setText(QApplication::translate("MainWindow", "Export Instance", 0));
		mainToolBar->setWindowTitle(QApplication::translate("MainWindow", "Main Toolbar", 0));
		instanceToolBar->setWindowTitle(QApplication::translate("MainWindow", "Instance Toolbar", 0));
		newsToolBar->setWindowTitle(QApplication::translate("MainWindow", "News Toolbar", 0));
	} // retranslateUi
};

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new MainWindow::Ui)
{
	ui->setupUi(this);

	QString winTitle = tr("MultiMC 5 - Version %1").arg(BuildConfig.printableVersionString());
	if (!BuildConfig.BUILD_PLATFORM.isEmpty())
	{
		winTitle += tr(" on %1", "on platform, as in operating system").arg(BuildConfig.BUILD_PLATFORM);
	}
	setWindowTitle(winTitle);

	// OSX magic.
	setUnifiedTitleAndToolBarOnMac(true);

	// Global shortcuts
	{
		// FIXME: This is kinda weird. and bad. We need some kind of managed shutdown.
		auto q = new QShortcut(QKeySequence::Quit, this);
		connect(q, SIGNAL(activated()), qApp, SLOT(quit()));
	}

	// The instance action toolbar customizations
	{
		// disabled until we have an instance selected
		ui->instanceToolBar->setEnabled(false);

		// the rename label is inside the rename tool button
		renameButton = new LabeledToolButton();
		renameButton->setText("Instance Name");
		renameButton->setToolTip(ui->actionRenameInstance->toolTip());
		connect(renameButton, SIGNAL(clicked(bool)), SLOT(on_actionRenameInstance_triggered()));
		ui->instanceToolBar->insertWidget(ui->actionLaunchInstance, renameButton);
		ui->instanceToolBar->insertSeparator(ui->actionLaunchInstance);
		renameButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	}

	// Add the news label to the news toolbar.
	{
		m_newsChecker.reset(new NewsChecker(BuildConfig.NEWS_RSS_URL));
		newsLabel = new QToolButton();
		newsLabel->setIcon(MMC->getThemedIcon("news"));
		newsLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
		newsLabel->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
		ui->newsToolBar->insertWidget(ui->actionMoreNews, newsLabel);
		QObject::connect(newsLabel, &QAbstractButton::clicked, this, &MainWindow::newsButtonClicked);
		QObject::connect(m_newsChecker.get(), &NewsChecker::newsLoaded, this, &MainWindow::updateNewsLabel);
		updateNewsLabel();
	}

	// Create the instance list widget
	{
		view = new GroupView(ui->centralWidget);

		view->setSelectionMode(QAbstractItemView::SingleSelection);
		view->setItemDelegate(new ListViewDelegate());
		view->setFrameShape(QFrame::NoFrame);
		// do not show ugly blue border on the mac
		view->setAttribute(Qt::WA_MacShowFocusRect, false);

		view->installEventFilter(this);
		view->setContextMenuPolicy(Qt::CustomContextMenu);
		connect(view, &QWidget::customContextMenuRequested, this, &MainWindow::showInstanceContextMenu);

		proxymodel = new InstanceProxyModel(this);
		proxymodel->setSourceModel(MMC->instances().get());
		proxymodel->sort(0);
		connect(proxymodel, &InstanceProxyModel::dataChanged, this, &MainWindow::instanceDataChanged);

		view->setModel(proxymodel);
		ui->horizontalLayout->addWidget(view);
	}
	// The cat background
	{
		bool cat_enable = MMC->settings()->get("TheCat").toBool();
		ui->actionCAT->setChecked(cat_enable);
		connect(ui->actionCAT, SIGNAL(toggled(bool)), SLOT(onCatToggled(bool)));
		setCatBackground(cat_enable);
	}
	// start instance when double-clicked
	connect(view, &GroupView::doubleClicked, this, &MainWindow::instanceActivated);

	// track the selection -- update the instance toolbar
	connect(view->selectionModel(), &QItemSelectionModel::currentChanged, this, &MainWindow::instanceChanged);

	// track icon changes and update the toolbar!
	connect(ENV.icons().get(), &IconList::iconUpdated, this, &MainWindow::iconUpdated);

	// model reset -> selection is invalid. All the instance pointers are wrong.
	connect(MMC->instances().get(), &InstanceList::dataIsInvalid, this, &MainWindow::selectionBad);

	m_statusLeft = new QLabel(tr("No instance selected"), this);
	m_statusRight = new ServerStatus(this);
	statusBar()->addPermanentWidget(m_statusLeft, 1);
	statusBar()->addPermanentWidget(m_statusRight, 0);

	// Add "manage accounts" button, right align
	QWidget *spacer = new QWidget();
	spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	ui->mainToolBar->addWidget(spacer);

	accountMenu = new QMenu(this);
	manageAccountsAction = new QAction(tr("Manage Accounts"), this);
	manageAccountsAction->setCheckable(false);
	manageAccountsAction->setIcon(MMC->getThemedIcon("accounts"));
	connect(manageAccountsAction, SIGNAL(triggered(bool)), this, SLOT(on_actionManageAccounts_triggered()));

	repopulateAccountsMenu();

	accountMenuButton = new QToolButton(this);
	accountMenuButton->setText(tr("Profiles"));
	accountMenuButton->setMenu(accountMenu);
	accountMenuButton->setPopupMode(QToolButton::InstantPopup);
	accountMenuButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	accountMenuButton->setIcon(MMC->getThemedIcon("noaccount"));

	QWidgetAction *accountMenuButtonAction = new QWidgetAction(this);
	accountMenuButtonAction->setDefaultWidget(accountMenuButton);

	ui->mainToolBar->addAction(accountMenuButtonAction);

	// Update the menu when the active account changes.
	// Shouldn't have to use lambdas here like this, but if I don't, the compiler throws a fit.
	// Template hell sucks...
	connect(MMC->accounts().get(), &MojangAccountList::activeAccountChanged, [this]
			{
				activeAccountChanged();
			});
	connect(MMC->accounts().get(), &MojangAccountList::listChanged, [this]
			{
				repopulateAccountsMenu();
			});

	// Show initial account
	activeAccountChanged();

	auto accounts = MMC->accounts();

	QList<CacheDownloadPtr> skin_dls;
	for (int i = 0; i < accounts->count(); i++)
	{
		auto account = accounts->at(i);
		if (!account)
		{
			qWarning() << "Null account at index" << i;
			continue;
		}
		for (auto profile : account->profiles())
		{
			auto meta = Env::getInstance().metacache()->resolveEntry("skins", profile.id + ".png");
			auto action = CacheDownload::make(QUrl("https://" + URLConstants::SKINS_BASE + profile.id + ".png"), meta);
			skin_dls.append(action);
			meta->stale = true;
		}
	}
	if (!skin_dls.isEmpty())
	{
		auto job = new NetJob("Startup player skins download");
		connect(job, &NetJob::succeeded, this, &MainWindow::skinJobFinished);
		connect(job, &NetJob::failed, this, &MainWindow::skinJobFinished);
		for (auto action : skin_dls)
		{
			job->addNetAction(action);
		}
		skin_download_job.reset(job);
		job->start();
	}

	// run the things that load and download other things... FIXME: this is NOT the place
	// FIXME: invisible actions in the background = NOPE.
	{
		if (!MMC->minecraftlist()->isLoaded())
		{
			m_versionLoadTask = MMC->minecraftlist()->getLoadTask();
			startTask(m_versionLoadTask);
		}
		if (!MMC->lwjgllist()->isLoaded())
		{
			MMC->lwjgllist()->loadList();
		}

		m_newsChecker->reloadNews();
		updateNewsLabel();
	}

	if(BuildConfig.UPDATER_ENABLED)
	{
		// set up the updater object.
		auto updater = MMC->updateChecker();
		connect(updater.get(), &UpdateChecker::updateAvailable, this, &MainWindow::updateAvailable);
		connect(updater.get(), &UpdateChecker::noUpdateFound, this, &MainWindow::updateNotAvailable);
		// if automatic update checks are allowed, start one.
		if (MMC->settings()->get("AutoUpdate").toBool())
		{
			updater->checkForUpdate(MMC->settings()->get("UpdateChannel").toString(), false);
		}
	}

	{
		auto checker = new NotificationChecker();
		checker->setNotificationsUrl(QUrl(BuildConfig.NOTIFICATION_URL));
		checker->setApplicationChannel(BuildConfig.VERSION_CHANNEL);
		checker->setApplicationPlatform(BuildConfig.BUILD_PLATFORM);
		checker->setApplicationFullVersion(BuildConfig.FULL_VERSION_STR);
		m_notificationChecker.reset(checker);
		connect(m_notificationChecker.get(), &NotificationChecker::notificationCheckFinished, this, &MainWindow::notificationsChanged);
		checker->checkForNotifications();
	}

	setSelectedInstanceById(MMC->settings()->get("SelectedInstance").toString());

	// removing this looks stupid
	view->setFocus();
}

MainWindow::~MainWindow()
{
}

void MainWindow::skinJobFinished()
{
	activeAccountChanged();
	skin_download_job.reset();
}

void MainWindow::showInstanceContextMenu(const QPoint &pos)
{
	QList<QAction *> actions;

	QAction *actionSep = new QAction("", this);
	actionSep->setSeparator(true);

	bool onInstance = view->indexAt(pos).isValid();
	if (onInstance)
	{
		actions = ui->instanceToolBar->actions();

		QAction *actionVoid = new QAction(m_selectedInstance->name(), this);
		actionVoid->setEnabled(false);

		QAction *actionRename = new QAction(tr("Rename"), this);
		actionRename->setToolTip(ui->actionRenameInstance->toolTip());

		QAction *actionCopyInstance = new QAction(tr("Copy instance"), this);
		actionCopyInstance->setToolTip(ui->actionCopyInstance->toolTip());

		connect(actionRename, SIGNAL(triggered(bool)), SLOT(on_actionRenameInstance_triggered()));
		connect(actionCopyInstance, SIGNAL(triggered(bool)), SLOT(on_actionCopyInstance_triggered()));

		actions.replace(1, actionRename);
		actions.prepend(actionSep);
		actions.prepend(actionVoid);
		actions.append(actionCopyInstance);
	}
	else
	{
		QAction *actionVoid = new QAction(tr("MultiMC"), this);
		actionVoid->setEnabled(false);

		QAction *actionCreateInstance = new QAction(tr("Create instance"), this);
		actionCreateInstance->setToolTip(ui->actionAddInstance->toolTip());

		connect(actionCreateInstance, SIGNAL(triggered(bool)), SLOT(on_actionAddInstance_triggered()));

		actions.prepend(actionSep);
		actions.prepend(actionVoid);
		actions.append(actionCreateInstance);
	}
	QMenu myMenu;
	myMenu.addActions(actions);
	if (onInstance)
		myMenu.setEnabled(m_selectedInstance->canLaunch());
	myMenu.exec(view->mapToGlobal(pos));
}

void MainWindow::updateToolsMenu()
{
	QMenu *launchMenu = ui->actionLaunchInstance->menu();
	if (launchMenu)
	{
		launchMenu->clear();
	}
	else
	{
		launchMenu = new QMenu(this);
	}

	QAction *normalLaunch = launchMenu->addAction(tr("Launch"));
	connect(normalLaunch, &QAction::triggered, [this]()
			{
				launch(m_selectedInstance);
			});
	launchMenu->addSeparator()->setText(tr("Profilers"));
	for (auto profiler : MMC->profilers().values())
	{
		QAction *profilerAction = launchMenu->addAction(profiler->name());
		QString error;
		if (!profiler->check(&error))
		{
			profilerAction->setDisabled(true);
			profilerAction->setToolTip(tr("Profiler not setup correctly. Go into settings, \"External Tools\"."));
		}
		else
		{
			connect(profilerAction, &QAction::triggered, [this, profiler]()
					{
						launch(m_selectedInstance, true, profiler.get());
					});
		}
	}
	launchMenu->addSeparator()->setText(tr("Tools"));
	for (auto tool : MMC->tools().values())
	{
		QAction *toolAction = launchMenu->addAction(tool->name());
		QString error;
		if (!tool->check(&error))
		{
			toolAction->setDisabled(true);
			toolAction->setToolTip(tr("Tool not setup correctly. Go into settings, \"External Tools\"."));
		}
		else
		{
			connect(toolAction, &QAction::triggered, [this, tool]()
					{
						tool->createDetachedTool(m_selectedInstance, this)->run();
					});
		}
	}
	ui->actionLaunchInstance->setMenu(launchMenu);
}

void MainWindow::repopulateAccountsMenu()
{
	accountMenu->clear();

	std::shared_ptr<MojangAccountList> accounts = MMC->accounts();
	MojangAccountPtr active_account = accounts->activeAccount();

	QString active_username = "";
	if (active_account != nullptr)
	{
		active_username = accounts->activeAccount()->username();
	}

	if (accounts->count() <= 0)
	{
		QAction *action = new QAction(tr("No accounts added!"), this);
		action->setEnabled(false);
		accountMenu->addAction(action);
	}
	else
	{
		// TODO: Nicer way to iterate?
		for (int i = 0; i < accounts->count(); i++)
		{
			MojangAccountPtr account = accounts->at(i);
			for (auto profile : account->profiles())
			{
				QAction *action = new QAction(profile.name, this);
				action->setData(account->username());
				action->setCheckable(true);
				if (active_username == account->username())
				{
					action->setChecked(true);
				}

				action->setIcon(SkinUtils::getFaceFromCache(profile.id));
				accountMenu->addAction(action);
				connect(action, SIGNAL(triggered(bool)), SLOT(changeActiveAccount()));
			}
		}
	}

	accountMenu->addSeparator();

	QAction *action = new QAction(tr("No Default Account"), this);
	action->setCheckable(true);
	action->setIcon(MMC->getThemedIcon("noaccount"));
	action->setData("");
	if (active_username.isEmpty())
	{
		action->setChecked(true);
	}

	accountMenu->addAction(action);
	connect(action, SIGNAL(triggered(bool)), SLOT(changeActiveAccount()));

	accountMenu->addSeparator();
	accountMenu->addAction(manageAccountsAction);
}

/*
 * Assumes the sender is a QAction
 */
void MainWindow::changeActiveAccount()
{
	QAction *sAction = (QAction *)sender();
	// Profile's associated Mojang username
	// Will need to change when profiles are properly implemented
	if (sAction->data().type() != QVariant::Type::String)
		return;

	QVariant data = sAction->data();
	QString id = "";
	if (!data.isNull())
	{
		id = data.toString();
	}

	MMC->accounts()->setActiveAccount(id);

	activeAccountChanged();
}

void MainWindow::activeAccountChanged()
{
	repopulateAccountsMenu();

	MojangAccountPtr account = MMC->accounts()->activeAccount();

	if (account != nullptr && account->username() != "")
	{
		const AccountProfile *profile = account->currentProfile();
		if (profile != nullptr)
		{
			accountMenuButton->setIcon(SkinUtils::getFaceFromCache(profile->id));
			accountMenuButton->setText(profile->name);
			return;
		}
	}

	// Set the icon to the "no account" icon.
	accountMenuButton->setIcon(MMC->getThemedIcon("noaccount"));
	accountMenuButton->setText(tr("Profiles"));
}

bool MainWindow::eventFilter(QObject *obj, QEvent *ev)
{
	if (obj == view)
	{
		if (ev->type() == QEvent::KeyPress)
		{
			QKeyEvent *keyEvent = static_cast<QKeyEvent *>(ev);
			switch (keyEvent->key())
			{
			case Qt::Key_Enter:
			case Qt::Key_Return:
				on_actionLaunchInstance_triggered();
				return true;
			case Qt::Key_Delete:
				on_actionDeleteInstance_triggered();
				return true;
			case Qt::Key_F5:
				on_actionRefresh_triggered();
				return true;
			case Qt::Key_F2:
				on_actionRenameInstance_triggered();
				return true;
			default:
				break;
			}
		}
	}
	return QMainWindow::eventFilter(obj, ev);
}

void MainWindow::updateNewsLabel()
{
	if (m_newsChecker->isLoadingNews())
	{
		newsLabel->setText(tr("Loading news..."));
		newsLabel->setEnabled(false);
	}
	else
	{
		QList<NewsEntryPtr> entries = m_newsChecker->getNewsEntries();
		if (entries.length() > 0)
		{
			newsLabel->setText(entries[0]->title);
			newsLabel->setEnabled(true);
		}
		else
		{
			newsLabel->setText(tr("No news available."));
			newsLabel->setEnabled(false);
		}
	}
}

void MainWindow::updateAvailable(GoUpdate::Status status)
{
	UpdateDialog dlg;
	UpdateAction action = (UpdateAction)dlg.exec();
	switch (action)
	{
	case UPDATE_LATER:
		qDebug() << "Update will be installed later.";
		break;
	case UPDATE_NOW:
		downloadUpdates(status);
		break;
	}
}

void MainWindow::updateNotAvailable()
{
	UpdateDialog dlg(false);
	dlg.exec();
}

QList<int> stringToIntList(const QString &string)
{
	QStringList split = string.split(',', QString::SkipEmptyParts);
	QList<int> out;
	for (int i = 0; i < split.size(); ++i)
	{
		out.append(split.at(i).toInt());
	}
	return out;
}
QString intListToString(const QList<int> &list)
{
	QStringList slist;
	for (int i = 0; i < list.size(); ++i)
	{
		slist.append(QString::number(list.at(i)));
	}
	return slist.join(',');
}
void MainWindow::notificationsChanged()
{
	QList<NotificationChecker::NotificationEntry> entries = m_notificationChecker->notificationEntries();
	QList<int> shownNotifications = stringToIntList(MMC->settings()->get("ShownNotifications").toString());
	for (auto it = entries.begin(); it != entries.end(); ++it)
	{
		NotificationChecker::NotificationEntry entry = *it;
		if (!shownNotifications.contains(entry.id))
		{
			NotificationDialog dialog(entry, this);
			if (dialog.exec() == NotificationDialog::DontShowAgain)
			{
				shownNotifications.append(entry.id);
			}
		}
	}
	MMC->settings()->set("ShownNotifications", intListToString(shownNotifications));
}

void MainWindow::downloadUpdates(GoUpdate::Status status)
{
	qDebug() << "Downloading updates.";
	ProgressDialog updateDlg(this);
	status.rootPath = MMC->rootPath;

	auto dlPath = FS::PathCombine(MMC->root(), "update", "XXXXXX");
	if (!FS::ensureFilePathExists(dlPath))
	{
		CustomMessageBox::selectable(this, tr("Error"), tr("Couldn't create folder for update downloads:\n%1").arg(dlPath), QMessageBox::Warning)->show();
	}
	GoUpdate::DownloadTask updateTask(status, dlPath, &updateDlg);
	// If the task succeeds, install the updates.
	if (updateDlg.execWithTask(&updateTask))
	{
		MMC->installUpdates(updateTask.updateFilesDir(), updateTask.operations());
	}
	else
	{
		CustomMessageBox::selectable(this, tr("Error"), updateTask.failReason(), QMessageBox::Warning)->show();
	}
}

void MainWindow::onCatToggled(bool state)
{
	setCatBackground(state);
	MMC->settings()->set("TheCat", state);
}

void MainWindow::setCatBackground(bool enabled)
{
	if (enabled)
	{
		view->setStyleSheet("GroupView"
							"{"
							"background-image: url(:/backgrounds/kitteh);"
							"background-attachment: fixed;"
							"background-clip: padding;"
							"background-position: top right;"
							"background-repeat: none;"
							"background-color:palette(base);"
							"}");
	}
	else
	{
		view->setStyleSheet(QString());
	}
}

static QFileInfo findRecursive(const QString &dir, const QString &name)
{
	for (const auto info : QDir(dir).entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files, QDir::DirsLast))
	{
		if (info.isFile() && info.fileName() == name)
		{
			return info;
		}
		else if (info.isDir())
		{
			const QFileInfo res = findRecursive(info.absoluteFilePath(), name);
			if (res.isFile() && res.exists())
			{
				return res;
			}
		}
	}
	return QFileInfo();
}

// FIXME: eliminate, should not be needed
void MainWindow::waitForMinecraftVersions()
{
	if (!MMC->minecraftlist()->isLoaded() && m_versionLoadTask && m_versionLoadTask->isRunning())
	{
		QEventLoop waitLoop;
		waitLoop.connect(m_versionLoadTask, SIGNAL(failed(QString)), SLOT(quit()));
		waitLoop.connect(m_versionLoadTask, SIGNAL(succeeded()), SLOT(quit()));
		waitLoop.exec();
	}
}

InstancePtr MainWindow::instanceFromZipPack(QString instName, QString instGroup, QString instIcon, QUrl url)
{
	InstancePtr newInstance;

	QString instancesDir = MMC->settings()->get("InstanceDir").toString();
	QString instDirName = FS::DirNameFromString(instName, instancesDir);
	QString instDir = FS::PathCombine(instancesDir, instDirName);

	QString archivePath;
	if (url.isLocalFile())
	{
		archivePath = url.toLocalFile();
	}
	else
	{
		const QString path = url.host() + '/' + url.path();
		auto entry = ENV.metacache()->resolveEntry("general", path);
		entry->stale = true;
		CacheDownloadPtr dl = CacheDownload::make(url, entry);
		NetJob job(tr("Modpack download"));
		job.addNetAction(dl);

		// FIXME: possibly causes endless loop problems
		ProgressDialog dlDialog(this);
		job.setStatus(tr("Downloading modpack:\n%1").arg(url.toString()));
		if (dlDialog.execWithTask(&job) != QDialog::Accepted)
		{
			return nullptr;
		}
		archivePath = entry->getFullPath();
	}

	QTemporaryDir extractTmpDir;
	QDir extractDir(extractTmpDir.path());
	qDebug() << "Attempting to create instance from" << archivePath;
	if (MMCZip::extractDir(archivePath, extractDir.absolutePath()).isEmpty())
	{
		CustomMessageBox::selectable(this, tr("Error"), tr("Failed to extract modpack"), QMessageBox::Warning)->show();
		return nullptr;
	}
	const QFileInfo instanceCfgFile = findRecursive(extractDir.absolutePath(), "instance.cfg");
	if (!instanceCfgFile.isFile() || !instanceCfgFile.exists())
	{
		CustomMessageBox::selectable(this, tr("Error"), tr("Archive does not contain instance.cfg"))->show();
		return nullptr;
	}
	if (!FS::copy(instanceCfgFile.absoluteDir().absolutePath(), instDir)())
	{
		CustomMessageBox::selectable(this, tr("Error"), tr("Unable to copy instance"))->show();
		return nullptr;
	}

	auto error = MMC->instances()->loadInstance(newInstance, instDir);
	QString errorMsg = tr("Failed to load instance %1: ").arg(instDirName);
	switch (error)
	{
	case InstanceList::UnknownLoadError:
		errorMsg += tr("Unkown error");
		CustomMessageBox::selectable(this, tr("Error"), errorMsg, QMessageBox::Warning)->show();
		return nullptr;
	case InstanceList::NotAnInstance:
		errorMsg += tr("Not an instance");
		CustomMessageBox::selectable(this, tr("Error"), errorMsg, QMessageBox::Warning)->show();
		return nullptr;
	default:
		break;
	}

	newInstance->setName(instName);
	if (instIcon != "default")
	{
		newInstance->setIconKey(instIcon);
	}
	else
	{
		instIcon = newInstance->iconKey();
		auto importIconPath = FS::PathCombine(newInstance->instanceRoot(), instIcon + ".png");
		if (QFile::exists(importIconPath))
		{
			// import icon
			auto iconList = ENV.icons();
			// FIXME: check if the file is OK before removing the existing one...
			if (iconList->iconFileExists(instIcon))
			{
				// FIXME: ask if icon should be overwritten. Show difference in the question dialog.
				iconList->deleteIcon(instIcon);
			}
			iconList->installIcons({importIconPath});
		}
	}
	newInstance->setGroupInitial(instGroup);
	// reset time played on import... because packs.
	newInstance->resetTimePlayed();
	MMC->instances()->add(InstancePtr(newInstance));
	MMC->instances()->saveGroupList();

	finalizeInstance(newInstance);
	return newInstance;
}

InstancePtr MainWindow::instanceFromVersion(QString instName, QString instGroup, QString instIcon, BaseVersionPtr version)
{
	InstancePtr newInstance;

	QString instancesDir = MMC->settings()->get("InstanceDir").toString();
	QString instDirName = FS::DirNameFromString(instName, instancesDir);
	QString instDir = FS::PathCombine(instancesDir, instDirName);
	auto error = MMC->instances()->createInstance(newInstance, version, instDir);
	QString errorMsg = tr("Failed to create instance %1: ").arg(instDirName);
	switch (error)
	{
	case InstanceList::NoCreateError:
		break;

	case InstanceList::InstExists:
	{
		errorMsg += tr("An instance with the given directory name already exists.");
		CustomMessageBox::selectable(this, tr("Error"), errorMsg, QMessageBox::Warning)->show();
		return nullptr;
	}

	case InstanceList::CantCreateDir:
	{
		errorMsg += tr("Failed to create the instance directory.");
		CustomMessageBox::selectable(this, tr("Error"), errorMsg, QMessageBox::Warning)->show();
		return nullptr;
	}

	default:
	{
		errorMsg += tr("Unknown instance loader error %1").arg(error);
		CustomMessageBox::selectable(this, tr("Error"), errorMsg, QMessageBox::Warning)->show();
		return nullptr;
	}
	}
	newInstance->setName(instName);
	newInstance->setIconKey(instIcon);
	newInstance->setGroupInitial(instGroup);
	MMC->instances()->add(InstancePtr(newInstance));
	MMC->instances()->saveGroupList();
	finalizeInstance(newInstance);
	return newInstance;
}

void MainWindow::finalizeInstance(InstancePtr inst)
{
	view->updateGeometries();
	setSelectedInstanceById(inst->id());
	if (MMC->accounts()->anyAccountIsValid())
	{
		ProgressDialog loadDialog(this);
		auto update = inst->createUpdateTask();
		connect(update.get(), &Task::failed, [this](QString reason)
				{
					QString error = QString("Instance load failed: %1").arg(reason);
					CustomMessageBox::selectable(this, tr("Error"), error, QMessageBox::Warning)->show();
				});
		loadDialog.execWithTask(update.get());
	}
	else
	{
		CustomMessageBox::selectable(this, tr("Error"), tr("MultiMC cannot download Minecraft or update instances unless you have at least "
														   "one account added.\nPlease add your Mojang or Minecraft account."),
									 QMessageBox::Warning)
			->show();
	}
}

void MainWindow::on_actionAddInstance_triggered()
{
	waitForMinecraftVersions();

	NewInstanceDialog newInstDlg(this);
	if (!newInstDlg.exec())
		return;

	MMC->settings()->set("LastUsedGroupForNewInstance", newInstDlg.instGroup());

	const QUrl modpackUrl = newInstDlg.modpackUrl();

	if (modpackUrl.isValid())
	{
		instanceFromZipPack(newInstDlg.instName(), newInstDlg.instGroup(), newInstDlg.iconKey(), modpackUrl);
	}
	else
	{
		instanceFromVersion(newInstDlg.instName(), newInstDlg.instGroup(), newInstDlg.iconKey(), newInstDlg.selectedVersion());
	}
}

void MainWindow::on_actionREDDIT_triggered()
{
	DesktopServices::openUrl(QUrl("https://www.reddit.com/r/MultiMC/"));
}

void MainWindow::on_actionDISCORD_triggered()
{
	DesktopServices::openUrl(QUrl("https://discord.gg/0k2zsXGNHs0fE4Wm"));
}

void MainWindow::on_actionCopyInstance_triggered()
{
	if (!m_selectedInstance)
		return;

	CopyInstanceDialog copyInstDlg(m_selectedInstance, this);
	if (!copyInstDlg.exec())
		return;

	QString instancesDir = MMC->settings()->get("InstanceDir").toString();
	QString instDirName = FS::DirNameFromString(copyInstDlg.instName(), instancesDir);
	QString instDir = FS::PathCombine(instancesDir, instDirName);
	bool copySaves = copyInstDlg.shouldCopySaves();

	InstancePtr newInstance;
	auto error = MMC->instances()->copyInstance(newInstance, m_selectedInstance, instDir, copySaves);

	QString errorMsg = tr("Failed to create instance %1: ").arg(instDirName);
	switch (error)
	{
	case InstanceList::NoCreateError:
		newInstance->setName(copyInstDlg.instName());
		newInstance->setIconKey(copyInstDlg.iconKey());
		MMC->instances()->add(newInstance);
		newInstance->setGroupPost(copyInstDlg.instGroup());
		return;

	case InstanceList::InstExists:
	{
		errorMsg += tr("An instance with the given directory name already exists.");
		CustomMessageBox::selectable(this, tr("Error"), errorMsg, QMessageBox::Warning)->show();
		break;
	}

	case InstanceList::CantCreateDir:
	{
		errorMsg += tr("Failed to create the instance directory.");
		CustomMessageBox::selectable(this, tr("Error"), errorMsg, QMessageBox::Warning)->show();
		break;
	}

	default:
	{
		errorMsg += tr("Unknown instance loader error %1").arg(error);
		CustomMessageBox::selectable(this, tr("Error"), errorMsg, QMessageBox::Warning)->show();
		break;
	}
	}
}

void MainWindow::on_actionChangeInstIcon_triggered()
{
	if (!m_selectedInstance)
		return;

	IconPickerDialog dlg(this);
	dlg.execWithSelection(m_selectedInstance->iconKey());
	if (dlg.result() == QDialog::Accepted)
	{
		m_selectedInstance->setIconKey(dlg.selectedIconKey);
		auto ico = ENV.icons()->getBigIcon(dlg.selectedIconKey);
		ui->actionChangeInstIcon->setIcon(ico);
	}
}

void MainWindow::iconUpdated(QString icon)
{
	if (icon == m_currentInstIcon)
	{
		ui->actionChangeInstIcon->setIcon(ENV.icons()->getBigIcon(m_currentInstIcon));
	}
}

void MainWindow::updateInstanceToolIcon(QString new_icon)
{
	m_currentInstIcon = new_icon;
	ui->actionChangeInstIcon->setIcon(ENV.icons()->getBigIcon(m_currentInstIcon));
}

void MainWindow::setSelectedInstanceById(const QString &id)
{
	if (id.isNull())
		return;
	const QModelIndex index = MMC->instances()->getInstanceIndexById(id);
	if (index.isValid())
	{
		QModelIndex selectionIndex = proxymodel->mapFromSource(index);
		view->selectionModel()->setCurrentIndex(selectionIndex, QItemSelectionModel::ClearAndSelect);
	}
}

void MainWindow::on_actionChangeInstGroup_triggered()
{
	if (!m_selectedInstance)
		return;

	bool ok = false;
	QString name(m_selectedInstance->group());
	auto groups = MMC->instances()->getGroups();
	groups.insert(0, "");
	groups.sort(Qt::CaseInsensitive);
	int foo = groups.indexOf(name);

	name = QInputDialog::getItem(this, tr("Group name"), tr("Enter a new group name."), groups, foo, true, &ok);
	name = name.simplified();
	if (ok)
		m_selectedInstance->setGroupPost(name);
}

void MainWindow::on_actionViewInstanceFolder_triggered()
{
	QString str = MMC->settings()->get("InstanceDir").toString();
	DesktopServices::openDirectory(str);
}

void MainWindow::on_actionRefresh_triggered()
{
	MMC->instances()->loadList();
}

void MainWindow::on_actionViewCentralModsFolder_triggered()
{
	DesktopServices::openDirectory(MMC->settings()->get("CentralModsDir").toString(), true);
}

void MainWindow::on_actionConfig_Folder_triggered()
{
	if (m_selectedInstance)
	{
		QString str = m_selectedInstance->instanceConfigFolder();
		DesktopServices::openDirectory(QDir(str).absolutePath());
	}
}

void MainWindow::on_actionCheckUpdate_triggered()
{
	if(BuildConfig.UPDATER_ENABLED)
	{
		auto updater = MMC->updateChecker();
		updater->checkForUpdate(MMC->settings()->get("UpdateChannel").toString(), true);
	}
	else
	{
		qWarning() << "Updater not set up. Cannot check for updates.";
	}
}

void MainWindow::on_actionSettings_triggered()
{
	SettingsUI::ShowPageDialog(MMC->globalSettingsPages(), this, "global-settings");
	// FIXME: quick HACK to make this work. improve, optimize.
	proxymodel->invalidate();
	proxymodel->sort(0);
	updateToolsMenu();
	update();
}

void MainWindow::on_actionInstanceSettings_triggered()
{
	SettingsUI::ShowInstancePageDialog(m_selectedInstance, this, "settings");
}

void MainWindow::on_actionEditInstNotes_triggered()
{
	SettingsUI::ShowInstancePageDialog(m_selectedInstance, this, "notes");
}

void MainWindow::on_actionEditInstance_triggered()
{
	SettingsUI::ShowInstancePageDialog(m_selectedInstance, this);
}

void MainWindow::on_actionScreenshots_triggered()
{
	SettingsUI::ShowInstancePageDialog(m_selectedInstance, this, "screenshots");
}

void MainWindow::on_actionManageAccounts_triggered()
{
	SettingsUI::ShowPageDialog(MMC->globalSettingsPages(), this, "accounts");
}

void MainWindow::on_actionReportBug_triggered()
{
	DesktopServices::openUrl(QUrl("https://github.com/MultiMC/MultiMC5/issues"));
}

void MainWindow::on_actionPatreon_triggered()
{
	DesktopServices::openUrl(QUrl("http://www.patreon.com/multimc"));
}

void MainWindow::on_actionMoreNews_triggered()
{
	DesktopServices::openUrl(QUrl("http://multimc.org/posts.html"));
}

void MainWindow::newsButtonClicked()
{
	QList<NewsEntryPtr> entries = m_newsChecker->getNewsEntries();
	if (entries.count() > 0)
	{
		DesktopServices::openUrl(QUrl(entries[0]->link));
	}
	else
	{
		DesktopServices::openUrl(QUrl("http://multimc.org/posts.html"));
	}
}

void MainWindow::on_actionAbout_triggered()
{
	AboutDialog dialog(this);
	dialog.exec();
}

void MainWindow::on_mainToolBar_visibilityChanged(bool)
{
	// Don't allow hiding the main toolbar.
	// This is the only way I could find to prevent it... :/
	ui->mainToolBar->setVisible(true);
}

void MainWindow::on_actionDeleteInstance_triggered()
{
	if (!m_selectedInstance)
	{
		return;
	}
	auto response = CustomMessageBox::selectable(this, tr("CAREFUL!"), tr("About to delete: %1\nThis is permanent and will completely erase "
																		  "all data, even for tracked instances!\nAre you sure?")
																		   .arg(m_selectedInstance->name()),
												 QMessageBox::Warning, QMessageBox::Yes | QMessageBox::No)
						->exec();
	if (response == QMessageBox::Yes)
	{
		m_selectedInstance->nuke();
	}
}

void MainWindow::on_actionExportInstance_triggered()
{
	if (m_selectedInstance)
	{
		ExportInstanceDialog dlg(m_selectedInstance, this);
		dlg.exec();
	}
}

void MainWindow::on_actionRenameInstance_triggered()
{
	if (m_selectedInstance)
	{
		bool ok = false;
		QString name(m_selectedInstance->name());
		name = QInputDialog::getText(this, tr("Instance name"), tr("Enter a new instance name."), QLineEdit::Normal, name, &ok);

		if (name.length() > 0)
		{
			if (ok && name.length())
			{
				m_selectedInstance->setName(name);
				renameButton->setText(name);
			}
		}
	}
}

void MainWindow::on_actionViewSelectedInstFolder_triggered()
{
	if (m_selectedInstance)
	{
		QString str = m_selectedInstance->instanceRoot();
		DesktopServices::openDirectory(QDir(str).absolutePath());
	}
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	// Save the window state and geometry.

	MMC->settings()->set("MainWindowState", saveState().toBase64());
	MMC->settings()->set("MainWindowGeometry", saveGeometry().toBase64());

	QMainWindow::closeEvent(event);
	QApplication::exit();
}

void MainWindow::instanceActivated(QModelIndex index)
{
	if (!index.isValid())
		return;
	QString id = index.data(InstanceList::InstanceIDRole).toString();
	InstancePtr inst = MMC->instances()->getInstanceById(id);
	if (!inst)
		return;

	launch(inst);
}

void MainWindow::on_actionLaunchInstance_triggered()
{
	if (m_selectedInstance)
	{
		launch(m_selectedInstance);
	}
}

void MainWindow::on_actionLaunchInstanceOffline_triggered()
{
	if (m_selectedInstance)
	{
		launch(m_selectedInstance, false);
	}
}

void MainWindow::launch(InstancePtr instance, bool online, BaseProfilerFactory *profiler)
{
	m_launchController.reset(new LaunchController());
	m_launchController->setInstance(instance);
	m_launchController->setOnline(online);
	m_launchController->setParentWidget(this);
	m_launchController->setProfiler(profiler);
	m_launchController->start();
}

void MainWindow::taskEnd()
{
	QObject *sender = QObject::sender();
	if (sender == m_versionLoadTask)
		m_versionLoadTask = NULL;

	sender->deleteLater();
}

void MainWindow::startTask(Task *task)
{
	connect(task, SIGNAL(succeeded()), SLOT(taskEnd()));
	connect(task, SIGNAL(failed(QString)), SLOT(taskEnd()));
	task->start();
}

void MainWindow::instanceChanged(const QModelIndex &current, const QModelIndex &previous)
{
	if (!current.isValid())
	{
		MMC->settings()->set("SelectedInstance", QString());
		selectionBad();
		return;
	}
	QString id = current.data(InstanceList::InstanceIDRole).toString();
	m_selectedInstance = MMC->instances()->getInstanceById(id);
	if (m_selectedInstance)
	{
		ui->instanceToolBar->setEnabled(m_selectedInstance->canLaunch());
		renameButton->setText(m_selectedInstance->name());
		m_statusLeft->setText(m_selectedInstance->getStatusbarDescription());
		updateInstanceToolIcon(m_selectedInstance->iconKey());

		updateToolsMenu();

		MMC->settings()->set("SelectedInstance", m_selectedInstance->id());
	}
	else
	{
		MMC->settings()->set("SelectedInstance", QString());
		selectionBad();
		return;
	}
}

void MainWindow::instanceDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
	auto current = view->selectionModel()->currentIndex();
	QItemSelection test(topLeft, bottomRight);
	if (test.contains(current))
	{
		instanceChanged(current, current);
	}
}

void MainWindow::selectionBad()
{
	// start by reseting everything...
	m_selectedInstance = nullptr;

	statusBar()->clearMessage();
	ui->instanceToolBar->setEnabled(false);
	renameButton->setText(tr("Rename Instance"));
	updateInstanceToolIcon("infinity");

	// ...and then see if we can enable the previously selected instance
	setSelectedInstanceById(MMC->settings()->get("SelectedInstance").toString());
}

void MainWindow::checkSetDefaultJava()
{
	const QString javaHack = "IntelHack";
	bool askForJava = false;
	do
	{
		QString currentHostName = QHostInfo::localHostName();
		QString oldHostName = MMC->settings()->get("LastHostname").toString();
		if (currentHostName != oldHostName)
		{
			MMC->settings()->set("LastHostname", currentHostName);
			askForJava = true;
			break;
		}
		QString currentJavaPath = MMC->settings()->get("JavaPath").toString();
		QString actualPath = FS::ResolveExecutable(currentJavaPath);
		if (currentJavaPath.isNull())
		{
			askForJava = true;
			break;
		}
#if defined Q_OS_WIN32
		QString currentHack = MMC->settings()->get("JavaDetectionHack").toString();
		if (currentHack != javaHack)
		{
			CustomMessageBox::selectable(this, tr("Java detection forced"), tr("Because of graphics performance issues caused by Intel drivers on Windows, "
																			   "MultiMC java detection was forced. Please select a Java "
																			   "version.<br/><br/>If you have custom java versions set for your instances, "
																			   "make sure you use the 'javaw.exe' executable."),
										 QMessageBox::Warning)
				->exec();
			askForJava = true;
			break;
		}
#endif
	} while (0);

	if (askForJava)
	{
		qDebug() << "Java path needs resetting, showing Java selection dialog...";

		JavaInstallPtr java;

		VersionSelectDialog vselect(MMC->javalist().get(), tr("Select a Java version"), this, false);
		vselect.setResizeOn(2);
		vselect.exec();

		if (vselect.selectedVersion())
			java = std::dynamic_pointer_cast<JavaInstall>(vselect.selectedVersion());
		else
		{
			CustomMessageBox::selectable(this, tr("Invalid version selected"), tr("You didn't select a valid Java version, so MultiMC will "
																				  "select the default. "
																				  "You can change this in the settings dialog."),
										 QMessageBox::Warning)
				->show();

			JavaUtils ju;
			java = ju.GetDefaultJava();
		}
		if (java)
		{
			MMC->settings()->set("JavaPath", java->path);
			MMC->settings()->set("JavaDetectionHack", javaHack);
		}
		else
			MMC->settings()->set("JavaPath", QString("java"));
	}
}

void MainWindow::checkInstancePathForProblems()
{
	QString instanceFolder = MMC->settings()->get("InstanceDir").toString();
	if (FS::checkProblemticPathJava(QDir(instanceFolder)))
	{
		QMessageBox warning(this);
		warning.setText(tr("Your instance folder contains \'!\' and this is known to cause Java problems!"));
		warning.setInformativeText(tr("You have now two options: <br/>"
									  " - change the instance folder in the settings <br/>"
									  " - move this installation of MultiMC5 to a different folder"));
		warning.setDefaultButton(QMessageBox::Ok);
		warning.exec();
	}
	auto tempFolderText = tr("This is a problem: <br/>"
							 " - MultiMC will likely be deleted without warning by the operating system <br/>"
							 " - close MultiMC now and extract it to a real location, not a temporary folder");
	QString pathfoldername = QDir(instanceFolder).absolutePath();
	if (pathfoldername.contains("Rar$", Qt::CaseInsensitive))
	{
		QMessageBox warning(this);
		warning.setText(tr("Your instance folder contains \'Rar$\' - that means you haven't extracted the MultiMC zip!"));
		warning.setInformativeText(tempFolderText);
		warning.setDefaultButton(QMessageBox::Ok);
		warning.exec();
	}
	else if (pathfoldername.contains(QDir::tempPath()))
	{
		QMessageBox warning(this);
		warning.setText(tr("Your instance folder is in a temporary folder: \'%1\'!").arg(QDir::tempPath()));
		warning.setInformativeText(tempFolderText);
		warning.setDefaultButton(QMessageBox::Ok);
		warning.exec();
	}
}
