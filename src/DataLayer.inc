<?php

/**
 * @file
 * Generates a dataLayer JavaScript object for Google Tag Manager to pick up.
 */


class DataLayer {

  /**
   * The data to be added to the dataLayer.
   *
   * @var array
   */
  public static $output_data = array();

  /*
   * @param $user (optional)
   *   User object.
   */
  public function __construct($user = NULL) {
    // Use currently logged in user by default.
    if(is_null($user)) {
      $user = $GLOBALS['user'];
    }

    if(!is_null($user)) {
      // Add User ID by default.
      self::$output_data[]['userId'] = $user->uid;
    }
  }

  /**
   * Sets values for the dataLayer - as we're using a static property this just gets added to.
   * @param $output_data_new
   */
  public function set($output_data_new) {
    self::$output_data[] = $output_data_new;
  }

  /**
   * Build a JavaScript object compatible with the dataLayer schema.
   * @return bool|string
   */
  public function build() {

    // Merge together all items for the dataLayer.
    $output_flat = array();
    foreach (self::$output_data as $key => $item) {
      $output_flat = array_merge($item, $output_flat);
    }

    // Output the dataLayer data as JavaScript.
    return !empty($output_flat) ? drupal_json_encode($output_flat) : FALSE;
  }

  /**
   * Render the dataLayer in Drupal.
   * @param $output
   *   A JSON encoded object.
   */
  public function render($output) {
    $render = array(
      '#type' => 'html_tag',
      '#tag' => 'script',
      '#value' => 'dataLayer = [' . $output . '];',
      '#attributes' => array(
        'type' => 'text/javascript',
      ),
    );

    drupal_add_html_head($render, 'datalayer_meta');
  }

  /**
   * Generate transaction and product data for integration with the dataLayer.
   * Inspired by: https://www.drupal.org/project/commerce_google_analytics
   *
   * For Google Analytics & Tag Manager ECommerce API info see:
   *   - https://support.google.com/tagmanager/answer/6106097?rd=1
   *   - https://developers.google.com/analytics/devguides/collection/analyticsjs/ecommerce?hl=en
   *
   * @return array - PHP array compatible with the Google Analytics ecommerce specification
   * ready to be JSON encoded and added to the dataLayer.
   */
  public function getEcommerce($uid) {
    $past_orders = commerce_order_load_multiple(array(), array('uid' => $uid));

    if (!empty($past_orders)) {
      // Get the latest order.
      $latest_order = array_pop($past_orders);

      // Make sure the order is an order object.
      $latest_order = entity_metadata_wrapper('commerce_order', $latest_order);

      // Check this isn't just a shopping cart and the order has been paid for.
      if ($latest_order->status->value() === 'pending') {

        // Get product info from each line item
        $products = array();
        foreach ($latest_order->commerce_line_items as $line_item) {
          if (!empty($line_item)) {
            $products[] = array(
              'sku' => $line_item->commerce_product->sku->value(),
              'name' => $line_item->commerce_product->title->value(),
              'price' => commerce_currency_amount_to_decimal($line_item->commerce_unit_price->amount->value(), $line_item->commerce_unit_price->currency_code->value()),
              'quantity' => (int) $line_item->quantity->value(),
            );
          }
        }

        // Build the transaction array.
        $datalayer_ecommerce = array(
          'transactionId' => $latest_order->order_id->value(),
          'transactionTotal' => $transaction_total = commerce_currency_amount_to_decimal($latest_order->commerce_order_total->amount->value(), $latest_order->commerce_order_total->currency_code->value()),
          'transactionTax' => 0,
          'transactionShipping' => 0,
          'transactionProducts' => $products,
        );
        return $datalayer_ecommerce;
      }
    }

    return FALSE;
  }
}
