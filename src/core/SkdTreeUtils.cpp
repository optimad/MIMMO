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

# include "SkdTreeUtils.hpp"
# include "MimmoCGUtils.hpp"
# include <bitpit_surfunstructured.hpp>
# include <surface_skd_tree.hpp>
# include <CG.hpp>
# include <queue>

namespace mimmo{

namespace skdTreeUtils{

/*!
 * It computes the unsigned distance of a point to a geometry linked in a SkdTree
 * object. The geometry has to be a surface mesh, in particular an object of type
 * bitpit::SurfUnstructured (a static cast is hardly coded in the method).
 * It searches the element with minimum distance in a box of length 2*r, if none
 * is found return a default value of distance equal to std::numeric_limits<double>::max().
 * \param[in] point Pointer to coordinates of input point.
 * \param[in] tree Pointer to Boundary Volume Hierarchy tree that stores the geometry.
 * \param[out] id Label of the element found as minimum distance element in the skd-tree.
 * \param[in] r Length of the side of the box or radius of the sphere used to search. (The algorithm checks
 * every element encountered inside the box/sphere).
 * \return Unsigned distance of the input point from the patch in the skd-tree.
 */
double distance(const std::array<double,3> *point, const bitpit::PatchSkdTree *tree, long &id, double r)
{

    double h = std::numeric_limits<double>::max();
    if(!tree ){
        throw std::runtime_error("Invalid use of skdTreeUtils::distance method: a void tree is detected.");
    }
    if(!dynamic_cast<const bitpit::SurfUnstructured*>(&(tree->getPatch()))){
        throw std::runtime_error("Invalid use of skdTreeUtils::distance method: a not surface patch tree is detected.");
    }

    static_cast<const bitpit::SurfaceSkdTree*>(tree)->findPointClosestCell(*point, r, &id, &h);
    return h;

}

/*!
 * It computes the signed distance of a point to a geometry linked in a SkdTree
 * object. The geometry must be a surface mesh, in particular an object of type
 * bitpit::SurfUnstructured (a static cast is hardly coded in the method).
 * The sign of the distance is provided by the normal to the geometry locally
 * computed. A positive distance means that the point is located, in respect to the
 * surface mesh, on the same side of the outer normal vector.
 * It searches the element with minimum distance in a box of length 2*r, if none
 * is found return a default value of distance equal to std::numeric_limits<double>::max().
 * \param[in] point Pointer to coordinates of input point.
 * \param[in] tree Pointer to Boundary Volume Hierarchy tree that stores the geometry.
 * \param[out] id Label of the element found as minimum distance element in the skd-tree.
 * \param[out] normal Pseudo-normal of the element (i.e. unit vector with direction (P-xP),
 * where P is the input point and xP the nearest point on the element (simplex) to
 * the projection of P on the plane of the simplex.
 * \param[in] r Length of the side of the box or radius of the sphere used to search. (The algorithm checks
 * every element encountered inside the box/sphere).
 * \return Signed distance of the input point from the patch in the skd-tree.
 */
double signedDistance(const std::array<double,3> *point, const bitpit::PatchSkdTree *tree, long &id, std::array<double,3> &normal, double r)
{

    double h = std::numeric_limits<double>::max();

    if(!tree ){
    	throw std::runtime_error("Invalid use of skdTreeUtils::signedDistance method: a void tree is detected.");
    }
    const bitpit::SurfUnstructured *spatch = dynamic_cast<const bitpit::SurfUnstructured*>(&(tree->getPatch()));
    if(!spatch){
    	throw std::runtime_error("Invalid use of skdTreeUtils::signedDistance method: a not surface patch tree is detected.");
    }

    static_cast<const bitpit::SurfaceSkdTree*>(tree)->findPointClosestCell(*point, r, &id, &h);

    normal = computePseudoNormal(*point, spatch, id);

    return h;

}

/*!
 * It computes the unsigned distance of a set of points to a geometry linked in a SkdTree
 * object. The geometry has to be a surface mesh, in particular an object of type
 * bitpit::SurfUnstructured (a static cast is hardly coded in the method).
 * It searches the element with minimum distance in a box of length 2*r, if none
 * is found return a default value of distance equal to std::numeric_limits<double>::max() in the
 * position of the distances array related to the input point.
 * \param[in] nP Number of input points.
 * \param[in] points Pointer to coordinates of input points.
 * \param[in] tree Pointer to Boundary Volume Hierarchy tree that stores the geometry.
 * \param[out] distances Unsigned distance of the input points from the patch in the skd-tree.
 * \param[out] ids Label of the elements found as minimum distance elements in the skd-tree.
 * \param[in] r Length of the side of the box or radius of the sphere used to search. (The algorithm checks
 * every element encountered inside the box/sphere).
 */
void distance(size_t nP, const std::array<double,3> *points, const bitpit::PatchSkdTree *tree, long *ids, double *distances, double r)
{
    std::vector<double> rs(nP, r);
    distance(nP, points, tree, ids, distances, rs.data());
}

/*!
 * It computes the unsigned distance of a set of points to a geometry linked in a SkdTree
 * object. The geometry has to be a surface mesh, in particular an object of type
 * bitpit::SurfUnstructured (a static cast is hardly coded in the method).
 * It searches the element with minimum distance in a box of length 2*r, if none
 * is found return a default value of distance equal to std::numeric_limits<double>::max() in the
 * position of the distances array related to the input point.
 * \param[in] nP Number of input points.
 * \param[in] points Pointer to coordinates of input points.
 * \param[in] tree Pointer to Boundary Volume Hierarchy tree that stores the geometry.
 * \param[out] distances Unsigned distance of the input points from the patch in the skd-tree.
 * \param[out] ids Label of the elements found as minimum distance elements in the skd-tree.
 * \param[in] r Length of the side of the box or radius of the sphere used to search for each
 * input point. (The algorithm checks every element encountered inside the box/sphere).
 */
void distance(size_t nP, const std::array<double,3> *points, const bitpit::PatchSkdTree *tree, long *ids, double *distances, double *r)
{

    // Initialize distances
    for (std::size_t ip = 0; ip < nP; ip++){
        distances[ip] = std::numeric_limits<double>::max();
    }

    if(!tree ){
        throw std::runtime_error("Invalid use of skdTreeUtils::distance method: a void tree is detected.");
    }
    if(!dynamic_cast<const bitpit::SurfUnstructured*>(&(tree->getPatch()))){
        throw std::runtime_error("Invalid use of skdTreeUtils::distance method: a not surface patch tree is detected.");
    }

    for (std::size_t ip = 0; ip < nP; ip++){
        const std::array<double,3> *point = &points[ip];
        long *id = &ids[ip];
        double *distance = &distances[ip];
        double rpoint = r[ip];
        static_cast<const bitpit::SurfaceSkdTree*>(tree)->findPointClosestCell(*point, rpoint, id, distance);
    }

}

/*!
 * It computes the signed distance of a set of points to a geometry linked in a SkdTree
 * object. The geometry must be a surface mesh, in particular an object of type
 * bitpit::SurfUnstructured (a static cast is hardly coded in the method).
 * The sign of the distance is provided by the normal to the geometry locally
 * computed. A positive distance means that the point is located, in respect to the
 * surface mesh, on the same side of the outer normal vector.
 * It searches the element with minimum distance in a box of length 2*r, if none
 * is found return a default value of distance equal to std::numeric_limits<double>::max() in the
 * position of the distances array related to the input point.
 * \param[in] nP Number of input points.
 * \param[in] points Pointer to coordinates of input points.
 * \param[in] tree Pointer to Boundary Volume Hierarchy tree that stores the geometry.
 * \param[out] distances Signed distance of the input points from the patch in the skd-tree.
 * \param[out] ids Label of the elements found as minimum distance elements in the skd-tree.
 * \param[out] normals Pseudo-normal of the elements (i.e. unit vector with direction (P-xP),
 * where P is the input point and xP the nearest point on the element (simplex) to
 * the projection of P on the plane of the simplex.
 * \param[in] r Length of the side of the box or radius of the sphere used to search. (The algorithm checks
 * every element encountered inside the box/sphere).
 */
void signedDistance(std::size_t nP, const std::array<double,3> *points, const bitpit::PatchSkdTree *tree, long *ids, double *distances, std::array<double,3> *normals, double r)
{
    std::vector<double> rs(nP, r);
    signedDistance(nP, points, tree, ids, distances, normals, rs.data());
}

/*!
 * It computes the signed distance of a set of points to a geometry linked in a SkdTree
 * object. The geometry must be a surface mesh, in particular an object of type
 * bitpit::SurfUnstructured (a static cast is hardly coded in the method).
 * The sign of the distance is provided by the normal to the geometry locally
 * computed. A positive distance means that the point is located, in respect to the
 * surface mesh, on the same side of the outer normal vector.
 * It searches the element with minimum distance in a box of length 2*r, if none
 * is found return a default value of distance equal to std::numeric_limits<double>::max() in the
 * position of the distances array related to the input point.
 * \param[in] nP Number of input points.
 * \param[in] points Pointer to coordinates of input points.
 * \param[in] tree Pointer to Boundary Volume Hierarchy tree that stores the geometry.
 * \param[out] distances Signed distance of the input points from the patch in the skd-tree.
 * \param[out] ids Label of the elements found as minimum distance elements in the skd-tree.
 * \param[out] normals Pseudo-normal of the elements (i.e. unit vector with direction (P-xP),
 * where P is the input point and xP the nearest point on the element (simplex) to
 * the projection of P on the plane of the simplex.
 * \param[in] r Length of the side of the box or radius of the sphere used to search for each input
 * point. (The algorithm checks every element encountered inside the box/sphere).
 */
void signedDistance(std::size_t nP, const std::array<double,3> *points, const bitpit::PatchSkdTree *tree, long *ids, double *distances, std::array<double,3> *normals, double *r)
{

    // Initialize distances
    for (std::size_t ip = 0; ip < nP; ip++){
        distances[ip] = std::numeric_limits<double>::max();
    }

    if(!tree ){
        throw std::runtime_error("Invalid use of skdTreeUtils::signedDistance method: a void tree is detected.");
    }
    const bitpit::SurfUnstructured *spatch = dynamic_cast<const bitpit::SurfUnstructured*>(&(tree->getPatch()));
    if(!spatch){
        throw std::runtime_error("Invalid use of skdTreeUtils::signedDistance method: a not surface patch tree is detected.");
    }

    for (std::size_t ip = 0; ip < nP; ip++){
        const std::array<double,3> *point = &points[ip];
        long *id = &ids[ip];
        double *distance = &distances[ip];
        double rpoint = r[ip];
        static_cast<const bitpit::SurfaceSkdTree*>(tree)->findPointClosestCell(*point, rpoint, id, distance);
        normals[ip] = computePseudoNormal(*point, spatch, *id);
    }

}

/*!
 * It selects the elements of a geometry stored in a skd-tree by a distance criterion
 * in respect to an other geometry stored in a different skd-tree.
 * \param[in] selection Pointer to skd-tree used as selection patch.
 * \param[in] target Pointer to skd-tree that store the target geometry.
 * \param[in] tol Distance threshold used to select the elements of target.
 * \return Vector of the label of all the elements of the target skd-tree placed
 * at a distance <= tol from the bounding boxes of the leaf nodes of the skd-tree
 * selection.
 */
std::vector<long> selectByPatch(bitpit::PatchSkdTree *selection, bitpit::PatchSkdTree *target, double tol){

    int nleafs = selection->getLeafCount();
    int nnodess = selection->getNodeCount();
    std::vector<const bitpit::SkdNode*> leafSelection(nleafs);
    int count = 0;
    for (int i=0; i<nnodess; i++){
        if (selection->getNode(i).isLeaf()){
            if (bitpit::CGElem::intersectBoxBox(selection->getNode(i).getBoxMin()-tol,
                    selection->getNode(i).getBoxMax()+tol,
                    target->getNode(0).getBoxMin(),
                    target->getNode(0).getBoxMax() ) ){
                leafSelection[count] = &(selection->getNode(i));
                count++;

            }
        }
    }
    leafSelection.resize(count);


    std::vector<long> extracted;
    extractTarget(target, leafSelection, extracted, tol);

    return extracted;

}

/*!
 * It extracts the elements of a leaf node of geometry stored in a skd-tree
 * by a distance criterion in respect to an other geometry stored
 * in a different skd-tree. It is a method used in selectByPatch method.
 * \param[in] target Pointer to skd-tree that store the target geometry.
 * \param[in,out] leafSelection Vector of pointers to the leaf nodes currently interesting
 * for the selection procedure.
 * \param[in,out] extracted of the label of all the elements of the target skd-tree,
 * currently found placed at a distance <= tol from the bounding boxes of the
 * leaf nodes in leafSelection.
 * \param[in] tol Distance threshold used to select the elements of target.
 * the next-th node is not a leaf node the method is recursively called.
 *
 *
 */
void extractTarget(bitpit::PatchSkdTree *target, const std::vector<const bitpit::SkdNode*> & leafSelection, std::vector<long> &extracted, double tol){

    if(leafSelection.empty())   return;
    std::size_t rootId = 0;

    std::vector<std::size_t> candidates;
    std::vector<std::pair< std::size_t, std::vector<const bitpit::SkdNode*> > > nodeStack;

    std::vector<const bitpit::SkdNode*> tocheck;
    for (int i=0; i<(int)leafSelection.size(); i++){
        if (bitpit::CGElem::intersectBoxBox(leafSelection[i]->getBoxMin()-tol,
                                            leafSelection[i]->getBoxMax()+tol,
                                            target->getNode(rootId).getBoxMin(),
                                            target->getNode(rootId).getBoxMax() ) )
        {
            tocheck.push_back(leafSelection[i]);
        }
    }
    nodeStack.push_back(std::make_pair(rootId, tocheck) );

    while(!nodeStack.empty()){

        std::pair<std::size_t,  std::vector<const bitpit::SkdNode*> >  touple = nodeStack.back();
        const bitpit::SkdNode & node = target->getNode(touple.first);
        nodeStack.pop_back();


        bool isLeaf = true;
        for (int i = bitpit::SkdNode::CHILD_BEGIN; i != bitpit::SkdNode::CHILD_END; ++i) {
            bitpit::SkdNode::ChildLocation childLocation = static_cast<bitpit::SkdNode::ChildLocation>(i);
            std::size_t childId = node.getChildId(childLocation);
            if (childId != bitpit::SkdNode::NULL_ID) {
                isLeaf = false;
                tocheck.clear();
                for (int i=0; i<(int)touple.second.size(); i++){
                    if (bitpit::CGElem::intersectBoxBox(touple.second[i]->getBoxMin()-tol,
                                                        touple.second[i]->getBoxMax()+tol,
                                                        target->getNode(childId).getBoxMin(),
                                                        target->getNode(childId).getBoxMax() ) )
                    {
                        tocheck.push_back(touple.second[i]);
                    }
                }
                if(!tocheck.empty())    nodeStack.push_back(std::make_pair(childId, tocheck) );
            }
        }

        if (isLeaf) {
            candidates.push_back(touple.first);
        }
    }

    std::unordered_set<long> cellExtracted;
    for(const auto & nodeId: candidates){
        const bitpit::SkdNode & node = target->getNode(nodeId);
        std::vector<long> cellids = node.getCells();
        cellExtracted.insert(cellids.begin(), cellids.end());
    }

    extracted.insert(extracted.end(), cellExtracted.begin(), cellExtracted.end());
}

/*!
 * It computes the projection of a point on a geometry linked in a skd-tree
 * object. The geometry must be a surface mesh, in particular an object of type
 * bitpit::SurfUnstructured (a static cast is hardly coded in the method).
 * It searches the elements of the geometry with minimum distance
 * recursively in a sphere of radius r, by increasing the size r at each step
 * until at least one element is found.
 * \param[in] point Pointer to coordinates of input point.
 * \param[in] skdtree Pointer to Boundary Volume Hierarchy tree that stores the geometry.
 * \param[in] r Initial length of the sphere radius used to search. (The algorithm checks
 * every element encountered inside the sphere).
 * \return Coordinates of the projected point.
 */
darray3E projectPoint(const std::array<double,3> *point, const bitpit::PatchSkdTree *skdtree, double r )
{
    darray3E projected_point;
    long id;
    projectPoint(1, point, skdtree, &projected_point, &id, r);
    return projected_point;
}

/*!
 * It computes the projection of a set of points on a geometry linked in a skd-tree
 * object. The geometry must be a surface mesh, in particular an object of type
 * bitpit::SurfUnstructured (a static cast is hardly coded in the method).
 * It searches the elements of the geometry with minimum distance
 * recursively in a sphere of radius r, by increasing the size r at each step
 * until at least one element is found.
 * \param[in] nP Number of input points.
 * \param[in] points Pointer to coordinates of input points.
 * \param[in] tree Pointer to Boundary Volume Hierarchy tree that stores the geometry.
 * \param[out] ids Labels of the elements found as minimum distance element into the points are projected.
 * \param[in] r Initial length of the sphere radius used to search. (The algorithm checks
 * every element encountered inside the sphere).
 * \param[out] Coordinates of the projected points.
 */
void projectPoint(std::size_t nP, const std::array<double,3> *points, const bitpit::PatchSkdTree *tree, std::array<double,3> *projected_points, long *ids, double r )
{
    std::vector<double> rs(nP, r);
    projectPoint(nP, points, tree, projected_points, ids, rs.data());
}

/*!
 * It computes the projection of a set of points on a geometry linked in a skd-tree
 * object. The geometry must be a surface mesh, in particular an object of type
 * bitpit::SurfUnstructured (a static cast is hardly coded in the method).
 * It searches the elements of the geometry with minimum distance
 * recursively in a sphere of radius r, by increasing the size r at each step
 * until at least one element is found.
 * \param[in] nP Number of input points.
 * \param[in] points Pointer to coordinates of input points.
 * \param[in] tree Pointer to Boundary Volume Hierarchy tree that stores the geometry.
 * \param[out] ids Labels of the elements found as minimum distance element into the points are projected.
 * \param[in] r Initial length of the sphere radius used to search for each input point. (The algorithm checks
 * every element encountered inside the sphere).
 * \param[out] Coordinates of the projected points.
 */
void projectPoint(std::size_t nP, const std::array<double,3> *points, const bitpit::PatchSkdTree *tree, std::array<double,3> *projected_points, long *ids, double *r )
{

    // Initialize ids and ranks
    for (std::size_t ip = 0; ip < nP; ip++){
        ids[ip] = bitpit::Cell::NULL_ID;
        r[ip] = std::max(r[ip], tree->getPatch().getTol());
    }
    std::vector<darray3E>    normals(nP);

    std::vector<double> dist(nP, std::numeric_limits<double>::max());
    double max_dist = std::numeric_limits<double>::max();
    while (max_dist == std::numeric_limits<double>::max()){
        //use method sphere by default
        signedDistance(nP, points, tree, ids, dist.data(), normals.data(), r);
        for (std::size_t ip = 0; ip < nP; ip++){
            if (std::abs(dist[ip]) >= max_dist){
                r[ip] *= 1.5;
            }
        }
        max_dist = std::abs(*std::max_element(dist.begin(), dist.end()));
        max_dist = std::max(max_dist, std::abs(*std::min_element(dist.begin(), dist.end())));
    }

    for (std::size_t ip = 0; ip < nP; ip++){
        projected_points[ip] = points[ip] - dist[ip] * normals[ip];
    }
}

/*!
 * Given the specified point find the cell of a surface patch it is into.
 * The method works only with trees generated with bitpit::SurfUnstructured mesh.
 *
 * \param[in] point is the point
 * \param[in] tree reference to SkdTree relative to the target surface geometry.
 * \return id of the geometry cell the point is into. Return bitpit::Cell::NULL_ID if no cell is found.
 */
long locatePointOnPatch(const std::array<double, 3> &point, const bitpit::PatchSkdTree *tree)
{
    if(!dynamic_cast<const bitpit::SurfUnstructured*>(&(tree->getPatch()))){
        throw std::runtime_error("Invalid use of skdTreeUtils::locatePointOnPatch method: a non surface patch or void patch was detected.");
    }

    // Initialize the cell id and distance
    long id = bitpit::Cell::NULL_ID;
    double distance = std::numeric_limits<double>::max();

    // Find the closest cell to the input point
    long cellId = bitpit::Cell::NULL_ID;
    static_cast<const bitpit::SurfaceSkdTree*>(tree)->findPointClosestCell(point, &cellId, &distance);

    // Check if the point belongs to the closest cell
    bool checkBelong = false;

    bitpit::ElementType eltype = tree->getPatch().getCell(cellId).getType();
    auto vList = tree->getPatch().getCell(cellId).getVertexIds();
    dvecarr3E vertCoords(vList.size());
    int counterList = 0;
    for(const auto & idV : vList){
        vertCoords[counterList] = tree->getPatch().getVertexCoords(idV);
        ++counterList;
    }

    switch(eltype){
    case bitpit::ElementType::LINE:
        checkBelong = mimmoCGUtils::isPointInsideSegment(point, vertCoords[0], vertCoords[1]);
        break;
    case bitpit::ElementType::TRIANGLE:
        checkBelong = mimmoCGUtils::isPointInsideTriangle(point, vertCoords[0], vertCoords[1], vertCoords[2]);
        break;
    case bitpit::ElementType::PIXEL:
    case bitpit::ElementType::QUAD:
    case bitpit::ElementType::POLYGON:
        checkBelong = mimmoCGUtils::isPointInsidePolygon(point, vertCoords);
        break;
    default:
        throw std::runtime_error("Not supported cell type");
        break;
    }
    if (checkBelong) {
        id = cellId;
    }

    return id;
}

/*!
 * It computes the pseudo-normal of a cell of a surface mesh from an input point,
 * i.e. the unit vector with direction (P-xP), where P is the input point and
 * xP is the projection on the cell.
 * \param[in] point point coordinates
 * \param[in] surface_mesh pointer to surface mesh
 * \param[in] id cell id belong to the input surface mesh
 * \return pseudo-normal components
 */
std::array<double, 3>
computePseudoNormal(const std::array<double, 3> &point, const bitpit::SurfUnstructured *surface_mesh, long id)
{

    std::array<double, 3> pseudo_normal({0.,0.,0.});
    double h;

    //Pseudo normal only for 2D element patches
    if (id != bitpit::Cell::NULL_ID){

        const bitpit::Cell & cell = surface_mesh->getCell(id);
        bitpit::ConstProxyVector<long> vertIds = cell.getVertexIds();
        dvecarr3E VS(vertIds.size());
        int count = 0;
        for (const auto & iV: vertIds){
            VS[count] = surface_mesh->getVertexCoords(iV);
            ++count;
        }

        darray3E xP = {{0.0,0.0,0.0}};
        darray3E normal= {{0.0,0.0,0.0}};

        if ( vertIds.size() == 3 ){ //TRIANGLE
            darray3E lambda;
            h = bitpit::CGElem::distancePointTriangle(point, VS[0], VS[1], VS[2],lambda);
            int count = 0;
            for(const auto &val: lambda){
                normal += val * surface_mesh->evalVertexNormal(id,count) ;
                xP += val * VS[count];
                ++count;
            }
        }else if ( vertIds.size() == 2 ){ //LINE/SEGMENT
            darray2E lambda;
            h = bitpit::CGElem::distancePointSegment(point, VS[0], VS[1], lambda);
            int count = 0;
            for(const auto &val: lambda){
                normal += val * surface_mesh->evalVertexNormal(id,count) ;
                xP += val * VS[count];
                ++count;
            }
        }else{ //GENERAL POLYGON
            std::vector<double> lambda;
            h = bitpit::CGElem::distancePointPolygon(point, VS,lambda);
            int count = 0;
            for(const auto &val: lambda){
                normal += val * surface_mesh->evalVertexNormal(id,count) ;
                xP += val * VS[count];
                ++count;
            }
        }

        double s =  sign( dotProduct(normal, point - xP) );
        if(s == 0.0)    s =1.0;
        h = s * h;
        //pseudo-normal (direction P and xP closest point on triangle)
        pseudo_normal = s * (point - xP);
        double normX = norm2(pseudo_normal);
        if(normX < surface_mesh->getTol()){
            pseudo_normal = normal/norm2(normal);
        }else{
            pseudo_normal /= normX;
        }
    }//end if not id null

    return pseudo_normal;
}

/*!
 * It check if a point belongs to an input cell. If the point is located on the plane of the
 * cell and inside it, it returns true, false otherwise.
 * \param[in] point point coordinates
 * \param[in] surface_mesh pointer to surface mesh
 * \param[in] cellId cell id belong to the input surface mesh to be checked
 * \return true if the point belongs to the input cell
 */
bool
checkPointBelongsToCell(const std::array<double, 3> &point, const bitpit::SurfUnstructured *surface_mesh, long cellId)
{
    bool checkBelong = false;
    bitpit::ElementType eltype = surface_mesh->getCell(cellId).getType();
    auto vList = surface_mesh->getCell(cellId).getVertexIds();
    dvecarr3E vertCoords(vList.size());
    int counterList = 0;
    for(const auto & idV : vList){
        vertCoords[counterList] = surface_mesh->getVertexCoords(idV);
        ++counterList;
    }

    switch(eltype){
    case bitpit::ElementType::LINE:
        checkBelong = mimmoCGUtils::isPointInsideSegment(point, vertCoords[0], vertCoords[1]);
        break;
    case bitpit::ElementType::TRIANGLE:
        checkBelong = mimmoCGUtils::isPointInsideTriangle(point, vertCoords[0], vertCoords[1], vertCoords[2]);
        break;
    case bitpit::ElementType::PIXEL:
    case bitpit::ElementType::QUAD:
    case bitpit::ElementType::POLYGON:
        checkBelong = mimmoCGUtils::isPointInsidePolygon(point, vertCoords);
        break;
    default:
        throw std::runtime_error("Not supported cell type");
        break;
    }
    return checkBelong;
}

#if MIMMO_ENABLE_MPI
/*!
 * It computes the unsigned distance of a set of points to a distributed geometry linked in a SkdTree
 * object. Analogous of serial function skdTreeUtils::distance.
 * \param[in] nP Number of input points.
 * \param[in] points Pointer to coordinates of input points.
 * \param[in] tree Pointer to Boundary Volume Hierarchy tree that stores the geometry.
 * \param[out] ids Labels of the elements found as minimum distance element in the skd-tree.
 * \param[out] ranks Ranks of the process owner of the elements found as minimum distance elements in the skd-tree.
 * \param[out] distances Distance of the input points from the patch in the skd-tree.
 * \param[in] shared True if the input points are shared between the processes
 * \param[in] r Length of the side of the box or radius of the sphere used to search. (The algorithm checks
 * every element encountered inside the box/sphere).
 */
void globalDistance(std::size_t nP, const std::array<double,3> *points, const bitpit::PatchSkdTree *tree, long *ids, int *ranks, double *distances, double r, bool shared)
{
    std::vector<double> rs(nP, r);
    globalDistance(nP, points, tree, ids, ranks, distances, rs.data(), shared);
}

/*!
 * It computes the unsigned distance of a set of points to a distributed geometry linked in a SkdTree
 * object. Analogous of serial function skdTreeUtils::distance.
 * \param[in] nP Number of input points.
 * \param[in] points Pointer to coordinates of input points.
 * \param[in] tree Pointer to Boundary Volume Hierarchy tree that stores the geometry.
 * \param[out] ids Labels of the elements found as minimum distance element in the skd-tree.
 * \param[out] ranks Ranks of the process owner of the elements found as minimum distance elements in the skd-tree.
 * \param[out] distances Distance of the input points from the patch in the skd-tree.
 * \param[in] shared True if the input points are shared between the processes
 * \param[in] r Length of the side of the box or radius of the sphere used to search for each input point (The algorithm checks
 * every element encountered inside the box/sphere).
 */
void globalDistance(std::size_t nP, const std::array<double,3> *points, const bitpit::PatchSkdTree *tree, long *ids, int *ranks, double *distances, double* r, bool shared)
{

    if(!tree ){
        throw std::runtime_error("Invalid use of skdTreeUtils::distance method: a void tree is detected.");
    }
    if(!dynamic_cast<const bitpit::SurfUnstructured*>(&(tree->getPatch()))){
        throw std::runtime_error("Invalid use of skdTreeUtils::distance method: a not surface patch tree is detected.");
    }

    if (shared){
        findSharedPointClosestGlobalCell(nP, points, tree, ids, ranks, distances, r);
    } else {
        static_cast<const bitpit::SurfaceSkdTree*>(tree)->findPointClosestGlobalCell(nP, points, r, ids, ranks, distances);
    }

}

/*!
 * It computes the signed global distance of a set of points to a distributed geometry linked in a SkdTree
 * object. Analogous of serial function skdTreeUtils::signedDistance.
 * \param[in] nP Number of input points.
 * \param[in] points Pointer to coordinates of input points.
 * \param[in] tree Pointer to Boundary Volume Hierarchy tree that stores the geometry.
 * \param[out] ids Labels of the elements found as minimum distance element in the skd-tree.
 * \param[out] ranks Ranks of the process owner of the elements found as minimum distance elements in the skd-tree.
 * \param[out] normals Pseudo-normals of the elements (i.e. unit vector with direction (P-xP),
 * where P is the input point and xP the nearest point on the element (simplex) to
 * the projection of P on the plane of the simplex.
 * \param[out] distances Signed distance of the input points from the patch in the skd-tree.
 * \param[in] shared True if the input points are shared between the processes
 * \param[in] r Length of the side of the box or radius of the sphere used to search. (The algorithm checks
 * every element encountered inside the box/sphere).
 */
void signedGlobalDistance(std::size_t nP, const std::array<double,3> *points, const bitpit::PatchSkdTree *tree, long *ids, int *ranks, std::array<double,3> *normals, double *distances, double r, bool shared)
{
    std::vector<double> rs(nP, r);
    signedGlobalDistance(nP, points, tree, ids, ranks, normals, distances, rs.data(), shared);
}

/*!
 * It computes the signed global distance of a set of points to a distributed geometry linked in a SkdTree
 * object. Analogous of serial function skdTreeUtils::signedDistance.
 * \param[in] nP Number of input points.
 * \param[in] points Pointer to coordinates of input points.
 * \param[in] tree Pointer to Boundary Volume Hierarchy tree that stores the geometry.
 * \param[out] ids Labels of the elements found as minimum distance element in the skd-tree.
 * \param[out] ranks Ranks of the process owner of the elements found as minimum distance elements in the skd-tree.
 * \param[out] normals Pseudo-normals of the elements (i.e. unit vector with direction (P-xP),
 * where P is the input point and xP the nearest point on the element (simplex) to
 * the projection of P on the plane of the simplex.
 * \param[out] distances Signed distance of the input points from the patch in the skd-tree.
 * \param[in] shared True if the input points are shared between the processes
 * \param[in] r Length of the side of the box or radius of the sphere used to search for each input point. (The algorithm checks
 * every element encountered inside the box/sphere).
 */
void signedGlobalDistance(std::size_t nP, const std::array<double,3> *points, const bitpit::PatchSkdTree *tree, long *ids, int *ranks, std::array<double,3> *normals, double *distances, double *r, bool shared)
{

    if(!tree){
        throw std::runtime_error("Invalid use of skdTreeUtils::signedDistance method: a void tree is detected.");
    }
    const bitpit::SurfUnstructured & spatch = static_cast<const bitpit::SurfUnstructured&>(tree->getPatch());

    if (shared){
        // All processes share the points
        findSharedPointClosestGlobalCell(nP, points, tree, ids, ranks, distances, r);
    } else {
        // Distributed points
        static_cast<const bitpit::SurfaceSkdTree*>(tree)->findPointClosestGlobalCell(nP, points, r, ids, ranks, distances);
    }

    int myrank = spatch.getRank();

    // Map with rank to ask info in case of distributed points
    std::map<int,std::vector<long>> ask_to_rank;
    std::map<int,std::vector<std::array<double,3>>> point_to_rank;
    std::map<int,std::vector<std::size_t>> point_index_received_from_rank;

    // Loop on points
    for (std::size_t ip = 0; ip < nP; ip++){

        const std::array<double,3> & point = points[ip];
        const long & cellId = ids[ip];
        const int & cellRank = ranks[ip];
        double & distance = distances[ip];
        std::array<double,3> & pseudo_normal = normals[ip];

        // If the current rank is the the owner of the cell compute normal
        if (cellRank == myrank){

            pseudo_normal = computePseudoNormal(point, &spatch, cellId);

        } else {

            // If not owner fix the normal to maximum value
            pseudo_normal = std::array<double,3>({std::numeric_limits<double>::max(), std::numeric_limits<double>::max(), std::numeric_limits<double>::max()});

            if (!shared){
                // Push to rank map to ask normal
                ask_to_rank[cellRank].push_back(cellId);

                // Push to rank map the related points to ask normal
                point_to_rank[cellRank].push_back(point);

                // Push to rank map the index of the points to fill output normals
                point_index_received_from_rank[cellRank].push_back(ip);
            }

        } // end if owner rank

    } // end loop on points

    int nProcessors = spatch.getProcessorCount();

    if (nProcessors > 1 && spatch.isPartitioned()){
        if (shared){

            // If all points are shared between processes all reduce on normal by minimum operation
            MPI_Allreduce(MPI_IN_PLACE, normals, 3*nP, MPI_DOUBLE, MPI_MIN, tree->getCommunicator());

        } else {

            // Send/receive number of normals to send to other processes
            std::vector<std::size_t> n_send_to_rank(nProcessors, 0);
            for (int irank = 0; irank < nProcessors; irank++){
                std::size_t n_ask_to_current_rank = ask_to_rank[irank].size();
                MPI_Gather(&n_ask_to_current_rank, 1, MPI_LONG, n_send_to_rank.data(), 1, MPI_LONG, irank, tree->getCommunicator());
            }

            // Map with rank to send info for cellId and point coordinates
            std::map<int,std::vector<long>> send_to_rank;
            std::map<int,std::vector<std::array<double,3>>> point_from_rank;
            for (int irank = 0; irank < nProcessors; irank++){
                send_to_rank[irank].resize(n_send_to_rank[irank], bitpit::Cell::NULL_ID);
                point_from_rank[irank].resize(n_send_to_rank[irank], std::array<double,3>({{0.,0.,0.}}));
            }

            // Recover id of the cell and points on which compute the pseudonormal
            for (int irank = 0; irank < nProcessors; irank++){
                if (myrank == irank){
                    for (int jrank = 0; jrank < nProcessors; jrank++){
                        if (jrank != irank){
                            std::size_t n_send_to_current_rank = n_send_to_rank[jrank];
                            MPI_Recv(send_to_rank[jrank].data(), n_send_to_current_rank, MPI_LONG, jrank, 100, tree->getCommunicator(), MPI_STATUS_IGNORE);
                            MPI_Recv(point_from_rank[jrank].data(), n_send_to_current_rank*3, MPI_DOUBLE, jrank, 101, tree->getCommunicator(), MPI_STATUS_IGNORE);
                        }
                    }
                } else {
                    std::size_t n_ask_to_current_rank = ask_to_rank[irank].size();
                    MPI_Send(ask_to_rank[irank].data(), n_ask_to_current_rank, MPI_LONG, irank, 100, tree->getCommunicator());
                    MPI_Send(point_to_rank[irank].data(), n_ask_to_current_rank*3, MPI_DOUBLE, irank, 101, tree->getCommunicator());
                }
            }

            // Compute pseudo-normal for each received point from each rank
            std::map<int, std::vector<std::array<double,3>>> normal_to_rank;
            for (std::size_t irank = 0; irank < nProcessors; irank++){
                normal_to_rank[irank].resize(n_send_to_rank[irank], std::array<double,3>({std::numeric_limits<double>::max(), std::numeric_limits<double>::max(), std::numeric_limits<double>::max()}));
                for (std::size_t ip = 0; ip < n_send_to_rank[irank]; ip++){

                    const std::array<double,3> & point = point_from_rank[irank][ip];
                    const long & cellId = send_to_rank[irank][ip];
                    std::array<double,3> & pseudo_normal = normal_to_rank[irank][ip];

                    pseudo_normal = computePseudoNormal(point, &spatch, cellId);

                } // end loop on points received
            }  // end loop on ranks

            // Communicate back the computed pseudonormals
            std::map<int, std::vector<std::array<double,3>>> normal_from_rank;

            for (int irank = 0; irank < nProcessors; irank++){
                if (myrank == irank){
                    for (int jrank = 0; jrank < nProcessors; jrank++){
                        if (jrank != irank){
                            std::size_t n_ask_to_current_rank = ask_to_rank[jrank].size();
                            normal_from_rank[jrank].resize(n_ask_to_current_rank);
                            MPI_Recv(normal_from_rank[jrank].data(), n_ask_to_current_rank*3, MPI_DOUBLE, jrank, 102, tree->getCommunicator(), MPI_STATUS_IGNORE);

                            // Fill normals output with the correct received pseudonormals (TODO move out of communications scope?)
                            for (std::size_t ip = 0; ip < n_ask_to_current_rank; ip++){
                                std::size_t point_index = point_index_received_from_rank[jrank][ip];
                                normals[point_index] = normal_from_rank[jrank][ip];
                            }

                        }
                    }
                } else {
                    std::size_t n_send_to_current_rank = n_send_to_rank[irank];
                    MPI_Send(normal_to_rank[irank].data(), n_send_to_current_rank*3, MPI_DOUBLE, irank, 102, tree->getCommunicator());
                }
            }

        } // end if not shared points
    }
}

/*!
 * It computes the projection of a set of points on a distributed geometry linked in a skd-tree
 * object. Analogous of serial function skdTreeUtils::projectPoint.
 * \param[in] nP Number of input points.
 * \param[in] points Pointer to coordinates of input points.
 * \param[in] tree Pointer to Boundary Volume Hierarchy tree that stores the geometry.
 * \param[out] ids Labels of the elements found as minimum distance element into the points are projected.
 * \param[out] ranks Ranks of the process owner of the elements into the points are projected.
 * \param[in] r Initial length of the sphere radius used to search. (The algorithm checks
 * every element encountered inside the sphere).
 * \param[out] Coordinates of the projected points.
 */
void projectPointGlobal(std::size_t nP, const std::array<double,3> *points, const bitpit::PatchSkdTree *tree, std::array<double,3> *projected_points, long *ids, int *ranks, double r, bool shared )
{
    std::vector<double> rs(nP, r);
    projectPointGlobal(nP, points, tree, projected_points, ids, ranks, rs.data(), shared);
}

/*!
 * It computes the projection of a set of points on a distributed geometry linked in a skd-tree
 * object. Analogous of serial function skdTreeUtils::projectPoint.
 * \param[in] nP Number of input points.
 * \param[in] points Pointer to coordinates of input points.
 * \param[in] tree Pointer to Boundary Volume Hierarchy tree that stores the geometry.
 * \param[out] ids Labels of the elements found as minimum distance element into the points are projected.
 * \param[out] ranks Ranks of the process owner of the elements into the points are projected.
 * \param[in] r Initial length of the sphere radius used to search for each input point. (The algorithm checks
 * every element encountered inside the sphere).
 * \param[out] Coordinates of the projected points.
 */
void projectPointGlobal(std::size_t nP, const std::array<double,3> *points, const bitpit::PatchSkdTree *tree, std::array<double,3> *projected_points, long *ids, int *ranks, double *r, bool shared )
{

    // Initialize ids and ranks
    for (std::size_t ip = 0; ip < nP; ip++){
        ids[ip] = bitpit::Cell::NULL_ID;
        ranks[ip] = -1;
        r[ip] = std::max(r[ip], tree->getPatch().getTol());
    }
    std::vector<darray3E>    normals(nP);

    std::vector<double> dist(nP, std::numeric_limits<double>::max());
    double max_dist = std::numeric_limits<double>::max();
    while (max_dist >= std::numeric_limits<double>::max()){
        //use method sphere by default
        signedGlobalDistance(nP, points, tree, ids, ranks, normals.data(), dist.data(), r, shared);
        for (std::size_t ip = 0; ip < nP; ip++){
            if (std::abs(dist[ip]) >= max_dist){
                r[ip] *= 1.5;
            }
        }
        max_dist = std::abs(*std::max_element(dist.begin(), dist.end()));
        max_dist = std::max(max_dist, std::abs(*std::min_element(dist.begin(), dist.end())));
    }

    for (std::size_t ip = 0; ip < nP; ip++){
        projected_points[ip] = points[ip] - dist[ip] * normals[ip];
    }
}

/*!
 * Given the specified set of points find the cells of a distributed surface patch it is into.
 * Analogous of serial function skdTreeUtils::locatePointOnPatch.
 * \param[in] nP Number of input points
 * \param[in] points is the set of points
 * \param[in] tree pointer to SkdTree relative to the target surface geometry.
 * \param[out] ids of the geometry cells the points is into. Return bitpit::Cell::NULL_ID if no cell is found.
 * \param[out] ranks Ranks of the process owner of the elements found as location of the points.
 * \param[in] shared True if the input points are shared between the processes
 */
void locatePointOnGlobalPatch(std::size_t nP, const std::array<double,3> *points, const bitpit::PatchSkdTree *tree, long *ids, int *ranks, bool shared)
{
    if(!dynamic_cast<const bitpit::SurfUnstructured*>(&(tree->getPatch()))){
        throw std::runtime_error("Invalid use of skdTreeUtils::locatePointOnPatch method: a non surface patch or void patch was detected.");
    }
    const bitpit::SurfUnstructured & spatch = static_cast<const bitpit::SurfUnstructured&>(tree->getPatch());

    // Initialize the cell id and distance
    for (std::size_t ip = 0; ip < nP; ip++){
        ids[ip] = bitpit::Cell::NULL_ID;
    }
    std::vector<double> distances(nP, std::numeric_limits<double>::max());

    // Find the closest cell to the input point
    std::vector<long> cellIds(nP, bitpit::Cell::NULL_ID);
    std::vector<int> cellRanks(nP, -1);
    if (shared){
        // All processes share the points
        findSharedPointClosestGlobalCell(nP, points, tree, cellIds.data(), ranks, distances.data());
    } else {
        // Distributed points
        static_cast<const bitpit::SurfaceSkdTree*>(tree)->findPointClosestGlobalCell(nP, points, cellIds.data(), ranks, distances.data());
    }

    int myrank = spatch.getRank();

    // Map with rank to ask info in case of distributed points
    std::map<int,std::vector<long>> ask_to_rank;
    std::map<int,std::vector<std::array<double,3>>> point_to_rank;
    std::map<int,std::vector<std::size_t>> point_index_received_from_rank;

    // Loop on points
    for (std::size_t ip = 0; ip < nP; ip++){

        const std::array<double,3> & point = points[ip];
        const long & cellId = cellIds[ip];
        const int & cellRank = ranks[ip];
        double & distance = distances[ip];

        // If the current rank is the the owner of the cell check if the point belongs to the closest cell
        if (cellRank == myrank){

            bool checkBelong = checkPointBelongsToCell(point, &spatch, cellId);

            if (checkBelong) {
                ids[ip] = cellId;
            }

        } else {

            // If not owner maintain the Cell::NULL_ID value, equal to minimum long int for bitpit::Cell

            if (!shared){

                // Push to rank map to ask normal
                ask_to_rank[cellRank].push_back(cellId);

                // Push to rank map the related points to ask normal
                point_to_rank[cellRank].push_back(point);

                // Push to rank map the index of the points to fill output normals
                point_index_received_from_rank[cellRank].push_back(ip);

            }

        } // end if owner rank

    } // End loop on points

    int nProcessors = spatch.getProcessorCount();

    if (nProcessors > 1 && spatch.isPartitioned()){
        if (shared){

            // If all points are shared between processes all reduce on ids by maximum operation
            MPI_Allreduce(MPI_IN_PLACE, ids, nP, MPI_LONG, MPI_MAX, tree->getCommunicator());

        } else {

            // Send/receive number of check belong to send to other processes
            std::vector<std::size_t> n_send_to_rank(nProcessors, 0);
            for (int irank = 0; irank < nProcessors; irank++){
                std::size_t n_ask_to_current_rank = ask_to_rank[irank].size();
                MPI_Gather(&n_ask_to_current_rank, 1, MPI_LONG, n_send_to_rank.data(), 1, MPI_LONG, irank, tree->getCommunicator());
            }

            // Map with rank to send info for cellId and point coordinates
            std::map<int,std::vector<long>> send_to_rank;
            std::map<int,std::vector<std::array<double,3>>> point_from_rank;
            for (int irank = 0; irank < nProcessors; irank++){
                send_to_rank[irank].resize(n_send_to_rank[irank], bitpit::Cell::NULL_ID);
                point_from_rank[irank].resize(n_send_to_rank[irank], std::array<double,3>({{0.,0.,0.}}));
            }

            // Recover id of the cell and points on which check the belonging
            for (int irank = 0; irank < nProcessors; irank++){
                if (myrank == irank){
                    for (int jrank = 0; jrank < nProcessors; jrank++){
                        if (jrank != irank){
                            std::size_t n_send_to_current_rank = n_send_to_rank[jrank];
                            MPI_Recv(send_to_rank[jrank].data(), n_send_to_current_rank, MPI_LONG, jrank, 100, tree->getCommunicator(), MPI_STATUS_IGNORE);
                            MPI_Recv(point_from_rank[jrank].data(), n_send_to_current_rank*3, MPI_DOUBLE, jrank, 101, tree->getCommunicator(), MPI_STATUS_IGNORE);
                        }
                    }
                } else {
                    std::size_t n_ask_to_current_rank = ask_to_rank[irank].size();
                    MPI_Send(ask_to_rank[irank].data(), n_ask_to_current_rank, MPI_LONG, irank, 100, tree->getCommunicator());
                    MPI_Send(point_to_rank[irank].data(), n_ask_to_current_rank*3, MPI_DOUBLE, irank, 101, tree->getCommunicator());
                }
            }

            // Check if the point belong to cell for each received point from each rank
            std::map<int, std::vector<int>> checkBelong_to_rank;
            for (std::size_t irank = 0; irank < nProcessors; irank++){
                checkBelong_to_rank[irank].resize(n_send_to_rank[irank], false);
                for (std::size_t ip = 0; ip < n_send_to_rank[irank]; ip++){

                    const std::array<double,3> & point = point_from_rank[irank][ip];
                    const long & cellId = send_to_rank[irank][ip];
                    int & checkBelong = checkBelong_to_rank[irank][ip];

                    checkBelong = int(checkPointBelongsToCell(point, &spatch, cellId));

                } // end loop on points received
            }  // end loop on ranks

            // Communicate back the computed cehck belong
            std::map<int, std::vector<int>> checkBelong_from_rank;

            for (int irank = 0; irank < nProcessors; irank++){
                if (myrank == irank){
                    for (int jrank = 0; jrank < nProcessors; jrank++){
                        if (jrank != irank){
                            std::size_t n_ask_to_current_rank = ask_to_rank[jrank].size();
                            checkBelong_from_rank[jrank].resize(n_ask_to_current_rank);
                            MPI_Recv(checkBelong_from_rank[jrank].data(), n_ask_to_current_rank, MPI_INT, jrank, 102, tree->getCommunicator(), MPI_STATUS_IGNORE);

                            // Fill cell ids output if the received check belong is true (TODO move out of communications scope?)
                            for (std::size_t ip = 0; ip < n_ask_to_current_rank; ip++){
                                std::size_t point_index = point_index_received_from_rank[jrank][ip];
                                long cellId = ask_to_rank[jrank][ip];
                                // Check received int value (zero or one) casting to boolean
                                if (bool(checkBelong_from_rank[jrank][ip])) {
                                    ids[point_index] = cellId;
                                }
                            }

                        }
                    }
                } else {
                    std::size_t n_send_to_current_rank = n_send_to_rank[irank];
                    MPI_Send(checkBelong_to_rank[irank].data(), n_send_to_current_rank, MPI_INT, irank, 102, tree->getCommunicator());
                }
            }

        } // end if not shared points
    }
}

/*!
 * It selects the elements of a distributed geometry stored in a skd-tree by a distance criterion
 * in respect to an other distributed geometry stored in a different global skd-tree.
 * Analogous of serial function skdTreeUtils::selectByPatch.
 * \param[in] selection Pointer to skd-tree used as selection patch.
 * \param[in] target Pointer to skd-tree that store the target geometry.
 * \param[in] tol Distance threshold used to select the elements of target.
 * \return Vector of the label of all the local (current rank) elements of the target skd-tree placed
 * at a distance <= tol from the bounding boxes of the leaf nodes of the global skd-tree
 * selection.
 */
std::vector<long> selectByGlobalPatch(bitpit::PatchSkdTree *selection, bitpit::PatchSkdTree *target, double tol){

    if (!selection->isCommunicatorSet() || !target->isCommunicatorSet()){
        throw std::runtime_error("Error: at least one PatchSkdTree communicator not set in selectByGlobalPatch");
    }
    if (!selection->getPatch().isPartitioned() && !target->getPatch().isPartitioned()){
        return(selectByPatch(selection, target, tol));
    }

    // Leaf selection nodes of current rank initialized for each rank of target bounding box
    int nprocs = selection->getPatch().getProcessorCount();
    int myRank = selection->getPatch().getRank();
    std::size_t nleafs = selection->getLeafCount();
    std::size_t nnodess = selection->getNodeCount();
    std::unordered_map<int, std::vector<const bitpit::SkdNode*>> rankLeafSelection;
    for (int irank = 0; irank < nprocs; irank++){
        std::vector<const bitpit::SkdNode*> leafSelection(nleafs);
        int count = 0;
        for (int i=0; i<nnodess; i++){
            if (selection->getNode(i).isLeaf()){
                if (bitpit::CGElem::intersectBoxBox(selection->getNode(i).getBoxMin()-tol,
                        selection->getNode(i).getBoxMax()+tol,
                        target->getPartitionBoxMin(irank),
                        target->getPartitionBoxMax(irank) ) ){
                    leafSelection[count] = &(selection->getNode(i));
                    count++;
                }
            }
        }
        leafSelection.resize(count);
        rankLeafSelection[irank].swap(leafSelection);
    }

    // Collect all the leaf selection SkdBox bounding boxes from each process in the map structure
    // Collect and store from other processes the leaf selection bounding boxes for the current target rank
    std::vector<bitpit::SkdBox> leafSelectionBoxes;

    // Recover the number of selection elements of each process for each target process
    // This is the number of elements sent and received to/from each process from/to each process
    std::vector<int> nLeafSend(nprocs, 0);
    std::vector<int> nLeafRecv(nprocs, 0);
    for (int irank = 0; irank < nprocs; irank++){
        nLeafSend[irank] = rankLeafSelection[irank].size();
    }

    for (int irank = 0; irank < nprocs; irank++){
        // Scatter send to all process of irank process data
        int * recvbuff = &nLeafRecv[irank];
        MPI_Scatter(nLeafSend.data(), 1, MPI_INT, recvbuff, 1, MPI_INT, irank, selection->getCommunicator());
    }

    // Recover global leaf received to resize local received boxes container
    int nGlobalReceived = 0;
    for (int val : nLeafRecv){
        nGlobalReceived += val;
    }

    if (nGlobalReceived > 0){
        leafSelectionBoxes.reserve(nGlobalReceived);
    }

    // Communicate minimum and maximum point of each leaf bounding box to the right process
    // Gather bounding boxes for each process by communicating minimum and maximum point
    // coordinates as vector of 6 coordinates for each box
    std::vector<double> recvBoxes;
    for (int irank = 0; irank < nprocs; irank++){
        int nSendData = 0;
        std::vector<double> sendBoxes;
        int nRecvData = 0;
        std::vector<int> recvDispls(nprocs, 0);
        std::vector<int> nRecvDataPerProc(nprocs, 0);
        if (irank == myRank){
            // If root is current rank set receive buffer
            nRecvData = 6 * nGlobalReceived;
            recvBoxes.resize(nRecvData, std::numeric_limits<double>::max());
            // Set receiving displacements to gather bounding boxes
            nRecvDataPerProc[0] = nLeafRecv[0] * 6;
            for (int jrank = 1; jrank < nprocs; jrank++){
                recvDispls[jrank] = recvDispls[jrank - 1] + nLeafRecv[jrank - 1] * 6;
                nRecvDataPerProc[jrank] = nLeafRecv[jrank] * 6;
            }
        }
        // Set send buffer for all processes even for root
        nSendData = 6 * nLeafSend[irank];
        sendBoxes.reserve(nSendData);
        for (const bitpit::SkdNode* selectNode : rankLeafSelection[irank]){
            for (const double val : selectNode->getBoxMin()){
                sendBoxes.push_back(val);
            }
            for (const double val : selectNode->getBoxMax()){
                sendBoxes.push_back(val);
            }
        }

        // Gather bounding boxes points to the root process
        MPI_Gatherv(sendBoxes.data(), nSendData, MPI_DOUBLE, recvBoxes.data(), nRecvDataPerProc.data(), recvDispls.data(), MPI_DOUBLE, irank, selection->getCommunicator());

    } // End loop on root processes

    // Fill leaf selection boxes container
    std::vector<double>::iterator itBox = recvBoxes.begin();
    for (std::size_t ibox = 0; ibox < nGlobalReceived; ibox++){
        std::array<double, 3> minPoint;
        minPoint[0] = *(itBox++);
        minPoint[1] = *(itBox++);
        minPoint[2] = *(itBox++);
        std::array<double, 3> maxPoint;
        maxPoint[0] = *(itBox++);
        maxPoint[1] = *(itBox++);
        maxPoint[2] = *(itBox++);
        leafSelectionBoxes.emplace_back(bitpit::SkdBox(minPoint, maxPoint));
    }

    std::vector<long> extracted;
    extractTarget(target, leafSelectionBoxes, extracted, tol);

    return extracted;

}

/*!
 * It extracts the elements of a leaf node of a distributed geometry stored in a skd-tree
 * by a distance criterion in respect to an other distributed geometry stored
 * in a different skd-tree and passed as vector of bounding boxes.
 * It is a method used in skdTreeUtils::selectByGlobalPatch method.
 * \param[in] target Pointer to skd-tree that store the target geometry.
 * \param[in] leafSelection Vector of bounding boxes SkdBox of the leaf nodes currently interesting
 * for the selection procedure.
 * \param[in,out] extracted of the label of all the elements of the target skd-tree,
 * currently found placed at a distance <= tol from the bounding boxes of the
 * leaf nodes in leafSelection.
 * \param[in] tol Distance threshold used to select the elements of target.
 * the next-th node is not a leaf node the method is recursively called.
 */
void extractTarget(bitpit::PatchSkdTree *target, const std::vector<bitpit::SkdBox> &leafSelectionBoxes, std::vector<long> &extracted, double tol)
{

    if(leafSelectionBoxes.empty()) return;
    std::size_t rootId = 0;

    // Candidates saved in set to avoid duplicate
    std::unordered_set<std::size_t> candidates;

    // Loop on leaf selection boxes and go across the target tree with a node stack
    for (const bitpit::SkdBox & selectionBox : leafSelectionBoxes){

        // Initialize stack with target root node
        std::queue<std::size_t> nodeStack;
        nodeStack.push(rootId);

        // Pop and push target tree nodes checked with current selection boxe
        // Fill candidates structure with leaf target nodes intersected by selection box
        // Stop when stack is empty
        while(!nodeStack.empty()){

            std::size_t nodeId = nodeStack.front();
            nodeStack.pop();
            const bitpit::SkdNode & node = target->getNode(nodeId);

            bool isLeaf = true;
            for (int i = bitpit::SkdNode::CHILD_BEGIN; i != bitpit::SkdNode::CHILD_END; ++i) {
                bitpit::SkdNode::ChildLocation childLocation = static_cast<bitpit::SkdNode::ChildLocation>(i);
                std::size_t childId = node.getChildId(childLocation);
                if (childId != bitpit::SkdNode::NULL_ID) {
                    isLeaf = false;
                    if (bitpit::CGElem::intersectBoxBox(selectionBox.getBoxMin()-tol, selectionBox.getBoxMax()+tol,
                                target->getNode(childId).getBoxMin(), target->getNode(childId).getBoxMax())){
                        nodeStack.push(childId);
                    }
                }
            }
            if (isLeaf) {
                candidates.insert(nodeId);
            }
        }
    } // End loop on selection boxes

    // Extract cell from candidates nodes
    // Do not check for intersection -> exctraction performed only by using bounding boxes of leaf nodes of the skd-trees
    std::unordered_set<long> cellExtracted;
    for(std::size_t nodeId : candidates){
        const bitpit::SkdNode & node = target->getNode(nodeId);
        std::vector<long> cellids = node.getCells();
        cellExtracted.insert(cellids.begin(), cellids.end());
    }

    extracted.insert(extracted.end(), cellExtracted.begin(), cellExtracted.end());
}


/*!
* Given the specified set of points, considered shared on the processes, find the
* closest cells contained in a distributed skd-tree and evaluates the distance values
* between those cells and the given points.
* \param[in] nPoints number of the points
* \param[in] points points coordinates
* \param[in] tree pointer to SkdTree relative to the target surface geometry.
* \param[in] r all cells whose distance is greater than
* this parameters will not be considered for the evaluation of the
* distance
* \param[out] ids on output it will contain the ids of the cells closest
* to the points. If all cells contained in the tree are farther from a point
* than the maximum distance, the related id will be set to the null id
* \param[out] ranks on output it will contain the rank indices of the processes
* owner of the cells closest to the points
* \param[out] distances on output it will contain the distances
* between the points and closest cells. If all cells contained in the tree are
* farther than the maximum distance, the related argument will be set to the
* maximum representable distance.
*/
void findSharedPointClosestGlobalCell(std::size_t nPoints, const std::array<double, 3> *points, const bitpit::PatchSkdTree *tree,
        long *ids, int *ranks, double *distances, double r)
{
    std::vector<double> rs(nPoints, r);
    findSharedPointClosestGlobalCell(nPoints, points, tree, ids, ranks, distances, rs.data());
}

/*!
* Given the specified set of points, considered shared on the processes, find the
* closest cells contained in a distributed skd-tree and evaluates the distance values
* between those cells and the given points.
* \param[in] nPoints number of the points
* \param[in] points points coordinates
* \param[in] tree pointer to SkdTree relative to the target surface geometry.
* \param[in] r all cells whose distance is greater than
* this parameters, one for each input point, will not be considered
* for the evaluation of the distance
* \param[out] ids on output it will contain the ids of the cells closest
* to the points. If all cells contained in the tree are farther from a point
* than the maximum distance, the related id will be set to the null id
* \param[out] ranks on output it will contain the rank indices of the processes
* owner of the cells closest to the points
* \param[out] distances on output it will contain the distances
* between the points and closest cells. If all cells contained in the tree are
* farther than the maximum distance, the related argument will be set to the
* maximum representable distance.
*/
void findSharedPointClosestGlobalCell(std::size_t nPoints, const std::array<double, 3> *points, const bitpit::PatchSkdTree *tree,
        long *ids, int *ranks, double *distances, double *r)
{
    // Initialize the cell ids and ranks
    for (std::size_t i = 0; i < nPoints; i++){
        ids[i] = bitpit::Cell::NULL_ID;
        ranks[i] = -1;
    }

    // Initialize rank and number of processes
    int myRank = tree->getPatch().getRank();
    int nProcs = tree->getPatch().getProcessorCount();

    // Call local find point closest cell for each global point collected

    // Instantiate global container for distances, ids and ranks (SkdCellInfo)
    std::vector<bitpit::PatchSkdTree::SkdCellInfo> dri_data(nPoints, bitpit::PatchSkdTree::SkdCellInfo(std::numeric_limits<double>::max(), myRank, bitpit::Cell::NULL_ID));

    // Call local find point closest cell for each global point collected
    for (std::size_t ip = 0; ip < nPoints; ip++){

        const std::array<double,3> & point = points[ip];
        double & distance = dri_data[ip].distance;
        int & rank = dri_data[ip].rank;
        long & id = dri_data[ip].id;

        // Use a maximum distance for each point given by an estimation based on partition
        // bounding boxes. The distance will be lesser than or equal to the point maximum distance
        double pointMaxDistance = r[ip];
        for (int irank = 0; irank < nProcs; irank++){
            pointMaxDistance = std::min(tree->getPartitionBox(irank).evalPointMaxDistance(point), pointMaxDistance);
        }

        // Call local find point closest cell with estimated distance used as maximum distance
        long nDistanceEvaluations = tree->findPointClosestCell(point, pointMaxDistance, &id, &distance);

    }

    // Force distance to numeric limits maximum value for point projected on ghost cells
    // The desired result id is the local cell id on the ghost owner rank
    for (std::size_t ip = 0; ip < nPoints; ip++){
        long cellId = dri_data[ip].id;
        if(cellId != bitpit::Cell::NULL_ID && !tree->getPatch().getCell(cellId).isInterior()) {
            dri_data[ip].id = tree->getGhostCellsRemoteIds().at(cellId);
            dri_data[ip].rank = tree->getPatch().getCellRank(cellId);
        }
    }

    // Communicate the computed distances of the distributed input points to all processes
    // and retain the indices of the rank owner and the id of the closest cell

    // Prepare MPI custom data type and Operation
    // The data are of MPI custom data type  with distance, ids and rank as member
    int blocklengths[3] = {1,1,1};
    MPI_Aint displacements[3] = {offsetof(bitpit::PatchSkdTree::SkdCellInfo, distance),
            offsetof(bitpit::PatchSkdTree::SkdCellInfo, rank),
            offsetof(bitpit::PatchSkdTree::SkdCellInfo, id)};
    MPI_Datatype types[3] = {MPI_DOUBLE, MPI_INT, MPI_LONG};
    MPI_Datatype MPI_DRI;
    MPI_Type_create_struct(3, blocklengths, displacements, types, &MPI_DRI);
    MPI_Type_commit(&MPI_DRI);

    MPI_Op MPI_MIN_DRI;
    MPI_Op_create((MPI_User_function *) bitpit::minSkdCellInfo, false, &MPI_MIN_DRI);

    // Communicate the closest cells distances, ranks and ids by reduce with custom minimum distance operation
    // Reduce only the right portion of data to the right process
    for (int irank = 0; irank < nProcs; irank++){
        MPI_Allreduce(MPI_IN_PLACE, dri_data.data(), nPoints, MPI_DRI, MPI_MIN_DRI, tree->getCommunicator());
    }

    // Update distances, rank indices and cell ids
    for (std::size_t ip = 0; ip < nPoints; ip++){
        distances[ip] = dri_data[ip].distance;
        ranks[ip] = dri_data[ip].rank;
        ids[ip] = dri_data[ip].id;
    }
}

#endif

/*!
* Given the specified point find the closest cell contained in a
* skd-tree and evaluates the distance between that cell and the given
* point.
* \param[in] point is the point
* \param[in] tree pointer to SkdTree relative to the target volume geometry.
* \param[out] id on output it will contain the id of the cell closest
* to the point, if the distance between the point and the surface is
* greater than the specified maximum distance, the id parameter will
* be set to the null id
* \param[in,out] distance on output it will contain the distance
* between the point and closest cell. If all cells contained in the
* tree are farther than the maximum distance, the function will return
* the maximum representable distance.
*/
long findPointClosestCell(const std::array<double, 3> &point, const bitpit::VolumeSkdTree *tree, long *id, double *distance)
{
    return findPointClosestCell(point, tree, std::numeric_limits<double>::max(), id, distance);

}
/*!
* Given the specified point find the closest cell contained in a
* skd-tree and evaluates the distance between that cell and the given
* point.
* \param[in] point is the point
* \param[in] tree pointer to SkdTree relative to the target volume geometry.
* \param[in] maxDistance all cells whose distance is greater than
* this parameters will not be considered for the evaluation of the
* distance
* \param[out] id on output it will contain the id of the closest cell.
* If all cells contained in the tree are farther than the maximum
* distance, the argument will be set to the null id
* \param[out] distance on output it will contain the distance between
* the point and closest cell. If all cells contained in the tree are
* farther than the maximum distance, the argument will be set to the
* maximum representable distance
*/
long findPointClosestCell(const std::array<double, 3> &point, const bitpit::VolumeSkdTree *tree,
        double maxDistance, long *id, double *distance)
{

    // Initialize the cell id
    *id = bitpit::Cell::NULL_ID;

    // Initialize the distance with an estimate
    //
    // The real distance will be lesser than or equal to the estimate.
    std::size_t rootId = 0;
    const bitpit::SkdNode &root = tree->getNode(rootId);
    *distance = std::min(root.evalPointMaxDistance(point), maxDistance);

    // Get a list of candidates nodes
    //
    // Initialize list of candidates
    std::vector<std::size_t> candidateIds;
    std::vector<double> candidateMinDistances;

    std::vector<std::size_t> nodeStack;
    nodeStack.push_back(rootId);
    while (!nodeStack.empty()) {
        std::size_t nodeId = nodeStack.back();
        const bitpit::SkdNode &node = tree->getNode(nodeId);
        nodeStack.pop_back();

        // Do not consider nodes with a minimum distance greater than
        // the distance estimate
        double nodeMinDistance = node.evalPointMinDistance(point);
        if (nodeMinDistance > *distance) {
            continue;
        }

        // Update the distance estimate
        //
        // The real distance will be lesser than or equal to the
        // estimate.
        double nodeMaxDistance = node.evalPointMaxDistance(point);
        *distance = std::min(nodeMaxDistance, *distance);

        // If the node is a leaf add it to the candidates, otherwise
        // add its children to the stack.
        bool isLeaf = true;
        for (int i = bitpit::SkdNode::CHILD_BEGIN; i != bitpit::SkdNode::CHILD_END; ++i) {
            bitpit::SkdNode::ChildLocation childLocation = static_cast<bitpit::SkdNode::ChildLocation>(i);
            std::size_t childId = node.getChildId(childLocation);
            if (childId != bitpit::SkdNode::NULL_ID) {
                isLeaf = false;
                nodeStack.push_back(childId);
            }
        }

        if (isLeaf) {
            candidateIds.push_back(nodeId);
            candidateMinDistances.push_back(nodeMinDistance);
        }
    }

    // Process the candidates and find the closest cell
    long nDistanceEvaluations = 0;
    for (std::size_t k = 0; k < candidateIds.size(); ++k) {
        // Do not consider nodes with a minimum distance greater than
        // the distance estimate
        if (candidateMinDistances[k] > *distance) {
            continue;
        }

        // Evaluate the distance
        std::size_t nodeId = candidateIds[k];
        const bitpit::SkdNode &node = tree->getNode(nodeId);

        node.updatePointClosestCell(point, id, distance);
        ++nDistanceEvaluations;
    }

    // If no closest cell was found set the distance to the maximum
    // representable distance.
    if (*id == bitpit::Cell::NULL_ID) {
        *distance = std::numeric_limits<double>::max();
    }

    return nDistanceEvaluations;
}

#if MIMMO_ENABLE_MPI
/*!
* Given the specified points, considered distributed on the processes, find the
* closest cells contained in a distributed skd-tree and evaluates the distance values
* between those cells and the given points.
* \param[in] nPoints number of the points
* \param[in] points points coordinates
* \param[in] tree pointer to SkdTree relative to the target volume geometry.
* \param[out] ids on output it will contain the ids of the cells closest
* to the local points
* \param[out] ranks on output it will contain the rank indices of the processes
* owner of the cells closest to the points
* \param[out] distances on output it will contain the distances
* between the points and closest cells
*/
void findPointClosestGlobalCell(const std::size_t nPoints, const std::array<double, 3> *points,
        const bitpit::VolumeSkdTree *tree, long *ids, int *ranks, double *distances)
{
    std::vector<double> maxDistances(nPoints, std::numeric_limits<double>::max());

    // Patch is partitioned call the parallel method
    if (!tree->getPatch().isCommunicatorSet()){
        throw std::runtime_error ("There is no communicator set for the patch.");
    }

    int nprocs = tree->getPatch().getProcessorCount();
    int myRank = tree->getPatch().getRank();

    // Initialize the cell ids and ranks
    for (std::size_t i = 0; i < nPoints; i++){
        ids[i] = bitpit::Cell::NULL_ID;
        ranks[i] = -1;
    }

    // Communicate all the points to all the processes
    // Gather number of points multiplied by number of coordinates,
    // i.e. number of data to communicate
    std::vector<int> pointsCount(nprocs);
    MPI_Allgather(&nPoints, 1, MPI_INT, pointsCount.data(), 1, MPI_INT, tree->getCommunicator());

    // Recover points displacements, offset and number of points data per process
    std::vector<int> pointsDispls(nprocs, 0);
    std::vector<int> pointsOffsets(nprocs, 0);
    std::vector<int> pointsCountData(nprocs, 0);
    pointsCountData[0] = pointsCount[0] * 3;
    for (int i = 1; i < nprocs; ++i) {
        pointsDispls[i] = pointsDispls[i - 1] + pointsCount[i - 1] * 3;
        pointsOffsets[i] = pointsOffsets[i - 1] + pointsCount[i - 1];
        pointsCountData[i] = pointsCount[i] * 3;
    }

    // Sum to global number of points
    int nGlobalPoints = 0;
    for (int np : pointsCount){
        nGlobalPoints += np;
    }

    // Gather vector with all global points
    std::vector<std::array<double,3>> globalPoints(nGlobalPoints);
    std::size_t nPointsCountData = nPoints * 3;
    MPI_Allgatherv(points, nPointsCountData, MPI_DOUBLE, globalPoints.data(),
            pointsCountData.data(), pointsDispls.data(), MPI_DOUBLE, tree->getCommunicator());

    // Gather vector with all maximum distances
    std::vector<double> globalMaxDistances(nGlobalPoints);
    MPI_Allgatherv(maxDistances.data(), nPoints, MPI_DOUBLE, globalMaxDistances.data(),
            pointsCount.data(), pointsOffsets.data(), MPI_DOUBLE, tree->getCommunicator());

    // Call local find point closest cell for each global point collected

    // Instantiate global container for distances, ids and ranks (SkdCellInfo)
    std::vector<bitpit::PatchSkdTree::SkdCellInfo> dri_data(nGlobalPoints, bitpit::PatchSkdTree::SkdCellInfo(std::numeric_limits<double>::max(), myRank, bitpit::Cell::NULL_ID));

    // Call local find point closest cell for each global point collected
    for (std::size_t ip = 0; ip < nGlobalPoints; ip++){

        const std::array<double,3> & point = globalPoints[ip];
        double & distance = dri_data[ip].distance;
        int & rank = dri_data[ip].rank;
        long & id = dri_data[ip].id;

        // Use a maximum distance for each point given by an estimation based on partition
        // bounding boxes. The distance will be lesser than or equal to the point maximum distance
        double pointMaxDistance = globalMaxDistances[ip];
        for (int irank = 0; irank < nprocs; irank++){
            pointMaxDistance = std::min(tree->getPartitionBox(irank).evalPointMaxDistance(point), pointMaxDistance);
        }

        // Call local find point closest cell with estimated distance used as maximum distance
        long nDistanceEvaluations = findPointClosestCell(point, tree, pointMaxDistance, &id, &distance);

    }

    // Force distance to numeric limits maximum value for point projected on ghost cells
    // The desired result id is the local cell id on the ghost owner rank
    for (std::size_t ip = 0; ip < nGlobalPoints; ip++){
        long cellId = dri_data[ip].id;
        if(cellId != bitpit::Cell::NULL_ID && !tree->getPatch().getCell(cellId).isInterior()) {
            dri_data[ip].id = tree->getGhostCellsRemoteIds().at(cellId);
            dri_data[ip].rank = tree->getPatch().getCellRank(cellId);
        }
    }

    // Communicate the computed distances of the distributed input points to all processes
    // and retain the indices of the rank owner and the id of the closest cell

    // Prepare MPI custom data type and Operation
    // The data are of MPI custom data type  with distance, ids and rank as member
    int blocklengths[3] = {1,1,1};
    MPI_Aint displacements[3] = {offsetof(bitpit::PatchSkdTree::SkdCellInfo, distance),
            offsetof(bitpit::PatchSkdTree::SkdCellInfo, rank),
            offsetof(bitpit::PatchSkdTree::SkdCellInfo, id)};
    MPI_Datatype types[3] = {MPI_DOUBLE, MPI_INT, MPI_LONG};
    MPI_Datatype MPI_DRI;
    MPI_Type_create_struct(3, blocklengths, displacements, types, &MPI_DRI);
    MPI_Type_commit(&MPI_DRI);

    MPI_Op MPI_MIN_DRI;
    MPI_Op_create((MPI_User_function *) bitpit::minSkdCellInfo, false, &MPI_MIN_DRI);

    // Communicate the closest cells distances, ranks and ids by reduce with custom minimum distance operation
    // Reduce only the right portion of data to the right process
    for (int irank = 0; irank < nprocs; irank++){
        bitpit::PatchSkdTree::SkdCellInfo *buffer = (dri_data.data() + pointsOffsets[irank]);
        if (myRank == irank){
            MPI_Reduce(MPI_IN_PLACE, buffer, pointsCount[irank], MPI_DRI, MPI_MIN_DRI, irank, tree->getCommunicator());
        } else {
            MPI_Reduce(buffer, buffer, pointsCount[irank], MPI_DRI, MPI_MIN_DRI, irank, tree->getCommunicator());
        }
    }

    // Update distances, rank indices and cell ids
    for (std::size_t ip = 0; ip < nPoints; ip++){
        std::size_t globalIndex = ip + pointsOffsets[myRank];
        distances[ip] = dri_data[globalIndex].distance;
        ranks[ip] = dri_data[globalIndex].rank;
        ids[ip] = dri_data[globalIndex].id;
    }

}
#endif
}

}; // end namespace mimmo
