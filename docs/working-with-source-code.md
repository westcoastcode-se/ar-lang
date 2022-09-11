# Working with the Source Code

## Git Commits

All changes has to have a matching issue. Commit messages must start with a reference (`#<issue_id>`) to the issue. 

Committing without an issue id is allowed if you are working inside your own branch and you squash the content of that branch before
creating a pull-request or merge it into master

## Merging

Branches has to be squashed before merging into the master branch. This can be done using:

```bash
git checkout branch_name
git reset $(git merge-base master $(git branch --show-current))
```

You must then force push the squashed branch before creating a pull request (or merge with master)

