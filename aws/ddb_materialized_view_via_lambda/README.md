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

![DDB Materialized View Flow](https://github.com/robertpyke/Tutorials/blob/master/aws/ddb_materialized_view_via_lambda/PointCategoryRollUpTutorial.svg "DDB Materialized View Flow")

