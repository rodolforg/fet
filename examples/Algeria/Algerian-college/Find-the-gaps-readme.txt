A difficult file. Solved in about 2 hours 30 minutes on my computer (in another run, it solved in about 30 minutes).

The words of Benahmed Abdelkrim, the author of this file (from https://lalescu.ro/liviu/fet/forum/index.php?topic=3383.0):


Here is an example of timetable that satisfies the Algerian conditions but produced by official FET.
I used virtual activities for students and teachers.

I used the following relation to calculate the number of virtual activities for teachers:
Nv = 20 - Mg - Nh
Nv: number of virtual(pseudo) activities
Mg: Max gaps per week
Nh: number of hours worked per week for the teacher.

Nb:Nv must be an exactly positive number. For teachers who have more than 20h (or 19h if Mg = 1) not need virtual ativities.

The virtual activities must be placed on the first morning hours or around the break time of the Lunch (Lu).

The virtual activities represent a GAPS, they must be placed in the time slots allowed by the Algerian system.

This relation may be called: relation of the GAPS.


This relation is applied on condition that all teachers are given max intervals =6 (intervals = mornings or afternoons) except for teachers who exceed
their number of hours of work per week the 22 hours.


Mg may be null even if the user gives it a non-zero value, because the placement of the activities is semi-random. In this case may not take into consideration this value in the above relation;
Which is simplified to the following relation:
Nv = 20 - Nh


I think it is better to use the latter relation(Nv = 20 - Nh) because of the random nature of the activities, which increases the number of virtual
activities that should be added to the teachers concerned with one(+1). It is also better to put these virtual activities on the mornings
(8:00 - 9:00 or 11:00 - 12:00), so as not to be surprised after the generation, we get a teacher table with a single hour grouped with a virtual
activity or with a normal gap resulting from the constraint: Max gaps per week.
