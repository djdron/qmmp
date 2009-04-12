make distclean
for  /r %%B in (*.qm)  do del /s /q %%B
for  /r %%B in (*.dll)  do del /s /q %%B
for  /r %%B in (*.a)  do del /s /q %%B
for  /r %%B in (*.Release)  do del /s /q %%B
for  /r %%B in (Makefile*)  do del /s /q %%B
for /d /r %%B in (.build)  do rmdir /s /q %%B
for /d /r %%B in (debug)  do rmdir /s /q %%B
for /d /r %%B in (release)  do rmdir /s /q %%B
for /r %%B in (*.rc)  do del /s /q %%B
del /s /q bin\qmmp.exe
rmdir /s /q bin\plugins

