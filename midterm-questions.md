 #### 1. Describe one "top-down" and one "bottoms-up" method of estimating effort in a software project. (No credit for just naming it.)

#### 2. What is the "cost of defects"?

Ref: http://istqbexamcertification.com/what-is-the-cost-of-defects-in-software-testing/

#### 3. (According to TSP) what are the 3 most important factors that a team leader should consider when assigning tasks to team members?

* the person's interests
* the person's capability to perform the task
* opportunity to improve the person's skill/knowledge

Ref: TSP Body of Knowledge, section D4.5

#### 4. Describe the basic work-flow in using test-driven development.

1. write tests for what the component should do. The tests will fail at this step because the component hasn't been written yet.
2. write the code until the tests pass
3. review the code; refactor if necessary.  After any changes you must run the tests again, of course.

Ref: Sterling, Managing Software Debt, page 68, "Test-Driven Development (or Design?)"

#### 5. In RUP, what is the goal of business modeling? At what phase should business modeling be substantially performed?

The goals of business modeling are

* to understand the business structure and dynamics
* ensure that customers, end users, and developers have a common understanding of the organization
* to derive system requirements to support the organization (where the software will be used)

Ref: Kruchten, The Rational Unified Process: An Introduction, 3E, Figure 3-5. Nine Core Disciplines

#### 6. What are some benefits of an iterative process, compared to a linear process like waterfall?

1. risks are uncovered and mitigated earlier
2. change causes less rework and, hence, easier to accommodate
3. more opportunity for learning about the problem domain during development
4. better overall product quality

Ref: Rational Unified Process (IBM), page 7,
http://www.ibm.com/developerworks/rational/library/content/03July/1000/1251/1251_bestpractices_TP026B.pdf

#### 7. What are common problems that configuration management should control?

1. Simultaneous Updates - When two or more workers work separately on the same artifact,
the last one to make changes destroys the work of the former.
2. Limited Notification - When a problem is fixed in artifacts shared by several developers,
and some of them are not notified of the change.
3. Multiple Versions - Most large programs are developed in evolutionary releases.
One release could be in use by customer, while another is in test, and the third is still in development.
If problems are found in any one of the versions, fixes need to be propagated between them.
Confusion can arise leading to costly fixes and re-work unless changes are carefully controlled and monitored.

Ref: (IBM) Rational Unified Process
[page 13] http://www.ibm.com/developerworks/rational/library/content/03July/1000/1251/1251_bestpractices_TP026B.pdf

#### 8. Suppose you write software very carefully, always designing before coding, and constantly review your work. By applying such a careful process, can you produce high quality software without testing?

No. Design and code reviews will find a lot of errors, but no amount of inspection will find them all.
Attempts at formal proof of software correction have mostly failed.  Software needs to be thoroughly tested.
Testing is a part of all engineering disciplines.

William Schach's book on *Classical and O-O Software Engineering* (chapter 6: Testing) has example of this:

In 1969, Naur reported on a technique for constructing and proving a product correct
[Naur, 1969]. Naur demonstrated his technique to prove correctness of an algorithm for a text-processing problem.

A reviewer of Naur's paper pointed out a small error in the algorithm, which would
have been detected by testing. Later, London [1971] found three more
faults in Naur’s procedure. One is that the procedure does not terminate unless a word
longer than maxpos characters is encountered. Again, this fault is likely to have been
detected if the procedure had been tested. London then presented a corrected version
of the procedure and proved formally that the resulting procedure was correct!
Still later, Goodenough and Gerhart [1975] found three
faults that London had not detected, despite his formal “proof.” These included the
fact that the last word is not output unless it is followed by a blank or newline.

All these errors would probably have been detected by thorough testing.

Ref: [5 Reasons We Need Software Testing](http://www.te52.com/testtalk/2014/08/07/5-reasons-we-need-software-testing/)
and [Agile Testing Fundamentals](http://softwaretestingfundamentals.com/agile-testing/)
William Schach, *Classical and O-O Software Engineering*, 8th Ed, section 6.5.2.

#### 9. Why have a coding standard?

Consistently written code, including consistent use of names, formatting of code, and documentation of code has several benefits. First, it makes it easier for others to *use* your code. Second, it makes code more readable by others, which helps *reviews* find defects. Third, it makes maintenance and evolution of code more efficient, as other people can more quickly understand the code.

Other answers are possible.

Ref: http://www.sitepoint.com/coding-standards/

#### 10. Defects: a) Give a definition of a software "defect".

Anything that prevents the software from behaving as specified by the customer, behaving in unintended ways, or giving results different from the customer's specification.


#### b) During what activities are defects injected?

Principally in requirements specification, design, and coding.

#### c) Other than testing, in what activity(ies) are defects detected?

This is easy.

### 11. What software process would be suitable for the project below?  Give a reason for your answer.

In a semester project, 5 SKE students must create a web application that provides information about SKE senior projects.  The customer is a teacher, who meets with students during the first class to discuss requirements.  There are not many requirements and students think they clearly understand what is wanted.
The instructor gives the team 2 weeks to finish the project, and then goes to Japan for 2 weeks.
The students agree to finish in 2 weeks and present their work when instructor gets back.

Refs:
http://www.slideshare.net/MarrajuBollapRagada/agile-vs-iterativevswaterfall
http://pm.stackexchange.com/questions/389/when-to-use-waterfall-when-to-use-scrum

#### 12.  A software inspection or code review involves 5 steps or activities.  Name and briefly describe each one.

  1. Planning - select the reviewers, notify them, and schedule the review meeting.
  2. Preparation - reviewers individually review the software product. They may use guidelines, checklists, or standards.
  3. Review meeting - group inspects the code line by line. If issues are found, they are discussed and author clarifies.  If any reviewer thinks there is an issue or defect, it is recorded.
  4. ____
  5. ____

Ref: Pankaj Jalote, Software Project Management in Practice,  section 10.1

#### 13. What is a software walk-through, and what are the benefits of it?

There are at least 4 objectives or benefits of walk-throughs *besides* finding defects.
According to Pongsachon's question (from perlmonks.org) the benefits include:

* find and fix defects early
* more cost effective than testing (in terms of defect removal)
* good mentoring, training, learning, and team-building activity, esp. for new hires.
* conveys experience to team members.
* makes it easier for one person to maintain another's code (because they understand it)
* improve overall quality of code
* enforce coding standards

Among the benefits, it is an effective way to pass knowledge among developers.  Junior devs (as code reader) can learn from more advanced devs, or (as code author) receive ideas for improvement from others.

Ref: http://www.perlmonks.org/?node_id=776607

#### What is the difference between a walk-through and a software inspection?

A software inspection has the goal solely of finding defects. It is usually conducted more formally and the review of code is *not* led by the code author.  The goal is to find defects, not to fix them or explain the code.

A walk-through has multiple objectives, including to improve the code by finding defects as well as other ways to improve code such as changing the logic, applying design patterns, or refactoring.
Another walk-through goal is to share knowledge.
The code author can share insight from the implementation, and more skilled developers can share knowledge by suggesting improvements to code.
A walk-through is led by the code author (unlike inspection) and it is not required that everyone review the code individually before the walk-through.

#### 14. Which of these characteristics contribute to difficulty of estimating project size? Circle _all_ correct answers.

1. Invisibility of software - progress isn't always visible.
2. Complexity - in general, software development is inherently complex.
3. Marketability - market size and market needs must be analysed.
4. Flexibility - software typically undergoes change in requirements during development.

Ans: 1, 2, and 4.

Ref: http://www.nyu.edu/classes/jcf/CSCI-GA.2440-001/handouts/PlanningProjWithRUP.pdf
page 6 "Estimating Project Size" referencing *The Mythical Man Month* by Fredrick Brooks.

#### 15.1 Describe what is meant by an "ad hoc [software] process" and a "defined process".

An *ad hoc* process is one that is created for a specific project only.  An *ad hoc* process will change
with each project and may not even be completely known at the start of the project.  
In contrast, a defined process is exactly what the name suggestions: a software process
where the activities and outputs are defined in advance, such as Waterfall and RUP.  A defined process can be modified or tailored
for a specific project, but the tailored process is still defined in advance and based on a base defined process.

#### 15.2 Explain how a using a defined process makes a project more predictable and saves time.

* Predictability: with a defined process you have a much better basis for estimating the time and effort needed since you will know all (or most of) the activities you must perform, the correct order to perform them, and have past data as a basis for estimates.  If the process is well-defined it will also be easier to track. Your measurements of progress have more meaning since you typically have a historical basis for comparing them, and assurance that you are tracking all the work.

With an *ad hoc* process, you could still define all the activities at the beginning of project but you'd have a much weaker basis for estimates. Tracking would likely be harder since an ad hoc process is likely to fail to plan for some activities or tasks.

* Saves time: many activities are pretty much the same in every project within an organization. For example, project planning, CM, documentation, and the workflow in an iteration.  With a defined process, you can reuse "process assets", including guides, forms, checklists, and some documents rather than re-invent them for each project. You also avoid wasting time deciding what to do next -- and possibly forgetting some tasks. Less effort is spent learning if you re-apply a defined workflow.

Consider issue tracking, for example. The first time a team uses an issue tracker for tasks and defects they have to establish some conventions for issues, probably will forget some to record some issues, and not make best use of the tracker. With experience they discover an effective way of using issue tracking, and reuse that way.
In effect it becomes part of their defined process.

Watts Humphrey wrote that another benefit of a defined process is that you automate the routine aspects of a project
thereby freeing your creative mind to focus on things that contribute value to the software product.
Humphrey also claims that is improves team communication.  With a defined process you have a clear idea of everyone's roles and responsibilities so communication can be directed effectively, and team members share a common vocabulary.

Ref: *Introduction to the Team Software Process* by Watts Humphrey, Introduction.

#### 16. What are the purpose and goals of Project Monitoring and Control?

* Purpose
    The purpose of Project Monitoring and Control (PMC) is to provide an understanding of the project's progress so that appropriate corrective actions can be taken when the project's performance deviates significantly from the plan.

* Goals
    * Enable project to produce a high-quality product on time.
    * Detect and correct problems related to schedule, progress on product, or failure to meet commitments.

    Refs:
      http://www.tutorialspoint.com/cmmi/cmmi_process_areas.htm, Project Monitoring & Control section.
