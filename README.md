# MONARCH 1.1
A Web Browser for the modern application
By Dan Whitcomb 2016

## Introduction
Monarch is the product of a senior research project by 
Dan Whitcomb '16 at Dartmouth College for the computer science major. 
It came out of the recognition that web applications require 
a different type of experience than basic web pages. 
Monarch allows any web page to be opened as a desktop application
on Mac OS X 10.9+. Apps are accompanied by custom desktop icons, 
separate windows, and a customizable menu system. 

## Requirements
Monarch has only been tested thoroughly on Mac OS 10.10,
but should be stable onother recent versions of Mac OS X. Just open the app bundle to run

## Build Instructions
Download [GYP](https://gyp.gsrc.io/) and run the setup from the root dir
    
    $ python setup.py install


Download and install [Ninja](https://www.google.com/search?q=ninja&oq=ninja&aqs=chrome..69i57j69i60l3j69i59j69i61.775j0j1&sourceid=chrome&ie=UTF-8#q=Ninja+build), the required build system.


Set the generators for the ninja build:

    $ export GYP_GENERATORS=ninja

If you want to make the Xcode project, then this as well:

    $ export GYP_GENERATORS=xcode-ninja, $GYP_GENERATORS

Finally, create the build files from the Monarch root directory:

    $ cd src/build
    $ ./gyp_chromium
    
### Do the build
The `-C` option makes ninja switch directories before doing anything. GYP puts the build files for both Debug and Release in their respective `out/` directory. All built files will be in its respective directory. Doing an initial build can take hours. Don't try unless you really want to commit that time.

#### Build Debug Version
Run from project root:

    $ ninja -C out/Debug chrome

#### Build Release Version
Run from project root:

    $ ninja -C out/Release chrome


## How to Use
There are three ways to open a page in an app bundle form.
1. Navigate to the desired page, then either right click somewhere on the page, or select 'View' 
in Monarch's menu bar. Then on either menu, select 'Enter App Mode'. The current tab you are on 
will close and the app will open with that tab's page: sessions are not preserved when opening in
app mode, so you will lose any current data on the page
2. You can open an app by tying the URL into the navigation bar. If it is recognized as a valid URL, 
there should also be an option to open that URL as an App, and there will be an icon next to the URL
that says 'App'. Selecting this option will launch that URL in an app.

## Getting a bundle and reading the paper
You can get a bundle that was created on Mac OS 10.10 [here](https://halfbyte.io/projects/). If you want to take the team, the accompanying paper justifiying and explaining the work can be found there.

## Licensing
This is distributed under the FreeBSD License, see LICENSE file in release directory for more information

## DISCLAIMER
This SOFTWARE PRODUCT is provided by THE PROVIDER "as is" and "with all faults." THE PROVIDER makes no representations or warranties of any kind concerning the safety, suitability, lack of viruses, inaccuracies, typographical errors, or other harmful components of this SOFTWARE PRODUCT. There are inherent dangers in the use of any software, and you are solely responsible for determining whether this SOFTWARE PRODUCT is compatible with your equipment and other software installed on your equipment. You are also solely responsible for the protection of your equipment and backup of your data, and THE PROVIDER will not be liable for any damages you may suffer in connection with using, modifying, or distributing this SOFTWARE PRODUCT.

