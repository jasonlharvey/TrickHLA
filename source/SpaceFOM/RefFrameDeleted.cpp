/*!
@file SpaceFOM/RefFrameDeleted.cpp
@ingroup TrickHLAModel
@brief Callback class the user writes to do something once the object has been
deleted from the RTI.

@copyright Copyright 2023 United States Government as represented by the
Administrator of the National Aeronautics and Space Administration.
No copyright is claimed in the United States under Title 17, U.S. Code.
All Other Rights Reserved.

\par<b>Responsible Organization</b>
Simulation and Graphics Branch, Mail Code ER7\n
Software, Robotics & Simulation Division\n
NASA, Johnson Space Center\n
2101 NASA Parkway, Houston, TX  77058

@tldh
@trick_link_dependency{../source/TrickHLA/Object.cpp}
@trick_link_dependency{RefFrameDeleted.cpp}

@revs_title
@revs_begin
@rev_entry{Edwin Z. Crues, NASA ER7, TrickHLA, November 2023, --, Initial version.}
@revs_end

*/

// System include files.
#include <sstream>
#include <string>

// Trick include files.
#include "trick/exec_proto.h"
#include "trick/message_proto.h" // for send_hs

// TrickHLA include files.
#include "TrickHLA/Object.hh"

// Model include files.
#include "SpaceFOM/RefFrameDeleted.hh"

using namespace TrickHLA;
using namespace SpaceFOM;

/*!
 * @job_class{initialization}
 */
RefFrameDeleted::RefFrameDeleted()
   : TrickHLA::ObjectDeleted()
{
   return;
}

/*!
 * @job_class{shutdown}
 */
RefFrameDeleted::~RefFrameDeleted()
{
   return;
}

void RefFrameDeleted::deleted()
{
   std::ostringstream msg;
   msg << "SpaceFOM::RefFrameDeleted::deleted():" << __LINE__
       << " Object '" << this->object->get_name() << "' deleted from the federation.";
   send_hs( stdout, msg.str().c_str() );
}
