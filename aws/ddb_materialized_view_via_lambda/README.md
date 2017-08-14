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

Create a "Seattle" map:

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
    "MapId": "OregonMapId",
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
