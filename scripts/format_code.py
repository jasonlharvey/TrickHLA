#!/usr/bin/python
## @file format_code.py
# @brief This program applies a format standard to TrickHLA source code.
# 
# This is a Python program used to apply standardized code formatting
# rules to the TrickHLA source code.
#
# @revs_title
# @revs_begin
# @rev_entry{ Edwin Z. Crues, NASA ER7, TrickHLA, April 2019, --, Initial creation based off of TrickHLA scripts.}
# @rev_entry{ Dan Dexter, NASA ER6, TrickHLA, June 2020, --, Updated to process TrickHLA models code.}
# @revs_end

import sys
import os
import shutil
import argparse

from trickhla_message import *
from trickhla_environment import *

## Main routine.
def main() :
   
   # Setup command line argument parsing.
   parser = argparse.ArgumentParser( prog='format_code',\
                                     description='Format the TrickHLA source code.' )
   parser.add_argument( '-c', '--clean', help='Clean up all the TrickHLA code formatting artifacts.',\
                         action="store_true" )
   parser.add_argument( '-f', '--file', help='Process a single file in place.' )
   parser.add_argument( '-i', '--in_place', help='Format the TrickHLA source code in place.',\
                         action="store_true" )
   parser.add_argument( '-l', '--llvm_bin', help='Path to LLVM binaries.' )
   parser.add_argument( '-p', '--thla_home', help='Path to TrickHLA directory.' )
   parser.add_argument( '-t', '--test', help='Do not format just show what would be done.',\
                         action="store_true" )
   parser.add_argument( '-v', '--verbose', help='Generate verbose output.',\
                         action="store_true" )

   # Parse the command line arguments.
   args = parser.parse_args()

   # Check for TrickHLA.
   if args.thla_home :
      trickhla_home = ars.thla_home
   else:
      trickhla_home = os.environ.get( 'TRICKHLA_HOME' )
      if trickhla_home == None :
         trickhla_home = os.environ.get( 'TRICK_HLA_HOME' )
         if trickhla_home == None :
            TrickHLAMessage.failure( 'TRICKHLA_HOME not set!' )
   
   # Make sure that the directory actually exists before to move there.
   if os.path.isdir( trickhla_home ) :
      if args.verbose :
         TrickHLAMessage.status( 'Moving to TRICKHLA_HOME: ' + trickhla_home )
      os.chdir( trickhla_home )
   else :
      TrickHLAMessage.failure( 'TrickHLA Home directory not found!' )

   # Now let's check to make sure that the include and source directories exist.
   if not os.path.isdir( 'include' ) :
      TrickHLAMessage.failure( 'Could not find the \'include\' directory!' )
   if not os.path.isdir( 'source' ) :
      TrickHLAMessage.failure( 'Could not find the \'source\' directory!' )
   if not os.path.isdir( 'models' ) :
      TrickHLAMessage.failure( 'Could not find the \'models\' directory!' )
   if not os.path.isdir( 'scripts' ) :
      TrickHLAMessage.failure( 'Could not find the \'scripts\' directory!' )
   
   #
   # If we got this far, then things look like a TrickHLA directory.
   #
   trickhla_scripts = os.path.join( trickhla_home, 'scripts' )

   # Add all the top level source code directories to format.
   trickhla_src_paths = []
   trickhla_src_paths.append( os.path.join( trickhla_home, 'include' ) )
   trickhla_src_paths.append( os.path.join( trickhla_home, 'source' ) )
   trickhla_src_paths.append( os.path.join( trickhla_home, 'models/simconfig' ) )
   trickhla_src_paths.append( os.path.join( trickhla_home, 'models/sine' ) )


   # Find the CLANG format command.
   clang_format_cmd = find_clang_format( args.llvm_bin, args.verbose )
   if clang_format_cmd == None :
      TrickHLAMessage.failure( 'No clang-format command found!' )
      
   #
   # Let's do some command line option sanity checks.
   #
   # The 'in_place' and 'clean' options are incompatible.
   if args.in_place and args.clean :
      TrickHLAMessage.failure( 'The \'in_place\' and \'clean\' options are incompatible!' )
      
   # The 'file' and 'clean' options are incompatible.
   if args.file and args.clean :
      TrickHLAMessage.failure( 'The \'file\' and \'clean\' options are incompatible!' )
      
   # If 'in_place' specified as a command line option, make sure that the
   # user really wants to do that. It WILL change code in place.
   if args.in_place and not args.test:
      TrickHLAMessage.warning( 'Formatting TrickHLA source code in place!' )
      check_in_place = raw_input( 'Are you sure you want to do this? [y]: ' )
      if check_in_place == 'y' :
         TrickHLAMessage.status( 'Proceeding with formatting . . .' )
      else:
         TrickHLAMessage.status( 'You chose not to proceed. Exiting!' )
         sys.exit()
         
   # Format only a specific file if specified.
   if args.file :
      
      format_file( args.file, clang_format_cmd, trickhla_scripts,
                   args.in_place, args.test, args.verbose )
      
   else :
      for src_path in trickhla_src_paths:
         #
         # Format the include and source directories.
         #
         os.chdir( src_path )
         if args.verbose :
            if args.clean :
               TrickHLAMessage.status( 'Cleaning directory: ' + src_path )
            else :
               TrickHLAMessage.status( 'Formatting directory: ' + src_path )
         dir_list = os.listdir( '.' )
         for dir_entry in dir_list :
            # Only interested in directories. There really should not be any files.
            if os.path.isfile( dir_entry ) : continue
            # Either clean up or format the model directory.
            if args.clean :
               cleanup_directory( dir_entry, args.test, args.verbose )
            else :
               format_directory( dir_entry, clang_format_cmd, trickhla_scripts,
                                 args.in_place, args.test, args.verbose )
         # End: for dir_entry in dir_list :
   # End: args.file :
      
   # Let the user know that we are done.
   if args.clean :
      # Let the user know that we are done cleaning up.
      TrickHLAMessage.success( 'Finished cleaning up TrickHLA formatting artifacts.' )
   else :
      # Let the user know that we are done with the formatting.
      TrickHLAMessage.success( 'Finished formatting TrickHLA source code.' )

   return


## Function to find the clang-format command.
#
# This function searches common locations for the clang-format command.
# If the llvm-bin a
#
# @param llvm_bin  The path to the LLVM programs directory.
# @param verbose   Flag to set if verbose outputs are on.
#
def find_clang_format( llvm_bin, verbose = True ):
   
   # Initialize the clang-format command path.
   command_path = None

   # Check to see if the path to the LLVM binaries are set.
   if llvm_bin :
   
      # Use the command line path to set the command.
      command_path = llvm_bin + '/clang-format'
   
   # LLVM path is not set so lets check other options.
   else:

      # Check to see if the $LLVM_HOME environment variable is defined and set.
      llvm_home = os.environ.get( 'LLVM_HOME' )
      if llvm_home :
      
         # Check to see if LLVM_HOME exists.
         if os.path.isdir( llvm_home ) :
         
            if verbose :
               # Let the use know that we have LLVM_HOME and where it is.
               TrickHLAMessage.status( 'LLVM_HOME: ' + llvm_home )
            
               # Form the clang format command based on LLVM_HOME.
               clang_format_cmd = llvm_home + '/clang-format'
         
         else :
            TrickHLAMessage.failure( 'LLVM_HOME not found: ' + llvm_home )
      
      else :
      
         # LLVM_HOME is not set so look in the standard locations for clang-format.
         if os.path.isfile('/usr/bin/clang-format'):
            command_path = '/usr/bin/clang-format'
         elif os.path.isfile('/usr/local/bin/clang-format'):
            command_path = '/usr/local/bin/clang-format'
         else:
         
            # Let's look for it in Brew.
            if os.path.isdir( '/usr/local/Cellar/llvm' ):
   
               # There is an LLVM installation in brew.
               llvm_versions = os.listdir('/usr/local/Cellar/llvm' )
            
               # Assume we will be using the latest version.
               llvm_version = llvm_versions[0]
               for version in llvm_versions :
                  if ( llvm_version < version ) :
                     llvm_version = version
               
               # Generate the clang-format command
               bin_path = os.path.join( '/usr/local/Cellar/llvm', llvm_version )
               bin_path = os.path.join( bin_path, 'bin' )
               command_path = os.path.join( bin_path, 'clang-format' )

   # We're finished hunting. Now let's check for the format command.
   if command_path == None :
      TrickHLAMessage.failure( 'Could not find the clang-format command!' )
   else:
      if not os.path.isfile( command_path ) :
         TrickHLAMessage.failure( 'Could not find the clang-format command!: '\
                                  + command_path )
      else:
         if verbose :
            TrickHLAMessage.status( 'Using CLANG format command: '\
                                     + command_path )
            
   return command_path


## Function to link the .clang-format file.
#
# This check if the .clang-format file exists. If not, it links to the
# base .clang-format file in the TrickHLA scripts directory.
#
# @param thla_scripts  The path to the TrickHLA scripts directory.
# @param test_only     Flag indicating to only show what would be done.
# @param verbose       Flag to set if verbose outputs are on.
#
def link_clang_format( thla_scripts, test_only = False, verbose = True ):

   thla_clang_format = os.path.join( thla_scripts, 'clang-format' )

   # Check for the .clang-format file.
   if os.path.isfile( '.clang-format' ) :
      if verbose :
         TrickHLAMessage.status( 'Found existing format specification file: .clang-format' )
   if os.path.islink( '.clang-format' ) :
      if verbose :
         TrickHLAMessage.status( 'Found existing format specification link: .clang-format' )
   else :
      if test_only :
         TrickHLAMessage.status( 'Would link format specification: '\
                                 + thla_clang_format )
      else :
         os.symlink( thla_clang_format, '.clang-format' )
         if verbose :
            TrickHLAMessage.status( 'Linking to format specification: '\
                                    + thla_clang_format )

   return


## Function to format a specific source file.
#
# This routine will format a specified source file if it exists. The
# routine saves the current working directory and then moves to the
# directory in which the file exists. If not, the routine returns with an
# error condition. If the file exists, this routine runs the clang-format
# command on the specified file.
#
# @return error             The error status for the directory formatting.
# @param  file_path         The path to the file to format.
# @param  clang_format_cmd  The path to the CLANG format command.
# @param  thla_scripts      The path to the TrickHLA scripts directory.
# @param  in_place          Flag indicating files should be formatted in place.
# @param  test_only         Flag indicating to only show what would be done.
# @param  verbose           Flag to set if verbose outputs are on.
#
def format_file(
   file_path,
   clang_format_cmd,
   thla_scripts,
   in_place  = False,
   test_only = False,
   verbose   = True  ) :

   error = False  # No error yet.
   
   # First, let's check to see that the file exists.
   if os.path.isfile( file_path ) :
      if os.path.islink( file_path ) :
         TrickHLAMessage.error( 'File is a link: ' + file_path )
         error = True
         return error
   else :
      TrickHLAMessage.error( 'File does not exist: ' + file_path )
      error = True
      return error
   
   # Extract the directory path to the file and the file base name.
   dir_path = os.path.dirname( file_path )
   file = os.path.basename( file_path )
   
   # Move into the directory.
   original_dir = os.path.abspath(os.path.curdir)
   os.chdir( dir_path )
   
   # Check for the .clang-format file.
   link_clang_format( thla_scripts, test_only, verbose )
   
   # Check to see if we are formatting in place.
   if not in_place :
      
      # We need to create the 'formatted' directory is it does not
      # already exist.
      if not os.path.isdir( 'formatted' ):
         if test_only :
            if verbose :
               TrickHLAMessage.status( 'Would create \'formatted\' directory:' )
         else :
            if verbose :
               TrickHLAMessage.status( 'Creating \'formatted\' directory:' )
            os.mkdir( 'formatted' )
      #else :
         # We should probably clean out the directory if it exists
         # but we are not going to do that.
         
   # End: if not in_place :
   
   #         
   # Format the file.
   #
   # Execute different command depending on 'in place' setting.
   if in_place :
               
      # Build the clang-format command string.
      command = clang_format_cmd + ' -style=file -i ' + file
                  
      if test_only :
                  
         TrickHLAMessage.status( 'Would format file in place with command:' )
         TrickHLAMessage.status( '   ' + command )
                  
      else :
                  
         if verbose :
            TrickHLAMessage.status( 'Formatting in place: ' + file )
                     
         # Execute the command.
         os.system( command )
                  
   else :

      # Build the clang-format command string.
      command = clang_format_cmd + ' -style=file ' + file + ' > formatted/' + file
               
      if test_only :
                  
         TrickHLAMessage.status( 'Would format file with command:' )
         TrickHLAMessage.status( '   ' + command )
                  
      else :
         if verbose :
            TrickHLAMessage.status( 'Formatting in \'formatted\': ' + file )
                     
         # Execute the command.
         os.system( command )
                  
   # End: if in_place :
   
   # Move back to the original directory.
   os.chdir( original_dir )

   return


## Function to format all the identifiable source files in a directory.
#
# This routine will format all the identifiable source files in a given
# directory. The routine saves the current working directory and then
# moves to the specified directory if it exists. If not, the routine
# returns with an error condition. If the directory exists, this routine
# runs the clang-format command on all identifiable source files in the
# directory.
#
# @return error             The error status for the directory formatting.
# @param  dir_path          The path to directory to format.
# @param  clang_format_cmd  The path to the CLANG format command.
# @param  thla_scripts      The path to the TrickHLA scripts directory.
# @param  in_place          Flag indicating files should be formatted in place.
# @param  test_only         Flag indicating to only show what would be done.
# @param  verbose           Flag to set if verbose outputs are on.
#
def format_directory(
   dir_path,
   clang_format_cmd,
   thla_scripts,
   in_place  = False,
   test_only = False,
   verbose   = True  ) :

   error = False  # No error yet.
   
   # Move into the directory.
   if verbose :
      TrickHLAMessage.status( 'Formatting ' + dir_path + ' directory:' )
   original_dir = os.path.abspath(os.path.curdir)
   os.chdir( dir_path )
   
   # Check for the .clang-format file.
   link_clang_format( thla_scripts, test_only, verbose )
   
   # Check to see if we are formatting in place.
   if not in_place :
      
      # We need to create the 'formatted' directory is it does not
      # already exist.
      if not os.path.isdir( 'formatted' ):
         if test_only :
            if verbose :
               TrickHLAMessage.status( 'Would create \'formatted\' directory:' )
         else :
            if verbose :
               TrickHLAMessage.status( 'Creating \'formatted\' directory:' )
            os.mkdir( 'formatted' )
      #else :
         # We should probably clean out the directory if it exists
         # but we are not going to do that.

   # End: if not in_place :

   # Now format recognized files in the directory.
   dir_list = os.listdir( '.' )
   for file in dir_list :

      # Only process files.
      if os.path.isfile( file ) and not os.path.islink( file ) :

         # Only interested in files with certain extensions.
         if    file.endswith( '.hh' ) \
            or file.endswith( '.h' ) \
            or file.endswith( '.cpp' ) \
            or file.endswith( '.c' )   :

            # Execute different command depending on 'in place' setting.
            if in_place :
               
               # Build the clang-format command string.
               command = clang_format_cmd + ' -style=file -i ' + file
                  
               if test_only :
                  
                  TrickHLAMessage.status( 'Would format file in place with command:' )
                  TrickHLAMessage.status( '   ' + command )
                  
               else :
                  
                  if verbose :
                     TrickHLAMessage.status( 'Formatting in place: ' + file )
                     
                  # Execute the command.
                  os.system( command )
                  
            else :

               # Build the clang-format command string.
               command = clang_format_cmd + ' -style=file ' + file + ' > formatted/' + file
               
               if test_only :
                  
                  TrickHLAMessage.status( 'Would format file with command:' )
                  TrickHLAMessage.status( '   ' + command )
                  
               else :
                  if verbose :
                     TrickHLAMessage.status( 'Formatting in \'formatted\': ' + file )
                     
                  # Execute the command.
                  os.system( command )
                  
            # End: if in_place :
            
      # End: if os.path.isfile( file ) and not os.path.islink( file ) :
      
   # End: for file in dir_list :
   
   # Move back to the original directory.
   os.chdir( original_dir )

   return error


## Function to cleans up all the formatting artifacts in a directory.
#
# This routine will clean up all the identifiable formatting artifacts in
# a given directory. The routine saves the current working directory and
# then moves to the specified directory if it exists. If not, the routine
# returns with an error condition. If the directory exists, this routine
# removes the formatted directory.
#
# @param  dir_path     The path to the directory to clean up.
# @param  test_only    Flag indicating to only show what would be done.
# @param  verbose      Flag to set if verbose outputs are on.
#
def cleanup_directory(
   dir_path,
   test_only = False,
   verbose   = True  ) :
   
   # Move into the directory.
   if verbose :
      TrickHLAMessage.status( 'Cleaning up directory: ' + dir_path )
   original_dir = os.path.abspath(os.path.curdir)
   os.chdir( dir_path )
   
   # Check for the '.clang-format' file and remove it if a link.
   if os.path.islink( '.clang-format' ) :
      if test_only :
         TrickHLAMessage.status( 'Would remove \'.clang-format\' link.' )
      else :
         if verbose :
            TrickHLAMessage.status( 'Removing \'.clang-format\' link.' )
         os.remove( '.clang-format' )
      # End: if test_only :
   # End: if os.path.islink( '.clang-format' ) :
   
   # Check for the 'formatted' directory.
   if os.path.isdir( 'formatted' ) :
      if test_only :
         TrickHLAMessage.status( 'Would remove \'formatted\' directory.' )
      else :
         if verbose :
            TrickHLAMessage.status( 'Removing \'formatted\' directory.' )
         # This is not a test, so remove the directory.
         for root, dirs, files in os.walk( 'formatted' ):
            # Remove all the files in the formatted directory.
            # Otherwise, we will not be able to remove the directory.
            for file in files:
               # Need to prepend root directory to get the full path to the file.
               file_path = os.path.join( root, file )
               # Remove the file.
               os.remove( file_path )
               
         # Remove the formatted directory.
         os.removedirs( 'formatted' )
         
      # End: if test_only :
      
   # End: if os.path.isdir( 'formatted' ) :
   
   # Move back to the original directory.
   os.chdir( original_dir )
   
   return

#
# Call the main function.
#
main()
