DDB Materialized View Via Lambda
=================================

Through this tutorial, we'll learn how to generate a materialized view of 
data in a DynamoDB table. In this tutorial, we'll achieve this by listening for 
writes on a DynamoDB table, and when that write occurs, we'll write through 
to a summary table (our materialized view). This materialized view is effectively 
a pre-compute. Users will be able to read the summarized view, without re-calculating
its state based on the origin table.


This tutorial will describe how to generate a materialized view; we'll use a 
DynamoDB stream to trigger a Lambda, which will generate a roll-up view.


![DDB Materialized View Flow](https://cdn.rawgit.com/robertpyke/Tutorials/9b0f37f8/aws/ddb_materialized_view_via_lambda/PointCategoryRollUpTutorial.svg "DDB Materialized View Flow")


Overview
-----------------------

In this tutorial, we'll be representing many maps of "points of interest".
Let's say these maps are the "places of intereset" for
different cities. Assume that the "points of interest" are crowd sourced, and
having eventually consistent (or even slightly incorrect) summary data about 
our maps is fine (I'll describe why this is relevant later).

The following UML describes the Map to Point (one-to-many) relationship:

![MapToPointUML](https://cdn.rawgit.com/robertpyke/Tutorials/9d4c812e/aws/ddb_materialized_view_via_lambda/MapToPointUML.svg "MapToPointUML")

We'll be using DynamoDB to represent these objects.

A Map has a mapId (our unique identifier).

A Point is on a map (so it references the map).
A point will have a category associated with it.
For example: "Restaurant", or "Landmark".

The Map has some summary data about the points on it (e.g. how many points are 
associated with the map). That data (the summary about points), 
is our materialized view. We're going to store it back on the Map, but 
you could also write it to a separate table.

We're going to be using AWS to represent this overall flow.
Representing our infrastructure in code should be considered "best practice".
We'll be using CloudFormation to represent our infrastructure.
This allows us to review and track(audit) our infrastructure changes. It 
also makes it easier to roll-back to a previous infrastructure state.


CloudFormation Template
--------------------

You can find the architecture CF stack for this tutorial as a sibling file to
this README (cf.json).

The following is a graphical view of the Cloudformation template:

![CF Template](https://cdn.rawgit.com/robertpyke/Tutorials/af345960/aws/ddb_materialized_view_via_lambda/CF%20Template.png "CF Template")

We'll step through each part of the provided cloud formation template.

**The Map Table**:

```javascript

"Resources": {
    "MapTable": {
      "Type": "AWS::DynamoDB::Table",
      "Properties": {
        "TableName": "Map",
        "AttributeDefinitions": [
          {
            "AttributeName": "MapId",
            "AttributeType": "S"
          }
        ],
        "KeySchema": [
          {
            "AttributeName": "MapId",
            "KeyType": "HASH"
          }
        ],
        "ProvisionedThroughput": {
          "ReadCapacityUnits": "5",
          "WriteCapacityUnits": "5"
        }
      },
      "Metadata": {
        "Comment": "This resource defines our top-level Map table (A map will have many points)."
      }
    },...
```

The Map Table is represented by a AWS::DynamoDB::Table resource. [Docs are available describing this resource](http://docs.aws.amazon.com/AWSCloudFormation/latest/UserGuide/aws-resource-dynamodb-table.html).

The resource is named "MapTable". We will use this key to reference this resource
in other resources. For examples, we will need to provide our Lambda write access
to this table.

We're using a fixed Table Name, "Map".

As DynamoDB is schemaless, we only need to provide information about the attributes
used in keys. In this case, we're stating that MapId is a String (S), and that it is the HASH.
This table has no Range key. 

We're not using any secondary (LSI/GSI) indices on this table.

We're providing 5 Read/Write units for the table.


**The Point Table**:

```javascript

{
    "PointTable": {
        "Type": "AWS::DynamoDB::Table",
        "Properties": {
            "TableName": "Point",
            "AttributeDefinitions": [
            {
                "AttributeName": "MapId",
                "AttributeType": "S"
            },
            {
                "AttributeName": "PointId",
                "AttributeType": "S"
            }
            ],
                "KeySchema": [
                {
                    "AttributeName": "MapId",
                    "KeyType": "HASH"
                },
                {
                    "AttributeName": "PointId",
                    "KeyType": "RANGE"
                }
            ],
            "StreamSpecification": {
                "StreamViewType": "NEW_AND_OLD_IMAGES"
            },
            "ProvisionedThroughput": {
                "ReadCapacityUnits": "5",
                "WriteCapacityUnits": "5"
            }
        },
        "Metadata": {
            "Comment": "This resource defines our Point table (A map will have many points)."
        }
    }
}

```

The Point Table is represented by a AWS::DynamoDB::Table resource. [Docs are available describing this resource](http://docs.aws.amazon.com/AWSCloudFormation/latest/UserGuide/aws-resource-dynamodb-table.html).

The resource is named "PointTable".

We're using a fixed Table Name, "Point".

This Table uses MapId as the HASH (partition-key), and PointId as the RANGE.

We're not using any secondary (LSI/GSI) indices on this table.

We're providing 5 Read/Write units for the table.

We're defining a stream for this table. The stream will provide the last 24 hours of updates for the table.
It's effectively a managed, less-configurable, kinesis stream. In this case,
we're indicating that each record in the stream should contain the old and new image.
The old image is the state before a change, and the new image is the state after the change.
For example, if a record is new, the old image will be null. If the record is deleted, the new image will be null.
If the record is updated, the old image will be the initial state, and the new image will be the updated state.
[The official docs contain more info](http://docs.aws.amazon.com/amazondynamodb/latest/developerguide/Streams.html).

**Lambda that generates materialized view**

The PointCategoryRollUp is our Lambda function.

We're going to use it to "roll-up" the summary data to
the Map table, as points are added.

```javascript

    "PointCategoryRollUp": {
      "Type": "AWS::Lambda::Function",
      "Properties": {
        "Handler": "index.lambda_handler",
        "Role": {
          "Fn::GetAtt": [
            "LambdaExecutionRole",
            "Arn"
          ]
        },
        "Code": {
          "ZipFile": { "Fn::Join": ["\n", [
            "from __future__ import print_function",
            "import json",
            "print('Loading function')",
            "def lambda_handler(event, context):",
            "    print('Received event: ' + json.dumps(event, indent=2))",
            "    for record in event['Records']:",
            "        print(record['eventID'])",
            "        print(record['eventName'])",
            "        print('DynamoDB Record: ' + json.dumps(record['dynamodb'], indent=2))",
            "    return 'Successfully processed {} records.'.format(len(event['Records']))"
          ]]}
        },
        "Runtime": "python3.6",
        "Timeout": "25",
        "TracingConfig": {
          "Mode": "Active"
        }
      },
      "Metadata": {
        "Comment": "This resource will execute our roll-up code, which will let us write summary data into the map."
      }
    }

```

The PointCategoryRollUp Lambda function is represented by a AWS::Lambda::Function resource. [Docs are available describing this resource](http://docs.aws.amazon.com/AWSCloudFormation/latest/UserGuide/aws-resource-lambda-function.html). Note, we also need to represent the event sources (triggers) for our Lambda. Those are represented as separate resources.

The resource is named "PointCategoryRollUpTutorial".

Our function's runtime is python 3.6.

The function handler is the function we're expecting to execute within our Python file.

We've enabled tracing. You can read more about XRay Tracing in the official docs (http://docs.aws.amazon.com/lambda/latest/dg/lambda-x-ray.html#using-x-ray).

We've described our function's code inline. (ZipFile, for Python, can be just the code inline). We're just using some sample code to print the event source. You probably wouldn't leave your code inline like this for anything non-trivial.

Our function has a 25 second timeout (for no particular reason).

Our function will execute using the LambdaExecutionRole, which is described later.
All the permissions we need will need to be attached to that role.
At a minimum, our Lambda will need permission to read the source stream,
and write to the Map table. We will also add trace and logging permissions.

Generate the CF Stack
---------------------------

Go to your AWS Account, open the CloudFormation console.

Once at the Cloud Formation console, click, "Create Stack".

Click "Design Template".

At the bottom, choose "Template", you should see JSON like the following:

```javascript

{
    "AWSTemplateFormatVersion": "2010-09-09"
}

```

Copy the content of **cf.json** into the Template (replace the entire contents of the text field).

Now click the refreh in the top-right corner.

You should now see a view like this:

![CF Template Editor](https://cdn.rawgit.com/robertpyke/Tutorials/master/aws/ddb_materialized_view_via_lambda/CF%20Stack%20Image.png "CF Template Editor")

Take a moment to look around in the designer; explore the components. If you edit anything, just remember to change the template back before proceeding.

Now click the "Create Stack" icon. It looks like a cloud with an upload arrow.

You should now be back at the "Select Template" page, but with an S3 url for the template we just created.

Now click "Next" (from the "Select Template" page).

You can name the stack whatever you like. In this case, I'm going to call it: "MapPointRollUp". Click "Next".

We can now provide options for our stack. I'm going to leave these as the default. Click "Next".

We're now on the "Review" page. This stack is going to create an AWS IAM Role. As such, you need to check "I acknowledge that AWS CloudFormation might create IAM resources.". Once you've done that, click "Create".

Now your stack is being created. Click the refresh icon (top right), to see the stack generation progress.

![CF Stack Progress](https://cdn.rawgit.com/robertpyke/Tutorials/c956fa41/aws/ddb_materialized_view_via_lambda/Stack%20Progress%20Image.png "CF Stack Progress")

If everything works as expected, the stack should move to the "CREATE_COMPLETE" status. Now, click on the stack name "MapPointRollUp". This will take you to a detailed view of what the stack has done. Take a moment to explore this view.

At this point, we should see our 2 dynamo tables (Map, and Point), in the Dynamo Console.

Testing our Lambda event (and looking at logs/traces)
------------------------------------------------------

Let's now test the Lambda correctly executes when we the point is updated.

Go to the "DynamoDB" console.

Select "Tables".

We should now see "Map", and "Point".

![DynamoDB Tables](https://cdn.rawgit.com/robertpyke/Tutorials/cf13d567/aws/ddb_materialized_view_via_lambda/Dynamo%20Tables.png "Dynamo Tables")

### Insert Sample Map Records

Click on the "Map" table, and select the "Items" tab.

Click "Create item".

Create a "Seattle" map (use the Text view, instead of the Tree view):

```json
{
  "MapId": "SeattleMapId",
  "City": "Seattle",
  "State": "WA",
  "Country": "US"
}
```

Create an "Olympia" map:

```json
{
  "MapId": "OlympiaMapId",
  "City": "Olympia",
  "State": "WA",
  "Country": "US"
}
```

Create a "Sydney" map:

```json
{
  "MapId": "SydneyMapId",
  "City": "Sydney",
  "State": "NSW",
  "Country": "AUS"
}
```

At this point, we should have 3 maps in our map table.

![Map Records](https://cdn.rawgit.com/robertpyke/Tutorials/9a622b0e/aws/ddb_materialized_view_via_lambda/MapRecords.png "Map Records")

### Insert Sample Point Records

Now we're going to insert some sample Point records.

These will be some points of interest in each of these cities:

Insert "Space Needle":

```json
{
  "PointId": "SpaceNeedleId",
  "MapId": "SeattleMapId",
  "Category": "Landmark",
  "Address": "400 Broad St, Seattle, WA 98109"
}
```

Insert "Palace Kitchen":

```json
{
  "PointId": "PalaceKitchenId",
  "MapId": "SeattleMapId",
  "Category": "Restaurant"
}
```

Insert "Washington State Capitol":

```json
{
    "PointId": "WashingtonStateCapitolId",
    "MapId": "OlympiaMapId",
    "Category": "Landmark"
}
```


Insert "Sydney Opera House":

```json
{
  "PointId": "SydneyOperaHouseId",
  "MapId": "SydneyMapId",
  "Category": "Landmark"
}
```

You should now have 4 points in the Point table:

![Point Records](https://cdn.rawgit.com/robertpyke/Tutorials/master/aws/ddb_materialized_view_via_lambda/PointsRecords.png "Point Records")

### Lambda Console

As we've been inserting point records, our Lambda should have been executing. Recall, it's supposed to execute to process every record change in the Point table. Our default Lambda code should be logging the records we processed. Let's now look at the Lambda console.

Open the "Lambda" console. Select "Functions". We should be able to see our MapPointRollUp function:

![Lambda Console](https://cdn.rawgit.com/robertpyke/Tutorials/9e9a780d/aws/ddb_materialized_view_via_lambda/MapPointRollUpLambdaDashboard.png "Lambda Console")

Now, open the "Monitoring" tab. We should be able to see that our Lambda has executed for the Point uploads. In my case, we've invoked our Lambda exactly 4 times (once per Point added). Note: Lambda can execute a batch of Dynamo event changes, so you may not see exactly 4 invocations.

![Lambda Monitoring](https://cdn.rawgit.com/robertpyke/Tutorials/6d0c3b2a/aws/ddb_materialized_view_via_lambda/MapPointRollUpMonitoringDashboard.png "Lambda Monitoring")

### Lambda Logs

From the monitoring page for our function, click the "View logs in CloudWatch".

We should find a log stream for our Lambda.

Click on the log stream, and view the logs for your Lambda:

![Lambda Logs](https://cdn.rawgit.com/robertpyke/Tutorials/48581644/aws/ddb_materialized_view_via_lambda/MapPointRollUpLogs.png "Lambda Logs")

```javascript

{
   "Records":[
      {
         "eventID":"557c8825da98fd59e1ac0e4db8258c99",
         "eventName":"INSERT",
         "eventVersion":"1.1",
         "eventSource":"aws:dynamodb",
         "awsRegion":"us-east-2",
         "dynamodb":{
            "ApproximateCreationDateTime":1502740560.0,
            "Keys":{
               "PointId":{
                  "S":"SpaceNeedleId"
               },
               "MapId":{
                  "S":"SeattleMapId"
               }
            },
            "NewImage":{
               "Category":{
                  "S":"Landmark"
               },
               "Address":{
                  "S":"400 Broad St, Seattle, WA 98109"
               },
               "PointId":{
                  "S":"SpaceNeedleId"
               },
               "MapId":{
                  "S":"SeattleMapId"
               }
            },
            "SequenceNumber":"100000000000950939886",
            "SizeBytes":128,
            "StreamViewType":"NEW_AND_OLD_IMAGES"
         },
         "eventSourceARN":"XXX"
      }
   ]
}

```
As you can see, the logs contain information about the DynamoDB Stream events. In our case, we can see no "OldImage", but we do see a "NewImage".

Take a moment to explore the logs. Try to understand what code in our Lambda function is generating these logs.

### Lambda XRay

Return to the monitoring page for our Lambda function.

Click the "View traces in X-Ray" link.

These traces show us the call path to our Lambda for requests, and how long it took to execute each piece of the callpath.

Take a moment to look at your traces, and service map. While trivial in this case, they can be useful in more complicated flows, or when additional services, such as API Gateway, are added.

![Example Trace](https://cdn.rawgit.com/robertpyke/Tutorials/5a0b2ebe/aws/ddb_materialized_view_via_lambda/XRayTrace.png "Example Trace")

### Lambda Code

#### Lamdba Print Associated Map
Let's now update the Lambda, so that it prints the map object associate with the point being updated.

```python

from __future__ import print_function
import json
from boto3 import resource
from boto3.dynamodb.conditions import Key

print('Loading function')

dynamodb_resource = resource('dynamodb')
maps = dynamodb_resource.Table('Map')

def lambda_handler(event, context):
    print('Received event: ' + json.dumps(event, indent=2))
    for record in event['Records']:
        print(record['eventID'])
        print(record['eventName'])
        
        # Determine the map associated with this point
        dynamodbRecord = record['dynamodb']
        newImage = dynamodbRecord['NewImage'] 
        mapIdObj = newImage['MapId']
        mapId = mapIdObj['S']
        
        map = maps.get_item(Key={'MapId': mapId})
        print('map: ' + str(map))
        
        # print('DynamoDB Record: ' + json.dumps(record['dynamodb'], indent=2))
        print('MapId: ' + mapId)
    return 'Successfully processed {} records.'.format(len(event['Records']))

```

Our code is a little fragile, but let's run with this first.

Use the following as a test event:

```javascript

{
  "Records": [
    {
      "eventID": "1",
      "eventVersion": "1.0",
      "dynamodb": {
        "Keys": {
          "Id": {
            "N": "101"
          }
        },
        "NewImage":{
           "Category":{
              "S":"Landmark"
           },
           "Address":{
              "S":"400 Broad St, Seattle, WA 98109"
           },
           "PointId":{
              "S":"SpaceNeedleId"
           },
           "MapId":{
              "S":"SeattleMapId"
           }
        },
        "StreamViewType": "NEW_AND_OLD_IMAGES",
        "SequenceNumber": "111",
        "SizeBytes": 26
      },
      "awsRegion": "us-west-2",
      "eventName": "INSERT",
      "eventSourceARN": "XXX",
      "eventSource": "aws:dynamodb"
    }
  ]
}

```

Save and test with this event. We should see the map is logged:

![Lambda Test Response](https://cdn.rawgit.com/robertpyke/Tutorials/168e5aad/aws/ddb_materialized_view_via_lambda/LambdaTestLogs.png "Lambda Test Response")

#### Lamdba Increment Total Points

Let's now update the Lambda function, so that it increments a count of points on the associated map, everytime a point is added.

```python

from __future__ import print_function
import json
import decimal
from boto3 import resource
from boto3.dynamodb.conditions import Key

print('Loading function')

dynamodb_resource = resource('dynamodb')
maps = dynamodb_resource.Table('Map')

def lambda_handler(event, context):
    print('Received event: ' + json.dumps(event, indent=2))
    for record in event['Records']:
        print(record['eventID'])
        print(record['eventName'])
        
        # Determine the map associated with this point
        dynamodbRecord = record['dynamodb']
        oldImage = dynamodbRecord.get('OldImage')
        newImage = dynamodbRecord.get('NewImage')
        
        # Only add a point to the Map summary when a point is added
        # Note: We're not yet handling deletes, but we are skipping updates.
        if oldImage is None and newImage is not None:
            mapIdObj = newImage['MapId']
            mapId = mapIdObj['S']
            print('MapId: ' + mapId)
            
            response = maps.update_item(
                Key={
                    'MapId': mapId
                },
                UpdateExpression="ADD TotalPoints :val",
                ExpressionAttributeValues={
                    ':val': decimal.Decimal(1)
                },
                ReturnValues="UPDATED_NEW"
            )

            print('response: ' + str(response))   

    return 'Successfully processed {} records.'.format(len(event['Records']))

```

We're doing an unconditional update here. We're also performing a relative value update. We're adding 1 to whatever the value is at the time the update is executed. This is an [atomic counter operation](http://docs.aws.amazon.com/amazondynamodb/latest/APIReference/API_UpdateItem.html). This should mean we don't need to worry about optimistic locking scenarios (or another write clobbering our value). That said, we're not remembering if we performed the operation or not. If the Lambda fails after the write (low-risk), it may result in a higher TotalPoint count than we actually have. This technicque is good for things where this margin of error is acceptable. For example, page views, comment counts, etc.

Save and test the updated code.

Take a look at the "SeattleMapId" Map object in the Dynamo Console. You should see the TotalPoints attribute is now visible, and has a count of 1. 


Now let's create a new map, and then add some points to it.

Create a "Townsville" map:

```json
{
  "MapId": "TownsvilleMapId",
  "City": "Townsville",
  "State": "QLD",
  "Country": "AUS"
}
```

And some points in Townsville; as you add each point, take a look at the Map object. You should see it update to reflect the TotalPoints. 

```json
{
  "PointId": "StrandId",
  "MapId": "TownsvilleMapId",
  "Category": "Landmark"
}
```

```json
{
  "PointId": "OldHospital",
  "MapId": "TownsvilleMapId",
  "Category": "Landmark"
}
```

```json
{
  "PointId": "PizzaByTheSea",
  "MapId": "TownsvilleMapId",
  "Category": "Restaurant"
}
```

Once you've added the points, your "TownsvilleMapId" Map should look like this:

```json
{
  "City": "Townsville",
  "Country": "AUS",
  "MapId": "TownsvilleMapId",
  "State": "QLD",
  "TotalPoints": 3
}
```


#### Lamdba Increment Points Per Category

It's nice to have the TotalPoints available directly from the Map, but I would like to have a better understanding of the different types of places we have in each city. Let's modify the roll-up to express the per-category sums.

Specifically, lets add a CategoryCounts property to the Map, that has a key per category, and the count of the points for that category.

```python

from __future__ import print_function
import json
import decimal
from boto3 import resource
from boto3.dynamodb.conditions import Key

print('Loading function')

dynamodb_resource = resource('dynamodb')
maps = dynamodb_resource.Table('Map')

def lambda_handler(event, context):
    print('Received event: ' + json.dumps(event, indent=2))
    for record in event['Records']:
        print(record['eventID'])
        print(record['eventName'])
        
        # Determine the map associated with this point
        dynamodbRecord = record['dynamodb']
        oldImage = dynamodbRecord.get('OldImage')
        newImage = dynamodbRecord.get('NewImage')
        
        # Only add a point to the Map summary when a point is added
        # Note: We're not yet handling deletes, but we are skipping updates.
        if oldImage is None and newImage is not None:
            mapIdObj = newImage['MapId']
            mapId = mapIdObj['S']

            categoryObj = newImage['Category']
            category = categoryObj['S']

            print('MapId: ' + mapId)
            print('Category: ' + category)

            # Default the CategoryCounts to an empty object
            response = maps.update_item(
                Key={
                    'MapId': mapId
                },
                UpdateExpression="SET CategoryCounts = if_not_exists(CategoryCounts, :val)",
                ExpressionAttributeValues={
                    ':val': {}
                },
                ReturnValues="NONE"
            )
            
            # Increment TotalPoints and CategoryCounts
            response = maps.update_item(
                Key={
                    'MapId': mapId
                },
                UpdateExpression="SET TotalPoints = if_not_exists(TotalPoints, :zero) + :one,  CategoryCounts.#category = if_not_exists(CategoryCounts.#category, :zero) + :one",
                ExpressionAttributeNames={
                    '#category': category
                },
                ExpressionAttributeValues={
                    ':zero': decimal.Decimal(0),
                    ':one': decimal.Decimal(1)
                },
                ReturnValues="UPDATED_NEW"
            )

            print('response: ' + str(response))   

    return 'Successfully processed {} records.'.format(len(event['Records']))

```

The code above was updated with two update expresssions. I'll discuss those further now.

```python
            # Default the CategoryCounts to an empty object
            response = maps.update_item(
                Key={
                    'MapId': mapId
                },
                UpdateExpression="SET CategoryCounts = if_not_exists(CategoryCounts, :val)",
                ExpressionAttributeValues={
                    ':val': {}
                },
                ReturnValues="NONE"
            )
```

The above code defaults the CategoryCounts column to {} (an empty object), if it isn't yet set.
If it is set, this action will no-op. We need CategoryCounts to exist before we increment it using the atomic counter.

```python
            # Increment TotalPoints and CategoryCounts
            response = maps.update_item(
                Key={
                    'MapId': mapId
                },
                UpdateExpression="SET TotalPoints = if_not_exists(TotalPoints, :zero) + :one,  CategoryCounts.#category = if_not_exists(CategoryCounts.#category, :zero) + :one",
                ExpressionAttributeNames={
                    '#category': category
                },
                ExpressionAttributeValues={
                    ':zero': decimal.Decimal(0),
                    ':one': decimal.Decimal(1)
                },
                ReturnValues="UPDATED_NEW"
            )
```

The above code now performs two actions, it increments TotalPoints by one, but it also indexes into the CategoryCounts obj, and increments the specific category count. This update leverages the aforementioned atomic counter property.

Note that we can use # to define a key within our object property.

As we add points, our 'per-category' totals will now update.

I'm now going to delete all our Maps and Points, and re-create them.
