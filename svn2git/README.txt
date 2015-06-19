This file describes briefly the steps that were carried out to migrate from *PUBLIC* OAI SVN repo (http://svn.eurecom.fr/openair4G/trunk) to the gitlab repo (git@gitlab.eurecom.fr:oai/openairinterface5G.git). In order to reduce the size of repository, we have only included SVN commits from r4000 up till now. None of branches/tags are migrated from SVN Repository to Git. 

1. The steps to migrate from SVN to Git were carried out using the instructions/script here, https://www.atlassian.com/git/tutorials/migrating-overview

2. Currently, the repository is public during the "transition period" for everyone to get used to the repository. During the "transition period", we will check in changes only to SVN and synchronize SVN to Git repository once everyday. At the end of "transition period", we will mark SVN repo as read-only and Git repository will be the default working repository. Currently, the "transition period" is set to 1 month until 15 July. However, this date might be changed based on the feedback from the community.There are brief instructions to explain this workflow here, https://www.atlassian.com/git/tutorials/migrating-overview/. 

4. The author list file is located in authors.txt in the main working directory. This file was extracted from SVN logs, but is modified for name and email addresses. 

7. You will need to put your ssh keys in https://gitlab.eurecom.fr/profile/keys to access to the git repo (only important for users who want to commit to the repository)


Migration Instructions in Detail:
Assuming you are in svn2git directory.

wget https://bitbucket.org/atlassian/svn-migration-scripts/downloads/svn-migration-scripts.jar
java -jar ./svn-migration-scripts.jar verify
java -jar ./svn-migration-scripts.jar authors http://svn.eurecom.fr/openair4G > authors.txt
git svn clone -r4000:HEAD --authors-file=./svn2git/authors.txt http://svn.eurecom.fr/openair4G/trunk oaiRepoData1 >& log_svn_clone1.out

(Assuming you have added your ssh keys in https://gitlab.eurecom.fr/profile/keys)
//One time git configuration of user name/email address
git config --global user.name "Replace_With_Your_Name"
git config --global user.email "Replace_With_Your_Email_address"

git remote add origin git@gitlab.eurecom.fr:oai/openairinterface5g.git

git push -u origin --all
git push --tags

Steps to synchronize (to be carried out periodically)
git svn fetch --authors-file=./authors.txt
git svn rebase
git push -u origin --all
git push --tags



