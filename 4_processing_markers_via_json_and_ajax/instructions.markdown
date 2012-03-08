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

Make sure you have a Google Maps API Key. How to obtain one was explained in the tutorial: basic_google_maps_api_interactions.

Description/Purpose
====================

In this tutorial, you'll be making use of Ajax. You'll be processing server side data asynchronously.
You'll be requesting marker location information from the server via a JSON interface. You'll then process the JSON response, and
place markers based on the content of the response.

Server Side JSON
====================

For this tutorial, you're going to need a source for marker information. We're going to use PHP to produce fake marker location data.

Create a file called markers.php, and place it in your public folder (your tutorial specific html folder). This means that the markers.php file should be in the same directory as your index.html file (they should be siblings).

Put the following in the markers.php file:

```php

<?php
// Set the output type to json
header('Content-type: application/json');

$locations = array();

// Generate between 1 and 6 locations
for($i = 0; $i < rand(1,6); $i++) {
  // Generate a location and push it into our locations array.
  $locations[] = array(
    "id" => $i,
    "latitude" => rand(-45, 45),
    "longitude" => rand(-90, 90),
  );
}

// Output the locations as json.
echo json_encode(array("api_version" => "1", "locations" => $locations));

```

The code above does the following:

* It sets the response type to application/json. This tells the browser to process the content as JSON, and not as a normal html file.
* It creates a locations array, which will be populated with marker locations.
* It produces between 1 and 6 marker locations, where each marker location is given a latitude between -45 and 45 and a longitude between -90 and 90. The markers are also assigned an id.
* It outputs the marker locations as part of a JSON array.

In the real-world, it is likely that your marker location data will come from database queries, and the marker location ids will be set based on your primary key.

Client Side JSON Processing
====================

Modify your index.html to have the following content:

```html

<!DOCTYPE html>
<html>
  <head>
    <title>Place Markers via Ajax</title>
    <script type="text/javascript" src="javascript/jquery-1.7.1.js"></script>
    <script type="text/javascript" src="javascript/ui/jquery-ui.js"></script>
    <link rel="stylesheet" href="javascript/themes/base/jquery-ui.css" type="text/css" media="all">

    <meta name="viewport" content="initial-scale=1.0, user-scalable=no" />

    <style type="text/css">
      html { height: 100% }
      body { height: 100%; margin: 0; padding: 0 }
      #map_canvas { height: 100% }
    </style>

    <!-- TODO: Change API_KEY -->
    <script type="text/javascript" src="http://maps.googleapis.com/maps/api/js?key=API_KEY&sensor=false"></script>

    <script type="text/javascript">
      // Global Vars
      var map;

      // Set up the map so that it is centered on 'map_center' and
      // zoomed to a level of 'zoom'.
      function setupMap(map_center, zoom) {
        var myOptions = {
          center: map_center,
          zoom: zoom,
          mapTypeId: google.maps.MapTypeId.HYBRID
        };

        map = new google.maps.Map(document.getElementById("map_canvas"), myOptions);
      }

      // Get the markers from the website,
      // and place each marker on the map.
      //
      // TODO: Change the code so that the marker id is used to represent a single marker.
      // If an update is received with a marker that already exists on the map (id matches an existing marker), move the existing marker 
      //
      // HINT: Use a global Hash/Map to store references to your markers.
      // e.g. var markers = {}   ... etc.
      //      markers[<marker_id>] = <marker_object>;
      function updateMapWithMarkers() {
        // Do an Ajax get request to get the marker locations from the server.
        $.get( ('markers.php'), function(data) {
          // Create a Bounds to capture the area covered by the markers we're
          // processing.
          var new_markers_bounds = new google.maps.LatLngBounds();

          // Iterate over the locations provided by the server.
          $.each(data['locations'], function(index, loc) {
            // Get the marker's id, latitude and longitude
            var loc_id = loc['id'];
            var lat = loc['latitude'];
            var lng = loc['longitude'];

            // Create a LatLng object for the marker's position
            var marker_lat_lng = new google.maps.LatLng(lat, lng);
            // Extend the new_markers_bounds to covr the new marker's position
            new_markers_bounds.extend(marker_lat_lng);

            // Create and place a new marker.
            //
            // TODO: Instead of always creating a new marker...
            // either create or move the marker based on whether or not the marker's id is in use on our map.
            var marker = new google.maps.Marker({
              map:map,
              draggable: false,
              animation: google.maps.Animation.DROP,
              position: marker_lat_lng,
              // Set the hover-over title of the marker
              title: ( "Location id: " + loc_id + ", lat: " + lat + ", lng: " + lng )
            });
          });

          // Now that we've iterated over the server markers..
          // extend the new_markers_bounds to include what the map currently shows.
          new_markers_bounds.union(map.getBounds());
          if(!( map.getBounds().contains(new_markers_bounds.getNorthEast()) && map.getBounds().contains(new_markers_bounds.getSouthWest()))) {
            // Change the map to fit the bounds.
            map.fitBounds(new_markers_bounds);
          }

        });
      }

      // set up the map, and then start our update function
      function initialize() {
        var zoom = 10;
        jcu_lat_lng = new google.maps.LatLng(-19.32585, 146.756654),
        setupMap(jcu_lat_lng, zoom);

        // Update the markers info every X ms
        var delay = 10000;

        // Run the updateMapWithMarkers function every delay ms
        window.setInterval(updateMapWithMarkers, delay);
      }


    </script>
  </head>
  <body onload="initialize()">
    <div id="map_canvas" style="width:100%; height:100%"></div>
  </body>
</html>

````

Please read the code comments for a detailed understanding of what the code does. The following is a summary of what the code does:

* When the page loads, the initialize function is run.
* The initialize function runs the setupMap function, and then starts a timed event to call the updateMapWithMarkers function every 10 seconds.
* The updateMapWithMarkers function performs an Ajax request (using the <code>$.get</code> jQuery function) on the markers.php file we created earlier.
    * Ajax uses an XMLHttpRequest, which has specific limitations that you should be aware of. For example, an Ajax request can only be made against a resource with the same HOST and PORT as the requestor. This means, that we can't use Ajax against another site. See: http://en.wikipedia.org/wiki/XMLHttpRequest#Cross-domain_requests for more info.
* The JSON response is automatically converted into a javascript object. JSON stands for JavaScript Object Notation.
* The locations within the response are iterated over, and a marker is placed for each location in the response.
* The map is expanded to encapsulate the newly created markers.

Exercise
=========

Modify the code so that you have one marker per marker id. If you do an update, and you see an id that you don't yet have a marker for, you should create a marker.
If you see an id that you already have a marker for, you should move the existing marker.
A HINT on how to do this has been left in the source code.


Consider what would happen if the server was overloaded (503 response code). How would this affect the Ajax request. Using the API documentation for the $.get() method, describe how you could handle an unexpected http response code.

Further Reading
=============

HTTP Response Codes:

* http://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html

Ajax:

* http://api.jquery.com/category/ajax/
* http://api.jquery.com/jQuery.get/
* http://en.wikipedia.org/wiki/Ajax_(programming)
* http://en.wikipedia.org/wiki/XMLHttpRequest

JSON:

* http://en.wikipedia.org/wiki/JSON

GeoJSON:

* http://geojson.org/geojson-spec.html#introduction

PHP Docs:

* http://php.net/manual/en/function.json-encode.php
