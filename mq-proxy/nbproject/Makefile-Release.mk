#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=Cygwin-Windows
CND_DLIB_EXT=dll
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/error.o \
	${OBJECTDIR}/helper.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/multiplexper.o \
	${OBJECTDIR}/packet.o \
	${OBJECTDIR}/processor.o \
	${OBJECTDIR}/proxy_handler.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/mq-proxy.exe

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/mq-proxy.exe: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/mq-proxy ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/error.o: error.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/cygdrive/F/GitHub/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/error.o error.cpp

${OBJECTDIR}/fdhandle.h.gch: fdhandle.h 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/cygdrive/F/GitHub/include -MMD -MP -MF "$@.d" -o "$@" fdhandle.h

${OBJECTDIR}/helper.o: helper.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/cygdrive/F/GitHub/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/helper.o helper.cpp

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/cygdrive/F/GitHub/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/multiplexper.o: multiplexper.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/cygdrive/F/GitHub/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/multiplexper.o multiplexper.cpp

${OBJECTDIR}/packet.o: packet.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/cygdrive/F/GitHub/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/packet.o packet.cpp

${OBJECTDIR}/processor.o: processor.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/cygdrive/F/GitHub/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/processor.o processor.cpp

${OBJECTDIR}/proxy_handler.o: proxy_handler.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/cygdrive/F/GitHub/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/proxy_handler.o proxy_handler.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/mq-proxy.exe

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
