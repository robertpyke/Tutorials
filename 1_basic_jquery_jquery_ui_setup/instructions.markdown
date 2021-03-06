Author: Robert Pyke

JQuery and JQueryUI Setup
=========================

Purpose
---------

When writing javascript, it is commonplace to use a library such as jQuery.

jQuery provides functionality that developers have come to expect from modern programming languages.

Without using libraries, it can be very difficult to produce code that is cross-browser compatible.

Using jQuery, and the functions that it provides, will stop you from reinventing the wheel.

jQueryUI enhances jQuery. jQuery provides mechanical functions that allow you to perfom actions such as manipulating data-structures.
jQueryUI allows you to perform UI specific actions such as highlighting, fading-in and fading-out elements within the DOM.

Directory Structure
====================

Your initial directory structure should look like the following:

    public/
      index.html

      images/

      stylesheets/

      javascript/

Setup index.html
--------------------

For now, put the following inside the index.html file:

``` html
  <!DOCTYPE html>
  <html>
    <head>
      <title></title>
    </head>
    <body>
      <h1>This is the title of my webpage</h1>
      <p>This is the first paragraph of my webpage</p>
    </body>
  </html>
```

Setup jQuery
=============

Download jQuery
-----------------

Go to: http://jquery.com and download the development version of jQuery

You will likely end up at a URL like the following: http://code.jquery.com/jquery-1.7.1.js

Note: The remainder of this tutorial will assume that your jQuery version is 1.7.1

Save this javascript file inside your javascript folder.

Your javascript folder will now look like this:

    javascript/
      jquery-1.7.1.js

Modify html
-------------

Now that you've downloaded jQuery, and placed it in the correct folder, you should be able to point your index.html at it.

In the `<head></head>` section of your index.html, add the following:
    `<script type="text/javascript" src="javascript/jquery-1.7.1.js"></script>`

Now, when a user loads the index.html page of your site, they will download the jquery file, and process it as javascript.

Setup jQueryUI
=================

Download jQueryUI
-------------------

jQueryUI comes in a variety of different themes. For now, we will use the default theme. In future, you should choose a theme that matches the website you're creating.

Go to http://jqueryui.com and download the latest dev bundle.

The link will be something like: http://jquery-ui.googlecode.com/files/jquery-ui-1.8.17.zip

Once you've download the zip, unzip it.

Now move the *contents* of the unzipped folder to your javascript folder.

Note: jQueryUI comes bundled with jQuery, you should use the version of jQuery that comes in the jQueryUI bundle.

Your javascript folder will now look like this:

    javascript/
      jquery-1.7.1.js

      AUTHORS.txt

      GPL-LICENSE.txt

      MIT-LICENSE.txt

      demos/
        ..
      docs/
        ..
      external/
        ..
      tests/
        ..
      themes/
        ..
      ui/
        jquery-ui.js
        ..
      version.txt

Modify html
--------------

Now that you've downloaded jQueryUI, and placed the relevant files in your javascript folder, you should be able to point your index.html at it.

In the `<head></head>` section of your index.html, add the following:

``` html
  <script type="text/javascript" src="javascript/ui/jquery-ui.js"></script>
  <link rel="stylesheet" href="javascript/themes/base/jquery-ui.css" type="text/css" media="all">
```

Make sure you add the jquery-ui javascript script after the jquery javascript.

The head section of your index.html should now look like this:

``` html
  <head>
    <script type="text/javascript" src="javascript/jquery-1.7.1.js"></script>
    <script type="text/javascript" src="javascript/ui/jquery-ui.js"></script>

    <link rel="stylesheet" href="javascript/themes/base/jquery-ui.css" type="text/css" media="all">
  </head>
```

Now when a user loads the index.html page of your site, they will download the jquery and jquery-ui file, and process them both as javascript.
They will also download the default jQuery theme css file.

Testing
==========

To make sure that you have jQuery and jQueryUI set up right, we're going to add our own javascript file (test.js) and use it to change our web page on user interaction.

Modify your index.html
------------------------

Change your index.html to have the following

``` html

  <!DOCTYPE html>
  <html>

    <head>
      <title></title>
      <script type="text/javascript" src="javascript/jquery-1.7.1.js"></script>
      <script type="text/javascript" src="javascript/ui/jquery-ui.js"></script>
      <script type="text/javascript" src="javascript/test.js"></script>

      <link rel="stylesheet" href="javascript/themes/base/jquery-ui.css" type="text/css" media="all">
    </head>

    <body>
      <h1>This is the title of my webpage</h1>
      <p id='my_para'>This is the first paragraph of my webpage</p>
      <button id="run">Run..</button>
      <div id="block" class='ui-widget-content ui-corner-all'>
        <h3 class="ui-widget-header ui-corner-all">Test</h3>
        <p>Hello! Goodbye</p>
      </div>

      <div id="dialog" title="Basic dialog">
        <p>WELCOME</p>
      </div>

    </body>

  </html>
```

* We've added test.js to our included scripts.
* We've given our paragraph a new id.
* We've added a button.
* We've created a div, with an inner h3 and p. These have classes which should test out some of jquery-ui functions.
* We've added a dialog.

Create test.js
-----------------

Create a file called "test.js", and put it in your javascript folder.

Put the following in your test.js file:

``` javascript
  $(document).ready(function() {
    $("#my_para").click(function () {
      $(this).effect("highlight", {}, 3000);
    });

    $("#run").click(function(){
      options = {};
      $( "#block" ).toggle( 'fold', options, 500);
    });

    $(function() {
      $( "#dialog" ).dialog();
    });
  });
```

This file makes use of jQuery and jQueryUI to perform some basic animations.

Note: You should always use `$(document).ready` as a wrapper around your functions. This waits until the page is fully rendered to the user before continuing.
If you don't do this, you script may not act correctly, as the DOM will appear to be incomplete.

The `$("#my_para").click` function performs a highlight effect on the paragraph if it is clicked.

The `$("#run").click` function performs toggles the block div. The toggle makes use of the fold animation, which will take 500ms to complete.

The last function turns our dialog div into a jQueryUI dialog box.

Performing the test
---------------------

You should now point your browser at your index.html

Assuming everying is going well, you should be greeted by your dialog box.

What Now?
============

Now your ready to use jQuery and jQueryUI to build your web applications.

You should look into the jQuery and jQueryUI docs.

* jQuery Docs:         http://docs.jquery.com/Main_Page
* jQuery Tutorials:    http://docs.jquery.com/Tutorials
* jQUeryUI Docs:       http://docs.jquery.com/UI
* jQUeryUI Demos:      http://jqueryui.com/demos/


Google API Hosting
====================

Google provides free hosting of jQuery, jQueryUI and other commonly used libraries.

In this tutorial, you downloaded the necessary javascript files, and stored them locally.
You could of instead passed a non-local src, such as: https://ajax.googleapis.com/ajax/libs/jquery/1.7.1/jquery.js

You can read more about Google's library hosting at: http://code.google.com/apis/libraries/devguide.html
