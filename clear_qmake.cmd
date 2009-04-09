make distclean
for  /r %%B in (*.qm)  do del /s /q %%B
for  /r %%B in (*.dll)  do del /s /q %%B
for  /r %%B in (*.a)  do del /s /q %%B
