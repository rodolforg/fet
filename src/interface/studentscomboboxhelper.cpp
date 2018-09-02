/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include "studentscomboboxhelper.h"

#include "fetguisettings.h"

#include <QSet>

#include "rules.h"

int StudentsComboBoxHelper::populateStudentsComboBox(const Rules& rules, QComboBox* studentsComboBox, const QString& selectedStudentsSet, bool addEmptyAtBeginning)
{
	studentsComboBox->clear();

	int currentIndex=0;
	int selectedIndex=-1;

	if(addEmptyAtBeginning){
		studentsComboBox->addItem(QString(""));
		if(selectedStudentsSet==QString(""))
			selectedIndex=currentIndex;
		currentIndex++;
		if(STUDENTS_COMBO_BOXES_STYLE==STUDENTS_COMBO_BOXES_STYLE_CATEGORIZED && rules.yearsList.count()>0){
#if QT_VERSION >= 0x040400
			studentsComboBox->insertSeparator(studentsComboBox->count());
			currentIndex++;
#endif
		}
	}

	if(STUDENTS_COMBO_BOXES_STYLE==STUDENTS_COMBO_BOXES_STYLE_SIMPLE){
		foreach(StudentsYear* sty, rules.yearsList){
			studentsComboBox->addItem(sty->name);
			if(sty->name==selectedStudentsSet)
				selectedIndex=currentIndex;
			currentIndex++;
			foreach(StudentsGroup* stg, sty->groupsList){
				studentsComboBox->addItem(stg->name);
				if(stg->name==selectedStudentsSet)
					selectedIndex=currentIndex;
				currentIndex++;
				if(SHOW_SUBGROUPS_IN_COMBO_BOXES){
					foreach(StudentsSubgroup* sts, stg->subgroupsList){
						studentsComboBox->addItem(sts->name);
						if(sts->name==selectedStudentsSet)
							selectedIndex=currentIndex;
						currentIndex++;
					}
				}
			}
		}
	}
	else if(STUDENTS_COMBO_BOXES_STYLE==STUDENTS_COMBO_BOXES_STYLE_ICONS){
		foreach(StudentsYear* sty, rules.yearsList){
			studentsComboBox->addItem(sty->name);
			if(sty->name==selectedStudentsSet)
				selectedIndex=currentIndex;
			currentIndex++;
			foreach(StudentsGroup* stg, sty->groupsList){
				studentsComboBox->addItem(QIcon(":/images/group.png"), stg->name);
				if(stg->name==selectedStudentsSet)
					selectedIndex=currentIndex;
				currentIndex++;
				if(SHOW_SUBGROUPS_IN_COMBO_BOXES){
					foreach(StudentsSubgroup* sts, stg->subgroupsList){
						studentsComboBox->addItem(QIcon(":/images/subgroup.png"), sts->name);
						if(sts->name==selectedStudentsSet)
							selectedIndex=currentIndex;
						currentIndex++;
					}
				}
			}
		}
	}
	else if(STUDENTS_COMBO_BOXES_STYLE==STUDENTS_COMBO_BOXES_STYLE_CATEGORIZED){
		QSet<QString> years;

		bool haveGroups=false;

		foreach(StudentsYear* sty, rules.yearsList){
			assert(!years.contains(sty->name));
			years.insert(sty->name);
			studentsComboBox->addItem(sty->name);
			if(sty->name==selectedStudentsSet)
				selectedIndex=currentIndex;
			currentIndex++;

			if(!haveGroups && sty->groupsList.count()>0)
				haveGroups=true;
		}

		if(haveGroups){
#if QT_VERSION >= 0x040400
			studentsComboBox->insertSeparator(studentsComboBox->count());
			currentIndex++;
#endif
			QSet<QString> groups;

			bool haveSubgroups=false;

			foreach(StudentsYear* sty, rules.yearsList){
				foreach(StudentsGroup* stg, sty->groupsList){
					if(!groups.contains(stg->name)){
						groups.insert(stg->name);
						studentsComboBox->addItem(stg->name);
						if(stg->name==selectedStudentsSet)
							selectedIndex=currentIndex;
						currentIndex++;

						if(!haveSubgroups && stg->subgroupsList.count()>0)
							haveSubgroups=true;
					}
				}
			}

			if(SHOW_SUBGROUPS_IN_COMBO_BOXES && haveSubgroups){
#if QT_VERSION >= 0x040400
				studentsComboBox->insertSeparator(studentsComboBox->count());
				currentIndex++;
#endif

				QSet<QString> subgroups;

				foreach(StudentsYear* sty, rules.yearsList){
					foreach(StudentsGroup* stg, sty->groupsList){
						foreach(StudentsSubgroup* sts, stg->subgroupsList){
							if(!subgroups.contains(sts->name)){
								subgroups.insert(sts->name);
								studentsComboBox->addItem(sts->name);
								if(sts->name==selectedStudentsSet)
									selectedIndex=currentIndex;
								currentIndex++;
							}
						}
					}
				}
			}
		}
	}
	else{
		assert(0);
	}

	return selectedIndex;
}
