/********************************************************************
 * Copyright (C) 2015 Liangliang Nan <liangliang.nan@gmail.com>
 * https://3d.bk.tudelft.nl/liangliang/
 *
 * This file is part of Easy3D. If it is useful in your research/work,
 * I would be grateful if you show your appreciation by citing it:
 * ------------------------------------------------------------------
 *      Liangliang Nan.
 *      Easy3D: a lightweight, easy-to-use, and efficient C++ library
 *      for processing and rendering 3D data.
 *      Journal of Open Source Software, 6(64), 3255, 2021.
 * ------------------------------------------------------------------
 *
 * Easy3D is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License Version 3
 * as published by the Free Software Foundation.
 *
 * Easy3D is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 ********************************************************************/

#ifndef EASY3D_CORE_SPLINE_CURVE_INTERPOLATION_H
#define EASY3D_CORE_SPLINE_CURVE_INTERPOLATION_H


#include <vector>
#include <cassert>

#include <easy3d/core/spline_interpolation.h>


namespace easy3d {


    /**
     * \brief Cubic spline curve interpolation for arbitrary dimensions.
     * \details This is a wrapper of SplineInterpolation. It can be instantiated with any point type (1D, 2D, 3D etc.).
     *
     * \tparam Point A templated point class that supports basic arithmetic operations (addition and scalar
     *      multiplication). It must be parameterized as `Point<N, T>`, where `N` is the number of dimensions, and
     *      `T` is the data type.
     * \tparam N The number of dimensions (e.g., 2 for 2D, 3 for 3D).
     * \tparam T The scalar type (e.g., `float` or `double`).
     *
     * Example usage:
     *      \code
     *          const int resolution = 1000;    // Number of line subdivisions to display the spline
     *          SplineCurveInterpolation<Vec, 3, float> interpolator;
     *          interpolator.set_boundary(...);
     *          interpolator.set_points(points);
     *          for (int i = 0; i < resolution; ++i) {
     *              const vec3 p = interpolator.eval_f(float(i) / float(resolution - 1));
     *              std::cout << p << std::endl;
     *          }
     *      \endcode
     * \class SplineCurveInterpolation easy3d/core/spline_curve_interpolation.h
     */
    template <template <size_t, class> class Point, size_t N, typename T>
    class SplineCurveInterpolation {
    public:
        /// The point type.
        using Point_t = Point<N, T>;

        /// The boundary type.
        enum BoundaryType {
            first_deriv = 1,    ///< First derivative
            second_deriv = 2    ///< Second derivative
        };

    public:

        /**
         * Constructor.
         * \details Sets default boundary conditions to be zero curvature at both ends.
         */
        SplineCurveInterpolation() : left_(second_deriv), right_(second_deriv),
                                left_value_(0.0), right_value_(0.0), dim_(0), largest_t_(0.0) {
        }

        /**
         * Sets the boundary condition (optional).
         * \attention If called, it has to come before set_points().
         */
        void set_boundary(BoundaryType left, T left_value,
                          BoundaryType right, T right_value);

        /**
         * Sets the parameters and position of the point samples on the curve.
         * \param parameters The parameters (e.g., accumulated time or distance) of the points, each corresponding
         *      to a point in \c points.
         * \param points The points. Each point corresponds to a parameter in \c parameters.
         * \param cubic_spline \c true for cubic spline interpolation; \c false for linear interpolation.
         * \note The \c parameters have to be monotonously increasing along the curve.
         */
        void set_points(const std::vector<T> &parameters, const std::vector<Point_t> &points, bool cubic_spline = true);

        /**
         * Sets the position of the point samples on the curve.
         * \brief This is an overload of the set_points() function. The parameters are the accumulated curve length.
         * \param points The data points. The parameter of each point is its accumulated curve length from the
         *      first point.
         * \param cubic_spline \c true for cubic spline interpolation; \c false for linear interpolation.
         * \note The \c points have to be ordered along the curve.
         */
        void set_points(const std::vector<Point_t> &points, bool cubic_spline = true);

        /**
         * Evaluates the position of the spline curve at a given parameter.
         * \param u Curve parameter in the range [0, 1]. The actual meaning of the parameter is given by
         *      the \p parameters provided in set_points().
         */
        Point<N, T> eval_f(T u) const;

    private:
        // -------------------------------------------------------------------------
        /// @name Class tools
        // -------------------------------------------------------------------------
        BoundaryType left_, right_;
        T left_value_, right_value_;

        std::size_t dim_;
        std::vector< SplineInterpolation<T> > interpolators_;
        T largest_t_;
    };



    // Cubic spline interpolation implementation
    // -----------------------

    template <template <size_t, class> class Point, size_t N, typename T>
    void SplineCurveInterpolation<Point, N, T>::set_boundary(BoundaryType left, T left_value,
                                                             BoundaryType right, T right_value) {
        assert(interpolators_.size() == 0);  // set_points() must not have happened yet
        left_ = left;
        right_ = right;
        left_value_ = left_value;
        right_value_ = right_value;
    }


    template <template <size_t, class> class Point, size_t N, typename T>
    void SplineCurveInterpolation<Point, N, T>::set_points(const std::vector<T> &input_parameters,
                                                       const std::vector<Point<N, T>> &input_points, bool cubic_spline) {
        if (input_parameters.empty() || input_parameters.size() != input_points.size())
            return;

        // filter out non-monotone data
        std::vector<T> parameters;
        std::vector<Point<N, T>> points;
        for (std::size_t i=0; i<input_parameters.size(); ++i) {
            const T para = input_parameters[i];
            if (i == 0 || para > parameters.back()) {
                parameters.push_back(para);
                points.push_back(input_points[i]);
            }
        }
        const auto diff = input_points.size() - points.size();
        LOG_IF(diff > 0, WARNING) << diff << " data points discarded because the input has to be monotonously increasing";

        dim_ = points[0].dimension();
        largest_t_ = parameters.back();

        // an ND curve is represented in the parametric form: x1(t), x2(t), x3(t)...
        std::vector< std::vector<T> > coords(dim_, std::vector<T>(points.size()));

        T t(0);
        for (std::size_t i = 0; i < points.size(); ++i) {
            const auto &p = points[i];
            if (i > 0)
                t += distance(points[i-1], p);
            for (std::size_t j = 0; j<dim_; ++j)
                coords[j][i] = p[j];
        }

        // class instantiation
        interpolators_.resize(dim_);
        for (std::size_t i=0; i<dim_; ++i) {
            // set boundary condition
            interpolators_[i].set_boundary(
                    left_ == first_deriv ? SplineInterpolation<T>::first_deriv : SplineInterpolation<T>::second_deriv,
                    left_value_,
                    right_ == first_deriv ? SplineInterpolation<T>::first_deriv : SplineInterpolation<T>::second_deriv,
                    right_value_,
                    cubic_spline
            );
            // set data
            interpolators_[i].set_data(parameters, coords[i]);
        }
    }


    template <template <size_t, class> class Point, size_t N, typename T>
    void SplineCurveInterpolation<Point, N, T>::set_points(const std::vector<Point<N, T>> &points, bool cubic_spline) {
        if (points.size() < 2)
            return;

        // we use the accumulated curve distance as the parameters
        std::vector<T> parameters(points.size(), T(0));

        T t(0);
        for (std::size_t i = 1; i < points.size(); ++i) {
            const auto &p = points[i];
            t += distance(points[i-1], p);
            parameters[i] = t;
        }

        set_points(parameters, points, cubic_spline);
    }


    template <template <size_t, class> class Point, size_t N, typename T>
    Point<N, T> SplineCurveInterpolation<Point, N, T>::eval_f(T u) const {
        Point<N, T> p;
        for (std::size_t i=0; i<dim_; ++i)
            p[i] = interpolators_[i](u * largest_t_);
        return p;
    }


}   // namespace easy3d


#endif  // EASY3D_CORE_SPLINE_CURVE_INTERPOLATION_H
