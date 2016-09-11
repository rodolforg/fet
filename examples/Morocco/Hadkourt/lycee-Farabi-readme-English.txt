How to use FET for Moroccan schools (lycee or the secondary qualifying shools)

This documentation file written by Chafik Graiguer on 25 March 2010. Tricks to use official FET for Morocco schools were devised by:
- Chafik Graiguer
- Volker Dirr
- Liviu Lalescu
in a public discussion on the FET forum ( http://lalescu.ro/liviu/fet/forum/ )

This document was updated on 17 October 2011, to add new information about students max 7 hours per real day (in paragraph 5Bis).

1.Introduction
==============
FET is a powerful timetabling software. In most cases, it can produce de desirable timetable, however Moroccan schools have special requirements which cannot be fullfilled by FET without some tricks
FET does not distinguish between morning period, afternoon period and lunch break period. In FET there is one continuous day. But for Moroccan pedagogical system:
- A teacher may work either morning or afternoon, never both (there are few exceptions)
- Gaps around lunch periods are allowed. For example: students may study period 08-10, go home take lunch then return to shool to study in period 16-18 ! All the time from 10 to 16 is not counted as gap!!

2.How to make FET understand these requirements?
================================================
The trick is to treat morning and period time as two separated days
Here are the preliminary steps to customize FET for use by Moroccan shools

3. Entering days and hours
==========================
3.1 Go to DATA ---> Basics ---> days and hours ---> days per week
We should tell FET that morning period and afternoon period are two separated days, so we will have in total: 12 days
But we should also add another FAKE day between every two real days, so we will end with 17 days like this:
- Monday-morning
- Monday afternoon
- FAKE1
- Tuesday-morning
- Tuesday-afternoon
- FAKE2
... etc

3.2 Concerning Hours per day,
Go to DATA ---> Basics ---> days and hours ---> Hours (periods) per day
then enter the value 4
because we study from 08 until 12 on morning
then from 14 to 18 afternoon

3.3 Disallow all activities on Fake days
We need to make sure, no activities is scheduled in the 5 fake days
To ensure this:
Go to 
Time ---> Breaks 
Press "Add" then mark all time slots in Fake days with "X" 

4. Add pseudo activities to teachers
====================================
Now, we will force FET to schedule hours for teachers at morning or at afternoon periods, but never both
4.1 Go to DATA ---> Teachers
press "Add"
Then enter all teacher's names one by one

4.2 Go to DATA ---> Subjects
press "Add"
Then enter a pseudo subject called say "free"

4.3 Go to DATA ---> activities 
press "Add"
-Choose a teacher
-From Subject choose "free"
-Leave students field empty
-Under "split into.." choose 6
-Under minimum required distance.."choose 2
- from percentage.. choose 100%
- for every tab numbered 1, 2, 3 ..etc choose duration 4
-Finally press "Add"
Repeat those steps for every teacher

5. Allowing empty half days for students
========================================
By default, FET does not allow empty half days for students, we can overcome this from
5.1 Go to "Settings" Menu then "Advanced" then check "Enable students min hour daily with empty days"
A warning massage will appear, choose OK

5.2 Go to Time ---> Students ---> Min hours daily for all students
then choose 2
While on this window, don't forget to check "allow empty days" then press "Add"

5Bis. Set maximum hours per REAL day for students
=================================================
To prevent 8 hours per REAL day for student (Set max 7 hours per real day) proceed as follows:
1 - Time
2 - Activities
3 - A set of activities occupies max time slots from selection
4 - Add
5 - Select a students set
6 - A list of activities will show up
7 - Press All
8 - Click Time slots tab
9 - Select all slots from both Monday morning and Monday afternoon
10 - Max occupied: 7
11 - Click Add constraint
12 - Without changing anything else, click Unselect all
13 - Now, select all slots from both Tuesday morning and Tuesday afternoon
14 - Click Add constraint
15 - ... And so on, for all remaining days

Upon completing all days, click on Activities tab, select a new students set, then repeat steps from 5 to 15

Note: If you are using advanced students sectioning, you may need to adapt steps 5 to 7 to your situation

6. For teachers with contract hours 20 wishing a free day
=========================================================
Go to TIME ---> Teachers
Choose  "Max days per week for a teacher", then enter 11 days
Likewise, for teachers with contract hours 16 wishing two free days
Go to TIME ---> Teachers
Choose  "Max days per week for a teacher", then enter 10 days
..etc

7. If there is no school on Saturday afternoon
==============================================
If week study ends at saturady morning so there is no no school on Saturday afternoon, do the followins:
Go to Time ---> Students ---> A student set not available times
press "Add"
Choose a year, mark the 4 slots on Saturday afteroon with "X" then press "Add"
repead those steps for all years.
This way, we disallowed activitites on Satuday afternoon for students, which will disallow it for teachers as well !
To understand FET terminology about Years and Groups, see next paragraph:

8. How to understand Years, Groups and Subgroups
================================================
The following notes referes to
DATA ---> Students ---> Years/Groups/Subgroups
The "pedagogical structure" should be entered in a well organized way
 
8.1 Years
For Moroccan teminology, a year is the combination of year and section
So "Common Literacy Section" (TCL)is a Year
 "Common Scientific Section" or (TCS) is another
"Literacy First Year" (1L) is another one.. etc
 
8.2 Groups

Groups are simply what is called 'Classes"
So in the "Common Scientific Section" year, we may have 
Common Scientific Section-1 (TCS-1)
Common Scientific Section-2 (TCS-2)
Common Scientific Section-3 (TCS-3)
..etc
So, after entering Years,Go to DATA ---> Students ---> Groups
Shoose Year "Common Scientific Section" or (TCS) then add classes like:
TCS-1
TCS-2
...etc

8.3 Subgroups

Some groups need Two Subgroups. For example the "Common Scientific Section" group need two subgroups to be used in Laboratory Activities shared between Physics/Biology 
So, after entering Groups,Go to DATA ---> Students ---> Subgroups
Choose a group, say TCS-1 then add two subgroups
-TCS-1_1
-TCS-1_2


Finally, our FET is ready to deal with Moroccan schools requirements.
Now, you can use it to enter all activities, constraints... etc and generate the timetable.
On clicking on "generate", you will get a warning about using students min hours daily with allow empty days - there is no problem, you just need to click OK to continue generating.
