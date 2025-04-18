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

#include "viewer.h"
#include <easy3d/core/model.h>
#include <easy3d/renderer/drawable_points.h>
#include <easy3d/renderer/renderer.h>
#include <easy3d/util/resource.h>
#include <easy3d/util/initializer.h>

/**
 * \example{lineno} Tutorial_403_PointSelection/main.cpp
 * 
 * This example shows how to select a subset of a point cloud by sketching a rectangle or a lasso using the mouse.
 *
 * The header file of the viewer class:
 * \include{lineno} Tutorial_403_PointSelection/viewer.h
 * The source file of the viewer class:
 * \include{lineno} Tutorial_403_PointSelection/viewer.cpp
 */

using namespace easy3d;


int main(int argc, char **argv) {
    // initialize Easy3D.
    initialize();

    const std::string file = resource::directory() + "/data/polyhedron.bin";

    // create the viewer.
    TutorialPointSelection viewer(EXAMPLE_TITLE);

    auto model = viewer.add_model(file, true);
    if (!model) {
        LOG(ERROR) << "failed to load model. Please make sure the file exists and format is correct.";
        return EXIT_FAILURE;
    }

    auto drawable = model->renderer()->get_points_drawable("vertices");
    dynamic_cast<PointsDrawable*>(drawable)->set_point_size(3.0f);

    // run the viewer
    return viewer.run();
}

