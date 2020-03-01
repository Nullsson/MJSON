set CommonCompilerFlags=-Od -MTd -nologo -fp:fast -fp:except- -Gm- -GR- -EHa- -Zo -Oi -WX -W4 -wd4201 -wd4100 -wd4189 -wd4505 -wd4127 -FC -Z7
set CommonLinkerFlags= -incremental:no -opt:ref user32.lib gdi32.lib winmm.lib kernel32.lib

cl %CommonCompilerFlags% -D_CRT_SECURE_NO_WARNINGS mjson_unittest.cpp /link %CommonLinkerFlags%
