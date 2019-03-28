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
 \ *---------------------------------------------------------------------------*/
#ifndef __STENCILFUNCTIONS_HPP__
#define __STENCILFUNCTIONS_HPP__

#include "bitpit_discretization.hpp"
#include "MimmoObject.hpp"
#include "MimmoPiercedVector.hpp"


namespace mimmo{

/*!
 * \brief Collection of methods to precompute stencils of math operators (Laplacian, Gradients, etc...) on mimmo meshes
 * \ingroup propagators
 */
namespace FVolStencil{
    typedef MimmoPiercedVector<bitpit::StencilVector> MPVGradient; /**< typedef to save stencil of a Grad stuff */
    typedef MimmoPiercedVector<bitpit::StencilScalar> MPVDivergence; /**< typedef to save stencil of Div stuff */
    typedef std::unique_ptr< MPVGradient > MPVGradientUPtr;  /**< typedef to instantiate unique ptr of Grad stencil */
    typedef std::unique_ptr< MPVDivergence > MPVDivergenceUPtr;/**< typedef to instantiate unique ptr of Div stencil */

    void computeWeightsWLS( const std::vector<std::vector<double>> &P,
                            const std::vector<double> &w,
                            std::vector<std::vector<double>> *weights);

    MPVGradientUPtr   computeFVCellGradientStencil(MimmoObject & geo);
    MPVGradientUPtr   computeFVFaceGradientStencil(MimmoObject & geo, MPVGradient * cellGradientStencil = nullptr);

    bitpit::StencilVector computeNeumannBCFaceGradient(const double & neuval,
                                                       const std::array<double,3> & interfaceNormal,
                                                       const bitpit::StencilVector & CCellOwnerStencil);

    bitpit::StencilVector computeDirichletBCFaceGradient(const double &dirval,
                                                         const long & ownerID,
                                                         const std::array<double,3> & ownerCentroid,
                                                         const std::array<double,3> & interfaceCentroid,
                                                         const std::array<double,3> & interfaceNormal,
                                                         const double &distD,
                                                         const bitpit::StencilVector & CCellOwnerStencil);

    MPVDivergenceUPtr computeFVLaplacianStencil (MPVGradient & faceGradientStencil,
                                                 MimmoPiercedVector<double> * diffusivity = nullptr);

    MPVDivergenceUPtr computeFVBorderLaplacianStencil (MPVGradient & faceGradientStencil,
                                                       MimmoPiercedVector<double> * diffusivity = nullptr);




};//end namespace stencilFunction


}; //end namespace mimmo

#endif /* __STENCILFUNCTIONS_HPP__ */
