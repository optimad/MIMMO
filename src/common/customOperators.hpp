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

#ifndef __CUSTOMOPERATORS_HH__
#define __CUSTOMOPERATORS_HH__

#include <cmath>
#include <vector>
#include <array>
#include <algorithm>

#include "mimmoTypeDef.hpp"
#include <bitpit_patchkernel.hpp>



std::array<double, 3> operator-(const bitpit::Vertex &v1, const bitpit::Vertex &v2);

/*!
 * \ingroup common_Utils
 * \{
 */

int countSubstring(const std::string& str, const std::string& sub);


/*!
* Template for freeing any std::vector container, by swap
*  \param[in] t std::vector structure to be freed.
*/
template <class T>
void freeContainer (std::vector<T> & t){
        t.swap(std::vector<T>());
}

/*!
* Template for summing elements of any std::vector container, provided that
*  sum operators is defined for your element class in std library
*  \param[in] t std::vector structure to summed.
*/
template <class T>
T summing_Vector(std::vector<T> & t){
    T result = t[0];
    std::size_t size = t.size();
    for(int i=1; i<size; ++i){
        result += t[i];
    }
    return  result;
}

/*!
* Logical summing of given booleans, according to a given policy.
* The policy array is an array of 3 booleans, initialized to false.
*  -  policy[0] = true -> function return the result of target.
*  -  policy[1] = true -> function return the boolean sum of root and target.
*  -  policy[2] = true -> function return the boolean difference of root and target.
*
* An exception is made if root is FALSE and target is TRUE; the boolean difference
* here is handled as FALSE.
* If more than one TRUE is defined in the policy array, the policy with high index
* in the array is applied.
*
* \param[in] root first boolean
* \param[in] target second boolean
* \param[in] policy pointer to a boolean C-array of 3 Elements.
* \return boolean true/false
*
*/
bool inline logical_summing(bool &root, bool & target, bool * policy){
    bool result;
    if(policy[0]) result = target;
    if(policy[1]) result = root + target;

    if(policy[2]) {
        result = root - target;
        //further check if root is 0 and target is 1.
        // -1 is read as 1; I want to keep him as 0;
        if(result) result = result - target;
    }
    return result;
};

/*!
* Logical summing of given booleans, according to a given policy.
* The policy array is an array of 3 booleans, initialized to false.
*  -  policy[0] = true -> function return the result of target.
*  -  policy[1] = true -> function return the boolean sum of root and target.
*  -  policy[2] = true -> function return the boolean difference of root and target.
*
* An exception is made if root is FALSE and target is TRUE; the boolean difference
* here is handled as FALSE.
* If more than one TRUE is defined in the policy array, the policy with high index
* in the array is applied.
*
* \param[in] root first boolean
* \param[in] target second boolean
* \param[in] policy pointer to a boolean C-array of 3 Elements.
* \return integer 1/0 for true/false
*
*/
int inline logical_summing(int &root, int &target, bool policy){
    bool result;
    if(policy){
        result = (bool)root + (bool)target;
    }else{
        result = (bool)root - (bool)target;
        //further check if root is 0 and target is 1.
        // -1 is read as 1; I want to keep him as 0;
        if(result) result = result - (bool)target;
    }
    return  (int)result;
};

/*!
* Get sign of a scalar element of Class T
* \param[in] t element
* \return return sign of the element
*/
template <class T>
T getSign (T & t){
    T result = 1;
    if(t < 0) result= -1 * result;
    return  result;
}

/*!
* Get index position of a target element in a std::vector list.
* If the element is not found, return -1.
* \param[in] vect std::vector list
* \param[in] target target element
* \return position of the element in the list
*/
template <class T>
int  inline posVectorFind( std::vector<T>	&vect,	T& target){
    int pos = -1;
    typename std::vector< T >::iterator it_find = std::find(vect.begin(),vect.end(),target);
    if (it_find!=vect.end())  {
        pos = std::distance(vect.begin(),it_find);
    }

return pos;
};

/*!
* Get index position of a target element in an std::array list.
* If the element is not found, return -1.
* \param[in] vect std::array list
* \param[in] target target element
* \return position of the element in the list
*/
template <class T, size_t d >
int  inline posVectorFind( std::array<T, d> &vect, T& target){
    int pos = -1;
    typename std::array< T,d >::iterator it_find = std::find(vect.begin(),vect.end(),target);
    if (it_find!=vect.end()){
        pos = std::distance(vect.begin(),it_find);
    }
    return pos;
};

/*!
* Check if a target element belong to a std::vector list.
* If the element is not found, return false.
* \param[in] vect std::vector list
* \param[in] target target element
* \return true if the element belongs to the list
*/
template <class T>
bool inline checkVectorFind( std::vector<T> &vect, T& target){
    bool check = false;
    typename std::vector< T >::iterator it_find = std::find(vect.begin(),vect.end(),target);
    if (it_find!=vect.end())    check=true;
    return check;
};

/*!
* Check if a target element belong to a std::array list.
* If the element is not found, return false.
* \param[in] vect std::array list
* \param[in] target target element
* \return true if the element belongs to the list
*/
template <class T, size_t d>
bool inline checkVectorFind( std::array<T,d> &vect, T& target){
    bool check = false;
    typename std::array< T,d >::iterator it_find = std::find(vect.begin(),vect.end(),target);
    if (it_find!=vect.end()) check=true;
    return check;
};

/*!
* Converter from array to vector
* \param[in] origin std::array
* \return std::vector result
*/
template <class T, size_t d>
std::vector<T> inline conVect(std::array<T, d> & origin ){
    std::vector<T> result(d);
    for(size_t i=0; i<d; ++i)
        result[i] = origin[i];
    return  result;
}

/*!
* Converter from vector to array
* \param[in] origin std::vector
* \return std::array result
*/
template <class T, size_t d>
std::array<T,d> inline conArray(std::vector<T> & origin ){
    std::array<T,d> result;
    for(size_t i=0; i<d; ++i)
        result[i] = origin[i];
    return  result;
}

/*!
* Converter from vector of arrays to vector of vectors
* \param[in] origin std::vector of std::array's
* \return std::vector of std::vector's
*/
template <class T, size_t d>
std::vector< std::vector < T > > inline conVect(std::vector< std::array<T, d> > & origin ){
    std::vector<std::vector<T> > result(origin.size());
    for(size_t i=0; i<origin.size(); ++i)
        result[i] = conVect(origin[i]);
    return  result;
}

/*!
* Converter from vector of vectors to vector of arrays
* \param[in] origin std::vector of std::vector's
* \return std::vector of std::array's
*/
template <class T, size_t d>
std::vector< std::array< T, d > > inline conArray(std::vector<std::vector<T> > & origin ){
    std::vector< std::array<T,d> > result(origin.size());
    for(size_t i=0; i<origin.size(); ++i)
        result[i] = conArray< T, d>(origin[i]);
    return  result;
}

/*!
* Find position of a target T value in a vector source of T elements.
* Returns -1 if value is not found.
* \param[in] value target
* \param[in] source vector source
* \return  position value
*/
template <class T>
T findPosition(T & value, std::vector<T> & source){
    T result = -1;
    typename std::vector< T >::iterator itV = std::find(source.begin(), source.end(), value);
    if(itV != source.end())
        result = std::distance(source.begin(), itV);
    return  result;
};


/*!
* Get a subset of a std::vector structure containing all source elements included between
* two consecutive local positions i and j, with j-th element excluded.
* According to i and j values, the following behaviours occurr:
*
* - i < 0, j < 0                            ==> fill nothing;
* - i >= source.size(), j >= source.size()  ==> fill nothing;
* - i < 0, j >= source.size()               ==> fill whole source;
* - i >= source.size(), j < 0               ==> fill nothing;
* - i = j                                   ==> fill whole source;
* - i < j                                   ==> fill subset from i-th element up to j-th element excluded
* - j > i                                   ==> source looped, fill subset from ith -> up to vector end + vector begin -> up to jth element
*
* \param[in] i start position
* \param[in] j end position
* \param[in] source std::vector< > structure
* \return vector subset
*/
template <class T>
std::vector< T > getVectorSubset(int i, int j, std::vector<T> & source){
    std::vector < T> result;
    int counter = 0;
    if(i >= (int)source.size() || j < 0)    return  result;

    result.resize(source.size());

    // check your controlling indices
    int maxElement = source.size() -1;
    i = std::max(std::min(i,maxElement), 0);
    j = std::max(std::min(j,maxElement), 0);

    if(i>=j){
        for(int k=i; k<source.size(); ++k ){
            result[counter] = source[k];
            ++counter;
        }
        for(int k=0; k<j; ++k ){
            result[counter] = source[k];
            ++counter;
        }
    }else{
        for(int k=i; k<j; ++k ){
            result[counter] = source[k];
            ++counter;
        }
    }
    result.resize(counter);
    return  result;
}

/*!
* Fill std::vector structure subset containing all source elements included between
* two consecutive local positions i and j, with j-th element excluded. With a custom value.
* According to i and j values, the following behaviours occurr:
*
* - i < 0, j < 0                            ==> return empty subset;
* - i >= source.size(), j >= source.size()  ==> return empty subset;
* - i < 0, j >= source.size()               ==> return whole source;
* - i >= source.size(), j < 0               ==> return empty subset;
* - i = j                                   ==> return whole source, reordered starting from i-th element;
* - i < j                                   ==> return subset from i-th element up to j-th element excluded
* - j > i                                   ==> source looped, return subset with ith -> up to vector end + vector begin -> up to jth element
*
* \param[in] i start position
* \param[in] j end position
* \param[in,out] source std::vector< > structure
* \param[in] cValue custom value
*/
template <class T>
void fillVectorSubset(int i, int j, std::vector<T> & source, T cValue){
    if(i >= source.size() || j < 0) return;

    // check your controlling indices
    int maxElement = source.size() -1;
    i = std::max(std::min(i,maxElement), 0);
    j = std::max(std::min(j,maxElement), 0);

    if(i>=j){
        for(int k=i; k<source.size(); ++k ){
            source[k] = cValue;
        }
        for(int k=0; k<j; ++k ){
            source[k] = cValue;
        }
    }else{
        for(int k=i; k<j; ++k ){
            source[k] = cValue;
        }
    }
    return;
}

/*!
 * \}
 */


#endif //__CUSTOMOPERATORS_HH__
