#pragma once


// A macro to disallow the copy constructor and operator= functions
// This should be used in the private: declarations for a class
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);             \
  TypeName& operator=(const TypeName&)

#ifndef ATLTRACENOTIMPL2
#define ATLTRACENOTIMPL2() ATLTRACENOTIMPL(__FUNCTION__"\n")
#endif 

#define ATLTRACEMETHOD() ATLTRACE(_T("%s\n"), __FUNCTION__)