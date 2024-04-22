/*!
@file TrickHLA/ExecutionConfigurationBase.cpp
@ingroup TrickHLA
@brief The abstract base class for the TrickHLA simulation execution
configuration class.

@details This class is used to provide the fundamentals for exchanging
startup, initialization, and run time configuration information between
participating federates in an HLA federation execution.

@copyright Copyright 2019 United States Government as represented by the
Administrator of the National Aeronautics and Space Administration.
No copyright is claimed in the United States under Title 17, U.S. Code.
All Other Rights Reserved.

\par<b>Responsible Organization</b>
Simulation and Graphics Branch, Mail Code ER7\n
Software, Robotics & Simulation Division\n
NASA, Johnson Space Center\n
2101 NASA Parkway, Houston, TX  77058

\par<b>Assumptions and Limitations:</b>
- One and only one ExecutionConfigurationBase object should exist in an federation
execution.

@tldh
@trick_link_dependency{DebugHandler.cpp}
@trick_link_dependency{ExecutionConfigurationBase.cpp}
@trick_link_dependency{ExecutionControlBase.cpp}
@trick_link_dependency{Federate.cpp}
@trick_link_dependency{SleepTimeout.cpp}
@trick_link_dependency{Types.cpp}
@trick_link_dependency{Utilities.cpp}

@revs_title
@revs_begin
@rev_entry{Edwin Z. Crues, NASA ER7, TrickHLA, March 2020, --, Version 3 rewrite.}
@revs_end

*/

// System include files.
#include <cstdint>
#include <string>

// Trick include files.
#include "trick/MemoryManager.hh"
#include "trick/message_proto.h"

// TrickHLA include files.
#include "TrickHLA/DebugHandler.hh"
#include "TrickHLA/ExecutionConfigurationBase.hh"
#include "TrickHLA/ExecutionControlBase.hh"
#include "TrickHLA/Federate.hh"
#include "TrickHLA/SleepTimeout.hh"
#include "TrickHLA/StandardsSupport.hh"
#include "TrickHLA/StringUtilities.hh"
#include "TrickHLA/Types.hh"
#include "TrickHLA/Utilities.hh"

// C++11 deprecated dynamic exception specifications for a function so we need
// to silence the warnings coming from the IEEE 1516 declared functions.
// This should work for both GCC and Clang.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated"
// HLA include files.
#include RTI1516_HEADER
#pragma GCC diagnostic pop

using namespace std;
using namespace RTI1516_NAMESPACE;
using namespace TrickHLA;

#ifdef __cplusplus
extern "C" {
#endif
// Include the Trick generated ATTRIBUTES for the ExecutionConfiguration class.
// This is used to set up Trick based simulation variable mapping into the
// TrickHLA::Object associated with this class.
extern ATTRIBUTES attrTrickHLA__ExecutionConfigurationBase[];
#ifdef __cplusplus
}
#endif

/*!
 * @job_class{initialization}
 */
ExecutionConfigurationBase::ExecutionConfigurationBase()
   : S_define_name( NULL ),
     pending_update( false ),
     execution_control( NULL )
{
   // Set the name to an empty string.
   this->name = trick_MM->mm_strdup( "" );

   // This is both a TrickHLA::Object and Packing.
   // So, it can safely reference itself.
   this->packing = this;
}

/*!
 * @job_class{initialization}
 */
ExecutionConfigurationBase::ExecutionConfigurationBase(
   char const *s_define_name )
   : pending_update( false ),
     execution_control( NULL )
{
   // Set the name to an empty string.
   this->name = trick_MM->mm_strdup( "" );

   // Set the full path S_define name.
   this->S_define_name = trick_MM->mm_strdup( s_define_name );

   // This is both a TrickHLA::Object and Packing.
   // So, it can safely reference itself.
   this->packing = this;
}

/*!
 * @job_class{shutdown}
 */
ExecutionConfigurationBase::~ExecutionConfigurationBase()
{
   if ( this->S_define_name != NULL ) {
      if ( trick_MM->delete_var( static_cast< void * >( const_cast< char * >( this->S_define_name ) ) ) ) {
         send_hs( stderr, "ExecutionConfigurationBase::~ExecutionConfigurationBase():%d ERROR deleting Trick Memory for 'this->S_define_name'%c",
                  __LINE__, THLA_NEWLINE );
      }
      this->S_define_name = NULL;
   }
}

/*!
 * \par<b>Assumptions and Limitations:</b>
 * - The TrickHLA::ExecutionControlBase class is actually an abstract class.
 * Therefore, the actual object instance being passed in is an instantiable
 * polymorphic child of the TrickHLA::ExecutionControlBase class.
 *
 * @job_class{default_data}
 */
void ExecutionConfigurationBase::setup(
   TrickHLA::ExecutionControlBase &exec_control )
{
   // Set the TrickHLA::ExecutionControlBase instance reference.
   this->execution_control = &exec_control;

   // Configure the default Execution Configuration attributes.
   this->configure_attributes();
}

/*!
 * @job_class{initialization}
 */
void ExecutionConfigurationBase::set_S_define_name(
   char const *new_name )
{
   if ( this->S_define_name != NULL ) {
      if ( trick_MM->delete_var( static_cast< void * >( const_cast< char * >( this->S_define_name ) ) ) ) {
         send_hs( stderr, "ExecutionConfigurationBase::set_S_define_name():%d ERROR deleting Trick Memory for 'this->S_define_name'%c",
                  __LINE__, THLA_NEWLINE );
      }
      this->S_define_name = NULL;
   }

   // Set the full path S_define name.
   this->S_define_name = trick_MM->mm_strdup( new_name );
}

void ExecutionConfigurationBase::reset_preferred_order()
{
   this->any_attribute_timestamp_order = false;

   if ( ( attr_count > 0 ) && ( attributes != NULL ) ) {
      for ( int i = 0; i < attr_count; ++i ) {
         attributes[i].set_preferred_order( TRANSPORT_RECEIVE_ORDER );
      }
   }
}

void ExecutionConfigurationBase::reset_ownership_states()
{
   // Make sure we are setup to create an HLA instance of the sim-config which
   // means we will reserve the instance name as well.
   set_create_HLA_instance( true );

   if ( ( attr_count > 0 ) && ( attributes != NULL ) ) {
      for ( int i = 0; i < attr_count; ++i ) {

         // All the simulation configuration attributes must be configured
         // for initialization.
         if ( ( attributes[i].get_configuration() & CONFIG_INITIALIZE ) != CONFIG_INITIALIZE ) {
            int config = attributes[i].get_configuration() + CONFIG_INITIALIZE;
            attributes[i].set_configuration( (DataUpdateEnum)config );
         }

         // Set the attributes to be locally-owned if the publish flag is set,
         // since ownership will be established once we determine if we are
         // the master federate.
         if ( attributes[i].is_publish() ) {
            attributes[i].mark_locally_owned();
         }
      }
   }
}

void ExecutionConfigurationBase::set_master(
   bool const is_master )
{
   if ( is_master ) {
      // We are the Master so we will create the sim-config HLA instance.
      set_create_HLA_instance( true );

      // Since we will be publishing the execution configuration object, make
      // sure the attribute locally own flag is set, the publish flag is set,
      // and the subscribe flag is cleared.
      for ( int i = 0; i < attr_count; ++i ) {
         attributes[i].mark_locally_owned();
         attributes[i].set_publish( true );
         attributes[i].set_subscribe( false );
      }
   } else {
      // We are Not the Master so do not create the execution configuration instance.
      set_create_HLA_instance( false );

      // Since we will be subscribing the execution configuration object, make
      // sure the attribute remotely own flag is set, the publish flag is
      // cleared, and the subscribe flag is set.
      for ( int i = 0; i < attr_count; ++i ) {
         attributes[i].mark_remotely_owned();
         attributes[i].set_publish( false );
         attributes[i].set_subscribe( true );
      }
   }
}

/*!
 * @details Calling this function will block until the execution configuration
 * object instance in the Federation has been registered.
 * @job_class{initialization}
 */
void ExecutionConfigurationBase::wait_for_registration()
{
   if ( DebugHandler::show( DEBUG_LEVEL_2_TRACE, DEBUG_SOURCE_EXECUTION_CONFIG ) ) {
      send_hs( stdout, "ExecutionConfigurationBase::wait_for_registration():%d%c",
               __LINE__, THLA_NEWLINE );
   }

   Federate *federate = get_federate();

   int  obj_reg_cnt   = 0;
   bool print_summary = DebugHandler::show( DEBUG_LEVEL_2_TRACE, DEBUG_SOURCE_EXECUTION_CONFIG );
   bool any_unregistered_obj;
   int  total_obj_cnt = 1;

   int64_t      wallclock_time;
   SleepTimeout print_timer( federate->wait_status_time );
   SleepTimeout sleep_timer;

   do {

      // Check for shutdown.
      federate->check_for_shutdown_with_termination();

      // Data objects.
      if ( obj_reg_cnt < total_obj_cnt ) {
         int cnt = 0;

         // Determine if the Exec-Configuration object has been registered.
         if ( this->is_instance_handle_valid() ) {
            ++cnt;
         }

         // If we have a new registration count then update the object
         // registration count and set the flag to show a new summary.
         if ( cnt > obj_reg_cnt ) {
            obj_reg_cnt = cnt;
            if ( !print_summary ) {
               print_summary = DebugHandler::show( DEBUG_LEVEL_2_TRACE, DEBUG_SOURCE_EXECUTION_CONFIG );
            }
         }
      }

      // Print a summary of what objects are registered and which ones are not.
      if ( print_summary ) {
         print_summary = false;

         // Build the summary as an output string stream.
         ostringstream summary;
         summary << "ExecutionConfigurationBase::wait_for_registration()"
                 << __LINE__ << "\nOBJECTS: " << total_obj_cnt;

         // Execution-Configuration object
         summary << "\n  1:Object instance '" << this->get_name() << "' ";

         if ( this->is_instance_handle_valid() ) {
            string id_str;
            StringUtilities::to_string( id_str, this->get_instance_handle() );
            summary << "(ID:" << id_str << ") ";
         }
         summary << "for class '" << this->get_FOM_name() << "' is "
                 << ( this->is_required() ? "REQUIRED" : "not required" )
                 << " and is "
                 << ( this->is_instance_handle_valid() ? "REGISTERED" : "Not Registered" )
                 << THLA_ENDL;
         // Display the summary.
         send_hs( stdout, summary.str().c_str() );
      }

      // Determine if we have any unregistered objects.
      any_unregistered_obj = ( obj_reg_cnt < total_obj_cnt ); // cppcheck-suppress [knownConditionTrueFalse,unmatchedSuppression]

      // Wait a little while to allow the objects to be registered.
      if ( any_unregistered_obj ) { // cppcheck-suppress [knownConditionTrueFalse]
         sleep_timer.sleep();

         // Check again to determine if we have any unregistered objects.
         any_unregistered_obj = ( obj_reg_cnt < total_obj_cnt ); // cppcheck-suppress [knownConditionTrueFalse,unmatchedSuppression]

         if ( any_unregistered_obj ) { // cppcheck-suppress [knownConditionTrueFalse,unmatchedSuppression]

            // To be more efficient, we get the time once and share it.
            wallclock_time = sleep_timer.time();

            if ( sleep_timer.timeout( wallclock_time ) ) {
               sleep_timer.reset();
               if ( !federate->is_execution_member() ) {
                  ostringstream errmsg;
                  errmsg << "ExecutionConfigurationBase::wait_for_registration():" << __LINE__
                         << " ERROR: Unexpectedly the Federate is no longer an execution member."
                         << " This means we are either not connected to the"
                         << " RTI or we are no longer joined to the federation"
                         << " execution because someone forced our resignation at"
                         << " the Central RTI Component (CRC) level!"
                         << THLA_ENDL;
                  DebugHandler::terminate_with_message( errmsg.str() );
               }
            }

            if ( print_timer.timeout( wallclock_time ) ) {
               print_timer.reset();
               print_summary = true;
            }
         }
      }
   } while ( any_unregistered_obj );
}

bool ExecutionConfigurationBase::wait_for_update() // RETURN: -- None.
{
   Federate *federate = get_federate();

   // We can only receive the ExecutionConfiguration if we are not the master.
   if ( execution_control->is_master() ) {
      return false;
   }

   if ( DebugHandler::show( DEBUG_LEVEL_2_TRACE, DEBUG_SOURCE_EXECUTION_CONFIG ) ) {
      send_hs( stdout, "ExecutionConfigurationBase::wait_for_update():%d Waiting...%c",
               __LINE__, THLA_NEWLINE );
   }

   // Make sure we have at least one piece of Execution Configuration data we can receive.
   if ( this->any_remotely_owned_subscribed_init_attribute() ) {

      int64_t      wallclock_time;
      SleepTimeout print_timer( federate->wait_status_time );
      SleepTimeout sleep_timer;

      // Wait for the data to arrive.
      while ( !this->is_changed() ) {

         // Check for shutdown.
         federate->check_for_shutdown_with_termination();

         sleep_timer.sleep();

         if ( !this->is_changed() ) {

            // To be more efficient, we get the time once and share it.
            wallclock_time = sleep_timer.time();

            if ( sleep_timer.timeout( wallclock_time ) ) {
               sleep_timer.reset();
               if ( !federate->is_execution_member() ) {
                  ostringstream errmsg;
                  errmsg << "ExecutionConfigurationBase::wait_for_update():" << __LINE__
                         << " ERROR: Unexpectedly the Federate is no longer an execution member."
                         << " This means we are either not connected to the"
                         << " RTI or we are no longer joined to the federation"
                         << " execution because someone forced our resignation at"
                         << " the Central RTI Component (CRC) level!"
                         << THLA_ENDL;
                  DebugHandler::terminate_with_message( errmsg.str() );
               }
            }

            if ( print_timer.timeout( wallclock_time ) ) {
               print_timer.reset();
               send_hs( stdout, "ExecutionConfigurationBase::wait_for_update():%d Waiting...%c",
                        __LINE__, THLA_NEWLINE );
            }
         }
      }

      if ( DebugHandler::show( DEBUG_LEVEL_2_TRACE, DEBUG_SOURCE_EXECUTION_CONFIG ) ) {
         send_hs( stdout, "ExecutionConfigurationBase::wait_for_update():%d Received data.%c",
                  __LINE__, THLA_NEWLINE );
      }

      // Receive the Execution Configuration data from the master federate.
      this->receive_init_data();

   } else {
      ostringstream errmsg;
      errmsg << "ExecutionConfigurationBase::wait_for_update():" << __LINE__
             << " ERROR: Execution-Configuration"
             << " is not configured to receive at least one object attribute."
             << " Make sure at least one 'exec_config' attribute has"
             << " 'subscribe = true' set. Please check your input or modified-data"
             << " files to make sure the 'subscribe' value is correctly specified."
             << THLA_ENDL;
      DebugHandler::terminate_with_message( errmsg.str() );
   }

   return true;
}
