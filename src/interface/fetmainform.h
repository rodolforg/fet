//
//
// Description: This file is part of FET
//
//
// Author: Lalescu Liviu <Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)>
// Copyright (C) 2003 Liviu Lalescu <https://lalescu.ro/liviu/>
//
/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef FETMAINFORM_H
#define FETMAINFORM_H

#include "ui_fetmainform_template.h"

#include <QString>
#include <QStringList>

#include <QAction>

#include <QMap>

class QNetworkAccessManager;
class QNetworkReply;

class FetMainForm: public QMainWindow, public Ui::FetMainForm_template
{
	Q_OBJECT
	
private:
	static const int MAX_RECENT_FILES=10;

	QSize originalWindowSize;
	QRect windowSettingsRect;

	QMenu* shortcutBasicMenu;
	QMenu* shortcutDataSpaceMenu;
	QMenu* shortcutDataAdvancedMenu;
	QMenu* shortcutAdvancedTimeMenu;
	//2014-07-01
	QMenu* shortcutTimetableLockingMenu;
	QMenu* shortcutTimetableAdvancedMenu;
	
	QNetworkAccessManager* networkManager;
	
	QStringList recentFiles;
	
	QAction* recentFileActions[MAX_RECENT_FILES];
	QAction* recentSeparatorAction;
	
	void setEnabledIcon(QAction* action, bool enabled);
	
	void setCurrentFile(const QString& fileName);
	QString strippedName(const QString& fullFileName);
	void updateRecentFileActions();

	void populateLanguagesMap(QMap<QString, QString>& languagesMap);

	bool isValidFilepathForSaving(const QString &filepath);

public:
	FetMainForm();
	~FetMainForm();
	
	void closeOtherWindows();
	
	void openFile(const QString& fileName);
	bool fileSave();
	bool fileSaveAs();

	void resetSettings();
	void loadSettings();
	void saveSettings();

public slots:
	void enableNotPerfectMessage();

	void on_fileNewAction_triggered();
	void on_fileSaveAction_triggered();
	void on_fileSaveAsAction_triggered();
	void on_fileExitAction_triggered();
	void on_fileOpenAction_triggered();
	void on_fileClearRecentFilesListAction_triggered();
	void openRecentFile();

	void on_fileImportCSVActivityTagsAction_triggered();
	void on_fileImportCSVActivitiesAction_triggered();
	void on_fileImportCSVRoomsBuildingsAction_triggered();
	void on_fileImportCSVSubjectsAction_triggered();
	void on_fileImportCSVTeachersAction_triggered();
	void on_fileImportCSVYearsGroupsSubgroupsAction_triggered();
	void on_fileExportCSVAction_triggered();
	
	void on_dataInstitutionNameAction_triggered();
	void on_dataCommentsAction_triggered();
	void on_dataDaysAction_triggered();
	void on_dataHoursAction_triggered();
	void on_dataTeachersAction_triggered();
	void on_dataTeachersStatisticsAction_triggered();
	void on_dataSubjectsAction_triggered();
	void on_dataSubjectsStatisticsAction_triggered();
	void on_dataActivityTagsAction_triggered();
	void on_dataYearsAction_triggered();
	void on_dataGroupsAction_triggered();
	void on_dataSubgroupsAction_triggered();
	void on_dataStudentsStatisticsAction_triggered();
	void on_dataActivitiesRoomsStatisticsAction_triggered();
	void on_dataTeachersSubjectsQualificationsStatisticsAction_triggered();
	void on_dataHelpOnStatisticsAction_triggered();
	
	void checkForUpdatesToggled(bool checked);
	void on_helpSettingsAction_triggered();
	void on_settingsUseColorsAction_toggled();
	void showSubgroupsInComboBoxesToggled(bool checked);
	void showSubgroupsInActivityPlanningToggled(bool checked);
	void on_settingsShowShortcutsOnMainWindowAction_toggled(bool checked);
	void on_settingsShowToolTipsForConstraintsWithTablesAction_toggled();
	
	void on_timetablesToWriteOnDiskAction_triggered();
	
	void on_studentsComboBoxesStyleAction_triggered();
	///

	void showWarningForSubgroupsWithTheSameActivitiesToggled(bool checked);
	
	void enableActivityTagMaxHoursDailyToggled(bool checked);
	void enableStudentsMaxGapsPerDayToggled(bool checked);
	void showWarningForNotPerfectConstraintsToggled(bool checked);

	void enableStudentsMinHoursDailyWithAllowEmptyDaysToggled(bool checked);
	void showWarningForStudentsMinHoursDailyWithAllowEmptyDaysToggled(bool checked);
	
	void enableGroupActivitiesInInitialOrderToggled(bool checked);
	void showWarningForGroupActivitiesInInitialOrderToggled(bool checked);
	
	void on_groupActivitiesInInitialOrderAction_triggered();
	
	void on_dataActivitiesAction_triggered();
	void on_dataSubactivitiesAction_triggered();
	void on_dataRoomsAction_triggered();
	void on_dataBuildingsAction_triggered();

	void openConstraintDialog(int dialogIdx);
	void openConstraintDialog(const QString& name);

	void on_dataTimeConstraintsTeachersActivityTagMaxHoursDailyAction_triggered();
	void on_dataTimeConstraintsTeacherActivityTagMaxHoursDailyAction_triggered();
	void on_dataTimeConstraintsStudentsSetMaxGapsPerDayAction_triggered();
	void on_dataTimeConstraintsStudentsMaxGapsPerDayAction_triggered();
	void on_dataTimeConstraintsStudentsSetActivityTagMaxHoursDailyAction_triggered();
	void on_dataTimeConstraintsStudentsActivityTagMaxHoursDailyAction_triggered();

	void on_activityPlanningAction_triggered();
	void on_spreadActivitiesAction_triggered();
	void on_removeRedundantConstraintsAction_triggered();

	//about
	void on_helpAboutAction_triggered();
	//offline
	void on_helpFAQAction_triggered();
	void on_helpTipsAction_triggered();
	void on_helpInstructionsAction_triggered();
	//online
	void on_helpHomepageAction_triggered();
	void on_helpContentsAction_triggered();
	void on_helpForumAction_triggered();
	void on_helpAddressesAction_triggered();

	void on_timetableGenerateAction_triggered();
	void on_timetableViewStudentsDaysHorizontalAction_triggered();
	void on_timetableViewStudentsTimeHorizontalAction_triggered();
	void on_timetableViewTeachersDaysHorizontalAction_triggered();
	void on_timetableViewTeachersTimeHorizontalAction_triggered();
	void on_timetableViewRoomsDaysHorizontalAction_triggered();
	void on_timetableViewRoomsTimeHorizontalAction_triggered();
	void on_timetableShowConflictsAction_triggered();
	void on_timetablePrintAction_triggered();
	void on_timetableGenerateMultipleAction_triggered();

	void on_timetableLockAllActivitiesAction_triggered();
	void on_timetableUnlockAllActivitiesAction_triggered();
	void on_timetableLockActivitiesDayAction_triggered();
	void on_timetableUnlockActivitiesDayAction_triggered();
	void on_timetableLockActivitiesEndStudentsDayAction_triggered();
	void on_timetableUnlockActivitiesEndStudentsDayAction_triggered();
	void on_timetableLockActivitiesWithASpecifiedActivityTagAction_triggered();
	void on_timetableUnlockActivitiesWithASpecifiedActivityTagAction_triggered();

	void on_timetableSaveTimetableAsAction_triggered();

	void on_randomSeedAction_triggered();
	
	void on_languageAction_triggered();
	
	void on_settingsDivideTimetablesByDaysAction_toggled();
	void on_settingsDuplicateVerticalNamesAction_toggled();

	void on_settingsRestoreDefaultsAction_triggered();

	void on_settingsTimetableHtmlLevelAction_triggered();
	void on_settingsPrintActivityTagsAction_toggled();
	void on_settingsPrintDetailedTimetablesAction_toggled();
	void on_settingsPrintDetailedTeachersFreePeriodsTimetablesAction_toggled();
	void on_settingsPrintNotAvailableSlotsAction_toggled();
	void on_settingsPrintBreakSlotsAction_toggled();

	void on_settingsPrintActivitiesWithSameStartingTimeAction_toggled();

	void on_selectOutputDirAction_triggered();
	
	void on_statisticsExportToDiskAction_triggered();
	void on_statisticsPrintAction_triggered();
	
	void on_shortcutAllTimeConstraintsPushButton_clicked();
	void on_shortcutBreakTimeConstraintsPushButton_clicked();
	void on_shortcutTeachersTimeConstraintsPushButton_clicked();
	void on_shortcutStudentsTimeConstraintsPushButton_clicked();
	void on_shortcutActivitiesTimeConstraintsPushButton_clicked();
	void on_shortcutAdvancedTimeConstraintsPushButton_clicked();

	void on_shortcutAllSpaceConstraintsPushButton_clicked();
	void on_shortcutRoomsSpaceConstraintsPushButton_clicked();
	void on_shortcutTeachersSpaceConstraintsPushButton_clicked();
	void on_shortcutStudentsSpaceConstraintsPushButton_clicked();
	void on_shortcutSubjectsSpaceConstraintsPushButton_clicked();
	void on_shortcutActivityTagsSpaceConstraintsPushButton_clicked();
	void on_shortcutSubjectsAndActivityTagsSpaceConstraintsPushButton_clicked();
	void on_shortcutActivitiesSpaceConstraintsPushButton_clicked();
	
	void on_shortcutGeneratePushButton_clicked();
	void on_shortcutGenerateMultiplePushButton_clicked();
	void on_shortcutViewTeachersPushButton_clicked();
	void on_shortcutViewStudentsPushButton_clicked();
	void on_shortcutViewRoomsPushButton_clicked();
	void on_shortcutShowSoftConflictsPushButton_clicked();
	//2014-07-01
	void on_shortcutsTimetableAdvancedPushButton_clicked();
	void on_shortcutsTimetablePrintPushButton_clicked();
	void on_shortcutsTimetableLockingPushButton_clicked();
	
	void on_shortcutBasicPushButton_clicked();
	void on_shortcutSubjectsPushButton_clicked();
	void on_shortcutActivityTagsPushButton_clicked();
	void on_shortcutTeachersPushButton_clicked();
	void on_shortcutStudentsPushButton_clicked();
	void on_shortcutActivitiesPushButton_clicked();
	void on_shortcutSubactivitiesPushButton_clicked();
	void on_shortcutDataAdvancedPushButton_clicked();
	void on_shortcutDataSpacePushButton_clicked();

	void on_shortcutOpenPushButton_clicked();
	void on_shortcutOpenRecentPushButton_clicked();
	void on_shortcutNewPushButton_clicked();
	void on_shortcutSavePushButton_clicked();
	void on_shortcutSaveAsPushButton_clicked();

	void rules_contentsChanged();
	void rules_basicDataResized();

	void replyFinished(QNetworkReply* networkReply);
	
protected:
	void closeEvent(QCloseEvent* event);
};

#endif
