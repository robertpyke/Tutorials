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

Getting the user's geolocation
====================

The Geolocation API Specification is described in full detail at: http://dev.w3.org/geo/api/spec-source.html

It should be noted, that the API is a draft, and may change in the future.

For basic usage of the API, you should know the following:

Checking Broswer Support
-------------------------

Checking if the user's browser supports the geolocation API is very easy.
<code>navigator.geolocation</code> will return undefined if the browser doesn't support the api.

```javascript
  if (navigator.geolocation) {
    // Supports geolocation api
  } else {
    // Doesn't support geolocation api
  }
```

Asking the user for their position 
-------------------------

When requesting the user's current position, you use the API's getCurrentPosition function. This function takes two callbacks. The first is the callback to run if the user's position could be determined. The second is the callback to run if the user's position couldn't be determined.

Be aware that the user can decline your request for their geolocation. There are also a variety of others errors that can occur when getting the user's position.
The following errors are described in the API:

* UNKNOWN_ERROR (numeric value 0)
    * The location acquisition process failed due to an error not covered by the definition of any other error code in this interface.
* PERMISSION_DENIED (numeric value 1)
    * The location acquisition process failed because the application origin does not have permission to use the Geolocation API.
* POSITION_UNAVAILABLE (numeric value 2)
    * The position of the device could not be determined. One or more of the location providers used in the location acquisition process reported an internal error that caused the process to fail entirely.
* TIMEOUT (numeric value 3)
    * The specified maximum length of time has elapsed before the implementation could successfully acquire a new Position object.

The PositionError interface:

```javascript

 interface PositionError {
    const unsigned short UNKNOWN_ERROR = 0;
    const unsigned short PERMISSION_DENIED = 1;
    const unsigned short POSITION_UNAVAILABLE = 2;
    const unsigned short TIMEOUT = 3;
    readonly unsigned short code;
    readonly DOMString message;
  };

```

Using the getCurrentPosition function:

```javascript

  // Handle an error getting the user's position 
  function errorCallback(geolocation_error) {
    alert("Error: " + geolocation_error.message);
  }

  // Make use of the user's position
  function gotPositionCallback(position) {
  }

  // Ask the user for their position
  navigator.geolocation.getCurrentPosition(gotPositionCallback, errorCallback);

```

Using the user's position
-------------------------
The user's position is defined as:

```javascript

interface Position {
    readonly attribute Coordinates coords;
    readonly attribute DOMTimeStamp timestamp;
  };

```

The Position interface contains Coordinates, which is defined as:

```javascript

 interface Coordinates {
    readonly attribute double latitude;
    readonly attribute double longitude;
    readonly attribute double altitude;
    readonly attribute double accuracy;
    readonly attribute double altitudeAccuracy;
    readonly attribute double heading;
    readonly attribute double speed;
  };

```

In general, the things you will care about is latitude, longitude and accuracy.

The latitude and longitude attributes are geographic coordinates specified in decimal degrees.

The accuracy attribute denotes the accuracy level of the latitude and longitude coordinates. It is specified in meters and must be supported by all implementations.

Putting it all together
-------------------------

```javascript

  // Handle an error getting the user's position 
  function errorCallback(geolocation_error) {
    alert("Error: " + geolocation_error.message);
  }

  // Make use of the user's position
  function gotPositionCallback(position) {
    var users_latitude = position.coords.latitude;
    var users_longitude = position.coords.longitude;
    var accuracy_of_position_in_meters = position.coords.accuracy

    // create a google maps lat lng object
    var lat_lng = new google.maps.LatLng(latitude, longitude);

    // Make use of the lat_lng to create a map, place a marker, etc.
    // ...
  }

  if (navigator.geolocation) {
    // Supports geolocation api
    // Ask the user for their position
    navigator.geolocation.getCurrentPosition(gotPositionCallback, errorCallback);
  } else {
    // Doesn't support geolocation api
    alert("Doesn't support geolocation api");
  }

```

Full Example - Create a map and marker at the user's location
==================

```html

<!DOCTYPE html>
<html>
  <head>
    <title>Google Maps At User Location</title>
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
      src="http://maps.googleapis.com/maps/api/js?key=YOUR_API_KEY_HERE&sensor=true">
    </script>
    <script type="text/javascript">
      // Geolocation API Specification: http://dev.w3.org/geo/api/spec-source.html
      //
      // accuracy:
      //  The accuracy attribute denotes the accuracy level of the latitude and longitude coordinates.
      //  It is specified in meters and must be supported by all implementations.
      //  The value of the accuracy attribute must be a non-negative real number.

      // Process the user's position
      function processPosition(position) {
        // Log the details of the position
        //
        // Note: console.log is not supported in all browsers, and should only be used
        // when developing.
        //
        // NOTE: Always remove uses of console.log before putting code into production.
        console.log("Process position: ", position);

        // Get the latitude and longitude out of the geolocation object.
        var latitude = position.coords.latitude;
        var longitude = position.coords.longitude;
        // Create a google maps lat lng object from the latitude and longitude
        var lat_lng = new google.maps.LatLng(latitude, longitude);

        // Set up the map using the provided lat and lng
        //
        // TODO: Consider determining the zoom level by processing the position's accuracy.
        var zoom = 10;
        setupMap(lat_lng, zoom);
      }

      // Set up the map so that it is centered on 'map_center' and
      // zoomed to a level of 'zoom'.
      function setupMap(map_center, zoom) {
        var myOptions = {
          center: map_center,
          zoom: zoom,
          mapTypeId: google.maps.MapTypeId.HYBRID
        };
        var map = new google.maps.Map(document.getElementById("map_canvas"), myOptions);
        var marker = new google.maps.Marker({
          map:map,
          draggable:false,
          animation: google.maps.Animation.DROP, // When the marker initially comes into existence, drop it in place
          position: map_center,
        });
      }

      // Handle the error by performing an alert
      function handleError(error) {
        alert("Error: " + error.message);
      }

      // Handle a geolocation error
      function handleGeoLocationError(error) {
        handleError(error);
      }

      // Handle the fact that the user's browser doesn't support
      // the geo location api.
      function handleGeoLocationUnsupported() {
        handleError(new Error("It appears that your browser doesn't support the geolocation API"));
      }

      // Ask the user's browser for the user's location
      function initialize() {
        // If the user's browser supports getting a geo-location
        // then ask them for access to their current location.
        if (navigator.geolocation) {
          navigator.geolocation.getCurrentPosition(processPosition, handleGeoLocationError);
        } else {
          handleGeoLocationUnsupported();
        }
      }
    </script>
  </head>
  <body onload="initialize()">
    <div id="map_canvas" style="width:100%; height:100%"></div>
  </body>
</html>

```

This code determine the user's location, and will center the map on the user's position.
I'll run you through how this code works.
We first check if the user's browser support's the geolocation api.
If the user's browser supports the geolocation api, we attempt to get the user's position.
If we get the position, we process it via the <code>processPosition</code> function.

The <code>processPosition</code> function does the following:

1. It gets the latitude and longitude of the position.
2. It creates a Google LatLng object from the user's position.
3. It creates a map centered at the user's position.
4. It places a marker at the user's position.

Exercise
=========

Try modifying the code so that the map's zoom is set based on the accuracy of the geolocation position.
For example, a geolocation position with accuracy of zoom 40 can be zoomed very closely (as it is accurate to 40 meters).
If the position accuracy is 180000, then you should have a much lower zoom (zoomed out further).

Further Reading
==================

You can read more about the Geolocation API at:

* http://dev.w3.org/geo/api/spec-source.html

References
=============

This tutorial makes use of documentation provided at:

* http://dev.w3.org/geo/api/spec-source.html
