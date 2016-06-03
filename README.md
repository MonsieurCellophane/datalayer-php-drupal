PHP Data Layer
==============

Custom module to add user information to a [Data Layer](https://developers.google
.com/tag-manager/devguide?hl=en). Inspired by the [dataLayer module](https://www.drupal
.org/project/datalayer), but simpler and easy to use programmatically.

## Installation

1. Add new custom dimensions in Google Analytics for the following:
   * 1: User Role, scope: session
   * 2: User ID, scope: hit

2. Add a new dataLayer variable in GTM for the following:
   * userId
   * userRole

3. In Google Tag Manager, add a new fields in the 'Google Analytics' tag:
   * field name '&uid' value '{{userId}}'

4. In Google Tag Manager, add a new custom dimension in the 'Google Analytics tag':
   * index: 1, value: {{userRole}}
   * index: 2, value {{userId}}

5. Enable Ecommerce settings for the GA view (via the GA admin interface) & create a new GA tag in
GTM for 'transaction' hit types - this will pick up ECommerce data through the DataLayer.

## Notes

* E.g we can attribute a 'user role' to a hit, session or user (see https://support.google.com/analytics/answer/2709828?hl=en#example-hit)
  * Attributing to a hit will allow us to find out about how other content
    has been viewed, e.g how many 'Custom Role' users viewed a given page?
  * Attributing to a 'user' will allow us to find out the past behaviour of a given
    user type - e.g how did users who ended up being a "Custom Role" behave on
    our site when before they did something.

We have a custom dimension scoped to the hit level set up for the User ID (see this post for more
information on the advantages and motivation behind this: http://www.simoahava.com/analytics/improve-data-collection-with-four-custom-dimensions/#5).

## Future Development

### Enhanced Ecommerce

Using the [Enhanced Ecommerce](https://developers.google.com/tag-manager/enhanced-ecommerce)
feature of Google Analytics it's possible to track more advanced data like when users view a
product and when they add it to their cart. It can also be used to track abandonment through the
checkout process.

Enabling this will involve configuring more data in the dataLayer.

### More reliable advanced data

Currently we hardcode all data in the dataLayer JS object at the top of each page, this can cause
 issues if, for instance, a user purchases a product but their connection is interrupted before
 they get to the the page - or they reload the page.

 Using the dataLayer.push() method (or server-side events?) could help mitigate this, and will
 also allow for dynamically updating the dataLayer without the user leaving the page.
