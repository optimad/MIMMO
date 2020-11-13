/*---------------------------------------------------------------------------*\
 *
 *  mimmo
 *
 *  Copyright (C) 2015-2017 OPTIMAD engineering Srl
 *
 *  -------------------------------------------------------------------------
 *  License
 *  This file is part of mimmo.
 *
 *  mimmo is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU Lesser General Public License v3 (LGPL)
 *  as published by the Free Software Foundation.
 *
 *  mimmo is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 *  License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with mimmo. If not, see <http://www.gnu.org/licenses/>.
 *
\*---------------------------------------------------------------------------*/
# ifndef __MIMMOCGUTILS_HPP__
# define __MIMMOCGUTILS_HPP__

# include "mimmoTypeDef.hpp"

namespace mimmo{


/*!
 * \brief Additional Utilities for Elementary Computational Geometry handling in mimmo.
 * \ingroup core
 */
namespace mimmoCGUtils{

    double distancePointPolygon(const darray3E & point, const dvecarr3E & vertCoords);
    bool   isPointInsideSegment(const darray3E & point, const darray3E &  V0, const darray3E &  V1, double tol = 1.E-12);
    bool   isPointInsideTriangle(const darray3E & point, const darray3E &  V0, const darray3E &  V1, const darray3E &  V2, double tol = 1.E-12);
    bool   isPointInsidePolygon(const darray3E & point, const dvecarr3E & vertCoords, double tol = 1.E-12);

}; //end namespace mimmoCGUtils

} //end namespace mimmo

#endif
