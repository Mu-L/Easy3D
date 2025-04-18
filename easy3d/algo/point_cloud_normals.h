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

#ifndef EASY3D_ALGO_POINT_CLOUD_NORMALS_H
#define EASY3D_ALGO_POINT_CLOUD_NORMALS_H


namespace easy3d {

    class PointCloud;

    /**
     * \brief Estimates and reorients point cloud normals.
     * \details This class provides methods to estimate the normals of a point cloud using Principal Component Analysis (PCA)
     *          and to reorient the normals based on a minimum spanning tree algorithm.
     * \class PointCloudNormals easy3d/algo/point_cloud_normals.h
     */
    class PointCloudNormals {
    public:
        /**
         * \brief Estimates the point cloud normals using PCA.
         * \details This method estimates the normals of the input point cloud by constructing a covariance matrix
         *          from the neighboring points and computing its eigenvectors.
         * \param cloud The input point cloud.
         * \param k The number of neighboring points to construct the covariance matrix.
         * \param compute_curvature Whether to also compute the curvature.
         * \return True if the estimation is successful, false otherwise.
         */
        static bool estimate(PointCloud *cloud, unsigned int k = 16, bool compute_curvature = false);

        /**
         * \brief Reorients the point cloud normals.
         * \details This method reorients the normals of the input point cloud using the normal reorientation method
         *          described in Hoppe et al. Surface reconstruction from unorganized points. SIGGRAPH 1992.
         * \param cloud The input point cloud.
         * \param k The number of neighboring points to construct the graph.
         * \return True if the reorientation is successful, false otherwise.
         */
        static bool reorient(PointCloud *cloud, unsigned int k = 16);
    };


} // namespace easy3d


#endif  // EASY3D_ALGO_POINT_CLOUD_NORMALS_H

