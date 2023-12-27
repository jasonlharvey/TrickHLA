##############################################################################
# PURPOSE:
#    (This is a python input file class to set up the general parameters that
#     describe an JEOD-based SpaceFOM reference frame tree object.)
#
# REFERENCE:
#    (Trick documentation.)
#
# ASSUMPTIONS AND LIMITATIONS:
#    ((Inherits from the base HLAObjectConfig class)
#     (Assumes that trick is available globally.))
#
# PROGRAMMERS:
#    (((Edwin Z. Crues) (NASA) (July 2023) (--) (Space FOM development)))
##############################################################################
import trick
from ..SpaceFOM.SpaceFOMFederateConfig import *
from ..SpaceFOM.SpaceFOMRefFrameObject import *


#class JEODRefFrameTreeObject(TrickHLAObjectConfig):
class JEODRefFrameTreeObject():
   
   federate = None

   # Persistent access to tree frames.
   ssbary_frame = None
   sun_frame = None
   embary_frame = None
   earth_frame = None
   moon_frame = None
   mars_frame = None
   earth_pfix_frame = None
   moon_pfix_frame = None
   mars_pfix_frame = None

   def __init__( self,
                 federate_instance,
                 tree_instance,
                 create_frame_objects = False,
                 lag_comp_type = trick.TrickHLA.LAG_COMPENSATION_NONE ):
      
      # Refernce the federate object.
      federate = federate_instance

      # Setup shortcuts to planetary reference frame objects.
      ssbary_frame     = tree_instance.ssbary_frame_ptr
      sun_frame        = tree_instance.sun_frame_ptr
      embary_frame     = tree_instance.embary_frame_ptr
      earth_frame      = tree_instance.earth_frame_ptr
      moon_frame       = tree_instance.moon_frame_ptr
      mars_frame       = tree_instance.mars_frame_ptr
      earth_pfix_frame = tree_instance.earth_pfix_frame_ptr
      moon_pfix_frame  = tree_instance.moon_pfix_frame_ptr
      mars_pfix_frame  = tree_instance.mars_pfix_frame_ptr

      #---------------------------------------------------------------------------
      # Set up the Reference Frame tree RefFrame objects for discovery.
      #---------------------------------------------------------------------------
      # Setup the solar system barycenter frame.
      # This will be the root reference frame for the tree.
      ssbary_frame_obj = SpaceFOMRefFrameObject( create_frame_objects,
                                                 'SolarSystemBarycentricInertial',
                                                 ssbary_frame,
                                                 'solar_system_barycenter.frame_packing' )
      # Set the root frame and add it to the federate.
      federate.set_root_frame( ssbary_frame_obj )

      # Setup the Sun inertial frame.
      sun_frame_obj = SpaceFOMRefFrameObject( create_frame_objects,
                                              'SunCentricInertial',
                                              sun_frame,
                                              'sun_inertial.frame_packing',
                                              solar_system_barycenter.frame_packing,
                                              'SolarSystemBarycentricInertial',
                                              frame_conditional   = sun_inertial.conditional,
                                              frame_lag_comp      = sun_inertial.lag_compensation,
                                              frame_lag_comp_type = lag_comp_type,
                                              frame_ownership     = sun_inertial.ownership_handler,
                                              frame_deleted       = sun_inertial.deleted_callback )
      federate.add_fed_object( sun_frame_obj )

      # Setup the Earth-Moon barycenter inertial frame.
      embary_frame_obj = SpaceFOMRefFrameObject( create_frame_objects,
                                                 'EarthMoonBarycentricInertial',
                                                 embary_frame,
                                                 'earth_moon_barycenter.frame_packing',
                                                 solar_system_barycenter.frame_packing,
                                                 'SolarSystemBarycentricInertial',
                                                 frame_conditional   = earth_moon_barycenter.conditional,
                                                 frame_lag_comp      = earth_moon_barycenter.lag_compensation,
                                                 frame_lag_comp_type = lag_comp_type,
                                                 frame_ownership     = earth_moon_barycenter.ownership_handler,
                                                 frame_deleted       = earth_moon_barycenter.deleted_callback )
      federate.add_fed_object( embary_frame_obj )

      # Setup the Earth inertial frame.
      earth_frame_obj = SpaceFOMRefFrameObject( create_frame_objects,
                                                'EarthMJ2000Eq',
                                                earth_frame,
                                                'earth_centered_inertial.frame_packing',
                                                earth_moon_barycenter.frame_packing,
                                                'EarthMoonBarycentricInertial',
                                                frame_conditional   = earth_centered_inertial.conditional,
                                                frame_lag_comp      = earth_centered_inertial.lag_compensation,
                                                frame_lag_comp_type = lag_comp_type,
                                                frame_ownership     = earth_centered_inertial.ownership_handler,
                                                frame_deleted       = earth_centered_inertial.deleted_callback )
      federate.add_fed_object( earth_frame_obj )

      # Setup the Moon inertial frame.
      moon_frame_obj = SpaceFOMRefFrameObject( create_frame_objects,
                                               'MoonCentricInertial',
                                               moon_frame,
                                               'moon_centered_inertial.frame_packing',
                                               earth_moon_barycenter.frame_packing,
                                               'EarthMoonBarycentricInertial',
                                               frame_conditional   = moon_centered_inertial.conditional,
                                               frame_lag_comp      = moon_centered_inertial.lag_compensation,
                                               frame_lag_comp_type = lag_comp_type,
                                               frame_ownership     = moon_centered_inertial.ownership_handler,
                                               frame_deleted       = moon_centered_inertial.deleted_callback )
      # Add the frame to federate.
      federate.add_fed_object( moon_frame_obj )

      mars_frame_obj = SpaceFOMRefFrameObject( create_frame_objects,
                                               'MarsCentricInertial',
                                               mars_frame,
                                               'mars_centered_inertial.frame_packing',
                                               solar_system_barycenter.frame_packing,
                                               'SolarSystemBarycentricInertial',
                                               frame_conditional   = mars_centered_inertial.conditional,
                                               frame_lag_comp      = mars_centered_inertial.lag_compensation,
                                               frame_lag_comp_type = lag_comp_type,
                                               frame_ownership     = mars_centered_inertial.ownership_handler,
                                               frame_deleted       = mars_centered_inertial.deleted_callback )
      federate.add_fed_object( mars_frame_obj )

      # Setup the Earth planet fixed frame.
      earth_pfix_frame_obj = SpaceFOMRefFrameObject( create_frame_objects,
                                                     'EarthCentricFixed',
                                                     earth_pfix_frame,
                                                     'earth_centered_fixed.frame_packing',
                                                     earth_centered_inertial.frame_packing,
                                                     'EarthMJ2000Eq',
                                                     frame_conditional   = earth_centered_fixed.conditional,
                                                     frame_lag_comp      = earth_centered_fixed.lag_compensation,
                                                     frame_lag_comp_type = lag_comp_type,
                                                     frame_ownership     = earth_centered_fixed.ownership_handler,
                                                     frame_deleted       = earth_centered_fixed.deleted_callback )
      federate.add_fed_object( earth_pfix_frame_obj )

      # Setup the Moon planet fixed frame.
      moon_pfix_frame_obj = SpaceFOMRefFrameObject( create_frame_objects,
                                                    'MoonCentricFixed',
                                                    moon_pfix_frame,
                                                    'moon_centered_fixed.frame_packing',
                                                    moon_centered_inertial.frame_packing,
                                                    'MoonCentricInertial',
                                                    frame_conditional   = moon_centered_fixed.conditional,
                                                    frame_lag_comp      = moon_centered_fixed.lag_compensation,
                                                    frame_lag_comp_type = lag_comp_type,
                                                    frame_ownership     = moon_centered_fixed.ownership_handler,
                                                    frame_deleted       = moon_centered_fixed.deleted_callback )
      federate.add_fed_object( moon_pfix_frame_obj )

      # Setup the Mars planet fixed frame.
      mars_pfix_frame_obj = SpaceFOMRefFrameObject( create_frame_objects,
                                                    'MarsCentricFixed',
                                                    mars_pfix_frame,
                                                    'mars_centered_fixed.frame_packing',
                                                    mars_centered_inertial.frame_packing,
                                                    'MarsCentricInertial',
                                                    frame_conditional   = mars_centered_fixed.conditional,
                                                    frame_lag_comp      = mars_centered_fixed.lag_compensation,
                                                    frame_lag_comp_type = lag_comp_type,
                                                    frame_ownership     = mars_centered_fixed.ownership_handler,
                                                    frame_deleted       = mars_centered_fixed.deleted_callback )
      federate.add_fed_object( mars_pfix_frame_obj )

      return
