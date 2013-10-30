#!/usr/bin/env python
# svn merge-tool python wrapper for meld
# to use: edit ~/.subversion/config and uncomment the line merge-tool-cmd =
# and set the path to your command

# Note that when a conflict occurs, you will be prompted what to do with it. You need to type a single 'l' and for svn to run this script. When you've finished your merge, you need to type an 'r' to resolve the conflict and copy the merged version to the working copy.

import os, sys
import subprocess
import shutil

try:
   # path to meld
   meld = "/usr/bin/meld"

   print "Set the file paths for mine, theirs, and merged\n"
   print "When you've finished your merge, you need to type an 'r' to resolve the conflict and copy the merged version to the working copy\n"

   # the base or "left" revision version of the file you're merging with
   base   = sys.argv[1] 

   # this is "later" or "right"revision version of the file you're merging with
   theirs = sys.argv[2] 
   
   # this is my version, recommandations: sync your copy to the latest version 
   mine   = sys.argv[3]
   
   #this starts out as a copy of 'mine' (your working copy), and this is where you want the final results of the merge to end up.
   merged = sys.argv[4] 

   print "Calling meld: the order of the files is (merged, theirs, and mine)"
   # all 4 versions: worke right-to-left, reslut being stored in the left file 
   #   cmd = [meld, mine, base, theirs, merged]
   # only 3 versions
   cmd = [meld, merged, theirs, mine]

   # Call meld, making sure it exits correctly
   subprocess.check_call(cmd)
except:
   print "Oh, an error!\n"
   sys.exit(-1)
