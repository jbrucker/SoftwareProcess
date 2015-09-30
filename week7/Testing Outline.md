## Kinds of Tests

### Black-box Testing

System is viewed as a black box.  You test inputs and outputs (or responses),
based on what the system *should* do.  *Should* means the requirements specification.

Within black-box testing there are a few methods you can use:

* Boundary-Value Analysis
If the software functions correctly at boundary values,
it will probably function correctly for values in between.
The "boundaries" are edges or near intersections between equivalence classes.

* Equivalence Partitioning
Separate inputs into classes (equivalence classes) that should
produce identical behavior or similar output.
This avoids wasting time on redundant tests.

* Decision Tables
Tables that record business rules in the program.
A table shows possible input conditions and events.
Each column in the table is a combination of business rules.
Rows are events or test conditions.

For example, for testing ExceedVote you could have a table with
different user classes in columns (admin, voter, guest).
Rows are different actions each user can perform.
A cell in the table is a test case.

* Diabolical Test Cases

Tests designed to break the system. They may use extreme values,
illegal values, or circumvent the usual UI (in web apps).

### White-box Testing

Tests based on knowledge of the source code, and can directly
exercise the API.  The aim of these tests is cover as much of
the source code as possible, including all possible flows.

Ref: http://www.elementool.com/ebook/softwaretestcases.pdf

## Example test case formats

You should use a format that contains the info you need to perform the tests repeatedly,
but avoid including useless data.

Some test cases repeatedly test a single form or component with multiple values,
and other test cases perform a longer series of steps, such as e-shopping.
So, it may be helpful to have a few different formats for describing test data.

**Repeatable** is important. You're test case should specify enough info that someone
else can repeat the test.

1. Test case ID for communicating results. This can be descriptive, like member-login-01.
1. Product being tested, including version.
1. Test environment and tools. If persistent storage (database) or external service is involved, you need to specify those, too. For testing, real services are often replaced by **mock objects**, an in-memory or test database is used so you can reset data before each test.
1. Pre-requisites are related to the test environment and test setup.
1. Steps to perform, and how. For UI testing, the "how" sometimes is important for reproducability.
1. Expected results.

At [SoftwareTestingHelp.com](http://www.softwaretestinghelp.com/how-to-write-effective-test-cases-test-cases-procedures-and-definitions/) they recommend the general format:

**Verify** what you want to test <br/>
**Using** tools, environment, url <br/>
**With** conditions and data <br/>
**To** what is to be done, shown, or returned

**Keep test cases simple and easy to understand". Don't write long explanations.

## Web Testing

Things to test in a web application

* Functionality, including forms, links, navigation, cookies
* Performance
* Server interface
* Client compatibility
* Security

## References

There are lots of web sites devoted to testing. Clearly its an important topic!
A few are:

* [SoftwareTestingHelp](http://www.softwaretestinghelp.com/how-to-write-effective-test-cases-test-cases-procedures-and-definitions/)
* [TestingEducation.org](http://www.testingeducation.org) has free articles, lessons, and courses on testing.  Noteworthy:
    * [Blackbox testing by Cem Kaner](http://www.testingeducation.org/coursenotes/kaner_cem/cm_200204_blackboxtesting/)
    * [Rapid Testing by James Back](http://www.testingeducation.org/coursenotes/bach_james/cm_200204_rapidtesting/)

Good article by Cem Kaner (author of *Lessons Learned in Software Testing* and *Testing Computer Software*):
* [What is a Good Test?](http://www.kaner.com/pdfs/GoodTest.pdf) (2003).


## Test Case Forms

* http://readyset.tigris.org/nonav/templates/test-case-format.html (ReadySet has forms and guidance for 
many project activities).

