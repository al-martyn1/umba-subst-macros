@set OPTIONS=--raw --overwrite --verbose=detailed
umba-subst-macros %OPTIONS% @%~dp0\kits-name-replace.rsp %~dp0\cmake-tools-kits.auto-scanned.json %~dp0\cmake-tools-kits.result.json