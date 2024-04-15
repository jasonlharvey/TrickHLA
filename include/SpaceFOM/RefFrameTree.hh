/*!
@file SpaceFOM/RefFrameTree.hh
@ingroup SpaceFOM
@brief This class provides an reference implementation for a SpaceFOM
reference frame tree.

This is the base implementation for the Space Reference FOM (SpaceFOM)
interface to a Reference Frame tree object. This needs to be available
to the SpaceFOM initialization process for the root reference frame
discovery step in the initialization process.

The reference frame tree takes the form of a Directed Acyclic Graph (DAG).
A node in the tree is a SpaceFOM::RefFrameBase object.

@copyright Copyright 2019 United States Government as represented by the
Administrator of the National Aeronautics and Space Administration.
No copyright is claimed in the United States under Title 17, U.S. Code.
All Other Rights Reserved.

\par<b>Responsible Organization</b>
Simulation and Graphics Branch, Mail Code ER7\n
Software, Robotics & Simulation Division\n
NASA, Johnson Space Center\n
2101 NASA Parkway, Houston, TX  77058

@trick_parse{everything}

@python_module{SpaceFOM}

@tldh
@trick_link_dependency{../../source/SpaceFOM/LRTreeNodeBase.cpp}
@trick_link_dependency{../../source/SpaceFOM/LRTreeBase.cpp}
@trick_link_dependency{../../source/SpaceFOM/RefFrameBase.cpp}
@trick_link_dependency{../../source/SpaceFOM/RefFrameTree.cpp}

@revs_title
@revs_begin
@rev_entry{Edwin Z. Crues, NASA ER7, TrickHLA, July 2023, --, Initial version.}
@revs_end

*/

#ifndef SPACEFOM_REF_FRAME_TREE_HH
#define SPACEFOM_REF_FRAME_TREE_HH

// System include files.
#include <map>
#include <string>

// TrickHLA include files.

// SpaceFOM include files.
#include "SpaceFOM/LRTreeBase.hh"
#include "SpaceFOM/RefFrameBase.hh"

namespace SpaceFOM
{

class RefFrameTree : public SpaceFOM::LRTreeBase
{

   // Let the Trick input processor access protected and private data.
   // InputProcessor is really just a marker class (does not really
   // exists - at least yet). This friend statement just tells Trick
   // to go ahead and process the protected and private data as well
   // as the usual public data.
   friend class InputProcessor;
   // IMPORTANT Note: you must have the following line too.
   // Syntax: friend void init_attr<namespace>__<class name>();
   friend void init_attrSpaceFOM__RefFrameTree();

  public:
   // Public constructors and destructors.
   /*! @brief Default constructor for the SpaceFOM RefFrameBase class. */
   RefFrameTree();
   /*! @brief Destructor for the SpaceFOM RefFrameBase class. */
   virtual ~RefFrameTree();

   /*! @brief Add a reference frame to the tree.
    *  @details Add a SpaceFOM reference frame into the reference frame tree.
    *  @param frame_ptr Pointer to the reference frame to add.
    *  @return Success or failure of the add. */
   virtual bool add_frame( RefFrameBase *frame_ptr );

   virtual bool build_tree();
   virtual bool check_tree();

   virtual bool has_frame( char const *name );
   virtual bool has_frame( std::string const &name );
   virtual bool has_frame( RefFrameBase const *frame );

   virtual RefFrameBase *find_frame( char const *name );
   virtual RefFrameBase *find_frame( std::string const &name );

   /*! @brief Print out the Reference Frame Tree nodes.
    *  @param stream Output stream. */
   virtual void print_tree( std::ostream &stream = std::cout );

  private:
   // This object is not copyable
   /*! @brief Copy constructor for RefFrameTree class.
    *  @details This constructor is private to prevent inadvertent copies. */
   RefFrameTree( RefFrameTree const &rhs );
   /*! @brief Assignment operator for RefFrameTree class.
    *  @details This assignment operator is private to prevent inadvertent copies. */
   RefFrameTree &operator=( RefFrameTree const &rhs );
};

} // namespace SpaceFOM

#endif // SPACEFOM_REF_FRAME_TREE_HH: Do NOT put anything after this line!
