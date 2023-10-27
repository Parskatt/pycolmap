// Authors: Johan Edstedt (parskatt)

#include "colmap/estimators/similarity_transform.h"

#include "colmap/math/random.h"
#include "colmap/optim/loransac.h"
#include "colmap/scene/camera.h"

#include <fstream>
#include <iostream>

using namespace colmap;

#include <pybind11/eigen.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;
using namespace pybind11::literals;

#include "log_exceptions.h"
#include "utils.h"

py::object similarity_transform_estimation(
    const std::vector<Eigen::Vector3d> points3D1,
    const std::vector<Eigen::Vector3d> points3D2,
    const RANSACOptions options) {
  SetPRNGSeed(0);
  THROW_CHECK_EQ(points3D1.size(), points3D2.size());
  py::object failure = py::none();
  py::gil_scoped_release release;

  LORANSAC<SimilarityTransformEstimator<3, true>,
           SimilarityTransformEstimator<3, true>>
      ransac(options);

  const auto report = ransac.Estimate(points3D1, points3D2);
  if (!report.success) {
    return failure;
  }

  const auto sim_transform = report.model;

  py::gil_scoped_acquire acquire;
  return py::dict("sim_transform"_a = sim_transform,
                  "num_inliers"_a = report.support.num_inliers,
                  "inliers"_a = ToPythonMask(report.inlier_mask));
}

void bind_similarity_transform_estimation(py::module& m) {
  auto est_options = m.attr("RANSACOptions")().cast<RANSACOptions>();

  m.def("similarity_transform_estimation",
        &similarity_transform_estimation,
        "points3D1"_a,
        "points3D2"_a,
        "estimation_options"_a = est_options,
        "LORANSAC + 3-point algorithm.");
}
