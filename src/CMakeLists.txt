target_sources(fateInjector PRIVATE cApp.cpp        
cMain.cpp       
config.cpp      
FixFilePerms.cpp
inject.cpp
pch.cpp
taskBarIcon.cpp
icon.rc)

target_precompile_headers(fateInjector PRIVATE "pch.h")