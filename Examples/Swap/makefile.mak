
#
# makefile for swapvaluation.cpp under Borland C++

.autodepend
.silent

# Debug version
!ifdef DEBUG
    _D = _d
!endif

# Directories
QL_INCLUDE_DIR   = "$(QL_DIR)"
QL_LIB_DIR       = "$(QL_DIR)\lib\Win32\Borland"
BCC_INCLUDE      = $(MAKEDIR)\..\include
BCC_LIBS         = $(MAKEDIR)\..\lib

#Warning W8057 : Parameter 'argc' is never used in function main(int,char * *)

# Options
CC_OPTS = -vi- -w-8057 \
    -I$(QL_INCLUDE_DIR) \
    -I$(BCC_INCLUDE)
!ifdef DEBUG
CC_OPTS = $(CC_OPTS) -v -DQL_DEBUG
!endif

# Primary target:
SwapValuation$(_D).exe: swapvaluation.cpp
    bcc32 $(CC_OPTS) -L$(QL_LIB_DIR) -L$(BCC_LIBS) -oSwapValuation$(_D).obj -eSwapValuation$(_D).exe swapvaluation.cpp QuantLib$(_D).lib

# Clean up
clean::
    if exist *.obj   del /q *.obj
    if exist *.tds   del /q *.tds
    if exist *.exe   del /q *.exe
