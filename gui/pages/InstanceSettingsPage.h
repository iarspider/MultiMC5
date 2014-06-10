/* Copyright 2014 MultiMC Contributors
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

#pragma once
#include <QWidget>

#include <logic/OneSixInstance.h>
#include <logic/net/NetJob.h>
#include <logic/java/JavaChecker.h>
#include "BasePage.h"

class JavaChecker;
namespace Ui
{
class InstanceSettingsPage;
}

class InstanceSettingsPage : public QWidget, public BasePage
{
	Q_OBJECT

public:
	explicit InstanceSettingsPage(SettingsObject *s, QWidget *parent = 0);
	virtual ~InstanceSettingsPage();
	virtual QString displayName() override;
	virtual QIcon icon() override;
	virtual QString id() override;
	virtual bool apply();
	virtual QString helpPage() override { return "Instance-settings"; };
private:
	void updateCheckboxStuff();
private slots:
	void on_javaDetectBtn_clicked();

	void on_javaTestBtn_clicked();

	void on_javaBrowseBtn_clicked();

	void checkFinished(JavaCheckResult result);

	void applySettings();
	void loadSettings();
private:
	Ui::InstanceSettingsPage *ui;
	SettingsObject *m_settings;
	std::shared_ptr<JavaChecker> checker;
};
