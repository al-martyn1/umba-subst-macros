..\.out\msvc2019\x64\Debug\umba-subst-macros --help >help.txt

@rem ..\.out\msvc2019\x64\Debug\umba-subst-macros --verbose=detailed
@rem ..\.out\msvc2019\x64\Debug\umba-subst-macros --verbose=detailed  @macros.rsp    
@rem tests01_in.txt tests01_out.txt
@rem ..\.out\msvc2019\x64\Debug\umba-subst-macros --verbose=detailed -Y @macros.rsp
..\.out\msvc2019\x64\Debug\umba-subst-macros --verbose=detailed                  @macros.rsp in_tests01.txt       out_tests01.txt
..\.out\msvc2019\x64\Debug\umba-subst-macros --verbose=detailed   --raw          @macros.rsp in_tests01.txt       out_tests02.txt
..\.out\msvc2019\x64\Debug\umba-subst-macros --verbose=detailed   --raw          @macros.rsp in_tests03.txt       out_tests03.txt
..\.out\msvc2019\x64\Debug\umba-subst-macros --verbose=detailed   --raw  --batch @macros.rsp in_tests01.txt=out_tests04.txt in_tests03.txt=out_tests05.txt

