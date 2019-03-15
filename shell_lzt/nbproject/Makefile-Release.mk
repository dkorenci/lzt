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
CND_PLATFORM=GNU-Linux-x86
CND_DLIB_EXT=so
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/431032908/HuffmanIndexMapTest.o \
	${OBJECTDIR}/_ext/1814928920/hash.o \
	${OBJECTDIR}/_ext/1886800203/TestException.o \
	${OBJECTDIR}/_ext/431032908/LzTrieDictTest.o \
	${OBJECTDIR}/_ext/1145054952/serialization.o \
	${OBJECTDIR}/_ext/754122250/etimer.o \
	${OBJECTDIR}/_ext/1145054952/BitSequence.o \
	${OBJECTDIR}/_ext/1886800203/StackTrace.o \
	${OBJECTDIR}/_ext/1239015908/na_utils.o \
	${OBJECTDIR}/_ext/1145054952/BitSequenceArray.o \
	${OBJECTDIR}/_ext/1814928920/morph.o \
	${OBJECTDIR}/_ext/754122250/utils.o \
	${OBJECTDIR}/_ext/1886800203/DebugException.o \
	${OBJECTDIR}/_ext/1145054952/BitVector.o \
	${OBJECTDIR}/_ext/1814928920/build_fsa.o \
	${OBJECTDIR}/_ext/283673646/HuffmanTest.o \
	${OBJECTDIR}/_ext/303287594/instantiator.o \
	${OBJECTDIR}/_ext/529248358/HuffmanDecoder.o \
	${OBJECTDIR}/_ext/1814928920/unode.o \
	${OBJECTDIR}/_ext/1814928920/common.o \
	${OBJECTDIR}/_ext/754122250/TempFile.o \
	${OBJECTDIR}/_ext/1814928920/synth.o \
	${OBJECTDIR}/_ext/140826260/char_symbol.o \
	${OBJECTDIR}/_ext/102078018/TestCaseReaderTest.o \
	${OBJECTDIR}/_ext/163303923/CharStringSA.o \
	${OBJECTDIR}/_ext/1814928920/prefix.o \
	${OBJECTDIR}/_ext/1814928920/guess.o \
	${OBJECTDIR}/_ext/754122250/Timer.o \
	${OBJECTDIR}/_ext/1814928920/spell.o \
	${OBJECTDIR}/_ext/1145054952/SerializationUtils.o \
	${OBJECTDIR}/_ext/1814928920/buildu_fsa.o \
	${OBJECTDIR}/_ext/1879633229/CmmExporter.o \
	${OBJECTDIR}/_ext/1814928920/nindex.o \
	${OBJECTDIR}/_ext/1814928920/nnode.o \
	${OBJECTDIR}/_ext/1145054952/BitPointer.o \
	${OBJECTDIR}/_ext/1814928920/accent.o \
	${OBJECTDIR}/_ext/1814928920/nstr.o \
	${OBJECTDIR}/_ext/2022600590/BitSequenceArraySer.o \
	${OBJECTDIR}/_ext/1377768871/NodeArrayBrowser.o \
	${OBJECTDIR}/_ext/529248358/HuffmanCodecCreator.o \
	${OBJECTDIR}/_ext/720288350/CompressorTest.o \
	${OBJECTDIR}/_ext/529248358/HuffmanCoder.o \
	${OBJECTDIR}/lzt.o \
	${OBJECTDIR}/_ext/102078018/FileReaderTest.o \
	${OBJECTDIR}/_ext/1814928920/visualize.o \
	${OBJECTDIR}/_ext/1359100091/CmmExporterTest.o \
	${OBJECTDIR}/_ext/1814928920/one_word_io.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
#CCFLAGS=-Wno-deprecated -DFLEXIBLE -DGENERALIZE -DSORT_ON_FREQ -DSHOW_FILLERS -DSTOPBIT -DNEXTBIT -DMORE_COMPR -DCASECONV -DRUNON_WORDS -DMORPH_INFIX -DPOOR_MORPH -DCHCLASS -DGUESS_LEXEMES -DGUESS_PREFIX -DGUESS_MMORPH -DDUMP_ALL -DLOOSING_RPM
#CXXFLAGS=-fpermissive -Wno-deprecated -DFLEXIBLE -DGENERALIZE -DSORT_ON_FREQ -DSHOW_FILLERS -DSTOPBIT -DNEXTBIT -DMORE_COMPR -DCASECONV -DRUNON_WORDS -DMORPH_INFIX -DPOOR_MORPH -DCHCLASS -DGUESS_LEXEMES -DGUESS_PREFIX -DGUESS_MMORPH -DDUMP_ALL -DLOOSING_RPM
#CCFLAGS=-Wno-deprecated -DFLEXIBLE -DGENERALIZE -DSORT_ON_FREQ -DSHOW_FILLERS -DSTOPBIT -DNEXTBIT -DMORE_COMPR -DCASECONV -DRUNON_WORDS -DMORPH_INFIX -DPOOR_MORPH -DCHCLASS -DGUESS_LEXEMES -DGUESS_PREFIX -DGUESS_MMORPH -DDUMP_ALL -DLOOSING_RPM
FSAFLAGS= -DFLEXIBLE -DGENERALIZE -DSORT_ON_FREQ -DSHOW_FILLERS -DSTOPBIT -DNEXTBIT -DMORE_COMPR -DCASECONV -DRUNON_WORDS -DMORPH_INFIX -DPOOR_MORPH -DCHCLASS -DGUESS_LEXEMES -DGUESS_PREFIX -DGUESS_MMORPH -DDUMP_ALL -DLOOSING_RPM
#CXXFLAGS=-fpermissive -Wno-deprecated 
CXXFLAGS=-fpermissive -Wno-deprecated ${FSAFLAGS}

# C++ optimization
#CPPOPT=-g
CPPOPT=-Ofast -march=native

# Link Libraries and Options
#LDLIBSOPTIONS=-flto -O2
LDLIBSOPTIONS=-flto -Ofast

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=


# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/lzt

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/lzt: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/lzt ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/_ext/431032908/HuffmanIndexMapTest.o: ../lzt_core/dictionary/lztrie_dict/utest/HuffmanIndexMapTest.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/431032908
	${RM} $@.d
	$(COMPILE.cc) ${CPPOPT} -I../lzt_core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/431032908/HuffmanIndexMapTest.o ../lzt_core/dictionary/lztrie_dict/utest/HuffmanIndexMapTest.cpp

${OBJECTDIR}/_ext/1814928920/hash.o: ../lzt_core/fsa_convert/s_fsa_subset/hash.cc 
	${MKDIR} -p ${OBJECTDIR}/_ext/1814928920
	${RM} $@.d
	$(COMPILE.cc) ${CPPOPT} -I../lzt_core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1814928920/hash.o ../lzt_core/fsa_convert/s_fsa_subset/hash.cc

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

${OBJECTDIR}/_ext/754122250/etimer.o: ../lzt_core/util/etimer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/754122250
	${RM} $@.d
	$(COMPILE.cc) ${CPPOPT} -I../lzt_core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/754122250/etimer.o ../lzt_core/util/etimer.cpp

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

${OBJECTDIR}/_ext/1145054952/BitSequenceArray.o: ../lzt_core/serialization/BitSequenceArray.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1145054952
	${RM} $@.d
	$(COMPILE.cc) ${CPPOPT} -I../lzt_core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1145054952/BitSequenceArray.o ../lzt_core/serialization/BitSequenceArray.cpp

${OBJECTDIR}/_ext/1814928920/morph.o: ../lzt_core/fsa_convert/s_fsa_subset/morph.cc 
	${MKDIR} -p ${OBJECTDIR}/_ext/1814928920
	${RM} $@.d
	$(COMPILE.cc) ${CPPOPT} -I../lzt_core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1814928920/morph.o ../lzt_core/fsa_convert/s_fsa_subset/morph.cc

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

${OBJECTDIR}/_ext/1814928920/build_fsa.o: ../lzt_core/fsa_convert/s_fsa_subset/build_fsa.cc 
	${MKDIR} -p ${OBJECTDIR}/_ext/1814928920
	${RM} $@.d
	$(COMPILE.cc) ${CPPOPT} -I../lzt_core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1814928920/build_fsa.o ../lzt_core/fsa_convert/s_fsa_subset/build_fsa.cc

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

${OBJECTDIR}/_ext/1814928920/unode.o: ../lzt_core/fsa_convert/s_fsa_subset/unode.cc 
	${MKDIR} -p ${OBJECTDIR}/_ext/1814928920
	${RM} $@.d
	$(COMPILE.cc) ${CPPOPT} -I../lzt_core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1814928920/unode.o ../lzt_core/fsa_convert/s_fsa_subset/unode.cc

${OBJECTDIR}/_ext/1814928920/common.o: ../lzt_core/fsa_convert/s_fsa_subset/common.cc 
	${MKDIR} -p ${OBJECTDIR}/_ext/1814928920
	${RM} $@.d
	$(COMPILE.cc) ${CPPOPT} -I../lzt_core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1814928920/common.o ../lzt_core/fsa_convert/s_fsa_subset/common.cc

${OBJECTDIR}/_ext/754122250/TempFile.o: ../lzt_core/util/TempFile.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/754122250
	${RM} $@.d
	$(COMPILE.cc) ${CPPOPT} -I../lzt_core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/754122250/TempFile.o ../lzt_core/util/TempFile.cpp

${OBJECTDIR}/_ext/1814928920/synth.o: ../lzt_core/fsa_convert/s_fsa_subset/synth.cc 
	${MKDIR} -p ${OBJECTDIR}/_ext/1814928920
	${RM} $@.d
	$(COMPILE.cc) ${CPPOPT} -I../lzt_core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1814928920/synth.o ../lzt_core/fsa_convert/s_fsa_subset/synth.cc

${OBJECTDIR}/_ext/140826260/char_symbol.o: ../lzt_core/node_array/types/symbol/char/char_symbol.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/140826260
	${RM} $@.d
	$(COMPILE.cc) ${CPPOPT} -I../lzt_core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/140826260/char_symbol.o ../lzt_core/node_array/types/symbol/char/char_symbol.cpp

${OBJECTDIR}/_ext/102078018/TestCaseReaderTest.o: ../lzt_core/util/utest/TestCaseReaderTest.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/102078018
	${RM} $@.d
	$(COMPILE.cc) ${CPPOPT} -I../lzt_core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/102078018/TestCaseReaderTest.o ../lzt_core/util/utest/TestCaseReaderTest.cpp

${OBJECTDIR}/_ext/163303923/CharStringSA.o: ../lzt_core/suffix_array/CharStringSA.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/163303923
	${RM} $@.d
	$(COMPILE.cc) ${CPPOPT} -I../lzt_core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/163303923/CharStringSA.o ../lzt_core/suffix_array/CharStringSA.cpp

${OBJECTDIR}/_ext/1814928920/prefix.o: ../lzt_core/fsa_convert/s_fsa_subset/prefix.cc 
	${MKDIR} -p ${OBJECTDIR}/_ext/1814928920
	${RM} $@.d
	$(COMPILE.cc) ${CPPOPT} -I../lzt_core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1814928920/prefix.o ../lzt_core/fsa_convert/s_fsa_subset/prefix.cc

${OBJECTDIR}/_ext/1814928920/guess.o: ../lzt_core/fsa_convert/s_fsa_subset/guess.cc 
	${MKDIR} -p ${OBJECTDIR}/_ext/1814928920
	${RM} $@.d
	$(COMPILE.cc) ${CPPOPT} -I../lzt_core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1814928920/guess.o ../lzt_core/fsa_convert/s_fsa_subset/guess.cc

${OBJECTDIR}/_ext/754122250/Timer.o: ../lzt_core/util/Timer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/754122250
	${RM} $@.d
	$(COMPILE.cc) ${CPPOPT} -I../lzt_core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/754122250/Timer.o ../lzt_core/util/Timer.cpp

${OBJECTDIR}/_ext/1814928920/spell.o: ../lzt_core/fsa_convert/s_fsa_subset/spell.cc 
	${MKDIR} -p ${OBJECTDIR}/_ext/1814928920
	${RM} $@.d
	$(COMPILE.cc) ${CPPOPT} -I../lzt_core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1814928920/spell.o ../lzt_core/fsa_convert/s_fsa_subset/spell.cc

${OBJECTDIR}/_ext/1145054952/SerializationUtils.o: ../lzt_core/serialization/SerializationUtils.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1145054952
	${RM} $@.d
	$(COMPILE.cc) ${CPPOPT} -I../lzt_core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1145054952/SerializationUtils.o ../lzt_core/serialization/SerializationUtils.cpp

${OBJECTDIR}/_ext/1814928920/buildu_fsa.o: ../lzt_core/fsa_convert/s_fsa_subset/buildu_fsa.cc 
	${MKDIR} -p ${OBJECTDIR}/_ext/1814928920
	${RM} $@.d
	$(COMPILE.cc) ${CPPOPT} -I../lzt_core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1814928920/buildu_fsa.o ../lzt_core/fsa_convert/s_fsa_subset/buildu_fsa.cc

${OBJECTDIR}/_ext/1879633229/CmmExporter.o: ../lzt_core/node_array/util/CmmExporter.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1879633229
	${RM} $@.d
	$(COMPILE.cc) ${CPPOPT} -I../lzt_core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1879633229/CmmExporter.o ../lzt_core/node_array/util/CmmExporter.cpp

${OBJECTDIR}/_ext/1814928920/nindex.o: ../lzt_core/fsa_convert/s_fsa_subset/nindex.cc 
	${MKDIR} -p ${OBJECTDIR}/_ext/1814928920
	${RM} $@.d
	$(COMPILE.cc) ${CPPOPT} -I../lzt_core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1814928920/nindex.o ../lzt_core/fsa_convert/s_fsa_subset/nindex.cc

${OBJECTDIR}/_ext/1814928920/nnode.o: ../lzt_core/fsa_convert/s_fsa_subset/nnode.cc 
	${MKDIR} -p ${OBJECTDIR}/_ext/1814928920
	${RM} $@.d
	$(COMPILE.cc) ${CPPOPT} -I../lzt_core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1814928920/nnode.o ../lzt_core/fsa_convert/s_fsa_subset/nnode.cc

${OBJECTDIR}/_ext/1145054952/BitPointer.o: ../lzt_core/serialization/BitPointer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1145054952
	${RM} $@.d
	$(COMPILE.cc) ${CPPOPT} -I../lzt_core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1145054952/BitPointer.o ../lzt_core/serialization/BitPointer.cpp

${OBJECTDIR}/_ext/1814928920/accent.o: ../lzt_core/fsa_convert/s_fsa_subset/accent.cc 
	${MKDIR} -p ${OBJECTDIR}/_ext/1814928920
	${RM} $@.d
	$(COMPILE.cc) ${CPPOPT} -I../lzt_core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1814928920/accent.o ../lzt_core/fsa_convert/s_fsa_subset/accent.cc

${OBJECTDIR}/_ext/1814928920/nstr.o: ../lzt_core/fsa_convert/s_fsa_subset/nstr.cc 
	${MKDIR} -p ${OBJECTDIR}/_ext/1814928920
	${RM} $@.d
	$(COMPILE.cc) ${CPPOPT} -I../lzt_core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1814928920/nstr.o ../lzt_core/fsa_convert/s_fsa_subset/nstr.cc

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

${OBJECTDIR}/_ext/720288350/CompressorTest.o: ../lzt_core/compress/utest/CompressorTest.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/720288350
	${RM} $@.d
	$(COMPILE.cc) ${CPPOPT} -I../lzt_core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/720288350/CompressorTest.o ../lzt_core/compress/utest/CompressorTest.cpp

${OBJECTDIR}/_ext/529248358/HuffmanCoder.o: ../lzt_core/dictionary/lztrie_dict/huffman/HuffmanCoder.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/529248358
	${RM} $@.d
	$(COMPILE.cc) ${CPPOPT} -I../lzt_core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/529248358/HuffmanCoder.o ../lzt_core/dictionary/lztrie_dict/huffman/HuffmanCoder.cpp

${OBJECTDIR}/lzt.o: lzt.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) ${CPPOPT} -I../lzt_core -MMD -MP -MF $@.d -o ${OBJECTDIR}/lzt.o lzt.cpp

${OBJECTDIR}/_ext/102078018/FileReaderTest.o: ../lzt_core/util/utest/FileReaderTest.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/102078018
	${RM} $@.d
	$(COMPILE.cc) ${CPPOPT} -I../lzt_core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/102078018/FileReaderTest.o ../lzt_core/util/utest/FileReaderTest.cpp

${OBJECTDIR}/_ext/1814928920/visualize.o: ../lzt_core/fsa_convert/s_fsa_subset/visualize.cc 
	${MKDIR} -p ${OBJECTDIR}/_ext/1814928920
	${RM} $@.d
	$(COMPILE.cc) ${CPPOPT} -I../lzt_core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1814928920/visualize.o ../lzt_core/fsa_convert/s_fsa_subset/visualize.cc

${OBJECTDIR}/_ext/1359100091/CmmExporterTest.o: ../lzt_core/node_array/util/utest/CmmExporterTest.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1359100091
	${RM} $@.d
	$(COMPILE.cc) ${CPPOPT} -I../lzt_core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1359100091/CmmExporterTest.o ../lzt_core/node_array/util/utest/CmmExporterTest.cpp

${OBJECTDIR}/_ext/1814928920/one_word_io.o: ../lzt_core/fsa_convert/s_fsa_subset/one_word_io.cc 
	${MKDIR} -p ${OBJECTDIR}/_ext/1814928920
	${RM} $@.d
	$(COMPILE.cc) ${CPPOPT} -I../lzt_core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1814928920/one_word_io.o ../lzt_core/fsa_convert/s_fsa_subset/one_word_io.cc

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/lzt

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
