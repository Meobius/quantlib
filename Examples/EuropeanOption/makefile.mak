
#
# makefile for EuropeanOption.cpp under Borland C++
#
# $Id$

.autodepend
.silent

# Debug version
!ifdef DEBUG
    _D = _d
!endif

# Directories
QL_INCLUDE_DIR = "$(QL_DIR)"
QL_LIB_DIR     = "$(QL_DIR)\lib\Win32\Borland"

#Warning W8026 :
#Warning W8027 :
#Warning W8012 :
#Warning W8057 : Parameter 'argc' is never used in function main(int,char * *)

# Options
CC_OPTS = -vi- -w-8026 -w-8027 -w-8012 -w-8057 \
    -I$(QL_INCLUDE_DIR)
!ifdef DEBUG
CC_OPTS = $(CC_OPTS) -v -DQL_DEBUG
!endif

# Primary target:
EuropeanOption$(_D).exe: EuropeanOption.cpp
    bcc32 $(CC_OPTS) -L$(QL_LIB_DIR) -oEuropeanOption$(_D).obj -eEuropeanOption$(_D).exe EuropeanOption.cpp QuantLib$(_D).lib

# Clean up
clean::
    if exist *.obj   del /q *.obj
    if exist *.tds   del /q *.tds
    if exist *.exe   del /q *.exe
