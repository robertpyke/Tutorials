Introduction
============

The most basic use of the twitter API comes in the form of adding a tweet button.
The tweet button allows your users to share your webpage with their followers
on twitter.

The tweet button is explained in great detail at
the following url: https://dev.twitter.com/docs/tweet-button

You can build a custom tweet button for your page using the
following interactive builder: https://twitter.com/about/resources/buttons#tweet

The interactive builder produces some html. The html includes a link, and some
javascript associated with the link.

e.g. example tweet button code:

```html
<a href="https://twitter.com/share" class="twitter-share-button" data-via="robertpyke">Tweet</a>
<script>
    !function(d,s,id){
        var js,fjs=d.getElementsByTagName(s)[0];
        if(!d.getElementById(id)) {
            js=d.createElement(s);
            js.id=id;
            js.src="//platform.twitter.com/widgets.js";
            fjs.parentNode.insertBefore(js,fjs);
        }
    }
    (document,"script","twitter-wjs");
</script>
```

You don't need to understand the javascript.
For those of you who are curious, it's basically injecting 
a remote twitter api javascript file inline.
The javascript is acting on the attributes of the html link above it.
In the example above, the data-via attribute defines who to attribute the
tweet to.

e.g. generated tweet: <code><page_title> <page_url> via @robertpyke</code>

You would normally set the data-via to your company's twitter handle.

References
==========

* Tweet Button: https://dev.twitter.com/docs/tweet-button
* Tweet Button Generator: https://twitter.com/about/resources/buttons#tweet
