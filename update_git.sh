#!/bin/sh

# this is needed only if there was a .git dir someplace it shouldnt have been
#git rm --cached path_to_submodule
#git rm -r --cached *

git add .
git status

git commit -am 'automatic update'
git push -u origin master


