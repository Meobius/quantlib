
.autodepend
.silent

# Debug version
!ifdef DEBUG
    _D = _d
!endif

# Directories
INCLUDE_DIR    = ..\..
#SRCDIR         = "."
#OBJDIR         = "..\..\build\Borland"
BCC_INCLUDE    = $(MAKEDIR)\..\include
BCC_LIBS       = $(MAKEDIR)\..\lib

# Object files
OBJS = \
    calibrationhelper.obj$(_D) \
    model.obj$(_D) \
    onefactormodel.obj$(_D) \
    twofactormodel.obj$(_D)

# Tools to be used
CC        = bcc32
TLIB      = tlib


# Options
CC_OPTS        = -vi- -q -c -tWM \
    -I$(INCLUDE_DIR) \
    -I$(BCC_INCLUDE)

!ifdef DEBUG
CC_OPTS = $(CC_OPTS) -v -DQL_DEBUG
!endif
!ifdef SAFE
CC_OPTS = $(CC_OPTS) -DQL_EXTRA_SAFETY_CHECKS
!endif

TLIB_OPTS    = /P128
!ifdef DEBUG
TLIB_OPTS    = /P128
!endif

# Generic rules
.cpp.obj:
    $(CC) $(CC_OPTS) $<
.cpp.obj$(_D):
    $(CC) $(CC_OPTS) -o$@ $<

# Primary target:
# static library
ShortRateModels$(_D).lib:: $(OBJS)
    if exist ShortRateModels$(_D).lib     del ShortRateModels$(_D).lib
    $(TLIB) $(TLIB_OPTS) ShortRateModels$(_D).lib /a $(OBJS)





# Clean up
clean::
    if exist *.obj         del /q *.obj
    if exist *.obj$(_D)    del /q *.obj
    if exist *.lib         del /q *.lib
