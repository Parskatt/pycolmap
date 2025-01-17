#include <pybind11/eigen.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;
using namespace pybind11::literals;

#include "colmap/geometry/pose.h"

#include "estimators/absolute_pose.cc"
#include "estimators/alignment.cc"
#include "estimators/essential_matrix.cc"
#include "estimators/fundamental_matrix.cc"
#include "estimators/similarity_transform.cc"
#include "estimators/generalized_absolute_pose.cc"
#include "estimators/homography.cc"
#include "estimators/two_view_geometry.cc"
#include "geometry/quaternion.cc"
#include "geometry/rigid3.cc"
#include "geometry/sim3.cc"
#include "helpers.h"
#include "homography_decomposition.cc"
#include "pipeline/meshing.cc"
#include "pipeline/mvs.cc"
#include "pipeline/sfm.cc"
#include "reconstruction/correspondence_graph.cc"
#include "reconstruction/incremental_triangulator.cc"
#include "reconstruction/reconstruction.cc"
#include "sift.cc"
#include "utils.h"

void init_reconstruction(py::module&);
void init_quaternion(py::module&);

PYBIND11_MODULE(pycolmap, m) {
  m.doc() = "COLMAP plugin";
#ifdef VERSION_INFO
  m.attr("__version__") = py::str(VERSION_INFO);
#else
  m.attr("__version__") = py::str("dev");
#endif

  auto PyDevice = py::enum_<Device>(m, "Device")
                      .value("auto", Device::AUTO)
                      .value("cpu", Device::CPU)
                      .value("cuda", Device::CUDA);
  AddStringToEnumConstructor(PyDevice);

  m.attr("has_cuda") = IsGPU(Device::AUTO);

  // Geometry bindings
  init_quaternion(m);
  init_sim3(m);
  init_rigid3(m);

  // Estimators
  auto PyRANSACOptions =
      py::class_<RANSACOptions>(m, "RANSACOptions")
          .def(py::init<>([]() {
            RANSACOptions options;
            options.max_error = 4.0;
            options.min_inlier_ratio = 0.01;
            options.confidence = 0.9999;
            options.min_num_trials = 1000;
            options.max_num_trials = 100000;
            return options;
          }))
          .def_readwrite("max_error", &RANSACOptions::max_error)
          .def_readwrite("min_inlier_ratio", &RANSACOptions::min_inlier_ratio)
          .def_readwrite("confidence", &RANSACOptions::confidence)
          .def_readwrite("dyn_num_trials_multiplier",
                         &RANSACOptions::dyn_num_trials_multiplier)
          .def_readwrite("min_num_trials", &RANSACOptions::min_num_trials)
          .def_readwrite("max_num_trials", &RANSACOptions::max_num_trials);
  make_dataclass(PyRANSACOptions);

  bind_absolute_pose_estimation(m, PyRANSACOptions);
  bind_essential_matrix_estimation(m);
  bind_fundamental_matrix_estimation(m);
  bind_similarity_transform_estimation(m);
  bind_generalized_absolute_pose_estimation(m);
  bind_homography_estimation(m);
  bind_two_view_geometry_estimation(m);
  bind_alignment(m);

  // Homography Decomposition.
  m.def("homography_decomposition",
        &homography_decomposition_estimation,
        "H"_a,
        "K1"_a,
        "K2"_a,
        "points1"_a,
        "points2"_a,
        "Analytical Homography Decomposition.");

  // Reconstruction bindings
  init_reconstruction(m);

  // Correspondence graph bindings
  init_correspondence_graph(m);

  // Incremental triangulator bindings
  init_incremental_triangulator(m);

  // Automatic conversion from python dicts to colmap cameras for backwards
  // compatibility
  py::implicitly_convertible<py::dict, colmap::Camera>();

  // Main reconstruction steps
  init_sfm(m);
  init_mvs(m);
  init_meshing(m);

  // SIFT feature detector and descriptor
  init_sift(m);

  py::add_ostream_redirect(m, "ostream");
}
