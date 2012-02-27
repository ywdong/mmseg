// Copyright 2011. All Rights Reserved.
// Author: shunpingye@gmail.com (Shunping Ye)

/*
How to compile this module:
#include <Python.h>
#include <boost/python.hpp>
using namespace boost::python;
g++ -shared -fPIC wap_mmseg.cc -o wrap_mmseg.so -lboost_python -I/usr/include/python2.6 -I/usr/local/include/mmseg
cp ye_mmseg.so /usr/local/lib/python2.6/dist-packages/
*/

#include <Python.h>
#include <boost/python.hpp>

using namespace boost::python;

#include "SegmenterManager.h"
#include "Segmenter.h"
#include "segmenter_token.h"

using css::SegmenterManager;
using css::Segmenter;
using css::SegmentedToken;


BOOST_PYTHON_MODULE(mmseg)
{
  class_<SegmentedToken, boost::noncopyable > ("SegmentedToken", "This is a SegmentedToken project python extend", init<>())
            .def("begin", &SegmentedToken::begin)
            .def("end", &SegmentedToken::end)
            .def("word", &SegmentedToken::word, return_value_policy<copy_const_reference>())
            .def("token_type", &SegmentedToken::types);
  class_<Segmenter, boost::noncopyable > ("Segmenter", "This is a Segmenter project python extend", init<>())
            .def("SetBuffer", &Segmenter::SetBuffer)
            .def("GetNextToken", &Segmenter::GetNextToken);
  class_<SegmenterManager, boost::noncopyable > ("SegmenterManager", "This is a SegmenterManager project python extend", init<>())
            .def("GetSegmenter", &SegmenterManager::getSegmenter, return_value_policy<manage_new_object>())
            .def("Init", &SegmenterManager::init);
}
