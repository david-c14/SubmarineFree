# Changelog

## v2.0.0
- 11-Oct-2021 Port shared components to v2
- 11-Oct-2021 Port Blanking plates to v2
- 14-Oct-2021 Port LA-108 to v2
- 15-Oct-2021 Port AG-xxx OG-xxx NG-xxx XG-xxx LD-xxx and PG-xxx to v2
- 15-Oct-2021 Persist DS suite voltage range between devices. When voltage is changed, new devices use the same option
- 15-Oct-2021 Port BB-xxx DN-xxx FF-xxx OA-xxx to v2
- 25-Oct-2021 Port Text devices and Wiremanager

## v1.1.10
- 10-Dec-2020 Update images in manual
- 26-Nov-2020 Add exponential response to VM-needle
- 26-Nov-2020 Fix bug rendering WM-102 in module browser

## v1.1.9
- 23-Nov-2020 Add DS-2 series polyphonic logic gates
- 22-Nov-2020 Add OA-1xx Operation Amplifiers
- 22-Nov-2020 Add visuals menu to scopes (missing since addition of "Match Cable Colors")
- 20-Nov-2020 VM-1xx and VM-2xx VU Meters

## v1.1.8
- 12-Sep-2020 Bug fix #145 EN-104 inputs

## v1.1.7
- 09-Sep-2020 Bug fix #141 TD-3/4/5 minimum width
- 05-Sep-2020 Add EN-104 Envelope generator
- 30-Jul-2020 Extend time-base of HS-101 down to as short as 1/1000 of a second
- 30-Jul-2020 Extend time-base of LA-108 to as long as 128 seconds
- 24-Jul-2020 Bug fix for MZ-909 manual. Thank you Ryan Monro.

## v1.1.6
- 19-Jul-2020 Added MZ-909 Masterizer
- 18-Jul-2020 Correct urls that refer to master
- 15-Jul-2020 Make devices work with Modular Fungi LightsOff
- 04-Jul-2020 Bug fix - #127 BP-102 incorrect behaviour
- 16-May-2020 WM-102 persist data in Module not in Widget
- 12-May-2020 Deal with warning over hidden send methods in torpedo
- 09-May-2020 Correct use of integer abs to float fabs
- 09-May-2020 Move from deprecated SVGWidget to SvgWidget

## v1.1.5
- 07-May-2020 Issue #116 - Sum Polyphonic inputs for AO-XXX and XF-XXX
- 30-Apr-2020 TD-510 Vertical multiple label textbox
- 30-Apr-2020 TD-410 Add font size option
- 29-Apr-2020 TD-410 Add duplicate label option
- 29-Apr-2020 TD-410 persist data in Module not in Widget
- 25-Apr-2020 TD-202 persist data in Module not in Widget
- 25-Apr-2020 TD-116 persist data in Module not in Widget
- 24-Apr-2020 TD-316 persist data in Module not in Widget
- 23-Apr-2020 TD-410 Make resizeable
- 23-Apr-2020 TD-316 Make resizeable
- 15-Apr-2020 WM-101 persist width in Module not in Widget
- 14-Apr-2020 HS-101 Trigger output pulse width changed to 1ms

## v1.1.4
- 06-Apr-2020 Tidy plugin manifest to suit library
- 02-Apr-2020 Add HS-101 Hi-Res Storage Scope
- 01-Apr-2020 Add TD-410 multiple label textbox
- 01-Apr-2020 Add bias compensator to XF-XXX cross faders
- 01-Apr-2020 Tidy up filenames
- 01-Apr-2020 Tidy up source code for XF modules
- 01-Apr-2020 Add XF-302 Small Form Factor Stereo Cross Fader
- 31-Mar-2020 Make WM-102 billboard automatically track WM-101

## v1.1.3
- 30-Mar-2020 Add WM-102 Wire Manager Billboard
- 30-Mar-2020 Fix memory leak and crash in WM-101 color labels

## v1.1.2
- 25-Mar-2020 Jim Allman added names to colors - and billboard view
- 21-Mar-2020 Jim Allman added some popular color conventions as collections
- 21-Mar-2020 Add undo to load collections
- 21-Mar-2020 Bug fix - Undo on move collection down was not working
- 18-Mar-2020 Issue #80 - Save and load collections in WM-101
- 17-Mar-2020 Add LT-116 Linear Transformations
- 17-Mar-2020 Bug fix #78 - Color one wire uses stale color
- 28-Jan-2020 Replace Digital Ports (Blue) with Analog (Silver) on EO-102
- 21-Jan-2020 Issue #50 - Protect internal classes with anonymous namespaces

## v1.1.1
- 02-Jan-2020 Secret Santa 2019 is now obsolete
- 02-Jan-2020 Make WK-XXX case insensitive when searching for Scala files
- 24-Dec-2019 Add Smooth Noise SN-101 module
- 19-Dec-2019 Two typos in the manual kindly corrected by Darío Hereñú
- 17-Dec-2019 Improve Manual for DO-1xx devices
- 07-Dec-2019 Make DO-1xx connectors scrolling consistent between connectors and across devices
- 07-Dec-2019 Bug fix #54 - DN-112 does not produce maximal sequence
- 03-Dec-2019 Remove unnecessary base constructor calls

## v1.1.0
- 26-Nov-2019 Add Secret Santa 2019
- 26-Nov-2019 Add DO-1xx Digital Operator Devices
- 17-Nov-2019 Fix module id on LA-108 front panel
- 27-Oct-2019 Fix typo in manual (courtesy of kant)
- 22-Oct-2019 Bug fix #48 - Conflicting class names
- 19-Oct-2019 PO-xxx devices moved to SIMD code
- 19-Oct-2019 Relicenced to GPL-3.0
- 18-Oct-2019 Bug fix - Color 1 wire not working responding to existing cables
- 25-Sep-2019 Bug fix - Scopes should not have transparent cables when matching colors

## v1.0.4
- 21-Sep-2019 Add Shift-F keys for WM-101 selection
- 20-Sep-2019 Add Ctrl-F keys for WM-101 collections
- 15-Sep-2019 Add TF-102 and TD-316 devices
- 15-Sep-2019 Add expander backplane to TF-1xx and TD-1xx devices
- 14-Sep-2019 Bug fix #40 - TD-202 Text difficult to erase
- 03-Sep-2019 EO-102 Add menu option to match trace colors to cables
- 03-Sep-2019 LA-108 Add menu option to match trace colors to cables
- 03-Sep-2019 WM-101 Add color collections
- 25-Aug-2019 WM-101 Add ability to color a single wire
- 17-Aug-2019 WM-101 Allow redo cable changes to maintain original color
- 10-Aug-2019 WM-101 Bug fix inactive WM-101 not shifting other modules when minimized
- 06-Aug-2019 WM-101 Add undo to all functional changes
- 01-Aug-2019 WM-101 Add text editor for hex colors
- 31-Jul-2019 Add parameter labels to all parameters
- 31-Jul-2019 WM-101 Add tooltips to sliders

## v1.0.3
- 30-Jul-2019 WM-101 change menu text from Delete... to Delete Wire... for clarity
- 30-Jul-2019 Bug fix #38 - WM-101 in module browser was causing wire highlighting. Fixed
- 27-Jul-2019 Change WhiteLight to RGBLight to avoid conflict with new WhiteLight defined in API
- 27-Jul-2019 WM-101 Add Undo action for Recolor All Wires
- 27-Jul-2019 Added changelog

## v1.0.2
- 20-Jul-2019 Add WM-101

## v1.0.1
- 05-Jul-2019 PO-xxx Deprecated code brought up to date 
- 05-Jul-2019 Bug fix #34 - EO-102 and LA-108 cont/once switch is unstable. Fixed
- 05-Jul-2019 Bug fix #33 - DN-112 manual missing. Fixed
- 05-Jul-2019 Bug fix #32 - SS-xxx devices outputting 0v. Fixed

## v1.0.0
- 04-Jul-2019 Release of completed port to VCV Rack v1
