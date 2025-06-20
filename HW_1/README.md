# 1. Building and publishing a package
#### Creating a build farm that will automatically build our projects and run tests using GitHub Actions.
### *What needs to be done?*
Write a program that prints two lines to the console:
```
build N
Hello, World!
```
Where the current build number should be displayed instead of N. Run tests at the build stage to check the validity of the version number. Upload the source code to the repository on github.

Set up a [<u>**Github Actions workflow**</u>](https://docs.github.com/en/actions) to build the project automatically for each commit. Also, set up an automatic release either for each commit or upon tag creation. Due to recent [<u>**security policy updates**</u>](https://github.blog/security/application-security/token-authentication-requirements-for-git-operations/), you will need to [<u>**create an access token**</u>](https://docs.github.com/en/authentication/keeping-your-account-and-data-secure/managing-your-personal-access-tokens) to work with the repository from the console.
#### Options for organizing source materials for independent work:
- separate repository for each work
- a separate branch in the same repository for each job
- separate directory in the same branch of the same repository
### *Self-control*
- the package version increases from build to build
- The current version is displayed in the welcome message
- the helloworld package containing the helloworld executable has been published as a release in the repository
### *Verification*
The task is considered completed successfully if after installing the package downloaded from the release:
```
apt update && apt install -y helloworld-0.0.X-Linux.deb
```
(instead of X – build number), run binary file:
```
helloworld_cli
```
A message appeared:
```
Version: X
Hello, World!
```
(instead of X – again, the build number).
### *Additional exercise*
Add a unit test that completely repeats the test_version.cpp test proposed in the webinar materials, but make it using the GoogleTest framework. GoogleTest should either be connected to the repository as a submodule and built together with the test or installed at the stage of preparing for the build via Github Actions.
