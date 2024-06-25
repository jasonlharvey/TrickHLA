/*!
@file TrickHLA/SyncPointManager.hh
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
@trick_link_dependency{../../source/TrickHLA/SyncPointManager.cpp}
@trick_link_dependency{../../source/TrickHLA/Federate.cpp}
@trick_link_dependency{../../source/TrickHLA/MutexLock.cpp}
@trick_link_dependency{../../source/TrickHLA/SyncPointList.cpp}

@revs_title
@revs_begin
@rev_entry{Dan Dexter, NASA ER6, TrickHLA, March 2024, --, Initial implementation.}
@revs_end

*/

#ifndef TRICKHLA_SYNC_POINT_MANAGER_HH
#define TRICKHLA_SYNC_POINT_MANAGER_HH

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
#include "TrickHLA/SyncPntLoggable.hh"
#include "TrickHLA/SyncPointList.hh"

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

typedef std::vector< SyncPointList * > SyncPointListVector;

static std::string const UNKNOWN_SYNC_POINT_LIST = "UNKNOWN_SYNC_POINT_LIST";

class SyncPointManager
{
   // Let the Trick input processor access protected and private data.
   // InputProcessor is really just a marker class (does not really
   // exists - at least yet). This friend statement just tells Trick
   // to go ahead and process the protected and private data as well
   // as the usual public data.
   friend class InputProcessor;
   // IMPORTANT Note: you must have the following line too.
   // Syntax: friend void init_attr<namespace>__<class name>();
   friend void init_attrTrickHLA__SyncPointManager();

  public:
   //
   // Public constructors and destructor.
   //
   /*! @brief Default constructor for the TrickHLA SyncPointManager class. */
   SyncPointManager();
   /*! @brief Constructor for the TrickHLA SyncPointManager class. */
   explicit SyncPointManager( Federate *fed );

   /*! @brief Pure virtual destructor for the TrickHLA SyncPointManager class. */
   virtual ~SyncPointManager() = 0;

  protected:
   void setup( Federate *fed );

   int const get_list_index_for_sync_point( std::wstring const &label ); // Search all lists for the unique sync-point label.

   int const get_list_index_for_list_name( std::string const &list_name );

   /*! @brief Add the given synchronization point label to the named list.
    *  @param label Synchronization point label.
    *  @param list_name The name of the list of sync-points to add to.
    *  @return True if the synchronization point is added, false otherwise. */
   bool add_sync_point_list( std::string const &list_name );

   bool add_sync_point( std::wstring const &label, std::string const &list_name );

   bool add_sync_point( std::wstring const &label, std::string const &list_name, Int64Time time );

   bool contains_sync_point( std::wstring const &label );

   bool contains_sync_point_list_name( std::string const &list_name );

   bool is_sync_point_registered( std::wstring const &label );

   bool mark_sync_point_registered( std::wstring const &label );

   bool register_sync_point( std::wstring const &label );

   bool register_sync_point( std::wstring const &label, RTI1516_NAMESPACE::FederateHandleSet const &handle_set );

   bool register_all_sync_points( std::string const &list_name );

   bool register_all_sync_points( std::string const &list_name, RTI1516_NAMESPACE::FederateHandleSet const &handle_set );

   bool is_sync_point_announced( std::wstring const &label );

   bool mark_sync_point_announced( std::wstring const &label );

   bool wait_for_sync_point_announced( std::wstring const &label );

   bool wait_for_all_sync_points_announced( std::string const &list_name );

   bool is_sync_point_achieved( std::wstring const &label );

   bool achieve_sync_point( std::wstring const &label );

   bool achieve_all_sync_points( std::string const &list_name );

   bool is_sync_point_synchronized( std::wstring const &label );

   bool mark_sync_point_synchronized( std::wstring const &label );

   bool wait_for_sync_point_synchronized( std::wstring const &label );

   bool wait_for_all_sync_points_synchronized( std::string const &list_name );

   // Callbacks from FedAmb.
   virtual void sync_point_registration_succeeded( std::wstring const &label );

   virtual void sync_point_registration_failed( std::wstring const &label, RTI1516_NAMESPACE::SynchronizationPointFailureReason reason );

   virtual void sync_point_announced( std::wstring const &label, RTI1516_NAMESPACE::VariableLengthData const &user_supplied_tag );

   virtual void sync_point_federation_synchronized( std::wstring const &label );

  protected:
   MutexLock mutex; ///< @trick_io{**} Mutex to lock thread over critical code sections.

   SyncPointListVector sync_pnt_lists; ///< @trick_io{**} Map of named sync-point lists.

   Federate *federate; ///< @trick_units{--} Associated TrickHLA Federate.

  private:
   // Do not allow the copy constructor or assignment operator.
   /*! @brief Copy constructor for SyncPointManager class.
    *  @details This constructor is private to prevent inadvertent copies. */
   SyncPointManager( SyncPointManager const &rhs );
   /*! @brief Assignment operator for SyncPointManager class.
    *  @details This assignment operator is private to prevent inadvertent copies. */
   SyncPointManager &operator=( SyncPointManager const &rhs );
};

} // namespace TrickHLA

#endif /* TRICKHLA_SYNC_POINT_MANAGER_HH */
