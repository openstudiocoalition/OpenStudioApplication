> [Wiki](Home) ▸ **OpenStudio Application's GitHub Workflow for Internal Developers**

## Contents

- [Branching from develop](#branching-from-develop)
- [Cloning the Repository to Your Local Computer](#cloning-the-repository-to-your-local-computer)
- [Creating A Branch](#creating-a-branch)
- [Committing Your Changes](#committing-your-changes)
- [Reintegrating a Branch into Develop](#reintegrating-a-branch-into-develop)
- [Pushing All of Your Local Commits to GitHub](#pushing-all-of-your-local-commits-to-github)
- [Synchronizing Your Fork with NREL/OpenStudioApplication](#synchronizing-your-fork-with-nrelopenstudioapplication)

## Branching from develop

All work should be completed in feature branches created from the _develop_ branch.  Biweekly iterations will be branched from _develop_ to _iteration_, and releases will be branched from _iteration_ to _master_.  No commits or development work should be made to _iteration_ or _master_ unless you are authorized to modify that iteration or release.

[[/images/Using-OpenStudio-with-Git-and-GitHub/workflow.png]]

_Modified from http://nvie.com/posts/a-successful-git-branching-model/_

## Cloning the Repository to Your Local Computer

If you want to download the latest stable release, select the _master_ branch.  Otherwise, if you want to work with the latest development code, use the _develop_ branch:

    git clone -b develop git@github.com:NREL/OpenStudioApplication.git .

The final dot is required if you want to clone into your current directory.  Without the dot, this command will create a directory called OpenStudioApplication and clone into that.  This clone operation will download ~87MB of files and reconstruct the full develop branch within that directory (which will then total ~337MB).  This command also makes all branches available locally.


## Creating a Branch
To create a new local branch and switch to it:

    git checkout -b mynewbranch

To make this branch available to everyone, push the branch to GitHub:

    git push origin mynewbranch

Alternatively, you can do both these steps by typing the new branch name into the branch filter on GitHub and click Create branch:

<!--
    TODO: Image needs to be updated to show OpenStudio Application GitHub Repo.
-->

[[/images/Using-OpenStudio-with-Git-and-GitHub/newbranch.png]]

## Committing Your Changes
To track new files, _and_ to stage modified files for commits:

    git add mynewfile
        or
    git add mymodifiedfile

After creating your branch and making changes, commit all your staged changes and modified/deleted files.  The first line of your commit message should be a very brief description of the commit, followed by more details:

```bash
git commit -m "#3 Bug number & brief commit summary go here (~65 chars)
> More details can go on the additional lines [delivers #12345678]"
```

Adding a `-a` flag to your commit command will automatically commit all modified files, even if you haven't explicitly used `git add` on them.

## Reintegrating a Branch into Develop
If a code review is necessary and your changes are complete, click the [[/images/Using-OpenStudio-with-Git-and-GitHub/compare.png]] Compare button in your branch and follow [GitHub's instructions](https://help.github.com/articles/creating-a-pull-request) for submitting a pull request.  After creating the pull request, you can assign it to the bug or ticket owner for review.

However, if your branch does not require a code review, then it can be merged immediately:

    git checkout develop
    git merge --no-ff mybranch

The no-fast-forward `--no-ff` flag is important for merging to maintain branch history, and it stays consistent with GitHub's automatic merge settings:

[[/images/Using-OpenStudio-with-Git-and-GitHub/no-ff.png]]

_Credit: [nvie](http://nvie.com/posts/a-successful-git-branching-model/)_

## Pushing All of Your Local Commits to GitHub
When you're ready to share your changes and commits from any branch with the rest of the team:

    git push origin

## Synchronizing Your Fork with NREL/OpenStudioApplication
If you want to update your fork with the latest changes from OpenStudio Application's main repository, you will first have to add an upstream remote, and then merge the latest changes to your fork:

```bash
# set the upstream remote once
git remote add upstream git@github.com:NREL/OpenStudioApplication.git

# grab the upstream remote's branches
git fetch upstream

# merge NREL/OpenStudio's develop branch into your fork's develop branch
git checkout develop
git merge upstream/develop
```

If you have made commits to your fork but want to reset it:
```bash
git fetch upstream
git checkout develop
git reset --hard upstream/develop
git push origin develop --force 
```
