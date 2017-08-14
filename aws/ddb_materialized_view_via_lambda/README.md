DDB Materialized View Via Lambda
=================================

Through this tutorial, we'll learn how to generate a materialized view of 
data in a DynamoDB table. In this tutorial, we'll achieve this by listening for 
writes on a DynamoDB table, and when that write occurs, we'll write through 
to a summary table (our materialized view). This materialized view is effectively 
a pre-compute. Users will be able to read the summarized view, without re-calculating
its state based on the origin table.


This tutorial will describe how to generate a materialized view, using a 
Dynamo stream trigger and a Lambda to produce a roll-up view.

![DDB Materialized View Flow](https://cdn.rawgit.com/robertpyke/Tutorials/9b0f37f8/aws/ddb_materialized_view_via_lambda/PointCategoryRollUpTutorial.svg "DDB Materialized View Flow")

CloudFormation Template
--------------------

```json

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

