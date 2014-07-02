/**
 * @author Manuel Guenther <manuel.guenther@idiap.ch>
 * @date Mon Jun 30 19:58:28 CEST 2014
 *
 * @brief Binds the FaceEyesNorm class to python
 *
 * Copyright (C) 2011-2014 Idiap Research Institute, Martigny, Switzerland
 */

#include "main.h"

/******************************************************************/
/************ Constructor Section *********************************/
/******************************************************************/

static auto FaceEyesNorm_doc = bob::extension::ClassDoc(
  BOB_EXT_MODULE_PREFIX ".FaceEyesNorm",
  "Objects of this class, after configuration, can perform a geometric normalization of facial images based on their eye positions",
  "The geometric normalization is a combination of rotation, scaling and cropping an image. "
  "The underlying implementation relies on a :py:class:`bob.ip.base.GeomNorm` object to perform the actual geometric normalization."
).add_constructor(
  bob::extension::FunctionDoc(
    "__init__",
    "Constructs a FaceEyesNorm object.",
    "Basically there exist two ways to define a FaceEyesNorm. "
    "Both ways require the resulting ``crop_size``. "
    "The first constructor takes the inter-eye-distance and the center of the eyes, which will be used as transformation center. "
    "The second version takes the image resolution and **two arbitrary** positions in the face, with which the image will be aligned. "
    "Usually, these positions are the eyes, but any other pair (like mouth and eye for profile faces) can be specified.",
    true
  )
  .add_prototype("crop_size, eyes_distance, eyes_center", "")
  .add_prototype("crop_size, right_eye, left_eye", "")
  .add_prototype("other", "")
  .add_parameter("crop_size", "(int, int)", "The resolution of the **normalized face**")
  .add_parameter("eyes_distance", "float", "The inter-eye-distance in the **normalized face**")
  .add_parameter("eyes_center", "(float, float)", "The center point between the eyes in the **normalized face**")
  .add_parameter("right_eye", "(float, float)", "The location of the right eye (or another fix point) in the normalized image")
  .add_parameter("left_eye", "(float, float)", "The location of the left eye (or another fix point) in the normalized image")
  .add_parameter("other", ":py:class:`FaceEyesNorm`", "Another FaceEyesNorm object to copy")
);


static int PyBobIpBaseFaceEyesNorm_init(PyBobIpBaseFaceEyesNormObject* self, PyObject* args, PyObject* kwargs) {
  TRY

  char* kwlist1[] = {c("crop_size"), c("eyes_distance"), c("eyes_center"), NULL};
  char* kwlist2[] = {c("crop_size"), c("right_eye"), c("left_eye"), NULL};
  char* kwlist3[] = {c("other"), NULL};

  // get the number of command line arguments
  Py_ssize_t nargs = (args?PyTuple_Size(args):0) + (kwargs?PyDict_Size(kwargs):0);

  switch (nargs){
    case 1:{
      // copy constructor
      PyBobIpBaseFaceEyesNormObject* faceEyesNorm;
      if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O!", kwlist3, &PyBobIpBaseFaceEyesNormType, &faceEyesNorm)){
        FaceEyesNorm_doc.print_usage();
        return -1;
      }
      self->cxx.reset(new bob::ip::base::FaceEyesNorm(*faceEyesNorm->cxx));
      return 0;
    } // nargs == 1
    case 3:{
      // check the second parameter
      PyObject* k = Py_BuildValue("s", kwlist2[1]);
      auto k_ = make_safe(k);
      if ((args && PyTuple_Size(args) > 1 && PySequence_Check(PyList_GET_ITEM(args,1))) ||
          (kwargs && PyDict_Contains(kwargs, k))){
        // with two eyes
        blitz::TinyVector<int,2> size;
        blitz::TinyVector<double,2> right, left;
        if (!PyArg_ParseTupleAndKeywords(args, kwargs, "(ii)(dd)(dd)", kwlist2, &size[0], &size[1], &right[0], &right[1], &left[0], &left[1])){
          FaceEyesNorm_doc.print_usage();
          return -1;
        }
        self->cxx.reset(new bob::ip::base::FaceEyesNorm(size, right, left));
        return 0;
      } else {
        // with eye distance
        blitz::TinyVector<int,2> size;
        blitz::TinyVector<double,2> center;
        double dist;
        if (!PyArg_ParseTupleAndKeywords(args, kwargs, "(ii)d(dd)", kwlist1, &size[0], &size[1], &dist, &center[0], &center[1])){
          FaceEyesNorm_doc.print_usage();
          return -1;
        }
        self->cxx.reset(new bob::ip::base::FaceEyesNorm(size, dist, center));
        return 0;
      }
    }
    default:
      // unknown
      FaceEyesNorm_doc.print_usage();
      PyErr_Format(PyExc_TypeError, "`%s' got an unsupported number of parameters", Py_TYPE(self)->tp_name);
      return -1;
  }

  CATCH("cannot create FaceEyesNorm object", -1)
}

static void PyBobIpBaseFaceEyesNorm_delete(PyBobIpBaseFaceEyesNormObject* self) {
  Py_TYPE(self)->tp_free((PyObject*)self);
}

int PyBobIpBaseFaceEyesNorm_Check(PyObject* o) {
  return PyObject_IsInstance(o, reinterpret_cast<PyObject*>(&PyBobIpBaseFaceEyesNormType));
}

static PyObject* PyBobIpBaseFaceEyesNorm_RichCompare(PyBobIpBaseFaceEyesNormObject* self, PyObject* other, int op) {
  TRY

  if (!PyBobIpBaseFaceEyesNorm_Check(other)) {
    PyErr_Format(PyExc_TypeError, "cannot compare `%s' with `%s'", Py_TYPE(self)->tp_name, Py_TYPE(other)->tp_name);
    return 0;
  }
  auto other_ = reinterpret_cast<PyBobIpBaseFaceEyesNormObject*>(other);
  switch (op) {
    case Py_EQ:
      if (*self->cxx==*other_->cxx) Py_RETURN_TRUE; else Py_RETURN_FALSE;
    case Py_NE:
      if (*self->cxx==*other_->cxx) Py_RETURN_FALSE; else Py_RETURN_TRUE;
    default:
      Py_INCREF(Py_NotImplemented);
      return Py_NotImplemented;
  }
  CATCH("cannot compare FaceEyesNorm objects", 0)
}


/******************************************************************/
/************ Variables Section ***********************************/
/******************************************************************/
static auto eyesDistance = bob::extension::VariableDoc(
  "eyes_distance",
  "float",
  "The distance between the eyes in the normalized image, with read and write access"
);
PyObject* PyBobIpBaseFaceEyesNorm_getEyesDistance(PyBobIpBaseFaceEyesNormObject* self, void*){
  TRY
  return Py_BuildValue("d", self->cxx->getEyesDistance());
  CATCH("eyes_distance could not be read", 0)
}
int PyBobIpBaseFaceEyesNorm_setEyesDistance(PyBobIpBaseFaceEyesNormObject* self, PyObject* value, void*){
  TRY
  double d = PyFloat_AsDouble(value);
  if (PyErr_Occurred()) return -1;
  self->cxx->setEyesDistance(d);
  return 0;
  CATCH("eyes_distance could not be set", -1)
}

static auto eyesAngle = bob::extension::VariableDoc(
  "eyes_angle",
  "float",
  "The angle between the eyes in the normalized image (relative to the horizontal line), with read and write access"
);
PyObject* PyBobIpBaseFaceEyesNorm_getEyesAngle(PyBobIpBaseFaceEyesNormObject* self, void*){
  TRY
  return Py_BuildValue("d", self->cxx->getEyesAngle());
  CATCH("eyes_angle could not be read", 0)
}
int PyBobIpBaseFaceEyesNorm_setEyesAngle(PyBobIpBaseFaceEyesNormObject* self, PyObject* value, void*){
  TRY
  double d = PyFloat_AsDouble(value);
  if (PyErr_Occurred()) return -1;
  self->cxx->setEyesAngle(d);
  return 0;
  CATCH("eyes_angle could not be set", -1)
}

static auto cropSize = bob::extension::VariableDoc(
  "crop_size",
  "(int, int)",
  "The size of the normalized image, with read and write access"
);
PyObject* PyBobIpBaseFaceEyesNorm_getCropSize(PyBobIpBaseFaceEyesNormObject* self, void*){
  TRY
  auto r = self->cxx->getCropSize();
  return Py_BuildValue("(ii)", r[0], r[1]);
  CATCH("crop_size could not be read", 0)
}
int PyBobIpBaseFaceEyesNorm_setCropSize(PyBobIpBaseFaceEyesNormObject* self, PyObject* value, void*){
  TRY
  blitz::TinyVector<double,2> r;
  if (!PyArg_ParseTuple(value, "dd", &r[0], &r[1])){
    PyErr_Format(PyExc_RuntimeError, "%s %s expects a tuple of two floats", Py_TYPE(self)->tp_name, cropSize.name());
    return -1;
  }
  self->cxx->setCropSize(r);
  return 0;
  CATCH("crop_size could not be set", -1)
}

static auto cropOffset = bob::extension::VariableDoc(
  "crop_offset",
  "(float, float)",
  "The transformation center in the processed image, which is usually the center between the eyes; with read and write access"
);
PyObject* PyBobIpBaseFaceEyesNorm_getCropOffset(PyBobIpBaseFaceEyesNormObject* self, void*){
  TRY
  auto r = self->cxx->getCropOffset();
  return Py_BuildValue("(dd)", r[0], r[1]);
  CATCH("crop_offset could not be read", 0)
}
int PyBobIpBaseFaceEyesNorm_setCropOffset(PyBobIpBaseFaceEyesNormObject* self, PyObject* value, void*){
  TRY
  blitz::TinyVector<double,2> r;
  if (!PyArg_ParseTuple(value, "dd", &r[0], &r[1])){
    PyErr_Format(PyExc_RuntimeError, "%s %s expects a tuple of two floats", Py_TYPE(self)->tp_name, cropOffset.name());
    return -1;
  }
  self->cxx->setCropOffset(r);
  return 0;
  CATCH("crop_offset could not be set", -1)
}

static auto lastAngle = bob::extension::VariableDoc(
  "last_angle",
  "float",
  "The rotation angle that was applied on the latest normalized image, read access only"
);
PyObject* PyBobIpBaseFaceEyesNorm_getLastAngle(PyBobIpBaseFaceEyesNormObject* self, void*){
  TRY
  return Py_BuildValue("d", self->cxx->getLastAngle());
  CATCH("last_angle could not be read", 0)
}

static auto lastScale = bob::extension::VariableDoc(
  "last_scale",
  "float",
  "The scale that was applied on the latest normalized image, read access only"
);
PyObject* PyBobIpBaseFaceEyesNorm_getLastScale(PyBobIpBaseFaceEyesNormObject* self, void*){
  TRY
  return Py_BuildValue("d", self->cxx->getLastScale());
  CATCH("last_scale could not be read", 0)
}

static auto lastOffset = bob::extension::VariableDoc(
  "last_offset",
  "(float, float)",
  "The original transformation offset (eye center) in the normalization process, read access only"
);
PyObject* PyBobIpBaseFaceEyesNorm_getLastOffset(PyBobIpBaseFaceEyesNormObject* self, void*){
  TRY
  auto r = self->cxx->getLastOffset();
  return Py_BuildValue("(dd)", r[0], r[1]);
  CATCH("last_offset could not be read", 0)
}

static auto geomNorm = bob::extension::VariableDoc(
  "geom_norm",
  ":py:class:`bob.ip.base.GeonNorm`",
  "The geometric normalization class that was used to compute the last normalization, read access only"
);
PyObject* PyBobIpBaseFaceEyesNorm_getGeomNorm(PyBobIpBaseFaceEyesNormObject* self, void*){
  TRY
  PyBobIpBaseGeomNormObject* geomNorm = (PyBobIpBaseGeomNormObject*)PyBobIpBaseGeomNormType.tp_alloc(&PyBobIpBaseGeomNormType, 0);
  geomNorm->cxx = self->cxx->getGeomNorm();
  return Py_BuildValue("O", geomNorm);
  CATCH("geom_norm could not be read", 0)
}

static PyGetSetDef PyBobIpBaseFaceEyesNorm_getseters[] = {
    {
      eyesDistance.name(),
      (getter)PyBobIpBaseFaceEyesNorm_getEyesDistance,
      (setter)PyBobIpBaseFaceEyesNorm_setEyesDistance,
      eyesDistance.doc(),
      0
    },
    {
      eyesAngle.name(),
      (getter)PyBobIpBaseFaceEyesNorm_getEyesAngle,
      (setter)PyBobIpBaseFaceEyesNorm_setEyesAngle,
      eyesAngle.doc(),
      0
    },
    {
      cropSize.name(),
      (getter)PyBobIpBaseFaceEyesNorm_getCropSize,
      (setter)PyBobIpBaseFaceEyesNorm_setCropSize,
      cropSize.doc(),
      0
    },
    {
      cropOffset.name(),
      (getter)PyBobIpBaseFaceEyesNorm_getCropOffset,
      (setter)PyBobIpBaseFaceEyesNorm_setCropOffset,
      cropOffset.doc(),
      0
    },
    {
      lastAngle.name(),
      (getter)PyBobIpBaseFaceEyesNorm_getLastAngle,
      0,
      lastAngle.doc(),
      0
    },
    {
      lastScale.name(),
      (getter)PyBobIpBaseFaceEyesNorm_getLastScale,
      0,
      lastScale.doc(),
      0
    },
    {
      lastOffset.name(),
      (getter)PyBobIpBaseFaceEyesNorm_getLastOffset,
      0,
      lastOffset.doc(),
      0
    },
    {
      geomNorm.name(),
      (getter)PyBobIpBaseFaceEyesNorm_getGeomNorm,
      0,
      geomNorm.doc(),
      0
    },
    {0}  /* Sentinel */
};


/******************************************************************/
/************ Functions Section ***********************************/
/******************************************************************/
static auto extract = bob::extension::FunctionDoc(
  "extract",
  "This function extracts and normalized the facial image",
  "This function extracts the facial image based on the eye locations (or the location of other fixed point, see note below). "
  "The geometric normalization is applied such that the eyes are placed to **fixed positions** in the normalized image. "
  "The image is cropped at the same time, so that no unnecessary operations are executed.\n\n"
  ".. note:: Instead of the eyes, any two fixed positions can be used to normalize the face. "
  "This can simply be achieved by selecting two other nodes in the constructor (see :py:class:`FaceEyesNorm`) and in this function. "
  "Just make sure that 'right' and 'left' refer to the same landmarks in both functions.\n\n"
  ".. note:: The :py:func:`__call__` function is an alias for this method.",
  true
)
.add_prototype("input, right_eye, left_eye", "output")
.add_prototype("input, output, right_eye, left_eye")
.add_prototype("input, input_mask, output, output_mask, right_eye, left_eye")
.add_parameter("input", "array_like (2D)", "The input image to which FaceEyesNorm should be applied")
.add_parameter("output", "array_like (2D, float)", "The output image, which must be of size :py:attr:`cropped_size`")
.add_parameter("right_eye", "(float, float)", "The position of the right eye (or another landmark) in ``input`` image coordinates.")
.add_parameter("left_eye", "(float, float)", "The position of the left eye (or another landmark) in ``input`` image coordinates.")
.add_parameter("input_mask", "array_like (2D, bool)", "An input mask of valid pixels before geometric normalization, must be of same size as ``input``")
.add_parameter("output_mask", "array_like (2D, bool)", "The output mask of valid pixels after geometric normalization, must be of same size as ``output``")
.add_return("output", "array_like(2D, float)", "The resulting normalized face image, which is of size :py:attr:`cropped_size`")
;

template <typename T>
static void extract_inner(PyBobIpBaseFaceEyesNormObject* self, PyBlitzArrayObject* input, PyBlitzArrayObject* input_mask, PyBlitzArrayObject* output, PyBlitzArrayObject* output_mask, const blitz::TinyVector<double,2>& right, const blitz::TinyVector<double,2>& left){
  if (input_mask && output_mask){
    self->cxx->extract(*PyBlitzArrayCxx_AsBlitz<T,2>(input), *PyBlitzArrayCxx_AsBlitz<bool,2>(input_mask), *PyBlitzArrayCxx_AsBlitz<double,2>(output), *PyBlitzArrayCxx_AsBlitz<bool,2>(output_mask), right, left);
  } else {
    self->cxx->extract(*PyBlitzArrayCxx_AsBlitz<T,2>(input), *PyBlitzArrayCxx_AsBlitz<double,2>(output), right, left);
  }
}

static PyObject* PyBobIpBaseFaceEyesNorm_extract(PyBobIpBaseFaceEyesNormObject* self, PyObject* args, PyObject* kwargs) {
  TRY
  static char* kwlist1[] = {c("input"), c("right_eye"), c("left_eye"), 0};
  static char* kwlist2[] = {c("input"), c("output"), c("right_eye"), c("left_eye"), 0};
  static char* kwlist3[] = {c("input"), c("input_mask"), c("output"), c("output_mask"), c("right_eye"), c("left_eye"), 0};

  // get the number of command line arguments
  Py_ssize_t nargs = (args?PyTuple_Size(args):0) + (kwargs?PyDict_Size(kwargs):0);

  PyBlitzArrayObject* input = 0,* input_mask = 0,* output = 0,* output_mask = 0;
  blitz::TinyVector<double,2> right(0,0), left(0,0);

  switch (nargs){
    case 3:{
      // with input only
      if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O&(dd)(dd)", kwlist1, PyBlitzArray_Converter, &input, &right[0], &right[1], &left[0], &left[1])){
        extract.print_usage();
        return 0;
      }
    }
    case 4:{
      // with input and output
      if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O&O&(dd)(dd)", kwlist2, &PyBlitzArray_Converter, &input, &PyBlitzArray_OutputConverter, &output, &right[0], &right[1], &left[0], &left[1])){
        extract.print_usage();
        return 0;
      }
      break;
    }
    case 6:{
      // with mask
      if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O&O&O&O&(dd)(dd)", kwlist3, &PyBlitzArray_Converter, &input, &PyBlitzArray_Converter, &input_mask, &PyBlitzArray_OutputConverter, &output, &PyBlitzArray_OutputConverter, &output_mask, &right[0], &right[1], &left[0], &left[1])){
        extract.print_usage();
        return 0;
      }
      break;
    }
    default:{
      extract.print_usage();
      PyErr_Format(PyExc_TypeError, "`%s' extract called with wrong number of parameters", Py_TYPE(self)->tp_name);
      return 0;
    }
  } // switch

  auto input_ = make_safe(input), output_ = make_xsafe(output);
  auto input_mask_ = make_xsafe(input_mask), output_mask_ = make_xsafe(output_mask);

  if (input->ndim != 2){
    extract.print_usage();
    PyErr_Format(PyExc_TypeError, "'%s' only 2D facial images can be normalized", Py_TYPE(self)->tp_name);
    return 0;
  }

  if (output){
    // check that data type is correct and dimensions fit
    if (output->ndim != 2){
      extract.print_usage();
      PyErr_Format(PyExc_TypeError, "'%s' the 'output' array must be 2D", Py_TYPE(self)->tp_name);
      return 0;
    }
    if (output->type_num != NPY_FLOAT64){
      extract.print_usage();
      PyErr_Format(PyExc_TypeError, "'%s': the 'output' array must be of type float64", Py_TYPE(self)->tp_name);
      return 0;
    }
  } else {
    // create output in the desired dimensions
    auto shape = self->cxx->getCropSize();
    Py_ssize_t n[] = {shape[0], shape[1]};
    output = reinterpret_cast<PyBlitzArrayObject*>(PyBlitzArray_SimpleNew(NPY_FLOAT64, 2, n));
    output_ = make_safe(output);
  }

  if (input_mask && output_mask){
    if (input_mask->ndim != 2 || output_mask->ndim != 2){
      PyErr_Format(PyExc_TypeError, "`%s' masks must have the same shape as the input or output matrix", Py_TYPE(self)->tp_name);
      extract.print_usage();
      return 0;
    }
    if (input_mask->type_num != NPY_BOOL || output_mask->type_num != NPY_BOOL){
      PyErr_Format(PyExc_TypeError, "`%s' masks must be of boolean type", Py_TYPE(self)->tp_name);
      extract.print_usage();
      return 0;
    }
  }

  // finally, process the data
  switch (input->type_num){
    case NPY_UINT8:   extract_inner<uint8_t>(self, input, input_mask, output, output_mask, right, left); break;
    case NPY_UINT16:  extract_inner<uint16_t>(self, input, input_mask, output, output_mask, right, left); break;
    case NPY_FLOAT64: extract_inner<double>(self, input, input_mask, output, output_mask, right, left); break;
    default:
      PyErr_Format(PyExc_TypeError, "`%s' input array of type %s are currently not supported", Py_TYPE(self)->tp_name, PyBlitzArray_TypenumAsString(input->type_num));
      extract.print_usage();
      return 0;
  }

  if (nargs == 3){
    Py_INCREF(output);
    return PyBlitzArray_AsNumpyArray(output,0);
  } else {
    Py_RETURN_NONE;
  }

  CATCH("cannot extract face from image", 0)
}

static PyMethodDef PyBobIpBaseFaceEyesNorm_methods[] = {
  {
    extract.name(),
    (PyCFunction)PyBobIpBaseFaceEyesNorm_extract,
    METH_VARARGS|METH_KEYWORDS,
    extract.doc()
  },
  {0} /* Sentinel */
};


/******************************************************************/
/************ Module Section **************************************/
/******************************************************************/

// Define the FaceEyesNorm type struct; will be initialized later
PyTypeObject PyBobIpBaseFaceEyesNormType = {
  PyVarObject_HEAD_INIT(0,0)
  0
};

bool init_BobIpBaseFaceEyesNorm(PyObject* module)
{
  // initialize the Gabor wavelet type struct
  PyBobIpBaseFaceEyesNormType.tp_name = FaceEyesNorm_doc.name();
  PyBobIpBaseFaceEyesNormType.tp_basicsize = sizeof(PyBobIpBaseFaceEyesNormObject);
  PyBobIpBaseFaceEyesNormType.tp_flags = Py_TPFLAGS_DEFAULT;
  PyBobIpBaseFaceEyesNormType.tp_doc = FaceEyesNorm_doc.doc();

  // set the functions
  PyBobIpBaseFaceEyesNormType.tp_new = PyType_GenericNew;
  PyBobIpBaseFaceEyesNormType.tp_init = reinterpret_cast<initproc>(PyBobIpBaseFaceEyesNorm_init);
  PyBobIpBaseFaceEyesNormType.tp_dealloc = reinterpret_cast<destructor>(PyBobIpBaseFaceEyesNorm_delete);
  PyBobIpBaseFaceEyesNormType.tp_richcompare = reinterpret_cast<richcmpfunc>(PyBobIpBaseFaceEyesNorm_RichCompare);
  PyBobIpBaseFaceEyesNormType.tp_methods = PyBobIpBaseFaceEyesNorm_methods;
  PyBobIpBaseFaceEyesNormType.tp_getset = PyBobIpBaseFaceEyesNorm_getseters;
  PyBobIpBaseFaceEyesNormType.tp_call = reinterpret_cast<ternaryfunc>(PyBobIpBaseFaceEyesNorm_extract);

  // check that everything is fine
  if (PyType_Ready(&PyBobIpBaseFaceEyesNormType) < 0)
    return false;

  // add the type to the module
  Py_INCREF(&PyBobIpBaseFaceEyesNormType);
  return PyModule_AddObject(module, "FaceEyesNorm", (PyObject*)&PyBobIpBaseFaceEyesNormType) >= 0;
}
