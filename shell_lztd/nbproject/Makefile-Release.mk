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
FC=
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_CONF=Release
CND_DISTDIR=dist

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/431032908/HuffmanIndexMapTest.o \
	${OBJECTDIR}/_ext/1886800203/TestException.o \
	${OBJECTDIR}/_ext/431032908/LzTrieDictTest.o \
	${OBJECTDIR}/_ext/1145054952/serialization.o \
	${OBJECTDIR}/_ext/1145054952/BitSequence.o \
	${OBJECTDIR}/_ext/1886800203/StackTrace.o \
	${OBJECTDIR}/_ext/1239015908/na_utils.o \
	${OBJECTDIR}/_ext/754122250/etimer.o \
	${OBJECTDIR}/lztd.o \
	${OBJECTDIR}/_ext/1145054952/BitSequenceArray.o \
	${OBJECTDIR}/_ext/754122250/utils.o \
	${OBJECTDIR}/_ext/1886800203/DebugException.o \
	${OBJECTDIR}/_ext/1145054952/BitVector.o \
	${OBJECTDIR}/_ext/754122250/TempFile.o \
	${OBJECTDIR}/_ext/283673646/HuffmanTest.o \
	${OBJECTDIR}/_ext/303287594/instantiator.o \
	${OBJECTDIR}/_ext/529248358/HuffmanDecoder.o \
	${OBJECTDIR}/_ext/140826260/char_symbol.o \
	${OBJECTDIR}/_ext/754122250/Timer.o \
	${OBJECTDIR}/_ext/1145054952/SerializationUtils.o \
	${OBJECTDIR}/_ext/1879633229/CmmExporter.o \
	${OBJECTDIR}/_ext/1145054952/BitPointer.o \
	${OBJECTDIR}/_ext/2022600590/BitSequenceArraySer.o \
	${OBJECTDIR}/_ext/1377768871/NodeArrayBrowser.o \
	${OBJECTDIR}/_ext/529248358/HuffmanCodecCreator.o \
	${OBJECTDIR}/_ext/529248358/HuffmanCoder.o \
	${OBJECTDIR}/_ext/102078018/FileReaderTest.o \
	${OBJECTDIR}/_ext/1359100091/CmmExporterTest.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-Wno-deprecated
CXXFLAGS=-fpermissive -Wno-deprecated

# C++ optimization
#CPPOPT=-g
CPPOPT=-Ofast -march=native

# Link Libraries and Options
LDLIBSOPTIONS=-flto -Ofast

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-Release.mk dist/Release/GNU-Linux-x86/lztd

dist/Release/GNU-Linux-x86/lztd: ${OBJECTFILES}
	${MKDIR} -p dist/Release/GNU-Linux-x86
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/lztd ${OBJECTFILES} ${LDLIBSOPTIONS} 
	

${OBJECTDIR}/_ext/431032908/HuffmanIndexMapTest.o: ../lzt_core/dictionary/lztrie_dict/utest/HuffmanIndexMapTest.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/431032908
	${RM} $@.d
	$(COMPILE.cc) ${CPPOPT} -I../lzt_core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/431032908/HuffmanIndexMapTest.o ../lzt_core/dictionary/lztrie_dict/utest/HuffmanIndexMapTest.cpp

${OBJECTDIR}/_ext/1886800203/TestException.o: ../lzt_core/debug/TestException.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1886800203
	${RM} $@.d
	$(COMPILE.cc) ${CPPOPT} -I../lzt_core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1886800203/TestException.o ../lzt_core/debug/TestException.cpp

${OBJECTDIR}/_ext/431032908/LzTrieDictTest.o: ../lzt_core/dictionary/lztrie_dict/utest/LzTrieDictTest.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/431032908
	${RM} $@.d
	$(COMPILE.cc) ${CPPOPT} -I../lzt_core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/431032908/LzTrieDictTest.o ../lzt_core/dictionary/lztrie_dict/utest/LzTrieDictTest.cpp

${OBJECTDIR}/_ext/1145054952/serialization.o: ../lzt_core/serialization/serialization.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1145054952
	${RM} $@.d
	$(COMPILE.cc) ${CPPOPT} -I../lzt_core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1145054952/serialization.o ../lzt_core/serialization/serialization.cpp

${OBJECTDIR}/_ext/1145054952/BitSequence.o: ../lzt_core/serialization/BitSequence.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1145054952
	${RM} $@.d
	$(COMPILE.cc) ${CPPOPT} -I../lzt_core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1145054952/BitSequence.o ../lzt_core/serialization/BitSequence.cpp

${OBJECTDIR}/_ext/1886800203/StackTrace.o: ../lzt_core/debug/StackTrace.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1886800203
	${RM} $@.d
	$(COMPILE.cc) ${CPPOPT} -I../lzt_core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1886800203/StackTrace.o ../lzt_core/debug/StackTrace.cpp

${OBJECTDIR}/_ext/1239015908/na_utils.o: ../lzt_core/node_array/na_utils.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1239015908
	${RM} $@.d
	$(COMPILE.cc) ${CPPOPT} -I../lzt_core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1239015908/na_utils.o ../lzt_core/node_array/na_utils.cpp

${OBJECTDIR}/lztd.o: lztd.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) ${CPPOPT} -I../lzt_core -MMD -MP -MF $@.d -o ${OBJECTDIR}/lztd.o lztd.cpp

${OBJECTDIR}/_ext/1145054952/BitSequenceArray.o: ../lzt_core/serialization/BitSequenceArray.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1145054952
	${RM} $@.d
	$(COMPILE.cc) ${CPPOPT} -I../lzt_core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1145054952/BitSequenceArray.o ../lzt_core/serialization/BitSequenceArray.cpp

${OBJECTDIR}/_ext/754122250/utils.o: ../lzt_core/util/utils.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/754122250
	${RM} $@.d
	$(COMPILE.cc) ${CPPOPT} -I../lzt_core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/754122250/utils.o ../lzt_core/util/utils.cpp

${OBJECTDIR}/_ext/1886800203/DebugException.o: ../lzt_core/debug/DebugException.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1886800203
	${RM} $@.d
	$(COMPILE.cc) ${CPPOPT} -I../lzt_core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1886800203/DebugException.o ../lzt_core/debug/DebugException.cpp

${OBJECTDIR}/_ext/1145054952/BitVector.o: ../lzt_core/serialization/BitVector.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1145054952
	${RM} $@.d
	$(COMPILE.cc) ${CPPOPT} -I../lzt_core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1145054952/BitVector.o ../lzt_core/serialization/BitVector.cpp

${OBJECTDIR}/_ext/754122250/TempFile.o: ../lzt_core/util/TempFile.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/754122250
	${RM} $@.d
	$(COMPILE.cc) ${CPPOPT} -I../lzt_core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/754122250/TempFile.o ../lzt_core/util/TempFile.cpp

${OBJECTDIR}/_ext/283673646/HuffmanTest.o: ../lzt_core/dictionary/lztrie_dict/huffman/utest/HuffmanTest.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/283673646
	${RM} $@.d
	$(COMPILE.cc) ${CPPOPT} -I../lzt_core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/283673646/HuffmanTest.o ../lzt_core/dictionary/lztrie_dict/huffman/utest/HuffmanTest.cpp

${OBJECTDIR}/_ext/303287594/instantiator.o: ../lzt_core/node_array/concepts/instantiator.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/303287594
	${RM} $@.d
	$(COMPILE.cc) ${CPPOPT} -I../lzt_core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/303287594/instantiator.o ../lzt_core/node_array/concepts/instantiator.cpp

${OBJECTDIR}/_ext/529248358/HuffmanDecoder.o: ../lzt_core/dictionary/lztrie_dict/huffman/HuffmanDecoder.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/529248358
	${RM} $@.d
	$(COMPILE.cc) ${CPPOPT} -I../lzt_core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/529248358/HuffmanDecoder.o ../lzt_core/dictionary/lztrie_dict/huffman/HuffmanDecoder.cpp

${OBJECTDIR}/_ext/140826260/char_symbol.o: ../lzt_core/node_array/types/symbol/char/char_symbol.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/140826260
	${RM} $@.d
	$(COMPILE.cc) ${CPPOPT} -I../lzt_core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/140826260/char_symbol.o ../lzt_core/node_array/types/symbol/char/char_symbol.cpp

${OBJECTDIR}/_ext/754122250/Timer.o: ../lzt_core/util/Timer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/754122250
	${RM} $@.d
	$(COMPILE.cc) ${CPPOPT} -I../lzt_core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/754122250/Timer.o ../lzt_core/util/Timer.cpp

${OBJECTDIR}/_ext/1145054952/SerializationUtils.o: ../lzt_core/serialization/SerializationUtils.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1145054952
	${RM} $@.d
	$(COMPILE.cc) ${CPPOPT} -I../lzt_core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1145054952/SerializationUtils.o ../lzt_core/serialization/SerializationUtils.cpp

${OBJECTDIR}/_ext/1879633229/CmmExporter.o: ../lzt_core/node_array/util/CmmExporter.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1879633229
	${RM} $@.d
	$(COMPILE.cc) ${CPPOPT} -I../lzt_core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1879633229/CmmExporter.o ../lzt_core/node_array/util/CmmExporter.cpp

${OBJECTDIR}/_ext/1145054952/BitPointer.o: ../lzt_core/serialization/BitPointer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1145054952
	${RM} $@.d
	$(COMPILE.cc) ${CPPOPT} -I../lzt_core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1145054952/BitPointer.o ../lzt_core/serialization/BitPointer.cpp

${OBJECTDIR}/_ext/2022600590/BitSequenceArraySer.o: ../lzt_core/serialization/array/BitSequenceArraySer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2022600590
	${RM} $@.d
	$(COMPILE.cc) ${CPPOPT} -I../lzt_core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/2022600590/BitSequenceArraySer.o ../lzt_core/serialization/array/BitSequenceArraySer.cpp

${OBJECTDIR}/_ext/1377768871/NodeArrayBrowser.o: ../lzt_core/node_array/na_browser/NodeArrayBrowser.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1377768871
	${RM} $@.d
	$(COMPILE.cc) ${CPPOPT} -I../lzt_core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1377768871/NodeArrayBrowser.o ../lzt_core/node_array/na_browser/NodeArrayBrowser.cpp

${OBJECTDIR}/_ext/529248358/HuffmanCodecCreator.o: ../lzt_core/dictionary/lztrie_dict/huffman/HuffmanCodecCreator.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/529248358
	${RM} $@.d
	$(COMPILE.cc) ${CPPOPT} -I../lzt_core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/529248358/HuffmanCodecCreator.o ../lzt_core/dictionary/lztrie_dict/huffman/HuffmanCodecCreator.cpp

${OBJECTDIR}/_ext/529248358/HuffmanCoder.o: ../lzt_core/dictionary/lztrie_dict/huffman/HuffmanCoder.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/529248358
	${RM} $@.d
	$(COMPILE.cc) ${CPPOPT} -I../lzt_core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/529248358/HuffmanCoder.o ../lzt_core/dictionary/lztrie_dict/huffman/HuffmanCoder.cpp

${OBJECTDIR}/_ext/102078018/FileReaderTest.o: ../lzt_core/util/utest/FileReaderTest.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/102078018
	${RM} $@.d
	$(COMPILE.cc) ${CPPOPT} -I../lzt_core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/102078018/FileReaderTest.o ../lzt_core/util/utest/FileReaderTest.cpp

${OBJECTDIR}/_ext/1359100091/CmmExporterTest.o: ../lzt_core/node_array/util/utest/CmmExporterTest.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1359100091
	${RM} $@.d
	$(COMPILE.cc) ${CPPOPT} -I../lzt_core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1359100091/CmmExporterTest.o ../lzt_core/node_array/util/utest/CmmExporterTest.cpp

${OBJECTDIR}/_ext/754122250/etimer.o: ../lzt_core/util/etimer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/754122250
	${RM} $@.d
	$(COMPILE.cc) ${CPPOPT} -I../lzt_core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/754122250/etimer.o ../lzt_core/util/etimer.cpp	
	
# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Release
	${RM} dist/Release/GNU-Linux-x86/lztd

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
