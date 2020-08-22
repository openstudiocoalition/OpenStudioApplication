Version 1.0

The OpenStudio Coalition welcomes your contribution to the OpenStudio Application project. Since the OpenStudio Application is open source, user contributions of code, documentation, issues, or any other kind of contribution moves the project forward. The process for submitting contributions and obtaining approval is described below. If you contribute code, you agree that your contribution may be incorporated into the OpenStudio Application and made available under the OpenStudio Application license on the internet or via other applicable means.

Our open source software benefits greatly from the contributions of its user base and the development community. If the contribution that you propose is aligned with the OpenStudio Coalition’s goals and strategy, we would love to hear from you. The contribution process for the OpenStudio Application is composed of three steps:

Send us a consent email

In order for us to distribute your code as part of the OpenStudio Application under the OpenStudio Application license, we’ll need your consent. An email from you acknowledging that you understand these terms and agree to them is all that will be asked of any contributor. The return email from you should include the following text and a list of co-contributors (if any):

I agree to contribute to the OpenStudio Application. I agree to the following terms and conditions for my contributions: First, I agree that I am licensing the copyright to my contributions under the terms of the current OpenStudio Application license. Second, I agree that, in order to conform to any future open source software license(s) under which the OpenStudio Application may be provided, the terms of my copyright license may be modified without any notice to me and without my consent. Third, I hereby grant to the OpenStudio Coalition, to any successor manager and distributor of the OpenStudio Application, and to all recipients of a version of the OpenStudio Application that includes my contributions, a non-exclusive, worldwide, royalty-free, irrevocable patent license under any patent claims owned by me, or owned by my employer and known to me, that are or will be, necessarily infringed upon by my contributions alone, or by combination of my contributions with the version of the OpenStudio Application to which they are contributed, to make, have made, use, offer to sell, sell, import, and otherwise transfer any version of the OpenStudio Application that includes my contributions, in source code and object code form. Fourth, I represent and warrant that I am authorized to make the contributions and grant the foregoing license(s). Additionally, if, to my knowledge, my employer has rights to intellectual property that covers my contributions, I represent and warrant that I have received permission to make these contributions and grant the foregoing license(s) on behalf of my employer.

Once we have your consent on file, you’ll only need to redo it if conditions change (e.g. a change of business or employer).


Scope agreement and timeline commitment

If your contribution is small (e.g. a bug fix), simply submit your contribution via a GitHub pull request. If your contribution is larger or more involving (e.g. a new feature or an enhancement), we’ll need to evaluate your proposed contribution. To do that, we need a written description of why you wish to contribute to the OpenStudio Application, a detailed description of the proposed project, the precise new and/or enhanced functionalities that you plan to implement as part of the project, and a timeframe for implementation (template contribution proposal document). After we review your materials, we will get in touch via email, or if needed we will schedule a call to discuss in more detail. We may ask you to revise your materials and make changes to it, which we will re-review. Before you get started on any work, we must reach prior agreement and written approval on project areas, scope, timeframe, expected contents, and functionalities to be addressed – this to make sure we are all on the same page and you don’t waste time.


Technical contribution process

We want the OpenStudio Application to adhere to our established quality standards. As such, we ask that you follow our Agile development process - particularly with respect to coding standards, code review, unit tests, and code coverage. These items are explained further below. Smaller, non-code contributions may not require as much review as for code contributions, but all contributions must be reviewed. Code contributions will initially be in a source control branch, and then will be merged into the official OpenStudio Application GitHub repository after review, test and approval. Any bugs, either discovered by you, us, or any users will be tracked as GitHub issues. We request you that you take full responsibility for correcting bugs introduced by the contribution. Be aware that, unless notified otherwise, the correction of bugs takes priority over the submission or creation of new code.


Release Schedule - Contributions should be aligned with the OpenStudio Application release schedule. There are cutoff dates after which new code contributions are not allowed to proceed in the next upcoming release. We call these dates “code freeze”. However, these “frozen” contributions may be re-targeted for the following public release.

Agile Development Process - Contributions may be tracked and planned as Agile user stories, tasks and/or issues. Our Agile development cycle (called iteration) is two weeks long, and we may periodically reach out for progress and completion status of your contribution, according to the timeline and cutoff dates for the project.

Coding Standards - We will point you to coding guidelines to help you with writing OpenStudio code. Although these standards are not complete or detailed, they should give you an idea of the style that we would like you to adopt. Please also look at the OpenStudio Application code on GitHub as a living example of the preferred coding style.

Code Reviews - You will be working and testing your code in a branch. When a piece of functionality is complete, tested and working, let us know (via a pull request) and we will review your code. If the functionality that you contributed is complex, we may ask you for a written design as well. We want your code to follow coding standards, be clear, readable and maintainable, and of course it should do what it is supposed to do. We will look for errors, style issues, comments (or lack thereof), and any other issues in your code. We will inform you of our comments and we expect you to make the recommended changes. New re-reviews may be expected until the code complies with our required processes.

Unit Tests - We ask that you supply unit tests along with the code that you have written. A unit test is a program that exercises your code in isolation to verify that it does what it is supposed to do. Your unit tests are very important to us. First, they give an indication that your code works according to its intended functionality. Second, we execute your unit tests automatically through our Continuous Integration (CI) framework along with our unit tests to verify that the overall OpenStudio Application code continues to work according to expectations.

Code Coverage - We require that your unit tests provide comprehensive coverage of the source code you are submitting. You will need to design your unit tests in a way that all critical parts of the code (at least) are tested and verified. You should strive for 90% coverage or greater. There are tools to help you measure and analyze code coverage.

Documentation - Proper documentation is crucial for our users, without it users will not know how to best use your contribution, its features and limitations. We require that you create user documentation so that end users know how to use your new or enhanced functionality. You will also need to provide us with a short summary of the contribution, to include in the release notes.

For further questions or information:
The OpenStudio Coalition
osc@openstudiocoalition.org

OpenStudio® is a registered trademark of the Alliance for Sustainable Energy LLC, the operator and manager of the National Renewable Energy Laboratory for the U.S. Department of Energy.
