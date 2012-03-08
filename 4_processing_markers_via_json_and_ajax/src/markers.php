<?php
// Set the output type to json
header('Content-type: text/json');

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
