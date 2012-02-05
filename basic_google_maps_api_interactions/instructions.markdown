Author: Robert Pyke

Setup
========

Initial Directory Setup
--------------------------

Setup your environment based on the tutorial: basic_jquery_jquery_ui_setup

You should have a directory structure as follows:

    public/
      index.html

      images/

      stylesheets/

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

Your index.html should have the following `<head></head>`:

```html
<head>
  <script type="text/javascript" src="javascript/jquery-1.7.1.js"></script>
  <script type="text/javascript" src="javascript/ui/jquery-ui.js"></script>
  <script type="text/javascript" src="javascript/test.js"></script>

  <link rel="stylesheet" href="javascript/themes/base/jquery-ui.css" type="text/css" media="all">
</head>
```

Google Maps API Key
=====================

Before we can make use of the google maps api, we need to get a google maps api key.

Go to https://code.google.com/apis/console/

You need to create a project. Name it whatever you like.

Now that you've created a project, select the "services" tab, and enable the "Google Maps API v3" service.

Now select the "API access" tab, take note of your "Simple API Access, API key"

API Key Initial Test
-----------------------

It can take a few minutes for your API key to become active.

In the meantime, let's do the google maps api Hello World example http://code.google.com/apis/maps/documentation/javascript/tutorial.html#HelloWorld

Change your index.html file to be as follows:

```html

<!DOCTYPE html>
<html>
  <head>
    <title>Google Maps Hello World</title>
    <script type="text/javascript" src="javascript/jquery-1.7.1.js"></script>
    <script type="text/javascript" src="javascript/ui/jquery-ui.js"></script>
    <link rel="stylesheet" href="javascript/themes/base/jquery-ui.css" type="text/css" media="all">

    <meta name="viewport" content="initial-scale=1.0, user-scalable=no" />
    <style type="text/css">
      html { height: 100% }
      body { height: 100%; margin: 0; padding: 0 }
      #map_canvas { height: 100% }
    </style>
    <script type="text/javascript"
      src="http://maps.googleapis.com/maps/api/js?key=YOUR_API_KEY&sensor=SET_TO_TRUE_OR_FALSE">
    </script>
    <script type="text/javascript">
      function initialize() {
        var myOptions = {
          center: new google.maps.LatLng(-19.32585, 146.756654),
          zoom: 16,
          mapTypeId: google.maps.MapTypeId.HYBRID
        };
        var map = new google.maps.Map(document.getElementById("map_canvas"),
            myOptions);
      }
    </script>
  </head>
  <body onload="initialize()">
    <div id="map_canvas" style="width:100%; height:100%"></div>
  </body>
</html>
```

Change `YOUR_API_KEY` to be the api key you got earlier in the tutorial. 

Change `SET_TO_TRUE_OR_FALSE` to either true or false.
* Set it to true if your application uses a sensor from the client device, e.g. GPS
* Set it to false otherwsie.
For this tutorial, I will be setting the sensor value to false.

Now load the page. If everything is in place, you should be greeted by a Google Map centred on James Cook University.

For more information about what the different commands to, read http://code.google.com/apis/maps/documentation/javascript/tutorial.html

Moving the map
====================

Let's create a new html file called move_map_randomly.html. Put it in the root of your public folder.

Put the following code in your move_map_randomly.html file.

```html

<!DOCTYPE html>
<html>
  <head>
    <title>Move Map Randomly</title>
    <script type="text/javascript" src="javascript/jquery-1.7.1.js"></script>
    <script type="text/javascript" src="javascript/ui/jquery-ui.js"></script>
    <link rel="stylesheet" href="javascript/themes/base/jquery-ui.css" type="text/css" media="all">

    <meta name="viewport" content="initial-scale=1.0, user-scalable=no" />
    <style type="text/css">
      html { height: 100% }
      body { height: 100%; margin: 0; padding: 0 }
      #map_canvas { height: 100% }
    </style>
    <script type="text/javascript"
      src="http://maps.googleapis.com/maps/api/js?key=YOUR_API_KEY&sensor=SET_TO_TRUE_OR_FALSE">
    </script>
    <script type="text/javascript">
      var map;

      function initialize() {
        var myOptions = {
          center: new google.maps.LatLng(-19.32585, 146.756654),
          zoom: 4,
          mapTypeId: google.maps.MapTypeId.HYBRID
        };
        map = new google.maps.Map(document.getElementById("map_canvas"), myOptions);

        setInterval( "moveMapRandomly()", 2000); // Run every 2000ms (every 2 seconds)
      }

      function moveMapRandomly() {
        // Look up our current location
        var current_lat_long = map.getCenter();

        var current_lat = current_lat_long.lat();
        var current_long = current_lat_long.lng();

        // Generate new latitude and longitude co-ords
        var latitude = Math.random() * 90;       // -90 to 90 is valid, so generate 0 to 90
        var longitude = Math.random() * 180;     // -180 to 180 is valid, so generate 0 to 180

        var new_location = new google.maps.LatLng(latitude, longitude);       // Create a LatLng object from our new latitude and longitude


        // Set the center of our map to our new location
        map.setCenter(new_location);

        // Set the info paragraph to tell the user the map is moved. Use a highlight effect, to bring it to the user's attention
        $('#info').html("Moved from " + current_lat + ", " + current_long + " to " + latitude + ", " + longitude).effect("highlight", {}, 1000);
      }

    </script>
  </head>

  <body onload="initialize()">
    <p id="info">Moving a Map Randomly</p>
    <div id="map_canvas" style="width:100%; height:100%"></div>
  </body>
</html>
```

This code will move a map randomly.

I'll now run you through how this code works.

We first create a `map` variable. This map variable is outside of a function scope, so it persists across the functions.

The `initialize` function creates a map centered on `latitude`: -19.32585, `longitude`: 146.756654.
Once the map is created, we use the jQuery function `setInterval` to call the `moveMapRandomly` function every 2 seconds.

The `moveMapRandomly` function can be broken down into 4 parts.

1. The `current_lat` and `current_long`  of the map's center is calculated. This is done by first getting the map center via the map's `getCenter` function. Then we look up our current_lat and current_long from the center of the map.
2. Generate new `map` `latitude` and `longitude` co-ordinates. This is done by using javascript's `Math.random` function, which generates a float between 0 and 1. We can then multiply the result of `Math.random` to get a random number in our desired range.
3. We set the map's center to our generated `latitude` and `longitude` values.
4. We update our `info` paragraph to inform the user that we moved the map's center. After we change the html content, we use jQueryUI's `highlight` function to bring the user's atterntion to the change.

Map Markers
====================

Let's create a new html file called map_marker.html. Put it in the root of your public folder.

Put the following code in your map_marker.html file.

```html
<!DOCTYPE html>
<html>
  <head>
    <title>Map Marker</title>
    <script type="text/javascript" src="javascript/jquery-1.7.1.js"></script>
    <script type="text/javascript" src="javascript/ui/jquery-ui.js"></script>
    <link rel="stylesheet" href="javascript/themes/base/jquery-ui.css" type="text/css" media="all">

    <meta name="viewport" content="initial-scale=1.0, user-scalable=no" />
    <style type="text/css">
      html { height: 100% }
      body { height: 100%; margin: 0; padding: 0 }
      #map_canvas { height: 100% }
    </style>
    <script type="text/javascript"
      src="http://maps.googleapis.com/maps/api/js?key=YOUR_API_KEY&sensor=false">
    </script>
    <script type="text/javascript">
      var jcu = new google.maps.LatLng(-19.32585, 146.756654);
      var marker;
      var map;

      function initialize() {
        var mapOptions = {
          zoom: 9,
          mapTypeId: google.maps.MapTypeId.ROADMAP,
          center: jcu
        };

        map = new google.maps.Map(document.getElementById("map_canvas"), mapOptions);

        // The marker will remain hidden until the location is set
        marker = new google.maps.Marker({
          map:map,
          draggable:true,
          animation: google.maps.Animation.DROP // When the marker initially comes into existence, drop it in place
        });

        // Event Listener - map.click
        //
        // The map incurred a single click event
        // Move our marker to location of the click event
        google.maps.event.addListener(map, 'click', function(event) {
          var clicked_lat  = event.latLng.lat();
          var cliecked_lon = event.latLng.lng();

          marker.setPosition(event.latLng);
          $('#info').html("Marker moved because of click event, new lat: " + clicked_lat + ", lng: " + cliecked_lon).effect("highlight", {}, 1000);
        });

        // Event Listener - marker.dragend
        //
        // The marker incurred a drag event.
        // Print the result of the drag event.
        google.maps.event.addListener(marker, 'dragend', function(event) {
          var clicked_lat  = event.latLng.lat();
          var cliecked_lon = event.latLng.lng();

          $('#info').html("Marker moved because of drag event, new lat: " + clicked_lat + ", lng: " + cliecked_lon).effect("highlight", {}, 1000);
        });
      }
    </script>
  </head>
  <body onload="initialize()">
    <p id="info">Click to place a marker</p>
    <div id="map_canvas" style="width:100%; height:100%"></div>
  </body>
</html>
```

This code will allow a user to place a map marker, and will report the latitude and longitude of the placed marker.

I'll now run you through how this code works.

We first create a `map`, `marker` and `jcu` variable. These variables are all outside of a function scope, so that they can persist across the functions. The `jcu` variable is a google maps LatLng instance with latitude and longitude set to James Cook University's location.

The `initialize` function does a various things.

1. It creates a map centered on `jcu`.
2. It initializers the marker variable. The marker is assigned to our map (the one centered on jcu). The marker is set so as to be draggable. The marker's animation is assigned to DROP. This means when the marker is assigned a location for the first time, it will appear to drop into the map.
3. We add an event listener to listen for click events on the map. When the user clicks in the map, the listener will run our function with the click event as a local variable. The click event object contains a latLng value  which is the location the user clicked. We use our listener to set the location of our marker to the location the user clicked. We also use jQuery and jQueryUI to inform the user that the marker was moved due to a click event.
4. We add a `dragend` event listener to listen for the end of drag events on the marker. When this event fires, we use jQuery and jQueryUI to inform the user that the marker was moved due to the user dragging it.

Further Reading
==================

You can read more about the Google Maps javascript API at:

* http://code.google.com/apis/maps/documentation/javascript/tutorial.html
* http://code.google.com/apis/maps/documentation/javascript/basics.html

References
=============

This tutorial makes use of documentation provided via:

* http://code.google.com/apis/maps/documentation/javascript/tutorial.html
