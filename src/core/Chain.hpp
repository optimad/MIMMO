/*---------------------------------------------------------------------------*\
 *
 *  mimmo
 *
 *  Copyright (C) 2015-2021OPTIMAD engineering Srl
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
#ifndef __CHAIN_HPP__
#define __CHAIN_HPP__

#include "BaseManipulation.hpp"
#include <memory>

namespace mimmo{

/*!
 * \class Chain
 * \ingroup core
 * \brief Chain is the class used to manage the chain execution of multiple executable blocks (manipulation object).
 *
 * Chain is a wrapper, executable container capable to manage the workflow execution of multiple
 * manipulation objects, connected each other through port PINs.
 * Each object added to the chain is inserted in the chain. According to the mutual connections with other object,
 * the class will automatically assign to it a priority of execution.
 * An ID is assigned to a new chain and to every object in the chain, in order to give to the user a persistent
 * information to recover or the position or the ID of the object in the chain.
 * The execution is performed following the correct order of the object chain in order to avoid
 * conflicts in parent/child dependencies.
 * Closed connections loops in the chain are not allowed.
 *
 */
class Chain{

protected:
	//members
	uint8_t							m_id;				/**<ID of the chain.*/
	std::vector<BaseManipulation*>	m_objects;			/**<Pointers to manipulation objects placed in the current execution chain. */
	std::vector<int>				m_idObjects;		/**<ID (order of insertion) of the mimmo objects in the chain. */
	uint32_t						m_objcounter;		/**<Counter of objects inserted the chain.*/

    bitpit::Logger*                 m_log;              /**<Pointer to logger.*/

    bool                            m_plotDebRes;       /**<boolean to activate plotting of debug intermediate results */
    std::string                     m_outputDebRes;     /**<directory path to store the debug intermediate results, if plot is enabled*/
	//static members
	static	uint8_t					sm_chaincounter;	/**<Current global number of chain in the instance. */


public:
	Chain();
	virtual ~Chain();

    std::unique_ptr<Chain> clone() const;

    //get/set methods
	uint32_t	getNObjects();
	uint8_t		getID();
	uint8_t		getNChains();
	int			getID(int i);
	std::string	getName(int i);

	int			addObject(BaseManipulation* obj, int id_ = -1);

	bool		deleteObject(int idobj);
	void		clear();

    void            setPlotDebugResults(bool active);
    void            setOutputDebugResults(std::string path);
    bool            isPlottingDebugResults();
    std::string     getOutputDebugResults();

	//relationship methods
	void 		exec(bool debug = false);
	void 		exec(int idobj);

protected:
    void swap(Chain &x) noexcept;
    //check methods
	void		checkLoops();

private:
    // preventing copy constr and assignment. use clone instead.
    Chain(const Chain & other);
    Chain & operator=(const Chain & other);
};

}


#endif /* __CHAIN_HPP__ */
