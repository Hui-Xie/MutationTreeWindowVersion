cmake_minimum_required(VERSION 3.5)
project(MutationTree)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
Constraints.cpp
ConstraintsDlg.cpp
CSVFile.cpp
MutationTree.cpp
MutationTreeDlg.cpp
OpenCsvDlg.cpp
stdafx.cpp
TreeDiagram.cpp
TreeFactory.cpp
Constraints.h
ConstraintsDlg.h
CSVFile.h
MutationTree.h
MutationTreeDlg.h
OpenCsvDlg.h
OpenCSVDLRc.h
resource.h
stdafx.h
targetver.h
TreeDiagram.h
TreeFactory.h
VAF_CSVTree_struct.h)

add_executable(MutationTree ${SOURCE_FILES})
