/*!
@file TrickHLA/SyncPointList.hh
@ingroup TrickHLA
@brief This class will manage different lists of HLA synchronization points. It
       is intended for this class to be extended by an Execution Control class.

@copyright Copyright 2024 United States Government as represented by the
Administrator of the National Aeronautics and Space Administration.
No copyright is claimed in the United States under Title 17, U.S. Code.
All Other Rights Reserved.

\par<b>Responsible Organization</b>
Simulation and Graphics Branch, Mail Code ER7\n
Software, Robotics & Simulation Division\n
NASA, Johnson Space Center\n
2101 NASA Parkway, Houston, TX  77058

@trick_parse{everything}

@python_module{TrickHLA}

@tldh
@trick_link_dependency{../../source/TrickHLA/SyncPointList.cpp}
@trick_link_dependency{../../source/TrickHLA/Federate.cpp}
@trick_link_dependency{../../source/TrickHLA/MutexLock.cpp}
@trick_link_dependency{../../source/TrickHLA/SyncPnt.cpp}

@revs_title
@revs_begin
@rev_entry{Dan Dexter, NASA ER6, TrickHLA, March 2024, --, Initial implementation.}
@revs_end

*/

#ifndef TRICKHLA_SYNC_POINT_LIST_HH
#define TRICKHLA_SYNC_POINT_LIST_HH

// System includes.
#include <map>
#include <string>
#include <vector>

// Trick include files.

// TrickHLA include files.
#include "TrickHLA/Federate.hh"
#include "TrickHLA/Int64Time.hh"
#include "TrickHLA/MutexLock.hh"
#include "TrickHLA/StandardsSupport.hh"
#include "TrickHLA/SyncPnt.hh"
#include "TrickHLA/SyncPntLoggable.hh"

// C++11 deprecated dynamic exception specifications for a function so we need
// to silence the warnings coming from the IEEE 1516 declared functions.
// This should work for both GCC and Clang.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated"
// HLA include files.
#include RTI1516_HEADER
#pragma GCC diagnostic pop

namespace TrickHLA
{

typedef std::vector< SyncPnt * > SyncPointVector;

class SyncPointList
{
   // Let the Trick input processor access protected and private data.
   // InputProcessor is really just a marker class (does not really
   // exists - at least yet). This friend statement just tells Trick
   // to go ahead and process the protected and private data as well
   // as the usual public data.
   friend class InputProcessor;
   // IMPORTANT Note: you must have the following line too.
   // Syntax: friend void init_attr<namespace>__<class name>();
   friend void init_attrTrickHLA__SyncPointList();

  public:
   //
   // Public constructors and destructor.
   //
   /*! @brief Constructor for the TrickHLA SyncPointList class. */
   SyncPointList( std::string const &name,
                  MutexLock         &mtx,
                  Federate          *fed );

   /*! @brief Pure virtual destructor for the TrickHLA SyncPointList class. */
   virtual ~SyncPointList();

  public:
   void setup( Federate *fed );

   std::string &get_list_name();

   void clear();

   SyncPnt *get_sync_point( std::wstring const &label ); // Search all lists for the unique sync-point label.

   bool add( std::wstring const &label );

   bool add( std::wstring const &label, Int64Time time );

   bool contains( std::wstring const &label );

   bool is_registered( std::wstring const &label );

   bool mark_registered( std::wstring const &label );

   bool register_sync_point( std::wstring const &label );

   bool register_sync_point( std::wstring const &label, RTI1516_NAMESPACE::FederateHandleSet const &handle_set );

   bool register_all();

   bool register_all( RTI1516_NAMESPACE::FederateHandleSet const &handle_set );

   bool register_sync_point( SyncPnt *sp );

   bool register_sync_point( SyncPnt *sp, RTI1516_NAMESPACE::FederateHandleSet const &handle_set );

   bool is_announced( std::wstring const &label );

   bool mark_announced( std::wstring const &label );

   bool wait_for_announced( std::wstring const &label );

   bool wait_for_all_announced();

   bool wait_for_announced( SyncPnt *sp );

   bool is_achieved( std::wstring const &label );

   bool achieve( std::wstring const &label );

   bool achieve_all();

   bool achieve_sync_point( SyncPnt *sp );

   bool is_synchronized( std::wstring const &label );

   bool mark_synchronized( std::wstring const &label );

   bool wait_for_synchronized( std::wstring const &label );

   bool wait_for_all_synchronized();

   bool wait_for_synchronized( SyncPnt *sp );

  protected:
   SyncPointVector list; ///< @trick_io{**} Vector of sync-points objects.

   std::string list_name; ///< @trick_io{**} Name of this sync-point list.

   MutexLock &mutex; ///< @trick_io{**} Mutex to lock thread over critical code sections.

   Federate *federate; ///< @trick_units{--} Associated TrickHLA Federate.

  private:
   /*! @brief Don't allow the default constructor because the mutex needs to be assigned. */
   SyncPointList();

   // Do not allow the copy constructor or assignment operator.
   /*! @brief Copy constructor for SyncPointList class.
    *  @details This constructor is private to prevent inadvertent copies. */
   SyncPointList( SyncPointList const &rhs );
   /*! @brief Assignment operator for SyncPointList class.
    *  @details This assignment operator is private to prevent inadvertent copies. */
   SyncPointList &operator=( SyncPointList const &rhs );
};

} // namespace TrickHLA

#endif /* TRICKHLA_SYNC_POINT_LIST_HH */
