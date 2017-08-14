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

You can find the architecture CF stack for  this tutorial as a sibling file to
this README (cf.json).

CloudFormation Template
--------------------

We'll step through each part of the provided cloud formation template.

First, the Map Table.

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

The resource is named "MapTable". We will use this key to reference this resource
in other resources. For examples, we will need to provide our Lambda write access
to this table.
We're using a fixed Table Name, "Map".
As DynamoDB is schemaless, we only need to provide information about the attributes
used in keys. In this case, we're stating that MapId is a String (S), and that it is the HASH.
This table has no Range key. 
We're not using any secondary (LSI/GSI) indices on this table.
We're providing 5 Read/Write units for the table.

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


![CF Template](https://cdn.rawgit.com/robertpyke/Tutorials/af345960/aws/ddb_materialized_view_via_lambda/CF%20Template.png "CF Template")


