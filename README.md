# OpenStudio User Documentation


This is the repo for OpenStudio user documentation.

The documentation is written in Markdown, and uses [MkDocs] (http://www.mkdocs.org/) to compile the markdown files into a static site.  The site is hosted on github-pages at: http://nrel.github.io/OpenStudio-user-documentation/.

## Setup
### Mac
Install MkDocs by following the directions at [MkDocs](http://www.mkdocs.org); you will need python.

We recommend using mkdocs version 0.11.1, as newer versions have caused issues with this doc site.

To force-install version 0.11.1 of MkDocs on Mac/Linux, run:
```python
pip install -r requirements.txt
```

### Windows
1. Install [Python 3.8](https://www.python.org/ftp/python/3.8.0/python-3.8.0.exe) with the option to `Add Python 3.8 to PATH`
2. Open a command prompt in the source directory and run: `pip install -r requirements.txt`

## Run
Clone this repo locally.  In a terminal window, navigate to the repo directory, and type:
```shell
mkdocs serve
```

On Windows you can also double-click the start_mkdocs.bat file (assuming you have used standard install locations).

This will start a server that will let you see what your changes will look like on the site.  Open a browser and go to http://127.0.0.1:8000/ to see your local site.


## Adding and Editing Pages
Edit existing .md files using either markdown or html.  If you need to add a new image, place it in the `docs/img` directory.

If you need to add a new page, first decide where it should go in the site structure.  The folders within the docs/ directory mostly correspond to the top nav.  Once you have decided where the page should go, add a reference to it in the `mkdocs.yml` file.

## Deploying

When you are done making changes, test your deployment first and then commit back to the repo.

To test, first delete the site directory if it already exists, and then build the site and open index.html
```shell
rm -rf site
mkdocs build
```

If you are ready to deploy your changes to the live site, first **commit your markdown changes to the master branch**.  Then return to the terminal window and type:

```shell
mkdocs gh-deploy
```
This will generate the static site in the `site/` directory.  In a separate repo checkout the gh-pages branch.  Delete all the files in the gh-pages checkout and copy the files from the `site/` directory into the gh-pages checkout.  Add any new files and delete any missing ones from the repo.  Finally push this directory to the gh-pages branch of the repo, which will update the site.  Do not commit the `site/` directory to the master or gh-pages branch, however.
