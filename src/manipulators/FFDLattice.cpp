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

#include "FFDLattice.hpp"
#include "Operators.hpp"
#include "customOperators.hpp"


using namespace std;
namespace mimmo{

/*! Basic Constructor.*/
FFDLattice::FFDLattice(){
    m_knots.resize(3);
    m_mapEff.resize(3);
    m_deg.fill(1);
    m_mapNodes.resize(3);
    m_globalDispl = false;
    m_bfilter = false;
    m_name = "mimmo.FFDlattice";
};

/*!
 * Custom constructor reading xml data
 * \param[in] rootXML reference to your xml tree section
 */
FFDLattice::FFDLattice(const bitpit::Config::Section & rootXML){

    m_knots.resize(3);
    m_mapEff.resize(3);
    m_deg.fill(1);
    m_mapNodes.resize(3);
    m_globalDispl = false;
    m_bfilter = false;
    m_name = "mimmo.FFDlattice";

    std::string fallback_name = "ClassNONE";
    std::string input = rootXML.get("ClassName", fallback_name);
    input = bitpit::utils::string::trim(input);
    if(input == "mimmo.FFDLattice"){
        absorbSectionXML(rootXML);
    }else{
        warningXML(m_log, m_name);
    };
}

/*! Destructor */
FFDLattice::~FFDLattice(){};

/*! Copy Constructor. Result displacements are never copied.
 *\param[in] other FFDLattice where copy from
 */ 
FFDLattice::FFDLattice(const FFDLattice & other):Lattice(other){
    m_deg = other.m_deg;
    m_knots = other.m_knots;
    m_mapEff = other.m_mapEff;
    m_weights = other.m_weights;
    m_displ = other.m_displ;
    m_mapNodes = other.m_mapNodes;
    m_mapdeg = other.m_mapdeg;
    m_globalDispl = other.m_globalDispl;
    m_bfilter = other.m_bfilter;
    m_filter = other.m_filter;
    m_collect_wg = other.m_collect_wg;
};


/*!
 * Assignement operator.  Result displacements are never copied.
 */
FFDLattice & FFDLattice::operator=(FFDLattice other){
    swap(other);
    return *this;
}

/*!
 * Swap function.
 * \param[in] x object to be swapped
 */
void FFDLattice::swap(FFDLattice & x) noexcept
{
   std::swap(m_deg, x.m_deg);
   std::swap(m_knots, x.m_knots);
   std::swap(m_mapEff, x.m_mapEff);
   std::swap(m_weights, x.m_weights);
   std::swap(m_displ, x.m_displ);
   std::swap(m_mapNodes, x.m_mapNodes);
   std::swap(m_mapdeg, x.m_mapdeg);
   std::swap(m_globalDispl, x.m_globalDispl);
   std::swap(m_bfilter, x.m_bfilter);
   //std::swap(m_filter, x.m_filter);
   m_filter.swap(x.m_filter);
   std::swap(m_collect_wg, x.m_collect_wg);
   //std::swap(m_gdispl, x.m_gdispl);
   m_gdispl.swap(x.m_gdispl);
   Lattice::swap(x);
}

/*! It builds the input/output ports of the object
 */
void
FFDLattice::buildPorts(){

    Lattice::buildPorts();

    bool built = true;

    //input
    built = (built && createPortIn<dvecarr3E, FFDLattice>(this, &mimmo::FFDLattice::setDisplacements, M_DISPLS));
    built = (built && createPortIn<dmpvector1D, FFDLattice>(this, &mimmo::FFDLattice::setFilter,M_FILTER));
    built = (built && createPortIn<iarray3E, FFDLattice>(this, &mimmo::FFDLattice::setDegrees, M_DEG));
    built = (built && createPortIn<dvector1D, FFDLattice>(this, &mimmo::FFDLattice::setNodalWeight, M_NURBSWEIGHTS));
    built = (built && createPortIn<std::array<mimmo::CoordType,3>, FFDLattice>(this, &mimmo::FFDLattice::setCoordType, M_NURBSCOORDTYPE));

    //output
    built = (built && createPortOut<dmpvecarr3E, FFDLattice>(this, &mimmo::FFDLattice::getDeformation, M_GDISPLS));
    built = (built && createPortOut<iarray3E, FFDLattice>(this, &mimmo::FFDLattice::getDegrees, M_DEG));
    built = (built && createPortOut<dmpvector1D, FFDLattice>(this, &mimmo::FFDLattice::getFilter, M_FILTER));
    built = (built && createPortOut<dvector1D, FFDLattice>(this, &mimmo::FFDLattice::getWeights,M_NURBSWEIGHTS));
    built = (built && createPortOut<std::array<mimmo::CoordType,3>, FFDLattice>(this, &mimmo::FFDLattice::getCoordType, M_NURBSCOORDTYPE));
    m_arePortsBuilt = built;
};

/*!Clean all stuffs in your lattice */
void
FFDLattice::clearLattice(){
    Lattice::clearLattice();
    clearKnots(); //clear all knots stuff;
    clearFilter();
    m_displ.clear();

};

/*!Clean filter field */
void
FFDLattice::clearFilter(){
    m_filter.clear();
    m_bfilter = false;
};


/*! Return a vector of six elements reporting the real number of knots effectively stored in the current class (first 3 elements)
 * and the theoretical number of knots (last 3 elements) for Nurbs representation (see Nurbs Books of Peigl)
 * \return six elements vector
 */
ivector1D
FFDLattice::getKnotsDimension(){
    ivector1D res(6,0);

    //effectively stored
    res[0] = m_knots[0].size();
    res[1] = m_knots[1].size();
    res[2] = m_knots[2].size();

    //theoretical stored
    res[3] = m_mapEff[0].size();
    res[4] = m_mapEff[1].size();
    res[5] = m_mapEff[2].size();
    return(res);
};

/*! Return weight actually set for each control node 
 * \return list of weights
 */
dvector1D
FFDLattice::getWeights(){
    return(recoverFullNodeWeights());
};

/*! Return knots structure and theoretical map of knots distributions for the current Lattice
 * \param[out] knots knots list 
 * \param[out] mapTheo  map of knots theoretical distribution
 */ 
void
FFDLattice::returnKnotsStructure(dvector2D & knots, ivector2D &mapTheo){
    knots.resize(3);
    mapTheo.resize(3);

    returnKnotsStructure(0, knots[0], mapTheo[0]);
    returnKnotsStructure(1, knots[1], mapTheo[1]);
    returnKnotsStructure(2, knots[2], mapTheo[2]);
};

/*! Return knots structure and knot multiplicity vector for a specified Nurbs curve "dir"
 * \param[in] dir integer (0,1,2) identifies nurbs curve in x,y,and z direction respectively
 * \param[out] knots knots list
 * \param[out] mapT theoretical knot map distribution
 */ 
void
FFDLattice::returnKnotsStructure( int dir, dvector1D & knots, ivector1D & mapT){
    if(dir<0 || dir>2){return;}

    knots.resize(m_knots[dir].size());
    mapT.resize(m_mapEff[dir].size());

    knots = m_knots[dir];
    mapT = m_mapEff[dir];
};

/*! Get the degree of nurbs curve in each direction.
 * \return array of degree of nurbs curve in each direction
 */
iarray3E
FFDLattice::getDegrees(){
    iarray3E deg;
    deg[0] = m_deg[0];
    deg[1] = m_deg[1];
    deg[2] = m_deg[2];
    return(deg);
}

/*! It gets current DOF displacements of lattice.
 * \return displacements of control points.
 */
dvecarr3E*
FFDLattice::getDisplacements(){
    if(m_displ.empty())    return NULL;
    return(&m_displ);
};

/*! It gets current set filter field. See FFDLattice::setFilter
 * \return filter field.
 */
dmpvector1D
FFDLattice::getFilter(){
    return(m_filter);
};

/*!
 * Return actual computed deformation field (if any) for the geometry linked.
 * \return deformation field
 */
dmpvecarr3E
FFDLattice::getDeformation(){
    return m_gdispl;
};


/*! Check if displacements are meant as global-true or local-false
 * \return global displacements flag
 */
bool
FFDLattice::isDisplGlobal(){return(m_globalDispl);}


/*! Set the degree of nurbs curve in each direction. If the number of control nodes are
 * not initialized, they are set to the minimum number admissible.
 *  Weights are reset to unitary value
 * \param[in] degrees vector of degree of nurbs curve in each direction
 */
void
FFDLattice::setDegrees(iarray3E degrees){
    m_deg = degrees;
    m_isBuild = false;
};


/*! set current DOF displacements of your lattice. If Input list does not fit current DOF size
 * of lattice, return doing nothing.
 * \param[in]  displacements displacements of control nodes
 */
void
FFDLattice::setDisplacements(dvecarr3E displacements){
    m_displ = displacements;
};

/*! Set if displacements are meant as global-true or local-false.
 * \param[in]  flag global displacements flag
 *
 */
void
FFDLattice::setDisplGlobal(bool flag){m_globalDispl = flag;}


/*! Set lattice mesh, dimensions and curve degree for Nurbs trivariate parameterization.
 *  If curve degrees matches current cell Dimensions (n_nodes -1) in each coordinate, a Bezier 
 *  trivariate parameterization is recovered.The lattice can be build also with inherited method 
 *    setMesh. In this case, curve degrees are set to 1 by default. Use FFDLattice::setDegrees to
 *  customize later your curve degrees, and FFDLattice::build() method to apply your modifications.   
 * 
 * \param[in] origin point origin in global reference system
 * \param[in] span span for each shape coordinate in space (local r.s.)
 * \param[in] type ShapeType enum identifies the shape
 * \param[in] dimensions number of control nodes for each direction
 * \param[in] degrees   curve degrees for each direction;
 */
void
FFDLattice::setLattice(darray3E &origin,darray3E & span, ShapeType type, iarray3E & dimensions, iarray3E & degrees){

    if(m_shape){m_shape.reset(nullptr);}

    setShape(type);
    setOrigin(origin);
    setSpan(span);
    setDimension(dimensions);
    setDegrees(degrees);
    build();
};

/*! Set lattice mesh, dimensions and curve degree for Nurbs trivariate parameterization.
 *  If curve degrees matches current cell Dimensions (n_nodes -1) in each coordinate, a Bezier 
 *  trivariate parameterization is recovered.The lattice can be build also with inherited method 
 *    setMesh. In this case, curve degrees are set to 1 by default. Use FFDLattice::setDegrees to
 *  customize later your curve degrees, and FFDLattice::build() method to apply your modifications.   
 *
 * \param[in] origin point origin in global reference system
 * \param[in] span span for each shape coordinate in space (local r.s.)
 * \param[in] type ShapeType enum identifies the shape
 * \param[in] spacing define spacing step for each lattice dimension
 * \param[in] degrees   curve degrees for each direction;
 */
void
FFDLattice::setLattice(darray3E &origin,darray3E & span, ShapeType type, dvector1D & spacing, iarray3E & degrees ){

    ivector1D dimLimit(3,2);
    //create internal shape using unique_ptr member.
    if(m_shape){m_shape.reset(nullptr);}

    switch(type){
    case ShapeType::CYLINDER :
        dimLimit[1] = 4;
        break;
    case ShapeType::SPHERE :
        dimLimit[1] = 5; dimLimit[2] = 3;
        break;
    default://CUBE //WEDGE
        break;
    }

    setShape(type);
    setOrigin(origin);
    setSpan(span);

    darray3E span2 = getSpan();
    iarray3E dim;

    for(int i=0; i<3; ++i){
        if(spacing[i] != 0.0) {
            dim[i] = (int) std::floor(span2[i]/spacing[i] +0.5) + 1;
        }else{
            dim[i] = dimLimit[i];
        }
    }

    setDimension(dim);
    setDegrees(degrees);
    build();
};

/*! Set lattice mesh, dimensions and curve degree for Nurbs trivariate parameterization.
 *  If curve degrees matches current cell Dimensions (n_nodes -1) in each coordinate, a Bezier 
 *  trivariate parameterization is recovered.The lattice can be build also with inherited method 
 *    setMesh. In this case, curve degrees are set to 1 by default. Use FFDLattice::setDegrees to
 *  customize later your curve degrees, and FFDLattice::build() method to apply your modifications. 
 * 
 * \param[in] shape pointer to an external BasicShape object
 * \param[in] dimensions number of control nodes for each direction
 * \param[in] degrees   curve degrees for each direction;
 */
void
FFDLattice::setLattice(BasicShape * shape, iarray3E & dimensions, iarray3E & degrees){

    setShape(shape);
    setDimension(dimensions);
    setDegrees(degrees);
    build();

};

/*! Set lattice mesh, dimensions and curve degree for Nurbs trivariate parameterization.
 *  If curve degrees matches current cell Dimensions (n_nodes -1) in each coordinate, a Bezier 
 *  trivariate parameterization is recovered.The lattice can be build also with inherited method 
 *    setMesh. In this case, curve degrees are set to 1 by default. Use FFDLattice::setDegrees to
 *  customize later your curve degrees, and FFDLattice::build() method to apply your modifications. 
 *   
 * \param[in] shape pointer to an external BasicShape object
 * \param[in] spacing define spacing step for each lattice dimension
 * \param[in] degrees   curve degrees for each direction;
 */
void
FFDLattice::setLattice(BasicShape * shape, dvector1D & spacing, iarray3E & degrees){

    ivector1D dimLimit(3,2);
    //create internal shape using unique_ptr member.
    if(m_shape){m_shape.reset(nullptr);}

    switch(shape->getShapeType()){
    case ShapeType::CYLINDER :
        dimLimit[1] = 4;
        break;
    case ShapeType::SPHERE :
        dimLimit[1] = 5; dimLimit[2] = 3;
        break;
    default://CUBE //WEDGE
        break;
    }

    setShape(shape);

    darray3E span2 = getSpan();
    iarray3E dim;

    for(int i=0; i<3; ++i){
        if(spacing[i] != 0.0) {
            dim[i] = (int) std::floor(span2[i]/spacing[i] +0.5) + 1;
        }else{
            dim[i] = dimLimit[i];
        }
    }

    setDimension(dim);
    setDegrees(degrees);
    build();

};

/*! Modify a weight of a control node. Access to a node in global GRID indexing
 * \param[in] val weight value
 * \param[in] index index of the control node -> gloab indexing
 */
void
FFDLattice::setNodalWeight(double val, int index){
    int ind = accessDOFFromGrid(index);
    m_collect_wg[ind] =  val;
    m_isBuild = false;
};

/*! Modify a weight of a control node. Access to a node in GRID cartesian indexing
 * \param[in] val weight value
 * \param[in] i index of x coordinate
 * \param[in] j index of y coordinates
 * \param[in] k index of z coordinate
 */
void
FFDLattice::setNodalWeight(double val, int i, int j, int k){
    int index = accessPointIndex(i,j,k);
    setNodalWeight(val, index);
};

/*! Modify weights of each control node. 
 * \param[in] wg weights vector of each nx*ny*nz control nodes
 */
void
FFDLattice::setNodalWeight(dvector1D wg){
    int counter = 0;
    for(auto & val : wg){
        setNodalWeight(val, counter);
        counter++;
    }

};

/*! Sets filter field. Note: filter field is defined on nodes of the current linked geometry.
 * coherent size between field size and number of geometry vertices is expected.
 * \param[in] filter fields.
 */
void
FFDLattice::setFilter(dmpvector1D filter){
    m_filter.clear();
    m_bfilter = !(filter.empty());
    m_filter = filter;
};

/*! Plot your current lattice as a structured grid to *vtu file. Wrapped method of plotGrid of father class UCubicMesh.
 * \param[in] directory output directory
 * \param[in] filename  output filename w/out tag
 * \param[in] counter   integer identifier of the file
 * \param[in] binary     boolean flag for 0-"ascii" or 1-"appended" writing 
 * \param[in] deformed  boolean flag for plotting 0-"original lattice", 1-"deformed lattice"
 */
void
FFDLattice::plotGrid(std::string directory, std::string filename,int counter, bool binary, bool deformed){

    if(deformed){
        iarray3E n = getDimension();
        dvecarr3E dispXYZ;
        if(isDisplGlobal()){
            dispXYZ = recoverFullGridDispl();
        }else{
            dispXYZ = convertDisplToXYZ();
        }
        int size = n[0]*n[1]*n[2];
        dvecarr3E data(size);
        for(int i=0; i<size; ++i){
            data[i] = getGlobalPoint(i) + dispXYZ[i];
        }
        UStructMesh::plotGrid(directory, filename, counter, binary, &data);
    }else{
        dvecarr3E* pnull = NULL;
        UStructMesh::plotGrid(directory, filename, counter, binary,  pnull);

    }


};

/*! Plot your current lattice as a point cloud to *vtu file.Wrapped method of plotCloud of father class UCubicMesh.
 * \param[in] directory output directory
 * \param[in] filename  output filename w/out tag
 * \param[in] counter   integer identifier of the file
 * \param[in] binary     boolean flag for 0-"ascii" or 1-"appended" writing 
 * \param[in] deformed  boolean flag for plotting 0-"original lattice", 1-"deformed lattice"
 */
void
FFDLattice::plotCloud(std::string directory, std::string filename, int counter, bool binary, bool deformed){

    if(deformed){
        iarray3E n = getDimension();

        dvecarr3E dispXYZ;
        if(isDisplGlobal()){
            dispXYZ = recoverFullGridDispl();
        }else{
            dispXYZ = convertDisplToXYZ();
        }

        int size = n[0]*n[1]*n[2];
        dvecarr3E data(size);
        for(int i=0; i<size; ++i){
            data[i] = getGlobalPoint(i) + dispXYZ[i];
        }
        UStructMesh::plotCloud(directory, filename, counter, binary, &data);
    }else{
        dvecarr3E* pnull = NULL;
        UStructMesh::plotCloud(directory, filename, counter, binary, pnull);

    }

};

/*! Given pointer to a reference geometry and, execute deformation w/ the current setup.
 * Result is stored in BaseManipulation member m_gdispl. Execution build your mesh,
 * if not done already.
 */
void
FFDLattice::execute(){

    MimmoObject * container = getGeometry();
    if(container == NULL){
//        throw std::runtime_error(m_name + "NULL pointer to linked geometry found");
        (*m_log)<<m_name + " : NULL pointer to linked geometry found"<<std::endl;
        return;
    }

    if(container->isEmpty()){
//        throw std::runtime_error(m_name + " empty linked geometry found");
        (*m_log)<<m_name + " : empty linked geometry found"<<std::endl;
        return;
    }
    
    if(!isBuilt()){
        build();
    }
    
    //build trees
    if(container->isSkdTreeSupported() && !container->isSkdTreeSync())    container->buildSkdTree();
    else if(!container->isKdTreeSync())                                container->buildKdTree();

    livector1D map;
    dvecarr3E localdef = apply(map);

    //reset displacement in a unique vector
    m_gdispl.clear();
    m_gdispl.setDataLocation(mimmo::MPVLocation::POINT);
    m_gdispl.reserve(getGeometry()->getNVertices());
    m_gdispl.setGeometry(getGeometry());
    
    long int ID;
    darray3E zero;
    zero.fill(0.0);
    for (const auto & vertex : container->getVertices()){
        ID = vertex.getId();
        m_gdispl.insert(ID, zero);
    }
    {
        int counter = 0;
        for(const auto &mapp: map){
            m_gdispl[mapp] = localdef[counter];
            ++counter;
        }
    }

};

/*! Apply current deformation setup to a single 3D point. If point is not included in lattice return zero.
 *  The method does not apply filter field modulation if any.
 * \param[in] point coordinate of the points 
 * \return point displacement 
 */
darray3E
FFDLattice::apply(darray3E & point){
    darray3E result;
    result.fill(0.0);
    if(!getShape()->isPointIncluded(point) || !isBuilt()) return result;
    return(nurbsEvaluator(point));
};

/*! Apply current deformation setup to geometry linked as a MimmoObject container, member of the class 
 * (see method getGeometry).If MimmoObject member m_geometry is NULL,return void results. 
 * The method applies filter field modulation if any
 * \param[out] list list of non-zero displacement of m_geometry vertices 
 * \return list of ids of non-zero displaced vertex belonging to geometry
 */
dvecarr3E
FFDLattice::apply(livector1D & list){

    MimmoObject * container = getGeometry();
    if(container == NULL) return dvecarr3E(0);
    if(container->isEmpty() || !isBuilt()) return dvecarr3E(0);

    list.clear();

    //check simplex included and extract their vertex in global IDs;
    if(container->isSkdTreeSupported()) list= container->getVertexFromCellList(getShape()->includeGeometry(container));
    else                               list= getShape()->includeCloudPoints(container);
    //return deformation
    dvecarr3E result = nurbsEvaluator(list);
    if(m_bfilter){

        checkFilter();

        dvecarr3E::iterator itL= result.begin();
        for ( auto && vID : list){
            *itL = (*itL) * m_filter[vID];
            ++itL;
        }
    }
    return(result);
};

/*! Apply current deformation setup to a custom list of 3D points. Only points contained into the lattice will be deformed, 
 *  displacement of the others will be set to zero. If point list is NULL,return void results. 
 *  The method does not apply filter field modulation if any
 * \param[in] point pointer to a list of 3D points.
 * \return displacements of points 
 */
dvecarr3E
FFDLattice::apply(dvecarr3E * point){

    dvecarr3E result;
    if(point ==NULL || !isBuilt() ) return result;

    result.resize(point->size(), darray3E{{0,0,0}});
    livector1D list = getShape()->includeCloudPoints(*point);

    for(const auto & index : list){
        darray3E target = (*point)[index];
        result[index] = nurbsEvaluator(target);
    }

    return(result);
};

/*!
 * Directly apply deformation field to target geometry.
 */
void
FFDLattice::apply(){

    if (getGeometry() == NULL) return;
    if (getGeometry()->isEmpty() || m_gdispl.isEmpty()) return;
    darray3E vertexcoords;
    long int ID;
    for (const auto & vertex : m_geometry->getVertices()){
        vertexcoords = vertex.getCoords();
        ID = vertex.getId();
        vertexcoords += m_gdispl[ID];
        getGeometry()->modifyVertex(vertexcoords, ID);
    }

}

/*!
 * Check if the filter is related to the target geometry.
 * If not create a unitary filter field.
 */
void
FFDLattice::checkFilter(){
    bool check = m_filter.getDataLocation() == mimmo::MPVLocation::POINT;
    check = check && m_filter.completeMissingData(0.0);
    check = check && m_filter.getGeometry() == getGeometry();
    
    if (!check){
        m_log->setPriority(bitpit::log::Verbosity::DEBUG);
        (*m_log)<<"Not valid filter found in "<<m_name<<". Proceeding with default unitary field"<<std::endl;
        m_log->setPriority(bitpit::log::Verbosity::NORMAL);
        
        m_filter.clear();
        m_filter.setGeometry(m_geometry);
        m_filter.setDataLocation(mimmo::MPVLocation::POINT);
        m_filter.reserve(getGeometry()->getNVertices());
        for (const auto & vertex : getGeometry()->getVertices()){
            m_filter.insert(vertex.getId(), 1.0);
        }
    }
}

/*! Convert a target displacement (expressed in local shape ref frame) in XYZ frame
 *    \param[in] target  target displacement
 *  \param[in] i reference displacement index
 *     \return displacement in xyz ref frame
 */
darray3E
FFDLattice::convertDisplToXYZ(darray3E & target, int i){

    darray3E scaling = getScaling();
    darray3E work;
    for(int i=0; i<3; ++i){
        work[i] = target[i]/scaling[i];
    }
    work += getLocalPoint(i);
    darray3E result = transfToGlobal(work) -  getGlobalPoint(i);
    return(result);
};

/*! Convert and return all target displacements (expressed in local shape ref frame) in XYZ frame
 *    \return displacement in xyz ref frame
 */
dvecarr3E
FFDLattice::convertDisplToXYZ(){

    dvecarr3E displ = recoverFullGridDispl();
    int sizeD = displ.size();

    dvecarr3E result(sizeD);
    for(int i=0; i<sizeD; ++i){
        result[i] = convertDisplToXYZ(displ[i],i);
    }
    return(result);
};

/*! Return displacement of a given point, under the deformation effect of the whole Lattice. 
 * \param[in] pointOr 3D point
 * \return point displacement
 */
darray3E
FFDLattice::nurbsEvaluator(darray3E & pointOr){

    darray3E point = transfToLocal(pointOr);
    darray3E scaling = getShape()->getScaling();

    ivector1D knotInterval(3,0);
    dvector2D BSbasis(3);
    dvector1D valH(4,0), temp1(4,0),temp2(4,0), zeros(4,0);
    int uind, vind, wind, index;

    dvecarr3E* displ = getDisplacements();

    int i0 = m_mapdeg[0];
    int i1 = m_mapdeg[1];
    int i2 = m_mapdeg[2];

    iarray3E mappedIndex;

    for(int i=0; i<3; i++){
        knotInterval[i] = getKnotInterval(point[i],i);
        BSbasis[i] = basisITS0(knotInterval[i], i, point[i]);
    }

    uind = knotInterval[i0] - m_deg[i0];
    vind = knotInterval[i1] - m_deg[i1];
    wind = knotInterval[i2] - m_deg[i2];

    for(int i=0; i<=m_deg[i0]; ++i){

        mappedIndex[0] = uind+i;
        temp1 = zeros;

        for(int j=0; j<=m_deg[i1]; ++j){

            mappedIndex[1] = vind+j;
            temp2 = zeros;

            for(int k=0; k<=m_deg[i2]; ++k){

                mappedIndex[2] = wind+k;
                index = accessMapNodes(mappedIndex[0],mappedIndex[1],mappedIndex[2]);

                for(int intv=0; intv<3; ++intv){
                    temp2[intv] += BSbasis[i2][k]*m_weights[m_intMapDOF[index]]*(*displ)[m_intMapDOF[index]][intv];
                }
                temp2[3] += BSbasis[i2][k]*m_weights[index];
            }
            for(int intv=0; intv<4; ++intv){
                temp1[intv] += BSbasis[i1][j]*temp2[intv];
            }

        }
        for(int intv=0; intv<4; ++intv){
            valH[intv] += BSbasis[i0][i]*temp1[intv];
        }
    }

    darray3E outres;
    if(isDisplGlobal()){
        for(int i=0; i<3; ++i){
            outres[i] =  valH[i]/valH[3];
        }

    }else{
        //summing scaled displ in local ref frame;
        for(int i=0; i<3; ++i){
            point[i] +=  valH[i]/(valH[3]*scaling[i]);
        }

        //get final displ in global ref frame:
        outres = transfToGlobal(point) - pointOr;
    }
    return(outres);

}; 

/*! Return displacement of a list of points,
 * under the deformation effect of the whole Lattice.
 *  
 * \param[in] list 3D points
 * \return points displacements
 */
dvecarr3E
FFDLattice::nurbsEvaluator(livector1D & list){

    bitpit::PatchKernel * tri = getGeometry()->getPatch();
    long int id;
    int lsize = list.size();
    livector1D::iterator it, itend = list.end();
    darray3E target, point;
    ivector1D knotInterval(3,0);
    dvector1D BSbasisi0, BSbasisi1, BSbasisi2;
    dvector1D valH(4,0), temp1(4,0),temp2(4,0);

    dvecarr3E displ = recoverFullGridDispl();
    dvector1D weig = recoverFullNodeWeights();

    int uind, vind, wind, index;

    int intv, i, j, k;

    int i0 = m_mapdeg[0];
    int i1 = m_mapdeg[1];
    int i2 = m_mapdeg[2];
    iarray3E mappedIndex;

    int md0 = m_deg[i0];
    int md1 = m_deg[i1];
    int md2 = m_deg[i2];

    dvecarr3E outres(lsize);
    dvecarr3E::iterator itout = outres.begin();
    darray3E scaling = getShape()->getScaling();

    double bbasisw2;
    double bbasis1, bbasis0;

    for(it = list.begin(); it != itend; ++it){

        for(intv=0; intv<4; ++intv){
            valH[intv] = 0.0;
        }
        id  = *it;
        target = tri->getVertex(id).getCoords();
        point = transfToLocal(target);

        // get reference Interval int the knot matrix
        for(i=0; i<3; i++){
            knotInterval[i] = getKnotInterval(point[i],i);
        }
        BSbasisi0 = basisITS0(knotInterval[i0], i0, point[i0]);
        BSbasisi1 = basisITS0(knotInterval[i1], i1, point[i1]);
        BSbasisi2 = basisITS0(knotInterval[i2], i2, point[i2]);

        uind = knotInterval[i0] - md0;
        vind = knotInterval[i1] - md1;
        wind = knotInterval[i2] - md2;


        for(i=0; i<=md0; ++i){

            mappedIndex[i0] = uind + i;

            for(int intv=0; intv<4; ++intv){
                temp1[intv] = 0.0;
            }

            for(j=0; j<=md1; ++j){

                mappedIndex[i1] = vind + j;

                for(intv=0; intv<4; ++intv){
                    temp2[intv] = 0.0;
                }

                for(k=0; k<=md2; ++k){

                    mappedIndex[i2] = wind + k;

                    index = accessMapNodes(mappedIndex[0], mappedIndex[1], mappedIndex[2]);

                    bbasisw2 = BSbasisi2[k]* weig[index];

                    for(intv=0; intv<3; ++intv){
                        temp2[intv] += bbasisw2 * displ[index][intv];
                    }
                    temp2[3] += bbasisw2;

                }
                bbasis1 = BSbasisi1[j];
                for(intv=0; intv<4; ++intv){
                    temp1[intv] += bbasis1*temp2[intv];
                }

            }
            bbasis0 = BSbasisi0[i];
            for(intv=0; intv<4; ++intv){
                valH[intv] += bbasis0*temp1[intv];
            }
        }


        if(isDisplGlobal()){

            //adding to local point displ rescaled
            for(i=0; i<3; ++i){
                (*itout)[i] = valH[i]/valH[3];
            }

        }else{

            //adding to local point displ rescaled
            for(i=0; i<3; ++i){
                point[i]+= valH[i]/(valH[3]*scaling[i]);
            }

            //get absolute displ as difference of
            for(i=0; i<3; ++i){
                (*itout)[i] = transfToGlobal(point)[i] - target[i];
            }

        }

        itout++;

    }//next list id

    itout = outres.end();

    return(outres);

};

/*! Return a specified component of a displacement of a given point, under the deformation effect of the whole Lattice. 
 * \param[in] coordOr 3D point
 * \param[in] targ component of displacement vector (0,1,2)
 * \return point displacement disp[intV]
 */
double
FFDLattice::nurbsEvaluatorScalar(darray3E & coordOr, int targ){

    darray3E point = transfToLocal(coordOr);
    double scaling = getScaling()[targ];

    ivector1D knotInterval(3,0);
    dvector2D BSbasis(3);
    dvector1D valH(2,0), temp1(2,0),temp2(2,0), zeros(2,0);
    int uind, vind, wind, index;

    dvecarr3E* displ = getDisplacements();

    int i0 = m_mapdeg[0];
    int i1 = m_mapdeg[1];
    int i2 = m_mapdeg[2];

    int md0 = m_deg[i0];
    int md1 = m_deg[i1];
    int md2 = m_deg[i2];

    iarray3E mappedIndex;

    for(int i=0; i<3; i++){
        knotInterval[i] = getKnotInterval(point[i],i);
        BSbasis[i] = basisITS0(knotInterval[i], i, point[i]);
    }

    uind = knotInterval[i0] - md0;
    vind = knotInterval[i1] - md1;
    wind = knotInterval[i2] - md2;

    for(int i=0; i<=md0; ++i){

        mappedIndex[0] = uind+i;
        temp1 = zeros;

        for(int j=0; j<=md1; ++j){

            mappedIndex[1] = vind+j;
            temp2 = zeros;

            for(int k=0; k<=md2; ++k){

                mappedIndex[2] = wind+k;
                index = accessMapNodes(mappedIndex[0],mappedIndex[1],mappedIndex[2]);
                temp2[0] += BSbasis[i2][k]*m_weights[m_intMapDOF[index]]*(*displ)[m_intMapDOF[index]][targ];
                temp2[1] += BSbasis[i2][k]*m_weights[index];
            }

            for(int intv=0; intv<2; ++intv){
                temp1[intv] += BSbasis[i1][j]*temp2[intv];
            }

        }

        for(int intv=0; intv<2; ++intv){
            valH[intv] += BSbasis[i0][i]*temp1[intv];
        }
    }

    darray3E res;
    if(isDisplGlobal()){
        res[targ] = valH[0]/valH[1];
    }else{
        point[targ] += valH[0]/(valH[1]*scaling);
        res = transfToGlobal(point)- coordOr;
    }
    return(res[targ]);
};

/*!Return the local basis function of a Nurbs Curve.Please refer to NURBS book of PEIGL 
 * for this Inverted Triangular Scheme Algorithm (pag 74); 
 *\param[in] k  local knot interval in which coord resides -> theoretical knot indexing, 
 *\param[in] pos identifies which nurbs curve of lattice (3 curve for 3 box direction) you are pointing
 *\param[in] coord the evaluation point on the curve
 *\return result local basis of ITS algorithm-> coefficient of interpolation of control nodes for position u of curve pos
 */
dvector1D
FFDLattice::basisITS0(int k, int pos, double coord){

    //return local basis function given the local interval in theoretical knot index,
    //local degree of the curve -> Please refer to NURBS book of PEIGL for this Inverted Triangular Scheme Algorithm (pag 74);
    int dd1 = m_deg[pos]+1;
    dvector1D basis(dd1,1);
    dvector1D left(dd1,0), right(dd1,0);
    double saved, tmp;

    for(int j = 1; j < dd1; ++j){
        saved = 0.0;
        left[j] = coord - getKnotValue(k+1-j, pos);
        right[j]= getKnotValue(k+j, pos) - coord;

        for(int r = 0; r < j; ++r){
            tmp = basis[r]/(right[r+1] + left[j-r]);
            basis[r] = saved + right[r+1] * tmp;
            saved = left[j-r] * tmp;
        }//next r

        basis[j] = saved;
    }//next j

    return(basis);
};

/*!Return list of equally spaced knots for the Nurbs curve in a specific lattice direction
 * \param[in] dir 0,1,2 int identifier of Lattice's Nurbs Curve.
 * \return Nurbs knots
 */
dvector1D
FFDLattice::getNodeSpacing(int dir){

    dvector1D result;
    int dim = getDimension()[dir];
    double span = getLocalSpan()[dir];
    double locOr= getShape()->getLocalOrigin()[dir];

    int nn, retroOrigin;
    double dKn, origin;

    switch(getCoordType(dir))
    {
    case CoordType::PERIODIC :
        nn = dim+m_deg[dir]-1;
        result.resize(nn);
        dKn = span/(dim-1);

        retroOrigin = (m_deg[dir]-1)/2 + (m_deg[dir]-1)%2;
        origin = locOr-1.0 * retroOrigin * dKn;

        for(int i=0; i<nn; ++i){
            result[i] = origin + i*dKn;
        }
        break;

    case CoordType::SYMMETRIC :
        nn = dim+m_deg[dir]-1;
        result.resize(nn);
        dKn = span/(dim-1);

        retroOrigin = (m_deg[dir]-1)/2 + (m_deg[dir]-1)%2;
        origin = locOr-1.0 * retroOrigin * dKn;

        for(int i=0; i<nn; ++i){
            result[i] = origin + i*dKn;
        }
        break;

    case CoordType::CLAMPED :
        nn = dim;
        result.resize(nn);
        dKn = span/(dim-1);
        for(int i=0; i<nn; ++i){
            result[i] =locOr+ i*dKn;
        }
        break;

    case CoordType::UNCLAMPED :
        nn = dim;
        result.resize(nn);
        dKn = span/(dim-1);
        for(int i=0; i<nn; ++i){
            result[i] =locOr+ i*dKn;
        }
        break;

    default: //never been reached
        break;
    }

    return(result);
};

/*!Clean all knots stuff in your lattice */
void
FFDLattice::clearKnots(){

    m_knots.clear();
    m_mapEff.clear();
    m_weights.clear();
    m_mapNodes.clear();
    m_collect_wg.clear();

    m_knots.resize(3);
    m_mapEff.resize(3);
    m_deg.fill(1.0);
    m_mapNodes.resize(3);
    m_mapdeg[0]=0; m_mapdeg[1]=1; m_mapdeg[2]=2;

};

/*! Apply knots structure after modifications to Nurbs curve degrees member */
void
FFDLattice::setKnotsStructure(){
    for(int i=0; i<3; i++){
        setKnotsStructure(i,getCoordType(i));
    }
}

/*! Apply knots structure after modifications to Nurbs curve degrees member, to a specific curve knots structure.
 * Curve is identified by a int 0,1,2 for Nurbs curves in x,y,z, directions respectively.
 * \param[in] dir direction
 * \param[in] type identifies the type of coordinates (UNCLAMPED, CLAMPED, PERIODIC or SYMMETRIC)
 */
void
FFDLattice::setKnotsStructure(int dir,CoordType type){

    //recover number of node for direction dir;
    iarray3E dim = getDimension();
    int nn = dim[dir];

    // free necessary knot structures
    m_knots[dir].clear();
    m_mapEff[dir].clear();

    dvector1D equinode = getNodeSpacing(dir);
    int nEff, kEff,kTheo, kend;
    double tol = 1.0E-12;

    switch(type){
    //clamped curve structure
    case CoordType::CLAMPED :

        m_deg[dir] = min(m_deg[dir], nn-1);
        kEff = nn - m_deg[dir] + 1;
        kTheo = nn + m_deg[dir] + 1;
        m_knots[dir].resize(kEff);
        m_mapEff[dir].resize(kTheo,0);

        //set knots grid in the dir space considered 1 for this example
        m_knots[dir][0] = equinode[0];
        m_knots[dir][kEff-1] = equinode[equinode.size()-1];

        for(int i=1; i<kEff-1; ++i){
            m_knots[dir][i] = 0.0;
            for(int j=i; j<=i+m_deg[dir]-1; ++j){
                m_knots[dir][i] += equinode[j]/((double) m_deg[dir]);
            }
        }

        for(int i=m_deg[dir]; i<(m_deg[dir]+kEff); i++){
            m_mapEff[dir][i]=i-m_deg[dir];
        }

        for(int i=(m_deg[dir]+kEff); i<kTheo; i++){
            m_mapEff[dir][i]=kEff-1;
        }
        break;

    case CoordType::PERIODIC :

        m_deg[dir] = min(m_deg[dir], nn-1);
        nEff = nn + m_deg[dir] - 1;
        kEff = nEff -m_deg[dir] + 1;
        kTheo = nEff +m_deg[dir] + 1;
        m_knots[dir].resize(kTheo);
        m_mapEff[dir].resize(kTheo,0);

        //set knots grid in the dir space considered 1 for this example

        m_knots[dir][m_deg[dir]] = 0.0;
        for(int j=0; j<=m_deg[dir]-1; ++j){
            m_knots[dir][m_deg[dir]] += equinode[j]/((double) m_deg[dir]);
        }
        if(abs(m_knots[dir][m_deg[dir]])<1.e-12) m_knots[dir][m_deg[dir]] = 0.0;

        for(int i=1; i<kEff; ++i){
            m_knots[dir][i+m_deg[dir]] = 0.0;
            for(int j=i; j<=i+m_deg[dir]-1; ++j){
                m_knots[dir][i+m_deg[dir]] += equinode[j]/((double) m_deg[dir]);
            }
        }

        kend = m_deg[dir] + kEff-1;
        //unclamp the other nodes
        for(int i=0; i<m_deg[dir]; ++i){
            m_knots[dir][m_deg[dir]-i-1] = m_knots[dir][m_deg[dir]-i] - (m_knots[dir][kend-i] - m_knots[dir][kend-i-1]);
            m_knots[dir][kend +1 +i] = m_knots[dir][kend + i] + (m_knots[dir][m_deg[dir]+i+1] - m_knots[dir][m_deg[dir]+i]);
        }

        for(int i=0; i<kTheo; i++){
            m_mapEff[dir][i]=i;
        }
        break;

    case CoordType::SYMMETRIC :

        m_deg[dir] = min(m_deg[dir], nn-1);
        nEff = nn + m_deg[dir]-1;
        kEff = nEff -m_deg[dir] + 1;
        kTheo = nEff +m_deg[dir] + 1;
        m_knots[dir].resize(kTheo);
        m_mapEff[dir].resize(kTheo,0);

        //set knots grid in the dir space considered 1 for this example

        m_knots[dir][m_deg[dir]] = 0.0;
        for(int j=0; j<=m_deg[dir]-1; ++j){
            m_knots[dir][m_deg[dir]] += equinode[j]/((double) m_deg[dir]);
        }
        if(abs(m_knots[dir][m_deg[dir]])<1.e-12) m_knots[dir][m_deg[dir]] = 0.0;

        for(int i=1; i<kEff; ++i){
            m_knots[dir][i+m_deg[dir]] = 0.0;
            for(int j=i; j<=i+m_deg[dir]-1; ++j){
                m_knots[dir][i+m_deg[dir]] += equinode[j]/((double) m_deg[dir]);
            }
        }

        kend = m_deg[dir] + kEff-1;
        //unclamp the other nodes
        for(int i=0; i<m_deg[dir]; ++i){
            m_knots[dir][m_deg[dir]-i-1] = m_knots[dir][m_deg[dir]-i] - (m_knots[dir][kend-i] - m_knots[dir][kend-i-1]);
            m_knots[dir][kend +1 +i] = m_knots[dir][kend + i] + (m_knots[dir][m_deg[dir]+i+1] - m_knots[dir][m_deg[dir]+i]);
        }

        for(int i=0; i<kTheo; i++){
            m_mapEff[dir][i]=i;
        }
        break;


    case CoordType::UNCLAMPED :

        m_deg[dir] = min(m_deg[dir], nn-1);
        kEff = nn - m_deg[dir] + 1;
        kTheo = nn + m_deg[dir] + 1;
        m_knots[dir].resize(kTheo);
        m_mapEff[dir].resize(kTheo,0);

        kend = m_deg[dir] + kEff-1;
        //set knots grid in the dir space considered 1 for this example
        m_knots[dir][m_deg[dir]] = equinode[0];
        m_knots[dir][kend] = equinode[equinode.size()-1] + tol;

        for(int i=1; i<kEff-1; ++i){
            m_knots[dir][m_deg[dir]+i] = 0.0;
            for(int j=i; j<=i+m_deg[dir]-1; ++j){
                m_knots[dir][m_deg[dir]+i] += equinode[j]/((double) m_deg[dir]);
            }
        }

        //unclamp the other nodes
        for(int i=0; i<m_deg[dir]; ++i){
            m_knots[dir][m_deg[dir]-i-1] = m_knots[dir][m_deg[dir]-i] - (m_knots[dir][kend-i] - m_knots[dir][kend-i-1]);
            m_knots[dir][kend +1 +i] = m_knots[dir][kend + i] + (m_knots[dir][m_deg[dir]+i+1] - m_knots[dir][m_deg[dir]+i]);
        }

        for(int i=0; i<kTheo; i++){
            m_mapEff[dir][i]=i;
        }
        break;

    default: //never been reached
        break;
    }

    setMapNodes(dir);

};

/*! Given a knots distribution for one curve in direction "dir", return the index of
 * interval which a coord belongs to
 * \param[in] coord target position
 * \param[in] dir   0,1,2 identifies the three direction x,y,z in space, and the relative knots distribution
 * \return interval index.
 */
int
FFDLattice::getKnotInterval(double coord, int dir){

    int size = m_knots[dir].size();
    if(coord< m_knots[dir][0] ){ return(getTheoreticalKnotIndex(0, dir));}
    if(coord >= m_knots[dir][size-1]){ return(getTheoreticalKnotIndex(size-2, dir));}

    int low = 0;
    int high = size-1;
    int mid = (low + high)/2;
    while( coord < m_knots[dir][mid] || coord>= m_knots[dir][mid+1]){
        if(coord < m_knots[dir][mid])    {high=mid;}
        else                {low=mid;}
        mid = (low+high)/2;
    }
    return(getTheoreticalKnotIndex(mid, dir));
};

/*! Return value of a knot for a given its theoretical index and a direction in space
 * \param[in] index theoretical index of the knot
 * \param[in] dir 0,1,2 identifies the three direction x,y,z in space, and the relative knots distribution
 * \return knot value
 */
double
FFDLattice::getKnotValue(int index, int dir){
    int target = getKnotIndex(index, dir);
    if(target ==-1){return(-1.0);}
    return(m_knots[dir][target]);
};

/*! Return a knot real index given its theoretical index and a direction in space
 * \param[in] index theoretical index of the knot
 * \param[in] dir 0,1,2 identifies the three direction x,y,z in space, and the relative knots distribution
 * \return knot index
 */
int
FFDLattice::getKnotIndex(int index ,int dir){
    if(index < 0 || index>=(int)m_mapEff[dir].size()) return -1;
    return(m_mapEff[dir][index]);
};

/*! Return a knot theoretical index given its real index and a direction in space
 * \param[in] locIndex theoretical index of the knot
 * \param[in] dir 0,1,2 identifies the three direction x,y,z in space, and the relative knots distribution
 * \return theoretical  knot index
 */
int
FFDLattice::getTheoreticalKnotIndex(int locIndex,int dir){
    if(locIndex <0 || locIndex >= (int)m_knots[dir].size()){return(-1);}

    // search from the end your m_mapEff vector
    ivector1D::reverse_iterator it = find(m_mapEff[dir].rbegin(), m_mapEff[dir].rend(), locIndex);
    int result = std::distance(m_mapEff[dir].begin(), (it.base()-1));
    return(result);
};

/*! Resize map of effective nodes of the lattice grid to fit a total number od degree of freedom nx*ny*nz.
 * Old structure is deleted and reset to zero.
 */
void         FFDLattice::resizeMapDof(){
    Lattice::resizeMapDof();
    m_displ.clear();
    m_displ.resize(m_np, darray3E{{0.0,0.0,0.0}});
}

/*!
 * Plot Optional results of the class, that is the lattice grid as a hexahedron mesh and as a point cloud
 * in *.vtu format, for both undeformed/ deformed lattice mesh.
 */
void
FFDLattice::plotOptionalResults(){

    std::string dir = m_outputPlot;
    std::string nameGrid  = m_name+"GRID";
    //    std::string nameCloud = m_name+"CLOUD";
    std::string nameGridD  = m_name+"GRID_deformed";
    //    std::string nameCloudD = m_name+"CLOUD_deformed";


    plotGrid(dir, nameGrid, getId(), true, false );
    //    plotCloud(dir, nameCloud, getClassCounter(), true, false );
    plotGrid(dir, nameGridD, getId(), true, true );
    //    plotCloud(dir, nameCloudD, getClassCounter(), true, true );
}

/*! Recover full displacements vector from DOF.
 * \return control nodes displacements
 */
dvecarr3E
FFDLattice::recoverFullGridDispl(){

    iarray3E dim = getDimension();
    int size = dim[0]*dim[1]*dim[2];
    dvecarr3E result(size);
    for(int i=0; i<size; ++i){
        result[i] = m_displ[m_intMapDOF[i]];
    }
    return(result);
};

/*! Recover full displacements vector from DOF */
dvector1D FFDLattice::recoverFullNodeWeights(){

    iarray3E dim = getDimension();
    int size = dim[0]*dim[1]*dim[2];
    dvector1D result(size);
    for(int i=0; i<size; ++i){
        result[i] = m_weights[m_intMapDOF[i]];
    }
    return(result);
};

/*! Fill m_mapnodes, to access correct displacement w knots structure 
 * theoretical knot indexing*/
void
FFDLattice::setMapNodes( int ind){

    int dimdir = getDimension()[ind];
    int nn,preNNumb,postNNumb, pInd;
    m_mapNodes[ind].clear();

    switch(getCoordType(ind)){
    case CoordType::PERIODIC :

        nn = dimdir+m_deg[ind]-1;
        m_mapNodes[ind].resize(nn+1);

        preNNumb = (m_deg[ind]-1)/2 + (m_deg[ind]-1)%2;
        postNNumb = (m_deg[ind]-1) - preNNumb;

        // set the other internal loads
        for(int i=0; i<dimdir; ++i){
            m_mapNodes[ind][i+preNNumb] = i;
        }

        //postpend the first preNNumb loads
        pInd = dimdir - preNNumb -1;
        for(int i=0; i<preNNumb; ++i){
            m_mapNodes[ind][i] = pInd + i;
        }
        //prepend the last postNNumb loads.
        pInd = 1;
        for(int i=0; i<=postNNumb; ++i){
            m_mapNodes[ind][i+preNNumb+dimdir] = pInd+i;
        }
        break;

    case CoordType::SYMMETRIC :

        nn = dimdir+m_deg[ind]-1;
        m_mapNodes[ind].resize(nn+1);

        preNNumb = (m_deg[ind]-1)/2 + (m_deg[ind]-1)%2;
        postNNumb = (m_deg[ind]-1) - preNNumb;

        // set the other internal loads
        for(int i=0; i<dimdir; ++i){
            m_mapNodes[ind][i+preNNumb] = i;
        }

        //prepend symmetrically loads
        for(int i=0; i<preNNumb; ++i){
            m_mapNodes[ind][preNNumb -1 - i] = (i+1)%(dimdir-1);
        }
        //postpend symmetrically loads.
        for(int i=0; i<=postNNumb; ++i){
            m_mapNodes[ind][i+preNNumb+dimdir] = (dimdir-2-i)%(dimdir-1);
        }
        break;

    default:
        m_mapNodes[ind].resize(dimdir);
        for (int i=0; i<dimdir; ++i){
            m_mapNodes[ind][i] = i;
        }
        break;
    }
};

/*! Fill m_mapdeg with the ordered indices of dimensions.
 */
void
FFDLattice::orderDimension(){

    map<pair<int,int>, int > mapsort;
    mapsort[make_pair(m_nx,0)] = 0;
    mapsort[make_pair(m_ny,1)] = 1;
    mapsort[make_pair(m_nz,2)] = 2;

    int i=0;
    for (map<pair<int,int>, int >::iterator it = mapsort.begin(); it != mapsort.end(); ++it){
        m_mapdeg[i] = it->second;
        i++;
    }

};

/*! Build your lattice and all your knot structures. Execute this method every 
 *  time a parameter modification is applied, in order to enable it 
 *  \return id lattice is successfully built.
 */
bool
FFDLattice::build(){
    bool check = Lattice::build();
    //check degrees;
    m_deg[0] = std::min(m_nx, std::max(1, m_deg[0]));
    m_deg[1] = std::min(m_ny, std::max(1, m_deg[1]));
    m_deg[2] = std::min(m_nz, std::max(1, m_deg[2]));

    m_weights.clear();
    m_weights.resize(m_np, 1.0);
    
    m_displ.resize(m_np, {{0.0,0.0,0.0}});

    std::unordered_map<int, double>::iterator it;
    //transfer data from collector of weights m_collect_wg
    for(it=m_collect_wg.begin(); it != m_collect_wg.end(); ++it){
        m_weights[it->first] = it->second;
    }
    //empty m_collect_wg
    m_collect_wg.clear();

    setKnotsStructure();
    orderDimension();
    return check;
};

/*!
 * It sets infos reading from a XML bitpit::Config::section.
 * \param[in] slotXML bitpit::Config::Section of XML file
 * \param[in] name   name associated to the slot
 */
void
FFDLattice::absorbSectionXML(const bitpit::Config::Section & slotXML, std::string name){


    Lattice::absorbSectionXML(slotXML, name);


    if(slotXML.hasOption("CoordType")){
        std::string input = slotXML.get("CoordType");
        std::stringstream ss(bitpit::utils::string::trim(input));
        std::string temp;
        for(int i=0; i<3; ++i){
            ss>>temp;
            temp = bitpit::utils::string::trim(temp);
            if(temp == "SYMMETRIC"){
                setCoordType(CoordType::SYMMETRIC,i);
            }else if(temp =="UNCLAMPED"){
                setCoordType(CoordType::UNCLAMPED,i);
            }else if(temp =="PERIODIC"){
                setCoordType(CoordType::PERIODIC,i);
            }else{
                setCoordType(CoordType::CLAMPED,i);
            }
        }
    };

    if(slotXML.hasOption("Degrees")){
        std::string input = slotXML.get("Degrees");
        input = bitpit::utils::string::trim(input);
        iarray3E temp = {{1,1,1}};
        if(!input.empty()){
            std::stringstream ss(input);
            for(auto &val : temp) ss>>val;
        }
        setDegrees(temp);
    };

    if(slotXML.hasOption("DisplGlobal")){
        std::string input = slotXML.get("DisplGlobal");
        input = bitpit::utils::string::trim(input);
        bool temp = false;
        if(!input.empty()){
            std::stringstream ss(input);
            ss>>temp;
        }
        setDisplGlobal(temp);
    };

};

/*!
 * It sets infos from class members in a XML bitpit::Config::section.
 * \param[in] slotXML bitpit::Config::Section of XML file
 * \param[in] name   name associated to the slot
 */
void
FFDLattice::flushSectionXML(bitpit::Config::Section & slotXML, std::string name){

    Lattice::flushSectionXML(slotXML, name);

    {std::stringstream ss;

    for(int i=0; i<3; ++i){
        std::string towrite = "CLAMPED";
        if(getCoordType()[i] == CoordType::SYMMETRIC){
            towrite = "SYMMETRIC";
        } else if(getCoordType()[i] == CoordType::UNCLAMPED){
            towrite = "UNCLAMPED";
        } else if(getCoordType()[i] == CoordType::PERIODIC){
            towrite = "PERIODIC";
        }
        ss<<towrite<<'\t';
    }
    slotXML.set("CoordType", ss.str());
    }

    {
        std::stringstream ss;
        ss<<getDegrees()[0]<<'\t'<<getDegrees()[1]<<'\t'<<getDegrees()[2];
        slotXML.set("Degrees", ss.str());
    }

    {
        slotXML.set("DisplGlobal", std::to_string(int(isDisplGlobal())));
    }

};

}
