## Contributing to this repository

The master branch (representing Core targets) is periodically refreshed from main AROS repository. This means the more the master branch diverges, the more costly it is to bring back changes from main AROS repository. The divergence is unavoidable, however rules are set in place to keep it under control over time. Additionally rules are introduced to manage cooperation of different projects on repository itself.

**VERSION: 1.2**

### Rules

#### Repository structure

* **Do not delete/move/rename directories without a very good reason.** Doing so, mostly for "clean up" purposes, really brings high value, but always causes conflicts when applying patches. Always check with master branch maintainer before doing so.

* **Avoid deleting/moving/renaming files.** This again causes patch conflicts so be sure to have a good reason to do so. Resolving some compilation problems can be such reason, but "general cleaning up" should not be used as justification.

* **Consider creating pull requests to AROS repository (https://github.com/aros-development-team/) for changes committed to this repository.** This helps keep the divergence under control which is appreciated by master branch maintainer. This is not a hard requirement though.

* **Remember that there are alternatives based on master branch**. Alternatives will try to limit the divergence with master branch by committing as much code as possible to master. This however can give impression that there is unused code in master branch. Always check alternatives before making any actions on such code.

* **Do not use sub-modules and external repository linking.** Sub-modules causes problems with rebase flow and branch switching. The value they give is less than problems they create.

#### Commits / Pushes

* **Do not push with force (-f, --force).** If git reports non forwardable changes, understand what caused this and fix the problem.

* **Avoid merging branches.** Push changes directly to master rather then have branches merged. This keeps a clean and simple to understand repository history.

* **Pull changes with rebase (git pull --rebase)** This way your changes are not merged, but put on top of already pushed commits, which makes it easier to understand repository history.

* **Separate formatting changes.** If you are reformatting code, make it a separate commit so that formatting and actuall code changes are not mixed.

* **Describe 'why' along with 'what' in commit messages.** Describing the reason behing the change, the seen and expected behavior, the bug condition etc. is an immense help to person in future reviewing a change and trying to understand why it is necessary.

#### Code format

* **Use 4 spaces for indentation instead of tab.** Different editors interpret tab differently which over time causes the code to become unreadable when multiple people work on it, each with different tab settings.

* **Use \r\n line ending and ISO-8859-15 file encoding.** Having same settings across different developers avoid unnecessary re-formating changes.

* **Limit line length to 120 characters for code and comments. Limit line length to 80 characters for autodocs.** While 80 characters limits is no longer needed in 2020s, we are keeping it for backward compatibility with autodocs format which can be displayed on older hardware (Amigas).

* **Use D(bug(x)); form, not D(bug(x);) form for single line debug statements.** The recommended form allows quick enabling of selected debug lines just by deleting the 'D' letter.
