This is FET version 5.30.7


Program description:

	FET is free timetabling software (licensed under the GNU Affero General Public License version 3 or later).
	This program aims to automatically generate the timetable of a school, high-school or university.
	It may be used for other timetabling purposes.

	FET can mean "Free Educational Timetabling" (the "E" in the middle may also stand for other words, based on your personal preference).

	FET homepage: http://lalescu.ro/liviu/fet/


Requirements:

	FET is created in the following environment: openSUSE Leap 42.2 GNU/Linux distribution, Linux 4.4.27, Xfce 4.12,
	Midnight Commander 4.8.18, KDiff3 0.9.98, Qt 5.7.0, gcc 6.2.1, g++ 6.2.1, make 4.0, sed 4.2.2,
	Valgrind 3.11.0, Coverity Scan 8.5.0.1, Cppcheck 1.76, other great free tools.
	FET can be run on any platform supported by the free software Qt (GNU/Linux, Windows, Mac OS X).

	GNU/Linux, Mac OS X:
	Software:
		make 4.0 or similar (GNU Make)
		gcc 6.2.1 or similar
		g++ 6.2.1 or similar (or gcc-g++, or gcc-c++, the name may vary. This program may be included in the gcc package)
		Qt 5.7.0 or compatible (The authors also tried to maintain backwards source compatibility with Qt 4.v.v).

	Windows:
	For running, you need only the archive of FET compiled for Windows (which contains the MinGW and Qt dlls).
	For compiling, you need Qt and a compatible C++ compiler (MinGW is a possibility).


Download:

	You can download the latest version from the FET homepage, http://lalescu.ro/liviu/fet/


Getting - compiling - installing GNU tools: make, gcc and g++ (or gcc-g++, or gcc-c++):

	GNU/Linux, maybe Mac OS X: Most probably, your platform has these installed by default or available on the internet
	as precompiled packages. If you don't have them, please install them using YaST (on openSUSE), Adept Manager
	(on Ubuntu) or other package manager.
	To verify that you have the correct packages, write "make --version", "gcc --version" and
	"g++ --version".
	Advice from a user on Ubuntu: "sudo apt-get install build-essential" installs build libraries commonly used.
	
	Maybe Mac OS X from a user: Install Xcode (current version is 7.3.1) from the Mac App Store.
	(Once installed, I think you'll need to open the Xcode app to finish the installation.)

	Windows: you need a Qt compatible C++ compiler.
	Maybe an easy option is the MinGW compiler, available inside the Qt or on the Qt download page. You may
	alternatively download MinGW from its homepage.


Getting - compiling - installing Qt:

	Qt homepage: http://www.qt.io/

	You will need Qt 5.7.0 or compatible to compile FET (The authors also tried to maintain backwards source compatibility with Qt 4.v.v).
	You can get the corresponding version of Qt with your package manager or from the Qt homepage.
	Qt can be used with the LGPL or commercial license.

	GNU/Linux: It is possible to use YaST (or Adept Manager) or other package managers to get your version of Qt, pre-compiled.
	Or you can download, (possibly) compile and install Qt from its homepage.
	A possible name for the precompiled package is libqt5.
	A user reported that on Ubuntu "sudo apt-get install qt5-default" without quotes should work.

	Windows - Download latest Qt from its homepage. Usually, you can get a precompiled Qt version for your C++ compiler, but sometimes
	you may need to compile Qt from its sources.

	Mac OS X:
	Maybe you can get pre-compiled Qt packages for Mac OS X. If not, get Qt from its homepage.

	Note about the Qt "QMAKESPEC" variable: advanced topic, read this if you get Qt errors about the environment not being set, or environment
	variable QMAKESPEC not being set, or if the Qt environment is not set correctly: Qt needs to know the environment you are using. It is usually
	something like: linux-g++, macx-g++ or win32-g++ (platform-compiler). You can find all the supported platforms in the directory "mkspecs" of Qt
	(some usual locations are /usr/share/qt5/mkspecs or /usr/lib/qt5/mkspecs on GNU/Linux or C:\Qt\5.7.0\mkspecs on Windows).
	It seems that Qt automatically finds the platform (there is a default configuration in the "default" subdirectory of the "mkspecs" directory,
	which is created automatically when you install Qt). If the default configuration is not working or is wrong, you may need to set the
	correct QMAKESPEC variable for your platform. Under Windows, you can create an environment variable QMAKESPEC equal to win32-g++
	(if you are using MinGW), under other platforms create an environment variable named QMAKESPEC equal with the correct platform-compiler
	you are working on (for instance macx-g++ or linux-g++).


Compiling FET:

	Note: if after unpacking the tar.bz2 sources archive you obtain some files with incomplete/truncated file names in the sources directories,
	you might need to use a better (un)packer (probably a recent version of the official tar and bzip2 should be fine).
	For GNU/Linux and maybe Mac OS X, "tar -jxvf fet-v.v.v.tar.bz2" should work correctly.
	For Windows, either use a modern good (un)packer, or, advice from a user:
		1. download mingw-get-setup.exe from http://sourceforge.net/projects/mingw/files
		2. run mingw-get-setup.exe
		3. check msys-base to be installed, Apply Changes from Installation menu and close MinGW Installation Manager
		4. run C:\MinGW\msys\1.0\msys.bat
		5. $ cd to the directory where the .tar.bz2 file is
		6. $ tar -jxvf fet-v.v.v.bz2

	Note2: compilation takes long (maybe even 1 hour, if you are using an older computer). The file
	rules.cpp takes the longest time to compile, maybe even 1 minute or more.

	Note3: the following situation may appear under GNU/Linux, but it could be met also on other platforms.
	Depending on your Qt installation, it may happen that the FET intermediary files and executables are very large (~100 MB for the fet executable
	instead of ~10 MB). It is not sure if this affects the FET speed and performance, but anyway it is unpleasant. In this case, you may want to
	modify some files in your system (Qt files) to improve this. You need to change some Qt configuration files and recompile FET from the beginning
	(remove any temporary files with make distclean or unpack FET sources again, in an empty location).
	Notations: mkspecs is a directory and QMAKE_CFLAGS_RELEASE and QMAKE_CXXFLAGS_RELEASE are two sections (text on two lines) contained in a file
	in a subdirectory of mkspecs (some possible location for these are given in this paragraph, shortly below).
	If mkspecs files for your platform contain the -g flag (text) in the QMAKE_CFLAGS_RELEASE or QMAKE_CXXFLAGS_RELEASE sections, you need to remove
	this flag. If they do not contain -g flag in these sections, then you don't need to remove anything (and most probably the intermediary files and fet
	executable will have normal size).
	More details about where can you find the mkspecs files: under openSUSE GNU/Linux, it seems that for newer versions of Qt (>=4.7.1), the problem
	is no longer present. In the past, the location of this minor problem was in the file /usr/share/qt5/mkspecs/common/g++.conf, at lines 10 and 25,
	you needed to remove the "-g" flag from the QMAKE_CFLAGS_RELEASE and QMAKE_CXXFLAGS_RELEASE sections (root access needed to make this change).
	Under Fedora GNU/Linux for the same problem, a user reported that you need to modify /usr/lib/qt4/mkspecs/common/g++.conf
	and /usr/lib/qt4/mkspecs/linux-g++/qmake.conf (remove the -g flag from QMAKE_CFLAGS_RELEASE and QMAKE_CXXFLAGS_RELEASE sections).
	(The mkspecs directory contains more subdirectories, in the form platform-compiler, you need to search for your corresponding directories/files.
	For instance, GNU/Linux with gcc compiler is named "linux-g++", and you may also need to modify the "common" section).
	Another solution, by another user: the addition of the lines
	 "linux-g++*: QMAKE_CXXFLAGS_RELEASE -= -g"
	 and
	 "linux-g++*: QMAKE_CFLAGS_RELEASE -= -g"
	 (without quotes) in the files src/src.pro and src/src-cl.pro (you can add them at the end of these files, or in another proper location).
	
	Note NMkJobs: advice from a user: if you have a multiple core computer, you may be able to split the compilation into more threads, with the -j option for the
	make command, so that the compilation is faster. This option creates a number of compilation threads equal with the number specified along with the -j option.
	Under GNU/Linux or Mac OS X, just write "make -j 2" if you have dual core or "make -j 4" if you have quad core (in case you have quad core and you want
	to keep a core free for other tasks, write "make -j 3"). This is proven to work under GNU/Linux and Mac OS X. Under Windows it depends on the C++ compiler (you
	may try it to see if it works).

	Currently FET can be compiled with a C++ compiler, using Qt version 5.7.0 or compatible.

	Note NUseSystemLocale: If you want FET to autodetect the system locale language, type <<qmake fet.pro "DEFINES+=USE_SYSTEM_LOCALE">>
	or simply <<qmake "DEFINES+=USE_SYSTEM_LOCALE">>. This will work if you recompile from the beginning all the FET package (remove
	all the intermediary files and recompile), and also if you did not use FET on this machine or if you remove the settings file/registry entry for FET
	(otherwise FET will retain the language which was already saved in its settings).
	
	Note4: If you get an error like this:
	In file included from ../../Qt5.3.2/5.3/gcc_64/include/QtGui/qopenglcontext.h:62:0,
		from ../../Qt5.3.2/5.3/gcc_64/include/QtGui/QtGui:32,
		from ../../Qt5.3.2/5.3/gcc_64/include/QtWidgets/QtWidgetsDepends:3,
		from ../../Qt5.3.2/5.3/gcc_64/include/QtWidgets/QtWidgets:3,
		from engine/import.cpp:35:
	../../Qt5.3.2/5.3/gcc_64/include/QtGui/qopengl.h:110:21: fatal error: GL/gl.h: No such file or directory
	 #  include <GL/gl.h>
					^
	compilation terminated.
	you may need to install additional packages before compiling FET. For instance, under my GNU/Linux openSuse I need to install Mesa-devel.

	Note 5: If you only want to compile the command-line version, open the file fet.pro with a text editor and from the second line remove the
	text "src/src.pro ".


	GNU/Linux:
	- You will need Qt 5.7.0 or compatible to compile this program.
	- type "qmake fet.pro" or simply "qmake". You have to use qmake from Qt 5 series, which on some systems
	might be named qmake-qt5 (this command is executed very fast, so don't worry if you get immediate return from it)
	See also notes NUseSystemLocale and Note4 above.
	- type "make" (this takes a long time, maybe even 1 hour). See also note NMkJobs above.
	To remove the compiled objects/executable: "make clean" and/or "make distclean".


	Mac OS X:
	3 Variants:

	1. First variant, if you use the gcc compiler:
	- You will need Qt 5.7.0 or compatible to compile this program.
	- type "qmake fet.pro" or simply "qmake". You have to use qmake from Qt 5 series.
	(this command is executed very fast, so don't worry if you get immediate return from it)
	- IMPORTANT: you might need to write: "qmake -spec macx-g++ fet.pro", if "qmake fet.pro"
	does not produce a correct makefile. Please see
	http://lists.trolltech.com/qt-interest/2007-04/thread01126-0.html for details.
	See also notes NUseSystemLocale and Note4 above.
	- type "make" (this takes a long time, maybe even 1 hour). See also note NMkJobs above.
	To remove the compiled objects/executable: "make clean" and/or "make distclean".

	2. Another variant:
	- Type "qmake fet.pro" and then "xcodebuild".

	If you encounter problems, please see:
	http://lists.trolltech.com/qt-interest/2007-04/thread01126-0.html
	
	3. Another variant, if you use the Clang C++ compiler (which seems to be the default for Qt 5.7.0 on Mac):
	It seems that you need to type:
	"/Users/Yourname/Qt5.7.0/5.7/clang_64/bin/qmake -r -spec /Users/Yourname/Qt5.7.0/5.7/clang_64/mkspecs/macx-clang", then "make"
	(the -r parameter might not be necessary.)
	Please see: http://lalescu.ro/liviu/fet/forum/index.php?topic=2488.msg13425#msg13425 (and also see the whole topic there.)


	Windows: MinGW C++ compiler: you need Qt 5.7.0 or compatible, and MinGW.
	You might use other compilers if you want, please see the Qt homepage.
	for other variants. Any C++ compiler that supports Qt should be good.
	You need to run "qmake fet.pro" or "qmake" (this command is executed very fast, so don't worry if you get
	immediate return from it) and then, if you are using MinGW, "mingw32-make" (this takes a long time, maybe even 1 hour)
	in a command line in the FET directory.
	See also note NUseSystemLocale above.
	See also Note4 above.
	See also note NMkJobs above.
	(You can use the command line prompt of Windows or the Qt command line prompt shortcut in the Qt shortcuts menu.)
	Then, you can remove the directories "src" and "tmp", to save up space, if you want.
	You will need to copy the dll's in the same directories as in the official FET archive for Windows.


Installing FET:

	GNU/Linux, and maybe Mac OS X:
	Type "make install" with root permissions.
	
	Mac OS X: A user reported that one can obtain a universal Mac OS X executable with:
	$ cd /"pathto"/Qt/5.5/clang_64/bin/
	$ ./macdeployqt /"pathto"/fet.app
	(with Qt 5.5)

	Windows:
	Move the FET directory to a place of your choice.


Running FET:
	Note: The FET main window contains a menu with the items: File, Data, Statistics, Advanced,
	Timetable, Settings, Help. Everything is right if you have that. Optionally, on the main window there may be a toolbox
	containing shortcuts (if selected by the user in the settings. By default it is selected).
	
	To run FET in interface mode, you just need to run the executable named fet or fet.exe.

	GNU/Linux, and maybe Mac OS X:
	chdir to the directory where the fet executable is (for instance, "cd ~/fet-v.v.v/") and run it with "./fet" or with the suitable command
	for your platform. Alternatively, you may run the executable named fet from the fet-v.v.v directory, using another method. The fet executable
	will be located inside the fet-v.v.v directory after a successful compilation.
	
	Note for Mac OS X about using translations (if you want to run FET into other language): it seems that you need to copy the contents
	of the "translations/" directory into the directory fet-v.v.v/fet.app/Contents/MacOS/translations/ or into the directory /usr/share/fet/translations/
	(if you compiled/installed FET by yourself). Or copy the whole directory translations/ into fet-v.v.v/fet.app/Contents/MacOS/ or into usr/share/fet/
	More notes for the Mac OS X platform, from a user: copying the translations/ directory into the directory fet.app/Contents/MacOS/ is
	possible from the command line in X11 xterm (but it is not possible with the finder program, because the file extension ".app" is
	exclusive for applications, so the program finder interprets a directory with the extension ".app" as an application).

	Windows:
	Run the executable fet.exe (its location is inside the fet-v.v.v directory, you can find it there if you have FET precompiled for Windows or after you compiled it).

	COMMAND LINE: no GUI, just run "fet-cl --inputfile=x [--outputdir=d] [--timelimitseconds=y] [--htmllevel=z] [--language=t]
	 [--writetimetableconflicts=wt1] [--writetimetablesstatistics=wt2] [--writetimetablesxml=wt3] [--writetimetablesdayshorizontal=wt4]
	 [--writetimetablesdaysvertical=wt5] [--writetimetablestimehorizontal=wt6] [--writetimetablestimevertical=wt7] [--writetimetablessubgroups=wt8]
	 [--writetimetablesgroups=wt9] [--writetimetablesyears=wt10] [--writetimetablesteachers=wt11] [--writetimetablesteachersfreeperiods=wt12]
	 [--writetimetablesrooms=wt13] [--writetimetablessubjects=wt14] [--writetimetablesactivitytags=wt15] [--writetimetablesactivities=wt16]
	 [--printactivitytags=a] [--printnotavailable=u] [--printbreak=b] [--dividetimeaxisbydays=v] [--duplicateverticalheaders=e]
	 [--printsimultaneousactivities=w] [--randomseedx=rx --randomseedy=ry] [--warnifusingnotperfectconstraints=s]
	 [--warnifusingstudentsminhoursdailywithallowemptydays=p] [--warnsubgroupswiththesameactivities=ssa]
	 [--printdetailedtimetables=pdt] [--printdetailedteachersfreeperiodstimetables=pdtfp] [--verbose=r]",
	where	x is the input file, for instance "data.fet"
			d is the path to results directory, without trailing slash or backslash (default is current working path).
				Make sure you have write permissions there.
			y is integer (seconds) (default 2000000000, which is practically infinite).
			z is integer from 0 to 7 and represents the detail level for the generated HTML timetables
				(default 2, larger values have more details/facilities and larger file sizes).
			t is one of en_US, ar, ca, cs, da, de, el, es, eu, fa, fr, gl, he, hu, id, it, lt, mk, ms, nl, pl, pt_BR, ro, ru, si, sk, sq, sr, tr, uk,
				uz, vi, zh_CN, zh_TW (default en_US).
			wt1 to wt16 are either true or false and represent whether you want the corresponding timetables to be written on the disk (default true).
			a is either true or false and represets if you want activity tags to be present in the final HTML timetables (default true).
			u is either "true" or "false" and represents if you want -x- (for true) or --- (for false) in the generated timetables for the
				not available slots (default true).
			b is either "true" or "false" and represents if you want -X- (for true) or --- (for false) in the generated timetables for the
				break slots (default true).
			v is either true or false, represents if you want the HTML timetables with time-axis divided by days (default false).
			e is either true or false, represents if you want the HTML timetables to duplicate vertical headers to the right of the tables,
				for easier reading (default false).
			w is either true or false, represents if you want the HTML timetables to show related activities which have constraints with same starting time (default false).
				(for instance, if A1 (T1, G1) and A2 (T2, G2) have constraint activities same starting time, then in T1's timetable will appear also A2, at the same slot
				as A1).
			rx is the random seed X component, minimum 1 to maximum 2147483646, ry is the random seed Y component, minimum 1 to maximum 2147483398
				(you can get the same timetable if the input file is identical, if the FET version is the same and if the random seed X and Y components are the same).
			s is either true or false, represents whether you want a message box to be shown, with a warning, if the input file contains not perfect constraints
				(activity tag max hours daily or students max gaps per day) (default true).
			p is either true or false, represents whether you want a message box to be shown, with a warning, if the input file contains nonstandard constraints
				students min hours daily with allow empty days (default true).
			ssa is either true or false, represents whether you want a message box to be show, with a warning, if your input file contains subgroups which have
				the same activities (default true).
			pdt is either true or false, represents whether you want to show the detailed (true) or less detailed (false) years and groups timetables (default true).
			pdtfp is either true or false, represents whether you want to show the detailed (true) or less detailed (false) teachers free periods timetables (default true).
			r is either true or false, represents whether you want additional generation messages and other messages to be shown on the command line (default false).

	Alternatively, you can run "fet-cl --version [--outputdir=d]" to get the current FET version.
	where d is the path to results directory, without trailing slash or backslash (default is current working path).
	Make sure you have write permissions there.
	(If you specify the "--version" argument, FET just prints version number on the command line prompt and in the output directory and exits.)
	
	You can ask the FET command line process to stop the timetable generation, by sending it the SIGTERM signal.
	FET will then write the current timetable and the highest stage timetable and exit.

	The result of the operation is logged into file outputdir/logs/result.txt.

	Also, you have in logs the initial order of activities and a list of maximum placed number of activities. If your timetable is impossible, say it stops
	at maximum n placed activities, look at the n+1-st activity in the initial order, for it or its students/teachers may be the problem.

	For the command line version, probably the most useful output to use internally in an application might be the activities_timetable.xml.
	You have of course also the xml timetables for students and teachers and the nicely formatted HTML timetables to present directly.

	You'll get a console output to help you with data, along with the files in the directory outputdir/logs/.
	
	If you are a programmer, in the file src/engine/messageboxes.cpp you'll find the implementation of the various messages. You
	can modify these functions, catching them in your program. Also, you can catch the end of the program (successful or unsuccessful) in
	the file src/interface/fet.cpp, in the command-line code part.
	
	If you want to be completely independent from Qt, using only standard C++, you can remove all the Qt includes and
	reimplement the used Qt classes QString, QList, QSet, and others (only the member functions which are necessary). Or just get the corresponding
	code from Qt (Please review the Qt licensing options, in this case. Currently, they include LGPL and commercial).
	The program should have independence of a particular implementation of QSet and QHash so, if you reimplement these two containers,
	any order of elements should lead to the same generation results. Contrary behavior should be reported as a bug.


Languages (translations):

	The translation files which the FET executable needs in order to be able to run in other languages are ending in .qm (like fet_**.qm)
	and they are located in the "translations" directory in the FET distribution.

	FET will search for translations (fet_**.qm files) in the following order:
	1) directly in the directory where the fet executable is,
	2) the directory "translations" in the directory where fet executable is,
	3) the directory /usr/share/fet/translations (under GNU/Linux or Mac OS X).

	So, on:
	Windows: if fet.exe is in C:\fet-v.v.v, fet will search for the file fet_**.qm in
		C:\fet-v.v.v and then in C:\fet-v.v.v\translations
	GNU/Linux: if the fet executable is in ~/fet-v.v.v, fet will search for the file fet_**.qm in
		~/fet-v.v.v, then in ~/fet-v.v.v/translations and then in /usr/share/fet/translations
	Mac OS X: the operation is approximately similar to the one for GNU/Linux.
		The difference is that it seems that the fet application on Mac OS X contains in turn other directories (application directories),
		so FET will search for the translations inside these application directories, not in the fet-v.v.v directory. The GNU/Linux style fet executable
		seems to be located in fet-v.v.v/fet.app/Contents/MacOS, so FET will search for the file fet_**.qm in the directory
		fet-v.v.v/fet.app/Contents/MacOS, then in the directory fet-v.v.v/fet.app/Contents/MacOS/translations,
		then in the directory /usr/share/fet/translations.
		Please see also the Mac OS X note from the section "Running FET".
		More notes for the Mac OS X platform, from a user: copying the translations/ directory into the directory fet.app/Contents/MacOS/ is
		possible from the command line in X11 xterm (but it is not possible with the finder program, because the file extension ".app" is
		exclusive for applications, so the program finder interprets a directory with the extension ".app" as an application).

	What to do if you get stuck with an unknown language: in each language, the submenu (Settings->)Language is translated also in English
		(like: Limba/Language in Romanian). You can select the US English language (en_US abbreviation) and then close and restart FET (press the middle button
		from the dialog that appears after trying to close FET, so that FET will exit without saving the current file).
	Another solution: modify the FET settings on your operating system:
		- Under GNU/Linux, and maybe Mac OS X, usually in your $HOME/.config directory you'll find a directory "fet",
		containing file "fettimetabling.conf". Edit that file - the line beginning with language, instead of language=... (for instance, language=ro),
		make it language=en_US (case sensitive).
		- Under Windows: run regedit.exe (Registry Editor), search key "fet". You will find a section "fet" with sub-section "fettimetabling".
		Modify the value name language (from, for instance, ro, or other language) to have the value data en_US (5 characters, case sensitive).
	Another solution: simply remove the translation files (or move them so they are not available to the fet executable) and start FET. It will report
		that the custom language is not found and it will revert to en_US.


Documentation:

	Some hints in the directory "fet-v.v.v/doc/" (under GNU/Linux or Mac OS X, they may be found in usr/share/doc/fet/ directory).
	Some other hints in the program Help menu.
	You may find more documentation on the FET homepage.


Input files:

	You can open a few sample input files in "fet-v.v.v/examples/" directory
	(under GNU/Linux or Mac OS X, they may be found in /usr/share/fet/examples/)


Uninstalling FET:

	GNU/Linux, and maybe Mac OS X: type "make uninstall" with root permissions. Then,

	GNU/Linux and Windows:
	Remove the directory fet-v.v.v. Also, you may want to remove the directory with the fet results (default: $HOME/fet-results).
	This will leave the settings of FET on your system. FET stores its settings in your operating system's dedicated section.
	If you really want to get rid of everything FET created, read below.

	To completely remove stored FET settings from your operating system:

	GNU/Linux:
	Usually, in your $HOME/.config directory you will find a directory "fet" containing the file "fettimetabling.conf".
	Remove the directory "fet" and its contents.

	Mac OS X: It seems that the configuration file might be: $HOME/Library/Preferences/com.fet.fettimetabling.plist

	Windows:
	Run regedit.exe (Registry Editor), search for the key "fet". You will find a section "fet" with the subsection "fettimetabling".
	Remove the "fet" section and its contents.


Feed-back / help:

	You are encouraged to express your opinion about this program, to send your data files, for the purpose of testing or
	for inclusion into examples, and also to report possible bugs and suggestions. See the FET homepage for contact information.


Copyright (C) 2002-2016 Liviu Lalescu, Volker Dirr.
