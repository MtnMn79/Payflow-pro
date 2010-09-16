<?php

/**
 * Returns the PFP format needed for requests for a given internal period type.
 */
function _uc_to_pfp_format_period($period) {
  $map = _uc_pfp_period_map();
  return $map[$period];
}

/**
 * Returns a map from the internal period type to the PFP request type.
 */
function _uc_pfp_period_map() {
  static $uc_pfp_period_map;
  if($uc_pfp_period_map) {
    return $uc_pfp_period_map;
  }
  $uc_pfp_period_map = array(
    '1 weeks'  => 'Weekly',
    '2 weeks'  => 'Bi-weekly',
    //'SM' => 'Semi-monthly', // This is not currently possible to do in uc_recurring.
    '4 weeks' => 'Every four weeks',
    '1 months'  => 'Monthly',
    '3 months'  => 'Quarterly',
    '6 months' => 'Semi-yearly',
    '1 years'  => 'Yearly',
  );

  return $uc_pfp_period_map;
}

function _uc_payflowpro_get_auth() {
  $auth = array();
  $auth['vendor'] = variable_get('uc_payflowpro_vendor', '');
  $auth['partner'] = variable_get('uc_payflowpro_partner', '');
  $auth['user'] = variable_get('uc_payflowpro_user', '');
  $auth['password'] = variable_get('uc_payflowpro_password', '');
  return $auth;
}

/**
 * Returns a billing or shipping address from the order in the format
 * that is needed by the recurring class.
 */
function _uc_payflowpro_format_address($order, $type = 'billto') {
  //TODO: This function is somewhat redundant with the address XML generation
  // block used for normal transactions, condense them.

  if ($type == 'shipto') {
    $prefix = 'delivery';
  } else {
    $prefix = 'billing';
  }

  $sourceData = array(
    'FirstName' => 'billing_first_name',
    'LastName' => 'billing_last_name',
    'Phone' => 'billing_phone',
    'EMail' => 'primary_email',
    'CustCode' => 'uid',
    'Street' => 'billing_street1',
    'Street2' => 'billing_street2',
    'City' => 'billing_city',
    'State' => 'billing_zone',
    'Zip' => 'billing_postal_code',
    'Country' => 'billing_country',
  );

  $address = array();

  foreach ($sourceData as $newKey => $orderKey)
  {
    $orderKey = str_replace('billing',$prefix,$orderKey);
    $address[$newKey] = $order->$orderKey;
  }

  // Special field handling for state and country codes.
  $country = uc_get_country_data(array('country_id' => $address['Country']));
  if ($country === FALSE) {
    $country = array(0 => array('country_iso_code_2' => 'US'));
  }
  $address['State'] = uc_get_zone_code($address['State']);
  $address['Country'] = $country[0]['country_iso_code_2'];

  return $address;
}