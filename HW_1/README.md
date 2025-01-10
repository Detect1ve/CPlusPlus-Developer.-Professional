# 1. Building and publishing a package
**Creating a build farm that will automatically build our projects and run tests using GitHub Actions.**
### What needs to be done?
Write a program that prints two lines to the console:
```cpp
build N
Hello, World!
```
Where the current build number should be displayed instead of N. Run tests at the build stage to check the validity of the version number. Upload the source code to the GitHub repository.

Set up a [Github Actions workflow](https://docs.github.com/en/actions) to build the project automatically for each commit. Also, set up an automatic release for each commit or upon tag creation. Due to recent [security policy updates](https://github.blog/security/application-security/token-authentication-requirements-for-git-operations/), you will need to [create an access token](https://docs.github.com/en/authentication/keeping-your-account-and-data-secure/managing-your-personal-access-tokens) to work with the repository from the console.

**Options for organizing source materials for independent work:**
- separate repository for each work
- a separate branch in the same repository for each job
- a separate directory in the same branch of the same repository
### **Self-control**
- the package version increases from build to build
- The current version is displayed in the welcome message
- the `helloworld` package containing the `helloworld` executable has been published as a release in the repository
### **Verification**
The task is considered completed successfully if, after installing the package downloaded from the release:
```cpp
apt update && apt install -y helloworld-0.0.X-Linux.deb
```
(instead of X – build number), run the binary file:
```cpp
helloworld_cli
```
A message appeared:
```cpp
Version: X
Hello, World!
```
(instead of X – again, the build number).
- **Additional exercise**

  Add a unit test that completely replicates the test_version.cpp test proposed in the webinar materials, but implement it using the [GoogleTest framework](https://github.com/google/googletest). GoogleTest should either be connected to the repository as a submodule and built together with the test, or installed at the stage of preparing for the build via Github Actions.
