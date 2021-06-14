# fx-project Open Source
[Homepage: fx-project.org](https://www.fx-project.org)

## VERSION
- __Version__\
21.1

- __Release date__:\
2021-06-07

- __Copyright__:\
[FeRox Management Consulting GmbH & Co. KG](https://www.ferox.de), Adolf-Langer-Weg 11a, D-94036 Passau (Germany)

![fx-project Open Source Image](https://www.fx-project.org/fxpos_image.png?v=community)
_(fx-project Open Source Image)_

![Screenshot of fx-project Open Source - Matrix: Graphically as Gantt](https://www.fx-project.org/fxpos_matrix.png)
_(Screenshot of fx-project Open Source - Matrix: Graphically as Gantt)_


---

## DESCRIPTION
fx-project is a free Open Source PHP Project Management Software with powerfull expansion packs. A complete Ultimate Edition has the following main features:
- __Master Data__\
No matter whether client or employee - all data on the parties involved are managed here. Employee roles in the project or their skills complete the whole. The corresponding personnel master data sheet contains all information from vacation to hours worked etc.

- __Project__\
Of course, projects are the highlight of a modern PM system, especially in fx-project with its unique dynamic interface for project planning. Simply use the mouse to plan your project on the calendar, assign the necessary resources with a click and see immediately how busy they are. The workload of the resources for each individual task is graphically displayed and dynamically changed when tasks or subprojects are moved, so that you can immediately find the best time for execution. With this task planner you can quickly define the necessary tasks and resources. Tasks that belong together organizationally or in terms of time are grouped into subprojects, resulting in a tidy project structure.

- __Time Recording__\
Time recording is the core element of fx-project and informs the project manager about the current status of a project. In order to make time recording as effective as possible for the employee, there are various ways of entering data: week/month entry, hourly totals, right through to the convenient time recording app for smartphones. With each recorded time unit, the stored costs are synchronously booked in the project. With the current data the project manager is always informed about the project status. Vacation, illness and other absences are also entered for information purposes.

- __Controlling__\
As a project manager, always being informed with the right numbers is what characterizes a successful project manager. It is necessary to get specific data of projects or project parts with the desired information. Network diagram, work breakdown structure (WBS), flow chart, Gantt chart, milestone trend analysis (MTA), personnel deployment plan - these are just a few examples of the numerous project tools in fx-project. With the right selection fields, the desired reports can be generated quickly. For further processing, these can generally be exported as a spreadsheet or generated as PDF.

- __Documents__\
fx-project has an effective document management system (DMS). Whether general documents, on a project, employee or client. Any type of file can be linked to it. With full versioning, every changed document is saved and can be retrieved at any time. The history log shows in detail who has viewed or changed which document, when and what. New documents are displayed in the control center. So nothing gets lost and everyone is in the picture as far as their rights allow it.

- __Travel Expenses__\
The travel expenses recording function is used for daily trip, receipt, and per diem/flat rate recording. Travel expenses are often incurred in projects. These burden the project with time and money. In fx-project all trips can be recorded and accounted for. Regardless of whether the costs are incurred by the employee or the company. All expenses are directly assigned to a task or project. Travel can be accounted for separately or combined with daily time recording.

- __Administration__\
The necessary technology is stored in the admin area. User profiles can be created individually up to the program level. Access rights, departments, individual configurations, automatic mails, scheduler entries, individual project categories, client settings from currency to date format. Even individual new masks can be created here and fully integrated into fx-project.

- __Interfaces__\
By default, all reporting data can be created as CSV (Comma-Separated-Value), Spreadsheet or PDF.

---
### A detailed listing of all functions and description can be found here:
https://fx-project.org/en/extension-packages.html




---

## INSTALLATION
__Requirements:__

- PHP Version 7.4.x
- Webserver that supports PHP, e.g. APACHE, NGINX or IIS
- Database PostgreSQL or SQLServer

When you first enter the URL in your browser the necessary preparations are done automatically, i.e. gathering initialization informations, creating the tables in your database and fill the tables with basic data.

Full instructions can be downloaded here: [Downloads: fx-project.org](https://www.fx-project.org/en/downloads.html)


---

## VIRTUAL MACHINES
Pre-installed and ready-to-use VMware versions (Desktop/Server) can be downloaded here: [Downloads: fx-project.org](https://www.fx-project.org/en/downloads.html)


---

## THIRD PARTY SOFTWARE: CHARTDIRECTOR
The following third party software should be downloaded separately and integrated into PHP as an __Extension__

- __ChartDirector__\
https://www.advsofteng.com \
V6.3, 2020-01-29 - Advanced Software Engineering Ltd\
\
Chart and Graph Plotting Library\
\
_fx-project uses this PHP extension library to generate and display charts and graph plotting diagrams_\
\
Full instructions can be downloaded here: [Downloads: fx-project.org](https://www.fx-project.org/en/downloads.html)


---

## THIRD PARTY SOFTWARE: FPDF, FPDI, PHPMAILER, PHPSPREADSHEET, TCPDF
The following third party softwares should be downloaded separately and unpacked into the fx-project subfolder for extensions __./EXT__

- __FPDF__\
http://www.fpdf.org \
V1.82, 2019-12-07 - Olivier PLATHEY\
\
Free PDF: A free PHP class for generating PDF's\
\
_fx-project uses this PHP class to convert the generated and displayed HTML reports into a downloadable format (PDF), which can be displayed uniformly and printed on any printer_\
\
_If this PHP class is not available, the PHP class "TCPDF" is used as an alternative, if available_

- __FPDI__\
https://www.setasign.com \
V2.3.4, 2020-08-27 - Setasign GmbH & Co. KG\
\
Free PDF Document Importer: A free PHP class for reading and analysing existing PDF documents\
\
_fx-project uses this PHP class to generate and display a preview of the first page of a PDF document in the DMS (Document Management System)_

- __PHPMailer__\
https://github.com/PHPMailer/PHPMailer \
V6.1.8, 2020-11-07 - Marcus Bointon, Jim Jagielski + Andy Prevost\
\
A full-featured email creation and transfer class for PHP\
\
_fx-project uses this PHP class to create HTML emails, which can be sent via SMTP and a socket connection_

- __PhpSpreadsheet__\
https://github.com/PHPOffice/PhpSpreadsheet \
V1.15.0, 2020-11-11 - Various\
\
A set of PHP classes with which various spreadsheet file formats such as Excel and LibreOffice Calc can be read and written\
\
_fx-project uses this PHP class to convert the generated and displayed HTML reports into downloadable Excel files, which can be loaded, displayed and further processed with appropriate software_

- __TCPDF__\
https://tcpdf.org \
V6.3.2, 2020-01-04 - Nicola Asuni - Tecnick.com LTD\
\
Tecnick PDF: A free PHP class for generating PDF's\
\
_fx-project uses this PHP class to convert the generated and displayed HTML reports into a downloadable format (PDF), which can be displayed uniformly and printed on any printer_\
\
_If this PHP class is not available, the PHP class "FPDF" is used as an alternative, if available_


---

## LICENSE
__GNU-GPL v3__ [License Link](https://opensource.org/licenses/GPL-3.0)

_This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version._

_This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details._

You should have received a copy of the GNU General Public License along with this program.  If not, see [Licenses](https://www.gnu.org/licenses/).


__LICENSING ADDENDUM:__

Programs in the SPP (Special Programs) subfolder are coded extensions of the open source software fx-project. These programs are offered for sale by the manufacturer FeRox Management Consulting GmbH & Co. KG and require a valid key for execution. It is forbidden to resell these programs and/or keys or to pass them on free of charge or use them without the express written permission of FeRox Management Consulting GmbH & Co. KG.


---

## SUPPORT
__WE WISH TO IMPROVE AND EXPAND FX-PROJECT, BUT WE NEED YOUR SUPPORT.__

Please buy one of our powerful expansion packages for fx-project, or if you want to support us more you can make a donation.

[Expansion Packages: fx-project.org](https://www.fx-project.org/en/expansion-packages.html)
